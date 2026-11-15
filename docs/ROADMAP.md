# Roadmap

The roadmap is ordered by **uncertainty reduction and observable gameplay value**, not calendar dates. A completed research deliverable is **not** a validated runtime capability.

## Phase 0 — Research and baseline (current)

- [x] Document the source-backed **original AC4 vs Resynced launch bindings**, known control conflicts and title-update caveats in [CONTROL_REFERENCE](CONTROL_REFERENCE.md).
- [x] Inventory public **ASI modding examples, state-hook evidence, alternative data-mod route and reuse/license constraints** in [INTEGRATION_FINDINGS](INTEGRATION_FINDINGS.md).
- [x] Create **source-labelled, explicitly unverified** test scenarios and validate their structure in GitHub Actions.
- [x] Verify a public **Puppet Native PluginLoader C ABI v1** and build a no-hook lifecycle test under GitHub Actions (integration seam only).
- [ ] Establish which **game action eligibility and dispatch** boundary is safe to integrate.
- [ ] Prove or disprove the ability to read **finisher / assassination eligibility** (public evidence before runtime instrumentation).
- [ ] Prove or disprove ability to dispatch an eligible action **independently of existing button mappings**.
- [ ] Verify Puppet Native loader coexistence and fail-closed executable identification without introducing fragile assumptions.

**Exit:** documented and independently supported decision for the *specific* action-eligibility and dispatch integration, or a defined, combined diagnostic needed to settle it. **Current result:** enough to design a pure controller resolver, not enough to deliver a playable game integration.

## Phase 1 — Minimal instrumentation and control core

- [x] Establish a reproducible **Windows offline core** build and `ctest` in GitHub Actions (not an installable mod).
- [x] Build an original, read-only x64 **diagnostic ASI** and exercise DLL loading/report generation in a **synthetic CI host**.
- [ ] Validate the actual **ASI loader and Resynced game integration**, as part of a future combined session.
- [x] Introduce an isolated, deterministic **candidate** contextual-action resolver and press edge tracker with synthetic test cases (not connected to a game).
- [x] Add a **non-modifying** diagnostic module that reports executable SHA-256, loaded local ASIs/proxies and XInput edges; **not** native gameplay context.
- [ ] Add version-safe, semantic game-state observation and fail-closed action hook integration.
- [x] Observe XInput button transitions in an external read-only observer (tested in a synthetic host and deterministic fake XInput provider).
- [x] Implement a **read-only PE import and pre-probe loaded DLL inventory** for input-backend hypotheses; test malformed synthetic PE fixtures in Windows Actions.
- [ ] Observe **verified in-game** player context/action eligibility and actual action dispatch.
- [x] Cap local JSON diagnostics (only X/B/A/RT edges, no paths or network), and require explicit installation of the ASI.
- [ ] Add validated semantic event telemetry for the **single comprehensive game test**; no game launch before that.

**Exit:** a harmless diagnostic build and testable action resolver (not necessarily playable gameplay changes).

## Phase 2 — First playable milestone: X

- [x] Document baseline AC4 and Resynced controller mapping and **desired** attack, finisher, assassination cases (not yet verified in game).
- [ ] Find an action-level dispatch mechanism instead of mapping multiple actions to X.
- [ ] Implement priority rules only for contexts that can actually be detected.
- [ ] Verify X attack + finisher + assassination scenarios and non-context behavior.
- [ ] Validate no double-actions, stuck inputs or regressions after context transitions.

**Exit:** X behaves consistently in documented scenarios on an explicitly identified game build.

## Phase 3 — Classic controller profile

- [ ] Implement context-aware B defense actions (first establish original B counter/dodge details).
- [ ] Investigate A defense-break equivalent (do not assume Resynced Sweep means AC4 guard-break).
- [ ] Investigate RT/high-profile traversal and unintended contextual movement.
- [ ] Add configurable profile, reversible activation and conflict handling.
- [ ] Regression-test controller hold/repeat timing and other ASI-mod interactions.

**Exit:** tested controls profile with a published limitations and compatibility matrix.

## Phase 4 — Hardening and release

- [ ] Robust game/version detection and fail-closed loading.
- [ ] Explicit diagnostics toggle and privacy review.
- [ ] Reproducible release builds, checksums, installation and removal instructions.
- [ ] Publish only files original to the project and components we are licensed to distribute.
- [ ] Document known issues and tested loader combinations.

**Exit:** first tagged *playable release*. Repository source versions already use Git tags, independent of public releases.

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
