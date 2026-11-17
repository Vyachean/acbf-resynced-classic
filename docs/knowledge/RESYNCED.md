# Assassin's Creed Black Flag Resynced (2026) — modding knowledge

**Scope:** Resynced PC **x64**, not original AC4 x86. Reviewed 2026-11-17. The owner's reported target is title update **1.0.6**, but exact executable SHA-256/distribution and loaded loader are not independently established. Do not equate 1.0.8 mod support with 1.0.6.

## Game platform, known generations and version boundaries

- The game executable is commonly named `ACBlackFlag.exe`; some tools mention `ACBlackFlag_Plus.exe` on additional channels. The newer assets use **FORGE v50**, layered boot patches, typed Anvil data and prefetch metadata. Many engine/gameplay subsystems are remade; original AC4 executable offsets, classes, binary patterns, input arbitration and 2013 `DataPC.forge` cannot simply be used.
- Key update boundary: official [TU 1.0.6](https://www.ubisoft.com/en-gb/game/assassins-creed/news/5c72WnN2ot90ii8d9Le1Wt/ac-black-flag-resynced-title-update-106-patch-notes), [1.0.7](https://www.ubisoft.com/de-de/game/assassins-creed/news/24bBcwoOoYNZ2gt2yi3HdH/ac-black-flag-resynced-title-update-107-patch-notes), [1.0.8](https://www.ubisoft.com/en-us/game/assassins-creed/news/5Z1RxwznM6CcrkSEVoibmh/ac-black-flag-resynced-title-update-108-patch-notes) each change state-specific gameplay behavior. Maintain build-specific status; an update note does not confirm a mod is compatible with that update.
- [Official Ubisoft combat breakdown](https://www.ubisoft.com/en-gb/game/assassins-creed/news/1A8XkbnvONciecEZm9my1C/assassins-creed-black-flag-resynced-combat-system-breakdown): normal/perfect parry, separate dodges, unparryable attacks and adaptive opponents. For testing, classifying incoming attack type and AI history matters.
- Major internal tool routes: data-level `AnvilToolkit` (typed resources + thin patch-FORGE), ASI/native hooks, optional Puppet Native plugin API, camera/time controls, and game- or launcher-specific mod packages. They are **distinct interfaces**; there is no documented unified user-created gameplay SDK.

## Resource/data editing: most actionable path (A + B)

[AnvilToolkit 1.3.7+](https://www.nexusmods.com/site/mods/455) and [author patch-forge wiki](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki/Creating-a-New-Patch-Forge-for-Assassin's-Creed-Shadows-or-later) explicitly support **BFR additive patch FORGEs** without repacking the game's large shipped archive. The wizard computes next patch number and prefetch-chain metadata. Copy resource data from *highest existing override*, inspect dependencies and **never just rename `patch_02` to `patch_03`**. See [Anvil data technical notes](ANVIL_DATA.md).

**Research clues from combat/gameplay author mods:**

| Source (B: author feature claim) | Claimed modified behavior | Likely research target, not verified schema |
| --- | --- | --- |
| [Easy Parry #617](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617) | More forgiving parry/light/heavy windows and double-counter opportunities | Defensive timing / action state data; **exact entry & compatibility unknown** |
| [Unrestricted Combat Finishers #635](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635) | MicroKill/SoftKill from ordinary lethal attacks, `Edward_Default.data` in patch resources | Finishing **animation selection**, not proof of native pre-X Takedown eligibility; author 1.0.8 |
| [Assassination Impact / Relentless Combat #587](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/587) | More frequent enemy attack succession, double takedowns, camera shake | NPC attack cadence / counter opportunity data, author 1.0.8; named schema unknown |
| [The Padre: Captain Combat Style #602](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/602) | Makes companion use Captain-like combat archetype and selected weapons | NPC combat-style reference / EntityBuilder class, **not runtime AI command** |
| [Heroes of the Jackdaw #569](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/569) | Makes named companions participate in boarding encounters | Actor/encounter participation, not arbitrary NPC spawn |
| [City Under Siege #618](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/618) | Greater patrol population and elite units | Crowd/encounter density, not a deterministic test arena |
| [Double Assassination Fix #597](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/597) | Author claims specific double-assassination correction | Selection/eligibility research lead; mechanism and build support unknown |
| [Unarmed Combat #336](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/336) | Alters player unarmed fight option | Weapon/combat-context route, unverified impact on events |
| [Lethal Combat #451](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/451) | One-strike lethality / difficulty modifications | Damage events may be **microdamage from non-attack actions**; not an action oracle |
| [Black Flag Overhauled #659](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/659) | Modular original-water animations, movement, prompts | Animation/UI data feasibility through author's proprietary packages, **1.0.8 only** |

**Do not download a mod and copy or reverse-engineer its binary payload into this all-rights-reserved project unless license permits.** Research independently from authorized game files and published technique. Author statements do not prove a readable numeric “parry window” resource on 1.0.6.

## Runtime tools / plugin host / telemetry

| Source | Known published ability | Important boundary |
| --- | --- | --- |
| [Puppet Native v1.5 / upstream](https://github.com/venvyl/puppet-native) | MIT `PN_Api` v1 plugin DLL lifecycle, module base, logging, INI API, plugin folder; this project builds a **read-only** plugin host and runs synthetic tests | **No exposed action eligibility/dispatch or AI spawning**. Installed binary/version/loader coexistence untested. [Full ABI](../PUPPET_NATIVE.md) |
| [ACBFHapticsBridge source](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge) | Sources for 1.0.6/1.0.7/1.0.8; input/haptic engine hook work; JSON maps **21 traversal-state** transitions; synthetic 1.0.6/1.0.7 transaction rollback tests | Source is under **non-commercial license**. Parry haptic reaction ≠ pre-action eligibility or enemy attack start. Do not reuse its pointer tables or copyrighted implementation |
| [GoatedACBlackFlag #19](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/19) | Author claims live-configurable **attack/execution/assassination animation speed**, context-sensitive camera and hot-reload config `SDK_DIR/GoatedACBlackFlag.conf` | **Pacing** hypothesis, not a logical action/API. Adds possible proxy and hooks, may conflict |
| [ACBFRTools #661](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/661) | F4 menu, game speed, Edward/ship, weather, noclip, free camera and save edits | No public scripting RPC or arbitrary AI API verified; build support unknown; loader collision possible |
| [Otis_Inf camera tools](https://opm.fransbouma.com/Cameras/acbfresynced.htm) | Time-stop, frameskip, adjustable speed, free camera | Camera state ≠ AI/world reset; timestop not a proof of deterministic timing oracle |
| [QuickSaveF5 #536](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/536) | Calls native save logic according to author | Save **does not** necessarily capture AI RNG, action state or a precise encounter reset |
| [Encrypted's Ultimate Rewrite #64](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/64) | CE-based teleport / tuning / freeze tools | Manual reconnaissance only; memory modifications contaminate baseline |
| [Forge Injector v1 BETA #108](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/108) | Resource injection in BFR FORGE v50 | No guarantee of logical `.data` validity or native runtime event interface |

**Repository-specific diagnostic:** [`docs/DIAGNOSTICS.md`](../DIAGNOSTICS.md) describes two alternative synthetic-tested read-only Windows hosts (ASI or Puppet Native), with input sampling/module fingerprint and explicit NOT_OBSERVABLE semantic fields. **Neither has been proven to work in the owner's game build nor can control combat.**

### Mod managers — carefully distinguish

- [Nexus official **BFR Vortex extension #1977**](https://www.nexusmods.com/site/mods/1977): adds BFR as a Vortex-managed game (author NexusMods). This is a **general mod-management frontend**, not a game engine action API and does not validate a conflicting FORGE override.
- [AC Mods Launcher](https://github.com/nickythesmithy/ac-mods-launcher) / [Nexus #658](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/658): portable launcher supports author's modular `.acmod` packages and author-reported mod conflict detection; explicitly **BFR 1.0.8 only**, and not all downloaded ZIP mods. Installs loader files; its Uninstall button may remove the entire mods directory, including unrelated mods.
- [ReForger](https://github.com/QuilLeeR/ReForger): upstream README documents **Shadows v42**, while [EK Replacer #636](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/636) asserts a later build can support a BFR workflow. **Contradictory scope**. Its `.rfgb` is unrelated to `.acmod`; verify actual binary before using.
- [BFR Modding Workshop #197](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/197): asset/appearance, save/character edits and revert workflow; no documented AI or combat SDK.
- [Black Flag Texture Replacer #364](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/364): browser texture replacement tool, not behavior editor; audit processing/licensing before uploading any game assets into a web service.
- [Jackdaw Flag Patcher (MIT)](https://github.com/emike09/Jackdaw-Flag-Patcher): read/verify/append single texture/redirect index/restore technique, though modifies existing FORGE instead of additive patch. Good pattern for transactional rollback.

## Historical local static evidence

[Sanitized static checkpoint](LOCAL_STATIC_EVIDENCE_2026-11-15.md) (historical PR #27) includes a metadata-only BFR v50 index enumeration and one checked GFL → EntityBuilder resource candidate. **Missing registered component schema** still prevents tracing behavior to actual player/NPC ownership, while the local title update was not authenticated. This is not a tested Resynced gameplay API.

### Community movement mods — why they are not a classic-control replacement

- [Classic Movement — Walk By Default Smooth Sprint #408](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/408) describes walking by default, configurable movement scale and sprint by RT/L3. Author claim; **not** proof of restoring original high/low-profile mode, native counter/takedown actions or all parkour contexts.
- [Walk By Default for PlayStation Controllers #514](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/514) describes a PlayStation-specific walk-first controller profile. Author claim; it introduces additional bindings to audit and is **not** a universal classic gameplay layer.

Both were identified in historical research [PR #32](https://github.com/Vyachean/acbf-resynced-classic/pull/32). This section replaces a redundant stand-alone community comparison and must remain version/build-specific.

## Is there an NPC, arena or character command API?

**No confirmed public API of that kind has been found for BFR 1.0.6** in the inspected sources as of 2026-11-17.

- Some mods modify **population, encounter participation or actor combat archetype** in data. This is not equivalent to calling `spawn_npc`, `set_ai_target`, `attack(player)`, or `reset_rng` at runtime.
- **Original AC4Coop** has a manually spawned `Human` puppet **in original x86 AC4**. In its working replication path **AI is disabled**. Copying an animation is not commanding a combat AI to hit, nor proof that animation dispatch validates real damage.
- **zzyrrup/BlackFlagCoop** [README](https://github.com/zzyrrup/BlackFlagCoop) mentions *Resynced* but explicitly says it is only a safe x64 ASI initialization/placeholder-networking milestone, with **no game memory hooks or real positional sync**. **Do not confuse this repo with Kamzik123/AC4Coop.**
- Main target for future runtime hook discovery: identify a **verified 1.0.6 player/enemy semantic state** or native action dispatch in independent binary analysis. Must meet exact build, rollback and license gates.

## Install/loader conflict matrix

One game directory may already have `version.dll`, `dinput8.dll`, `winmm.dll`, `ACBlackFlagFix.asi`, Puppet Native modules, a camera trainer and active `DataPC_boot_patch_XX.forge` modifications. **Do not stack or overwrite loaders just because each mod author says “install this DLL.”** The owner's previous experimental runtime failed when some `version.dll` combinations coexisted. Before any game-facing test:

1. Audit exact game EXE and active proxy DLL **hashes/bitness** (without uploading executables), backup relevant files and save, map module owners and hook collision risks.
2. Baseline with original unmodified runtime except the single known-compatible loader; keep camera/time/AI cheats disabled.
3. If a plugin is needed, prefer an already proven Puppet Native instance with the project DLL, otherwise a separately packaged compatible **x64** ASI host; do not deploy both simultaneously.
4. Collect one sanitized session; fail closed on mismatch. **No executable patching as a side effect of documentation reconnaissance**.

## Resynced research priority and cross-links

- **Data:** original 1.0.6 `Edward_Default.data` graph; independently discover attack/parry/finisher conditions; use additive, reversible forge patch [Anvil page](ANVIL_DATA.md).
- **Native:** engine-semantic attack/finisher eligibility and dispatch for **context-sensitive X**, not merely input/haptic/animation observation [integration study](../INTEGRATION_FINDINGS.md).
- **NPC:** map true actor/entity type and combat archetype resources from original game-owned data; avoid confusing scene density with runtime attack scheduling [ecosystem survey](../MODDING_ECOSYSTEM.md).
- **Testing:** single combined read-only opt-in session once instrumentation passes gates, use source-labelled outcome oracle [test harness](../RUNTIME_TEST_HARNESS_RESEARCH.md).
- **Version/behavior:** Ubisoft patch notes 1.0.6–1.0.8 and [combat conditions](../COMBAT_CONDITIONS.md), [whole control audit](../CONTROL_SYSTEM_AUDIT.md).

**Not implemented:** resynced native combat control mod; programmable arena; deterministic enemy attack API; verified player input router. Any successful synthetic CI build is not game-side evidence.
