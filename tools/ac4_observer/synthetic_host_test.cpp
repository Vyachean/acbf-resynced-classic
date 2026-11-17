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
    HANDLE journal_reader = INVALID_HANDLE_VALUE;
    const std::wstring pid_suffix = L"-" + std::to_wstring(GetCurrentProcessId()) + L".jsonl";
    for (unsigned attempt = 0; attempt < 40 && journal_reader == INVALID_HANDLE_VALUE; ++attempt) {
        for (const auto& entry : std::filesystem::directory_iterator(output, error)) {
            if (entry.path().extension() != L".jsonl" ||
                entry.path().filename().wstring().size() < pid_suffix.size() ||
                entry.path().filename().wstring().compare(
                    entry.path().filename().wstring().size() - pid_suffix.size(),
                    pid_suffix.size(), pid_suffix) != 0) continue;
            journal_reader = CreateFileW(entry.path().c_str(), GENERIC_READ,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (journal_reader == INVALID_HANDLE_VALUE && GetLastError() != ERROR_SHARING_VIOLATION)
                break;
        }
        if (journal_reader == INVALID_HANDLE_VALUE) Sleep(50);
    }
    if (journal_reader == INVALID_HANDLE_VALUE) {
        std::wcerr << L"journal could not be read while the observer writer was active\n";
        wchar_t failed_event_name[64]{};
        swprintf_s(failed_event_name, L"Local\\AC4ControlObserver.Stop.%lu", GetCurrentProcessId());
        HANDLE failed_stop = OpenEventW(EVENT_MODIFY_STATE, FALSE, failed_event_name);
        if (failed_stop) { SetEvent(failed_stop); CloseHandle(failed_stop); }
        Sleep(250);
        FreeLibrary(module);
        return 5;
    }
    CloseHandle(journal_reader);
    wchar_t event_name[64]{};
    swprintf_s(event_name, L"Local\\AC4ControlObserver.Stop.%lu", GetCurrentProcessId());
    HANDLE stop = OpenEventW(EVENT_MODIFY_STATE, FALSE, event_name);
    if (!stop) { FreeLibrary(module); return 6; }
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
        return 7;
    }
    std::cout << "ASI loaded; foreign executable was safely rejected; journal closed\n";
    return 0;
}

