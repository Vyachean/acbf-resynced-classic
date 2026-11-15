# Diagnostic and validation protocol

The maintainer cannot directly run the user's game. Manual tests should therefore be **short, deterministic and diagnostically useful**.

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
| B-01 | Counter/parry situation | B resolves to verified classic-style defense behavior. |
| B-02 | Dodge/evasion situation | B resolves safely and consistently; no unverified automatic choice. |
| A-01 | Guard-break eligible | A invokes the correct action without an extra modifier, where supported. |
| RT-01 | High-profile movement | Running and traversal intent matches verified reference behavior. |
| M-01 | Mod enabled/disabled and alternate loader setup | Compatibility/fallback is explicit; no silent input loss. |

These are **proposed test cases**, not evidence of current working behavior. Add exact setup, video evidence or logs as each case is independently validated.

## Suggested efficient workflow

1. Reproduce in an unmodified target build and document expected reference outcome.
2. Run one instrumented scenario; collect a minimal sanitized log.
3. Fix the observed mismatch and add an automated resolver regression test.
4. Ask for another user test only when integration-specific behavior cannot be determined from recorded evidence.

## Issue-report format

Include scenario ID, exact versions, steps (maximum reproducible case), expected behavior, actual behavior, whether removing the mod helps, and the smallest relevant **redacted** log excerpt.

## Release gate

No release should claim a scenario passes unless it was tested on the corresponding game version with the stated controller and loader conditions.
