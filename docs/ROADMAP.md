# Roadmap

The roadmap is ordered by **uncertainty reduction and observable gameplay value**, not calendar dates. A completed research deliverable is **not** a validated runtime capability.

## Phase 0 — Whole-system controls inventory (ongoing reference)

The [issue #18 audit](https://github.com/Vyachean/acbf-resynced-classic/issues/18) remains incomplete, but it no longer blocks unrelated feature work. It is a source-labelled reference inventory. A feature may proceed when its exact context, eligibility, command ownership, affected collisions, release behavior, pass-through boundary, rollback and off-by-default state are established. Unknown contexts remain native passthrough.

The feature-scoped gate and current Resynced evidence are recorded in [the Resynced vertical-slice decision](RESYNCED_X_VERTICAL_SLICE.md). It removes the full-catalog prerequisite; it does not authorize guessed state, command or address hooks.

- [x] Build a source-labelled [control-system audit](CONTROL_SYSTEM_AUDIT.md) that distinguishes documented functions from runtime priority.
- [x] Inventory 25 control surfaces across AC4 and Resynced, including combat phases, stealth, traversal, water, helm/gunnery, UI and special modes.
- [x] Inventory cross-surface collision hazards, explicit unknowns and source-backed/hypothesized transitions in a [CI-validated control ledger](../research/control_system_ledger.json).
- [x] Establish original AC4 counter follow-up buttons from 2013 Prima: **X kill, B throw, A disarm/stun, Y tool**; separate from B hold guard / B tap entry.
- [x] Identify additional original AC4 combat input exceptions from period guides: B double-counter on simultaneous warnings and A human shield against gunfire.
- [x] Identify Resynced LB Observe hold vs parry press, RB attack/heavy vs perch hang, and R3 gesture overlap from July controller documentation.
- [x] Extract original enemy archetype × move matchups (5 classes × 10 moves), including Brute counter-to-dodge and source-conflicted Captain guard break, into [research/combat_conditions.json](../research/combat_conditions.json).
- [x] Inventory TU1.0.6 action-state fixes and separate TU1.0.7/1.0.8 changes with official Ubisoft source links and tests.
- [x] Build a read-only [physical-button impact analyzer](INPUT_IMPACT.md) and CI tests that enumerate all documented uses and unresolved bindings for candidate overrides across the whole control catalog.
- [ ] Verify archetype/target cues, game-state transitions, exact counter timing, buffering/cancellation and native action dispatch; do not infer from the guide matchup table.
- [ ] Reconstruct the Resynced control graph for TU 1.0.6: input gestures, modifiers, combat states, parkour configuration and collision precedence.
- [ ] Cover missing tools, interaction prompts, special mission/minigame modes and transitions, or make those modes explicit passthrough.
- [ ] Verify that existing mock policies for X/B/A do not eliminate original AC4 counter, finishing or traversal actions.
- [ ] Approve every planned input interception point by target mode, version, action, gesture, cancel/release and fallback; no global remapping.

**Gate exit for each feature:** evidence for that feature's supported state and its collision blast radius, with native passthrough elsewhere. The global classic profile still requires the whole relevant input catalog. No single feature must wait for unrelated surfaces.

## Earlier Phase 0 — Research and baseline (partially completed)

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
- [x] Cap local JSON diagnostics (X/Y/A/B/LB/RB/LT/RT edges, no paths or network), and require explicit installation of a diagnostic host.
- [ ] Add validated semantic telemetry for one combined feature-specific game test when the local build can reach single-player; do not collect generic input traces.

**Exit:** a harmless diagnostic build and testable action resolver (not necessarily playable gameplay changes).

## Phase 2 — First playable milestone: X (blocked by missing semantic seam)

Issue #30 decoded selected combat states from the locally available Resynced executable; the user reports TU 1.0.6. No safe numeric parry patch point or pre-action X command seam has been established. See [the selected-resource findings and scoped gate](RESYNCED_X_VERTICAL_SLICE.md). No game adapter or playable X artifact is claimed.

**The earlier X-001 remapping-only test is deferred for the reported 1.0.6 profile.** The owner already observed ordinary Attack winning over Takedown when sharing one button. Do not repeat the same setup. TU 1.0.8 does not prove a changed input priority. X remains blocked by the missing pre-action eligibility and command seam, not by unrelated audit rows.

- [x] Document baseline AC4 and Resynced controller mapping and **desired** attack, finisher, assassination cases (not yet verified in game).
- [x] Document a controlled one-launch comparison of native Y takedown and shared-X mapping, with explicit pass/fail/inconclusive decision gates (not yet executed).
- [x] Implement an **offline-only** two-phase X action bridge contract with ten feature-scoped safety gates, physical input claim, single-shot dispatch and synthetic CTest coverage.
- [ ] Identify a game-verified eligibility source and native action dispatcher for Resynced (or document infeasibility).
- [ ] Implement priority rules only for contexts that can actually be detected.
- [ ] Verify X attack + finisher + assassination scenarios and non-context behavior.
- [ ] Validate no double-actions, stuck inputs or regressions after context transitions.

**Exit:** X behaves consistently in documented scenarios on an explicitly identified game build.

## Phase 3 — Classic controller profile (blocked only by each feature's own evidence)

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

## Diagnostic host consolidation (v0.1.7 candidate)

- [x] Reuse the same read-only diagnostic worker under Puppet Native ABI v1 and standalone ASI without duplicating the game observer.
- [x] Add separate, CI-generated packaging for two mutually exclusive diagnostic hosts.
- [ ] Confirm the actual target loader/executable and collect native *pre-action eligibility* evidence before any controller override.

## Confirmed same-button collision and research priority

The maintainer previously assigned **Attack and Takedown to the same physical button**; the game performed only ordinary Attack. This already-observed collision should not be retested without new, concrete evidence. The visibility of a Takedown prompt and separate Y baseline during that experiment are **unknown**, so do not generalize to all contexts. [X-001](X_INPUT_EXPERIMENT.md) is **deferred**. The next real capability gap is a verified *pre-action eligibility* signal and independent game action dispatch. No native bridge implementation currently satisfies these requirements.
