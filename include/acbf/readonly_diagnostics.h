#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

// Starts/stops the same read-only reporter that the standalone ASI uses.
// Called only after Puppet Native ABI v1 has been validated.
namespace acbf::diagnostics {
bool start() noexcept;
bool stop(DWORD timeout_ms) noexcept;
}
