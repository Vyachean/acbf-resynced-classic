# Why current community mods do not restore classic Black Flag gameplay

**Reviewed:** 2026-11-16  
**Status:** public-source survey only. No listed mod was installed or tested by this project.

The requested outcome is to make *Black Flag Resynced* behave as much as possible like the original *Assassin's Creed IV: Black Flag*. The reviewed community mods are not a solution to that goal: each changes one narrow feature, and none documents the integrated original control and gameplay rules.

## Survey

| Mod | What its page says it changes | Why that is not an AC4 restoration |
| --- | --- | --- |
| [Classic Movement - Walk By Default Smooth Sprint](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/408) | Default walking pace; sprint via RT or L3; configurable movement scale. | Walking pace alone does not restore AC4's low/high-profile mode, contextual actions, social-stealth behavior, or full parkour rules. |
| [Walk By Default for PlayStation Controllers](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/514) | Walk-by-default movement plus DualSense/controller changes and required custom bindings. | A controller-specific movement/input layer does not reproduce AC4's integrated control model and adds binding conflicts to audit. |
| [Easy Parry](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617) | Extends parry opportunities and changes the condition for double counter-kills. | Changing a timing window and double-kill trigger does not restore AC4's hold-B guard, tap-B counter entry, and follow-up selection. |
| [Lethal Combat and Realistic Difficulty Overhaul](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/451) | Advertises one-hit lethality. Its page includes reports that defensive actions also kill because they register as damage. | One-hit damage scaling is a different combat model and has reported side effects; it is not AC4's counter-led combat. |
| [Unrestricted Combat Finishers](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635) | Author describes a Resynced 1.0.8 Forge-data change affecting some finishing animations after normal lethal attacks. | Changing animation conditions does not establish original pre-action finisher selection or fix Attack/Takedown input arbitration. The author restricts reuse. |

These pages are useful only as examples of isolated techniques and as evidence that the community has explored some runtime and data-driven changes. Their descriptions do not provide a complete design or implementation that can be adopted for this project. Do not install, bundle, or recommend them as a classic gameplay preset. Do not copy their code, binaries, or data.

Ubisoft's [Title Update 1.0.8 notes](https://www.ubisoft.com/en-us/game/assassins-creed/news/5Z1RxwznM6CcrkSEVoibmh/ac-black-flag-resynced-title-update-108-patch-notes) say the game adds Takedown animation variants inspired by the original and a Nightmare difficulty option. Those features add content and difficulty; they do not document restoration of AC4's counter-selection control flow. Ubisoft's [control customization guide](https://www.ubisoft.com/en-au/help/assassins-creed-black-flag-resynced/gameplay/article/customising-controls-in-assassins-creed-black-flag-resynced/000116769?origin=middle) confirms native control customization but does not specify priority when multiple actions share an input.

## Project direction

A closer result requires a project-owned, integrated compatibility layer for the specific Resynced build. It must reproduce verified AC4 behavior across its control surfaces and preserve Resynced-only actions when the mode is unknown. It cannot be designed safely as a few global button remaps.

The repository's mandatory [whole-control-system audit](CONTROL_SYSTEM_AUDIT.md) remains incomplete and blocks new game-facing input hooks, suppression, action dispatch, and playable claims. Continue source-based research and offline design only until each planned intervention is justified. Do not treat the current C++ resolver as a verified game model.

The original AC4 counter sequence is a key design requirement: guard by holding B; tap B during an incoming melee warning to enter a counter; then use a separate face-button choice for kill, throw, disarm/stun, or selected tool. Other documented exceptions and transitions are catalogued in the [control reference](CONTROL_REFERENCE.md) and [machine-readable audit ledger](../research/control_system_ledger.json). These guides establish documented behavior, not exact engine timing or Resynced action dispatch.

## Evidence limits

- Nexus pages are author descriptions, not independent compatibility or gameplay tests.
- No source code, binary, save, or game archive from the listed mods was downloaded or inspected.
- The local Resynced directory has not been authenticated as a clean official store build. No game was launched or changed.
- No conclusion is made here about whether a custom implementation is technically feasible or which internal hooks it would require.

## Source pages

- [Classic Movement, Nexus mod 408](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/408) — viewed 2026-11-16.
- [PlayStation Walk mod, Nexus mod 514](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/514) — viewed 2026-11-16.
- [Easy Parry, Nexus mod 617](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617) — viewed 2026-11-16.
- [Lethal Combat, Nexus mod 451](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/451) — viewed 2026-11-16.
- [Unrestricted Combat Finishers, Nexus mod 635](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635) — viewed 2026-11-16.
- [Ubisoft TU 1.0.8 notes](https://www.ubisoft.com/en-us/game/assassins-creed/news/5Z1RxwznM6CcrkSEVoibmh/ac-black-flag-resynced-title-update-108-patch-notes) — viewed 2026-11-16.
- [Ubisoft control customization guide](https://www.ubisoft.com/en-au/help/assassins-creed-black-flag-resynced/gameplay/article/customising-controls-in-assassins-creed-black-flag-resynced/000116769?origin=middle) — viewed 2026-11-16.
