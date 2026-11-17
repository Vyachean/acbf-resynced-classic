# X-001: native remapping feasibility, one game session

**Status: DEFERRED — the maintainer already observed Attack winning over Takedown with a shared physical button. On 2026-11-17, the owner clarified that native Resynced Takedown uses Y and displays a Y prompt when available. Do not repeat the same binding; the earlier shared-button result still lacks a record of which prompt was visible after remapping.**
**Target:** the owner's **existing** Resynced 1.0.6 installation and Xbox-style controller. Do **not** update to 1.0.8 or install a new game build for this test.

**Historical question (now deferred):** does Resynced itself prioritize a valid **Takedown** over **Attack** when both are assigned to the **same X press**, if conflicting crouch/sprint controls are configured as in a publicly demonstrated profile?

A July 19, 2026 video describes working `Attack = X` and `Takedown = X`, but the owner previously observed X only swinging in 1.0.6. This is an **unresolved contradiction**, not an established bug fix.

## Source and reproducibility

- Public controller configuration and description, July 19: https://www.youtube.com/watch?v=q4r1P2Q2bsE
- Ubisoft control customization and input-mode options: https://www.ubisoft.com/en-gb/help/assassins-creed-black-flag-resynced/gameplay/article/customising-controls-in-assassin-s-creed-black-flag-resynced/000116769
- Ubisoft defines takedown availability after breaking enemy defense or successful Perfect Parry: https://www.ubisoft.com/en-gb/game/assassins-creed/news/1A8XkbnvONciecEZm9my1C/assassins-creed-black-flag-resynced-combat-system-breakdown
- Ubisoft's separately reported assassination multiple-press issue ACBFR-641, **version 1.0.2** (not proof for 1.0.6): https://www.ubisoft.com/en-us/game/assassins-creed/black-flag-resynced/bug-reporter/issues/ACBFR-641

The video description confirms the **button names** but does not explicitly establish that each action used input mode `Press`. Setting both to Press is this project's deliberately controlled **test condition**. Do not represent it as an observed video setting.

## Scope and operator budget

- **One game launch**, no game restart; change the controller mappings through the game's own menu **during** the same session.
- **Do not install our diagnostic ASI or Puppet DLL** for this first test; they cannot observe the outcome of a Takedown and could introduce a loader confounder.
- Do not replace the existing `version.dll`, alter Puppet Native, recompile code, install debugging tools, export game assets, or upload an executable.
- Use an ordinary accessible combat encounter, without relying on specific quest content. Never force a game event that is unavailable in the current save.
- Existing user result `X only attacks` is **already recorded**. Only repeat the shared binding if the complete profile below is materially different from the settings already tested. If the profile was identical, report the existing observation and **skip the launch**.

## Controlled procedure

### A. Baseline: native Y Takedown (owner-reported)

The owner reports that a ready Takedown displays a Y-button prompt and that Resynced performs it through Y. Treat this as the baseline for the current profile; do not spend another session re-proving native Y unless a later observation contradicts it. This establishes a visible, pre-action cue for the player. It does not establish a programmatically readable eligibility signal or prove that the cue remained visible during the earlier shared-X test.

Owner-reported native mapping:

| Action | Button | Input method |
| --- | --- | --- |
| Attack | RB | Press |
| Takedown | Y | Press |

This baseline is already reported by the owner; do not repeat it unless a later observation contradicts it.

### B. Shared X: match the published control layout, not merely two key assignments

In the **same launch**, use these assignments (keep the remaining unrelated bindings as they were, avoiding other bare-X conflicts):

| Action | Button | Input method |
| --- | --- | --- |
| Crouch | L3 | Press |
| Sprint | RB | Hold |
| Attack | X | **Press** (controlled experimental choice) |
| Takedown | X | **Press** (controlled experimental choice) |
| Eagle Vision | Y | Hold **or** Double Tap, if conflicting with the profile |

The source's profile does not claim to put **Assassinate on X**. Do not turn this into an assassination test or claim success for X-03. Avoid LT/RT modifiers during the X test.

1. Without a Takedown prompt, press **X once**: did Edward attack normally?
2. Obtain a new **visible** Takedown prompt and note the **displayed button** (X, Y, another, or no prompt).
3. While it remains visible, press **X once** (not held); note `TAKEDOWN`, `ATTACK_ONLY`, `NO_ACTION`, `OTHER`, or `PROMPT_DISAPPEARED`.
4. If X worked in only one Takedown context (e.g. Perfect Parry but not depleted defense), report **PARTIAL**, not PASS.

### C. Optional, same launch only if B clearly failed

If the shared X press produced ordinary attacks instead of a valid Takedown **and** the game permits choosing a different input method for Takedown, temporarily set `Takedown = X / Hold` while retaining `Attack = X / Press`. With a new visible prompt, check whether **holding X** invokes the Takedown.

This is **only a degraded ergonomic fallback**, not the target classic single-press behavior. If the UI cannot set the input method or there is no prompt, record `NOT_TESTED`. Do not spend time on other speculative button combinations.

## Report back — a single line, no files needed

Copy the simplest applicable example and replace the values:

`X-001 | build=1.0.6 | baseline_Y=SUCCESS | shared_X_attack=YES | shared_X_prompt=X | shared_X_takedown=ATTACK_ONLY | hold_X=NOT_TESTED | profile_same_as_prior=NO`

Other valid labels: `baseline_Y=BASELINE_BLOCKED`, `shared_X_takedown=TAKEDOWN/PARTIAL/NO_ACTION/PROMPT_DISAPPEARED/NOT_TESTED`, `profile_same_as_prior=YES`.

A short ordinary-language result is acceptable. **No logs or screenshots are required**; attach a short video only if convenient. The observer cannot infer Takedown success from physical button edges, so user-observed behavior remains essential. The owner-reported default Y prompt is not evidence that it was visible after the earlier shared-X remapping.

## Decision after one experiment

| Finding | Engineering decision |
| --- | --- |
| Baseline Y fails/no prompt | **INCONCLUSIVE.** First identify native Takedown availability / game input problem. No claim of X binding bug. |
| Baseline Y works, X attack works, X Takedown works | **NATIVE_REMAP_POSSIBLE** for the tested finisher contexts. Don't write an invasive action hook for this feature. Separately investigate assassination and B/A/RT. |
| Baseline Y works, shared X attacks despite a stable visible Takedown prompt | **NATIVE_PRIORITY_CONFLICT** on the reported build/profile. Game-facing **eligibility and command dispatch** become the real blocker for exact AC4 controls; don't keep adding generic XInput logs. |
| Same X press works only for some prompts | **PARTIAL_NATIVE_CONTEXT**. Specify the contexts that fail; no blanket compatibility claim. |
| Hold X works, Press X fails | **GESTURE_WORKAROUND_ONLY**: may be a short-term optional profile, but is not classic controls. |
| Unclear prompt, interrupted action or different game version | **INCONCLUSIVE**. Do not convert uncertain observations to a PASS/FAIL or request repeated tests by default. |

## Next responsible implementation work

If `NATIVE_PRIORITY_CONFLICT` is confirmed, research a **version-verified** way to invoke the native Takedown command independently of the bound button, with access to pre-action eligibility. Puppet Native's published `PN_Api` v1 does **not** expose either action. No fabricated offsets or blind X→Y injection. If no source-backed seam exists, create a single *purposeful* integration probe rather than another generic diagnostic release.

If `NATIVE_REMAP_POSSIBLE`, move directly to validating contextual Assassinate and the next control priorities; only build code for specific failures.

**User interaction is unavoidable for this experiment because the assistant cannot execute the locally installed game.** It should not be delegated to the user until the experiment is already scoped and documented.
