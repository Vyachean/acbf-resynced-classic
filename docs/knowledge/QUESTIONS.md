# Research gaps and evidence acquisition backlog (two-game knowledge base)

**Date:** 2026-11-17. This is an **evidence backlog, not a programming issue backlog**. Answers must update the corresponding [AC4](ORIGINAL_AC4.md), [Resynced](RESYNCED.md), [Anvil](ANVIL_DATA.md), [runtime](RUNTIME.md), [testing](TESTING.md) and [source register](SOURCES.md) documents; keep test/implementation decisions in actual GitHub issues.

## Priority order / why

| Priority | Question | Known evidence | Smallest useful next proof and stop condition |
| --- | --- | --- | --- |
| P0 | **BFR 1.0.6: which resource controls parry tolerance, defense/perfect parry eligibility?** | [Easy Parry #617](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/617) B author claim; AnvilToolkit v50 typed patch tool A | Inspect independent original **1.0.6 game-owned** `.data` type/graph; name field and prefetch parent. If undecodable, record exact type/schema blocker and stop. No borrowed mod payload |
| P0 | **BFR 1.0.6: which resource governs enemy attack frequency/double takedown?** | [Assassination Impact #587](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/587) B (1.0.8); original AC4 [double counters #1029](https://www.nexusmods.com/assassinscreedivblackflag/mods/1029) B | Map **separate original game** resource graphs, describe differences, do **not** infer same field/offset across games |
| P0 | **BFR 1.0.6: can a finisher be eligible and invoked independently of the same physical X binding?** | Input audit, built-in Y vs shared X behavior, HapticsBridge events but **no pre-X eligibility** | Exact-build semantic hook/source for context + distinct logical native action. If unavailable, precise `COMMAND_BLOCKED`; do not keep logging XInput |
| P0 | **BFR 1.0.6: can a new additive combat patch load safely?** | [AnvilToolkit author v1.3.7+ new patch guide](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki/Creating-a-New-Patch-Forge-for-Assassin's-Creed-Shadows-or-later) A | Independently map installed actual patch head, prefetch graph and one small named game resource. CI synthetic round-trip and one later authorized game test; don't overwrite old patch |
| P0 | **Loader compatibility: what actually boots the maintainer's 1.0.6 + existing mods?** | Project Puppet Native ABI and original ASI builds offline only; reports of version.dll collision | Read-only module/proxy inventory and exact hashes in **one combined** future session; no speculative installs |
| P0 | **Can one real parry test be scored semantically?** | HapticsBridge traversal hooks and post-event clues; game has unparryable adaptive enemies | Locate genuine enemy attack start and game-accepted result; otherwise `BLOCKED_SEMANTIC_ANCHOR`. No fake pass from physical input/haptic |
| P1 | **NPC styles in BFR: which resource references combat archetype?** | [Padre #602](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/602), [Heroes of Jackdaw #569](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/569) B | Trace game-authored `EntityBuilder`/style/combat records, distinguish companion and player actors |
| P1 | **Can BFR spawn, position, freeze and command NPCs at runtime?** | None verified; original AC4Coop spawned **AI-disabled puppet** | Separately establish read-only enumeration → position → native AI task → attack/hit oracle; each step proof with exact build. Do not create “Arena API” without seam |
| P1 | **BFR combat animation speed hook?** | [GoatedACBlackFlag #19](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/19) B live-reload settings | Verify actual config behavior/version source, separate game time scale vs animation-only, test side effects; not proof of classic X semantics |
| P1 | **Original AC4 action dispatch vs playback: where are eligibility and hit resolution?** | AC4Coop `SetTargetAction` with AI disabled; Prima combat rules | Research own fingerprinted AC4 action control flow; don't reuse AC4Coop addresses without byte gate |
| P1 | **Original AC4 exact FORGE timing and archetype fields?** | [No counter slow motion #871](https://www.nexusmods.com/assassinscreedivblackflag/mods/871) names bootstrap data; [Combat Changes #765](https://www.nexusmods.com/assassinscreedivblackflag/mods/765) B | Independent original `DataPC.forge` resource parse / compare; small named graph with evidence |
| P1 | **Resynced patch/data mod conflicts** | Two or more mods can override same `patch_02.forge`; no existing project conflict resolver | Resource ID / override parent inventory, feature interaction table, safe rollback; no merge of proprietary payloads |
| P2 | **BFR 1.0.8 animation/UI workflow** | [AC Mods Launcher](https://github.com/nickythesmithy/ac-mods-launcher) A README, [Overhauled #659](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/659) B | Determine author package license/format and independent animation reference. Not a dependency on owner's 1.0.6 |
| P2 | **Mod-manager compatibility** | Vortex BFR/original extensions and ReForger claim/README mismatch | Compare actual supported versions and output paths; never use launcher uninstall blindly |
| P2 | **Original AC4 high-FPS fixes and camera latency** | [PCGW](https://www.pcgamingwiki.com/wiki/Assassin%27s_Creed_IV:_Black_Flag) C; [AC4 Modular Patch #1031](https://www.nexusmods.com/assassinscreedivblackflag/mods/1031) B limited exact Steam hash | Verify modern driver/game build and isolated performance effect; unrelated to BFR combat |
| P2 | **Legacy tools worth retaining** | ACViewer/Resorep/old Steam tutorials and missing REA URL | Mark historical/404/deprecated, verify only if a unique feature can't be supplied by a maintained tool |

## Findings format for future researcher / Codex

```markdown
### YYYY-MM-DD — Short finding title
- Game / target: AC4 x86 or BFR x64, actual build/executable if known
- Problem / research question:
- Source(s): original author's URL, date, source location or file path
- Observed fact / author claim / hypothesis: distinguish explicitly
- Evidence grade: A / B / C / U / E (+ E test environment/report if available)
- Game data resource or native function: only when verified; do not invent API names
- What this does NOT establish:
- License / rights / technical risk:
- Smallest independent test / stop condition:
- Follow-up: [doc + issue/PR], or 'research only'
```

## Search expansion method (repeatable but bounded)

1. Revisit official **AnvilToolkit** changelog and wiki for newer resource types and AC4/BFR formats.
2. Review newly published Nexus mods under **Gameplay / Utilities / Modders Resources** for **both games**, prioritizing those with descriptive file paths and source; annotate actual version availability.
3. Search public GitHub code (not just repo names) for **`ACBlackFlag.exe`**, **`AC4BFSP.exe`**, **`DataPC_boot_patch`**, **`EntityBuilder`**, **`CharacterActionSet`**, **`SetTargetAction`**, **`BhvAssassin`**, **`PN_Init`**, **`ActionComponent`**, and FORGE serialization. Do not assume strings uniquely identify architecture.
4. Compare actual claims against **developer updates**, game-specific patch notes, author source, and dated public tests. Separate source levels and recognize fake trainer/SEO repositories as high risk.
5. Update [source register](SOURCES.md), corresponding topical page, and implementation issue **only when the evidence changes a technical decision**. Preserve prior conflicting claims; don't erase provenance.
6. If a source is inaccessible (404/private/login), mark **U** with date and seek another primary source. Don't present an indexed excerpt as validated source code.

**Do not pursue an endless research treadmill.** Reserve a bounded offline review for each P0 unknown; then give project lead `GO` / `BLOCKED`, exact evidence, and one next experiment. Knowledge collection can continue, but it must not block an evidence-backed playable vertical slice.

## Project implementation anchors

- [#30 first playable combat vertical slice](https://github.com/Vyachean/acbf-resynced-classic/issues/30)
- [#33 minimum semantic combat test harness](https://github.com/Vyachean/acbf-resynced-classic/issues/33)
- [#18 feature-scoped input safety gate](https://github.com/Vyachean/acbf-resynced-classic/issues/18)
- [#10 action eligibility and native dispatch research](https://github.com/Vyachean/acbf-resynced-classic/issues/10)
- [#28 original AC4 read-only observation](https://github.com/Vyachean/acbf-resynced-classic/issues/28)
- [Project workflow](../WORKFLOW.md)
