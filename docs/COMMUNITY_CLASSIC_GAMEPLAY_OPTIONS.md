# Community options for a more classic Black Flag feel

**Reviewed:** 2026-11-16  
**Status:** public-source survey only. No listed mod was installed or tested by this project.

This note asks which existing community work can move *Black Flag Resynced* toward the original *Assassin's Creed IV: Black Flag*, and which apparent shortcuts change the gameplay in a different direction. Mod-page descriptions are author claims, not compatibility or quality verification.

## Best-aligned candidate: walk by default

The [Classic Movement - Walk By Default Smooth Sprint](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/408) page describes a default walking pace, sprint by holding RT or L3, a configurable walk scale, and an in-game toggle. Its published v2.1 notes list both 32-bit storefront builds and 64-bit DX12 Resynced support.

This is the closest ready-made match found for the original's deliberate low-profile walking and explicit high-profile sprint intent. The page does not establish that every original high-profile action, social-stealth rule, or parkour transition is restored. Treat it as a locomotion experiment, not an AC4 control conversion. Its author prohibits conversion, re-upload, or reuse of its files without permission.

**Candidate for a later, clean-build trial:** this mod alone, with its stated supported binary and a reversible backup. Before trial, identify the exact game build and inspect loader/proxy coexistence. Do not stack another movement or input hook on top.

## Other movement option: controller-specific

[Walk By Default Instead Of Jog - For PlayStation Controllers](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/514) advertises walk-by-default movement and DualSense features, but its instructions require several in-game remaps, including crouch to L3, sprint to R2 (hold), and lock target unassigned. It also bundles a controller input DLL and haptics.

This is a PlayStation-specific alternative, not an additional layer to combine with mod 408. Its mandatory remaps can collide with other actions. Do not use it for an Xbox-oriented profile without a separate controller and conflict audit.

## Combat and stealth: similar labels do not mean classic behavior

| Mod | What its page says | Fit to the original AC4 goal |
| --- | --- | --- |
| [Easy Parry](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617) | Extends parry opportunities and permits a double counter-kill without the original simultaneous-warning condition. | Convenience/easy-mode option. The changed double-kill condition differs from AC4's documented input and enemy-warning behavior; do not call it a restoration. |
| [Lethal Combat and Realistic Difficulty Overhaul](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/451) | Advertises one-hit kills and high lethality. Its page's posts include reports that defensive actions also kill because the game registers them as damage. | Not a close AC4 combat match and has reported side effects. Do not bundle or recommend as a classic combat preset. |
| [Unrestricted Combat Finishers](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635) | Its author describes a Resynced 1.0.8 Forge-data change that allows certain finishing animations after lethal normal attacks. | Animation trigger change, not proof of AC4's pre-action finisher selection or of Attack/Takedown input arbitration. The author restricts reuse. |

Ubisoft's [Title Update 1.0.8 notes](https://www.ubisoft.com/en-us/game/assassins-creed/news/5Z1RxwznM6CcrkSEVoibmh/ac-black-flag-resynced-title-update-108-patch-notes) add Takedown animation variants inspired by the original and Nightmare difficulty, including a shorter parry window and removed attack glint. These options alter the available experience but do not restore AC4's counter-selection flow. Ubisoft's [control customization guide](https://www.ubisoft.com/en-au/help/assassins-creed-black-flag-resynced/gameplay/article/customising-controls-in-assassins-creed-black-flag-resynced/000116769?origin=middle) confirms native control customization; it does not specify action priority for conflicting bindings.

## Project decision

1. Keep the repository's whole-control-system audit gate in force. No new input hook, input suppression, action dispatch, or playable claim follows from this mod survey.
2. If a clean, authenticated target build becomes available, evaluate the walk-by-default mod as an isolated, reversible locomotion candidate before building a custom movement hook.
3. Keep combat changes separate from controller mapping. The available community combat mods change difficulty or animation conditions; none of the reviewed descriptions demonstrates original AC4 counter → follow-up behavior or resolves the already-observed shared Attack/Takedown collision.
4. Do not combine ASI, proxy-DLL, controller, or Forge-data mods until their exact game versions, load order, and overlapping hooks are known. No compatibility claim is made here.

## Evidence limits and next evidence

- Nexus pages describe author-stated features and requirements. No source code, binary, or save was downloaded or inspected for this survey.
- The game build currently available in the workstation directory was not authenticated as an official clean release; no game was launched or changed.
- The next safe validation requires an authenticated store build, exact title-update identification, a backup of relevant config/save data, and a controlled test of one candidate at a time. This is separate from validating this project's own code.
- Original AC4 control and counter semantics remain documented in [the control reference](CONTROL_REFERENCE.md) and [the whole-system audit](CONTROL_SYSTEM_AUDIT.md). The audit is still incomplete.

## Source pages

- [Classic Movement, Nexus mod 408](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/408) — viewed 2026-11-16.
- [PlayStation Walk mod, Nexus mod 514](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/514) — viewed 2026-11-16.
- [Easy Parry, Nexus mod 617](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617) — viewed 2026-11-16.
- [Lethal Combat, Nexus mod 451](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/451) — viewed 2026-11-16.
- [Unrestricted Combat Finishers, Nexus mod 635](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/635) — viewed 2026-11-16.
- [Ubisoft TU 1.0.8 notes](https://www.ubisoft.com/en-us/game/assassins-creed/news/5Z1RxwznM6CcrkSEVoibmh/ac-black-flag-resynced-title-update-108-patch-notes) — viewed 2026-11-16.
- [Ubisoft control customization guide](https://www.ubisoft.com/en-au/help/assassins-creed-black-flag-resynced/gameplay/article/customising-controls-in-assassins-creed-black-flag-resynced/000116769?origin=middle) — viewed 2026-11-16.
