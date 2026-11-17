# Original AC4 combat reference — offline executable policy

**Scope:** the original 2013 Assassin's Creed IV **as documented in the reference/audit**, not Resynced gameplay. This project's `acbf::legacy` C++ model is a **synthetic, deterministic policy test**, not a replacement controller handler, a combat physics simulation, or an embedded game patch. Extracted as a bounded source-only increment from historical [PR #31](https://github.com/Vyachean/acbf-resynced-classic/pull/31).

## Modeled action selection

| Explicit synthetic context | Input gesture | Returned candidate |
| --- | --- | --- |
| Neutral combat | Hold B | Guard |
| Incoming melee / ordinary soldier | Tap B | Enter counter follow-up |
| Incoming melee / Brute | Tap B | Evade |
| Incoming melee / Captain or unclassified enemy | Tap B | **Unresolved** (conflicting/insufficient source semantics) |
| Incoming melee / two warning sources | Tap B | Double counter |
| Counter follow-up phase | Tap X / B / A / Y | Kill / Throw / Disarm-or-stun / selected tool |
| Incoming gunfire / nearby shield target | Tap A | Human shield |
| Incoming gunfire / unknown target | Tap A | **Unresolved** |
| Unknown phase | Any | **Unresolved** |
| Other known contexts | Unrecognized gestures | Native pass-through |

The enum labels represent **documented candidate behavior**, not exact original-engine inputs. Source provenance and contradictory Captain cases remain in [combat audit](CONTROL_SYSTEM_AUDIT.md) and [combat conditions](COMBAT_CONDITIONS.md).

**Crucial omissions:** no frame or millisecond timing, action duration, target ownership, actual state detection, engine access, input suppression, native action dispatch, counter damage, event/animation correctness or Resynced build compatibility. The model receives an explicitly supplied phase and target and outputs one candidate.

## Architecture and testing

- Independent `include/acbf/legacy_combat_model.h` + `src/legacy_combat_model.cpp` target `acbf_legacy_combat_model`.
- `tests/legacy_combat_model_tests.cpp`: pure deterministic CTest `legacy_combat_model_offline`.
- `tools/combat_arena/main.cpp`: console demonstration/sandbox using the **same resolver**; its `--demo` scenario is tested as `combat_arena_demo` in Windows GitHub Actions. The console “arena” has **no in-game NPCs or 3D runtime**.
- The project's normative [workflow](WORKFLOW.md) keeps compilation/testing in **GitHub Actions**; no owner local build is needed or requested.

The [feature-scoped gate #18](https://github.com/Vyachean/acbf-resynced-classic/issues/18) is independent of this synthetic model. A consuming game input hook requires verified **target build**, **pre-action semantic state**, **affected-context control inventory**, **native action dispatch**, release/cancel handling, **passthrough**, off-by-default activation and rollback. **No such hook is enabled by adding these files.** The global control audit remains ongoing for a global classic-control profile.
