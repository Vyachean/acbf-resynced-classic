# Resynced X vertical slice: local data findings and scoped gate proposal

**Scope:** issue #30 and the target reported as Resynced TU 1.0.6. This record distinguishes the locally inspected installation from a clean, verified 1.0.6 release. It does not claim a playable fix.

## Current result

| Route | Evidence from the available installation | Status |
| --- | --- | --- |
| Combat data | AnvilToolkit 1.3.7 decoded the local version-50 Forge entry containing `Edward_Default` into 338 objects using its embedded Resynced schema. Real Attack, Attack Finisher, Parry Success, Final Parry Success, Basic Takedown, and assassination states are present. | `DATA_PARTIAL` |
| Contextual X | The decoded regular-attack finisher has post-attack transitions to Microkill/Softkill initiator states. These are animation/action-state transitions after an attack, not evidence that X can select a pre-action Takedown command. The candidate input-definition and combat-repository IDs from the available 1.07/1.08 FGFL maps do not occur in any of the 186 local Forge indexes inspected. | `COMMAND_BLOCKED_ON_AVAILABLE_EVIDENCE` |
| Native action call | Static analysis of the local executable finds low-level SDL controller event handling, but no verified player-action dispatcher, eligibility predicate, or acknowledgement. | `NATIVE_BLOCKED` |

No game-facing control change is implemented. The local executable has no usable version strings in its file-version fields, and the installation contains additional loader/ASI components and modified-looking patch resources. The package title and timestamps do not establish its provenance or exact TU. Treat all decoded values as evidence about this particular local file set only; do not transfer them to a clean 1.0.6 install.

## Reproducible local data experiment

The investigation was read-only. It selected the `Edward_Default` payload from `DataPC_boot_patch_01.forge` by its Forge index entry, parsed that payload with `AnvilToolkit.FileTypes.AnvilNext.Containers.DataFile`, and decoded selected object payloads with the toolkit's embedded `ACBlackFlagResynced` schema. No full archive extraction, game launch, or game-file write was performed.

Observed facts:

- Forge header version: `50`; the selected Forge index contained 5,596 entries.
- The `Edward_Default` payload parsed to 338 objects. Its `Edward - Regular Attack 01` and `Edward - Regular Attack Finisher` resources decode as `FightLogicActionStateAttack`.
- The finisher's `ConditionalTransitionGroups` contain targets named `Edward - Microkill - Initiator` and `Edward - Softkill - Initiator`; its base attack data has `IgnoreBlock=False` and `IgnoreParry=False`.
- `Edward - Basic Takedown Initiator` and its denied-chain variant are separate `FightLogicActionStatePaired` objects in the same payload. Their existence alone does not establish which input selects them or when they are eligible.
- The `Edward - Parry` state has eight `StateFightWindows`. Its `AtomMarkupID` values decode to `Queue_AttackInput` (510), `Queue_DefensiveInput` (511), `Allowed_PerfectParry` (505), `Allowed_Evade` (504), `Allowed_AttackAction` (502), and `CustomMarkup_02` (311). Two other windows are `FightLogicTimeInSecondsProvider` intervals of 0–300 ms and 0–130 ms.
- The same Parry state has `FightLogicIsInWindowCondition` transitions. One `BeforeEndOfWindow` operation executes `ActionActivateProjectileParryData`; another checks `InsideWindow` and executes an AI action. The decoded handles do not expose the exact window/category ID at each condition field, so the 130 ms interval cannot yet be assigned to the projectile action or treated as the melee-counter window. The numeric values are real, but changing one would be an unsupported behavioral guess.
- The installed original-AC4 `BlackFlag.gfl` is the binary GFL format, not the human-readable FGFL accepted by AnvilToolkit's `FGFLToTxt`. A targeted search of its local Forge archives found no plaintext state names. Thus a direct original-vs-Resynced window comparison was not established from available files.
- The 1.07/1.08 FGFL maps available to the toolkit contain names for combat-repository and input-definition candidates, but those candidate IDs did not match any top-level ID in the 186 local Forge indexes checked. The maps are not authenticated as belonging to this installation; their names cannot bridge that mismatch.

These results show that Resynced combat state data is genuinely data-driven and locally decodable. They do **not** yet show a safe, independently tunable parry/finisher parameter, nor identify the complete data dependency containing X-to-command selection. The tuple returned by Toolkit's unpack routine reports `False` while producing the parsed 338-object data set; treat object parsing and archive/build identity as separate checks.

## What blocks the first safe patch

Two concrete evidence gaps remain for a patch against supported TU 1.0.6:

1. **Version/provenance:** the accessible installation is not an authenticated clean TU 1.0.6 set. Its executable has no file/product version metadata, while its resource set and loader directory contain third-party modifications. A patch built against it could not honestly be advertised as 1.0.6-compatible.
2. **Patch semantics:** the decoded parry state contains real 300 ms and 130 ms intervals and resolved input/action markups, but the available schema decode does not map the window handles back to their exact condition fields. A targeted decode of all 23 version-50 DataFile entries in the local patch archive (1,923 contained objects) found no named input-definition/repository/window-category resources and did not contain the candidate repository ID from the unverified FGFL map. No candidate field has yet been proven to mean “make X choose Takedown before Attack.”

Do not change `IgnoreParry`, animation markups, or transition conditions based on their names alone. A meaningful independent parry patch needs the exact controlling resource, field interpretation, dependency resolution, and a reversible validation against the same supported build. A contextual-X patch additionally needs a pre-action eligibility/command-selection seam.

## Command and native-route evidence

Static Ghidra analysis of the locally available executable found an SDL `PeepEvents` pump that dispatches controller axis/button/device events into generic controller handlers. This establishes a real low-level controller-event path in this binary, not the path from a player's X press to Attack/Takedown. The imported `GameInputCreate`, raw-input and SDL APIs similarly establish library use only; no supported hook is authorized from this evidence.

The prior shared-button observation in project research remains bounded: Attack won in that reported attempt, while whether a Takedown prompt was visible is unknown. It does not prove that contextual Takedown eligibility existed at the moment of input.

## Data-mod references (technique only)

- [Easy Parry](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617?tab=description) describes an extended parry window and more permissive double-kill conditions. Its public page does not establish TU 1.0.6 compatibility or provide a redistributable patch recipe. Respect its author's restrictions.
- [Unrestricted Combat Finishers](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635?tab=description) says Microkill/Softkill animations may trigger when a normal attack kills an enemy and explicitly supports TU 1.0.8. This aligns with the locally observed post-hit finisher transitions, but is not evidence of pre-attack X command selection. Do not copy its code, assets, or modified resources.
- [AnvilToolkit 1.3.7](https://www.nexusmods.com/site/mods/455?tab=description) lists Resynced Forge/Data support. This inspection independently exercised that parser on selected local data; toolkit support does not establish the source install's version.

## Proposed replacement for issue #18's global implementation gate

Keep the broad control rewrite gated. Permit a narrowly scoped feature only when **that intervention** independently demonstrates all of the following:

1. Exact supported executable/build identity is checked at runtime; mismatch disables the feature.
2. The native interception boundary and ownership of the affected logical input are verified. A raw physical-input poll is insufficient.
3. Every decision signal required before the native action is chosen is verified for that exact context. For X combat, this includes trustworthy combat context and Takedown-versus-Attack eligibility.
4. The feature can suppress only the owned input cycle, including its matching hold/release, and cannot send a second action on repeat or fallback.
5. A verified native command is submitted at most once; rejected or ambiguous submission never triggers a speculative retry.
6. All states outside the exact supported context pass through unchanged. Unknown context, target, mode, version, or signal fails closed.
7. The feature is off by default, has a project-owned disable/rollback path, changes no game executable, and has unit/CI checks plus a real-game acceptance case before release.
8. The audit covers every other action sharing the intercepted event or suppression boundary. Unrelated surfaces need not be exhaustively reverse-engineered while they remain native pass-through.

This is a proposal for an explicit amendment to issue #18, not permission to bypass its current gate or install an unverified hook. It changes the evidence burden from “complete every control surface before any feature” to “prove the safety envelope for each feature and its collision blast radius.”

## Next technical steps

1. Keep the present combat-data path on the local installation read-only. Resolve a trusted version/provenance marker and a matching FGFL/repository set before patch work; do not assume its contents are clean 1.0.6.
2. Resolve the Parry state's 24 handle references to individual fields and the linked global window-category data. The current schema decode reveals actual time values and operation types but loses field-to-handle association; this is the remaining data-path question before a reversible change can be justified.
3. Continue from the SDL event pump through the input mapping/action layer. Accept a candidate only when the call/data flow ties a physical X event to a distinct logical command and pre-action eligibility.
4. If runtime evidence becomes necessary, instrument only the verified controller-event handler, logical mapping dispatch, and action eligibility/selection boundary. Record a compatibility fingerprint and do not activate a mutating hook until issue #18's per-intervention proposal is explicitly accepted.

## Status boundary

- Implemented: no gameplay mechanic patch. Existing offline resolver and mock-only action bridge remain disconnected from Resynced.
- Verified statically/read-only: local Forge index parsing, selected `Edward_Default` object decoding, state/resource types and transitions described above, executable import inventory, and an SDL controller-event pump.
- Synthetic: existing resolver/action-bridge tests only; they do not establish real game behavior.
- Real game: not tested in this increment. No gameplay behavior is claimed.
- Exact supported build: unresolved. The local installation cannot currently qualify as clean or verified TU 1.0.6.
- Separate original-AC4 diagnostic #28 / PR #29: untouched.

