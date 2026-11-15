#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

namespace {
bool fail(const char* reason) {
    std::cerr << "diagnostic smoke failure: " << reason << '\n';
    return false;
}
}

int wmain(int argc, wchar_t** argv) {
    if (argc < 2) return fail("missing absolute ASI path") ? 0 : 1;

    wchar_t folder[MAX_PATH] = {};
    DWORD count = GetTempPathW(MAX_PATH, folder);
    if (count == 0 || count >= MAX_PATH) return 2;
    std::wstring dir = std::wstring(folder) +
                       L"acbf-classic-ci-" + std::to_wstring(GetCurrentProcessId());
    if (!CreateDirectoryW(dir.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
        return 3;
    if (!SetEnvironmentVariableW(L"ACBF_CLASSIC_TEST_OUTPUT", dir.c_str())) return 4;

    HMODULE module = LoadLibraryW(argv[1]);
    if (!module) return fail("ASI did not load") ? 0 : 5;

    using Stop = DWORD (WINAPI*)(DWORD);
    auto stop = reinterpret_cast<Stop>(GetProcAddress(module, "ACBFClassicDiagnostics_StopAndWait"));
    if (!stop) return fail("missing stop export") ? 0 : 6;

    // Allow worker to finish fingerprinting and at least two periodic flushes.
    Sleep(5000);
    if (stop(20000) != WAIT_OBJECT_0)
        return fail("diagnostic worker did not stop cleanly") ? 0 : 7;

    const auto report = dir + L"\\diagnostics.json";
    std::ifstream input(report, std::ios::binary);
    if (!input.is_open()) return fail("missing generated diagnostics.json") ? 0 : 8;

    const std::string json(std::istreambuf_iterator<char>{input},
                           std::istreambuf_iterator<char>{});
    auto require = [&](const std::string& pattern) {
        if (json.find(pattern) == std::string::npos) {
            std::cerr << "missing report field: " << pattern << '\n';
            return false;
        }
        return true;
    };
    bool ok = true;
    ok &= require("\"schema_version\": 1");
    ok &= require("\"mode\": \"read_only_diagnostic\"");
    ok &= require("\"gameplay_modifications\": false");
    ok &= require("\"complete\": true");
    ok &= require("\"exe_class\": \"other\"");
    ok &= require("\"exe_sha256_status\": \"OK\"");
    ok &= require("\"game_semantics\": \"NOT_OBSERVABLE\"");
    ok &= require("\"native_game_action_dispatch\": \"NOT_IMPLEMENTED\"");
    ok &= require("\"recent_controller_edges\": [");
    ok &= require("\"local_asi_modules\": [");
    FreeLibrary(module);

    // Keep the report for inspection inside the CI job only, not the playable artifact.
    std::wcout << L"CI synthetic host report: " << report << L'\n';
    return ok ? 0 : 9;
}
