# Combat eligibility: enemy classes and version-specific state boundaries

**Status: RESEARCH_ONLY_INCOMPLETE / NO GAMEPLAY HOOKS.**
Evidence register: [combat_conditions.json](../research/combat_conditions.json). Parent gate: [whole-system audit](CONTROL_SYSTEM_AUDIT.md), issue #18. See also [original AC4](https://primagames.com/eguides/assassins-creed-iv-black-flag-eguide/reference-analysis/enemy-archetypes), [official Resynced combat design](https://www.ubisoft.com/en-gb/game/assassins-creed/news/1A8XkbnvONciecEZm9my1C/assassins-creed-black-flag-resynced-combat-system-breakdown) and Ubisoft [TU 1.0.6 notes](https://www.ubisoft.com/en-gb/game/assassins-creed/news/5c72WnN2ot90ii8d9Le1Wt/ac-black-flag-resynced-title-update-106-patch-notes).

## Why a button-only or generic combat mode is unsafe

The original AC4 does **not** execute the same counter or kill on every opponent. The 2013 Prima guide distinguishes **Soldier, Scout, Gunner, Brute and Captain**, with different immunity/weakness to attack, counter kill, counter throw, disarm, special/tool counter, guard-break and kill streak. It explicitly describes a **Brute's counter input becoming a dodge**, not ordinary counter selection. A Captain has a distinct **hold-to-parry** stun response.

The guide uses *Weak*, *Immune*, and a dash in its matchup table. These values are preserved as **"weak", "immune", "unspecified"**, **NOT** converted into a game function's \`can_attack\` or \`can_counter\` boolean. Enemy placement, animations, weapons and other conditions remain necessary.

### Original AC4 enemy matchup (period guide; not engine evidence)

| Move | Soldier | Scout | Gunner | Brute | Captain |
| --- | --- | --- | --- | --- | --- |
| Ordinary Attack | Weak | Immune | Weak | Immune | Immune |
| Combo Kill | Weak | Weak | Weak | Immune | Immune |
| Kill Streak | Weak | Weak | Weak | Immune | Immune |
| Break Defense | Weak | Weak | Weak | Weak | Immune |
| Counter Kill / Throw / Disarm / Tool | Weak | Weak | Weak | Immune | Immune |
| Ranged Attack | Weak | Weak | Weak | Weak | Weak |
| Parry | Unspecified | Unspecified | Unspecified | Immune | Weak |

**Source conflict deliberately unresolved:** in the same Prima article, the Captain prose suggests a grounded assassination after repeated Break Defense, while the summary table marks Captain **immune** to Break Defense. We retain both claims and do not choose one based on intuition. Also, Brute's defensive counter input may select a dodge, but the exact original engine action selection and timing window remain unverified.

This is why the old provisional \`resolve_b()\` ("counter if available, otherwise dodge") cannot be treated as a reproduction of AC4. Even after the *physical input* is known, the action also depends on the **enemy archetype and attack class**.

## Resynced has different enemy conditions, not equivalent class semantics

Ubisoft's official [2026 combat overview](https://www.ubisoft.com/en-gb/game/assassins-creed/news/1A8XkbnvONciecEZm9my1C/assassins-creed-black-flag-resynced-combat-system-breakdown) describes independent opportunities:

- **Takedown** after an enemy's defense bar is broken. Perfect Parry may expose direct Takedown and chain opportunity.
- **Advanced Brute/Captain** can interrupt or deny a chain. A pistol quick shot can break their defense between chain targets.
- **Wall Takedown** after a kick into a wall/railing; **Ground Takedown** after a sweep/explosion knocks down an enemy.
- **Brute unparryable attack** needs a different defense (dodge); Perfect Dodge may enable an enhanced RB attack.

None of this establishes a callable native Takedown command in Resynced **1.0.6**. Do **not** reuse the original AC4 archetype table as Resynced combat eligibility: shared names do not imply identical gameplay systems.

## Target TU 1.0.6: official action/state boundary fixes

Ubisoft published [TU1.0.6 on August 3, 2026, deployed August 4](https://www.ubisoft.com/en-gb/game/assassins-creed/news/5c72WnN2ot90ii8d9Le1Wt/ac-black-flag-resynced-title-update-106-patch-notes). The following changes are especially relevant to an input hook:

| Fixed in TU1.0.6 (per patch notes) | State/action boundary | Hook implication |
| --- | --- | --- |
| Cannot parkour with **RT held after remapping RT to Sprint** | Modifier/gesture → traversal | Do not assume remap and parkour can always coexist regardless of patch |
| Quick Use Rope Dart lost after being **interrupted or hanging an enemy** | Combat interrupt → tool readiness | Distinguish interruption/cancellation from subsequent ability availability |
| Unable to assassinate **drunk pirates in smoke after entering combat** | Combat → stealth target eligibility | Context cannot be reduced to "in combat" boolean |
| **Takedown during vaulting over an enemy** could occur unintentionally | Parkour vault ↔ finishing action | A Takedown being selectable is not always *correct* or safe |
| Leg Sweep failed on **attentive/suspicious enemies** | Enemy awareness → ability availability | Enemy suspicion state matters |
| Hood toggle failed during **Leap of Faith** | Movement animation ↔ appearance control | Do not globally suppress D-pad/utility inputs during traversal |
| Bodies on benches after load could be looted/carried | Save/load ↔ interaction availability | Interactable state may change across save/load |

These are **official claimed fixes**, not validated runtime traces, call addresses, or proof of perfect behavior on the user's installed executable. The exact binary SHA-256 is not yet supplied.

### Avoid accidental later-version assumptions

- **TU1.0.7 (September 1):** Ubisoft separately fixed a Diabolist-pistol/suspicious burning-target Takedown, assassination from an investigated haystack, midair assassination when another actor killed the target, and lost crouch state after Photo Mode. None can be assumed fixed in the owner's **1.0.6**.
- **TU1.0.8 (October 27):** Ubisoft added new original-inspired Takedown animations and Nightmare difficulty (shorter parry window, no white glint), plus fixes to walk-toggle/underwater dive, corner-cover aiming and sleeping targets in bushes. Added **animations do not imply classic input priority**. This differs from the target build in both control outcomes and gameplay conditions.

Official sources: [TU1.0.7](https://www.ubisoft.com/de-de/game/assassins-creed/news/24bBcwoOoYNZ2gt2yi3HdH/ac-black-flag-resynced-title-update-107-patch-notes), [TU1.0.8](https://www.ubisoft.com/en-us/game/assassins-creed/news/5Z1RxwznM6CcrkSEVoibmh/ac-black-flag-resynced-title-update-108-patch-notes).

## Required future action-eligibility contract

Any candidate classic control override must first know or explicitly classify **UNKNOWN** for all relevant inputs:

1. **Player mode:** grounded combat, stealth, vault/climb, swimming/ship/menu; selected weapon and ongoing animation, interruption/cancel state.
2. **Target state:** enemy class/attack type, defense/vulnerability, awareness, distance, target lock, wall/ground geometry.
3. **Input gesture:** button, Press/Hold/Double Tap/Toggle, modifier state, physical release and stage-specific subsequent input.
4. **Build/mode:** exact executable fingerprint, control profile, difficulty and title-update behavior.
5. **Action dispatch:** independently verified native availability **and** action invocation, with safe, reversible physical input ownership.

If any necessary field is unknown, **the consuming override stays disabled**. The already completed offline bridge and synthetic tests do not satisfy these conditions.

The game-user's previous shared-button Attack+Takedown test produced only ordinary Attack. **Do not ask to repeat the same mapping.** Any future in-game test requires a materially different and justified hypothesis and an integrated, short session.

**This audit does not lift the gameplay-hook freeze.** It narrows the unknowns; it does not prove the unknown game engine contracts.
