// Copyright (c) 2026 Vyachean. All rights reserved.
//
// Read-only startup/controller diagnostics for ACBFR and a synthetic CI host.
// No memory patching, code injection, Wwise interception, or input suppression.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "acbf/diagnostic_exports.h"
#include "acbf/classic_control.h"

#include <bcrypt.h>
#include <psapi.h>
#include <xinput.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cwctype>
#include <deque>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace {
using acbf::classic::Button;
using acbf::classic::Edge;
using acbf::classic::EdgeTracker;

constexpr unsigned kIntervalMs = 25;
constexpr unsigned kFlushIntervalMs = 2000;
constexpr size_t kMaxEvents = 256;
constexpr size_t kMaxModules = 48;

struct ButtonEvent {
    uint64_t elapsed_ms = 0;
    unsigned slot = 0;
    const char* button = "";
    const char* edge = "";
};
struct ProbeState {
    bool xinput_api = false;
    uint64_t samples = 0;
    uint64_t connections = 0;
    uint64_t disconnects = 0;
    uint64_t total_edges = 0;
    std::array<bool, 4> connected{};
    std::array<EdgeTracker, 4> trackers{};
    std::deque<ButtonEvent> recent;
    std::vector<std::string> local_asi;
    std::vector<std::string> local_proxies;
    std::string exe_class = "other";
    std::string sha256 = "";
    std::string hash_status = "NOT_RUN";
};

std::atomic<bool> stop_requested{false};
HANDLE worker_handle = nullptr;

std::wstring current_exe_path() {
    std::wstring result(32768, L'\0');
    DWORD length = GetModuleFileNameW(nullptr, result.data(),
                                     static_cast<DWORD>(result.size()));
    if (length == 0 || length >= result.size()) return {};
    result.resize(length);
    return result;
}

std::wstring directory_of(const std::wstring& path) {
    const auto i = path.find_last_of(L"\\/");
    return i == std::wstring::npos ? std::wstring{} : path.substr(0, i);
}

std::wstring basename_of(const std::wstring& path) {
    const auto i = path.find_last_of(L"\\/");
    return path.substr(i == std::wstring::npos ? 0 : i + 1);
}

std::wstring lowercase(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(std::towlower(c)); });
    return s;
}

bool is_same_directory(const std::wstring& path, const std::wstring& root) {
    return !root.empty() && lowercase(directory_of(path)) == lowercase(root);
}

// Restrict logged filenames to a non-sensitive ASCII subset; never write paths.
std::string sanitized_filename(const std::wstring& name) {
    std::string safe;
    safe.reserve(std::min<size_t>(name.size(), 100));
    for (wchar_t c : name) {
        if (safe.size() >= 100) break;
        if ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') ||
            (c >= L'0' && c <= L'9') || c == L'.' || c == L'_' || c == L'-') {
            safe.push_back(static_cast<char>(c));
        } else {
            safe.push_back('_');
        }
    }
    return safe;
}

std::string file_sha256(const std::wstring& path, std::string& status) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        status = "FILE_UNAVAILABLE";
        return {};
    }
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    std::array<unsigned char, 32> digest{};
    std::vector<unsigned char> data(256 * 1024);
    std::string result;
    bool ok = BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) >= 0;
    if (ok) ok = BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0) >= 0;
    while (ok) {
        DWORD count = 0;
        if (!ReadFile(file, data.data(), static_cast<DWORD>(data.size()), &count, nullptr)) {
            ok = false;
            break;
        }
        if (count == 0) break;
        if (BCryptHashData(hash, data.data(), count, 0) < 0) ok = false;
        if (stop_requested.load()) { ok = false; status = "INTERRUPTED"; }
    }
    if (ok && BCryptFinishHash(hash, digest.data(), static_cast<ULONG>(digest.size()), 0) >= 0) {
        std::ostringstream hex;
        hex << std::hex << std::setfill('0');
        for (unsigned char byte : digest) hex << std::setw(2) << static_cast<unsigned>(byte);
        result = hex.str();
        status = "OK";
    } else if (status != "INTERRUPTED") {
        status = "HASH_FAILED";
    }
    if (hash) BCryptDestroyHash(hash);
    if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
    CloseHandle(file);
    return result;
}

void collect_local_modules(ProbeState& state, const std::wstring& exe_path) {
    std::vector<std::string> asi;
    std::vector<std::string> proxies;
    std::array<HMODULE, 512> modules{};
    DWORD bytes_needed = 0;
    if (!EnumProcessModulesEx(GetCurrentProcess(), modules.data(),
                              static_cast<DWORD>(sizeof(modules)),
                              &bytes_needed, LIST_MODULES_ALL)) return;
    const auto count = std::min<size_t>(bytes_needed / sizeof(HMODULE), modules.size());
    const auto exe_dir = directory_of(exe_path);
    for (size_t i = 0; i < count; ++i) {
        std::array<wchar_t, 32768> buffer{};
        DWORD n = GetModuleFileNameW(modules[i], buffer.data(),
                                    static_cast<DWORD>(buffer.size()));
        if (n == 0 || n >= buffer.size()) continue;
        const std::wstring full(buffer.data(), n);
        if (!is_same_directory(full, exe_dir)) continue;
        const auto lower = lowercase(basename_of(full));
        const auto clean = sanitized_filename(basename_of(full));
        if (lower.size() >= 4 && lower.substr(lower.size() - 4) == L".asi") {
            if (asi.size() < kMaxModules) asi.push_back(clean);
        } else if (lower == L"version.dll" || lower == L"dinput8.dll" ||
                   lower == L"winmm.dll" || lower == L"dxgi.dll") {
            if (proxies.size() < kMaxModules) proxies.push_back(clean);
        }
    }
    std::sort(asi.begin(), asi.end());
    std::sort(proxies.begin(), proxies.end());
    state.local_asi = std::move(asi);
    state.local_proxies = std::move(proxies);
}

const char* button_name(Button button) {
    switch (button) {
    case Button::X: return "X";
    case Button::B: return "B";
    case Button::A: return "A";
    case Button::RT: return "RT";
    default: return "?";
    }
}

void record(ProbeState& state, uint64_t t, unsigned slot, Button button, Edge edge) {
    if (edge != Edge::Pressed && edge != Edge::Released) return;
    ++state.total_edges;
    if (state.recent.size() == kMaxEvents) state.recent.pop_front();
    state.recent.push_back({t, slot, button_name(button),
                            edge == Edge::Pressed ? "pressed" : "released"});
}

using XInputGetStateFn = DWORD (WINAPI*)(DWORD, XINPUT_STATE*);
struct XInputLibrary {
    HMODULE module = nullptr;
    XInputGetStateFn get_state = nullptr;
    bool owned = false;
};

XInputLibrary open_xinput() {
    for (const wchar_t* name : {L"xinput1_4.dll", L"xinput9_1_0.dll"}) {
        HMODULE module = GetModuleHandleW(name);
        bool owned = false;
        if (!module) {
            module = LoadLibraryExW(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
            owned = module != nullptr;
        }
        if (!module) continue;
        auto fn = reinterpret_cast<XInputGetStateFn>(GetProcAddress(module, "XInputGetState"));
        if (fn) return {module, fn, owned};
        if (owned) FreeLibrary(module);
    }
    return {};
}

void sample_xinput(ProbeState& state, XInputGetStateFn fn, uint64_t elapsed) {
    if (!fn) return;
    for (DWORD slot = 0; slot < 4; ++slot) {
        XINPUT_STATE input{};
        const DWORD code = fn(slot, &input);
        if (code != ERROR_SUCCESS) {
            if (state.connected[slot]) {
                state.connected[slot] = false;
                state.trackers[slot].reset();
                ++state.disconnects;
            }
            continue;
        }
        ++state.samples;
        if (!state.connected[slot]) {
            state.connected[slot] = true;
            ++state.connections;
            state.trackers[slot].reset();
        }
        const WORD bits = input.Gamepad.wButtons;
        const bool active[] = {
            (bits & XINPUT_GAMEPAD_X) != 0,
            (bits & XINPUT_GAMEPAD_B) != 0,
            (bits & XINPUT_GAMEPAD_A) != 0,
            input.Gamepad.bRightTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD
        };
        constexpr Button buttons[] = {Button::X, Button::B, Button::A, Button::RT};
        for (size_t j = 0; j < 4; ++j) {
            const Edge edge = state.trackers[slot].observe(buttons[j], active[j]);
            record(state, elapsed, slot, buttons[j], edge);
        }
    }
}

std::wstring output_directory() {
    // Test override is accepted ONLY in a non-game host process.
    std::wstring root;
    const std::wstring exe = lowercase(basename_of(current_exe_path()));
    if (exe != L"acblackflag.exe" && exe != L"acblackflag_plus.exe") {
        DWORD n = GetEnvironmentVariableW(L"ACBF_CLASSIC_TEST_OUTPUT", nullptr, 0);
        if (n > 1 && n < 32768) {
            root.resize(n);
            DWORD got = GetEnvironmentVariableW(L"ACBF_CLASSIC_TEST_OUTPUT",
                                                 root.data(), n);
            if (got > 0 && got < n) {
                root.resize(got);
                return root;
            }
        }
    }
    DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", nullptr, 0);
    if (n < 2 || n > 32768) return {};
    root.resize(n);
    DWORD got = GetEnvironmentVariableW(L"LOCALAPPDATA", root.data(), n);
    if (got == 0 || got >= n) return {};
    root.resize(got);
    return root + L"\\ACBFResyncedClassic";
}

bool write_report(const std::wstring& directory, const ProbeState& state, bool final) {
    if (directory.empty()) return false;
    CreateDirectoryW(directory.c_str(), nullptr);
    const std::wstring target = directory + L"\\diagnostics.json";
    const std::wstring temporary = directory + L"\\diagnostics.tmp";
    std::ostringstream json;
    json << "{\n"
         << "  \"schema_version\": 1,\n"
         << "  \"project\": \"acbf-resynced-classic\",\n"
         << "  \"mode\": \"read_only_diagnostic\",\n"
         << "  \"gameplay_modifications\": false,\n"
         << "  \"complete\": " << (final ? "true" : "false") << ",\n"
         << "  \"exe_class\": \"" << state.exe_class << "\",\n"
         << "  \"exe_sha256_status\": \"" << state.hash_status << "\",\n"
         << "  \"exe_sha256\": \"" << state.sha256 << "\",\n"
         << "  \"native_game_action_dispatch\": \"NOT_IMPLEMENTED\",\n"
         << "  \"game_semantics\": \"NOT_OBSERVABLE\",\n"
         << "  \"controller_observation\": \"XInput polling only (not the game's action stream)\",\n"
         << "  \"xinput_api\": " << (state.xinput_api ? "true" : "false") << ",\n"
         << "  \"xinput_samples\": " << state.samples << ",\n"
         << "  \"controller_connections\": " << state.connections << ",\n"
         << "  \"controller_disconnects\": " << state.disconnects << ",\n"
         << "  \"total_edges\": " << state.total_edges << ",\n";
    auto arr = [&](const char* key, const std::vector<std::string>& values) {
        json << "  \"" << key << "\": [";
        for (size_t i = 0; i < values.size(); ++i) {
            if (i) json << ", ";
            json << "\"" << values[i] << "\"";
        }
        json << "],\n";
    };
    arr("local_asi_modules", state.local_asi);
    arr("local_proxy_modules", state.local_proxies);
    json << "  \"recent_controller_edges\": [";
    bool first = true;
    for (const ButtonEvent& e : state.recent) {
        if (!first) json << ",";
        first = false;
        json << "\n    {\"elapsed_ms\":" << e.elapsed_ms
             << ",\"slot\":" << e.slot
             << ",\"button\":\"" << e.button
             << "\",\"edge\":\"" << e.edge << "\"}";
    }
    if (!state.recent.empty()) json << "\n  ";
    json << "]\n}\n";
    const std::string content = json.str();
    HANDLE file = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    const bool success = WriteFile(file, content.data(),
                                   static_cast<DWORD>(content.size()), &written, nullptr) != 0 &&
                         written == content.size() &&
                         FlushFileBuffers(file) != 0;
    CloseHandle(file);
    if (!success) return false;
    return MoveFileExW(temporary.c_str(), target.c_str(),
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
}

DWORD WINAPI diagnostic_worker(void*) {
    const auto exe_path = current_exe_path();
    const auto name = lowercase(basename_of(exe_path));
    const auto directory = output_directory();
    ProbeState state;
    if (name == L"acblackflag.exe") state.exe_class = "ACBlackFlag.exe";
    else if (name == L"acblackflag_plus.exe") state.exe_class = "ACBlackFlag_Plus.exe";

    // Keep the DLL alive during the worker lifetime, even if a loader
    // releases its original reference. Do not intercept game APIs.
    HMODULE own_ref = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                       reinterpret_cast<LPCWSTR>(&diagnostic_worker), &own_ref);

    state.sha256 = file_sha256(exe_path, state.hash_status);
    collect_local_modules(state, exe_path);
    XInputLibrary xinput = open_xinput();
    state.xinput_api = xinput.get_state != nullptr;
    const uint64_t started = GetTickCount64();
    uint64_t last_flush = started;
    uint64_t last_modules = started;
    write_report(directory, state, false);

    while (!stop_requested.load()) {
        const uint64_t now = GetTickCount64();
        sample_xinput(state, xinput.get_state, now - started);
        if (now - last_modules >= 5000) {
            collect_local_modules(state, exe_path);
            last_modules = now;
        }
        if (now - last_flush >= kFlushIntervalMs) {
            write_report(directory, state, false);
            last_flush = now;
        }
        Sleep(kIntervalMs);
    }
    write_report(directory, state, true);
    if (xinput.owned && xinput.module) FreeLibrary(xinput.module);

    // Drop the temporary ref on this thread when safe to exit.
    if (own_ref) FreeLibraryAndExitThread(own_ref, 0);
    return 0;
}

}  // namespace

extern "C" __declspec(dllexport)
DWORD WINAPI ACBFClassicDiagnostics_StopAndWait(DWORD timeout_ms) {
    stop_requested.store(true);
    if (!worker_handle) return WAIT_FAILED;
    return WaitForSingleObject(worker_handle, timeout_ms);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
        worker_handle = CreateThread(nullptr, 0, diagnostic_worker, nullptr, 0, nullptr);
        // No waits, hook installation, file IO or heavy work inside DllMain.
        return TRUE;
    }
    if (reason == DLL_PROCESS_DETACH) {
        stop_requested.store(true);
        if (worker_handle) {
            CloseHandle(worker_handle);
            worker_handle = nullptr;
        }
    }
    return TRUE;
}
