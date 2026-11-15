# Puppet Native PluginLoader integration: verified public ABI

**Date:** 2026-11-15. **Outcome:** a real, versioned, source-published
third-party integration seam has been identified and a **read-only Windows DLL plugin**
is built/tested in GitHub Actions. This does **not** establish attack/takedown
gameplay integration.

## Primary source and license

- Source: https://github.com/venvyl/puppet-native
- Exact public ABI: https://github.com/venvyl/puppet-native/blob/main/plugin%20loader/PuppetNativeApi.h
- Loader implementation: https://github.com/venvyl/puppet-native/blob/main/plugin%20loader/PluginLoader.cpp
- Upstream license: **MIT**, see [third-party notice](../THIRD_PARTY_NOTICES/PUPPET_NATIVE.md).
- Published V1.5 release: https://github.com/venvyl/puppet-native/releases/tag/V1.5

This finding supersedes previous claims that Puppet Native had no publicly
verifiable plugin ABI. **API v1 exists** in public source.

## Verified host contract (source-level)

- Plugin must export `bool __cdecl PN_Init(PN_Api*)` and
  `void __cdecl PN_Shutdown()`.
- `PN_Api.version` must equal **1**. The structure provides:
  logging, `GetGameModule`, `GetBase`, and INI read/write helpers.
- `GetGameModule` queries `ACBlackFlag.exe`; `GetBase` returns that
  module's base address.
- PluginLoader scans **`%APPDATA%/PuppetNative/plugins/*.dll`**.
- A `[Plugin: ACBFClassicPuppetPlugin]` stanza with `Enabled=1` in
  `plugin_config.ini` is required by its config parser; absent or disabled
  entries are not loaded. The launcher may manage this file.
- The loader calls `LoadLibraryW` and obtains the two exports, calls init,
  and unloads failed plugins. Shutdown is invoked before unloading.
- API v1 has **no documented player state, action eligibility, or
  attack/takedown dispatch function**. `GetBase` is only a low-level
  integration lead, not permission to invoke unverified RVAs.

## Project integration decision

**Prefer this Puppet Native plugin host for a controlled first integration
prototype if the existing loader is confirmed on the target game build.**
Reasons: public ABI + standard lifecycle + no need for a *second*
`version.dll`. This does NOT assert the installed Puppet Native v1.5
is functional in the user's environment; conflicting loader reports remain.

The `ACBFClassicPuppetPlugin.dll` plugin now **reuses the same read-only
diagnostic worker as the standalone ASI**. It verifies ABI version, module/base
callbacks, logs initialization/shutdown and records one local report containing
executable SHA-256, input-related DLL imports, local ASI/proxy DLL names and
XInput X/B/A/RT edges. **No gameplay hooks, memory patches, input suppression,
game memory reads, or action calls are made.** Its worker holds a module
reference until exit so Puppet Native cannot unload executing code.

CMake/CTest validates both host types on a synthetic Windows process, including
invalid ABI, missing callbacks, missing/mismatched module base, reinitialization,
graceful shutdown, and XInput edges from a test-only DLL. CI creates **two
separate, alternative diagnostic archives** (`acbf-asi-diagnostic-*.zip`
and `acbf-puppet-diagnostic-*.zip`). Do not install both.

For a future controlled Puppet Native session, the only project DLL required
is `ACBFClassicPuppetPlugin.dll` in
`%APPDATA%\\PuppetNative\\plugins\\`. The archive has a relative
`PuppetNative/plugins/` layout for orientation; this is **not** a folder to
copy inside the game directory. The existing Puppet Native
`plugin_config.ini` must enable `[Plugin: ACBFClassicPuppetPlugin]` with
`Enabled=1` (ideally via the host's UI). Never overwrite an existing config or
replace `version.dll`.

**No game launch is requested yet:** the module remains unable to observe
takedown eligibility, parry windows or native action dispatch. Its compatibility
with the installed Puppet Native build remains untested.

## Remaining high-impact unknowns

1. Can the author's public engine code identify **pre-action finisher
   eligibility**? No such API was found in the *public plugin contract*.
2. Can it issue **attack/takedown/assassination** without using conflicting
   in-game bindings? Not demonstrated.
3. Does the user's exact Resynced build accept the preinstalled loader and
   plugin? Not tested. Do not demand a game run for the inert handshake alone.
4. How does Puppet Native interact with ACBlackFlagFix's `version.dll`?
   Existing loader compatibility is unresolved; **never replace or stack
   competing proxy DLLs blindly**.
5. Which Resynced state/animgraph or action command can be inspected safely
   in one aggregated session? Not known. Further public code research first.

**Go/no-go:** Using a known plugin lifecycle is now technically feasible.
Contextual combat restoration is **still unproven**. Do not treat the mere
availability of `GetBase` as an action API or fabricate function offsets.
