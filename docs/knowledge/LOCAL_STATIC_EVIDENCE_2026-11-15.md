# Local AC4 / Resynced static modding research — consolidated checkpoint

**Observation date:** 2026-11-15 · **Curated:** 2026-11-17 · **Sources:** historical [PR #27](https://github.com/Vyachean/acbf-resynced-classic/pull/27), especially its final read-only reports `INSTALLED_GAME_RESEARCH_2026-11-15.md` and `AC4_NATIVE_DISPATCH_FINDING_2026-11-15.md`; context in [issue #26](https://github.com/Vyachean/acbf-resynced-classic/issues/26) and [issue #18](https://github.com/Vyachean/acbf-resynced-classic/issues/18).

**Status:** historical local **static observations**, **not** currently reproducible from repository assets. Local executables/forges were not uploaded, both installation directories contained additional mod/loader files, and their exact pristine retail/build identities were unverified. This sanitized consolidation intentionally omits local absolute addresses, executable hashes and proprietary resource-index identifiers. No game was launched or modified as part of the original static checkpoint; no runtime action success was established. Read the original PR only when the history of the observation is required; its earlier commits received a public-metadata disclosure warning and are **not** copied into the new `main` history.

## 1. What was independently observed in local game files

| Local observation | Original AC4 (2013) | Resynced (2026) | What this does **not** demonstrate |
| --- | --- | --- | --- |
| PE architecture | Sample is PE32/x86 | Sample is PE32+/x64 | Clean build, valid game version or compatible engine call map |
| Input-related imports | DirectInput 8 and Win32 input declarations; an installed proxy could affect runtime resolution | XInput/HID/Win32 imports | Actual input provider in the active scene or native logical command arbitration |
| Archive generation | Bounded `scimitar` FORGE **v27** metadata/index reads | FORGE **v50**, metadata index enumerable via verified AnvilToolkit index-only path | Runtime resource choice, resource validity or schema compatibility |
| Selected typed resource | AnvilToolkit 1.3.7.0 `DataFile(Byte[], Game)` read one bounded local combat-like entry into memory; root + about 12 nested animation/phase-labelled records | One selected generic human/AI `EntityBuilder` candidate was mapped to v50 index/parent DataFile and checked against the local serialized key | Input edges, animation graph transitions, player/NPC ownership, combat rules or attack/takedown logic |
| Public GFL mapping | Public GFL resolved one Edward-template **candidate** and ordinary-soldier candidate to v27 entries, classified as `EntityBuilder` | Public Resynced GFL supported a **single checked** generic human/AI mapping against the local archive | Whole-file-list accuracy or identical GFL mappings across title updates |
| FGFL mismatch | One player-template candidate's FGFL association disagreed with its GFL association; the checked archive index agreed with GFL | No general conflict resolved | Automatic correctness of resource names, game actor ownership or selected override chain |

**Reproduction notes:** the original authors' tool was AnvilToolkit **1.3.7.0**. Original AC4 uses a selected in-memory byte-range `DataFile` parse; the toolkit's relevant generic root type was **not registered**, and its child `Animation` reader rejected the Black Flag enum. The BFR index-only call required code-path review and metadata/cache flags; it enumerated roughly **126,249 base index entries**, but a missing initialized GFL initially prevented names. Later public GFL files resolved **specific candidates**, not all entries. Published BFR GFL snapshots and local title update have differing/unknown version scope; notably a later public list was updated for **1.0.8**, not guaranteed matching the reported **1.0.6** target. No archive-wide extraction or arbitrary file-writer was used.

**Stop condition:** after selecting EntityBuilder candidates, the needed serialized component-payload schema/runtime type registration was **still unavailable** in the installed toolkit. Neither the original nor BFR selected resource yielded a verified `BhvAssassin` component list. A file name or type label **does not** establish that Edward or a particular NPC uses that resource.

## 2. Original AC4 x86 input-to-command *static* route

A separate bounded read-only Ghidra investigation in historical PR #27 reached a material finding beyond archive-name searches:

```text
logical input edge
  → active actor/context and input-consumption checks
  → candidate target / fallback resolution
  → status and component-interface predicates
  → conditional indirect virtual-handler selection
  → typed command / generic engine message submission
```

Source evidence showed **an ordered resolver chain** with A → B → C fallback and a separate actor/target pass, plus input edge/timing gates. A callback was registered through a `BhvAssassin`-related behavior factory. The generic command queue was confirmed downstream, but it has many callers; blindly enumerating generic message routes was explicitly stopped as low-value.

**Crucial boundaries:**

- Callback/behavior-factory registration **does not prove player Edward owns the callback**; it may be player, NPC or shared.
- Numeric candidate/status discriminators and selected vtable method targets **were not semantically named**. No evidence tied them to Attack, Takedown, Counter, Assassination, or actual hit resolution.
- One identified XInput logical-slot mapping related to **B**, but an internal slot is **not a universal physical button**, especially across DirectInput and remapping profiles.
- Decompiled pseudocode is *not* source code and belongs to the modified/unverified original-AC4 sample. The observed route is **not a Resynced x64 API**.
- No live game trace or causal input→native action validation was performed under this checkpoint.

**Research value:** the existence of native input/target/typed-routing checks makes the naive “one button remap = native action dispatch” assumption unsafe. However, it **does not prove which mechanism a Resynced mod can use**.

## 3. Version, privacy and source constraints

- Original target: local original AC4 x86 sample, exact release/build unknown; BFR: local x64 sample, title update not authenticated from the static observation. The owner's later reported **1.0.6** does not automatically retrospectively authenticate this older sample.
- Local game files, memory dumps, exact machine paths/hashes and proprietary raw archive entries **must not be added to GitHub**. A previous intermediate historical PR #27 commit received a review warning about overly specific local fingerprint/index details. This curated file copies no such raw material.
- The original source branch is an historical research record, **not** a dependency. This document is a distilled evidence table, not a new independent reinspection of game files.
- License/rights: public original-game data and upstream mods must remain outside a repository source distribution unless permission allows it. [AC4Coop](https://github.com/Kamzik123/AC4Coop) animation playback with an AI-disabled puppet is a **separate** source observation and not a semantics proof.

## 4. Best bounded follow-up

1. Find a **version-compatible, read-only** schema decoder for one original AC4 player and one comparable NPC `EntityBuilder` component graph; confirm **runtime actor-to-resource type ownership**. If both payloads remain opaque, record `SCHEMA_UNAVAILABLE` and stop expanding file-list/name guesses.
2. For original AC4, bind **one** statically identified candidate/virtual handler to a *named action outcome* in one authorized, exact-build witnessed session, preserving side-channel privacy. The game callback must first be proven player-owned or marked shared/unknown; no arbitrary virtual function guesses.
3. For BFR 1.0.6, independently verify its highest active patch-FORGE metadata, matching file-list provenance and a small combat resource. Do not port original x86 addresses/structures or 1.0.8 GFL IDs into a runtime hook or patch.
4. Feed confirmed findings into [action eligibility issue #10](https://github.com/Vyachean/acbf-resynced-classic/issues/10), [playable slice #30](https://github.com/Vyachean/acbf-resynced-classic/issues/30), [input safety #18](https://github.com/Vyachean/acbf-resynced-classic/issues/18) and [controlled harness #33](https://github.com/Vyachean/acbf-resynced-classic/issues/33). No duplicate unscoped collector or repeated owner remapping experiment.

**Outcome:** worthwhile **static technique and source graph**, **no native gameplay hook or compatibility approval**. More broad index enumeration would produce little additional information until the missing component schema or one credible actor/action runtime anchor is resolved.
