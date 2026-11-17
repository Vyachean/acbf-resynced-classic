# Research register

**Last public-source review:** 2026-11-15. All findings are **informational**; this project has **no tested in-game mod yet**.

This index separates **authoritative documented behavior**, **third-party source observations**, **user/community reports**, and **unresolved game-engine questions**.

## Safety gate: specific feature first, whole-system audit for global profile

[Issue #18](https://github.com/Vyachean/acbf-resynced-classic/issues/18) requires **feature-scoped** evidence for game-facing input consumption: every action affected in enabled contexts, exact build, verified native state/dispatch, gestures, release/cancellation, passthrough and rollback. The [whole-system audit](CONTROL_SYSTEM_AUDIT.md) and [coverage ledger](../research/control_system_ledger.json) remain a necessary reference for a global classic profile, not an unrelated prerequisite for narrow work. A documented button map is not an arbitration contract: original AC4 B guard/tap-counter/second face-button actions disprove the simplistic B resolver.

## Detailed findings

- [AC4 vs Resynced control reference](CONTROL_REFERENCE.md): source-backed Xbox-style mapping, AC4 reference vs Resynced default, release differences and contradictory remap reports.
- [Resynced integration feasibility](INTEGRATION_FINDINGS.md): x64 ASI examples, source license restrictions, version-aware state hook examples, unverified action dispatcher and decision gates.
- [Single-session native X experiment](X_INPUT_EXPERIMENT.md): **deferred** because the owner already observed a same-button attack/takedown conflict; repeat only for a new concrete hypothesis.
- [Single-session test strategy](TEST_STRATEGY.md): exhaust CI before any mod-based runtime diagnostic.
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
- Puppet Native's public MIT-licensed source **does** provide plugin ABI v1 (`PN_Init`/`PN_Shutdown`, logging and game module/base). Our plugin lifecycle is CI-tested. **Its ABI has no action-eligibility/finisher-dispatch method**, and loader compatibility with the user's installed executable is unverified. See [Puppet Native study](PUPPET_NATIVE.md).
- A third-party TU 1.0.8 data mod alters how certain combat finishers trigger from ordinary attacks, but cannot be assumed to implement the requested control resolver and is **not free to copy**.

Full URLs and study-only license caveats are in [integration findings](INTEGRATION_FINDINGS.md).

## Conflicting reports / why we cannot skip proof

User testing reports that placing X attack and finisher on the same binding causes only ordinary attacks. Some July 2026 community accounts say this combination worked in particular contexts, while other reports describe attacks overriding assassination. **The result may depend on player state, input mode and game version.** Neither side supplies a complete engine-level priority model.

**Do not repeat the already-observed same-button Attack/Takedown failure** without an independently grounded new question. [X-001](X_INPUT_EXPERIMENT.md) is deferred; the next critical proof is **verified native pre-action eligibility plus an independently invokable logical command** (or a precise blocker). The original Y takedown baseline and context of the old observation remain incompletely documented; do not claim the mapping fails in every situation.

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

**Next: verified internal eligibility and native dispatch research.** The maintainer has already observed Attack overriding Takedown when the two actions share a button. Do not repeat the same binding experiment. It requires no newly installed mod or CI artifact because the observable result is a Takedown animation, not an internal engine signal. If the native dispatcher fails under a validated Y baseline, continue with evidence-led game-action integration; do not ship speculative offsets or replace the user's loader. Any later *mod-based* test still requires the comprehensive CI-built single-session diagnostic.

## Source and copyright policy

For every new source note, include URL, target version, claim, and confidence. Study publicly accessible implementations but do **not** paste code, hook signature tables, derived proprietary data or third-party assets into this all-rights-reserved project without compatible permission. Especially note the upstream HapticsBridge non-commercial terms and the Nexus restrictions for individual mods.

## Additional primary-source warning (2026-11-15)

Ubisoft's [ACBFR-641](https://www.ubisoft.com/en-us/game/assassins-creed/black-flag-resynced/bug-reporter/issues/ACBFR-641) reports that some **assassinations** on game version **1.0.2** require three or four button presses despite an on-screen prompt. The report is marked `Deprioritized`. **Do not** extrapolate that to all Takedowns or version 1.0.6, but explicitly check the unmodified Y Takedown baseline before judging a shared-X remap. A July 19 [video](https://www.youtube.com/watch?v=q4r1P2Q2bsE) documents Attack=X and Takedown=X under a wider layout, but does not establish the exact Press/Hold method or success on 1.0.6. These sources support testing, not a compatibility claim.

## Data/animation alternative investigated, 2026-11-15

The independent [Unrestricted Combat Finishers](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635) mod, uploaded 2026-11-02, says it enables **MicroKill/SoftKill finishing animations from normal lethal attacks**, using modified contents of `Edward_Default.data` inside a BFR patch forge. The author supports **1.0.8**, not the maintainer's 1.0.6, and forbids modification/redistribution without permission. [AnvilToolkit 1.3.7](https://www.nexusmods.com/site/mods/455) supports Resynced Forge/Data editing. **This proves only that an animation/data modification path exists**, not the desired choice of a genuine pre-attack Takedown via X. Do not redistribute any Nexus author file, assume 1.0.6 support, or confuse kill animations with game action dispatch.

The project now has [a two-phase, synthetic-only X action bridge](../include/acbf/action_bridge.h) so that a future *verified* eligibility/dispatch seam can be connected without changing the control resolver. It sends no game commands; all compatibility gates default to false. Do not call it a playable mod.

## AC4 counter follow-up buttons clarified (2013 primary-era gameplay guide)

The 2013 [Prima Advanced Tips](https://primagames.com/tips/assassins-creed-4-black-flag-advanced-tips) explicitly lists Xbox input **B to enter counter**, followed by **X kill**, **B throw**, **A disarm/stun**, **Y use tool**. This resolves the follow-up *button labels* which were unreadable in the longer eGuide icon markup; it does **not** verify exact input-buffer timing, attack interruption, runtime action IDs or whether the present 2013 PC build has identical dispatch behavior. The audit and machine-readable ledger now separate those categories.

## Additional independently described input exceptions (2026-11-15)

- The original [Prima combat guide](https://primagames.com/eguides/assassins-creed-iv-black-flag-eguide/primer/combat) describes an exceptional **B-initiated double counter kill without a second input** when two melee enemies threaten Kenway simultaneously, a **repeat-A guard break followed by knockdown**, and a **human shield** during a firearm warning. The **A** button for human shield is explicitly corroborated by a [2013 Xbox achievements guide](https://www.xboxachievements.com/game/assassins-creed-4-black-flag-xbox-one/guide/). No actual runtime priority or timing threshold is inferred from these texts.
- The Resynced [launch controls table](https://canary.shacknews.com/article/149927/assassins-creed-black-flag-resynced-controls) includes **LB press Parry / hold LB Observe**, **RB Attack / RB Hold Heavy / RB while perched Hang Enemy Below**, and **R3 press Lock On / hold R3 Eagle Vision / double-tap R3 HUD**. These are launch-era button descriptions; behavior on the owner's **1.0.6** is unknown.
- These cases are now explicit in `research/control_system_ledger.json` with independently validated IDs, sources and default-false engine-priority assertions. The feature-scoped game-input safety gate remains in force; no repeat of the user-observed dual-binding failure.

## Enemy archetype matchups and title-update action-state boundaries

[Detailed combat condition analysis](COMBAT_CONDITIONS.md) now documents why a single global `can_counter` or `can_takedown` flag does not reconstruct either game's behavior. Original 2013 [Prima enemy archetype guide](https://primagames.com/eguides/assassins-creed-iv-black-flag-eguide/reference-analysis/enemy-archetypes) has a 5×10 susceptibility matrix, including a Brute's counter-to-dodge outcome and an **internal conflict** for Captain Break Defense. These are **guide labels**, not invokable native action APIs.

Ubisoft's [TU1.0.6 patch notes](https://www.ubisoft.com/en-gb/game/assassins-creed/news/5c72WnN2ot90ii8d9Le1Wt/ac-black-flag-resynced-title-update-106-patch-notes) document a fixed RT-held sprint remap/parkour collision, unintended takedown on a vault, Quick Rope Dart interruption problems, combat-to-smoke assassination and suspicious-target sweep failures. Official [TU1.0.7](https://www.ubisoft.com/de-de/game/assassins-creed/news/24bBcwoOoYNZ2gt2yi3HdH/ac-black-flag-resynced-title-update-107-patch-notes) and [TU1.0.8](https://www.ubisoft.com/en-us/game/assassins-creed/news/5Z1RxwznM6CcrkSEVoibmh/ac-black-flag-resynced-title-update-108-patch-notes) change additional state-specific behaviors, which **cannot be assumed present in 1.0.6**. Full version/source references and status guardrails are in [the research JSON](../research/combat_conditions.json). Full-control gameplay audit remains incomplete.
