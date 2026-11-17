# Source register — original AC4 and Black Flag Resynced modding

**Last primary-source review: 2026-11-17.** **Machine-readable companion:** [research/modding_sources.json](../../research/modding_sources.json) (88 curated source URLs, scope, evidence grade and annotations; regenerate/update when the human index changes). This curated catalogue maps toolchains, official documentation, public code and mod-author **claims**. It is **not** a copy of external source, a license grant, a tested compatibility list or a claim to have indexed all internet content. For precise findings follow [original AC4](ORIGINAL_AC4.md), [Resynced](RESYNCED.md), [Anvil](ANVIL_DATA.md), [runtime](RUNTIME.md), [testing](TESTING.md) or [detailed ecosystem survey](../MODDING_ECOSYSTEM.md).

**Grades:** **A** — public source/official author docs inspected; **B** — mod author's feature/version description; **C** — community/secondary; **U** — unresolved/404/contradictory. No empirical game-test grade E is asserted here. Source page review does **not** establish safe working support for arbitrary installed versions. Tags: **AC4** original x86, **BFR** Resynced x64, **Both** only when the source separately covers both.

## Official docs, game mechanics and format authorities

| Source / link | Scope / grade | Research value and limits |
| --- | --- | --- |
| [Ubisoft TU 1.0.6 patch notes](https://www.ubisoft.com/en-gb/game/assassins-creed/news/5c72WnN2ot90ii8d9Le1Wt/ac-black-flag-resynced-title-update-106-patch-notes) | BFR A | Parkour/sprint/takedown state changes, not third-party compatibility |
| [Ubisoft TU 1.0.7 patch notes](https://www.ubisoft.com/de-de/game/assassins-creed/news/24bBcwoOoYNZ2gt2yi3HdH/ac-black-flag-resynced-title-update-107-patch-notes) | BFR A | Different state fixes from 1.0.6 |
| [Ubisoft TU 1.0.8 patch notes](https://www.ubisoft.com/en-us/game/assassins-creed/news/5Z1RxwznM6CcrkSEVoibmh/ac-black-flag-resynced-title-update-108-patch-notes) | BFR A | Later update, not assumption for target 1.0.6 |
| [Ubisoft Resynced combat breakdown](https://www.ubisoft.com/en-gb/game/assassins-creed/news/1A8XkbnvONciecEZm9my1C/assassins-creed-black-flag-resynced-combat-system-breakdown) | BFR A | Parry/dodge/attack classes and adaptive enemies |
| [Ubisoft Resynced input customization](https://www.ubisoft.com/en-gb/help/assassins-creed-black-flag-resynced/gameplay/article/customising-controls-in-assassin-s-creed-black-flag-resynced/000116769) | BFR A | Press/hold/double-tap rebinding, no public native command API |
| [Prima AC4 advanced combat](https://primagames.com/tips/assassins-creed-4-black-flag-advanced-tips) | AC4 C (period guide) | B counter follow-up face actions, not exact runtime timings |
| [Prima AC4 combat eGuide](https://primagames.com/eguides/assassins-creed-iv-black-flag-eguide/primer/combat) | AC4 C | Combat rules and special cases, not code |
| [Prima AC4 enemy archetypes](https://primagames.com/eguides/assassins-creed-iv-black-flag-eguide/reference-analysis/enemy-archetypes) | AC4 C | Susceptibility matrix, contains reported internal guide inconsistency |
| [PCGamingWiki original AC4](https://www.pcgamingwiki.com/wiki/Assassin%27s_Creed_IV:_Black_Flag) | AC4 C | Graphic/performance/controllers, not engine action API |
| [Original AC4 Steam guide — DEPRECATED](https://steamcommunity.com/sharedfiles/filedetails/?id=2288998586) | AC4 C | Historical old Forge/outfit workflow; superseded tools |

## Anvil/asset editing / resource documentation

| Source / link | Scope / grade | Research value and limits |
| --- | --- | --- |
| [AnvilToolkit main Nexus entry](https://www.nexusmods.com/site/mods/455) | Both A | Official maintained tool, version/changelog and game support |
| [AnvilToolkit resources / author GitHub](https://github.com/Kamzik123/AnvilToolkit-Resources) | Both A | File lists, tutorials and author wiki |
| [AnvilToolkit wiki home](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki) | Both A | Author's current resource guides |
| [New BFR patch forge guide](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki/Creating-a-New-Patch-Forge-for-Assassin's-Creed-Shadows-or-later) | BFR A | 1.3.7+ additive patch, chain metadata, boot-crash caveats |
| [Texture modding in custom forges](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki/Texture-Modding-in-Custom-Forges) | BFR A | Mips/RawAsset and prefetch rules |
| [Forge Injector v1 Beta #108](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/108) | BFR B | FORGE v50 low-level importer; no AI API |
| [Jackdaw Flag Patcher source](https://github.com/emike09/Jackdaw-Flag-Patcher) | BFR A | MIT transactional asset patch and restore |
| [BFR Texture Replacer #364](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/364) | BFR B | Browser-based texture replacement, not combat editor |
| [BFR Modding Workshop #197](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/197) | BFR B | Visual/save editor with revert, not NPC control |
| [ReForger source README](https://github.com/QuilLeeR/ReForger) | Shadows only (A), BFR U | Public README says Shadows Pipeline v42, third-party claims new BFR support |
| [EK Replacer BFR #636](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/636) | BFR B | Claims ReForger newer release use; verify conflict with README |
| [AnvilToolkit AC4 Nexus mirror](https://www.nexusmods.com/assassinscreedivblackflag/mods/222) | AC4 A | Original AC4 tool features and game-specific type support |
| [Kirikirin AC/Rogue/Black Flag RE kit](https://kirikirin.com/en/blog/game-assassins-creed-modding-kit) | Related A/C | Rogue v27 parser / LZO notes, Black Flag transform; validate per build |
| [ACViewer docs](https://github.com/forge-master/ACViewer) | AC family U | Historical format exploration, verify real availability/fit before adopting |
| [Steam AC4 Resorep guide](https://www.nexusmods.com/assassinscreedivblackflag/mods/15) | AC4 C | Legacy 32-bit D3D texture injection; may collide with renderer wrappers |

## Native runtime tooling / public source / automation patterns

| Source / link | Scope / grade | Research value and limits |
| --- | --- | --- |
| [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) | Both A | Architecture-matching loader, proxy ownership matters |
| [Puppet Native MIT source](https://github.com/venvyl/puppet-native) | BFR A | Plugin loader and native ABI, not action scripting |
| [Puppet Native v1 API header](https://github.com/venvyl/puppet-native/blob/main/plugin%20loader/PuppetNativeApi.h) | BFR A | `PN_Init`/`PN_Shutdown`, module base, INI/log callbacks |
| [Puppet Native loader source](https://github.com/venvyl/puppet-native/blob/main/plugin%20loader/PluginLoader.cpp) | BFR A | Plugin discovery/enable/disable semantics |
| [Puppet Native Nexus #566](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/566) | BFR B | Parkour improvements + plugin UI feature claims |
| [HapticsBridge public source](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge) | BFR A | Version profiles, input/effects and limited state hooks; non-commercial license |
| [HapticsBridge state-mapping JSON](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/References/state_mapping108.json) | BFR A | 21 movement/traversal hooks across TU fields, **not combat start** |
| [HapticsBridge hook synthetic tests](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/test_hooks.cpp) | BFR A | 1.0.6/1.0.7 transaction/rollback, not live 1.0.8 proof |
| [HapticsBridge noncommercial license](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/LICENSE.txt) | BFR A | Study only, no unlicensed code/pattern copying |
| [AC4Coop source](https://github.com/Kamzik123/AC4Coop) | AC4 A | Working author-reported x86 puppet/action/transform synchronization, AI disabled |
| [Original AC4Tools source](https://github.com/45hgv43whgf/AC4Tools) | AC4 A | SHA-256, bytes/unique-pattern gates, ImGui, time-scale, input/camera |
| [Original AC4 camera source](https://github.com/LionAG/ACBlackFlag-CameraTool) | AC4 A | Camera/world pause; Ubisoft Connect build 1.07 |
| [LionAG ScriptEngine](https://github.com/LionAG/ScriptEngine) | AC4 A | `.scrx` loader; **not** generally supported across store builds |
| [Resynced BlackFlagCoop smoke-test source](https://github.com/zzyrrup/BlackFlagCoop) | BFR A | x64 ASI/placeholder network, **no gameplay hooks or real co-op** |
| [AC family PatchFix source (MIT)](https://github.com/playday3008/PatchFix) | Rogue/Syndicate A; AC4/BFR U | General hook architecture, not a ready plugin for either target game |
| [Universal Modder original AC4 field notes](https://universal-modder.dev/games/assassin-s-creed-iv-black-flag/) | AC4 A/C | Agent RE techniques and empirical notes, per-note version/status |
| [Universal Modder AC family notes](https://universal-modder.dev/games/assassin-s-creed/) | AC family A/C | Field-note aggregation, do not blindly trust addresses |
| [Frida](https://frida.re/) | General A | Hook instrumentation, not a game-native semantic API |
| [Ghidra](https://github.com/NationalSecurityAgency/ghidra) | General A | Static analysis, not gameplay controller |
| [x64dbg](https://x64dbg.com/) | General A | Native debugger, not game-integrated |
| [ImHex](https://github.com/WerWolv/ImHex) | General A | File/binary format inspection |
| [pefile](https://github.com/erocarrera/pefile) | General A | PE file parsing / executable fingerprint |
| [Capstone](https://github.com/capstone-engine/capstone) | General A | Disassembly, version-aware byte checking |
| [vgamepad](https://github.com/yannbouteiller/vgamepad) | General A | Python virtual gamepad API, Windows backend EOL |
| [ViGEmBus end-of-life](https://docs.nefarius.at/projects/ViGEm/End-of-Life/) | General A | Driver archived, not a maintained default |
| [libvirtualhid announcement](https://app.lizardbyte.dev/2026-08-16-introducing-libvirtualhid-and-virtual-hid-driver/) | General A | Maintained alternative, separate licensed Windows driver |
| [coop-anything fast iteration](https://github.com/cranebeaker/coop-anything/blob/main/playbook/fast-iteration.md) | General A | Main-thread RPC/event-driven loop, needs game-specific hooks |
| [coop-anything automated QA](https://github.com/cranebeaker/coop-anything/blob/main/playbook/automated-qa.md) | General A | Honest game-side oracle, mutation tests |
| [coop-anything hot reload](https://github.com/cranebeaker/coop-anything/blob/main/playbook/hot-reloading-your-mod.md) | General A | Valid only after verified callback/teardown, not out-of-box |
| [REA previously suggested repository](https://github.com/riccardoponte/Reverse-Engineering-REA) | General U | Direct URL returned 404 in 2026-11-17 research; not recommended |

## Resynced gameplay / combat / environment author projects

| Source / link | Scope / grade | Research lead |
| --- | --- | --- |
| [Classic Movement — Walk By Default Smooth Sprint #408](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/408) | BFR B | Author-described walk-first/RT-or-L3 sprint tuning; not AC4 high-/low-profile semantics or a tested 1.0.6 fix |
| [Walk By Default for PlayStation Controllers #514](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/514) | BFR B | Author-described PlayStation walk-first controller remapping; unknown native precedence and 1.0.6 support |
| [Easy Parry #617](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617) | BFR B | Parry windows, dual counters; resource unknown |
| [Unrestricted Combat Finishers #635](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635) | BFR B | `Edward_Default.data` / MicroKill/SoftKill, 1.0.8 |
| [Assassination Impact #587](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/587) | BFR B | Enemy cadence, double takedowns and camera shake |
| [The Padre Captain Style #602](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/602) | BFR B | NPC combat archetype change |
| [Heroes of the Jackdaw #569](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/569) | BFR B | NPC boarding participation |
| [City Under Siege #618](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/618) | BFR B | Guard spawn/population density |
| [Double Assassination Fix #597](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/597) | BFR B | Contextual takedown change, mechanism unknown |
| [Unarmed Combat #336](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/336) | BFR B | Alternative weapon/action state |
| [Lethal Combat #451](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/451) | BFR B | Damage model / microdamage event caveat |
| [Black Flag Overhauled #659](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/659) | BFR B | 1.0.8-only modular movement/water animation/UI |
| [GoatedACBlackFlag #19](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/19) | BFR B | Attack/execution/assassination animation speed runtime cfg |
| [ACBlackFlagFix #7](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/7) | BFR B | Camera/aim/control-adjacent hooks; coexistence hazard |
| [QuickSaveF5 #536](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/536) | BFR B | Native save routine, not replay/AI restore |
| [ACBFRTools #661](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/661) | BFR B | Trainer F4, timescale and free camera; no verified scripting RPC |
| [Encrypted Ultimate Rewrite #64](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/64) | BFR B | Cheat Engine manual exploration |
| [Otis_Inf BFR camera page](https://opm.fransbouma.com/Cameras/acbfresynced.htm) | BFR A/B | Pause/frameskip/time scale/camera, not AI reset |
| [Resynced Cheats Trainer #541](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/541) | BFR B | Author support claims 1.0.6–1.0.8, not a trusted API |
| [Resynced trainer #616](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/616) | BFR B | Author support 1.0.6–1.0.7, potentially incompatible hooks |
| [BFR inventory editor #631](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/631) | BFR B | Alpha/AOB failures reported, not combat tool |

## Original AC4 gameplay, rendering and mod management author projects

| Source / link | Scope / grade | Research lead |
| --- | --- | --- |
| [More Frequent Double Counters #1029](https://www.nexusmods.com/assassinscreedivblackflag/mods/1029) | AC4 B | Enemy cadence/double counter data |
| [Combat Changes #765](https://www.nexusmods.com/assassinscreedivblackflag/mods/765) | AC4 B | Enemy archetype/defenses |
| [AC4 Combat Overhaul #302](https://www.nexusmods.com/assassinscreedivblackflag/mods/302) | AC4 B | Damage/difficulty parameters |
| [Yet another combat mod #494](https://www.nexusmods.com/assassinscreedivblackflag/mods/494) | AC4 B | Parries/killstreak changes |
| [AC3 Improved Combat System #138](https://www.nexusmods.com/assassinscreedivblackflag/mods/138) | AC4 B | Alternate combat balance |
| [No Counter Slow Motion #871](https://www.nexusmods.com/assassinscreedivblackflag/mods/871) | AC4 B | Named `Game Bootstrap Settings.data` timing file |
| [Adewale's Counters for Edward #1039](https://www.nexusmods.com/assassinscreedivblackflag/mods/1039) | AC4 B | Single/double finisher animation replacement |
| [Combat/Sprint Camera Tweaks #695](https://www.nexusmods.com/assassinscreedivblackflag/mods/695) | AC4 B | Camera animations via data |
| [AC4Tools Nexus #901](https://www.nexusmods.com/assassinscreedivblackflag/mods/901) | AC4 B | GUI time/camera/noclip, source on GitHub |
| [AC4 modular PhysX/high-FPS patch #1031](https://www.nexusmods.com/assassinscreedivblackflag/mods/1031) | AC4 B | Very specific original Steam exe, alpha, DLL collisions |
| [AC4 Vortex extension #971](https://www.nexusmods.com/site/mods/971) | AC4 B | Resorep/full-forge installation automation |
| [BFR Nexus Vortex extension #1977](https://www.nexusmods.com/site/mods/1977) | BFR A/B | Official Nexus general mod manager |
| [BFR AC Mods Launcher source README](https://github.com/nickythesmithy/ac-mods-launcher) | BFR A | 1.0.8-only proprietary launcher `.acmod`, not all mods |
| [BFR AC Mods Launcher #658](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/658) | BFR B | Author package management claims |

## Modding communities, discovery and updates

- [Nexus original AC4 index](https://www.nexusmods.com/games/assassinscreedivblackflag): current author descriptions, files and posts. **May include restricted, old, conflicting mods.**
- [Nexus BFR index](https://www.nexusmods.com/games/assassinscreedblackflagresynced): current author descriptions, version-specific releases, posts.
- [AnvilToolkit resource wiki](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki): maintained first-party author resources.
- [BFR modding community discussion](https://www.reddit.com/r/ACBFResynced/comments/1vh8xti/we_created_a_black_flag_resynced_discord_for/): author community and troubleshooting, **not formal API documentation**.
- [ModDB original AC4 downloads](https://www.moddb.com/games/assassins-creed-iv-black-flag/downloads): historical FPS and widescreen mods, verify current binary safety.
- [Nexus original AC4 Vortex extension](https://www.nexusmods.com/site/mods/971) and [Nexus BFR Vortex extension](https://www.nexusmods.com/site/mods/1977) are **different adapters**, neither is a binary compatibility proof.

## Corrections and source-trust pitfalls to preserve

1. `AC4Coop` in **Kamzik123/AC4Coop** works on original **x86 AC4** and spawns an **AI-disabled** puppet. `zzyrrup/BlackFlagCoop` targets **BFR** but is still smoke-test/placeholder networking. They are not interchangeable.
2. `AnvilToolkit` supports **both games** but its BFR **v50 additive patch graph** must not be treated like original AC4 `DataPC.forge` or Rogue **v27**.
3. `HapticsBridge` 21 state hooks are **movement**, its synthetic test covers 1.0.6/1.0.7; effects/rumble don't prove pre-parry.
4. `AC Mods Launcher` only documents BFR **1.0.8** author-compatible `.acmod` packages; separate `ReForger` `.rfgb` and general Vortex are different ecosystems.
5. The previously suggested `riccardoponte/Reverse-Engineering-REA` repo could not be fetched (404); it is **not** a verified available dependency.
6. A public GitHub README or mod page **does not grant rights** to reuse author code/assets. The upstream license must be checked before incorporating anything; this repository is proprietary.
7. “Working” is only justified with **game-semantic evidence under a named tested build**; a green ASI CI build or screenshot of changed animation is not proof of native action dispatch.

## Source addition template

```markdown
| [Title](https://original-author-url) | AC4/BFR/Related; A/B/C/U | One precise fact; stated version; what it does NOT prove |
```

When adding a source, include date, author/publisher, primary URL, version range or “unknown”, license/permissions and status in the relevant game/technical page. Prefer direct first-party source over 10 SEO copies. Invalid/disappeared links become **U**, never silently deleted.
