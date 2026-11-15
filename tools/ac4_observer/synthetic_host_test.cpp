#include <Windows.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

int wmain(int argc, wchar_t** argv) {
    if (argc != 3) return 2;
    const std::filesystem::path asi(argv[1]);
    const std::filesystem::path output(argv[2]);
    std::error_code error;
    std::filesystem::create_directories(output, error);
    if (error) return 3;
    SetEnvironmentVariableW(L"AC4_OBSERVER_TEST_OUTPUT", output.c_str());
    HMODULE module = LoadLibraryW(asi.c_str());
    if (!module) return 4;
    Sleep(250);
    wchar_t event_name[64]{};
    swprintf_s(event_name, L"Local\\AC4ControlObserver.Stop.%lu", GetCurrentProcessId());
    HANDLE stop = OpenEventW(EVENT_MODIFY_STATE, FALSE, event_name);
    if (!stop) { FreeLibrary(module); return 5; }
    SetEvent(stop);
    CloseHandle(stop);
    Sleep(250);
    FreeLibrary(module);
    bool found_start = false;
    bool found_safe_guard = false;
    bool found_end = false;
    for (const auto& entry : std::filesystem::directory_iterator(output, error)) {
        if (entry.path().extension() != L".jsonl") continue;
        std::ifstream stream(entry.path(), std::ios::binary);
        const std::string text((std::istreambuf_iterator<char>(stream)), {});
        found_start = text.find("session_start") != std::string::npos;
        found_safe_guard = text.find("native_hook_status") != std::string::npos &&
                           text.find("\"state\":0") != std::string::npos;
        found_end = text.find("\"complete\":true") != std::string::npos;
    }
    if (!found_start || !found_safe_guard || !found_end) {
        std::cerr << "missing startup, mismatch guard or clean stop record\n";
        return 6;
    }
    std::cout << "ASI loaded; foreign executable was safely rejected; journal closed\n";
    return 0;
}
