# Project status and maintenance index

**Snapshot:** 2026-11-17 · **Repository:** [Vyachean/acbf-resynced-classic](https://github.com/Vyachean/acbf-resynced-classic) · **Main source tag confirmed:** `v0.1.17`. This is a **manually reviewed status snapshot**, not an automatic dashboard; use linked [open PRs](https://github.com/Vyachean/acbf-resynced-classic/pulls) and [issues](https://github.com/Vyachean/acbf-resynced-classic/issues) for current state.

## Product goal and actual evidence

**Goal:** progressively restore the original AC4's contextual controls and combat *behavior* in Black Flag Resynced; original AC4 serves as the reference game, **not** a binary-compatible code donor.

**Merged `main`:** synthetic-tested C++ controller logic and native action-bridge contract; Windows x64 read-only diagnostics with standalone ASI/Puppet Native host alternatives; project research and dual-game modding knowledge base. **No playable Resynced mod, no confirmed compatible target-loader stack, no demonstrated native pre-action eligibility or independent game action dispatch on `main`.** A green build or repository version tag is not an in-game pass.

**Unmerged findings:** [PR #31](https://github.com/Vyachean/acbf-resynced-classic/pull/31) reports a local Resynced 1.0.6 boot into gameplay with a reversible **parry-window FORGE candidate**, but no demonstrated changed parry/finisher result; its “arena” is an **offline C++ simulation**, not an in-engine combat arena. [PR #29](https://github.com/Vyachean/acbf-resynced-classic/pull/29) reports a version-gated **original AC4 x86** diagnostic smoke run, with unresolved action semantics. **Neither PR is merged; claims belong to their PRs and exact observed build, not to the default branch.**

## Current decision: feature-scoped safety gate

[Issue #18](https://github.com/Vyachean/acbf-resynced-classic/issues/18) states the **current canonical policy**: a narrow gameplay input feature **may proceed independently** once it verifies its own exact executable, genuine semantic context and action/command interface, all inputs and gestures affected **in that enabled context**, press/release/cancel ownership, native pass-through for all unsupported states, off-by-default activation, rollback and appropriate offline plus real-game checks. Unknown contexts remain untouched. **The global 25-surface controller inventory remains ongoing** and gates a **global** classic profile, **not all isolated features**.

This does **not** mean any current prototype has passed the feature gate. A synthetic or source-based eligibility hypothesis is not a verified game signal. Never ship a game-facing consuming hook before the feature-specific prerequisites are met.

## Workstreams (ordered by reducing the playable-game blocker)

| Lane | Priority | Current state / owner of next step | Definition of a meaningful result |
| --- | --- | --- | --- |
| [#30 Resynced playable combat slice](https://github.com/Vyachean/acbf-resynced-classic/issues/30) | **P0** | Target gameplay outcome; research separate **data-driven parry** and **native logical X** paths without requiring a full original-AC4 port | One *verified* reversible behavior change in the owner's exact build, or concrete `DATA_BLOCKED` / `COMMAND_BLOCKED` with evidence |
| [#10 Native eligibility/dispatch](https://github.com/Vyachean/acbf-resynced-classic/issues/10) and [#4 X proof](https://github.com/Vyachean/acbf-resynced-classic/issues/4) | **P0** | No pre-X finisher eligibility + independent game command seam proven | Source-backed command context and a real-game native outcome, not button/rumble/animation observation |
| [#18 Feature safety and global inventory](https://github.com/Vyachean/acbf-resynced-classic/issues/18), [#21](https://github.com/Vyachean/acbf-resynced-classic/issues/21), [#22](https://github.com/Vyachean/acbf-resynced-classic/issues/22) | **P0 per affected feature**, **P1 global inventory** | Maintain ongoing control coverage; avoid freezing all independent slices | Feature-specific audit and tested passthrough; global catalog remains open for full-profile release |
| [#33 Runtime semantic test harness](https://github.com/Vyachean/acbf-resynced-classic/issues/33) | **P1 supporting P0** | Physical input/limited movement instrumentation exists; full combat oracle not proven | One engine-semantic controlled combat observation or explicit missing anchor |
| [#28 Original AC4 x86 diagnostics](https://github.com/Vyachean/acbf-resynced-classic/issues/28) and [#26 Game evidence](https://github.com/Vyachean/acbf-resynced-classic/issues/26) | **P1 reference** | [#29](https://github.com/Vyachean/acbf-resynced-classic/pull/29) is a large unmerged observer; source-led static research in [#27](https://github.com/Vyachean/acbf-resynced-classic/pull/27) | Exact-build audit, clean integration and labeled native observations, without transferring x86 offsets |
| [#36 Research knowledge base](https://github.com/Vyachean/acbf-resynced-classic/issues/36) | **Ongoing** | [AC4+BFR knowledge index](knowledge/README.md) and 90-source catalog merged | Curated versioned source provenance; relevant findings become bounded implementation tickets |
| [#6 CI enforcement](https://github.com/Vyachean/acbf-resynced-classic/issues/6) | **Process risk** | Required GitHub status checks intentionally deferred by owner; CI must be manually verified | Block unsafe merges and enforce checks when owner opts in |

## Open pull request review queue (as of snapshot)

**Research cleanup:** historical draft PRs [#27](https://github.com/Vyachean/acbf-resynced-classic/pull/27) and [#32](https://github.com/Vyachean/acbf-resynced-classic/pull/32) are **closed as superseded**, not merged. Their distinct findings were curated and squash-merged via [PR #39](https://github.com/Vyachean/acbf-resynced-classic/pull/39) into [sanitized static evidence](knowledge/LOCAL_STATIC_EVIDENCE_2026-11-15.md) and the 90-source KB. The ongoing underlying research issues remain open. The independent, synthetic original-AC4 policy part of #31 is being integrated separately; the game-facing evidence and broad draft remain in #31 until independently reviewed.

| PR | Scope | Integration / review boundary |
| --- | --- | --- |
| [#27](https://github.com/Vyachean/acbf-resynced-classic/pull/27) | **Research only:** installed-game static and GFL evidence, unresolved actor linkage | Verify confidentiality and evidence grade, then CI / squash. Keep separate from native observer |
| [#29](https://github.com/Vyachean/acbf-resynced-classic/pull/29) | **Original AC4 x86** diagnostic ASI and analyzer, with reported local smoke evidence | Review exact-build guard, source permissions, crash/rollback, output privacy and Windows x86 pipeline; green CI ≠ every hook's semantic validity |
| [#31](https://github.com/Vyachean/acbf-resynced-classic/pull/31) | Broad feature-safety policy, offline counter model + standalone **offline arena**, experimental BFR patch report | High overlap with README/ROADMAP/audit; reconcile with issue #18 and this status document before merge. Do **not** classify the offline arena as in-game NPC control |
| [#32](https://github.com/Vyachean/acbf-resynced-classic/pull/32) | Research comparison of community combat mods | Check sources/claim scope and whether text is already covered by merged knowledge base; if duplicate, link or condense, avoid parallel contradictory summaries |

**Integration order:** finish small non-overlapping research/housekeeping decisions first. Do not merge #27/#29/#31/#32 in bulk or infer their readiness from PR title or local test claims. Before merging any PR, assess whether it overlaps later PRs; keep conflict/rebase responsibility explicit.

## Where information belongs

| Topic | Canonical entry |
| --- | --- |
| Product status, currently open work, priority and integration gates | **This page**; authoritative live status is GitHub issues/PRs |
| Project goal and getting started | [README](../README.md) / [SCOPE](SCOPE.md) |
| Long-term milestones and work completion criteria | [ROADMAP](ROADMAP.md) |
| Normative contribution, CI, squash/version rules and user-interaction budget | [WORKFLOW](WORKFLOW.md) |
| Feature-scoped safety contract | **[Issue #18](https://github.com/Vyachean/acbf-resynced-classic/issues/18)**, explained in [CONTROL_SYSTEM_AUDIT](CONTROL_SYSTEM_AUDIT.md) |
| Resynced semantic input/action research | [INTEGRATION_FINDINGS](INTEGRATION_FINDINGS.md), [RESEARCH](RESEARCH.md), issue #10 |
| Game mechanics / control catalog | [CONTROL_SYSTEM_AUDIT](CONTROL_SYSTEM_AUDIT.md), [COMBAT_CONDITIONS](COMBAT_CONDITIONS.md), machine ledger |
| Cross-game modding reference and public third-party URLs | [Knowledge base](knowledge/README.md), [source catalog](../research/modding_sources.json) |
| Test design and one-session diagnostics | [TEST_STRATEGY](TEST_STRATEGY.md), [RUNTIME_TEST_HARNESS_RESEARCH](RUNTIME_TEST_HARNESS_RESEARCH.md) |

## GitHub Actions and release facts

- [CI workflow](../.github/workflows/ci.yml): `repository-contract`, `unit-tests`, `windows-build`; PR `version-main` is **intentionally skipped**. **All applicable jobs must complete green** before an authorized squash merge; do not merge on pending/failed/unknown CI.
- `v0.1.17` was verified as an existing git ref with current dual-game knowledge material. It is a **repository snapshot**, not a playable mod release. The newest version must be checked again after each merge; GitHub Actions `version-main` is responsible for tags on `main`.
- [Issue #6](https://github.com/Vyachean/acbf-resynced-classic/issues/6) records that GitHub's branch ruleset does **not** yet require status checks (owner decision). This gap cannot be fixed by changing Markdown or a workflow; enforce manual review.
- No locally installed game/loader changes, copyrighted game files, platform protections or raw reverse-engineering dumps should be included in documentation commits.

## Next project maintenance steps

1. Align old blanket-blocking prose with issue #18. Keep all project binary hooks disabled without evidence, but avoid requiring unrelated surface catalog completion.
2. Review PR #32 for duplication against merged knowledge base; review #27 for publishable research provenance; **do not merge #31 / #29 just to reduce PR count**.
3. After any decision, update linked issue and this snapshot, then check that the roadmap and README no longer give conflicting next steps.
4. Verify required PR jobs and future `version-main` tagging after every squash. Do not prematurely close gameplay or knowledge-base tasks.

**Scope of this page:** organizational facts and confidence boundaries, not independent verification of third-party tool compatibility or measured game behavior.
