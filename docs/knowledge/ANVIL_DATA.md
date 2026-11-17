# Anvil / Scimitar resource formats and editing research

**Game scope:** Original AC4 (2013) and Resynced (2026) where explicitly marked. **Format versions are not interchangeable.** Reviewed 2026-11-17. This is a source-linked technical guide for **independent research** with the owner's legally obtained game files; not a redistribution of their contents.

## Resource glossary and compatibility

| Item | Known behavior or author description | Boundary |
| --- | --- | --- |
| `.forge` | Anvil container/archive of indexed resources and manifests; original AC4 distribution often `DataPC.forge`; BFR uses `DataPC_boot.forge` and title-update patch chain | “forge” is a family of formats, not one fixed binary layout. **BFR FORGE v50** is supported by [BFR Forge Injector #108](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/108); old Rogue parser **v27** should never process it by assumption |
| `.data` | Typed in-engine resource with headers, serialized objects and sometimes compression or references | A byte offset/text match does not identify a logical game setting; use type-aware parser and dependency graph |
| `EntityBuilder` / `EntityGroupBuilder` | Serialized entity definitions/construction references; AnvilToolkit release notes mention improved support | A discovered builder is not proof it is active Edward/NPC in an actual session |
| `CharacterActionSet` / action state | Animation-layer/action references in original AC4 runtime research, possibly related resources in game files | Original AC4 structure/API names do not identify BFR native dispatcher |
| `PrefetchingManifest`, `DeletedFilesManifest`, `PrefetchInfoOverrides`, `PrefetchingFileInfos`, `GlobalMetaFile` | BFR patch-FORGE chain metadata identified in [AnvilToolkit author's wiki](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki/Creating-a-New-Patch-Forge-for-Assassin's-Creed-Shadows-or-later) | Wrong parent/override graph can crash game before save load |
| `TextureMap` / `.RawAsset` / `CompiledMip` | Texture resource plus external high-mip data; [author texture guide](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki/Texture-Modding-in-Custom-Forges) explains creating self-contained patch resource | Texture replacement is visual-only; mip conversion does not provide combat logic |
| File lists/reference maps/GFL tables | AnvilToolkit resources help resolve hashes, resource names and references; toolkit supports original AC4 reference list downloads | A file-list candidate is **not** proof of the loaded resource binding; verify actual archive and patch head |
| XML sidecars, typed serializers | AnvilToolkit can export/import typed XML for supported resource classes / animation categories according to changelog | Not all classes or title updates round-trip; a parseable file is not proof of correct engine behavior |

## Official AnvilToolkit knowledge (A)

- [Tool download and full changelog](https://www.nexusmods.com/site/mods/455): original AC4, Rogue and BFR support, file reference lists, recent FORGE/data changes. Prefer versions explicitly supporting the game's format; **record exact toolkit version** in test artifacts.
- [Author resource repository + wiki](https://github.com/Kamzik123/AnvilToolkit-Resources) and [wiki home](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki): guides and file lists, not a uniform Python API.
- [Creating a New Patch Forge](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki/Creating-a-New-Patch-Forge-for-Assassin's-Creed-Shadows-or-later): **AnvilToolkit 1.3.7+** has **Game Explorer → New → Patch Forge** explicitly for **Resynced and Shadows**. The wizard creates correct next patch and chain metadata, unlike renaming existing file.
- [Texture Modding in Custom Forges](https://github.com/Kamzik123/AnvilToolkit-Resources/wiki/Texture-Modding-in-Custom-Forges): load RawAssets, remove compiled-mip dependencies and place TextureMap/data and any necessary prefetch entries in the new custom forge.

### BFR patch chaining: safe mental model

The game loads a *base* and one or more *overrides* (names are illustrative). **The user's 1.0.6 chain head has not been enumerated**:

```text
DataPC_boot.forge
  ↳ DataPC_boot_patch_01.forge
      ↳ DataPC_boot_patch_02.forge (if installed)
          ↳ [future newly generated patch, named by toolkit]
```

The next patch must:
1. Reference correct current parent head (`PrefetchInfoOverrides`), preserve already applied overrides/deletions and expected metadata.
2. Carry the modified asset/data resource **with its correct type/ID/reference graph**; for many non-texture `.data` assets, update `PrefetchingFileInfos` to point to the new location. The exact requirements are **type-dependent**; do not indiscriminately copy prefetch records.
3. Use **highest existing overriding resource** as base of modification or risk undoing another mod/patch. Do not assume that a resource in the base forge is the live version.
4. Avoid collisions: multiple mods touching the same head/name/ID cannot be safely combined by copying one file over the other.
5. Package only a reproducible **independent patch recipe/tool** where rights permit; never commit or distribute Ubisoft's original/modified `.data`, `.forge`, textures or extracted game resource dumps.

An empty patch that the toolkit generates is a useful sanity check, but **a syntactically valid patch is not proof the game will boot**. Test on a disposable backup and exact target build only when authorized.

## Old AC4 (2013) data route

- Original game modders commonly distribute entire modified `DataPC.forge`; examples include [More Frequent Double Counters](https://www.nexusmods.com/assassinscreedivblackflag/mods/1029) and [No Counter Slow Motion](https://www.nexusmods.com/assassinscreedivblackflag/mods/871). The latter explicitly names `51_-_Game Bootstrap Settings.data` for a global pacing tweak.
- Source-based [AC4Coop](https://github.com/Kamzik123/AC4Coop) targets **runtime** engine objects rather than an exported game behavior SDK; its `EntityBuilder` and `ActionComponent` names are conceptual bridges for file-graph research, not guaranteed 1:1 matches.
- Legacy [Steam AC modding guide (DEPRECATED)](https://steamcommunity.com/sharedfiles/filedetails/?id=2288998586) describes earlier `.forge` / `.data` workflow and outfit mods, but explicitly says old Delutto tools were superseded by AnvilToolkit. **Historical reference only**: modern tool UI and format support supersede its instructions.
- [Kirikirin RE kit overview](https://kirikirin.com/en/blog/game-assassins-creed-modding-kit) reports a Rogue `forge.py` parser v27, a read/modify test and LZO `.data` parser. It does not prove correct handling of AC4's particular build or BFR v50. Do not blindly transplant a byte-level parser.

## Tools with different write semantics

| Tool | Writes to | Purpose / no-go |
| --- | --- | --- |
| AnvilToolkit 1.3.7+ BFR patch wizard | **New BFR patch chain file** | Preferred non-destructive independent data experiment; requires metadata validation and safe restore |
| Forge Injector #108 | BFR v50 archive index/data | Low-level beta; may accept bytes but not validate dependencies/schema; inspect backups |
| [Jackdaw Flag Patcher](https://github.com/emike09/Jackdaw-Flag-Patcher) | Single resource in existing `DataPC_boot.forge` via verified append + index redirect | Excellent audited transactional/restore pattern, but **in-place base archive mutation** differs from additive FORGE |
| [ReForger](https://github.com/QuilLeeR/ReForger) | RFGB package / SHADOWS v42 in published README | Separate conflicting author claims of later BFR use; not trusted as a generic v50 patcher without release validation |
| [Black Flag Texture Replacer #364](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/364) | Browser-driven texture replacement | Appropriate for texture exploration only; do not upload protected game data to a third-party service without considering rights/privacy |
| Original AC4 Resorep | Runtime D3D11 texture substitution | Legacy x86 graphics hook, not a typed `.data` writer or gameplay mod |

## Minimal independent combat-data research plan

**Target questions:** Is there an identifiable game-authored resource affecting (a) parry tolerance, (b) enemy attack cadence/double counter state, (c) finishing animation selection, (d) actor combat archetype? The answer must not be assumed from a mod title.

1. **Discovery without patching:** inventory target game's executable hash, resource archives and patch order. Query AnvilToolkit's file list/reference map for candidate game resources such as `Edward_Default.data`; record type IDs and ancestry. Compare original AC4 **only as separate schema research**.
2. **Data provenance:** map candidate to a source forge, exact active override and referenced dependencies. A resource name with no verified in-game connection is only a candidate.
3. **Independent read-only parse:** validate typed container/header/object boundaries and round-trip on synthetic fixtures. If no schema, document **exact missing field/type**, don't guess numerical timing constants.
4. **One-variable patch:** independently author only a single minimal change to a **new BFR patch forge** with fail-closed SHA/type/parent checks, restore path, and no third-party payload copied.
5. **Behavior test:** use one controlled native scenario and engine-semantic outcome; distinguish **animation** change from action eligibility, parry window and NPC AI. Compare against baseline and record build/mod stack.
6. If no interpretable variable after bounded investigation, return `DATA_BLOCKED` with resource IDs/types and evidence, leaving native action research unblocked.

## Reproducible evidence fields

`game`, `architecture`, `build_version_claimed`, `exe_sha256_local_only`, `toolkit_version`, `forge_basename`, `patch_parent`, `resource_name`, `resource_type`, `resource_id`, `prefetch_ref_origin`, `source_url`, `evidence_grade`, `dependent_resources`, `change_or_no_change`, `validation_status`, `restore_procedure`, `license`, `unknowns`.

**Do not publish executable or game-derived bytes or private machine paths in the project.**

## Further reading

[Resynced ecosystem survey](../MODDING_ECOSYSTEM.md) · [BFR integration](../INTEGRATION_FINDINGS.md) · [AC4 game-side mechanics](../COMBAT_CONDITIONS.md) · [project source register](SOURCES.md)
