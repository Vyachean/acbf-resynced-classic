# Architecture: offline resolver and native action bridge contract; game integration not implemented

> The **pure C++ decision core, sampled-input edge tracker, and gated native action-bridge contract** are now implemented and tested offline. **No ACBFR input API, hook target, memory layout, action invocation, compatible loader, or runtime integration has been verified.**

## Feature-scoped integration safety gate

**Issue #18 requires a complete audit of every action affected by the specific enabled feature, not closure of the full-system catalog before any game-facing research.**
See [CONTROL_SYSTEM_AUDIT.md](CONTROL_SYSTEM_AUDIT.md) and its [coverage ledger](../research/control_system_ledger.json). Current `resolve_b()` assumes a one-step choice between counter and dodge; original AC4 instead documents **hold B to block, tap B to enter a counter phase, then a second face-button choice**. Thus the function cannot be treated as a faithful AC4 replica. `resolve_x()` and `resolve_a()` similarly encode **provisional goals**, not recovered runtime priority. **Do not use the existing offline bridge as authorization to suppress any physical input.**

No consuming game hook or remapping runtime shall be enabled until every action sharing its input **in supported contexts**, press/hold/release, modifiers, target states and exact game version is analyzed and proven safe. Unsupported and unknown contexts must pass through without consuming game input. The global multi-surface audit remains necessary for a full classic profile.

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
 Fail-closed action bridge (offline-only)
          |
          v
 Native input claim + game action adapter (unknown)
          |
          v
     Game behavior
```

Supporting modules: per-build compatibility policy, configuration, sanitized diagnostics, and emergency disable/fallback.

## Responsibilities

**Input adapter (not implemented):** collect button state through a **verified** game-compatible input path, then pass discrete transitions to the edge tracker. Input APIs and interception mode remain to be researched. Do not assume XInput/DirectInput is sufficient or that event suppression is safe.

**Context provider:** expose only game state that has been validated by evidence, with explicit unknown/unavailable values. Hypothetical inputs may include combat state, action eligibility, targeted enemy, and traversal state, but none of these are assumed to be readable today.

**Resolver (implemented):** [`resolve`](../include/acbf/classic_control.h) is a pure function of abstract button input, physical edge and an explicit context/eligibility snapshot. For X it proposes combat takedown before ordinary attack, stealth assassination before ordinary attack. For B it proposes counter before dodge as an **old, likely incorrect placeholder**. AC4's actual documented B behavior includes guard held and a multi-stage counter submenu; any one-step resolver must be redesigned after the audit. These are **design priorities**, not reverse-engineered game facts. No action is emitted when relevant eligibility is unknown. [`EdgeTracker`](../include/acbf/classic_control.h) provides one press edge per sampled down/up cycle.

**Decision states:** `Selected` = candidate intent, **not** executed; `NoAction` = complete negative eligibility for the handled context; `PassThrough` = leave native game handling alone; `Unresolved` = insufficient trusted data, **do not install/enable a consuming override**; `Ignored` = not a new press. Input-release propagation and safety remain the adapter's responsibility. No game adapter exists yet.

**Action bridge (implemented OFFLINE ONLY):** [`XActionBridge`](../include/acbf/action_bridge.h) requires six independent integration proofs, a supported X intent, and a verified pre-action eligibility snapshot before proposing any interception. A prepared ticket does **not** call the game or suppress input. Only an independently confirmed physical input claim allows a single dispatch; failed or ambiguous delivery must not replay X (to avoid duplicate actions). The original X hold/release cycle remains claimed until the release is observed. **Only a synthetic fake NativeActionSink exists**; no game API, hook or engine-specific offsets have been implemented.

**Game action adapter (not implemented):** identify the native game's eligibility/state and a verified action dispatcher. Puppet Native's public plugin ABI provides game base/log/config but **no combat action API**. Do not treat PostEvent sounds as pre-action eligibility.

**Compatibility guard:** identify the specific supported build before enabling invasive operations. Fail closed for mismatched/unknown versions.

**Diagnostics:** produce a small event timeline (input -> context availability -> decision -> execution result). Must be configurable and avoid dumping raw memory, keys, identifiers, or personal data.

## Offline testability

The pure module is built with C++17/CMake in GitHub Actions on Windows. `ctest` exercises synthetic combat, stealth, B/A decisions, unknown eligibility, button holds, releases, repeat suppression, menus, swimming and naval passthrough. **A green test does not imply game integration or safety inside Resynced.**

The Windows build packages **diagnostic-only** ASI/Puppet Native host alternatives, not the native action bridge. The action bridge links with the pure core; its fake native sink is compiled only into its CTest executable. No playable mod is currently packaged.

## Physical-button collision prerequisite

[Input impact analyzer](INPUT_IMPACT.md) derives every documented X/B/A/RT
and modifier use across both games from the central control ledger, including
known-unbound actions that may overlap. A candidate in
`XActionBridge` must never be considered installable merely because
it owns a matching input edge or synthetic eligibility snapshot. The
future game-facing adapter must first identify and preserve every
cross-mode shared action, press/hold/double-tap/chord and claimed release.
The analysis is **read-only** and denies every override until proven.

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

## Action bridge safety contract

All gates default to false: exact executable profile, verified input interception, ability to consume the original input, release handling, verified pre-action context, and verified native command dispatch. Only X is eligible for interception; B/A/RT are untouched. The off-game test suite validates each missing gate, unknown contexts, native pass-through, pending ticket cancellation, no dispatch without input ownership, one submission per claimed press, claimed held/release events, and no double replay after rejected or ambiguous submission. **These synthetic tests do not establish any in-game functionality.**
