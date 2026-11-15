# AC Black Flag Resynced Classic

**Restore the feel of the original Assassin's Creed IV: Black Flag in Black Flag Resynced — starting with controller behavior.**

> [!IMPORTANT]
> **Experimental diagnostic-only ASI and offline control core.** No playable mod, validated loader, compatible game build, or working action hook exists yet. The Windows diagnostic ASI makes **no game modifications** and is not yet intended for user installation.

## Priority: audit the ENTIRE input systems before gameplay work

**Gameplay input interception is frozen under [issue #18](https://github.com/Vyachean/acbf-resynced-classic/issues/18).** The earlier controller map covered only high-level button labels; it omitted AC4's **hold B block / tap B counter / second face-button follow-up**, high/low profile effects, tool and ship modes, and Resynced gesture conflicts. See [the whole-system behavior audit](docs/CONTROL_SYSTEM_AUDIT.md) and [its machine-readable source/transition/unknown ledger](research/control_system_ledger.json). The audit is **INCOMPLETE**; only offline research, non-game observability and design validation may proceed. No new gameplay hook is allowed until affected surfaces and fallbacks have been justified.

## Goal

Resynced changes how familiar controller inputs map to combat and traversal actions. This project aims to make **Assassin's Creed Black Flag Resynced (ACBFR)** behave more like **Assassin's Creed IV: Black Flag (AC4)**, without requiring players to fight the control scheme.

The first milestone is **classic-style contextual controls**: a button should perform the *appropriate* action for the current game state, rather than merely sharing a key binding with several conflicting actions.

This is an independent, unofficial fan project and is not affiliated with or endorsed by Ubisoft.

## Priorities

| Priority | Desired behavior | Initial acceptance criterion |
| --- | --- | --- |
| P0 | **X: attack / finish / contextual assassination** | A single X binding chooses the correct action in each tested context, without disabling finishers. |
| P0 | **B: counter / dodge** | One B input resolves sensibly by context; exact parry/dodge rules to be verified against AC4. |
| P1 | **A: guard break** | Execute the intended guard-break action without a modifier, if game interfaces permit. |
| P1 | **RT: high-profile movement** | Make high-profile traversal behave more like AC4, subject to investigation. |
| Later | Other actions, weapon selection, combat pacing, parkour | Decide separately after the input layer is proven. |

These mappings express **design targets**, not claims that the target game's internals or the original game's complete control logic have been verified. Xbox-style button names are used for clarity; other controllers are not yet validated.

## Design principles

- **Behavior before bindings.** Remapping alone cannot fix competing in-game actions.
- **Reference AC4, do not copy AC4.** Observe and reproduce interaction semantics; do not ship Ubisoft code, assets or binaries.
- **Minimal interference.** Prefer reversible runtime modifications and clear fail-safe behavior.
- **No blind compatibility claims.** Every supported game version and loader must be tested and recorded.
- **Evidence-driven reverse engineering.** Keep known facts, hypotheses and diagnostics separate.
- **Modular boundaries.** Input handling, state detection, action decisions, and game integration should remain separable.
- **Rare, purposeful manual testing.** Use focused builds and diagnostic logs rather than repeated guesswork.

## Current technical conclusion (2026-11-15)

Public research confirms a plausible x64 ASI distribution path and published **movement-state** instrumentation in an unrelated mod. It **does not confirm** a safe way to detect takedown/assassination eligibility or invoke either action independently. Built-in shared-button remapping has contradictory community reports; no runtime integration is yet implemented. See the [evidence register](docs/RESEARCH.md).

## Status and compatibility

**Status:** Windows Actions builds and tests the C++ controller core **and a read-only diagnostic ASI**, including a synthetic host loading the actual DLL. There is **no playable gameplay mod** and **no in-game validation** yet.

| Target | Status |
| --- | --- |
| ACBFR 1.0.6 | Not verified |
| ACBFR 1.0.8 | Not verified |
| Windows controllers / input APIs | To be investigated |
| ASI loading and coexistence with other mods | To be investigated |
| Original AC4 reference behavior | To be documented and tested |

No DRM bypass or unauthorized game redistribution is part of this project. Users must provide their own lawful copy of the game.

## Project documents

- [Scope and non-goals](docs/SCOPE.md) — target experience and boundaries
- [Roadmap](docs/ROADMAP.md) — research, implementation, validation, and release gates
- [Proposed architecture](docs/ARCHITECTURE.md) — preliminary design, not yet implemented
- [Test protocol](docs/TESTING.md) — focused player testing and diagnostic reports
- [Research register](docs/RESEARCH.md) — evidence and outstanding technical questions
- [Whole control-system audit](docs/CONTROL_SYSTEM_AUDIT.md) — 25 gameplay surfaces, multi-step actions, source quality and blocking unknowns (**mandatory**)
- [Audit coverage ledger](research/control_system_ledger.json) — machine-readable action/gesture catalog, transition hypotheses, hazards and source registry
- [Combat action conditions](docs/COMBAT_CONDITIONS.md) — original AC4 enemy-class matchup caveats and officially documented Resynced TU1.0.6/TU1.0.7/TU1.0.8 state fixes; **not runtime eligibility**
- [Physical-button impact audit](docs/INPUT_IMPACT.md) — conservative, Python-only report of documented actions that a proposed X/B/A/RT or other physical-button override could disable
- [Control reference](docs/CONTROL_REFERENCE.md) — older compact AC4 vs Resynced button map, **not** a full behavior model
- [Integration findings](docs/INTEGRATION_FINDINGS.md) — public modding evidence, licensing, compatibility gates
- [Puppet Native ABI](docs/PUPPET_NATIVE.md) — public plugin API and CI-tested lifecycle bridge, not a gameplay hook
- [Reference cases](research/reference_scenarios.json) — machine-readable design cases (**not tested in game**)
- [C++ control core](include/acbf/classic_control.h) — pure, provisional action resolver (no game integration)
- [Gated X action bridge](include/acbf/action_bridge.h) — single-shot native command contract, fully **offline** with a test fake, **no game hook**
- [Diagnostic ASI](docs/DIAGNOSTICS.md) — experimental read-only Windows observer, built by Actions and tested only with a synthetic host
- [Contributing](CONTRIBUTING.md) — issue reports and contribution rules

## Immediate validation task

**[X-001](docs/X_INPUT_EXPERIMENT.md) is deferred.** The maintainer already observed the same-button Attack/Takedown collision, and repeating it is not useful without a materially different hypothesis. It tests whether the game's own Attack/Takedown arbitration works under a published complete remapping profile. **No project ASI/DLL, loader changes, scripts, or uploads are required.** Only a visible in-game Takedown outcome can settle the observation. If the exact same profile was already tested, report that earlier result instead of repeating it. The protocol does **not** establish a playable mod or game-hook compatibility.

## Getting started

There is **no playable mod to install yet**. A read-only diagnostic ASI is produced as a distinct **diagnostic** GitHub Actions artifact (not a release or gameplay modification). **Do not install the diagnostic build for X-001**; it cannot observe native Takedown outcomes. A future *mod-based* session will require richer semantic probes. It does not include or replace an ASI loader. See [diagnostics](docs/DIAGNOSTICS.md).

Technical observations or reproducible control conflicts can be filed through [GitHub Issues](https://github.com/Vyachean/acbf-resynced-classic/issues). Do not upload game binaries, copyrighted assets, personal data, or crash dumps containing secrets.

## Development workflow and automation

**No game launch by default.** We research and test with public evidence and automated synthetic tests before requesting any in-game diagnostics. For a no-mod control comparison, use one [X-001](docs/X_INPUT_EXPERIMENT.md) game session and a short observed outcome; if a *mod-based* game session later becomes unavoidable, require one CI-built bundle and a sanitized report.

- [Development and versioning policy](docs/WORKFLOW.md): feature branches, **squash-only PR merges**, one version tag per completed merge, GitHub Actions as the sole automation/build service.
- [Single-session diagnostic strategy](docs/TEST_STRATEGY.md): offline-first testing and maximal evidence per game launch.
- [GitHub Actions CI](.github/workflows/ci.yml): documentation checks, offline tests, and a Windows build/package pipeline that activates once actual source exists.

**Project versions are Git tags, not necessarily public releases.** GitHub Actions builds/tests the Windows observer and offline core, and packages the diagnostic ASI separately. A green diagnostic build is **not** proof of game support. A separate Puppet Native ABI v1 plugin is now compiled and lifecycle-tested offline (not packaged for player use).

## Legal and licensing

**Copyright (c) 2026 Vyachean. All rights reserved.**

This repository is **publicly visible but not open source**. No general permission is granted to copy, modify, redistribute, republish, or incorporate its original material into other projects. GitHub's Terms of Service still permit viewing and forking through GitHub's service. See [LICENSE.md](LICENSE.md) for the full notice and third-party exclusions.

**Assassin's Creed**, its names, game content and related trademarks belong to their respective owners. This project's copyright notice applies only to original project material in which rights can legally subsist.

### Read-only diagnostic host alternatives

GitHub Actions builds two **alternative** non-playable diagnostic bundles: standalone ASI or Puppet Native v1 plugin. The Puppet Native plugin reuses the same worker, has no game hooks and needs no additional proxy DLL. Its ABI and report are checked in a synthetic Windows process; neither variant is verified inside Resynced. See [Puppet Native integration](docs/PUPPET_NATIVE.md). No game test is requested for this infrastructure alone.
