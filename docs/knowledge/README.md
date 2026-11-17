# Modding knowledge base — Assassin's Creed IV (2013) & Black Flag Resynced (2026)

**Maintained research corpus, not an SDK.** Started 2026-11-17. Covers original **AC4 Black Flag (Windows x86)** and **Black Flag Resynced (Windows x64)**. The games share names and some design concepts but have **different executables, engine generations, layouts and data pipelines**. **Do not port offsets, hooks, loaders, patch FORGEs or assets between games.**

This collection is the **entry point** for project agents. The repository's original `docs/*.md` documents remain the authoritative project-specific implementation state; this knowledge base indexes external technology, reusable methods, primary sources and unverified leads.

## Navigate by purpose

| Page | What it contains |
| --- | --- |
| [Original AC4](ORIGINAL_AC4.md) | Executable/integration facts, native player/action/puppet research, gameplay data mods, debug cameras, known compatibility constraints |
| [Black Flag Resynced](RESYNCED.md) | Resynced tools, gameplay-mod evidence, loader/plugin ecosystems, version split and where to search for native action/AI resources |
| [Anvil / data resources](ANVIL_DATA.md) | `.forge`, `.data`, prefetch, override graph, compatible tools and safe data-patch research workflow |
| [Runtime / native tooling](RUNTIME.md) | ASI, plugin ABIs, hooking patterns, game-state readback, safe frame-callback principles, diagnostics |
| [Testing / automation](TESTING.md) | Automated bot feasibility, virtual input caveats, game-semantic oracle, single-session approach, reproducibility |
| [Sanitized 2026-11-15 local static evidence](LOCAL_STATIC_EVIDENCE_2026-11-15.md) | Bounded original x86 input-to-command Ghidra path, v27/v50 Forge reader checkpoints, actor-template schema blockers; provenance from historical PR #27 | 
| [Source register](SOURCES.md) / [machine index](../../research/modding_sources.json) | Curated annotated bibliography with 90 structured source records, URLs, game scope and evidence tier |
| [Unknowns and research backlog](QUESTIONS.md) | Missing interfaces and bounded research questions, rather than assertions |
| [Broader 2026 Resynced ecosystem survey](../MODDING_ECOSYSTEM.md) | Detailed dated survey of resource mods, patch-forge tool and mod manager behavior |
| [Detailed runtime test harness investigation](../RUNTIME_TEST_HARNESS_RESEARCH.md) | Candidate automation stack, HapticsBridge / AC4Coop source findings, limits |
| [Original vs Resynced control audit](../CONTROL_SYSTEM_AUDIT.md) | Project-specific mechanics and coverage ledger |
| [Integration evidence](../INTEGRATION_FINDINGS.md) | BFR loader, source licenses, game action-dispatch blockers |
| [Puppet Native ABI](../PUPPET_NATIVE.md) | Concrete v1 plugin API and synthetic project host status |

## How to read evidence

**A — primary source inspected:** actual upstream source code, author-maintained wiki/README or official developer docs. It confirms *what the source says or implements*, not successful compatibility on our exact game build.

**B — author feature claim:** mod release notes or descriptions. May suggest a useful game resource/hook, **but not a verified engine schema**, nor permission to copy a payload.

**C — secondary/community report:** independently unverified anecdote, guide, issue or test report.

**U — unresolved:** URL unavailable, missing source, contradicting claims or no reproduction. Do not promote a hypothesis to an implementation fact.

**E — project empirical evidence:** only when this repository actually ran the relevant test on a fingerprinted executable, retained a sanitized result and documented conditions. **No current E-grade proof of a playable BFR combat fix or scripted NPC control**. Offline CI proves our code, not the game.

**Always record:** observation date; precise game title and architecture; version/store or executable fingerprint; link/author; claim; evidence grade; source license/permissions; contradictions; and how to independently validate. Do not cite search snippets as executable facts when an original source is available.

## Current cross-game boundary

| Dimension | Original AC4 (2013) | Resynced (2026) |
| --- | --- | --- |
| Game process | `AC4BFSP.exe` for single-player, **32-bit** | `ACBlackFlag.exe` / some distributions `ACBlackFlag_Plus.exe`, **64-bit** |
| Known author research | AC4Coop, AC4Tools, ScriptEngine camera; some source and concrete function concepts | HapticsBridge, Puppet Native, new BFR data/patch tools; many undocumented functions |
| Main asset route | Anvil/Scimitar FORGE and typed `.data` resources; replacement of modified `DataPC.forge` common in author mods | Newer FORGE v50 and layered `DataPC_boot_patch_NN.forge` prefetch chain; **AnvilToolkit 1.3.7+ additive patch forge** |
| Loader | **x86** ASI loader / plugin; older guides may require D3D11 x86 proxies | **x64** ASI loader / Puppet Native plugins; existing `version.dll` collisions are a real risk |
| Closest NPC-control evidence | AC4Coop spawns animation-driven `Human` puppet, AI disabled | No verified public NPC spawn/forced combat-action or arena scripting API discovered |
| Working action-command interface for classic X | Original AC4 defines reference behavior, but hooking semantics build-specific | Unknown; gamepad event recording and post-event sound are insufficient |
| Biggest hazard | Assuming one old Steam/Uplay executable is byte-identical to another; destructive FORGE replacement | Reusing original AC4 x86 classes/offsets or foreign title-update FORGE/ASI patterns |

## Coverage policy and agent workflow

1. Start from this index, then read the **smallest relevant page and its primary source**. Do not bulk-load every document or paste third-party code in prompts.
2. Search official developer guides, AnvilToolkit wiki/releases, public GitHub implementations, Nexus original-AC4/BFR categories, Nexus comments, curated forums/Steam/PCGamingWiki/ModDB as needed. Record fresh findings in the **proper game page** and [SOURCES.md](SOURCES.md).
3. Cross-link detailed observations to existing `docs/*.md` and relevant issue. Consolidate duplicates; **keep this KB project-agnostic**, implementation state lives in project docs.
4. Every new claim gets a source, version scope, evidence tag and *non-proof*. No guessed resource IDs, source-derived offsets copied from unlicensed mods, executable patches without fingerprint, copyrighted binary uploads or unapproved telemetry.
5. Candidate update sequence: primary-source review → named resource/function hypothesis → minimal independent offline parser or code proof → targeted version-safe game test when justified → E-grade observation, or exact blocker. Keep all game test decisions behind [testing strategy](../TEST_STRATEGY.md) and [workflow](../WORKFLOW.md).
6. A release or description may be updated tomorrow. **Reverify dated vendor claims and supported versions** before acting; source pages have last-reviewed dates, not perpetual compatibility.

## Explicit limits of this corpus

This is an **expandable, curated index of relevant public knowledge as of its review dates**, not a promise that all material on the internet has been exhaustively read. It does not store game binaries, dumps, reverse-engineered protected game code, third-party mod files, copyrighted textures or license-restricted hook tables. Community-provided mods can conflict, be unsafe or lack source. More sources can be added without weakening the evidence standards.

**Immediate priorities:** [resource graph/parry & finisher](QUESTIONS.md), [command eligibility/native dispatch](../INTEGRATION_FINDINGS.md), [test harness](../RUNTIME_TEST_HARNESS_RESEARCH.md). For playable development see [#30](https://github.com/Vyachean/acbf-resynced-classic/issues/30); for test automation see [#33](https://github.com/Vyachean/acbf-resynced-classic/issues/33).
