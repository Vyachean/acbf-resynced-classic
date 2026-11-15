# Installed-game research checkpoint (2026-11-15)

## Finding

Local files for both titles were available for read-only inspection. The inspected original AC4 executable identifies as 32-bit x86; the inspected Resynced executable identifies as 64-bit x64. This confirms a process-architecture difference between these local samples only. It does **not** establish a different action-dispatch design.

Both local directories contain additional mod/loader files, so neither sample is a verified clean baseline. The Resynced sample's exact Title Update could not be authenticated from the local evidence available in this pass. PE architecture and timestamps cannot establish a retail build or patch level. Exact file hashes and other unique local fingerprints were not published.

No game was launched, no installation was modified or copied, and no proprietary archive contents were uploaded. The installed original-game directory contains AnvilToolkit 1.3.7.0, but its presence alone does not prove support for the installed archive revisions or expose action-graph conditions. This pass did not extract archive resources or find a usable disassembler/decoder in the available command path. The Resynced executable's unusual section layout also prevented treating its file as ordinary, directly readable native code; no attempt was made to unpack or bypass protections.

## What is evidenced, and how strongly

| Item | Title/build scope | Evidence | Status | What remains unknown |
|---|---|---|---|---|
| The inspected executable samples use different process architectures | These local samples; exact releases unknown | Read-only PE header parsing | **Confirmed file property only** | Whether architecture changes the input model; clean-build identity |
| The local files represent pristine retail builds | Neither | Additional mod/loader files exist beside both executables | **Not established; do not assume** | Which additions affect input or executable behavior |
| Original AC4 has staged counter/follow-up, profile-sensitive movement/actions, and target/context-dependent moves | Original AC4, version scope in the existing source registry | Existing project sources describe these behaviors; no native branch or action graph was decoded in this pass | **Documented model, not runtime proof** | Exact transitions, timing, priority, interruptions, and local-build behavior |
| Resynced offers gesture/configuration choices and context-specific combat/traversal/tool actions | Published 2026 controls/design scope; local TU unknown | Existing project source registry; local executable headers do not authenticate the published scope | **Published description, not dispatcher proof** | Runtime precedence, eligibility signals, and local-build behavior |
| Attack and Takedown on one button produced ordinary Attack | Owner's Resynced session; exact version and eligibility state unknown | Existing firsthand owner report; not repeated | **One narrow firsthand observation** | Whether a visibly eligible finisher has another native dispatch route |
| Any universal action order or callable native finish/assassination dispatcher | Neither title | No verified code path, graph condition, function, or time-correlated runtime trace was obtained | **Unknown** | Actual engine arbitration and safe integration seam |

The project ledger's button maps and exception rows remain hypothesis/source catalogs. A button name, gesture option, action/animation label, synthetic test, or PE property is not evidence that the game chose that action in a particular state.

## Conservative action-selection model

The following is a hypothesis map to guide evidence collection, not a recovered engine graph:

| Game | Candidate selection inputs | Candidate stages/transitions | Priority evidence |
|---|---|---|---|
| Original AC4 | Physical input; movement/profile mode; player animation/state; target class, position and awareness; nearby interaction; tool/weapon mode | Input may nominate movement/combat/stealth/interaction action; a counter may open a follow-up choice; target, animation phase or interruption may reject/cancel it | No internal selector, branch order, timing window, or action API verified |
| Resynced | Physical input and configured press/hold/double/toggle gesture; context; player/target state; ability modifier; traversal/naval/UI mode | Input may nominate attack/defense/takedown/ability/traversal/interaction action; context changes may transfer input ownership; animation and eligibility may gate outcomes | No internal selector, simultaneous-binding order, eligibility API, or cancel path verified |

For both titles, the actual state identifiers and runtime transitions are **UNKNOWN**. Existing project `can_attack`/`can_takedown`-style model flags do not, by themselves, encode prompts, gesture phase, target geometry/archetype, animation window, mode, or interruption.

## Cross-title comparison and modification boundary

| Question | Evidence-based answer at this checkpoint |
|---|---|
| Which original mechanics exist in Resynced? | Public descriptions cover analogous combat, stealth, traversal, tools, and ship systems, but this pass did not prove runtime equivalence of their eligibility or dispatch. Similar names are not proof of identical mechanics. |
| Which mechanics changed or disappeared? | Not established from the executable/resource evidence obtained. Published design descriptions suggest changed gestures, modifiers, parkour options, and additional actions; exact local-build differences remain unverified. |
| Which actions cannot be combined by button remap alone? | Multi-stage, gesture-sensitive, target/context-gated actions are not demonstrated to be recoverable by remapping. The owner's Attack/Takedown observation is evidence only for that tested setup. |
| Minimum mod level needed? | Unknown. It may require native action dispatch, or a safe input seam plus complete context/eligibility/cancel signals. Neither interface has been verified. |
| What Resynced actions might be blocked? | Any action sharing a consumed physical input in another combat, stealth, parkour, ledge, swimming, naval, tool, interaction, menu, or specialized-mode context. Until context ownership and fallback are known, consuming such input risks silent regressions. |

## Unknowns and minimum discriminating runtime evidence

1. Identify each exact lawful game variant/build and control/remap settings; mark the build `unknown` if it cannot be authenticated. Record whether any loader/mod is active.
2. For original AC4, capture input, visible player/target state, prompts, and outcome continuously through counter entry, follow-up selection, press/hold/release, and interruption/cancel. This tests staged behavior and timing without inferring them from guide icons.
3. For Resynced, record a visibly eligible finisher in its own context and an ordinary attack in its own context. Only compare a shared binding when eligibility is shown. Do **not** repeat the owner's already reported unspecified Attack/Takedown collision.
4. Observe one relevant context transition (stealth/traversal) and one mode transition (water, ship, or menu) to learn whether native input ownership changes and whether an override can safely pass input through.
5. If runtime outcomes still leave the integration seam unknown, inspect ordinary authorized disassembly and cross-references for the authenticated build. Stop if code cannot be examined without unpacking or bypassing protection.

Record title/build, settings, device/remaps, exact press/hold/release sequence, player/target/prompt state, visible outcome, and whether the run was independently repeated. Guides, menu screenshots, animation labels, and synthetic CI tests cannot satisfy this runtime evidence requirement.

## Conclusion

The inspected files confirm only that the two local executables differ in architecture; they do not establish their exact clean versions or action dispatch rules. A settings-only remap is not shown to recreate AC4's candidate staged/contextual behavior in Resynced. A mod might need a native action interface or a safe interception point with complete context, eligibility, cancellation, and fallback signals, but that remains a hypothesis.

No game-facing hook or input suppression is supported by the present evidence. Issues #18, #21, #22, and #26 remain open. The next decisive evidence must come from authenticated builds and a narrowly designed witnessed runtime session; more button-map prose will not resolve the dispatcher unknowns.

## Local static inspection update — 2026-11-15 (review follow-up)

### What was checked

Read-only PE parsing reconfirmed the two local samples' architecture difference: the original sample is 32-bit x86 (PE32), while Resynced is 64-bit x64 (PE32+). The local directories contain additional mod/loader files, and the original release and Resynced Title Update could not be authenticated. These PE properties do not identify a retail build or explain action dispatch.

In-place reads of representative Forge headers identified the `scimitar` container family and two format generations: v27 in the original sample and v50 in Resynced. 7-Zip 25.01 listing attempts returned `Cannot open the file as archive` for the tested files. This is a limitation of that listing attempt, not proof that other readers cannot parse the archives.

AnvilToolkit 1.3.7.0 is present with the original sample. Read-only reflection confirmed a v50 deserializer. IL inspection showed that this path creates an output directory and calls archive-directory constructors; the directory type includes a disk-writing method. Since no single-entry-only contract was established, the deserializer was not invoked. No CLI listing command was found. No game data was extracted or written.

A bounded read-only index parser validated entries in several original v27 archives, including a character archive. It returned names, IDs and sizes for candidate combat animation-graph resources, including basic attack, combo, counter and stun-kill categories. The resource payloads were not decoded. Consequently, these names and categories do not establish player ownership, input binding, transition edges, eligibility, priority, timing or cancellation. Exact local resource identifiers and fingerprints are intentionally omitted from this public report.

String inspection of both executables found candidate identifiers related to counter/parry/assassination in the original sample and Takedown/parry/parkour in Resynced. No cross-references, native branches or dispatcher conditions were recovered. String presence is not action-selection evidence. No unpacking or protection bypass was attempted.

### Prioritized questions and status

| Question | Direct file evidence | Unknown |
|---|---|---|
| Original contextual attack/assassination | Candidate action-map identifiers only. | Input mapping, target/context checks, precedence and native dispatch. |
| Original guard → counter → follow-up | Indexed combat graph candidates and candidate counter/parry identifiers. | Whether the resources are player-facing, how nodes connect, input ownership, timing and interruption. |
| Resynced Attack vs Takedown | Candidate Takedown-related identifiers. Existing owner report remains the only outcome evidence: shared binding yielded ordinary Attack; build and eligibility state are unknown. | Selector order, eligibility signals and alternate native route. That same experiment was not repeated. |
| Resynced RT parkour collision | Candidate parkour identifier only. | RT binding, state gates, fallback and interaction with combat actions. |

No priority rule is verified. The next useful static step for original AC4 is to decode one selected v27 graph resource in memory and identify a verified player-input-to-node edge. For Resynced, first establish a reader that can list and inspect one selected v50 resource without bulk extraction. If that requires unsafe unpacking or bypassing protection, stop and use a narrowly scoped witnessed observation.

### Evidence record

- Game versions: original release unknown; Resynced Title Update unknown. Both local samples are modified/unverified.
- Toolkit: AnvilToolkit 1.3.7.0; reflection and IL inspection only; extraction method not invoked.
- Archive utility: 7-Zip 25.01; listing returned `Cannot open the file as archive` on tested Forge files.
- Index method: bounded read-only parser; validated entry tables and returned metadata. One selected combat container was parsed in memory; its transition schema remains undecoded.
- Runtime validation: none. Action dispatch, priority and integration interfaces remain unknown. No hook or input suppression is supported by this evidence.

### Selected-resource in-memory parse

After confirming that the archive-wide deserializer writes extracted files, a narrower route was tested: one already indexed, small original combat-resource byte range was read into memory and passed to AnvilToolkit's `DataFile(Byte[], Game)` overload. The constructor returned a parsed container with a root combat/graph-like record and a dozen nested records, including named animation resources labelled for start, positioning, wait/settle, and death/final phases. Only the selected byte range was read; no archive-wide deserializer, extraction method, or file writer was called, and no output path was supplied.

This is direct evidence that the selected archive entry is a structured container rather than one flat animation payload. The phase labels and record membership remain static metadata: no serialized transition edge, player input binding, eligibility condition, priority, or interruption rule has yet been decoded. The public report omits the exact local resource identifiers and names.


### Input import and parser boundary update — 2026-11-15

A read-only walk of each local executable's PE import directory found different declared input-related APIs. The original AC4 sample imports DirectInput 8 creation and Win32 keyboard/raw-input/message functions. The Resynced sample imports XInput state/set-state functions, HID APIs, and Win32 raw-input/keyboard functions. This proves static linkage only. It does not prove which device is polled in a given mode, which callback receives an event, or how an event reaches an action selector. The original sample's local directory contains a `dinput8.dll` proxy/mod, so the runtime implementation resolved for that import is not authenticated.

The Toolkit v50 reader can safely enumerate the Resynced archive index by setting `CreateCache=true` and `stream=true`: IL shows that this chooses `ForgeDirectory.ReadEntries`/`ReadEntriesBFR`, while payload reads/writes are in separate methods. In that mode the installed Toolkit parsed the base archive index but did not resolve entry names. `ForgeDirectory.ReadEntries` obtains names through `GameFileList.GetOriginalFileName`; its lookup table is uninitialized here, and the expected local GFL sidecar was not found. Therefore the index is real and readable but cannot yet identify combat/parkour entries by name. This does not establish that those resources are absent.

For the selected original combat container, the in-memory `DataFile` parser exposed a root record plus nested animation-class records. The installed Toolkit registry has an `Animation` class reader, but its `Animation.Read` method checks supported games, rejects the original Black Flag enum, and leaves the byte stream untouched. The root record's type is not registered in the Toolkit's compiled `ScimitarClassRegistry`. A generic reader attempt therefore did not decode the root into typed fields, edges, conditions, or transitions. The selected record has not been proved to be an action-selection graph or to be animation-only.

### Input-to-action path status

The best-confirmed chain now ends at platform input API linkage:

`game executable import table → declared DirectInput / XInput / HID / Win32 raw-input APIs`

No native callsite, game input event, context owner, eligibility predicate, selected action, or action-start transition has been recovered. It would be incorrect to extend this chain to Attack, Counter, Takedown, Assassinate or Parkour. The local samples remain modified/unverified; the Resynced Title Update and original release remain unknown.

### Current blocker and next discriminating method

No disassembler is available on the console path: checked Ghidra, IDA command-line names, LLVM objdump, dumpbin, objdump, Rizin/radare2, ILSpy and ildasm were absent; Python Capstone/LIEF modules are not installed. Recursive searches found no PDB files in either game installation. No protection bypass was attempted. The Resynced v50 index is now safely readable, but the missing GFL name map prevents finding named action resources. The original v27 selected payload is accessible in memory, but the installed Toolkit lacks a parser for its root class and rejects the child animation format for Black Flag.

Next informative work requires either (a) a locally available, authorized native disassembler for the unprotected original AC4 executable and a GFL/name index for Resynced, or (b) a witnessed minimal gameplay observation. The static imports alone are not enough to claim an input-to-action path. Do not add hooks based on them.




## Follow-up: bounded resource reads and parser limits — 2026-11-15

This section supersedes earlier statements in this document about the v50 reader not being invoked and the selected AC4 resource not yet being parsed. The game installations remain read-only; no game was launched.

### Original AC4 selected-resource parse

**Game/version:** local original AC4 sample; exact release unknown and installation modified/unverified.

**Source and method:** one small, previously indexed combat resource was read by its bounded byte range and passed in memory to AnvilToolkit 1.3.7.0 `DataFile(Byte[], Game.BlackFlag)`. No archive-wide deserialize, output path, extraction, or writer was used.

**Confirmed:** the parsed container has one root record and twelve nested records. The nested type maps to `Animation` in the Toolkit's compiled registry. The root type is not registered there. A generic typed read failed after the hashed-data lookup was initialized with an empty map; direct Animation parsing rejects the original Black Flag game enum and leaves the nested bytes unread. These are observed parser outcomes, not proof of malformed game data.

**Not confirmed:** the root record’s field schema; whether the container is an action graph; any input binding, state identifier, condition, transition, priority, cancellation, or target selection. Phase-like animation labels are not logic evidence. Further work on this resource stops until a valid schema/decoder is available.

### Resynced v50 index read

**Game/version:** local Resynced sample; exact Title Update unknown and installation modified/unverified.

**Source and method:** AnvilToolkit 1.3.7.0 IL was inspected before calling `ForgeFile.Deserialize50`. With `UseDepsFile=false`, `CreateCache=true`, an empty output-folder argument and `stream=true`, IL routes into the index-entry readers; `CreateCache=true` bypasses directory creation, and the selected route does not invoke payload read/write methods. The call was made in memory against the base archive index only. It returned 126,249 entries; no payload or output file was written. The Toolkit printed a misleading “Writing files...” console message despite this metadata-only path.

The index entries had no names because `GameFileList.GetOriginalFileName` depends on a game file-list map. The expected local Resynced GFL map was not found in the checked game/project locations. Consequently, the v50 index is safely enumerable, but combat and traversal resources cannot yet be selected by name. This does not show that those resources are absent.

### Input API linkage and native dispatcher

Read-only PE import-table parsing confirms that the inspected original executable declares DirectInput 8 creation plus Win32 keyboard/raw-input APIs. The inspected Resynced executable declares XInput state APIs, HID and Win32 raw-input/keyboard APIs. This proves declared static linkage only—not which API is called at runtime, event phase handling, context ownership, or action selection. The original local install includes an input proxy/mod, so the runtime provider for its DirectInput import remains uncertain.

No native callsite or branch from a physical event through player/target eligibility to a selected action was recovered. The original sample is 32-bit x86, the Resynced sample is 64-bit x64, both are modified/unverified, no local PDBs were found, and no ordinary disassembler was available in the checked console/tool paths. Resynced also has protected/virtualized executable sections. No unpacking, protection bypass, or DRM change was attempted. Therefore no action-priority rule or safe mod interface is confirmed.

### Evidence status and next useful step

| Claim | Evidence level | Limit |
|---|---|---|
| A bounded selected AC4 resource is a structured container with a root and nested typed records | **Confirmed static analysis** | Root schema and semantics unresolved |
| The installed Toolkit’s v50 metadata-only route can enumerate the Resynced base archive index without writing payloads | **Confirmed static analysis + bounded execution** | Names are unavailable without the GFL map; no payload decoded |
| The executables declare different input-related OS APIs | **Confirmed static file property** | No runtime callsite or input-to-action edge |
| Any button event selects Attack, Counter, Takedown, Assassinate, Parkour, or another specific action under a stated condition | **Unknown** | No selector branch, action graph edge, or witnessed gameplay trace |

The strongest chain still ends at the executable import table. The success criterion—one verified path from input/state condition to chosen action—has not been met. The next static step is to obtain an authorized native disassembler for ordinary original AC4 code and a Resynced name map plus a safe single-entry reader. Stop on Resynced executable analysis if it would require defeating protection. If those inputs are unavailable, a narrowly witnessed run is the next discriminating method, but it has not been performed in this console-only pass.

### Published-history review

Commit `1521d4c` includes local executable/archive fingerprints and proprietary resource-index metadata (including resource identifiers/offset details and binary string labels) that are omitted from the current report. It did not include game binaries, full archive payloads, or full-file SHA-256 digests. The details are retained only in the permitted local research note. The current branch tip’s report omits them, but the commit remains in published PR history. Cleaning that history requires a branch history rewrite and may not remove cached or unreachable GitHub objects; no force-push or rewrite was performed.

## Native input trace update — 2026-11-15 (Ghidra)

This section supersedes earlier statements that the original sample had no usable disassembler, that analysis ended at the import table, or that no native input callsite was found. Exact local function labels and addresses remain in the private local research note because the PR's history review identified local executable fingerprints and resource-index identifiers as unsuitable for public publication.

**Title/build scope:** the locally installed original AC4 executable, PE32 x86; exact release unknown and installation modified/unverified. AnvilToolkit/game resources were not modified. Ghidra 12.1.4 was used in a separate research project. The game was not launched, so all findings below are **confirmed static analysis**, not dynamic behavior.

### Confirmed input path

- Cross-references from the executable's input imports lead to actual native callsites: DirectInput 8 creation is called on a conditional initialization path, and an XInput state poller calls `XInputGetState`. This establishes code-level use in this binary sample, not which provider wins at runtime. The local installation includes a DirectInput proxy/mod.
- The XInput poller checks the API result, reads axis/trigger/button state, converts it into per-device fields, normalizes analog values, then calls a shared edge tracker.
- The edge tracker copies prior state and compares current flags with prior flags. A false-to-true transition records a timestamp in per-control event data. This confirms press-edge recording in native code; it does not establish release/hold/double semantics or map a particular slot to a gameplay action.
- A separate DirectInput path obtains device state through its interface, reads configured mapping records, fills logical button/axis arrays, and calls the same edge tracker.
- The profile loader reads the selected input profile, resolves device sections using VendorID/ProductID with a default-map fallback, loads button/stick/shoulder slots, and reads a High Profile toggle setting. The installed default map contains device-facing slots such as Button1–Button4 and movement/tool controls; it does not encode Attack/Counter/Finish precedence. The runtime toggle code changes mapped input state using elapsed-time checks, but the affected gameplay action is not identified.

The evidence therefore reaches:

`physical device state → per-device state fields → configured logical input slots → press-edge timestamp`

It does **not** yet reach player-context ownership, a target/eligibility predicate, selection between combat/stealth/traversal actions, action start, or cancellation.

### Candidate action dispatch rejected as evidence

A code path associated with an attack-related script/event wrapper was also cross-referenced. Its observed caller performs an internal sequence/probability choice and does not read the controller-state or logical input arrays. It is therefore not evidence of the player-input-to-attack path. Likewise, an engine frame profiler marker for an animation action-manager phase only identifies a scheduled phase; the marker alone does not prove arbitration logic.

### Evidence and remaining unknowns

| Mechanism | Evidence level | What the binary confirms | Still unknown |
|---|---|---|---|
| DirectInput initialization | **Confirmed static analysis** | A conditional native callsite and device-interface setup | Whether this provider is used in the unmodified retail runtime |
| XInput polling | **Confirmed static analysis** | A callsite, success check, state conversion and downstream edge tracking | Runtime device precedence and action ownership |
| Press edge | **Confirmed static analysis** | Current/prior comparison and timestamp write on false-to-true transition | Release/hold/double interpretation and consumer behavior |
| High/Low profile mapping | **Confirmed static analysis, partial** | Config profile selection, device-ID fallback and input-toggle handling | Which gameplay action a mapped slot controls and when the game switches context |
| Attack/Counter/Finisher arbitration | **Unknown** | No traced player-input branch or action-resource edge | Target checks, priority, state transitions, interruption and action-start interface |
| Resynced equivalence | **Unknown** | No new Resynced action-dispatch trace in this update | Exact local TU remains unauthenticated; named resource lookup and safe single-entry reader remain unresolved |

Full Ghidra auto-analysis is incomplete: repeated runs exhausted heap in Ghidra's x86 constant-reference, stack and string-model analyzers. Relevant function-level xrefs/decompilations were captured from the live partial analysis, but a complete saved analysis database has not been established. No function-count from that transient run is treated as a result.

### Consequence for restoration

The native code confirms that the original game's input layer contains device-specific polling, profile mapping, logical-state generation and edge timing. That is more than a button table, but it still does not show how a particular logical slot wins between Attack, a grounded finisher, kill-streak follow-up, assassination, or parkour. A remap-only restoration is therefore **not demonstrated**; a minimum safe modification seam also remains unknown. No hook or input suppression is supported yet.

The next static step is to trace readers/consumers of the logical input arrays and edge timestamps, rather than inspect more producer functions. If that does not expose the player action selector, the selected v27 resource needs a valid root-schema decoder. Resynced investigation still needs a legitimate name-map/single-entry path; its protected executable will not be unpacked or bypassed. Runtime confirmation remains unavailable under the current no-launch constraint.



## Open resource-map and actor-template follow-up (2026-11-15)

This section supersedes the earlier resource-name limitation for the checks described here. It does not establish action semantics or runtime ownership.

### Method and safety

The published resource lists were parsed with the installed AnvilToolkit 1.3.7.0 implementation, not a new GFL parser. The Toolkit's built-in list parser loaded both the original and Resynced GFL files. For archive validation, the original archive was opened through its index-only v27 path; the Resynced base archive was opened through the v50 index reader. Before either call, the corresponding Toolkit code path was inspected: these selected routes read index metadata and selected byte ranges only; they do not extract the archive or write game files. No full archive or game installation was copied.

The local Resynced title update is unknown. Therefore the successful match below proves one mapping against the installed archive, not compatibility of every published mapping, either FGFL snapshot, or the full unknown update.

### Results

| Evidence item | Result | Confidence and limit |
|---|---|---|
| Original public GFL | Toolkit parser loaded 664,387 entries, including Forge and DataFile labels. | Confirmed static parser result; list itself is not proof that each entry exists in the installed build. |
| Resynced public GFL | Toolkit parser loaded 1,924,462 entries. A selected generic human/AI EntityBuilder mapping resolved to a parent record in the local v50 archive index; one selected parent DataFile was read and contained the mapped key, whose Toolkit type resolved to EntityBuilder. | Confirmed for this one mapping and local archive. Installed update remains unknown; no whole-list compatibility claim. |
| Original Edward candidate | A selected player-character candidate resolved through the public GFL to the local v27 archive index. One parent DataFile was read; it contained the mapped key and the Toolkit classified the payload as EntityBuilder. | Confirms a real local EntityBuilder resource candidate, not that this template is instantiated as Edward at runtime. |
| Original ordinary soldier candidate | The same index-plus-single-parent procedure found the selected soldier candidate and its key; Toolkit classified it as EntityBuilder. | Confirms resource type and mapping only; it does not establish NPC behavior or component membership. |
| FGFL/refmap | The public original FGFL and reference map were inspected. For one player-template candidate, the FGFL text association differs from the GFL association; the installed local archive index agrees with the GFL path for the checked record. | This is a concrete mapping discrepancy; it warns against treating a list name or one FGFL snapshot as authoritative. No complete cross-version validation was done. |
| BhvAssassin ownership | Not established. | The selected EntityBuilder payloads were not decoded into a verified component list. |
| Action selection | Not established from these templates. | No input-to-component-to-handler association or action-priority rule was recovered. |

### Typed-decoding boundary

The installed Toolkit contains an EntityBuilder type, but its supported-game declaration excludes Resynced. The original and Resynced selected payloads carry an inner serialized class type that the installed runtime registry does not resolve to a concrete class. The available embedded schema data did not provide type/field definitions sufficient to decode the component graph. The selected original payload therefore falls back to a failed base ScimitarClass decode, and the Resynced EntityBuilder decoder is not declared compatible with that game.

This is a limitation of the installed decoder/schema coverage, not evidence that the resources are malformed or that they lack components. No speculative parser was written. Public maps solved resource discovery and single-record identity checks, but not actor ownership, runtime class binding, virtual implementation, or action semantics.

### Open-source mod source review

- [AC4Tools](https://github.com/45hgv43whgf/AC4Tools) documents a strict executable-build and image-base gate for gameplay addresses, with per-site byte checks. Its addresses must not be transferred to an unverified local executable. It does not provide a proven input-to-action dispatcher for this installation.
- [AC4Coop](https://github.com/Kamzik123/AC4Coop) exposes useful research leads in source: wrappers for EntityBuilder/CharacterActionSet and action playback through the engine's action controller. Its README describes disabling the puppet behavior component and replaying action aliases. This is evidence of a mod's use of those interfaces on its own target build, not proof that Edward's local actor uses BhvAssassin, nor a verified mapping to the static dispatcher in this report.
- The reviewed [universal-modder](https://github.com/rehan-remade/universal-modder) material did not resolve the missing AC4-specific template-to-runtime-class link.

### Conclusion and next useful check

The open maps remove the previous Resynced resource-name barrier for selective lookup, and they validate concrete original player/NPC EntityBuilder candidates against the installed v27 index. They do not remove the blocker that stopped static action analysis: component payload schemas/runtime type registration are missing for these selected resources.

The next static avenue should be a version-matched, existing decoder or an independently verified runtime class-registration source. If neither exists, the highest-information next step is a short, read-only dynamic observation session in the original game, but it remains outside this stage's no-launch constraint. Observe the already identified candidate resolver and its indirect target while Edward performs one controlled context action, and record actor vtable/class identity plus the command/message discriminator. That single trace would test ownership, concrete virtual dispatch, and one action path without changing game logic. Dynamic observation would still be required to establish behavior, and no runtime claim is made here.
