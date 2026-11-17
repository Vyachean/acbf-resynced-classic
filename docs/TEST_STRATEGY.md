# Single-session game testing strategy

## Feature-scoped safety gate before game-facing integration

[Control-system audit #18](CONTROL_SYSTEM_AUDIT.md) is a reference inventory, not a global prerequisite. A feature must prove its exact build, semantic state, source-input ownership, command routing, affected collisions, full press/release cycle, native pass-through elsewhere, off-by-default activation and tested rollback. CI tests of a synthetic bridge do not prove in-game behavior. Unrelated surfaces do not block work when their inputs remain untouched. Do not repeat the already reported shared Attack/Takedown remap.

## Game launches: one question, one focused session

Do static analysis and synthetic checks first. Launch only when a specific, unresolved feature question can be answered in one focused session. The user's current task explicitly authorizes a Resynced vertical-slice attempt; the local executable previously exited at ANIMUS-3-00029 before single-player, so do not repeat an identical launch or automate authentication.

| Layer | Runs in GitHub Actions? | Needs the game? |
| --- | --- | --- |
| Research review and behavior specification | No execution required | No |
| Parsing/building configuration and policy files | Yes | No |
| Deterministic candidate context -> action resolver cases | **Yes: C++/CTest in Windows Actions** | No |
| Synthetic controller event streams, held/released inputs, double-activation detection | Yes, once implemented | No |
| Windows offline core build/test | **Yes: CMake/CTest in GitHub Actions** | No |
| Read-only ASI diagnostics + synthetic DLL host and scripted XInput press/release cycles | **Yes: Windows CMake/CTest and distinct diagnostic artifact** | No; game compatibility remains unknown |
| Game integration and verified semantic action dispatch | **Not available** | Yes for final gameplay verification |
| Loader compatibility and actual player-state observation | Only with lawful game execution environment | Usually yes |
| Verifying in-game attack/finisher/parkour outcomes | No credible automated substitute is currently known | Yes |

The existence of an input/ASI mod **does not** prove we can automate scene creation, NPC states or action dispatch. Do not pretend that an offline simulator proves in-game behavior.

## Exception: one-session built-in control comparison (X-001)

A **remapping-only**, user-observed behavior comparison is different from a mod diagnostic. It requires **no instrumented build, compiler, loader, package or telemetry**. [X-001](X_INPUT_EXPERIMENT.md) records the owner's existing native Y Takedown and visible Y prompt as the baseline; it does not ask for that check again. The earlier shared-X result remains unresolved only as to whether a Takedown prompt was visible after remapping. Do not repeat the same profile; the game's own action/animation is the outcome, and an XInput report cannot observe it.

This exception is preferred before invasive hooks because a successful built-in mapping would remove the need to develop a fragile game-action dispatcher. It does **not** certify the project's plugin, 1.0.8 compatibility, or other AC4 control contexts.

## Gate before requesting a mod-based in-game diagnostic

A **mod-based diagnostic request** must have all of the following:

- Specific technical question(s) that other evidence cannot settle.
- Exact target build/version, loader assumptions and fallback behavior; unknown builds must not trigger invasive hooks.
- Passes on GitHub Actions for all applicable synthetic and Windows tests.
- One **downloadable Actions artifact** containing the mod files, license, a small readme, and any required default config. No third-party proprietary dependencies.
- A single session test matrix covering as many unresolved questions as can safely be observed.
- Explicit stop conditions and a minimal-report format; avoid collecting excessive raw events.
- Installation and cleanup instructions limited to copying/removing files and running the game, when supported by the integration.
- Known limitations and unsafe/unsupported combinations called out in advance.

If any gate is not met, **do not ask the player to start the game for that mod diagnostic**. This does not prohibit the separate, no-mod remapping experiment X-001.

## Controlled combat validation (user-authorized, TU 1.0.6)

The diagnostic-bundle gate above applies to requesting a new **instrumented project mod**. It must not block a narrowly scoped validation of an already-built, reversible data candidate or a user-approved, isolated test aid. The first combat pass must minimize enemy pressure and be repeatable:

1. Use an offline checkpoint or encounter already present in the game. Prefer the game's Forgiving combat difficulty if available; Ubisoft describes it as reducing attack frequency and the number of enemies that attack Edward when surrounded. This is a pressure reduction, not invulnerability.
2. Use one ordinary soldier and provoke a clearly visible **non-red** melee attack. Red-glow attacks are explicitly un-parryable and require dodge, so they are invalid parry samples. Keep other enemies out of range where the scene permits.
3. Compare the same input and attack under baseline and candidate data, recording attack cue, Q press timing, parry animation/result, health change and death/desynchronization. Reset to the same checkpoint between trials. Stop if additional enemies engage or health becomes critical; label incomplete trials `NOT_RUN`.
4. If the selected encounter cannot provide a repeatable single-enemy sample, do not improvise by editing save/world files or guessing runtime addresses. A third-party trainer may be considered only after its exact file, supported TU, integrity and cleanup are verified; do not bypass a login/download gate. Two candidate Nexus listings were found for 1.0.6: [Cheats Trainer #541](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/541) has an explicitly listed 1.0.6 file and advertises F4 God Mode; [+64 Trainer #616](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/616) has a file labeled 1.0.6–1.0.7 and advertises invulnerability. Neither file has been downloaded, inspected, or tested here, and the Nexus download was guest-gated. Do not treat their claims as verified support or use alternate mirrors. Treat any claimed invulnerability as unverified until its effect and rollback are observed.

This procedure is intentionally smaller than the X-input feature matrix: it answers only whether the reversible parry-window candidate changes a valid parry outcome. It does not validate X/Takedown routing or the full AC4 combat model. Forgiving difficulty alone does not make an uncontrolled checkpoint a safe test arena.

## Intended one-launch diagnostic bundle

The following is a **target design**, not fully implemented. The separate [read-only ASI](DIAGNOSTICS.md) currently covers only executable fingerprint, local mod inventory, XInput button edges and explicit `NOT_OBSERVABLE` game semantics. It is **not yet sufficient** for a requested game session:

1. **Preflight / startup:** automatically record project build ID, detected game version, loaded integration component status, controller capabilities, and relevant compatibility flags. Do not dump file paths, tokens or memory.
2. **Passive discovery:** automatically observe whether expected input and state signals are accessible; log hook/feature availability and clear errors.
3. **Synthetic self-check:** execute a local, isolated decision-engine test suite without emitting game actions, where feasible.
4. **One-session interactive matrix:** for cases that require gameplay, capture a timestamped event timeline as the player performs a short ordered set of X-01..X-05, B-01/02, A-01 and RT-01 situations. Avoid repeat launches, editor commands and changing profiles between cases.
5. **Safety checks:** track repeated presses, hold/release transitions, unknown contexts, conflicting actions, missed finishers, stuck input state, and clean disable/fallback.
6. **Automatic summary:** produce one compact UTF-8 report with build, supported feature checklist, case-by-case result/status, essential anonymized traces, and a checklist of what could not be evaluated.
7. **Exit:** close/flush the report automatically at normal game shutdown (and use best-effort crash-safe incremental writing where feasible). Never silently upload telemetry.

Only mark a scenario **passed** after its required observable signal has actually been captured. Any unsupported event source is `NOT_OBSERVABLE`; an unperformed player scenario is `NOT_RUN`, never `PASS`.

## Maximum automation, minimum operator work

**Ideal player procedure:** download Actions archive → copy documented mod files → start game once → play documented sequence only if automation cannot create required contexts → attach the single generated diagnostic report → remove files if desired.

The project must not require the player to compile, execute standalone scripts, enter console commands, measure frame timing manually, gather screenshots of file trees, or decipher logs.

Do not require manual input at all for tests that can run offline in CI. Do not make automated gameplay claims before verifying real game interfaces.

## Diagnostic data and privacy

- Default to local-only logs. Never exfiltrate data or require authentication.
- Use a fixed schema and identifiers, minimal event metadata, no keystrokes outside configured controller action probes.
- Exclude usernames, arbitrary process memory, secrets, installation paths and game assets.
- Automatically cap log size and rate; support explicit opt-out/uninstall.
- A report is evidence only for its **recorded build, loader, conditions and scenarios**.

## Approval for an extra game launch

Record the missing observation, why previous logs are inadequate, which automatic probes will be added, and why the new bundle can answer multiple remaining questions. Prefer one improved bundle over repeated one-off patches.

See [Testing protocol](TESTING.md) and [Development policy](WORKFLOW.md).

### Host consolidation

The project now builds **two alternative diagnostic delivery hosts** using the same worker: standalone ASI and Puppet Native v1 DLL. The Puppet variant avoids a second `version.dll` when the existing Puppet Native setup is actually functional. Both are tested with synthetic input in Windows Actions. Neither can yet observe action eligibility, so no separate game-start request is justified.

### Combat input trace completeness

Before an eventual one-session game check, the reporter now observes Xbox X/Y/A/B, shoulder LB/RB and thresholded triggers LT/RT. This explicitly covers Resynced's default attack/takedown buttons and the proposed shared-X layout. No keyboard keys, unrelated controller inputs or gameplay state are captured. GitHub Actions verifies **16 press/release edges** through a synthetic provider in both ASI and Puppet Native hosts. **Button events alone cannot prove a takedown was eligible or executed.**

### Research pivot: owner already confirmed the dual-binding collision

The owner tested binding Attack and Takedown to one button; ordinary Attack won. On 2026-11-17, the owner clarified that native Resynced shows a Y prompt when a Takedown is available and uses Y to perform it. The prompt state during the earlier shared-button test is still unknown. X-001 is **deferred**, not a requirement to repeat that binding. Extending raw XInput logs does not resolve prompt visibility or native command selection. A new [offline two-phase X bridge](../include/acbf/action_bridge.h) has unit-tested safety gates, but **no corresponding game APIs exist in this repo**.

### Required physical-button impact query for any future gameplay PR

For each physical input a feature consumes, use [the offline impact report](INPUT_IMPACT.md) to enumerate collisions in the feature's supported context. Review actions sharing the suppressed event there. Keep every other mode and ambiguous state native-pass-through. The report is impact inventory only; it does not grant or deny implementation by itself. The global classic profile still needs the full relevant catalog, but an isolated slice does not.

