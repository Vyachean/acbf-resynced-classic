# Original Assassin's Creed IV: Black Flag (2013) — modding knowledge

**Scope:** Windows single-player `AC4BFSP.exe` **x86**; original AC4, **not** Resynced. Reviewed 2026-11-17. Unless marked otherwise, findings are upstream-source statements, not reproduced on the owner's installation. See [sources](SOURCES.md) and [runtime boundary](RUNTIME.md).

## Engine / version boundary

- Original PC game uses an older **AnvilNext / Scimitar** generation, with archive assets in `.forge` and typed `.data`; single-player target is `AC4BFSP.exe`. `AC4BFMP.exe` is not an interchangeable single-player target. The original process is **32-bit**, so loaders, plugins, Direct3D proxies and debuggers must match x86.
- Binary offsets from published AC4 research are **specific to one fingerprinted executable**. Example: [AC4Tools README](https://github.com/45hgv43whgf/AC4Tools) lists its supported Steam single-player SHA-256 and refuses unknown builds/relocated image/pattern mismatches. [AC4Coop](https://github.com/Kamzik123/AC4Coop) explicitly assumes an image base of `0x400000` and hardcoded addresses in its wrapper layer. **Those assumptions are not universal**; never port them into Resynced.
- The original player interaction model is an important reference, but should be validated per context against [control audit](../CONTROL_SYSTEM_AUDIT.md), [combat conditions](../COMBAT_CONDITIONS.md), and [2013 Prima guide](https://primagames.com/tips/assassins-creed-4-black-flag-advanced-tips). Example: original counter involves a B entry followed by a face-button action, not simply “B is dodge”.

## Native / actor / action research (primary source)

### [AC4Coop](https://github.com/Kamzik123/AC4Coop) — main technical reference (A)

The author publishes an original AC4 **x86 ASI** and Scimitar class wrappers:
- Two instances share an UDP state snapshot; a second `Human` **puppet** is created and removed on travel/disconnect. F6 requests spawn (network disabled), F7 dumps player and swim chain.
- Main-thread `Human` update callback drives game-owned calls. Motion uses `SetGlobalMatrixInternal`; `CharacterController::m_DriveEntity=false` prevents physics from overriding puppet placement.
- Action playback uses `ActionComponent::SetTargetAction` (alias, channel, layer) plus action-set override, 46-factor locomotion blend, blend-in descriptor and animation playback phase. `src/netstate.h` documents the state structure; `src/phase1.cpp` the application lifecycle. Inspect these **as conceptual call graphs**, not as a ready-made BFR ABI.
- The **normal working mode disables puppet AI**. A puppet showing a sword swing is not necessarily causing combat damage, hitting a target, or producing a parryable AI strike. The source does not establish a programmable combat arena. Its ambient spawn suppression exists for co-op synchronization, **not** as a safe generic AI reset API.
- Known author limitations: climbing IK, action phase drift over loops, character override mismatch, pause menu disappearance. Preserve these as caveats when using it as an animation reference.

**Research decomposition:** (1) obtain player `Human`, (2) enumerate world/entities, (3) identify action controller, (4) read action state, (5) independently test creating or driving a noncombat puppet, (6) only then investigate AI tasks/attack/hit logic. Steps (1–5) in **Resynced x64 are wholly unproven**; copying source layouts would be unsafe.

### [Original AC4Tools](https://github.com/45hgv43whgf/AC4Tools) (A)

Source and detailed README show a supported Steam x86 executable with **SHA-256 gate, image-base gate, expected-bytes preflight, unique pattern search and per-feature refusal**. In-game ImGui toggles include player/ship cheats, godmode, time-scale, noclip, free camera, input blocking and save-changing unlocks. Its bundled *x86* `dinput8.dll` Ultimate ASI Loader has provenance/checksum checks.

**Transferable engineering lesson:** check build before any memory probe, verify all target bytes/ownership, avoid attaching to already modified hooks, make every feature independently fail-closed. **Not transferable:** hardcoded RVAs, input structures, game's nominal build label, bundled proxy DLL. Unlocks can persist or softlock saves; only use disposable fixtures.

### [LionAG AC Black Flag CameraTool](https://github.com/LionAG/ACBlackFlag-CameraTool) (A/B)

Public README identifies a **Ubisoft Connect 1.07** target, separate [ScriptEngine](https://github.com/LionAG/ScriptEngine) loader and `.scrx` plugin file. Functions include free camera, first person, camera save/restore, world stop/slow motion, time-of-day and HUD toggle; `AC4_CameraTools.ini` supports reload with F5. Author warns that first person while sailing may crash, and freezing simulation can leave NPCs in A-pose.

This proves an *author-supplied camera/world-stop interface for that build*, **not** deterministic combat-state reset, AI commands or compatibility with AC4Tools' x86 loader. Avoid mixing injectors until isolated.

### Other native/RE research leads (A/C)

- [Universal Modder AC4 field notes](https://universal-modder.dev/games/assassin-s-creed-iv-black-flag/) collect original-AC4 experiments using **Ghidra**, `pefile` + Capstone byte checking, Frida, ASI and entity/transform studies. Each note uses its own executable fingerprint and has a declared status; verify a claim against the linked experiment, not the aggregate summary.
- [PatchFix](https://github.com/playday3008/PatchFix) is an **MIT ASI hook architecture** with a common C++ core and project-specific game targets, diagnostics, crash journals and reversible hook design. As of the inspected README its supported games are **Rogue and Syndicate, not AC4 or BFR**. Use as an architectural reference only.
- [Kirikirin engine reverse-engineering kit overview](https://kirikirin.com/en/blog/game-assassins-creed-modding-kit) reports Black Flag transform research and Rogue `forge.py`/LZO `.data` parser. **Rogue format v27 data code is not automatically AC4 or Resynced compatible**.
- [BlackFlagCoop by zzyrrup](https://github.com/zzyrrup/BlackFlagCoop) actually targets **Resynced x64** and publicly describes only smoke-test ASI + placeholder networking as of this review. Do not conflate it with the **working original-AC4 puppet** in Kamzik123/AC4Coop.

## Original AC4 data / gameplay mods (author claims, B)

Resource and gameplay mod descriptions are valuable because they identify *classes of engine behavior already changed by others*. The downloadable `.forge` files are third-party works and do not expose or license a stable schema.

| Source | Author-described modification | What it suggests / does not prove |
| --- | --- | --- |
| [More Frequent Double Counters #1029](https://www.nexusmods.com/assassinscreedivblackflag/mods/1029) (Oct 2026) | Faster enemy attack cadence, more simultaneous/double counters; **DataPC.forge** distribution and separate data version | Original AC4 **enemy scheduling/counter timing** is a high-value data research seam; exact resource entry and schema not proven by page |
| [Combat Changes #765](https://www.nexusmods.com/assassinscreedivblackflag/mods/765) | Stronger enemy archetypes, altered resistance, countering killstreaks | NPC combat behavior/defenses can be adjusted in original AC4 according to author; **not** BFR-compatible |
| [AC4 Combat Overhaul #302](https://www.nexusmods.com/assassinscreedivblackflag/mods/302) | Increased incoming enemy damage and other combat options | Damage and difficulty are separate from player input arbitration |
| [Yet another combat mod #494](https://www.nexusmods.com/assassinscreedivblackflag/mods/494) | Changes enemy defenses, removes easy counter kills and streak auto-kills | Data behavior modification precedent; no public scriptable enemy attack API |
| [AC3 Improved combat system #138](https://www.nexusmods.com/assassinscreedivblackflag/mods/138) | Attempts original AC4 combat rule changes inspired by AC3 | *Design comparison*, not evidence of an AC3 combat-engine port |
| [No slow motion for counter attacks #871](https://www.nexusmods.com/assassinscreedivblackflag/mods/871) | Changes **`51_-_Game Bootstrap Settings.data`** in `DataPC.forge` to suppress counter slow motion | **Named resource** worth studying for global timing / animation pacing, not parry-action semantic rules |
| [Adewale's Counters for Edward #1039](https://www.nexusmods.com/assassinscreedivblackflag/mods/1039) (Nov 2026) | Replaces Edward single/double counter kill animations; author warns some positioning artifacts | Existing animation swapping mechanism; visual replacement alone ≠ new AI/counter behavior |
| [Combat and Sprint Camera Animation Tweaks #695](https://www.nexusmods.com/assassinscreedivblackflag/mods/695) | Camera/transition changes through a data distribution | Camera animation components are a separately editable subsystem |

**Prioritize references that explicitly give a file path, resource type or independently attributable source**, not unverified “all combat is controlled by one number.”

## Asset, texture, animation, visual and compatibility paths

- [AnvilToolkit (official author project)](https://www.nexusmods.com/site/mods/455) supports original AC4 and BFR with **different** serializers; changelog documents original AC4-specific MeshShape export and updated AC3/AC4/Rogue file-reference lists. Use the [author wiki](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki) and [data page](ANVIL_DATA.md).
- [Resorep](https://www.nexusmods.com/assassinscreedivblackflag/mods/15) legacy **x86 D3D11 texture substitution** is widely used for AC4 cosmetic mods. Its proxy `d3d11.dll` / `dx11proxy32.dll` may conflict with ReShade/3Dmigoto/other wrappers. Treat 2020 installation recipes as legacy, not modern security guidance.
- [Original AC4 Vortex extension #971](https://www.nexusmods.com/site/mods/971) automates some install types including Resorep and full-forge replacement; **not** a compiler, asset editor or conflict-free merge engine.
- [2013 AC4 PCGamingWiki](https://www.pcgamingwiki.com/wiki/Assassin%27s_Creed_IV:_Black_Flag) lists graphics, controllers, widescreen/performance and modern-hardware pitfalls. These fixes are **not** proof of changing combat mechanics.
- [AC4 Modular Patch #1031 (alpha, Oct 2026)](https://www.nexusmods.com/assassinscreedivblackflag/mods/1031) author reports PhysX/cloth/high-FPS fixes, **only exact original Steam x86 executable**, uses `dinput8.dll`, `nvcuda.dll` and runtime patch components; warns to stop on existing conflicting proxies. **Do not run this merely because the game is “Steam AC4.”** Author's own package indicates antivirus warnings and limited hardware validation.
- [Original AC4 camera tool](https://github.com/LionAG/ACBlackFlag-CameraTool) is the strongest explicit old-game world-stop/photomode interface found in open source; its author has a **specific** Uplay build target.

## AC4 research gaps

1. Identify original game's **actual** `DataPC.forge` resource for enemy attack cadence and counter windows, with provenance, not copying mods.
2. Obtain one verified native **player state / action / attack result** event on the owner's x86 executable, rather than assuming AC4Coop addresses are universal.
3. Determine whether original action controller offers logical Attack/Counter/Finisher dispatch with an explicit semantic eligibility result (AC4Coop `SetTargetAction` replays an animation, not necessarily a game action).
4. Clarify pause, save reload and NPC AI/RNG persistence before labeling an original AC4 session a controlled arena test.
5. Maintain separate build/loader tables for original 1.07, Steam 2023 and any portable builds; no universally valid offset set.

## Already documented in this repository

[Project AC4 reference behavior](../CONTROL_REFERENCE.md) · [whole-system input audit](../CONTROL_SYSTEM_AUDIT.md) · [combat condition and enemy archetypes](../COMBAT_CONDITIONS.md) · [original AC4 diagnostic task #28](https://github.com/Vyachean/acbf-resynced-classic/issues/28)

**No playable mod is proven solely by these materials.**
