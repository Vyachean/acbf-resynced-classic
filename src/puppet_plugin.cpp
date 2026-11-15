// Copyright (c) 2026 Vyachean. All rights reserved.
// Read-only, inert Puppet Native plugin compatibility proof.
// No hooks, guessed game RVAs, input suppression, threads, memory reads,
// or calls into gameplay functions.
#include "acbf/puppet_native_api.h"

namespace {
bool active = false;
void (__cdecl* log_message)(const char*) = nullptr;
}

extern "C" __declspec(dllexport) bool __cdecl PN_Init(PN_Api* api) {
    if (active || !api || api->version != kPuppetNativeApiVersion ||
        !api->Log || !api->GetGameModule || !api->GetBase) {
        return false;
    }

    const HMODULE module = api->GetGameModule();
    const auto base = api->GetBase();
    // In upstream PluginLoader v1 the game base is the module handle.
    // This validates ABI consistency, NOT a game executable fingerprint.
    if (!module || !base || base != reinterpret_cast<std::uintptr_t>(module)) {
        return false;
    }

    log_message = api->Log;
    active = true;
    log_message("ACBFClassic: Puppet Native ABI v1 handshake succeeded; "
                "read-only inert plugin; gameplay hooks disabled.");
    return true;
}

extern "C" __declspec(dllexport) void __cdecl PN_Shutdown() {
    if (!active) return;
    if (log_message) {
        log_message("ACBFClassic: Puppet Native plugin shut down.");
    }
    active = false;
    log_message = nullptr;
}
