# Controller behavior reference: AC4 vs Resynced

**Research date:** 2026-11-15. **Status:** source-backed baseline, **not** in-game validation of this mod.

This document compares Xbox-style inputs and separates three things:
1. what the original game's documentation specifies;
2. what the Resynced release/default keymap specifies;
3. what the project **wants to restore** but has not yet proven feasible.

**Important:** a face button sharing several gameplay actions in AC4 does *not* prove that the same priority system exists inside Resynced. Likewise, a third-party guide or player report does not establish engine behavior for every title update.

## Source-backed reference matrix

| Context / intent | Original AC4 (documented) | Resynced default, July 2026 control reference | Project target / remaining uncertainty |
| --- | --- | --- | --- |
| Ordinary melee attack | **X**: melee weapon / primary attack | **RB**, hold RB for heavy attack | **X** should perform exactly one ordinary attack when that is the relevant action. |
| Stealth assassination | **X**: assassinate | **Hold Y**: assassination | **X** should select assassination when eligible, without issuing a stray normal swing. Exact eligibility/API unknown. |
| Combat finisher / takedown | User's AC4 reference: **X** can resolve a finishing action; not separately established by a button table | **Y**: takedown | **X** should choose finisher when eligible; determine what the game calls microkill/softkill/takedown. Priority unknown. |
| Counter / parry | **B**: counter | **LB**: parry | **B** should invoke the intended classic-style defense only when it is available. |
| Dodge / roll | Original AC4 dodge's exact button/state semantics **not established by these sources** | **B**, double-B roll | Combined B counter/evade is a **requested design goal**, not yet a verified description of the original's decision tree. |
| Defense break | **A**: break defense in combat; also movement/jump context | **RT + A** is a **Sweep** shortcut in launch mapping, not necessarily equivalent to AC4 guard break | Investigate which Resynced move, if any, reproduces AC4 guard break. Do not map A to Sweep without testing semantics. |
| High-profile traversal | **RT**: high profile / free run | **L3**: sprint; **A**: jump/parkour up; **B**: parkour down | **RT** as a high-profile modifier is desired, not simply RT = L3; avoid breaking A/B parkour or quick abilities. |
| Interact | **B**: interact outside combat | **Y**: interact | Interaction vs defense priority needs a separate context policy. |
| Equipment/tool selection | Original AC4 D-pad: quick inventory | Multiple Resynced D-pad and LT+face-button tool mappings | Defer until core controls, avoid collisions with heal, tools and naval mode. |

**References**
- Original AC4 Xbox e-manual: https://dlassets-ssl.xboxlive.com/public/content/0044aef3-bd7e-4478-8782-6191546dcd9a/GameManual/240e30c4-b40a-4f93-9ae2-1c52ded445f7/en-US/index.html
- AC4 Xbox control comparison: https://gamefaqs.gamespot.com/xbox360/705969-assassins-creed-iv-black-flag/faqs/68114
- Resynced controller guide (July 9, 2026 launch mappings): https://www.shacknews.com/article/149927/assassins-creed-black-flag-resynced-controls (or https://canary.shacknews.com/article/149927/assassins-creed-black-flag-resynced-controls)
- Official remapping capabilities: https://www.ubisoft.com/en-gb/help/assassins-creed-black-flag-resynced/gameplay/article/customising-controls-in-assassin-s-creed-black-flag-resynced/000116769
- Official combat/takedown design: https://www.ubisoft.com/en-gb/game/assassins-creed/news/1A8XkbnvONciecEZm9my1C/assassins-creed-black-flag-resynced-combat-system-breakdown

## Version-sensitive differences

The default controls table was published at release. It is **not** evidence that all controls or bugs are unchanged in 1.0.6–1.0.8:

- **TU 1.0.6** (August 4, 2026) explicitly fixed one failure to parkour while holding RT after changing sprint to RT. Official patch: https://www.ubisoft.com/en-us/news/ignt.62343/ac-
- **TU 1.0.8** added/changed finisher variations and difficulty options. This affects the observed action palette and must not be confused with control dispatch. Official summary: https://news.ubisoft.com/en-us/article/5mmVVkAwsyCnArBk4c5j3E/assassins-creed-black-flag-resynced-latest-update-new-game-nightmare-difficulty-new-content
- Consequently, game build is a **dimension of every test**. A result for 1.0.8 must not automatically be used as proof for 1.0.6, nor vice versa.

## Community observations (conflicting / unverified)

Evidence suggesting the built-in remapper is not always sufficient:
- July 2026 classic-layout reports describe priority collisions, especially RT + A/B movement while abilities share face buttons: https://www.reddit.com/r/ACBFResynced/comments/1usa1zu/best_legacy_control_scheme_for_black_flag_resynced/
- A player guide recommends **not** assigning stealth assassination to X because a regular attack can fire instead: https://prodigygamers.com/2026/07/10/assassins-creed-black-flag-resynced-how-to-enable-classic-controls/
- Earlier user testing for this project reports **X attack + finisher** fails to choose the finisher reliably.

Counter-evidence:
- At least one community comment reports **attack and takedown on the same button** working after a parry/stun: https://www.reddit.com/r/ACBFResynced/comments/1usa1zu/best_legacy_control_scheme_for_black_flag_resynced/
- A July 2026 demonstration also configures X for both attack and takedown: https://www.youtube.com/watch?v=q4r1P2Q2bsE

**Conclusion:** there is evidence of **context-/version-dependent conflicts**, not proof that the shared binding universally works or universally fails. Do **not** turn this contradiction into a supposedly known engine priority. Our first test must distinguish **normal swing**, **eligible takedown**, **eligible assassination**, and **input mode (press/hold)** with exact settings.

## Ordered requirements for the first resolver

1. Ensure no unintended command is emitted for a physical button transition.
2. Resolve **eligible finishing action vs ordinary attack** without triggering both.
3. Resolve **eligible assassination vs ordinary attack** without attacking bystanders.
4. Preserve normal attacking when special actions are unavailable.
5. Be explicit about **unknown context**; do not pretend that a missing eligibility signal means false.
6. Do not infer the original's B dodge/parry priority from a binding table alone.
7. Keep ship, swimming, menu and on-foot contexts separate.

## Non-game validation and next experiment

Machine-readable cases live in [reference_scenarios.json](../research/reference_scenarios.json), intentionally recording **desired outputs and unresolved conditions**, not fake passing runtime tests.

**No game launch requested.** Before any session, investigate native action dispatch and eligibility in public code and design a single CI-built diagnostic package covering X and other unknowns. See [integration findings](INTEGRATION_FINDINGS.md) and [testing strategy](TEST_STRATEGY.md).
