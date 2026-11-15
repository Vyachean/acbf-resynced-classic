# Architecture: offline resolver implemented; game integration not implemented

> The **pure C++ decision core and sampled-input edge tracker** are now implemented and tested offline. **No ACBFR input API, hook target, memory layout, action invocation, compatible loader, or runtime integration has been verified.**

## Design objective

Translate **player intent** into **one appropriate game action** based on verified context, without relying on several conflicting game-level bindings to the same physical button.

```text
Controller / input events
          |
          v
 Input adapter (unknown)
          |
          v
 Snapshot of confirmed player context
          |
          v
 Pure contextual action resolver
          |
          v
 Game action adapter (unknown)
          |
          v
     Game behavior
```

Supporting modules: per-build compatibility policy, configuration, sanitized diagnostics, and emergency disable/fallback.

## Responsibilities

**Input adapter (not implemented):** collect button state through a **verified** game-compatible input path, then pass discrete transitions to the edge tracker. Input APIs and interception mode remain to be researched. Do not assume XInput/DirectInput is sufficient or that event suppression is safe.

**Context provider:** expose only game state that has been validated by evidence, with explicit unknown/unavailable values. Hypothetical inputs may include combat state, action eligibility, targeted enemy, and traversal state, but none of these are assumed to be readable today.

**Resolver (implemented):** [`resolve`](../include/acbf/classic_control.h) is a pure function of abstract button input, physical edge and an explicit context/eligibility snapshot. For X it proposes combat takedown before ordinary attack, stealth assassination before ordinary attack. For B it proposes available counter before dodge. These are **design priorities**, not reverse-engineered game facts. No action is emitted when relevant eligibility is unknown. [`EdgeTracker`](../include/acbf/classic_control.h) provides one press edge per sampled down/up cycle.

**Decision states:** `Selected` = candidate intent, **not** executed; `NoAction` = complete negative eligibility for the handled context; `PassThrough` = leave native game handling alone; `Unresolved` = insufficient trusted data, **do not install/enable a consuming override**; `Ignored` = not a new press. Input-release propagation and safety remain the adapter's responsibility. No game adapter exists yet.

**Game action adapter:** perform an action through the safest validated integration available, without fake key combinations when action-level dispatch is necessary. Integration feasibility is a milestone, not an assumption.

**Compatibility guard:** identify the specific supported build before enabling invasive operations. Fail closed for mismatched/unknown versions.

**Diagnostics:** produce a small event timeline (input -> context availability -> decision -> execution result). Must be configurable and avoid dumping raw memory, keys, identifiers, or personal data.

## Offline testability

The pure module is built with C++17/CMake in GitHub Actions on Windows. `ctest` exercises synthetic combat, stealth, B/A decisions, unknown eligibility, button holds, releases, repeat suppression, menus, swimming and naval passthrough. **A green test does not imply game integration or safety inside Resynced.**

There is no `install()` target and no packaging manifest: **the CI must not offer the static library as an installable game mod.** The CI packaging stage requires an explicit future `packaging/installable.json` plus a staged `.asi` file.

## Risk register

| Risk | Impact | Mitigation / research |
| --- | --- | --- |
| Only remapping is available | Contextual behavior may be impossible | Prove access to action dispatch before committing to the implementation. |
| Context or action state unavailable | Wrong or missed action | Use explicit unknown states; log and safely fall back. |
| ASI loader/mod collisions | Startup crash or silent failure | Verify loader path, isolated tests, documented compatibility matrix. |
| Version-specific addresses or layouts | Crashes / corruption after updates | Version checks, no unverified offsets, safe disable path. |
| Conflicting input repeats or action priority | Doubles, dropped finishers, stuck actions | Deterministic resolver tests and transition scenarios. |
| Closed-source game / copyright | Inadmissible redistribution | Ship original mod code only; no third-party game files. |

## Decision gates

1. **Can inputs be observed safely?**
2. **Can meaningful action eligibility be observed?**
3. **Can an action be triggered independently of its menu key binding?**
4. **Can hooks be limited to verified game builds and disabled cleanly?**

If answers 2–3 are no, do not advertise AC4-like contextual behavior as implemented. Re-evaluate scope rather than disguising key remapping as a finished solution.
