#pragma once
#ifdef _WIN32

#include <windows.h>

// Diagnostic-only ABI for the synthetic CI host.
// Runtime calls remain read-only; no game hooks or control overrides.
extern "C" __declspec(dllexport)
DWORD WINAPI ACBFClassicDiagnostics_StopAndWait(DWORD timeout_ms);

#endif
