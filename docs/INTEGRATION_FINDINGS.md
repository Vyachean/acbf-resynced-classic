# Resynced integration feasibility — public-source investigation

**Research date:** 2026-11-15. **Decision:** **runtime control replacement is not yet proven**. The evidence supports a plausible **x64 ASI integration path and observation of some game states**, but **not** a validated way to read takedown eligibility or invoke individual actions.

No game was launched, and no executable was obtained for this investigation.

## Evidence inventory

| Source / component | Verified public evidence | What it **does not** prove | Redistribution/licensing |
| --- | --- | --- | --- |
| **Ultimate ASI Loader** | Several Resynced mods document an x64 ASI loader as `version.dll` alongside `ACBlackFlag.exe`. | That any particular loader build works with the user's installed version or with Puppet Native simultaneously. | Third-party loader; do not bundle it without verifying upstream license/permissions. |
| **ACBFHapticsBridge** | Public C++ sources and tests include executable profiles for TU 1.0.6/1.0.7/1.0.8, input and selected gameplay-state instrumentation, and version-dependent hook validation. A published state mapping covers **21 enter/exit hooks for movement/traversal/swimming/haystack**, with checks/rollback tests. | That those hooks can observe enemy **takedown eligibility** or call **attack/finisher**. The state table inspected is **not** an action-dispatch API. | Upstream is under its own **non-commercial** source license. We may learn from documented techniques, but must **not copy licensed source, generated pattern tables, or binaries** into this proprietary repository without a compatible agreement. |
| **QuickSaveF5** | The mod documents a compatible x64 ASI loader and calling a native save routine. | That a gameplay action dispatcher exists at a known address, or that ASI collision is solved. | No copying; mod-specific terms need checking before any reuse. |
| **Puppet Native** | Public **MIT-licensed GitHub source** includes `PN_Api` v1 and loader lifecycle, plus Resynced support claim. | A gameplay action API, confirmed loader interoperability, or compatibility with a specific executable. Earlier user-reported startup collisions are unverified for a clean environment. | No redistribution or use of internals without permission. |
| **Unrestricted Combat Finishers** | A TU 1.0.8 data mod claims to change finishers triggered by normal attacks using `Edward_Default.data` / forge packages. | That it restores AC4's **input priorities**, helps 1.0.6, or can be incorporated safely with other data mods. | Nexus author forbids modification/redistribution without permission. Not usable as a dependency or code source. |
| **Resynced built-in controls** | Official help confirms rebinding controls and changing press/hold/double-tap/toggle modes. | A public scripting API, read-only state stream, or force-action API. | Ubisoft game internals/data must not be shipped in this repo. |

### Source URLs

- HapticsBridge public repository: https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge
- HapticsBridge compatibility/state mapping (study only): https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/References/state_mapping108.json
- HapticsBridge version-safe hook tests (study only): https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/test_hooks.cpp
- HapticsBridge license (do **not** copy code): https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/LICENSE.txt
- QuickSaveF5: https://www.nexusmods.com/assassinscreedblackflagresynced/mods/536
- Puppet Native: https://www.nexusmods.com/assassinscreedblackflagresynced/mods/566
- Unrestricted Combat Finishers: https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635
- ASI loader instructions/diagnostic patterns from another mod: https://www.nexusmods.com/assassinscreedblackflagresynced/mods/111
- Ubisoft official controls/customization: https://www.ubisoft.com/en-gb/help/assassins-creed-black-flag-resynced/gameplay/article/customising-controls-in-assassin-s-creed-black-flag-resynced/000116769

These links describe independently published projects and **do not** establish a legal right to incorporate their code.

## What is actually known

1. **ASI is a plausible distribution form** for a Windows Resynced runtime mod. Independent developers have shipped x64 ASI files; filenames like `version.dll` are **loader proxy names**, not interchangeable replacements with guaranteed coexistence.
2. **Some player movement-state instrumentation is feasible** as evidenced by HapticsBridge's published source/test assets, though this has not been reproduced independently here or proven on the user's exact executable.
3. **Selected game-native routines can be called** by some mods, e.g. a save function; this is not transferable proof for the action dispatcher.
4. **Data-file modifications can change finisher animations/conditions** according to mod descriptions. This is a separate seam from controller event arbitration and is not the first-choice solution for our input-first scope.

## What is not established

- Which input API(s) the target game uses for semantic action selection; controller polling is not equivalent to dispatch.
- Whether and where **attack/takedown/assassinate eligibility** is exposed to a third-party mod.
- Whether a finisher can be invoked independently of its bound physical button.
- Whether a loader can be **safely reused** in the user's setup without replacing an existing `version.dll`.
- Exact DLL/EXE hashes for the user's installation, supported store channel, or compatibility of a future project build.
- Whether a built-in mapping conflict survives the same test on official TU 1.0.8; community experiences conflict.
- Whether Puppet Native API v1 remains ABI-compatible with the user's installed v1.5 binary (**public source confirms the contract; deployed binary not tested**).

## Integration architecture decision

**Option A — configuration-only remapping:** reversible, no executable hooks, but does not solve verified context-priority failures. Keep as optional degraded profile, **not** as proof of an AC4-like mod.

**Option B — original standalone x64 ASI + pure decision core:** **preferred investigation**. Separate input collection, verified context/eligibility, decision rules, and action dispatch. Require strict executable fingerprint and fallback for unsupported versions. Do not assume old patterns/offsets can be used verbatim.

**Option C — plugin hosted by Puppet Native:** contingency only after confirming a documented interface and whether it can coexist with the user's installed mods.

**Option D — changing packaged game data:** research only if action-level integration is impossible and data edits clearly affect the target rule; conflicts/versioning/permissions make this unsuitable as the initial controller layer.

### Go/no-go gates

| Gate | Current verdict | Next evidence |
| --- | --- | --- |
| Can original mod logic compile and be tested without games? | **Yes in principle**, with an isolated resolver; no game-facing source currently exists. | Implement pure resolver and CI test fixtures. |
| Can an ASI load in an identified environment? | **Plausible; not verified for this project.** | Build safe no-hook diagnostic first, then a single controlled game session if necessary. |
| Can game context distinguish valid takedown and assassination from attack? | **Unknown; central blocker.** | Public code/disassembly study first; only then read-only in-game probe if unavoidable. |
| Can selected action be invoked without duplicate binding conflicts? | **Unknown; central blocker.** | A narrow action-dispatch proof, staged disabled until verified. |
| Can unsupported builds fail closed and be uninstalled cleanly? | **Architectural requirement, not demonstrated.** | Version guard + rollback tests; no unverified patches. |

**Therefore:** not yet safe or honest to ship a playable X-fix. The next implementation should be **an offline, deterministic action-resolution core** with synthetic cases, while continuing the action-dispatch research. It must remain independent of external mod code.

## Minimal-risk future diagnostic: a single launch

Before asking for an in-game session, GitHub Actions should build/test one **self-contained, original, read-only-by-default** diagnostic ASI, with:

- module fingerprint and loader path/availability status, without leaking full user paths;
- evidence of controller press/hold/release and potential action-state signals, **only if safely verified**;
- a compact ring-buffer log of the player's prepared X-01..X-05, B-01/B-02 and RT-01 cases;
- feature probes that report `NOT_SUPPORTED` / `NOT_OBSERVABLE` explicitly instead of silently passing;
- hard gate that disables invasive hooks on unsupported game hashes or unexpected byte patterns;
- clean uninstall (remove project-owned files) and no networking.

**Do not** request a game launch merely to identify the EXE hash unless all other preflight/diagnostic questions can be combined in the same session. A stand-alone checksum tool would violate the minimum-operator-work rule.

See [single-session strategy](TEST_STRATEGY.md), [controller matrix](CONTROL_REFERENCE.md), and [testing protocol](TESTING.md).

## Original read-only diagnostic module (implementation status)

This repository now contains its own [Windows x64 ASI](DIAGNOSTICS.md), built by GitHub Actions and loaded by a synthetic Windows test host. It **does not** rely on third-party hook signatures, and is **not** a live gameplay hook. It provides executable hash, local loader/module inventory and XInput edge sampling, explicitly marking semantic action-availability and game dispatch as unsupported.

This is useful infrastructure, but does not reduce the main blocker: identifying game-facing **action eligibility + dispatch**. The ASI must **not** be presented as fixing gameplay and should not trigger a premature game launch.

## Input API provenance without launching the game

Our [diagnostic implementation](DIAGNOSTICS.md) now includes an original read-only PE32+ **static import-table inspector**, plus a snapshot of loaded input-related DLL names **before** the observer itself opens XInput. Both are tested in Windows GitHub Actions, including intentionally malformed PE fixtures.

This is meant to narrow hypotheses around `XInput`, `GameInput` and `DirectInput` during a future combined session. The observation is strictly **HINT_ONLY_NOT_PROOF_OF_GAME_USAGE**. Static imports cannot reveal dynamic resolution, an injected proxy, or actual gameplay action selection.

Public HapticsBridge source also distinguishes GameInput and XInput input backends and exposes a game **XInputSetState IAT observer** for haptic output. This is not evidence of a safely usable `XInputGetState` input interception point or a native attack/takedown dispatch API; do not equate output vibration with a player action. See source: https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/src/main.cpp

**Still unresolved:** which semantic input/action boundary permits classic contextual controls.

## Confirmed Puppet Native plugin ABI (2026-11-15)

Earlier research understated the public availability of Puppet Native's plugin interface. The **public MIT-licensed repository** publishes a C ABI v1; our source-only compatibility plugin compiles and is checked with a synthetic host in Windows Actions. See [Puppet Native integration assessment](PUPPET_NATIVE.md) for exact signature, loader path and limitations. This is a meaningful integration seam; **it is not a game action dispatcher**.

## 2026-11-15: native action feasibility, not generic telemetry

- Maintainer-confirmed: mapping Attack and Takedown to one button produced **Attack only**. This observation should not be asked again; whether the HUD actually exposed Takedown on that attempt was not supplied.
- [AnvilToolkit 1.3.7](https://www.nexusmods.com/site/mods/455) lists Black Flag Resynced Forge/Data format support. The independent [Unrestricted Combat Finishers](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635) alters MicroKill/SoftKill animation behavior from **lethal basic attacks** through `Edward_Default.data`. The mod claims **1.0.8 only**; its author prohibits modification/reuse. This is a promising **data-driven animation avenue** but **not evidence** of 1.0.6 classic button dispatch, a pre-action eligibility signal, or a callable native command.
- MIT-licensed Puppet Native PluginLoader ABI v1 provides a process base and lifecycle but **not** an action execution API. Public HapticsBridge event observation is retrospective for combat sound events; it does not prove pre-press selection.
- Implemented a **two-phase, fail-closed X action command contract**, with independent evidence gates and mock-only native sink, under [`action_bridge.h`](../include/acbf/action_bridge.h). No game-specific address or fake native action API is introduced. It can prevent duplicate speculative dispatch **once** real, verified game-side input ownership and native command interfaces are available. Until then, it never intercepts X.

## 2026-11-16: selected local Resynced combat-resource decode

The earlier `RESYNCED_X_VERTICAL_SLICE.md` statement that no named combat entry had been decoded was superseded by a read-only experiment on the locally available installation. This installation is not authenticated as clean or as TU 1.0.6: the executable's file/product version fields are empty and the directory contains third-party loader/ASI components. No local executable, logs, resource bytes, installation hash, or third-party modified file is included in this repository.

- AnvilToolkit 1.3.7 parsed a local version-50 Forge index and decoded the selected `Edward_Default` payload into 338 objects with its embedded Resynced schema.
- Decoded states include Regular Attack, Regular Attack Finisher, Parry Success, Final Parry Success, Basic Takedown Initiator, and numerous contextual assassination states. Attack and finisher objects are `FightLogicActionStateAttack`; Takedown initiator objects are `FightLogicActionStatePaired`.
- The regular-attack finisher transitions to Microkill/Softkill initiators after an attack. This agrees with the *kind* of post-lethal animation behavior described by Unrestricted Combat Finishers, but does not expose the pre-X selection of Attack vs Takedown.
- The inspected attack state's base attack data has `IgnoreBlock=False` and `IgnoreParry=False`. A deeper schema decode of `Edward - Parry` resolved the `AtomMarkupID` names and found 0–300 ms and 0–130 ms `FightLogicTimeInSecondsProvider` windows. The same state has an `InsideWindow` action and a `BeforeEndOfWindow` `ActionActivateProjectileParryData` operation, but the toolkit loses field-to-handle identity for their window references. These times cannot yet be assigned to the melee counter or projectile-parry path; changing them would still be speculative.
- Candidate combat-repository and input-definition IDs named by the available 1.07/1.08 FGFL maps were absent from the 186 local Forge indexes scanned. The map provenance is unknown, and therefore cannot be used to resolve this installation's missing command-input mapping.
- Ghidra found an SDL event pump that consumes controller axis/button/device events. The pump proves low-level controller handling only; no player logical-command boundary or pre-action eligibility was established.

Status: **DATA_PARTIAL**, with the exact version/provenance and an independently interpretable patch point still unresolved; **COMMAND_BLOCKED_ON_AVAILABLE_EVIDENCE**; **NATIVE_BLOCKED**. The local file-set findings must not be generalized to a clean supported build. No mod/game file was changed and no game was launched.

