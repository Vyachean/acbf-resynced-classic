# Read-only diagnostic ASI (experimental)

**Status: built and smoke-tested under GitHub Actions, NOT tested inside Resynced. Do not install yet unless a subsequent issue explicitly requests a single combined test.**

This is **not** the classic controls mod. It does not change, block, synthesize or remap game inputs. It does not read game memory, modify game code, or intercept Wwise/engine events.

## Purpose

Provide a minimal, original runtime observer that can later be **combined with verified state/action instrumentation** into one comprehensive in-game diagnostic session. Creating this module now allows Windows CI to catch loader/ABI/report problems before involving a player.

### What it records automatically

- Process executable class (`ACBlackFlag.exe`, `ACBlackFlag_Plus.exe`, or `other`), **SHA-256 of the current executable** and hash status (no file is uploaded).
- Names (never paths) of loaded `.asi` files and selected local proxy DLLs: `version.dll`, `dinput8.dll`, `winmm.dll`, `dxgi.dll`.
- **Static import table** of the current executable (read-only PE import directory): input-related DLL names such as `GameInput.dll`, `XInput*.dll` or `dinput8.dll`, plus a parse status. A packed/virtualized PE may produce no usable import information.
- **Loaded input DLLs before the observer opens its own XInput library**; this distinguishes preexisting modules from the observer's own probes. Module presence can also be due to other mods, overlays or game launchers.
- Whether a system XInput API is accessible; connected/disconnected controller counts.
- **X, Y, B, A, LB, RB, LT and RT** pressed/released edges from **XInput polling**, 4 controller slots, maximum 512 recent events, total count.
- Explicit `NOT_OBSERVABLE` markers for in-game semantics and `NOT_IMPLEMENTED` for action dispatch.
- Updates the report every ~2 seconds while running. Maximum logged detailed events is bounded; no network telemetry.

**Limitations:** PE imports and loaded DLLs are **hints, not proof** of the game's chosen input API. Dynamic loading, code protection and other mods may hide or alter the evidence. A missing input DLL does not prove the game cannot use that API. XInput polling is **not** proof the game receives or interprets the same button events. The mod cannot currently distinguish a finisher from an ordinary attack, examine enemy eligibility, record real parry windows or map any game action. GameInput/DirectInput-only controllers may not appear. Module enumeration can miss modules loaded later or from non-game directories. Hashing a large executable may temporarily perform substantial read-only disk I/O. The diagnostic report does not declare any gameplay test `PASS`.

## Report

Produced locally at:

`%LOCALAPPDATA%\ACBFResyncedClassic\diagnostics.json`

This is a **single JSON file** with no usernames, full paths, arbitrary keystrokes, saves or memory dumps. It will be overwritten by the next run. If the report is absent, diagnostic startup could have failed or the destination could be unwritable.

## Runtime and distribution

- Built on **Windows x64** in GitHub Actions as alternative `ACBFClassicDiagnostics.asi` and `ACBFClassicPuppetPlugin.dll` builds. Do not install both.
- The ASI variant requires a compatible existing ASI loader; the Puppet variant requires a working Puppet Native v1 PluginLoader. **Neither includes or overwrites `version.dll`**.
- An ASI loader is not guaranteed to coexist with Puppet Native or other mods. Use a controlled session with a known compatible loader only after compatibility analysis.
- For eventual tests, the operator should only copy the supplied ASI to the game folder, launch once, play the prepared sequence when requested, then send the generated JSON. Remove only this project's ASI after closing the game.
- **No game launch is requested for this revision.** There is not yet enough semantic coverage for the intended single comprehensive diagnostic session.

## Test boundary

GitHub Actions compiles both the real x64 ASI and the Puppet Native DLL; synthetic Windows host programs load them with `LoadLibraryW` alongside a test-only fake `xinput1_4.dll`. The fake device generates eight button down/up cycles (X, Y, B, A, LB, RB, LT and RT). CTest verifies the report includes **exactly 16 transitions** (no duplicate presses during held polls), controller-connection counters, and explicit semantic-availability limitations. The fake XInput DLL is **never included** in installation archives, nor placed in the game folder. The separate PE-import parser is exercised against independently generated valid/malformed synthetic PE fixtures in CTest. The host never executes game code. CTest also exercises the pure control resolver. This proves neither that a particular ASI loader loads the module nor that semantic game hooks work.

## Safety

- Runtime observes only the process it is loaded into and never writes to game memory.
- A worker thread is initialized after attach; it does **not** block inside `DllMain`.
- A temporary self-reference is held by the worker to avoid it being unloaded during polling. The synthetic host calls an exported stop/wait function to verify graceful completion. Removing the ASI file or replacing the loader while the game is running is **unsupported**.
- No address scanning, signature hooking, DRM circumvention or network traffic.

See [integration findings](INTEGRATION_FINDINGS.md) and [single-session strategy](TEST_STRATEGY.md).

## Alternative Puppet Native host

The [Puppet Native v1 DLL](PUPPET_NATIVE.md) now starts the same diagnostic worker on successful `PN_Init`, then stops and flushes the report on `PN_Shutdown`. Windows CTest exercises this integration without Resynced, using a deterministic fake controller. Puppet Native allows a **single plugin DLL**, avoiding installation of a competing proxy loader. GitHub Actions produces separate ASI and Puppet diagnostic archives; neither is a playable mod. Do **not** request a game launch for this milestone alone.
