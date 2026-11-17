# Runtime modding / native game integration — AC4 vs Resynced

**Purpose:** explain exactly which publicly documented runtime extension seams exist, what they permit, and what remains unknown for game-mechanic automation. Reviewed 2026-11-17. Source tags A/B/C/U follow [index](README.md). All patterns are build-specific; no third-party address tables are reproduced here.

## Extension/instrumentation matrix

| Capability | Original AC4 x86 | Resynced x64 | Critical limitation |
| --- | --- | --- | --- |
| ASI loader | [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader); e.g. AC4Coop and AC4Tools (A) | Author-mod-documented x64 loader; project own synthetic read-only ASI (A) | Loading a DLL is **not** proof a hook can safely run |
| Plugin host | [LionAG ScriptEngine](https://github.com/LionAG/ScriptEngine) for `.scrx` camera scripts on its target (A) | [Puppet Native](https://github.com/venvyl/puppet-native) published MIT **`PN_Api` v1** (A) | Neither supplies a general NPC combat API |
| Player/actor management | [AC4Coop](https://github.com/Kamzik123/AC4Coop) documents a live extra `Human` puppet (A), AI disabled | No confirmed game-native player/NPC enumeration, spawn or task scheduling method | Original x86 code cannot be ported by address translation alone |
| Action animation | AC4Coop `SetTargetAction`, layered animation/blends, explicit action playback (A) | Some BFR data mods replace finisher animations (B) | Animation dispatch is not native combat **eligibility**, damage or collision |
| Time/camera | [AC4Tools](https://github.com/45hgv43whgf/AC4Tools), [LionAG CameraTool](https://github.com/LionAG/ACBlackFlag-CameraTool) (A/B) | [Otis_Inf](https://opm.fransbouma.com/Cameras/acbfresynced.htm), [ACBFRTools](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/661) (A/B) | Pause/frameskip/camera restore ≠ world/AI/RNG restore |
| Native events | Original AC4 co-op action snapshot/readback and game update hook (A) | [HapticsBridge](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge) 21 movement/traversal state hooks; haptic event correlations (A) | No verified **enemy attack start, parry eligibility, or native dispatch** |
| Runtime combat pacing | AC4 data mods and time-scale; no universal native schema | [GoatedACBlackFlag #19](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/19) author-described attack/execution/assassination speed config (B) | Global or animation-speed modulation may not alter actual parry timing |
| Full deterministic test agent | None ready for either game | None ready for owner's BFR 1.0.6 | Requires legitimate engine event observer + command API + fixture reset, developed and verified |

## Puppet Native plugin API (BFR only)

**Primary public source:** [venvyl/puppet-native](https://github.com/venvyl/puppet-native), [public v1 header](https://github.com/venvyl/puppet-native/blob/main/plugin%20loader/PuppetNativeApi.h), [loader code](https://github.com/venvyl/puppet-native/blob/main/plugin%20loader/PluginLoader.cpp), [project's ABI report](../PUPPET_NATIVE.md).

- Plugins export **`bool __cdecl PN_Init(PN_Api*)`** and **`void __cdecl PN_Shutdown()`**. Public `PN_Api.version == 1` has logging, `GetGameModule`, `GetBase`, and INI helpers, **not** a ready-made `GetEnemy`/`ExecuteTakedown`/`SpawnEntity` command.
- The loader searches **`%APPDATA%/PuppetNative/plugins/*.dll`**. A matching `[Plugin: <name>]` stanza with `Enabled=1` in `plugin_config.ini` must enable it; API/v1 source agreement is separate from actual installed binary compatibility.
- This project already builds a **Puppet Native plugin variant reusing read-only diagnostic logic**, with synthetic host tests. **Not tested on the real game**, no game memory/gameplay hooks. Avoid installing both project ASI and Puppet variant.

## HapticsBridge state and event taxonomy (BFR source inspected)

**Primary:** [state mapping](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/References/state_mapping108.json), [synthetic hook tests](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/test_hooks.cpp), [license](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/LICENSE.txt).

- The reviewed JSON includes **21 enter/exit slots** for movement: jump, fall, climb, swim and haystack; includes 1.0.6/1.0.7/1.0.8 fields. The **specific synthetic hook test** actually tests transaction/rollback for **1.0.6/1.0.7**, not proof that every 1.0.8 hook works in-game.
- The instrumentation also contains a `XInputSetState` (game **rumble output**) observer. This cannot be equated to input polling, **semantic player action**, or **pre-input action eligibility**.
- Safely transferable idea: verify exact executable profile, all pointer originals and expected bytes, install hooks **atomically**, rollback on partial failure, refuse unknown build. **Do not copy source, signatures or tables from this non-commercial code** without license compliance.

## Original AC4Coop native state pattern (A)

[AC4Coop README](https://github.com/Kamzik123/AC4Coop) and `src/phase1.cpp` describe:
- Per-frame main-thread `Human` update callback and safe command sequencing.
- `Human`, `Entity`, `ActionComponent`, `CharacterController`, action alias, channel, layer, 46-slot blend tree, animation phase and transition descriptors.
- Spawned puppet per network peer; ambient spawn suppression for synchronized world; action playback while **puppet AI disabled**.
- F6 manual spawn and F7 diagnostics. The author explicitly lists animation/IK limitations.

This is **real original-game native research for a fingerprint-specific AC4 x86 build**, not a BFR SDK or a confirmed combat attack dispatcher. Investigate new x64 Resynced equivalents from first principles; do not reuse AC4Coop's raw RVA / struct offsets.

## Source-level hook engineering references

- [AC4Tools](https://github.com/45hgv43whgf/AC4Tools): exact SHA-256/image base, unique pattern scan, target expected bytes, no unsupported-build override; per-feature fail-closed.
- [PatchFix](https://github.com/playday3008/PatchFix): reusable, source-available MIT hook framework with per-game targets, crash journal and minidumps, `safetyhook` / `Hooking.Patterns` / `spdlog`; upstream as reviewed only **Rogue/Syndicate**, no AC4/BFR ready port.
- [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader): x86 vs x64; different proxy names `version.dll`/`dinput8.dll`/`winmm.dll` are **process-loading mechanisms** and can conflict with unrelated mods already claiming those DLL names.
- [Ghidra](https://github.com/NationalSecurityAgency/ghidra), [Frida](https://frida.re/), [x64dbg](https://x64dbg.com/), [ImHex](https://github.com/WerWolv/ImHex), [Capstone](https://github.com/capstone-engine/capstone), [pefile](https://github.com/erocarrera/pefile): generic static/dynamic binary research. They **do not provide built-in AC4/BFR SDKs**.
- [Universal Modder AC4 field notes](https://universal-modder.dev/games/assassin-s-creed-iv-black-flag/) demonstrate how an agent can keep an executable hash, PE/RVA byte oracle and symbolic function research record. **Must verify each note's source/status**; don't assume it applies to another build.
- [coop-anything fast-iteration](https://github.com/cranebeaker/coop-anything/blob/main/playbook/fast-iteration.md) and [hot-reload](https://github.com/cranebeaker/coop-anything/blob/main/playbook/hot-reloading-your-mod.md) offer **architectural examples**, not an existing in-game bridge. Main-thread RPC and hot reload require actual callback, teardown discipline, correct thread ownership and no stale pointers into unloaded code.

## Native input vs logical command: non-negotiable distinction

```text
physical controller edge / XInput state
    ↓ (unknown game input mapper + mode + held/release/gesture)
game logical command / native attack vs takedown resolution
    ↓ (eligibility: target, distance, posture, enemy type, animation)
game-issued action (attack / finisher / dodge / counter)
    ↓
animation, health/damage, sound, rumble, UI notifications
```

- Merely observing an **X button edge** tells nothing about whether X *could* select a finisher.
- A combat animation, SFX, Wwise PostEvent or rumble is **post-selection evidence**; may be useful for outcome correlation but does not prove pre-input eligibility.
- Original AC4 action playback can force an **animation**, but the game may not register a genuine combat interaction. Do not assign that result to the native attack/takedown API.
- First project target: detect a **verified pre-action semantic state** and independently invoke **one native logical action**, or return a concrete gap. Never silently remap input outside audited combat contexts.

## Hook safety and deployment contract

1. **No blind compatibility claims.** Record exact local EXE hash, target architecture, store version where observed and current active loader/module hashes. Treat store version alone as insufficient.
2. **Non-invasive by default.** Passive observation first, no memory writes on unsupported build; strict source labels in report.
3. **Transactional install:** fail if any hook site is not uniquely identified, code bytes/pointers differ, target is already chained by an unknown mod, or rollback is not possible.
4. **No main-thread blocking.** Do not call unsafe game methods from worker/IO threads; if eventually authorized, queue commands to a verified frame/update hook. Async game activity must not outlive unloaded DLLs.
5. **One owner-controlled test installation** (minimal module). Avoid simultaneous proxy DLL replacement or untested combinations; project already has ASI vs Puppet Native alternatives.
6. **Clean unsubscribe/restore** before unloading, no protected game asset uploads or report of raw memory/addresses/paths. See [workflow](../WORKFLOW.md), [diagnostics](../DIAGNOSTICS.md), [test strategy](../TEST_STRATEGY.md).

## Missing game interfaces (as of reviewed sources)

For Resynced **1.0.6**, no verified `IsTakedownEligible`, `GetEnemyAttackStart`, `GetParryWindow`, `SpawnOpponent`, `ForceAttack`, `ResetCombatAI` or `ApplyNativeLogicalCommand` API. These are **desired capabilities, not known function names**. The safest research moves are bounded, exact-build **read-only** engine-semantic observations before developing any runtime control.
