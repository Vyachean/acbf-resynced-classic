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
- Index method: bounded read-only parser; validated entry tables and returned metadata; graph payloads were not decoded.
- Runtime validation: none. Action dispatch, priority and integration interfaces remain unknown. No hook or input suppression is supported by this evidence.

