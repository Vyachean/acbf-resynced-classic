# Diagnostic and validation protocol

The maintainer cannot directly run the user's game. Manual tests should therefore be **short, deterministic and diagnostically useful**.

## Required controls audit gate

Issue [#18](https://github.com/Vyachean/acbf-resynced-classic/issues/18) blocks any new game-facing controller override. Before approving a single new behavior, validate the [complete multi-surface behavior inventory](CONTROL_SYSTEM_AUDIT.md), including timed counter follow-ups, profile modifiers, aiming/tool chords, ship/water/menu states and input-release safety. Unknown modes must pass through. This test document's X/B/A scenarios describe **goals, not faithful recovered AC4 action logic**.

## Before testing

Record:

- Game name and **exact** executable version/build (do not infer from an installer label).
- Mod release/commit, mod-loader type and version, controller model, input mode.
- Other enabled mods and launch environment.
- Whether the game starts and whether controls work with this project **disabled**.

Never attach proprietary game files, executables, memory dumps or unredacted files with personal paths and identifiers.

## Reference scenarios (to be verified against original AC4)

| ID | Scenario | Desired observation |
| --- | --- | --- |
| X-01 | Enemy in combat, ordinary hit available | X yields normal attack once. |
| X-02 | Enemy ready for a finisher | X performs the applicable finishing action; does not degrade to repeated empty swings. |
| X-03 | Stealth/context assassination eligible | X selects the contextual action when that is the intended AC4 behavior. |
| X-04 | No valid target / unavailable attack | Safe no-op or documented fallback; no unintended action. |
| X-05 | Rapid transitions among X-01/02/03 | No double activation or stuck key state. |
| X-06 | Combat Takedown prompt and nearby world-interaction prompt | X chooses the combat Takedown; it never opens the chest or other world interaction. |
| B-01 | Counter/parry situation | B resolves to verified classic-style defense behavior. |
| B-02 | Dodge/evasion situation | B resolves safely and consistently; no unverified automatic choice. |
| B-03 | Noncombat world interaction prompt (e.g. chest) | B invokes the prompted Y interaction; in combat, B remains parry/evasion. |
| Y-01 | Combat Takedown prompt | Y still performs the Takedown after world interactions are reassigned; unrelated Y actions remain intact. |
| Y-02 | Noncombat world interaction prompt | Where the game supports separate action bindings, Y no longer opens the world object and B does. |
| A-01 | Guard-break eligible | A invokes the correct action without an extra modifier, where supported. |
| RT-01 | High-profile movement | Running and traversal intent matches verified reference behavior. |
| M-01 | Mod enabled/disabled and alternate loader setup | Compatibility/fallback is explicit; no silent input loss. |

These are **proposed test cases**, not evidence of current working behavior. Add exact setup, video evidence or logs as each case is independently validated.

## Immediate single-session native control experiment

The owner reports that native Resynced Takedown uses Y and displays a Y prompt when available. The previous shared-button trial made Attack win, but did not record whether the prompt remained visible after remapping. [X-001](X_INPUT_EXPERIMENT.md) is deferred; do not repeat the same profile. A future materially different remapping-only trial would need to check whether the Takedown prompt is visible under that mapping and whether X performs the finisher. No diagnostic package can establish the in-game outcome; only observed gameplay can.

## Suggested efficient workflow

1. Reproduce in an unmodified target build and document expected reference outcome.
2. Run one instrumented scenario; collect a minimal sanitized log.
3. Fix the observed mismatch and add an automated resolver regression test.
4. Ask for another user test only when integration-specific behavior cannot be determined from recorded evidence.

## Issue-report format

Include scenario ID, exact versions, steps (maximum reproducible case), expected behavior, actual behavior, whether removing the mod helps, and the smallest relevant **redacted** log excerpt.

## Release gate

No release should claim a scenario passes unless it was tested on the corresponding game version with the stated controller and loader conditions.

## Game-launch budget

**Zero launches by default**: exhaust public sources, CI builds, synthetic input traces and unit tests first. If in-game behavior cannot be established, create a single **GitHub Actions-built diagnostic package** covering the maximum set of unresolved scenarios in one launch. It should automatically save one sanitized report and require nothing from the player beyond copying mod files, launching the game and performing interactions that cannot be safely automated.

No in-game feature is considered verified by synthetic tests alone. See [Single-session test strategy](TEST_STRATEGY.md).

## Offline core status

The C++ candidate resolver (see [source](../include/acbf/classic_control.h)) and synthetic Windows tests cover these scenario **policies**, press/release debouncing, unresolved eligibility, and passthrough contexts. These tests **do not validate actual Resynced action availability, native input handling, or in-game results**.

Any integration receiving `Status::Unresolved` must fail closed: do not activate an input-consuming override with unknown eligibility. See [architecture](ARCHITECTURE.md).
