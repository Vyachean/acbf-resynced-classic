# Research register

**Last public-source review:** 2026-11-15. All findings are **informational**; this project has **no tested in-game mod yet**.

This index separates **authoritative documented behavior**, **third-party source observations**, **user/community reports**, and **unresolved game-engine questions**.

## Detailed findings

- [AC4 vs Resynced control reference](CONTROL_REFERENCE.md): source-backed Xbox-style mapping, AC4 reference vs Resynced default, release differences and contradictory remap reports.
- [Resynced integration feasibility](INTEGRATION_FINDINGS.md): x64 ASI examples, source license restrictions, version-aware state hook examples, unverified action dispatcher and decision gates.
- [Single-session test strategy](TEST_STRATEGY.md): exhaust CI before running a game.
- [Machine-readable scenarios](../research/reference_scenarios.json): **design-only fixture**, not actual game results.

## Authoritative facts established from public documentation

1. AC4's original reference control scheme has **X for melee/assassination, B for counter/interact, A for defense break/jump, RT for high-profile**. See original Xbox e-manual linked in the [control reference](CONTROL_REFERENCE.md); a separate written controller table is also cited there.
2. Resynced's launch controller map assigns **RB ordinary attack, Y takedown / hold Y assassinate, LB parry, B dodge, L3 sprint**; the source is a published July 2026 guide and **not guaranteed current for every update**.
3. Ubisoft documents native input remapping and configurable press/hold/double-tap/toggle behavior.
4. Ubisoft's title update 1.0.6 included a specific fix for **parkour while holding remapped RT sprint**; TU 1.0.8 changed/expanded takedown variants.
5. Ubisoft describes Resynced's combat finisher as a distinct action that depends on making opponents vulnerable (e.g. defense break or perfect parry). This is distinct from the original game's advertised control semantics.

## Independent projects / evidence with caveats

- Multiple Nexus mods distribute x64 ASI files, but working in another installation **does not validate our loader**.
- Public HapticsBridge code contains instrumentation for **21 movement/swimming/traversal state transitions**, compatibility profiles and hook validation tests. The inspected state hooks **are not a demonstrated takedown-eligibility or action-dispatch interface**.
- Puppet Native advertises plugin support but no sufficiently verified API contract for this project has been found. Earlier conflicting loader behavior remains a user report.
- A third-party TU 1.0.8 data mod alters how certain combat finishers trigger from ordinary attacks, but cannot be assumed to implement the requested control resolver and is **not free to copy**.

Full URLs and study-only license caveats are in [integration findings](INTEGRATION_FINDINGS.md).

## Conflicting reports / why we cannot skip proof

User testing reports that placing X attack and finisher on the same binding causes only ordinary attacks. Some July 2026 community accounts say this combination worked in particular contexts, while other reports describe attacks overriding assassination. **The result may depend on player state, input mode and game version.** Neither side supplies a complete engine-level priority model.

For the first playable feature, we therefore need **verified action eligibility and controllable dispatch**. Key remapping alone cannot be advertised as a fix until conflicts are demonstrably resolved.

## Evidence and unknowns

| Question | Status | What proves it |
| --- | --- | --- |
| Original/reference mapping (X/B/A/RT) | **Documented** for the reference control meanings | Original manual and published reference table |
| Resynced default launch bindings | **Documented** for July 2026 launch mappings | Published controller guide |
| Validity of shared X attack/takedown on target 1.0.6 and 1.0.8 | **Conflicting / unverified** | Targeted scenario with exact executable/build and binding modes |
| Controller event interception for target installation | **Unverified** | Safe original diagnostic ASI with compatible loader |
| Availability of finisher/assassination eligibility | **Unknown** | Public API evidence or a read-only state hook with test evidence |
| Action dispatch independent of game binding | **Unknown; primary blocker** | Minimal reversible, version-specific proof |
| Coexistence with Puppet Native and ACBlackFlagFix | **Unknown; possible loader collision reported** | Clean-loader matrix, after static review |
| Exact current target executable/build/hash | **Unknown** | Combined diagnostic session, not a separate manual step |
| Safe disable/uninstall | **Design requirement only** | Reversible integration and regression tests |

## Decision

**Proceed with original, deterministic resolver specifications/tests and further static API investigation; do not yet ship game-facing hooks.** Runtime feasibility remains a hypothesis. A real game launch should only be requested when a CI-built multi-probe diagnostic would answer otherwise unresolvable questions in a **single session**.

## Source and copyright policy

For every new source note, include URL, target version, claim, and confidence. Study publicly accessible implementations but do **not** paste code, hook signature tables, derived proprietary data or third-party assets into this all-rights-reserved project without compatible permission. Especially note the upstream HapticsBridge non-commercial terms and the Nexus restrictions for individual mods.
