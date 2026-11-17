# Original AC4 combat model: offline vertical slice

**Status:** a source-backed C++ decision model and deterministic scenario executable are implemented. They are not connected to Resynced, do not read process memory, and cannot consume or dispatch game input.

## Modeled sequence

The model accepts an already-known phase and gesture, then returns one candidate action:

| Phase | Input | Candidate |
|---|---|---|
| Neutral combat | Hold B | Guard |
| Incoming melee | Tap B | Enter the counter follow-up phase |
| Incoming melee | Hold B | Guard |
| Incoming melee | Tap B, Brute target | Evade |
| Incoming melee | Tap B, Captain or unknown target | Unresolved |
| Incoming melee | Tap B, two simultaneous warnings | Double counter |
| Counter follow-up | Tap X / B / A / Y | Counter kill / throw / disarm or stun / selected tool |
| Incoming gunfire | Tap A, nearby shield target | Human shield |
| Incoming gunfire | Tap A, no nearby shield target | Native pass-through |

Inputs outside these specified cases pass through. Unknown combat phase, enemy class, or shield-target state fails closed as unresolved. Captain routing remains unresolved because the period source describes a different parry/stun interaction and the matrix has source contradictions. In particular, the implementation does not assign durations to “tap”, decide how long the follow-up window lasts, or invent phase-detection rules.

## Verification

`legacy_combat_model_offline` exercises the sequence and fail-closed cases under the project's existing Windows CMake/CTest workflow. CI is the designated build and test environment; no local build was run for this increment.

## Evidence and boundary

The candidate behaviors are drawn from the documented AC4 combat audit in [CONTROL_SYSTEM_AUDIT.md](CONTROL_SYSTEM_AUDIT.md), which cites the original Prima combat/advanced-tips guides. They are reference behaviors, not evidence that Resynced has matching phases or dispatch commands.

This vertical slice makes the multi-stage policy executable and reviewable. A Resynced adapter still needs verified phase/eligibility signals, exact build identity, input ownership and fallback behavior. Until the whole-system gate in issue #18 is satisfied or formally amended, this model must remain disconnected from any ASI/Puppet runtime.
