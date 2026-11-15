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


