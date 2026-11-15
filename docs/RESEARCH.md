# Research register

**Purpose:** keep observations, independently verified technical facts, hypotheses, and gaps separate.

## Inputs supplied for initial design (not independently verified)

These are **reported observations and requirements**, not proof of how the Resynced engine works:

- Original AC4 uses context-sensitive action assignment for attacks, finishers, and related moves.
- ACBFR has separate combat and finisher bindings; attempting to put multiple actions on one button reportedly leads to the wrong action.
- Resynced's defensive actions reportedly require different buttons, while the desired reference uses a shared-feeling control.
- Original AC4 is the primary reference; the aim is not to clone every mechanic immediately.
- Earlier player testing encountered possible loader interactions with unrelated ASI mods. Such interactions must be reproduced before stating an incompatibility.

## Evidence table

| Question | Current status | Required evidence |
| --- | --- | --- |
| What exact ACBFR builds can be modified safely? | Unknown | Executable version/hash and controlled runtime test. |
| Which controller/input pipeline is used? | Unknown | Public technical source or instrumented observations. |
| Can action eligibility be observed? | Unknown | Reproducible state observation with version record. |
| Can finishers be invoked directly without rebinding? | Unknown; central blocker | Minimal proof of action-level dispatch. |
| What classic AC4 priorities govern X/B? | Partly specified by desired outcomes; details unknown | Captured reference scenarios in original game. |
| Which load methods coexist with other ASI mods? | Unknown | Loader/version matrix and clean baseline test. |
| Can changes be reverted without patching base files? | Not demonstrated | Uninstall/disable regression test. |

## Source handling

When adding research, include a **URL, upstream commit or release identifier, publication date (if known), what was actually observed, and how the finding affects architecture**. A tool name or mod description is not evidence of a functioning integration.

Do not copy proprietary source from leaked builds, lift game assets, or assume that public GitHub code has a permissive license.

## First research tasks

1. Build exact input/action behavior matrix from AC4 vs ACBFR.
2. Inventory credible public Resynced modding interfaces and licenses.
3. Verify a clean, reversible mod-loading approach.
4. Prove/disprove action dispatch for the X finisher case.
