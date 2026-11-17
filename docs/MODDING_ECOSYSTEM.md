# Black Flag Resynced modding ecosystem — source-led survey

**Researched:** 2026-11-17. **Focus:** capabilities, engineering workflows, legal/compatibility constraints, and the most promising leads for restoring classic AC4 controls/combat in **Resynced 1.0.6**. **Evidence level:** public documentation, original author descriptions and inspected public source; **no new in-game tests, executable disassembly or files from the installed game** in this pass.

Read this alongside [integration findings](INTEGRATION_FINDINGS.md), [combat conditions](COMBAT_CONDITIONS.md), [runtime test harness research](RUNTIME_TEST_HARNESS_RESEARCH.md), [Puppet Native ABI](PUPPET_NATIVE.md) and [scope](SCOPE.md). This is a **research index**, not instructions to install all listed tools.

## Executive findings

1. **Data modding is more mature than previously assumed.** [AnvilToolkit 1.3.7+](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki/Creating-a-New-Patch-Forge-for-Assassin's-Creed-Shadows-or-later) has a **Resynced-specific** wizard for generating correctly linked, separate patch FORGEs, without overwriting the huge original forge. This is a concrete way to prototype resource changes with restore.
2. **Multiple independent Resynced gameplay mods claim changes to real combat logic**, not merely visuals: enemy attack frequency, double takedown chance, parry forgiveness, normal-attack finishing rules and an NPC's combat archetype. These are **proof-of-feasibility clues by author report**, not verified schemas/API or permission to reuse a mod's files.
3. **A published Resynced ASI changes attack/execution/assassination animation speed live** by re-reading a config file. This is a plausible runtime hook research lead for pacing, **not** the input router or parry dispatcher.
4. **A new 1.0.8-only launcher** switches on/off modular animations, movement and text UI modifications without replacing base archives; its packages and claimed support are **not established for 1.0.6**.
5. **Programmatic NPC combat and repeatable arena control remain unsolved publicly**. An original-AC4 project spawns a puppet but disables AI while replaying its animation. Resynced crowd-density/companion mods prove gameplay *resource* seams by author description, not a public enemy-spawn or force-attack runtime API.

**Confidence key:** **A** = directly read public source or maintained author manual; **B** = published author's mod features/compatibility claims without independent validation; **C** = forum or inferred lead. A is confidence in the *public source's contents*, not demonstration on the owner's 1.0.6 build.

## I. Anvil resource pipeline and patch architecture (A)

**Canonical guide:** [AnvilToolkit author's guide: create a new patch forge](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki/Creating-a-New-Patch-Forge-for-Assassin's-Creed-Shadows-or-later) (edited 2026-10-12); [texture and prefetch guide](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki/Texture-Modding-in-Custom-Forges); [tool download and changelog](https://www.nexusmods.com/site/mods/455); [resource/tutorials repository](https://github.com/Kamzik123/AnvilToolkit-Resources).

### What the maintained toolkit now supports

- **AnvilToolkit 1.3.7+**: Game Explorer → right-click → **New → Patch Forge** with support explicitly documented for **Shadows and Black Flag Resynced**. Choose the actual highest existing patch in the target chain as its base. The toolkit generates next patch number and necessary metadata. An empty generated patch is intended to load without altering data.
- BFR uses a layered boot chain such as `DataPC_boot.forge` → `DataPC_boot_patch_01.forge` → `DataPC_boot_patch_02.forge` → *new* `DataPC_boot_patch_03.forge`. **Example numbers are not a manifest of the owner's 1.0.6 installation.** Inspect its actual head first; on an older build the patch chain can differ.
- The new head must carry forward the **prefetch graph** through `PrefetchInfoOverrides` and a parent reference to the previous head. The authored manual documents how an apparently valid blank patch **crashes at boot** if this is missed. **Do not simply rename `patch_02.forge` to `patch_03.forge`**.
- Key forge metadata: `PrefetchingManifest`, `DeletedFilesManifest`, `PrefetchInfoOverrides`, `PrefetchingFileInfos` and `GlobalMetaFile`. The 1.3.7+ wizard manages their shell; do not hand-copy/rebuild the graph by assumption.
- For inserted non-texture `.data` such as `Material`, `TextureSet` or `Mesh`, the **corresponding prefetch entry** may need transferring to the *new forge's* `PrefetchingFileInfos`, or dependent resources can fail to load/crash. `PrefetchInfoOverrides` describes the lower-chain parent and must not be casually replaced when adding individual data assets.
- For **BFR TextureMap** edits, largest mip levels may reside in external `.RawAsset` entries. AnvilToolkit's **Remove CompiledMip dependencies** tool (after loading those RawAssets) converts the texture into a self-contained `.data` file for a thin patch. A TextureMap-only `.data` does **not** need a `PrefetchingFileInfos` entry according to the guide.
- **Take resource data from the highest patch currently overriding that ID**, not blindly from the oldest base. A mod that edits an obsolete lower version can silently undo game fixes.

### Reproducible *research* workflow, not yet executed

1. Identify **actual 1.0.6** executable hash, installed forge list/head, 64-bit runtime and other game-mod files from a lawful local game copy; never store game file bytes in GitHub.
2. Make independent backups **outside** the game directory. Use the toolkit's **new patch** wizard rather than replacing a shipped patch. Verify an empty patch can be packed/round-tripped using synthetic data in CI where feasible; actual boot validation will eventually require one authorized game session.
3. Select **one named combat resource** or a small chain, inspect its types/IDs and dependencies, record prefetch provenance. Avoid mass-repacking huge base archives.
4. Record an original value and a **single independently authored change** with a reversible patch recipe; a different animation is not necessarily changed *action eligibility*.
5. Add preflight (build hash, parent forge metadata, original resource hash and expected type); reject mismatches and give restore procedure. Do not ship original or modified proprietary game assets.

**Why this matters:** It replaces the earlier assumption that FORGE edits inherently require dangerous overwriting. Packaging a reversible additive patch is **documented**; the actual **combat-resource schema, load order on 1.0.6 and successful patched gameplay** still require proof.

### Other resource tools (A/B)

| Tool | What is substantiated | Limits / value |
| --- | --- | --- |
| [AnvilToolkit](https://www.nexusmods.com/site/mods/455) | Active BFR support; archive and typed data editing, filename decrypt updates, new patch forge wizard; resource tutorial wiki | **Primary** resource research instrument; still no public game AI scripting SDK |
| [Forge Injector v1 Beta](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/108) | Author states FORGE **v50** resource injection and decoded resource import | Experimental; tool explicitly does not guarantee internal resource validity; not a high-level AI/animation editor |
| [ReForger](https://github.com/QuilLeeR/ReForger) | Source README describes a **Shadows-specific Anvil Pipeline v42** WinUI/.NET application and `.rfgb` packages (author's proprietary license) | [EK Replacer for BFR](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/636) separately claims ReForger **1.0.21+** works for its BFR mod. **Evidence conflicts:** do not assume general **BFR v50** support from the public README; verify release/build capability and actual target before using |
| [Jackdaw Flag Patcher (MIT source)](https://github.com/emike09/Jackdaw-Flag-Patcher) | Readable, independently authored *single-resource* BFR texture patcher: verify original resource, append replacement bytes, verify readback SHA-256, redirect index row, restore saved original index entry | Excellent **atomic-change / restore** architecture reference, but touches a base FORGE in place; do not make that the default when thin patches work |
| [Black Flag Resynced Modding Workshop](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/197) / [author community description](https://www.reddit.com/r/ACBFResynced/comments/1w7isjr/black_flag_resynced_modding_workshop/) | Author reports 52 character choices and hundreds of visual targets, backup/restore, some save edits | Asset/appearance/save research; **not** combat logic editor, programmable arena or NPC AI SDK |

## II. Gameplay-changing mods: functional evidence register (B)

Each row is a **published author claim and research lead**, not an endorsement, tested in-game result, legal permission to edit third-party payloads or a guarantee of target 1.0.6 support. In particular, many published `DataPC_boot_patch_02.forge` mods overwrite the same name and cannot be stacked by copying.

| Mod / source | Claimed effect and implied subsystem | Version / conflicts / research value |
| --- | --- | --- |
| [Easy Parry #617](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617) | Enlarges parry window, makes heavy/light attacks easier to parry; author also describes more accessible double counters | **Very high** for defense-window *data* seam; exact 1.0.6 compatibility and affected resource/type unknown. Prohibit copying author's files or values |
| [Unrestricted Combat Finishers #635](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635) | Alters normal-lethal-attack finishing **MicroKill/SoftKill** animation selection via `Edward_Default.data` (see [existing independent investigation](INTEGRATION_FINDINGS.md)) | **Very high** as evidence of animation selection, **NOT** evidence of pre-input Takedown eligibility or native dispatch; author claims 1.0.8; merge instructions target `patch_02` |
| [Assassination Impact / More Double Takedowns #587](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/587) | Author explicitly changes **enemy attack succession**, double-takedown occurrence and combat/assassination **camera-shake timing** | **Very high** for enemy attack frequency and double-counter conditions; v1.2 claims compatibility with **1.0.8**; provides separate **DATA version** and warns **not to rename patch_02 to patch_03** |
| [The Padre — Captain Style #602](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/602) | Changes an ally's combat **archetype** from his prior behavior to a **Captain-like fighting style**, with weapons, while retaining protected-companion logic | **High** for NPC `combat style / role` **resource-based substitution**. Separate DATA-only payload. Does **not** establish arbitrary spawn/attack scripting; compatibility older/future versions unverified |
| [Heroes of the Jackdaw #569](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/569) | Author enables companions Adewale/Lucy to participate in naval boarding battles | **Medium-high** for team/encounter participation rules, **not** native player-controlled AI |
| [City Under Siege #618](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/618) | More guards and higher share of elite/special patrol units in cities and ports | **Medium-high** for **encounter population** data; **not** a custom deterministic arena. More guards may contaminate combat timing trials |
| [Lethal Combat / One-Strike Lethality #451](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/451?tab=posts) | Reported one-hit trigger; author notes guard/counter/push/kick sometimes produce **micro-damage events** and cause unwanted kills | **High diagnostic caveat**: damage events alone are not proof of an actual sword strike. This is an author interpretation of user reports, not an independently reverse-engineered event schema |
| [Unarmed Combat #336](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/336) | Changes availability of unarmed fighting; community reports some hit/AI incompatibilities | **Medium** action/weapon-state clue, unverified on 1.0.6. Distinguish animation from damage model |
| [Black Flag Overhauled #659](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/659) | Modular **original AC4 water animations**, Unity-style sprint, Observe-and-Run movement, UI prompts in 13 languages | **High for animation replacement and UI context**, through author's `.acmod` workflow; verified **1.0.8 only**; user 1.0.6 unsupported by evidence |
| [ACBlackFlagFix #7](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/7) | Runtime camera/aim and quality-of-life fixes, including configurable aim-transition duration | Already in owner's reported setup; review **feature collisions** before attaching more camera/input hooks |
| [Double Assassination Fix #597](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/597) | Author advertises a separate double-assassination correction | **Potentially high** for contextual action semantics; precise mechanism and 1.0.6 compatibility **unverified**; needs source-level investigation before depending on it |

### What NOT to conclude from these mods

- `Easy Parry` is **not proof** that accurate windows are exposed as editable numbers in a documented schema. It only justifies focused inspection of the corresponding game-authored files.
- `Unrestricted Combat Finishers` is **not the desired X/finisher arbitration fix**: choosing a kill animation *after a lethal normal strike* is not the same as invoking native Takedown when the player is eligible *before* pressing X.
- `City Under Siege` and companion mods demonstrate different **population/role settings**, not a supported `spawn_enemy(type, location)` function.
- `Padre` changing styles suggests identifiable **NPC combat archetype dependencies** in data, but we do not know the exact resource names, schema or how to invoke a specific attack.
- Published archive replacement files must not be **blindly merged**. Overlapping same IDs, metadata, forge head, game versions and author restrictions matter. One mod author explicitly provides a separate `.data` distribution because naive merge fails: [Padre feedback](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/602?tab=posts).

## III. Resynced runtime extensions and hot configuration (A/B)

| Lead | Concrete technical fact | Opportunity / missing link |
| --- | --- | --- |
| [GoatedACBlackFlag #19](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/19) / [author replies](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/19?tab=posts) | Version 0.0.20 adds separate **attack speed, execution speed, assassination speed** controls; v0.0.25 improves state detection/animation speed, v0.0.30 improved camera-state logic. Runtime reads auto-generated `<game>/SDK_DIR/GoatedACBlackFlag.conf` when saved. Author says engine-triggered camera/FPS hooks and a plugin folder. | **High** evidence for hooking **animation pacing** and state-specific camera logic on Resynced, worth inspecting docs/settings. No public source or native combat-command API verified; side effects at high multipliers reported; engine speed change ≠ new timing/eligibility rules. Beware its own `version.dll` proxy |
| [Puppet Native](https://github.com/venvyl/puppet-native) | Published MIT **PN_Api v1**, plugin DLL lifecycle, module base and logging; the project's own isolated Puppet Native DLL passes synthetic tests | **High** as an extension host, **not** a game scripting API. Existing [Puppet Native ABI](PUPPET_NATIVE.md) documents layout and absence of action dispatcher |
| [ACBFHapticsBridge](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge) | Source profiles for 1.0.6/1.0.7/1.0.8 and 21 movement-state hook targets; includes haptic reactions to parry/attack/dodge | **High** instrumentation *research*, but combat reactions can be inferred from output/input, not necessarily native pre-action eligibility. Source uses a **non-commercial license**; do not copy hook signatures/source into this proprietary repo |
| [ACBFRTools #661](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/661) | New F4 menu: character/ship settings, weather, game speed, noclip/free cam, manual save editing. Author describes **fail-disabled gameplay features on unrecognized executable** | **High** for manual exploratory scenario setup. Published 2026-11-17; exact 1.0.6 support, scriptable RPC and compatibility with user's loader **unknown**. Archive includes x64 loader according to author, so **do not overwrite existing proxy DLL** |
| [Otis_Inf photomode](https://opm.fransbouma.com/Cameras/acbfresynced.htm) | Timestop, frameskip, game-speed and camera control | **High** for visual timing observation, **not** save/world-state reset or verified programmable frame-step SDK. Additional injection/hooks can alter the measured state |
| [QuickSaveF5 #536](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/536) | Author invokes a native save routine via ASI | Potential **fixture-saving** hook, but save ≠ restore exact enemy AI/RNG state; use controlled backup |
| [Encrypted Resynced Ultimate Rewrite #64](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/64) | Cheat Engine table with teleport, item edits, game-time and diagnostic functions | **Manual** exploration only; no public general NPC AI command API confirmed; memory modifying tables can affect the game baseline |
| [Cheats Trainer #541](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/541), [Trainer #616](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/616), [Inventory Editor #631](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/631) | Authors claim support including **1.0.6** on selected versions; editor #631 is explicitly an alpha with missing AOB matches reported by players | Candidates for isolated **coordinate/memory research only**. Executable builds/channels matter, and trainers are not scriptable engine APIs. Do not distribute, execute with elevated privileges casually or turn off antivirus on trust |

### Current loader danger map

- One game directory can contain proxy candidates **`version.dll`, `dinput8.dll`, `winmm.dll`**; a proxy DLL is **not** interchangeable with a plugin. Nested ASI loaders and plugin folders differ.
- **Puppet Native**, **ACBlackFlagFix**, **GoatedACBlackFlag**, **ACBFRTools**, standalone ASI and **AC Mods Launcher** may each assume ownership of a loader or hooks. Their coexistence on the owner's exact build is **not established**.
- An ASI loader's ability to discover a file does **not** demonstrate game hook success. Keep a **module/EXE fingerprint table**, explicit native fallback, and separate build/loader baselines. No replacement of `version.dll` on the owner's installation without provenance/rollback.
- An observed startup failure with Puppet Native and an alternative proxy is an integration blocker, not proof that Puppet Native itself is unusable (see [existing investigation](PUPPET_NATIVE.md)).

## IV. Mod managers and content packaging (A/B)

**[AC Mods Launcher](https://github.com/nickythesmithy/ac-mods-launcher)** — a new portable manager whose **README was inspected** on 2026-11-17:

- Supports **only PC Resynced 1.0.8** in the current release. Uses proprietary **`.acmod`**, ZIP/folder imports; author explicitly says **only their own mods** are compatible. Do not mistake it for a universal Resynced mod loader.
- Allows modular enable/disable, previews, and detects conflicting **same-object replacements** within its supported package ecosystem. Its `Black Flag Overhauled` bundle is direct evidence that whole animations, movement preferences and multilingual UI messages can be changed, **according to author and screenshots**.
- Leaves original `.forge` archives untouched but **installs/updates loader files**, with an explicit warning about other ASI loaders and older forge mods. Its **uninstall removes the entire mods folder**, including modules not added through the launcher; do not run it against an existing owner's mod environment without backup.
- GitHub README **publishes documentation but does not license the launcher source**: repository offers releases/metadata; author states source code is not published/licensed. No adoption of internals, package format or redistribution rights without source and license assessment.

**AnvilToolkit patch forge vs AC Mods Launcher** solve different problems. First is typed **data/asset authoring**; second is toggling the author's **packaged runtime mods**. The modern manager **does not supersede** additive patch-forge workflow for original project work, especially on 1.0.6.

**[ReForger](https://github.com/QuilLeeR/ReForger)** uses its own **`.rfgb`** mod data format; it is **not** the same as `.acmod`. Its README specifies **Shadows Pipeline v42**, while a BFR mod author advertises BFR use on 1.0.21+. Confirm the actual supported game/forge versions before choosing it for BFR 1.0.6.

## V. Original AC4 studies and why direct ports are hazardous (A)

- [Original AC4Coop source](https://github.com/Kamzik123/AC4Coop): ASI **x86** creates a puppet `Human` and synchronizes global transform, action alias/channel/layer, a 46-slot blend array and animation phase; main mode **disables puppet AI** and suppresses background spawns. The project documents the original `ActionComponent::SetTargetAction` and its main-thread update seam, but its **x86 addresses are NOT Resynced x64 APIs**.
- [AC4Tools (original AC4)](https://github.com/45hgv43whgf/AC4Tools): build-specific SHA-256, expected-byte and unique-pattern checks and memory-patch rollback offer a defensible hook pattern. **Not** a tool to run in Resynced.
- [Official Ubisoft developer explanation](https://www.rpgsite.net/interview/20396-assassins-creed-black-flag-resynced-interview-ubisoft-modern-day-future): combat was *rebooted* to become deterministic and many systems were rebuilt. This supports treating old AC4 behavior as a **design reference**, not interpreting shared asset names as evidence of binary/class compatibility.
- [Universal Modder AC4 notes](https://universal-modder.dev/games/assassin-s-creed-iv-black-flag/): candidate workflows, version separated; do not import patterns unverified.
- [AnvilToolkit's own resource wiki](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki): useful for modern BFR forge workflow. Different levels of engine metadata/resource format between old AC4 and Resynced must remain explicit.

**Substantive gap:** No independently confirmed public **Resynced** function to enumerate AI entities, spawn opponents on demand, force a named attack, reset AI decision memory, or expose pre-X combat eligibility. Generic trainers, actor replacement, animation playback or camera tools cannot substitute for these.

## VI. Game-side experiment design and evidence (A/B/C)

**Relevant official gameplay rules:** [Ubisoft combat explanation](https://www.ubisoft.com/en-gb/game/assassins-creed/news/1A8XkbnvONciecEZm9my1C/assassins-creed-black-flag-resynced-combat-system-breakdown) describes normal/perfect parries and dodges, **unparryable attacks**, and enemies adapting to repeated moves. Attack type, enemy archetype, player gear and repeated responses are confounders. An automatic keypress alone cannot measure parry effectiveness.

**Useful direct experiments once a legal, isolated 1.0.6 target is available:**

| Question | Best evidence source | What would change our implementation decision |
| --- | --- | --- |
| Which BFR `.data` controls **parry window / defense arbitration**? | BFR game-authored data identified by AnvilToolkit; compare named values and author-reported Easy Parry feature **without copying mod data** | A readable, version-specific value with safe reversible independent patch: `DATA_GO`; otherwise precise decoder/schema blocker |
| Which BFR resource controls **enemy action cadence / double counters**? | Target-resolved original game data + event/camera source from [mod #587](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/587) description | Named stable candidate for testing, not a guessed window |
| Is an NPC **style/archetype** a configurable asset? | Actor/entity builder references, original BFR entity data, author report from [Padre #602](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/602) | A verified resource relationship on 1.0.6; distinct from native live AI command |
| Can Resynced change **attack animation speed** without changing global timescale? | [Goated #19](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/19) public description/changelog; game-native state observer | A decoupled animation/pacing knob. Does **not** solve counter semantics or parry eligibility |
| Does the game expose **pre-input finisher eligibility and native action dispatch**? | 1.0.6 dynamic target analysis, public engine semantic hooks, self-authored read-only probe | Critical blocker for contextual classic X. Wwise/parry rumble and kill animation alone are insufficient |
| Can we create a **repeatable encounter**? | Existing encounter + isolated save + semantic event readback; optionally author-described guard-population edits | One measured, distinguishable attack/outcome; no claim of deterministic AI without attack-start signal |
| Will new package coexist with installed mods? | Exact exe/module hashes, active patch head, plugin host inventory and independent clean-profile launch | A reversible, build-gated delivery matrix. Do not combine multiple `version.dll` files |

**Test fidelity rules:** baseline at normal speed without changes to health, AI visibility, enemy damage, defense or attack cadence; identify any existing loader/ASI hooks; trial timestamps must reference **engine-semantic** outcomes and include uncertainty; make synthetic test oracles fail on deliberate negative controls. See [runtime test research](RUNTIME_TEST_HARNESS_RESEARCH.md).

## VII. Security, redistribution and current support

- **Nexus author permissions vary**, and some authors explicitly forbid modification, redistribution and AI-based modification of their files. Read each tool's permissions separately. Do not commit third-party `.data`, `.forge`, game code, offsets copied from restricted work or compiled ASI/loader files.
- **Public source ≠ open source**. E.g. [AC Mods Launcher](https://github.com/nickythesmithy/ac-mods-launcher) README and [ReForger](https://github.com/QuilLeeR/ReForger) are not broad permission grants. [HapticsBridge](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge) has a custom **non-commercial** license. Its ideas may be studied; do not incorporate its files without compatible permission.
- **No automatic execution of random trainer EXEs or kernel drivers**, no AV exclusions, no installer from a clone or an SEO-disguised game-repack repository. Favor official original author URLs, source inspection, local-only testing and checksum provenance.
- Some **patch authors test only 1.0.8**, while owner's game is reportedly **1.0.6**. A mod's UI version is not an executable hash. Updates may change patterns, patched resources, prefetch graph and behavior. Keep a per-version ledger; no blind 1.0.8→1.0.6 copy.
- A README can be stale relative to a release (notably **ReForger**); mark contradictions explicitly and verify the real binary/version rather than choosing whichever claim is more convenient.

## VIII. Ranked research opportunities for the classic-combat project

1. **P0 — BFR `Edward_Default` and the game's combat/actor resource graph.** Investigate the exact 1.0.6 original resource identities/classes implicated by [Easy Parry #617](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617), [Unrestricted Finishers #635](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635), [Assassination Impact #587](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/587) and [Padre #602](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/602). Output: a **small target-file/class/behavior map** with evidence, unknowns and independent patch viability; avoid full-archive research treadmill.
2. **P0 — additive patch feasibility.** Use **AnvilToolkit 1.3.7+** resource/metadata docs to design a single reversible, version-gated patch **recipe** based on verified 1.0.6 data. New patch, not overwriting `patch_02`; explicit prefetch dependency graph, rollback and no redistribution of game assets.
3. **P0 — runtime behavior/pacing seams.** Research the *kind* of state and animation transitions supported by **GoatedACBlackFlag #19**, the narrower observer capabilities of HapticsBridge, and the plugin ABI already implemented. Prioritize true **pre-action eligibility and native command** over more XInput logs.
4. **P1 — actor style / encounter control.** Determine whether 1.0.6 NPC archetype references and population settings are resource-driven, as inferred from `Padre`/companions/city mod author claims. Separate **static behavior configuration** from **runtime NPC command**, and design a controlled existing-encounter fixture first.
5. **P1 — animation and UI authoring.** Examine the public features of 1.0.8-only `Black Flag Overhauled` and its launcher for **modular animation substitution** and prompts; avoid adopting their proprietary packaging or running a 1.0.8-only tool on 1.0.6.
6. **P2 — cosmetics, save editing and shader mods.** Useful ecosystem knowledge but lower priority until combat/controls can be changed. Examples: Modding Workshop, Jackdaw Flag Patcher, ReForger, Resynced Inventory Editor. A save editor or renderer hook does not enable AI or combat actions.

### Research outputs to record (without adding game files)

For each actionable new lead, update [research register](RESEARCH.md) / linked issue with: **author + URL + observation date; exact stated target build; game/forge resource path, name and class when evidenced; claimed functionality; test method; proof tier A/B/C; risk/license; blockers; next smallest independent verification**. Never convert author claims into unqualified “working on 1.0.6”.

**Current bottom line:** There is substantial documented **data-level control** of Resynced gameplay, including a modern additive FORGE mechanism. It is a more promising immediate lane than building a generic NPC bot. But no primary source found here proves that **classic AC4 action arbitration**, **parry windows** or **programmable arena NPC attacks** can be restored without further BFR 1.0.6 resource/runtime research.
