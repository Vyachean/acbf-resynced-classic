# Resynced X vertical slice: local data findings and scoped gate proposal

**Scope:** issue #30 and the user's licensed offline Resynced copy, reported as TU 1.0.6. The executable at the reported `C:\Games` location is fingerprinted and confirmed as the same file loaded by Ghidra. The local save-storage manifest identifies its profile as `TU4 (1.0.6)`; the executable itself has no file/product version metadata. This supports the user's local profile identification but does not authenticate the exact executable/resource set as an official clean 1.0.6 build. Gameplay behavior remains unvalidated. TU 1.0.8 is comparison context only. This record does not claim a playable fix.

The exact executable SHA-256 (`19920f34bb2fac814023ee3c27f0ceca1872e80a2aafb37c092c419fff77cc0d`) is independently listed by [ACBFHapticsBridge](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge) as an additional TU 1.0.6-compatible profile. This corroborates compatibility-profile identity, not publisher provenance, a clean install, or behavior of this mod.

## Current target update — 2026-11-16

Ubisoft released TU 1.0.8 on October 27, 2026, and describes it as the final major content update. It reintroduced original-game hidden-blade, unarmed, single-sword, and gunblade takedown animations; it also changed Nightmare Mode's parry timing and fixed additional fighting/assassination issues. These later changes do **not** apply to the stated 1.0.6 target. They do not establish that original AC4 input selection or counter sequencing was restored. [Official 1.0.8 notes](https://www.ubisoft.com/en-us/game/assassins-creed/news/5Z1RxwznM6CcrkSEVoibmh/ac-black-flag-resynced-title-update-108-patch-notes).

Public community distribution now also documents an x64 Ultimate ASI Loader path for Resynced, including a November 13 mod using it. This upgrades ASI loading from a merely plausible route to a demonstrated community route. It does not prove compatibility with this project's hooks, Puppet Native coexistence, or the locally installed executable. See [recent Nexus example](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/648?tab=description) and [official TU 1.0.8 notes](https://www.ubisoft.com/en-us/game/assassins-creed/news/5Z1RxwznM6CcrkSEVoibmh/ac-black-flag-resynced-title-update-108-patch-notes).

### Public Resynced GFL map is not a TU 1.0.6 reference

The public `File Lists/BlackFlagResynced.gfl` entry currently has a length of 10,558,651 bytes. Its upstream [October 27, 2026 commit](https://github.com/Kamzik123/AnvilToolkit-Resources/commit/51470584) explicitly says the Black Flag Resynced lists were updated to 1.08. The local `.local-open-maps/BlackFlagResynced.gfl` is a OneDrive reparse placeholder with the same length; byte identity was not checked, and it must not be treated as a 1.0.6 map. The prior [September 9 map commit](https://github.com/Kamzik123/AnvilToolkit-Resources/commit/4168ba55) stored a 10,529,156-byte file, but its update version is not identified in the commit message. Neither public snapshot authenticates the local 1.0.6 resource mapping. Keep the 1.07/1.08 FGFL candidate IDs out of any TU 1.0.6 patch until an exact matching map is found.

### Control-name constructor references

Follow-up decompilation of the direct `Pad_Takedown`, `Pad_ChainAction`, `Pad_NonLethalHostageTakedown`, `Pad_BAM`, and `Key_BAM` references found small routines that construct a shared `0x68`-byte descriptor and associate a control-name string. The shared constructor stores its integer argument at descriptor offset `+0x40`; neither that integer nor the descriptor's vtable methods have been assigned input-button semantics. The strings are passed into a generic intern/registry helper, not a verified player-action dispatcher. Most wrapper functions have no direct callers in Ghidra; the one `Pad_Takedown` wrapper with a reported caller leads to obfuscated code whose decompilation is not trustworthy. Therefore these references explain how control-name metadata may be represented, but they do **not** establish runtime registration, active bindings, a contextual eligibility check, or a safe interception point. This is a narrowed static lead only; no game behavior or mod feature was implemented from it.

#### Follow-up: bounded caller query on the registry helper

The generic registry helper `FUN_1410ecb20` had 59 direct callers in an earlier bounded analysis. Its body compares strings in a global registry and performs reference-counted descriptor replacement; the broad call count alone does not identify a gameplay consumer. Six callers were identified as small name-bearing wrapper constructors, including `Pad_ThrowFood` and `Pad_DisengageStrafe`. An earlier Ghidra decompile reported `FUN_14df6fc10` as the caller of the wrapper carrying `Pad_Takedown`, but that edge was not reproducible in a fresh query against the saved database. The fresh query instead found a DATA reference to the wrapper from `0x15da7f154`; its 12 bytes decode as the wrapper's PE `RUNTIME_FUNCTION` exception record, not an executable registration/call site. Treat the earlier caller edge as an analysis artifact unless independently reproduced. The other named wrappers had no direct callers in the earlier analysis. The control-name wrappers remain non-actionable; this evidence does not reveal a logical command consumer.

The descriptor's observed vtable method at `0x1409a3f60` copies a 32-bit value from a serialization cursor to object offset `+0x40`; the method at `0x14895a6c0` copies that field into a newly allocated object except for one mode value. Ghidra reports no direct callers for either routine, consistent with indirect dispatch through the descriptor vtable. This strengthens the interpretation that `+0x40` is serialized/copyable descriptor metadata, while leaving its enum/schema meaning unknown. It supplies no basis for using values such as `2` or `4` as controller button IDs.

### Follow-up: `Pad_Takedown` caller edge is not a usable call-site

The earlier Ghidra analysis reported `FUN_14df6fc10` as a caller for the `Pad_Takedown` descriptor constructor. That function's decompilation had no callees and its caller context was empty; raw disassembly showed flattened arithmetic and computed control flow across discontiguous blocks. A fresh query did not reproduce this call edge. Treat it as an unverified analysis artifact, not a registration path. The constructor itself associates the name with two values produced from constants 10 and 12, but neither value has verified input or action semantics.

A bounded raw-file reference check was then run against the pinned executable for the six control-name wrapper function RVAs. It scanned each PE section not marked executable for 4-byte-aligned, signed relative references using both slot-relative and next-slot-relative encodings; it found no matches. Absolute VA/RVA scans found only the corresponding function-start values in the already-identified unwind-function table, not an ordinary registration pointer table. Combined with the earlier Ghidra call counts and the unreproduced caller edge, this cannot prove the wrappers are dead: runtime-generated or protected registration is possible. It does mean there is no ordinary static callback/registration table among the inspected data sections to follow from these wrappers. The control-name lead remains non-actionable for a gameplay patch.

## Runtime access attempt — 2026-11-16

An earlier local test launch stopped at an **Online Service Error** (`ANIMUS-3-00029`) before reaching the title/gameplay context; the process then exited. No protection was bypassed, no ASI was installed, and no game file or save was changed. Consequently this attempt observed no Attack/Takedown transitions and did not validate a gameplay hook. The user confirms this is a licensed offline copy and identifies it as TU 1.0.6. Its executable is pinned by the SHA-256 above; embedded version fields are empty, so the TU label remains user-reported pending an independent build comparison.

## Current result

| Route | Evidence from the available installation | Status |
| --- | --- | --- |
| Combat data | Reproduced the local v50 Forge `Edward_Default` parse to 338 objects using the Toolkit's embedded Resynced schema. It contains Attack/Finisher, Parry, Takedown Preparation, and Basic Takedown Initiator entries. The local archive resolves from the public 1.0.8 GFL search map, which is not authenticated for TU 1.0.6. No field has verified semantics for safely changing classic X behavior and the parry handles remain unmapped to condition fields. | `DATA_PARTIAL` |
| Contextual X | The decoded regular-attack finisher has post-attack transitions to Microkill/Softkill initiator states. These are animation/action-state transitions after an attack, not evidence that X can select a pre-action Takedown command. The candidate input-definition and combat-repository IDs from the available 1.07/1.08 FGFL maps do not occur in any of the 186 local Forge indexes inspected. | `COMMAND_BLOCKED` |
| Native action call | Static analysis of the local executable finds low-level SDL controller event handling, but no verified player-action dispatcher, eligibility predicate, or acknowledgement. | `NATIVE_BLOCKED` |

No game-facing control change is implemented. The local executable has no usable version strings in its file-version fields, and the installation contains additional loader/ASI components. Treat all decoded values as evidence about this particular fingerprinted local file set only; do not transfer them to another 1.0.6 install without comparison.

## Prior local data experiment and reproduction status

An earlier read-only investigation selected the `Edward_Default` payload from `DataPC_boot_patch_01.forge` by its Forge index entry, parsed that payload with `AnvilToolkit.FileTypes.AnvilNext.Containers.DataFile`, and decoded selected object payloads with the toolkit's embedded `ACBlackFlagResynced` schema. That run reported no full archive extraction, game launch, or game-file write. A later attempt to reproduce it through `ForgeFile.OpenStream` returned `EntriesCount=0`; do not use that high-level route until its side effects are fully controlled. The portable AnvilToolkit 1.3.7 package now enables a repeatable metadata-only index read. With an isolated copy of the public 1.0.8 GFL search map and an empty in-memory hash-name cache, `ReadEntriesBFR` resolves `Edward_Default` in the user's local patch Forge at ID `2207302156764`, offset `504313690`, length `354893` (extension hash `225137419`). This is a successful local name-to-index match, but the map is still not authenticated for TU 1.0.6. A single candidate file was read via Toolkit into ignored `build/tooling/candidate-read/`; no archive payload was placed in the Git tree or game folder. The index contains 5,596 unique IDs with zero out-of-range spans. In a fresh headless process, using the Toolkit's normal `DataFile(byte[], Game)` constructor after setting the Resynced game profile and Oodle path reproduced the parse to 338 objects; no decoded object files were emitted outside the in-memory result.

Observed facts:

- Forge header version: `50`; the selected Forge index contains 5,596 entries. A metadata-only pass at the header's stored index offset parsed all entries, populated in-file data offsets and lengths (`0` invalid ranges), and ended at byte `1,915,433,496`. The public GFL map can resolve the `Edward_Default` candidate in this local archive, but its post-1.0.6 provenance means this is a search hint, not proof that the map is authoritative for the target build.
- The reproduced `Edward_Default` parse yielded 338 objects. Its `Edward - Regular Attack 01` and `Edward - Regular Attack Finisher` resources decode as `FightLogicActionStateAttack`.
- The finisher's `ConditionalTransitionGroups` contain targets named `Edward - Microkill - Initiator` and `Edward - Softkill - Initiator`; its base attack data has `IgnoreBlock=False` and `IgnoreParry=False`.
- `Edward - Basic Takedown Initiator` and its denied-chain variant are separate `FightLogicActionStatePaired` objects in the same payload. Their existence alone does not establish which input selects them or when they are eligible.
- The `Edward - Parry` state has eight `StateFightWindows`. Its `AtomMarkupID` values decode to `Queue_AttackInput` (510), `Queue_DefensiveInput` (511), `Allowed_PerfectParry` (505), `Allowed_Evade` (504), `Allowed_AttackAction` (502), and `CustomMarkup_02` (311). Two other windows are `FightLogicTimeInSecondsProvider` intervals of 0–300 ms and 0–130 ms.
- Exact object-ID matching now resolves the relevant `FightLogicIsInWindowCondition.Window` handles to entries in `StateFightWindows`: ID `2391827913086` is entry `[6]`, whose default provider is 0–130 ms and whose unarmed and standalone-sword overrides are 0–200 ms; ID `2553264178665` is entry `[7]`, the `CustomMarkup_02` window. The entry `[6]` condition uses `InsideWindow` and occurs on an AI operation with `MemoryBlackboardActionData`, as well as a hit-reaction check. The entry `[7]` condition uses `BeforeEndOfWindow`; its operation activates `ActionActivateProjectileParryData` when the IconicSwords perk condition is met. Thus the 130/200 ms window is not the projectile-parry trigger. Its gameplay meaning and the blackboard key names remain unresolved, so these values still cannot safely be treated as melee-counter durations or changed as a behavioral patch.
- The installed original-AC4 `BlackFlag.gfl` is the binary GFL format, not the human-readable FGFL accepted by AnvilToolkit's `FGFLToTxt`. A targeted search of its local Forge archives found no plaintext state names. Thus a direct original-vs-Resynced window comparison was not established from available files.
- The 1.07/1.08 FGFL maps available to the toolkit contain names for combat-repository and input-definition candidates, but those candidate IDs did not match any top-level ID in the 186 local Forge indexes checked. The maps are not authenticated as belonging to this installation; their names cannot bridge that mismatch.
- A separate read-only scan of the base `DataPC_boot.forge` found a structurally continuous v50 index (126,247 records) and 33 DataFile entries (2,426 contained objects). Searching those object tables for input definitions, combat repositories, window categories, control checks, and attack-type repositories produced no named matches. The three 6,307-byte records initially considered as possible repositories have unrelated extension IDs and do not decode as DataFiles. The shared base and patch archives contain no DataFile entries. This narrows the inspected resource set but does not prove the repository is absent from other archives or from a matching title-update file set.

These results show that Resynced combat state data is genuinely data-driven and locally decodable. They do **not** yet show a safe, independently tunable parry/finisher parameter, nor identify the complete data dependency containing X-to-command selection. The tuple returned by Toolkit's unpack routine reports `False` while producing the parsed 338-object data set; treat object parsing and archive/build identity as separate checks.

## What blocks the first safe patch

### Fast-lane decision (2026-11-17)

- **`CANDIDATE_BUILT / EFFECT_UNVERIFIED` for the parry window.** The local selected resource is parsed and its window handle is linked. A guarded 130→200 ms candidate is built, round-tripped, and installed additively; the game reached offline play after restart. The exact player-counter semantics and live effect remain unproven, so this is not yet a gameplay-validated improvement.
- **`COMMAND_BLOCKED`.** Owner evidence establishes a visible Y prompt and native Y Takedown for the default profile, but static inspection has not found a programmatic eligibility signal, the logical Attack/Takedown command path, or action submission. The prompt could guide a screen-level experiment, but it is not itself a verified runtime-readable input to a mod. Do not intercept raw X or synthesize a replacement event from the SDL evidence.
- **Target behavior for the first X slice:** keep X as ordinary Attack when no Takedown prompt is active; when the ready-Takedown prompt is visibly active in on-foot combat, invoke the game's existing Y-bound Takedown once from X. Ambiguous prompt, context, or input ownership must pass through unchanged. This is a design target derived from the owner's observation, not an implementation claim.
- **Follow-up B slice:** in combat, leave B's parry/evasion route unchanged; outside combat, use B for a specifically identified noncombat Y interaction. Do not bind Y indiscriminately to either button, and keep menus, ship, water, stealth and traversal in native pass-through pending their own audit.
- **Feature-scoped #18 gate:** one independently validated action/context pair may proceed at a time, off by default, after its prompt, state, input ownership, affected collisions and rollback are evidenced. X/Takedown and B/world-interaction remain separate features; combat B stays native defense, and unknown contexts pass through. The full audit is still needed for a global classic profile, not for unrelated narrow slices.
- **Next bounded step:** establish the candidate's live parry effect in a controlled melee encounter. The user suggested a true empty 3D arena with configurable enemies/items/stats; the existing `acbf_combat_arena` is only the offline CLI policy simulator, and no verified in-game spawn API or arena resource path is available. Do not use guessed runtime addresses or mutate world/save files. A contextual-X screen cue still needs distinct combat-Takedown, noncombat-interaction, and no-prompt samples plus a proven way to suppress the source input and submit native Y once. The earlier online-services error is irrelevant to offline play per the user; the game has since reached its offline profile.

Two concrete evidence gaps remain for a patch against supported TU 1.0.6:

1. **Version/profile:** the user identifies this licensed offline profile as TU 1.0.6. The exact executable is pinned by SHA-256 and is the same file previously analyzed; its embedded version fields are empty, and the game directory contains third-party components. Therefore compatibility can be scoped to this pinned local profile, but not generalized to every TU 1.0.6 distribution without a clean comparison. This is not a licensing or ownership objection.
2. **Patch semantics:** the decoded `Edward - Parry` state maps `StateFightWindows[6]` (handle `2391827913086`, `InsideWindow`) to a 130 ms default and 200 ms Unarmed/standalone-Sword02 overrides. Its exact consumer semantics remain unresolved; the override proves 200 ms is already an accepted value for this same window, but does not prove the default window controls the player's melee counter timing.

## Reversible parry-window experiment

A narrow experiment changes only the default `EndTimeProvider.TimeMS` of `Edward - Parry` / window handle `2391827913086` from 130 to 200. The source value is guarded, the two conditional overrides are checked to remain at 200, and Toolkit round-tripping confirms that exactly one internal resource ID (`2214145886775`) changes among the 338 resources in `Edward_Default.data`.

Toolkit generated a normal next-patch Forge from the local base archive, preserving its five manifest records and adding the changed `Edward_Default` container. The staged `DataPC_boot_patch_02.forge` passed a v50 index read (six unique IDs, no invalid ranges); extracting it again and reparsing the resource recovered the changed 200 ms default and both unchanged overrides. The identical candidate was copied additively to the game folder; the original patch archive and executable hashes did not change. This is a data-format and round-trip check, not proof that the game applies the new archive or that the parameter improves combat.

The game was restarted after the candidate was installed and reached the offline Cat Island save on the user's 1.0.6 profile without crashing. This verifies launch compatibility only; patch application and combat effect remain unproven. The live keyboard menu confirms `Q` is Parry/Deflect and `B` is Auto Movement; earlier B-key trials were invalid. An attempt to return to the shore patrol did not reach a controlled fight because the starting route was obstructed by tree/fence geometry. No successful parry, counter, or finisher has been observed. Remove only `DataPC_boot_patch_02.forge` if the startup becomes unstable or a controlled comparison shows no useful effect. Do not alter `IgnoreParry`, animation markups, or action transitions based on names alone. A contextual-X patch still needs a pre-action eligibility/command-selection seam.

## Command and native-route evidence

Static Ghidra analysis of the locally available executable found an SDL `PeepEvents` pump that dispatches controller axis/button/device events into generic controller handlers. This establishes a real low-level controller-event path in this binary, not the path from a player's X press to Attack/Takedown. The imported `GameInputCreate`, raw-input and SDL APIs similarly establish library use only; no supported hook is authorized from this evidence.

The static call path was followed one step beyond the pump: the controller update calls the SDL pump, translates button down/up events into per-controller float slots using a lookup table, and queues generic controller/device records. The queue has multiple unrelated producers and dispatches through generic listeners; the inspected path still does not identify Attack/Takedown logical commands, combat eligibility, or a player-action recipient. This is a verified negative result for the examined path, not proof that no separate gameplay mapping path exists elsewhere in the binary.

**Owner-reported observation (2026-11-17):** in the current Resynced profile, Takedown is performed with Y and a Y-button prompt appears when the opponent is ready. This gives the player a visible pre-action cue. The earlier shared-button trial still has no recorded prompt state after remapping, so it does not establish whether eligibility was present during that X press. No programmatic HUD cue reader or native command-selection seam has been verified.

**Owner-provided control requirement (2026-11-17):** Y also triggers noncombat world interactions such as opening chests. Keep combat Takedown on Y; move the separate noncombat world-interaction role to B where the game exposes it as an independent action. Keep B as Resynced parry/evasion during combat. For X, only a distinctly identified combat-Takedown prompt may route X to native Y; outside combat, B may route to a specifically identified world interaction. A generic Y glyph is insufficient: nearby interaction prompts must not turn X into a chest interaction, and combat must never turn B into a Y interaction. Unknown contexts pass through. This is a user requirement, not a verified runtime implementation.

### Contextual routing contract

| Physical input | Verified condition at press | Native action to request |
| --- | --- | --- |
| X | On-foot combat and a distinctly identified Takedown-ready prompt | Y-bound Takedown |
| X | On-foot combat with no Takedown-ready prompt | X Attack |
| X | Any ambiguous or unverified context | Original X input, unchanged |
| B | Combat context | Original B parry/evasion input with its native press/hold/release behavior |
| B | Explicit noncombat world-interaction prompt bound to Y | Y-bound interaction |
| B | Any other or ambiguous context | Original B input, unchanged |
| Y | Combat Takedown, tools, and other known non-world roles | Preserve native action |
| Y | Noncombat world-interaction role | Move to B only if that role can be separated without suppressing other Y actions; otherwise retain native Y and make B an explicit alias |

The route chosen on a press must own the matching release, even if the HUD prompt or context changes while the button is held. A button cycle may produce at most one native action. The generic Y icon alone cannot distinguish a Takedown from a chest interaction; prompt type and combat state must both be identified before substituting X or B.

### Bounded UI-resource lookup

A metadata-only search of the local `DataPC_boot_patch_01.forge` index (5,596 unique entries, no invalid ranges) used the staged public 1.0.8 GFL strictly as a name-search hint. `Prompt` had no named hit. `Takedown` found a dual-sword camera animation and a chain-takedown tutorial string, not a combat prompt widget. `HUD` returned general scene/widget resources, but none whose mapped names mention combat, fight, action, Takedown, kill, enemy, button, input, prompt, or interaction; `Contextual` returned objective HUD scenes only. This bounded search does not rule out unnamed/hashed resources or establish TU 1.0.6 names. It found no UI resource suitable for a prompt-based patch.

### Follow-up: SDL controller event route

On the pinned executable, `SDL_PeepEvents` has one direct caller, `FUN_146f33960` at absolute address `0x146f33960` (RVA `0x06F33960`). Its caller is `FUN_1408816c0` (absolute address `0x1408816c0`). The pump reads SDL event types `0x650` through `0x654` and passes them to four handlers:

- `FUN_146f33ab0` handles controller axis events, matches the SDL device instance ID against the open-controller list and writes normalized axis floats into controller state.
- `FUN_146f33b60` handles controller button down/up events and writes 0/1 into a field selected through a button-index mapping table.
- `FUN_1429970f0` opens a controller on the device-added event; `FUN_146f33a40` closes it on device removal.
- The pump and its handlers contain no Attack/Takedown identifiers or player-action call. A separate `FUN_1408816c0` update later queues generic objects to listeners; that generic queue is not proven to be a player-action dispatcher.

The surrounding `FUN_1408811a0` update loop calls `FUN_1408816c0`, then walks generic registered objects and invokes virtual update callbacks. `FUN_1408827f0` is a dynamically growing multi-producer notification queue that fans queued objects to generic listener callbacks. Neither decompile showed a combat action chooser or a `FightLogicActionState` command submission. The 21 movement callbacks are a separate state-entry route and cannot supply the missing X eligibility signal by themselves.

This narrows the physical-input route to SDL's per-controller state layer, but does not reveal how the logical Attack and Takedown bindings read that state, whether the combat actor is eligible, or where the chosen action is submitted. The observed route remains a low-level input backend and is not a safe interception seam. Do not map SDL event codes directly to gameplay actions.

### Follow-up: per-controller state to frame-level button edges

The SDL-created controller object has a vtable at `0x149360db8`. Its frame-update entry at vtable offset `+0xb8` points to `FUN_146f33730` (`0x146f33730`) in the pinned executable. The generic controller update loop in `FUN_1408816c0` invokes registered objects through this slot after draining SDL events.

`FUN_146f33730` reads the controller-state buffer attached at object offset `0xbb0`. It copies 16 float-valued controls into the controller object's input fields, derives pressed booleans using the threshold `0.11764706`, and calls `FUN_142996ac0` to compare current and previous levels and timestamp rising edges with `rdtsc`. It also calls `FUN_142996de0`, which derives eight directional states from the two stick axes using a `0.3` threshold. These are verified translation and edge/timing operations. The currently recovered names and offsets do not identify the physical button labels or the gameplay actions that consume the resulting fields.

The follow-up decompilation of those two helpers makes the layout more precise. The sixteen current pressed flags are bytes at controller offsets `+0x48`–`+0x57`; their prior-frame values are at `+0x38`–`+0x47`. A rising edge updates its corresponding 64-bit timestamp at `+0x68`, `+0x70`, …, `+0xe0` from `rdtsc`. The separate eight-way stick-direction flags occupy current bytes `+0xb58`–`+0xb5f`, previous bytes `+0xb60`–`+0xb67`, and edge timestamps `+0xb68`–`+0xba0`. Raw controller float values are copied from the attached state buffer beginning at `+0x18`; directional derivation reads stick axes at controller offsets `+0x130/+0x134` and `+0x140/+0x144`. These offsets are scoped to the pinned executable. They establish storage and edge mechanics only; mapping-table indices and object fields still do not reveal their consumer semantics.

**Second producer path found:** `FUN_140882020` reaches the same `FUN_142996ac0` edge/timestamp helper as the SDL controller update. It reads a packed input sample through `FUN_149035300`, fills axis/button fields in the same controller-object layout, then advances the same edge state. The stub's underlying API is unresolved, so this is recorded as a second controller sample/update route, not labeled as a particular device API. This confirms that following SDL alone cannot represent every observed source feeding this controller layout. Neither update routine submits a gameplay action.

The installed `GameInput.dll` import table contains ordinal 1; the local Windows `GameInput.dll` exports ordinal 1 as `GameInputCreate`. This confirms the executable imports the GameInput factory entry, but does not resolve `FUN_149035300`: its six-byte thunk jumps through a nonstandard slot under the executable's large `.edata` mapping, not directly through the ordinary or SDL delay-IAT tables. Do not equate the shared controller sample routine with a specific GameInput read method on this evidence.

### Embedded Resynced control vocabulary

A bounded binary-string scan of the same pinned executable found paired metadata names `Key_Takedown` / `Pad_Takedown` and `Key_NonLethalHostageTakedown` / `Pad_NonLethalHostageTakedown`, plus `Pad_ChainAction`, `Pad_ParkourHighProfile`, and the type name `MeleeCombatInputMode`. It also contains combat event/type names such as `AttackStarted`, `AttackCompleted`, `AttackIndex`, `ParryWindow`, and `DodgeWindow`. This confirms the executable carries separate logical control and combat vocabulary that can guide the next resource/schema lookup. The strings alone do not prove that `Pad_ChainAction` means the player's ordinary attack, reveal binding values, show how `Pad_Takedown` is arbitrated, or give a safe hook address.

A new read-only byte scan located `Key_Takedown` / `Pad_Takedown` at file offsets `0x0a13c978` / `0x0a13c985`; the following ASCII strings are `nx_pad_down`, `xbx_pad_down`, `ggp_pad_down`, `ps5_pad_down`, `ps4_pad_down`, `xb1_pad_down`, and `kb_down`. Nearby vocabulary also includes `Key_ParkourHighProfileUp` / `Pad_ParkourHighProfileUp` and their `Down` variants. The image contains `Key_BAM` / `Pad_BAM` but no literal `Pad_Attack`, `Key_Attack`, `Pad_Parry`, or `Pad_Assassinate` in the bounded scan. These observations do not prove that `_down` strings are binding values or that `BAM` is ordinary Attack.

PE section-table parsing places those strings in the read-only, non-executable `.edata` section (RVA `0x09036000`, size `0x0127c000`). The ordinary export data directory is at RVA `0x1c2cd040`, outside that section; the control names are not normal named PE exports. Correctly mapped RIP-relative references from `.code`, verified with Ghidra, show `Pad_Takedown` used by `FUN_158a45340` and `FUN_1593028e0`; corresponding generated descriptor functions reference `Pad_NonLethalHostageTakedown`, `Pad_ChainAction`, `Pad_BAM` / `Key_BAM`, and ParkourDown/HighProfile variants.

For example, `FUN_1593028e0` builds a small typed value through `FUN_14145cc10(..., 2, ...)` and passes it with the `Pad_Takedown` name to `FUN_1410ecb00`. That helper stores the value and passes the name to `FUN_1410ecb20`, which searches a global string registry using a string-comparison helper. This is a code path that can register a name if invoked, but Ghidra reports no direct callers for `FUN_1593028e0`; execution of this helper is not established. Other small functions referencing `Pad_NonLethalHostageTakedown`, `Pad_ChainAction`, and `Pad_BAM` also have no direct callers in the current analysis. The integer argument `2` is not established as a button code.

The nearby 12-byte triples previously mistaken for a descriptor registry are entries in the PE Exception Directory: the `Pad_Takedown` function entry at file offset `0x1c502c14` and a separately rechecked data reference at VA `0x15da7f154` match the `RUNTIME_FUNCTION` begin/end/unwind-RVA layout and point to unwind data in `.link`. This is ordinary x64 exception metadata and does not associate functions with controls. No runtime consumer of the string registry has been established.

`FUN_1408811a0` is the enclosing frame loop. After `FUN_1408816c0` it calls a virtual method at `DAT_14c05f920 + 0x40`, then updates other generic subsystems. The global is zero-initialized in the executable image, so its static bytes do not reveal the runtime object or its vtable. No call/reference path yet ties the controller's 16 edge fields to logical Attack/Takedown commands. Treat the new boundary as **SDL → per-controller levels → frame-level press edges**, not as a game-action dispatcher.

### Follow-up: frame-loop input-manager initialization

A bounded scan for RIP-relative references to the frame-loop global found candidate sites; the key sites were checked against Ghidra's function analysis. `FUN_143df3ae0` is a lazy initializer: it allocates a `0x308`-byte object, assigns vtable `0x14935e640`, and stores the pointer in `DAT_14c05f920`. Its only recorded caller is `FUN_141df9a30`, which initializes the object during startup and invokes its virtual methods at `+0xc0` and `+0xf8`.

The frame-loop method at vtable `+0x40` is `FUN_140781030`. Its body calls `FUN_1407811e0`, `FUN_140781120`, and `FUN_140781090`, then handles a manager flag and one additional subsystem callback. The reviewed body contains no controller-edge read, combat eligibility check, or Attack/Takedown submission. Other game functions call methods at vtable `+0x98`, `+0xa0`, and `+0xa8`; these are plausible manager query methods, but their semantics remain unknown. Attempts to decompile that group timed out while Ghidra's full analysis remained active. Do not promote the manager or its query methods to an action seam without that missing data flow.

**Correction from follow-up disassembly:** the frame-loop object's role is still unclassified. Its vtable methods at `+0x98`, `+0xa0`, and `+0xa8` are not controller/action queries: `FUN_141bbcbc0` checks `[this+0x2fc] == 0`, `FUN_143df3c20` checks the same field against `3`, and `FUN_143df3c10` checks it against `1`, each returning a boolean. Treat these as lifecycle/state predicates unless further evidence says otherwise. The earlier description of the object as an “input-related manager” and the proposal to trace these methods as controller consumers are superseded.

### Follow-up: controller vtable and delayed SDL import

The SDL controller object's vtable at `0x149360db8` provides more methods around the already verified frame update at `+0xb8`:

- `+0xa0` (`0x146f32310`) stores the incoming low byte at object offset `0x240`.
- `+0xa8` (`0x142755110`) copies 16 bytes from the second argument into object offset `0x180`; nearby entries also access vector-sized fields around `0x160`–`0x180`.
- `+0xc8` (`FUN_146f333a0`) reports whether the first qword in the attached controller-state object at `+0xbb0` is nonzero.
- `+0xd8` (`FUN_146f33910`) calls thunk `0x149034f90`, then maps results `0`–`7` through a lookup table. Parsing the executable's PE **delay-import** directory (ordinary imports do not contain this entry) maps IAT slot `0x14ab861d0` to `SDL_GameControllerGetType`. Therefore this is controller classification, not a logical gameplay action resolver.
- `+0xe8` returns a field from the attached controller state or a fallback pointer; `+0xf0` begins with a small integer-range predicate. Their broader semantics remain unclassified.

The imports are delayed from `SDL2.dll`; the delay-import descriptor starts at RVA `0x1d06d608`, with the SDL IAT at VA `0x14ab861b8`. The exact slot mapping above was independently checked against `dumpbin /IMPORTS` and a local read-only PE parse. These controller-object methods still do not identify which generated edge field is Attack/Takedown or who submits the selected action.

The verified research boundary remains **SDL device events → per-controller levels → controller-level frame edges**. The frame-loop singleton is a separate, unclassified lifecycle/update object. Neither path currently supplies the required pre-action eligibility and native action-submission proof, so neither is a safe gameplay hook point.

## Hook diagnostic smoke — 2026-11-16

The user confirmed that their normal launch is the `ACBlackFlag.exe` in the target folder. Before testing, the installed probe-only ASI, executable and two save/profile files were hash-checked against the preserved local backup. The corrected diagnostic ASI was temporarily installed and started through the same direct executable route. Its game-directory log recorded the matching executable hash and `movement_state_hooks=21; mode=read_only`, proving that the loader loaded this ASI and that its checked hook installer completed on this launch. The game process then showed an `Error` window before gameplay; no state events or gameplay actions were observed.

The exact probe-only ASI was restored and hash-verified. A subsequent launch with that probe-only ASI also showed an `Error` window, so the current launch failure does not isolate the 21 hooks as its cause. The visible error text could not be captured, and the saved `VERSION.log` only confirms loader activity and ASI load. Both launch attempts are inconclusive for mod compatibility; no more game sessions are needed for issue #10's static research gate. Save/profile files still match their pre-test hashes.

## Data-mod references (technique only)

- [Easy Parry](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617?tab=description) describes an extended parry window and more permissive double-kill conditions. Its public page does not establish TU 1.0.6 compatibility or provide a redistributable patch recipe. Respect its author's restrictions.
- [Unrestricted Combat Finishers](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635?tab=description) says Microkill/Softkill animations may trigger when a normal attack kills an enemy and explicitly supports TU 1.0.8. This aligns with the locally observed post-hit finisher transitions, but is not evidence of pre-attack X command selection. Do not copy its code, assets, or modified resources.
- [AnvilToolkit 1.3.7](https://www.nexusmods.com/site/mods/455?tab=description) lists Resynced Forge/Data support. This inspection independently exercised that parser on selected local data; toolkit support does not establish the source install's version.

## Feature-scoped replacement for issue #18's global implementation gate

Keep the broad control rewrite gated. Permit a narrowly scoped feature only when **that intervention** independently demonstrates all of the following:

1. Exact supported executable/build identity is checked at runtime; mismatch disables the feature.
2. The native interception boundary and ownership of the affected logical input are verified. A raw physical-input poll is insufficient.
3. Every decision signal required before the native action is chosen is verified for that exact context. For X combat, this includes trustworthy combat context and Takedown-versus-Attack eligibility.
4. The feature can suppress only the owned input cycle, including its matching hold/release, and cannot send a second action on repeat or fallback.
5. A verified native command is submitted at most once; rejected or ambiguous submission never triggers a speculative retry.
6. All states outside the exact supported context pass through unchanged. Unknown context, target, mode, version, or signal fails closed.
7. The feature is off by default, has a project-owned disable/rollback path, changes no game executable, and has unit/CI checks plus a real-game acceptance case before release.
8. The audit covers every other action sharing the intercepted event or suppression boundary. Unrelated surfaces need not be exhaustively reverse-engineered while they remain native pass-through.

This scoped gate replaces the former global implementation freeze. It does not authorize an unverified hook: each feature still needs the safety evidence listed above. It changes the evidence burden from “complete every control surface before any feature” to “prove the safety envelope for each feature and its collision blast radius.”

## Single-session diagnostic, only after static preparation

If static work cannot identify the missing seam, use one ordinary, unscripted owner gameplay session on the fingerprinted local profile. Before that session, instrumentation must have two exact-build-verified read-only points: the pre-action eligibility/command selection and the resulting player action-state start. Record only those logical events, their timestamps, the controller press/release cycle, and the naturally visible HUD prompt state. Do not change bindings, stage a repeated Attack/Takedown test, or collect another generic XInput trace. If the two semantic observation points cannot be established statically, the session is not ready to run and must not be replaced by weaker telemetry.

## Next technical steps

1. Restart the game with the staged additive parry experiment and record real combat behavior; revert the patch if it is ignored, inert, or unstable.
2. Independently continue the original-AC4 comparison only where it can identify the logical Attack/Takedown commands and a pre-action eligibility/action-submission seam. Avoid repeating broad string scans or low-level SDL tracing.
3. Implement contextual X only after that seam is verified, with unknown contexts passed through and a tested rollback.

### Comparative lead from the installed original AC4 executable

The already-open, fully analyzed original `AC4BFSP.exe` in Ghidra exposes a separate high-level input route that can serve as a comparison lead:

- `FUN_0161cdd0` resolves current and prior input records via `FUN_015f38f0`, `FUN_015f3d10`, `FUN_015f3c30`, and `FUN_015f3ad0`, then computes changed/held state and conditionally calls `FUN_0161a170` for an event kind.
- `FUN_015f38f0` produces candidate numeric codes `0x1f` and `0x20` under context checks. `FUN_015eab80` considers candidate records and proximity/context data; the current static evidence does not map these numeric codes to Attack or Takedown.
- `FUN_0161a170` selects event-handling branches that ultimately call `FUN_0160ba10`. That routine builds a generic queued event through shared engine helpers (`FUN_01515640`, `FUN_00a0bde0`, `FUN_00649fe0` → `FUN_004545a0`). The queue helpers have hundreds of unrelated callers, so this is not by itself a player-action sink.
- A parallel original path is `FUN_01AB5890` → `FUN_01AA36F0` → `FUN_0161CC60` → `FUN_0161A170`. `FUN_01AB5890` has a special branch when its incoming control code is `0x58` or `4`; `FUN_01AA36F0` handles candidate output codes `0x20`, `0xd3`, and `0x1f` and consults input-state lookup helpers. This establishes a context-sensitive control/event translation route in original AC4, but the numeric codes still have no verified Attack/Takedown labels and the generic event queue recipient is unknown.
- `FUN_01AF8390` initializes a behavior callback table keyed by numeric state IDs and registers `FUN_01AE1400` at `0x1e` and `FUN_01AE1490` at `0x24`; it also resolves the `BhvAssassin` component. This places the two input front-ends in an assassin behavior-state framework, but does not prove that this actor is the player in every invocation or provide names for the state IDs.
- This is **original-game-only evidence** and does not bridge the Resynced controller fields to the same semantic commands. The prior observer's 4,566 calls through `FUN_01AA3990` still all returned `-1`; this newly inspected route has not been shown to be that observer's missing player-action path.

Next comparison task: establish what object/recipient consumes the specific queued event generated by the original input path, then locate a structurally equivalent Resynced logical mapping/action consumer. Until that is proven, neither the shared generic event queue nor raw controller edges is an acceptable hook seam.

## Status boundary

- Implemented: reversible candidate changes the default duration on the local `Edward - Parry` window, and an additive `DataPC_boot_patch_02.forge` is installed. The offline resolver and mock-only action bridge remain disconnected from Resynced.
- Verified in staging: 338-resource `.data` round-trip with only the target resource changed; generated six-entry v50 patch parses, extracts and reparses to 200 ms, with five manifests preserved and no invalid index ranges.
- Verified statically/read-only: local Forge index parsing, selected `Edward_Default` object decoding, state/resource types and transitions described above, executable import inventory, and an SDL controller-event pump.
- Synthetic: existing resolver/action-bridge tests only; they do not establish real game behavior.
- Real game: restarted after installing the candidate and reached the offline 1.0.6 profile/Cat Island save without a crash. The keyboard menu verified `Q` as Parry/Deflect and `B` as Auto Movement. No successful combat action was observed; patch selection and gameplay effect are not established.
- Exact profile: the local save-storage manifest reports TU4 (1.0.6); the executable SHA-256 appears above. Combat resource findings apply to selected files in that local folder; an independent official executable/resource manifest is still unavailable.
- Separate original-AC4 diagnostic #28 / PR #29: untouched.

## 2026-11-16 — bounded SDL button remap follow-up

Read-only Ghidra inspection of `FUN_146f33b60` and the bytes at `DAT_14992f4e0` confirms that a controller-button event's byte at `event+0x0c` indexes a 16-entry `int32` remap table. The values are `0,3,1,2,8,65535,9,14,15,11,13,6,4,5,7,65535`; `0xffff` entries are ignored by the handler. A valid result selects a float slot at `controllerState+0x18+slot*4`, where button-down writes `1.0` and button-up writes `0.0`.

The frame update copies normalized input values into the controller object, thresholds them into level booleans, and timestamps rising edges. Independent update paths also populate the same generic field layout from other controller APIs. This strengthens the physical-button normalization evidence, but the slot names and their consumers remain unresolved. The remap table acts before gameplay context is known and cannot safely implement contextual Attack/Takedown selection on its own.

A broad batch decompile of several candidate vtable methods stalled during ongoing whole-program Ghidra analysis; it was stopped and not retried. No hook, build, install, launch, or game-file change was made from this lead. The next useful step is to resolve a consumer of the generated edge fields or recover matching TU 1.0.6 logical-input/action resources.

### External compatibility cross-check for the 21 traversal slots

The public source for ACBFHapticsBridge v2.3.7.9 includes the same 21 TU 1.0.6 slot/function pairs used by this project's read-only movement observer and uses a four-register x64 callback type while forwarding the original call. Its README lists the pinned executable SHA-256 as an additional TU 1.0.6-compatible profile. This is useful corroboration for the slot table and a reason to preserve all four register arguments, but it is not proof that the external build was run on this exact local executable or that these callbacks can safely drive input changes. The source is linked for independent study only; no third-party implementation code was copied into this repository.
