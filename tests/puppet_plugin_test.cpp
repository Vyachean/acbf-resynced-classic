// Synthetic Puppet Native host exercising its public PluginLoader ABI.
// No game executable, injected loader or virtual controller is involved.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "acbf/puppet_native_api.h"
#include <windows.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>

namespace {
int failures = 0;
int log_calls = 0;
std::uintptr_t supplied_base = 0x0000000140000000ULL;
bool provide_module = true;

void expect(bool condition, const char* label) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL " << label << "\n";
    }
}
void __cdecl log_cb(const char* msg) {
    if (msg && std::strstr(msg, "ACBFClassic:")) ++log_calls;
}
HMODULE __cdecl game_cb() {
    return provide_module ? reinterpret_cast<HMODULE>(0x0000000140000000ULL) : nullptr;
}
std::uintptr_t __cdecl base_cb() { return supplied_base; }
std::int32_t __cdecl read_int_cb(const char*,const char*,std::int32_t d) { return d; }
float __cdecl read_float_cb(const char*,const char*,float d) { return d; }
void __cdecl write_int_cb(const char*,const char*,std::int32_t) {}
PN_Api valid_api() {
    return {kPuppetNativeApiVersion, log_cb, game_cb, base_cb,
            read_int_cb, read_float_cb, write_int_cb};
}

}

int wmain(int argc, wchar_t** argv) {
    if (argc != 3) return 1;
    wchar_t base[MAX_PATH] = {};
    DWORD n = GetTempPathW(MAX_PATH, base);
    if (!n || n >= MAX_PATH) return 5;
    const std::wstring dir = std::wstring(base) +
        L"acbf-puppet-ci-" + std::to_wstring(GetCurrentProcessId());
    if (!CreateDirectoryW(dir.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
        return 6;
    if (!SetEnvironmentVariableW(L"ACBF_CLASSIC_TEST_OUTPUT", dir.c_str())) return 7;
    HMODULE fake_xinput = LoadLibraryW(argv[2]);
    if (!fake_xinput) return 8;
    HMODULE plugin = LoadLibraryW(argv[1]);
    if (!plugin) {
        std::cerr << "Failed LoadLibraryW\n";
        return 2;
    }
    auto init = reinterpret_cast<PN_InitFn>(GetProcAddress(plugin, "PN_Init"));
    auto shutdown = reinterpret_cast<PN_ShutdownFn>(GetProcAddress(plugin, "PN_Shutdown"));
    expect(init && shutdown, "required exports exist");
    if (!init || !shutdown) return 3;

    auto api = valid_api();
    expect(!init(nullptr), "reject null API");
    api.version = 2;
    expect(!init(&api), "reject unknown ABI version");
    api = valid_api();
    api.Log = nullptr;
    expect(!init(&api), "reject absent logger");
    api = valid_api();
    api.GetGameModule = nullptr;
    expect(!init(&api), "reject absent game resolver");
    api = valid_api();
    api.GetBase = nullptr;
    expect(!init(&api), "reject absent base resolver");
    api = valid_api();
    provide_module = false;
    expect(!init(&api), "reject unresolved game module");
    provide_module = true;
    supplied_base += 4096;
    expect(!init(&api), "reject inconsistent module base");
    supplied_base -= 4096;

    expect(log_calls == 0, "no logs before a valid handshake");
    expect(init(&api), "accept ABI v1 game module/base");
    expect(log_calls == 1, "log successful init once");
    expect(!init(&api), "reject double initialization");
    // The fake XInput provider emits 16 down/up transitions in its first
    // 64 calls; no real game/controller is required. Give the worker enough
    // time to finish its initial executable scan and collect those calls.
    const auto path = dir + L"\\diagnostics.json";
    Sleep(5000);
    shutdown();
    expect(log_calls == 2, "log shutdown");
    std::ifstream report(path, std::ios::binary);
    expect(report.is_open(), "PN plugin wrote diagnostics.json");
    if (report.is_open()) {
        const std::string json(std::istreambuf_iterator<char>{report},
                               std::istreambuf_iterator<char>{});
        for (const auto& fragment : {
                "\"mode\": \"read_only_diagnostic\"",
                "\"complete\": true",
                "\"gameplay_modifications\": false",
                "\"exe_sha256_status\": \"OK\"",
                "\"total_edges\": 16",
                "\"xinput_api\": true",
                "\"game_semantics\": \"NOT_OBSERVABLE\"",
                "\"native_game_action_dispatch\": \"NOT_IMPLEMENTED\""}) {
            expect(json.find(fragment) != std::string::npos, fragment);
        }
    }
    shutdown();
    expect(log_calls == 2, "shutdown idempotent");
    expect(init(&api), "reinitialize after clean shutdown");
    shutdown();
    expect(log_calls == 4, "reload cycle logs");

    FreeLibrary(plugin);
    FreeLibrary(fake_xinput);
    std::cout << "Puppet Native ABI contract failures: " << failures << "\n";
    return failures == 0 ? 0 : 4;
}
