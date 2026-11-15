# Single-session game testing strategy

## Primary constraint: avoid game launches

Do not run ACBFR or original AC4 merely because development is uncertain. Perform **all possible testing without either game** first.

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

## Gate before requesting any in-game test

A diagnostic request must have all of the following:

- Specific technical question(s) that other evidence cannot settle.
- Exact target build/version, loader assumptions and fallback behavior; unknown builds must not trigger invasive hooks.
- Passes on GitHub Actions for all applicable synthetic and Windows tests.
- One **downloadable Actions artifact** containing the mod files, license, a small readme, and any required default config. No third-party proprietary dependencies.
- A single session test matrix covering as many unresolved questions as can safely be observed.
- Explicit stop conditions and a minimal-report format; avoid collecting excessive raw events.
- Installation and cleanup instructions limited to copying/removing files and running the game, when supported by the integration.
- Known limitations and unsafe/unsupported combinations called out in advance.

If any gate is not met, **do not ask the player to start the game**.

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
