#include <Windows.h>
#include <Xinput.h>
#include <MinHook.h>
#include <bcrypt.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <string>

namespace {
constexpr size_t kRingSize = 2048;
constexpr uint64_t kMaxJournalBytes = 128ull * 1024 * 1024;
constexpr DWORD kPollMs = 8;
constexpr DWORD kFlushMs = 1000;
constexpr uintptr_t kPreferredBase = 0x00400000;

struct Event {
    uint64_t qpc;
    uint64_t correlation;
    uint64_t physical_ref;
    uint64_t physical_id;
    uint32_t thread_id;
    uint32_t source;
    uint32_t type;
    uint32_t control;
    uint32_t state;
    uintptr_t object;
    uintptr_t recipient;
    int32_t command;
};

struct Cell {
    std::atomic<size_t> sequence{};
    Event event{};
};
std::array<Cell, kRingSize> ring;
std::atomic<size_t> enqueue_pos{0};
std::atomic<size_t> dequeue_pos{0};
std::atomic<uint64_t> lost_events{0};
std::atomic<uint64_t> physical_next{0};
std::atomic<uint64_t> latest_physical_qpc{0};
std::atomic<uint64_t> latest_physical_id{0};
std::atomic<bool> stop_requested{false};
std::atomic<bool> hook_active{false};
std::atomic<bool> source_disabled{false};
HANDLE stop_event = nullptr;
HANDLE worker = nullptr;
HMODULE self_module = nullptr;
FILE* journal = nullptr;
uint64_t journal_bytes = 0;
char session_id[64]{};
uint64_t qpc_frequency = 0;
LARGE_INTEGER qpc_origin{};
FILETIME utc_origin{};
std::atomic<uint64_t> correlation_next{0};
thread_local uint64_t active_correlation = 0;
thread_local uint16_t previous_buttons[4]{};
thread_local bool connected[4]{};

using CallbackFn = uint32_t(__thiscall*)(void*, uintptr_t);
using ResolverFn = void(__thiscall*)(void*, uintptr_t, int*, char);
// FUN_01AA3990 consumes four stack arguments and ends in RET 0x10.
using DispatchFn = int(__thiscall*)(void*, uint32_t, uint32_t, int*, uintptr_t);
using GateFn = uint32_t(__thiscall*)(void*, uint32_t);
using EdgeFn = uint32_t(__thiscall*)(void*, uint32_t, uintptr_t, uintptr_t);
using HoldFn = uint32_t(__thiscall*)(void*, uint32_t, uintptr_t, uintptr_t, uintptr_t);
// FUN_01147A20 is called with ECX=this, one stack argument, and ends in RET 4.
// Keep the thiscall stack-cleanup contract; treating it as cdecl corrupts ESP.
using AvailableFn = uint8_t(__thiscall*)(void*, uint32_t);
CallbackFn original_callback = nullptr;
ResolverFn original_resolver = nullptr;
DispatchFn original_dispatch = nullptr;
GateFn original_gate = nullptr;
EdgeFn original_edge = nullptr;
HoldFn original_hold = nullptr;
AvailableFn original_available = nullptr;

struct Site { uintptr_t rva; const uint8_t* bytes; size_t length; };
#ifdef AC4_LOCAL_PROFILE_HEADER
#include AC4_LOCAL_PROFILE_HEADER
#else
constexpr Site kSites[1]{};
constexpr char kExpectedExeSha256[] = "";
#endif

void push(uint32_t source, uint32_t type, uint32_t control, uint32_t state,
          uintptr_t object, uintptr_t recipient, int32_t command, uint64_t correlation,
          uint64_t physical_id = 0) noexcept {
    if (source_disabled.load(std::memory_order_relaxed)) return;
    size_t pos = enqueue_pos.load(std::memory_order_relaxed);
    Cell* cell = nullptr;
    for (;;) {
        cell = &ring[pos & (kRingSize - 1)];
        const size_t seq = cell->sequence.load(std::memory_order_acquire);
        const intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);
        if (diff == 0) {
            if (enqueue_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) break;
        } else if (diff < 0) {
            lost_events.fetch_add(1, std::memory_order_relaxed);
            return;
        } else {
            pos = enqueue_pos.load(std::memory_order_relaxed);
        }
    }
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);
    if (source == 1 && physical_id && state == 1 &&
        ((control >= 1 && control <= 14) || control == 16 || control == 17)) {
        latest_physical_qpc.store(static_cast<uint64_t>(now.QuadPart), std::memory_order_relaxed);
        latest_physical_id.store(physical_id, std::memory_order_relaxed);
    }
    uint64_t physical_ref = 0;
    if (source != 1 && physical_id == 0) {
        const uint64_t last_qpc = latest_physical_qpc.load(std::memory_order_relaxed);
        const uint64_t last_id = latest_physical_id.load(std::memory_order_relaxed);
        if (last_id && static_cast<uint64_t>(now.QuadPart) >= last_qpc &&
            static_cast<uint64_t>(now.QuadPart) - last_qpc <= qpc_frequency / 4)
            physical_ref = last_id;
    }
    cell->event = {static_cast<uint64_t>(now.QuadPart), correlation, physical_ref, physical_id, GetCurrentThreadId(),
                   source, type, control, state, object, recipient, command};
    cell->sequence.store(pos + 1, std::memory_order_release);
}

bool pop(Event& event) noexcept {
    size_t pos = dequeue_pos.load(std::memory_order_relaxed);
    Cell* cell = nullptr;
    for (;;) {
        cell = &ring[pos & (kRingSize - 1)];
        const size_t seq = cell->sequence.load(std::memory_order_acquire);
        const intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);
        if (diff == 0) {
            if (dequeue_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) break;
        } else if (diff < 0) {
            return false;
        } else {
            pos = dequeue_pos.load(std::memory_order_relaxed);
        }
    }
    event = cell->event;
    cell->sequence.store(pos + kRingSize, std::memory_order_release);
    return true;
}

const char* source_name(uint32_t value) noexcept {
    return value == 1 ? "xinput_poll" : value == 2 ? "native_callback" :
           value == 3 ? "native_resolver" : value == 4 ? "native_dispatch" :
           value == 5 ? "logical_edge_query" : value == 6 ? "logical_gate" :
           value == 7 ? "logical_hold_query" : value == 8 ? "logical_availability" : "status";
}
const char* type_name(uint32_t value) noexcept {
    return value == 1 ? "button_edge" : value == 2 ? "callback_enter" :
           value == 3 ? "callback_return" : value == 4 ? "candidate_dispatch" :
           value == 5 ? "rising_edge_result" : value == 6 ? "context_block_result" :
           value == 7 ? "hold_or_repeat_result" : value == 8 ? "availability_result" :
           value == 9 ? "controller_connection" : value == 10 ? "native_hook_status" :
           value == 11 ? "analog_value" : value == 12 ? "resolver_call" : "status";
}
const char* control_name(uint32_t value) noexcept {
    constexpr const char* names[] = {"none","dpad_up","dpad_down","dpad_left","dpad_right",
        "start","back","left_thumb","right_thumb","lb","rb","x","y","a","b","lt","rt"};
    constexpr const char* extra[] = {"left_trigger","right_trigger","left_x","left_y","right_x","right_y"};
    if (value < std::size(names)) return names[value];
    if (value >= 16 && value < 22) return extra[value - 16];
    return "unknown";
}
const char* correlation_method(const Event& e) noexcept {
    if (e.correlation && e.physical_ref) return "nested_internal_plus_temporal_physical";
    if (e.correlation) return "nested_internal";
    if (e.physical_ref) return "temporal_physical";
    return "none";
}

bool write_event(const Event& e) noexcept {
    if (!journal) return false;
    SYSTEMTIME utc{};
    ULARGE_INTEGER base{};
    base.LowPart = utc_origin.dwLowDateTime;
    base.HighPart = utc_origin.dwHighDateTime;
    const uint64_t delta = e.qpc >= static_cast<uint64_t>(qpc_origin.QuadPart)
        ? e.qpc - static_cast<uint64_t>(qpc_origin.QuadPart) : 0;
    const uint64_t ticks = base.QuadPart + (qpc_frequency ? delta * 10000000ull / qpc_frequency : 0);
    FILETIME time{static_cast<DWORD>(ticks), static_cast<DWORD>(ticks >> 32)};
    FileTimeToSystemTime(&time, &utc);
    const int n = std::fprintf(journal,
        "{\"record_type\":\"event\",\"seq\":%llu,\"qpc\":%llu,\"timestamp_utc\":\"%04u-%02u-%02uT%02u:%02u:%02u.%03uZ\",\"thread_id\":%u,\"source\":\"%s\",\"event\":\"%s\",\"control\":\"%s\",\"control_id\":%u,\"state\":%u,\"object_id\":\"0x%08llx\",\"recipient_id\":\"0x%08llx\",\"command_id\":%d,\"correlation_id\":%llu,\"physical_event_id\":%llu,\"physical_ref\":%llu,\"correlation_method\":\"%s\"}\n",
        static_cast<unsigned long long>(dequeue_pos.load() - 1), static_cast<unsigned long long>(e.qpc),
        utc.wYear, utc.wMonth, utc.wDay, utc.wHour, utc.wMinute, utc.wSecond, utc.wMilliseconds,
        e.thread_id, source_name(e.source), type_name(e.type),
        e.source >= 5 && e.source <= 8 ? "logical_slot" : control_name(e.control), e.control, e.state,
        static_cast<unsigned long long>(e.object), static_cast<unsigned long long>(e.recipient),
        e.command, static_cast<unsigned long long>(e.correlation), static_cast<unsigned long long>(e.physical_id),
        static_cast<unsigned long long>(e.physical_ref), correlation_method(e));
    if (n <= 0) return false;
    journal_bytes += static_cast<uint64_t>(n);
    if (journal_bytes >= kMaxJournalBytes) source_disabled.store(true);
    return true;
}

bool readable(const void* address, size_t size) noexcept {
    MEMORY_BASIC_INFORMATION info{};
    if (!address || VirtualQuery(address, &info, sizeof(info)) != sizeof(info) ||
        info.State != MEM_COMMIT || (info.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
    const uintptr_t start = reinterpret_cast<uintptr_t>(address);
    const uintptr_t end = start + size;
    const uintptr_t region_end = reinterpret_cast<uintptr_t>(info.BaseAddress) + info.RegionSize;
    return end >= start && end <= region_end;
}

bool verify_image(HMODULE image) noexcept {
#ifndef AC4_LOCAL_PROFILE_HEADER
    (void)image;
    return false;
#else
    auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
    if (!readable(dos, sizeof(*dos)) || dos->e_magic != IMAGE_DOS_SIGNATURE) return false;
    auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(reinterpret_cast<const uint8_t*>(image) + dos->e_lfanew);
    if (!readable(nt, sizeof(*nt)) || nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC ||
        nt->OptionalHeader.ImageBase != kPreferredBase) return false;
    if (std::size(kSites) != 7 || kExpectedExeSha256[0] == '\0') return false;
    std::wstring path(32768, L'\0');
    const DWORD path_len = GetModuleFileNameW(image, path.data(), static_cast<DWORD>(path.size()));
    if (!path_len || path_len >= path.size()) return false;
    path.resize(path_len);
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    std::array<uint8_t, 32> digest{};
    std::array<uint8_t, 256 * 1024> buffer{};
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    bool ok = BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) >= 0 &&
              BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0) >= 0;
    while (ok) {
        DWORD read = 0;
        if (!ReadFile(file, buffer.data(), static_cast<DWORD>(buffer.size()), &read, nullptr)) { ok = false; break; }
        if (read == 0) break;
        ok = BCryptHashData(hash, buffer.data(), read, 0) >= 0;
    }
    if (ok) ok = BCryptFinishHash(hash, digest.data(), static_cast<ULONG>(digest.size()), 0) >= 0;
    if (hash) BCryptDestroyHash(hash);
    if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
    CloseHandle(file);
    constexpr char hex[] = "0123456789abcdef";
    char actual_hash[65]{};
    for (size_t i = 0; i < digest.size(); ++i) {
        actual_hash[i * 2] = hex[digest[i] >> 4];
        actual_hash[i * 2 + 1] = hex[digest[i] & 15];
    }
    if (!ok || _stricmp(actual_hash, kExpectedExeSha256) != 0) return false;
    for (const auto& site : kSites) {
        if (site.rva + site.length > nt->OptionalHeader.SizeOfImage) return false;
        const auto* target = reinterpret_cast<const uint8_t*>(image) + site.rva;
        if (!readable(target, site.length) || std::memcmp(target, site.bytes, site.length) != 0) return false;
    }
    return true;
#endif
}

uint32_t __fastcall callback_hook(void* self, void*, uintptr_t arg) {
    const uint64_t previous = active_correlation;
    active_correlation = correlation_next.fetch_add(1, std::memory_order_relaxed) + 1;
    const uint64_t corr = active_correlation;
    push(2, 2, 0, 0, reinterpret_cast<uintptr_t>(self), 0, 0, corr);
    const uint32_t result = original_callback(self, arg);
    push(2, 3, 0, result, reinterpret_cast<uintptr_t>(self), 0, static_cast<int32_t>(result), corr);
    active_correlation = previous;
    return result;
}

void __fastcall resolver_hook(void* self, void*, uintptr_t flags, int* output, char mode) {
    const uint64_t corr = active_correlation;
    original_resolver(self, flags, output, mode);
    // The output pointer belongs to the game. Do not dereference or retain it.
    push(3, 12, 0, static_cast<uint32_t>(mode), reinterpret_cast<uintptr_t>(self),
         reinterpret_cast<uintptr_t>(output), static_cast<int32_t>(flags), corr);
}

int __fastcall dispatch_hook(void* self, void*, uint32_t flag_a, uint32_t flag_b,
                             int* handler, uintptr_t opaque_arg4) {
    const int result = original_dispatch(self, flag_a, flag_b, handler, opaque_arg4);
    push(4, 4, 0, (flag_a & 1u) | ((flag_b & 1u) << 1), reinterpret_cast<uintptr_t>(self),
         reinterpret_cast<uintptr_t>(handler), result, active_correlation);
    return result;
}

uint32_t __fastcall gate_hook(void* self, void*, uint32_t slot) {
    const uint32_t result = original_gate(self, slot);
    if (active_correlation)
        push(6, 6, slot, result, reinterpret_cast<uintptr_t>(self), 0,
             static_cast<int32_t>(slot), active_correlation);
    return result;
}

uint32_t __fastcall edge_hook(void* self, void*, uint32_t slot, uintptr_t arg3, uintptr_t arg4) {
    const uint32_t result = original_edge(self, slot, arg3, arg4);
    if (active_correlation)
        push(5, 5, slot, result, reinterpret_cast<uintptr_t>(self), 0,
             static_cast<int32_t>(slot), active_correlation);
    return result;
}

uint32_t __fastcall hold_hook(void* self, void*, uint32_t slot, uintptr_t arg3,
                              uintptr_t arg4, uintptr_t arg5) {
    const uint32_t result = original_hold(self, slot, arg3, arg4, arg5);
    if (active_correlation)
        push(7, 7, slot, result, reinterpret_cast<uintptr_t>(self), 0,
             static_cast<int32_t>(slot), active_correlation);
    return result;
}

uint8_t __fastcall available_hook(void* self, void*, uint32_t slot) {
    const uint8_t result = original_available(self, slot);
    if (active_correlation)
        push(8, 8, slot, result, reinterpret_cast<uintptr_t>(self), 0,
             static_cast<int32_t>(slot), active_correlation);
    return result;
}

void stop_hooks() noexcept {
    if (!hook_active.exchange(false)) return;
    MH_DisableHook(MH_ALL_HOOKS);
    const auto image = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
    for (const auto& site : kSites)
        MH_RemoveHook(reinterpret_cast<void*>(image + site.rva));
    MH_Uninitialize();
}

bool install_hooks(HMODULE image) noexcept {
    if (!verify_image(image) || MH_Initialize() != MH_OK) return false;
    void* targets[std::size(kSites)]{};
    for (size_t i = 0; i < std::size(kSites); ++i)
        targets[i] = reinterpret_cast<uint8_t*>(image) + kSites[i].rva;
    void* detours[] = {reinterpret_cast<void*>(&callback_hook), reinterpret_cast<void*>(&resolver_hook),
        reinterpret_cast<void*>(&dispatch_hook), reinterpret_cast<void*>(&gate_hook),
        reinterpret_cast<void*>(&edge_hook), reinterpret_cast<void*>(&hold_hook),
        reinterpret_cast<void*>(&available_hook)};
    void** originals[] = {reinterpret_cast<void**>(&original_callback), reinterpret_cast<void**>(&original_resolver),
        reinterpret_cast<void**>(&original_dispatch), reinterpret_cast<void**>(&original_gate),
        reinterpret_cast<void**>(&original_edge), reinterpret_cast<void**>(&original_hold),
        reinterpret_cast<void**>(&original_available)};
    size_t created = 0;
    while (created < std::size(kSites) && MH_CreateHook(targets[created], detours[created], originals[created]) == MH_OK)
        ++created;
    if (created != std::size(kSites)) {
        while (created) MH_RemoveHook(targets[--created]);
        MH_Uninitialize();
        return false;
    }
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        MH_DisableHook(MH_ALL_HOOKS);
        for (void* target : targets) MH_RemoveHook(target);
        MH_Uninitialize();
        return false;
    }
    hook_active.store(true);
    return true;
}

bool open_journal() noexcept {
    wchar_t root[MAX_PATH]{};
    wchar_t exe[MAX_PATH]{};
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    const wchar_t* base = wcsrchr(exe, L'\\');
    base = base ? base + 1 : exe;
    if (_wcsicmp(base, L"AC4BFSP.exe") != 0) {
        const DWORD test_len = GetEnvironmentVariableW(L"AC4_OBSERVER_TEST_OUTPUT", root, MAX_PATH);
        if (test_len > 0 && test_len < MAX_PATH) {
            std::wstring test_dir(root);
            if (!CreateDirectoryW(test_dir.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) return false;
            SYSTEMTIME test_now{}; GetSystemTime(&test_now);
            sprintf_s(session_id, "%04u%02u%02u-%02u%02u%02u-%lu", test_now.wYear, test_now.wMonth,
                test_now.wDay, test_now.wHour, test_now.wMinute, test_now.wSecond, GetCurrentProcessId());
            wchar_t file_name[96]{};
            swprintf_s(file_name, L"session-%04u%02u%02u-%02u%02u%02u-%lu.jsonl",
                test_now.wYear, test_now.wMonth, test_now.wDay, test_now.wHour,
                test_now.wMinute, test_now.wSecond, GetCurrentProcessId());
            const std::wstring test_path = test_dir + L"\\" + file_name;
            if (_wfopen_s(&journal, test_path.c_str(), L"wb") != 0 || !journal) return false;
            LARGE_INTEGER test_freq{}; QueryPerformanceFrequency(&test_freq);
            qpc_frequency = static_cast<uint64_t>(test_freq.QuadPart);
            QueryPerformanceCounter(&qpc_origin); GetSystemTimeAsFileTime(&utc_origin);
            const int test_written = std::fprintf(journal,
                "{\"record_type\":\"session_start\",\"schema_version\":1,\"session_id\":\"%s\",\"timestamp_utc\":\"%04u-%02u-%02uT%02u:%02u:%02uZ\",\"qpc_frequency\":%llu,\"game_build\":\"synthetic_host\",\"native_sources\":\"signature_gated\",\"physical_source\":\"xinput_polling_only\"}\n",
                session_id, test_now.wYear, test_now.wMonth, test_now.wDay, test_now.wHour,
                test_now.wMinute, test_now.wSecond, static_cast<unsigned long long>(qpc_frequency));
            journal_bytes = test_written > 0 ? static_cast<uint64_t>(test_written) : 0;
            return test_written > 0;
        }
    }
    if (!GetEnvironmentVariableW(L"LOCALAPPDATA", root, MAX_PATH)) return false;
    std::wstring dir = std::wstring(root) + L"\\AC4ControlObserver";
    if (!CreateDirectoryW(dir.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) return false;
    SYSTEMTIME now{}; GetSystemTime(&now);
    sprintf_s(session_id, "%04u%02u%02u-%02u%02u%02u-%lu", now.wYear, now.wMonth,
        now.wDay, now.wHour, now.wMinute, now.wSecond, GetCurrentProcessId());
    wchar_t file_name[96]{};
    swprintf_s(file_name, L"session-%04u%02u%02u-%02u%02u%02u-%lu.jsonl",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, GetCurrentProcessId());
    const std::wstring path = dir + L"\\" + file_name;
    if (_wfopen_s(&journal, path.c_str(), L"wb") != 0 || !journal) return false;
    LARGE_INTEGER freq{}; QueryPerformanceFrequency(&freq); qpc_frequency = static_cast<uint64_t>(freq.QuadPart);
    QueryPerformanceCounter(&qpc_origin); GetSystemTimeAsFileTime(&utc_origin);
    const int n = std::fprintf(journal,
        "{\"record_type\":\"session_start\",\"schema_version\":1,\"session_id\":\"%04u%02u%02u-%02u%02u%02u-%lu\",\"timestamp_utc\":\"%04u-%02u-%02uT%02u:%02u:%02uZ\",\"qpc_frequency\":%llu,\"game_build\":\"guarded_local_x86_candidate\",\"native_sources\":\"signature_gated\",\"physical_source\":\"xinput_polling_only\"}\n",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, GetCurrentProcessId(),
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond,
        static_cast<unsigned long long>(qpc_frequency));
    journal_bytes = n > 0 ? static_cast<uint64_t>(n) : 0;
    return n > 0;
}

void sample_xinput() noexcept {
    static constexpr WORD masks[] = {XINPUT_GAMEPAD_DPAD_UP, XINPUT_GAMEPAD_DPAD_DOWN,
        XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT, XINPUT_GAMEPAD_START,
        XINPUT_GAMEPAD_BACK, XINPUT_GAMEPAD_LEFT_THUMB, XINPUT_GAMEPAD_RIGHT_THUMB,
        XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER, XINPUT_GAMEPAD_X,
        XINPUT_GAMEPAD_Y, XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B};
    for (DWORD slot = 0; slot != 4; ++slot) {
        XINPUT_STATE state{};
        const DWORD status = XInputGetState(slot, &state);
        if (status != ERROR_SUCCESS) {
            if (connected[slot]) push(1, 9, 0, 0, slot, 0, 0, 0);
            connected[slot] = false; previous_buttons[slot] = 0; continue;
        }
        if (!connected[slot]) push(1, 9, 0, 1, slot, 0, 0, 0);
        connected[slot] = true;
        const WORD current = state.Gamepad.wButtons;
        for (uint32_t i = 0; i < std::size(masks); ++i) {
            const bool was = (previous_buttons[slot] & masks[i]) != 0;
            const bool now = (current & masks[i]) != 0;
            if (was != now) {
                const uint64_t id = physical_next.fetch_add(1, std::memory_order_relaxed) + 1;
                push(1, 1, i + 1, now ? 1u : 0u, slot, 0, 0, 0, id);
            }
        }
        previous_buttons[slot] = current;
        static BYTE previous_triggers[4][2]{};
        const BYTE triggers[] = {state.Gamepad.bLeftTrigger, state.Gamepad.bRightTrigger};
        for (uint32_t i = 0; i != 2; ++i) {
            const uint32_t control = 16 + i;
            const bool was_active = previous_triggers[slot][i] >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
            const bool is_active = triggers[i] >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
            if (was_active != is_active) {
                const uint64_t id = physical_next.fetch_add(1, std::memory_order_relaxed) + 1;
                push(1, 1, control, is_active ? 1u : 0u, slot, 0, 0, 0, id);
            }
            if (triggers[i] != previous_triggers[slot][i]) {
                const uint64_t id = physical_next.fetch_add(1, std::memory_order_relaxed) + 1;
                push(1, 11, control, triggers[i], slot, 0, 0, 0, id);
            }
            previous_triggers[slot][i] = triggers[i];
        }
        // Stick values are sampled only on meaningful changes and kept as raw signed values.
        static XINPUT_GAMEPAD previous[4]{};
        if (state.Gamepad.sThumbLX != previous[slot].sThumbLX) { const auto id = physical_next.fetch_add(1) + 1; push(1,11,18,static_cast<uint16_t>(state.Gamepad.sThumbLX),slot,0,0,0,id); }
        if (state.Gamepad.sThumbLY != previous[slot].sThumbLY) { const auto id = physical_next.fetch_add(1) + 1; push(1,11,19,static_cast<uint16_t>(state.Gamepad.sThumbLY),slot,0,0,0,id); }
        if (state.Gamepad.sThumbRX != previous[slot].sThumbRX) { const auto id = physical_next.fetch_add(1) + 1; push(1,11,20,static_cast<uint16_t>(state.Gamepad.sThumbRX),slot,0,0,0,id); }
        if (state.Gamepad.sThumbRY != previous[slot].sThumbRY) { const auto id = physical_next.fetch_add(1) + 1; push(1,11,21,static_cast<uint16_t>(state.Gamepad.sThumbRY),slot,0,0,0,id); }
        previous[slot] = state.Gamepad;
    }
}

DWORD WINAPI run(void*) {
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        reinterpret_cast<LPCWSTR>(&run), &self_module)) return 0;
    if (!open_journal()) {
        if (worker) { CloseHandle(worker); worker = nullptr; }
        if (self_module) FreeLibraryAndExitThread(self_module, 0);
        return 0;
    }
    wchar_t event_name[64]{};
    swprintf_s(event_name, L"Local\\AC4ControlObserver.Stop.%lu", GetCurrentProcessId());
    stop_event = CreateEventW(nullptr, TRUE, FALSE, event_name);
    if (!stop_event) stop_requested.store(true);
    if (stop_event && WaitForSingleObject(stop_event, 2000) == WAIT_OBJECT_0)
        stop_requested.store(true);
    HMODULE image = GetModuleHandleW(nullptr);
    const bool hooks = !stop_requested.load() && image && install_hooks(image);
    push(0, 10, 0, hooks ? 1u : 0u, reinterpret_cast<uintptr_t>(image), 0, hooks ? 0 : 1, 0);
    const ULONGLONG last_flush_origin = GetTickCount64();
    ULONGLONG last_flush = last_flush_origin;
    while (!stop_requested.load()) {
        sample_xinput();
        if (stop_event && WaitForSingleObject(stop_event, kPollMs) == WAIT_OBJECT_0) break;
        const ULONGLONG now = GetTickCount64();
        Event event{};
        while (pop(event)) {
            if (!write_event(event)) { stop_requested.store(true); break; }
        }
        if (now - last_flush >= kFlushMs && journal) {
            std::fflush(journal);
            last_flush = now;
        }
        if (now - last_flush_origin >= 24ull * 60 * 60 * 1000) stop_requested.store(true);
    }
    stop_hooks();
    Event event{}; while (pop(event)) write_event(event);
    if (journal) {
        SYSTEMTIME now{}; GetSystemTime(&now);
        std::fprintf(journal,
            "{\"record_type\":\"session_end\",\"session_id\":\"%s\",\"timestamp_utc\":\"%04u-%02u-%02uT%02u:%02u:%02uZ\",\"complete\":true,\"dropped_events\":%llu,\"source_disabled_by_size\":%s}\n",
            session_id, now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond,
            static_cast<unsigned long long>(lost_events.load()), source_disabled.load() ? "true" : "false");
        std::fflush(journal); std::fclose(journal); journal = nullptr;
    }
    if (stop_event) CloseHandle(stop_event);
    if (worker) { CloseHandle(worker); worker = nullptr; }
    if (self_module) FreeLibraryAndExitThread(self_module, 0);
    return 0;
}
} // namespace

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
        for (size_t i = 0; i < kRingSize; ++i) ring[i].sequence.store(i);
        worker = CreateThread(nullptr, 0, run, nullptr, 0, nullptr);
    }
    return TRUE;
}
