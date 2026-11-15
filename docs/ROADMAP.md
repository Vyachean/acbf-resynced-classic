# Roadmap

The roadmap is ordered by **uncertainty reduction and observable gameplay value**, not calendar dates. Nothing is considered implemented until a reproducible test demonstrates it.

## Phase 0 — Research and baseline (current)

- [ ] Identify testable ACBFR build(s), original AC4 reference build, architecture and input paths.
- [ ] Compare relevant actions and controller mappings in both games.
- [ ] Investigate loader integration, API availability, possible hooks and coexistence risks.
- [ ] Choose a legal and technically viable runtime integration approach.
- [ ] Record exact unknowns, references, and reproducible failure scenarios in [RESEARCH](RESEARCH.md).
- [ ] Define a version detection and safe disable strategy.

**Exit:** documented feasibility decision for capturing input and dispatching actions, with no unsupported assumptions.

## Phase 1 — Minimal instrumentation and control core

- [ ] Establish a reproducible Windows build toolchain and CI.
- [ ] Introduce an isolated, deterministic contextual-action resolver with synthetic test cases.
- [ ] Add version-safe integration and disabled-by-default diagnostics.
- [ ] Observe raw controller input, confirmed player context, and resolved actions.
- [ ] Make instrumentation opt-in and log only necessary, sanitizable data.

**Exit:** a harmless diagnostic build and testable action resolver (not necessarily playable gameplay changes).

## Phase 2 — First playable milestone: X

- [ ] Document AC4 and Resynced reference behaviors for attack, finisher, and assassination.
- [ ] Find an action-level dispatch mechanism instead of mapping multiple actions to X.
- [ ] Implement priority rules only for contexts that can actually be detected.
- [ ] Verify X attack + finisher + assassination scenarios and non-context behavior.
- [ ] Validate no double-actions, stuck inputs or regressions after context transitions.

**Exit:** X behaves consistently in the documented scenarios on an explicitly identified game build.

## Phase 3 — Classic controller profile

- [ ] Implement context-aware B defense actions.
- [ ] Investigate direct A defense break.
- [ ] Investigate RT/high-profile traversal and unwanted contextual movement.
- [ ] Add configurable profile, reversible activation and conflict handling.
- [ ] Regression-test controller hold/repeat timing and interaction with other ASI mods.

**Exit:** tested controls profile with a published limitations and compatibility matrix.

## Phase 4 — Hardening and release

- [ ] Robust game/version detection and fail-closed loading.
- [ ] Explicit diagnostics toggle and privacy review.
- [ ] Reproducible release builds, checksums, installation and removal instructions.
- [ ] Publish only files original to the project and components we are licensed to distribute.
- [ ] Document known issues and tested loader combinations.

**Exit:** first tagged release; no untested universal compatibility claim.

## Later / optional

- [ ] Combat timing and feedback (only if technically justified).
- [ ] Weapon switching ergonomics.
- [ ] Parkour action arbitration.
- [ ] Other targeted mechanic differences established by comparisons.

## Principles for prioritization

1. First remove action conflicts that make controls unreliable.
2. Prefer changes that can be verified using synthetic tests and short in-game scenarios.
3. Avoid invasive animation/combat redesign until the control layer is understood.
4. A failed feasibility study is a valid documented outcome; do not silently broaden scope.
