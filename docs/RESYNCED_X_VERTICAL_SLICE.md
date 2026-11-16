# Resynced X vertical slice: decision and scoped gate proposal

**Scope:** issue #30; target reported as Resynced TU 1.0.6. This is a bounded feasibility decision, not a claim of a playable control fix.

## Decision

| Route | Finding | Decision for X on TU 1.0.6 |
| --- | --- | --- |
| Data rules | Public mods demonstrate editable combat data, but their documented effects do not establish pre-attack X selection or 1.0.6 compatibility. No authenticated 1.0.6 combat entry has been independently decoded and patched. | `DATA_BLOCKED` |
| Logical command routing | No verified game-facing binding/command seam distinguishes Attack from Takedown before X is consumed. Existing XInput/HID imports and the Puppet Native v1 plugin contract do not expose semantic combat commands or eligibility. | `COMMAND_BLOCKED` |
| Native action call | No exact-build function, player/target object, eligibility predicate, or action acknowledgement has been verified. | `NATIVE_BLOCKED` |

**No playable X prototype is safe to produce yet.** The offline resolver and transactional bridge remain useful policy/test code, but are not connected to Resynced. Do not turn the current resolver into an input hook: it has no trusted source for zone or action eligibility.

## Evidence and limits

- The existing project research records that a shared Attack/Takedown binding produced Attack only. The HUD prompt state for that attempt was not captured, so it proves a binding conflict in that test, not that Takedown eligibility was present.
- Static inspection records XInput, HID and raw-input imports in the local Resynced sample. Imports identify possible input libraries, not the logical command dispatcher or its priority rules.
- Puppet Native's published v1 API and this repository's synthetic plugin test establish a plugin lifecycle/configuration/logging seam. They do not establish combat state or action dispatch.
- The repository's local-resource audit found a readable Resynced v50 archive index, but did not map a named combat entry to authenticated TU 1.0.6 data or decode its action conditions. A local name-map file alone does not prove its build provenance or that a selected entry is `Edward_Default.data`.
- No proprietary game data, executables, third-party mod files, or local installation fingerprints are included here.

## Data-mod references (technique only)

- [Easy Parry](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617?tab=description) describes an extended parry window and more permissive double-kill conditions. Its public page does not establish TU 1.0.6 compatibility or expose a redistributable patch recipe. The author restricts modification and redistribution.
- [Unrestricted Combat Finishers](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635?tab=description) says its MicroKill/SoftKill animations can trigger when a normal attack kills an enemy, and explicitly supports TU 1.0.8. That is a post-lethal-animation rule, not evidence of pre-attack X choosing Takedown; its author restricts modification and redistribution.
- [AnvilToolkit 1.3.7](https://www.nexusmods.com/site/mods/455?tab=description) lists Resynced Forge/Data support and Resynced filename decryption. Tool support alone does not validate a 1.0.6 resource set or any combat schema. Any future recipe must operate on the user's own verified files, with backup and restore, and must be independently authored.

**Data path ranking:** the best next low-risk probe if verified 1.0.6 resources become available. It could plausibly adjust guard/parry windows or finisher-animation conditions, but is not yet a solution to contextual X input. Do not borrow either Nexus mod's assets, code, data, or patch.

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

This amendment does **not** authorize a hook now. It changes the evidence burden from “complete every control surface before any feature” to “prove the complete safety envelope for each feature and its collision blast radius.” A feature that cannot prove its own context, eligibility, ownership, or rollback remains blocked.

## Exact next blocker and action

The shortest independent next step is access to a clean, authorized Resynced TU 1.0.6 resource set with a build manifest (or equivalent trusted version evidence) and its matching Forge/Data files. With that, inspect only the named player combat entry and determine whether it contains an editable pre-action/finisher condition. If it only controls animation selection, record that boundary and stop the data route for X.

If the data route does not expose pre-X eligibility, the remaining decisive evidence is one combined in-game session that records a visible Takedown prompt and its displayed control, then observes the native logical command/action selected from one press. Do not repeat the already-recorded shared-button test without capturing that eligibility state. Until a semantic command/action boundary is observed, no gameplay hook or installable artifact should be published.

## Status boundary

- Implemented: this decision record and a concrete scoped-gate proposal.
- Existing source: offline resolver, edge tracker, and mock-only X bridge; no game adapter or playable runtime hook.
- Tested here: static source/report review only. No synthetic test can prove a Resynced action seam.
- Real game: not tested in this increment; no game behavior is claimed.
- Separate original-AC4 diagnostic #28 / PR #29: untouched.

