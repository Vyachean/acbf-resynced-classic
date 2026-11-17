# Project scope

## Product vision

**Make Assassin's Creed Black Flag Resynced feel closer to the original Assassin's Creed IV: Black Flag**, initially by restoring the original's more natural, context-sensitive control philosophy.

Success is measured by **player-visible outcomes**, not whether the physical key mapping looks similar on a settings screen.

## Intended users and reference

- Primary reference: **original AC4** on an Xbox-style controller.
- Target: **Black Flag Resynced**, treated as a separate game build and integration surface.
- First focus: in-combat and nearby-context actions; later traversal and other mechanics.
- Community x64 ASI loading is documented, but this project's loader compatibility, supported build and game event hooks are **unverified** until tested.

## Initial target behaviors

1. **X — context-appropriate combat action**: normal hit when appropriate; finishing move when available; contextual assassination in the applicable state. Avoid the observed failure where putting attack and finisher on the same game binding causes the finisher to disappear.
2. **B — combat defense and world interaction**: preserve Resynced parry/evasion on B during combat; use B for a clearly prompted noncombat Y interaction such as opening a chest. Resolve the prompt/context first and leave unknown contexts native.
3. **A — defense break**: direct control without the Resynced modifier chord, where technically feasible.
4. **RT — high-profile movement**: align running/parkour intent with AC4 while avoiding unintended dives or wrong contextual actions, when a reliable input or movement seam exists.
5. Later candidate work: weapon switching, contextual action priorities, animation or combat pacing, traversal tuning.

The ordering and exact rules are **working requirements**, subject to reference testing and safe feasibility review.

## Non-goals for initial releases

- Rebuilding all of AC4's combat mechanics or combat AI.
- Replacing Resynced's engine, animations, or progression systems.
- Copying Ubisoft game code, textures, animations, assets, or redistributing any game's files.
- Providing modified or cracked executables, bypassing DRM, or distributing game binaries.
- Claiming compatibility with every game version, input stack, or mod combination.
- Multiplayer/online cheating features, protected-process bypasses, or anti-cheat evasion.

## Constraints and design rules

- Each invasive change must have a safe disable/fallback path.
- Unrecognized game versions must fail closed rather than applying unverified hooks.
- User controls and logging should make behaviors diagnosable without frequent manual tests.
- Every gameplay change should be individually configurable where practical.
- Before implementing hooks, establish whether a supported mod API is available. If not, treat binary integration as an independent risk item.
- Third-party code can be used only under compatible explicit rights and appropriate notices.

## Definition of first playable milestone

A test build in a **specified** ACBFR version demonstrably resolves X to attack or finishing action by context, without breaking either action in the documented test scenarios; it can be disabled without modifying installed game files, and its logs contain enough information to reproduce unexpected dispatch. Follow-up B behavior must keep combat defense native while routing explicitly prompted noncombat interactions.

This milestone does **not** mean combat is fully restored to AC4.
