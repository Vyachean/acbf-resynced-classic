// Copyright (c) 2026 Vyachean. All rights reserved.
// Read-only Puppet Native integration: logs one local diagnostic report.
// No game hooks, guessed RVAs, input suppression, game-memory access,
// third-party injected loader or gameplay calls.
#include "acbf/puppet_native_api.h"
#include "acbf/readonly_diagnostics.h"

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

    // No game hooks are installed. This starts the isolated read-only
    // reporter in a dedicated worker, not in DllMain.
    if (!acbf::diagnostics::start()) return false;
    log_message = api->Log;
    active = true;
    log_message("ACBFClassic: Puppet Native ABI v1 handshake succeeded; "
                "read-only diagnostics active; gameplay hooks disabled.");
    return true;
}

extern "C" __declspec(dllexport) void __cdecl PN_Shutdown() {
    if (!active) return;
    // Published PluginLoader requires shutdown to return within five seconds.
    // The worker owns a private DLL reference until it ends, preventing
    // use-after-unload even if this stop times out.
    const bool stopped = acbf::diagnostics::stop(4000);
    if (log_message)
        log_message(stopped
            ? "ACBFClassic: Puppet Native diagnostics stopped cleanly."
            : "ACBFClassic: diagnostic stop timed out; worker will exit independently.");
    active = false;
    log_message = nullptr;
}
