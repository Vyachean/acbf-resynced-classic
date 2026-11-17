# Runtime test harness and controlled combat testing — external-tool leads


> **Broader research index:** [two-game modding knowledge base](knowledge/README.md) with [runtime/native API comparison](knowledge/RUNTIME.md) and [testing methodology](knowledge/TESTING.md). This page retains detailed instrument investigations.

**Research date:** 2026-11-17. **Status:** public-source inventory and proposed architecture; **no in-game tests, engine NPC-control API, or Resynced 1.0.6 compatibility confirmed**. This document records tools worth evaluating, not dependencies or permission to install them. Related: [integration findings](INTEGRATION_FINDINGS.md), [single-session strategy](TEST_STRATEGY.md), [Puppet Native ABI](PUPPET_NATIVE.md), and issues [#30](https://github.com/Vyachean/acbf-resynced-classic/issues/30), [#10](https://github.com/Vyachean/acbf-resynced-classic/issues/10), [#18](https://github.com/Vyachean/acbf-resynced-classic/issues/18).

> **Broader resources:** See [the Resynced modding ecosystem survey](MODDING_ECOSYSTEM.md) for source-checked patch-FORGE authoring, combat/AI data-mod leads, animation-speed hooks and manager/version restrictions. This document remains focused on runtime test automation.

## Problem to solve

Current synthetic tests can validate project logic but cannot demonstrate that the game accepted a command or that a counter/parry/finisher actually occurred. A useful **minimal** runtime harness must capture reproducible initial conditions, input, and **engine-semantic outcome**, not simply record physical button edges. The priority is one auditable combat test on the owner's exact Resynced **1.0.6** build, with a clear failure report if the required hook is unavailable. Creating a custom arena, spawning NPCs or controlling their AI is a **later capability**, not a prerequisite for the first test.

## Source inventory and evidence bounds

“Published” means the cited project advertises the feature or its source documents it; it does **not** mean independently reproduced in our game build. Source code or description is not proof of stable runtime behavior.

| Lead / primary link | Publicly documented capability | Unverified for this project / priority |
| --- | --- | --- |
| [ACBFRTools — Nexus #661](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/661) | F4 in-game menu; control over game speed, Edward, weather, noclip/free camera, save editing | **High** for quickly preparing/repeating scenarios. Precise edition support, scriptable interface, ASI dependencies, hook overlap and reproducibility on 1.0.6 **unknown**. UI controls are not a documented NPC command API. |
| [Encrypted's Resynced Ultimate Rewrite — Nexus #64](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/64?tab=description) | Cheat Engine table; player/naval/time cheats, teleportation, inventory/item handling, live diagnostics | **Medium** for manual scenario setup and memory-research leads. Not a verified stable programmatic test API; memory writes and third-party dependencies can perturb combat. **Do not install in the normal test path**. |
| [Otis_Inf camera tools for Resynced](https://opm.fransbouma.com/Cameras/acbfresynced.htm) | Timestop, frameskip, adjustable game speed, free camera, HUD and environment options; page states v1.0.1, Ubisoft Connect/Steam | **High** for temporal observation and fixed-angle recording. Frameskip/timestop are confirmed *advertised features*, **not** proof of frame-perfect scriptable timing or unattended trials. Injected camera may interfere with other hooks. |
| [Puppet Native — Nexus #566](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/566) / [upstream source](https://github.com/venvyl/puppet-native) | Public MIT-licensed plugin ABI/lifecycle; project has an offline-tested read-only Puppet Native plugin | **High** as *possible host* for our own instrumentation once installed loader is verified. No documented native combat/AI control API. See [PUPPET_NATIVE.md](PUPPET_NATIVE.md) for actual exports and config; do not add another proxy DLL. |
| [ACBFHapticsBridge — public source](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge) | Published TU 1.0.6/1.0.7/1.0.8 profiles and gameplay/input hooks; source includes state mapping and hook tests. Existing research reports 21 movement/traversal-related enter/exit hooks | **High** for reverse-engineering *observation strategy*, **not** proof of pre-parry window, enemy intent/attack start, native action dispatch or spawn. Distinguish confirmed semantic hooks from Wwise/post-event/physical-input inference. **Non-commercial license; study only without copying code or generated patterns.** See [INTEGRATION_FINDINGS.md](INTEGRATION_FINDINGS.md). |
| [Forge Injector V1 BETA — Nexus #108](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/108) | Standalone injector for Resynced FORGE v50 resource format (with resource-ID/layout constraints) | **Low** for runtime tests; **medium** for a separately validated data-only prototype. Not an arena editor, AI API, or proof of 1.0.6 resource compatibility. Modification of game archives requires backups/restore and separate test isolation. |
| [AnvilToolkit — Nexus site #455](https://www.nexusmods.com/site/mods/455) | Anvil game-asset analysis and resource editing | **Medium** for mapping assets and independently inspecting target data. Not a runtime test driver. Avoid repacking or distributing game-owned files. |
| [AC4Coop — original AC4 research](https://github.com/Kamzik123/AC4Coop) | **Source-confirmed for AC4 x86**: creates an extra puppet, drives its world transform per frame, and replays action aliases/layers, blend factors, animation phase; has manual puppet spawn and logging hotkeys | **High as a reverse-engineering technique, not a Resynced integration**. The puppet's behavior component is disabled in the main mode: animation playback is **not** an AI combat attack or hit resolution. No Resynced x64 addresses can be imported. See source-level follow-up below. |
| [Universal Modder AC4 field notes](https://universal-modder.dev/games/assassin-s-creed-iv-black-flag/) | Public reverse-engineering/workflow leads for **original AC4** | **Exploratory** and version-separated. Verify individual claims and legal terms; no direct compatibility inference for Resynced. |
| [vgamepad — upstream source](https://github.com/yannbouteiller/vgamepad) | Python virtual Xbox 360/DS4 input via `press_button`, `release_button`, `update` API; on Windows requires the **retired ViGEmBus driver** | **Optional fallback, not default recommended dependency**. ViGEmBus has been archived since 2023. Do not silently install a kernel driver or claim this is a maintained end-to-end stack. Device injection is not engine-confirmed action acceptance. |
| [Frida](https://frida.re/) | Dynamic runtime instrumentation and event hooks | **Medium**, only once a safe, legal and version-guarded process boundary is identified. Attaching changes the environment, may be blocked, and is not itself a game API. No guessed offsets or bypass. |
| REA / Ghidra agent idea ([previously cited REA URL](https://github.com/riccardoponte/Reverse-Engineering-REA)) | **Unconfirmed source**: this specific URL returned 404 when accessed directly on 2026-11-17, despite an indexed search result. Generic Ghidra-based analysis remains a possible independent option. | **Do not install, recommend or automate REA from this link** until a verifiable maintained upstream, version and license are established. No implied game-control API. |
| [coop-anything fast-iteration playbook](https://github.com/cranebeaker/coop-anything/blob/main/playbook/fast-iteration.md) | Illustrates an *architecture*: in-game RPC queue on main thread, Python client, state queries, save/load, screenshots and test scripts | **High as architecture reference, not a ready-made Resynced plugin**. API methods exist only after **we** implement and validate their respective engine hooks. Do not equate a generic RPC template with existing combat command access. |

### Additional conservative compatibility notes

- Target is a locally reported **Resynced 1.0.6** installation; exact executable hash, store/distribution variant and installed 64-bit loader ABI are **not captured**. A filename or reported version is not a trustworthy fingerprint.
- Reports of **conflicting `version.dll` proxy loaders** and existing ACBlackFlagFix/Puppet Native installation require a **loader inventory before any install**. Never replace, overwrite or stack DLL proxies blindly. Prefer an already working Puppet Native plugin host *if and only if verified*; keep standalone ASI as a separate alternative.
- Otis_Inf, trainer and Cheat Engine tools may be used for isolated manual reconnaissance, **not concurrently by default** with a diagnostic ASI. They can modify time or state and invalidate an A/B comparison. Establish an unmodified baseline.
- All external tools must be assessed for licensing, safety, source provenance and whether automation requires proprietary files. Do not incorporate third-party binaries, mod assets, signatures, offsets or tables into this proprietary repository without explicit rights.
- None of the listed tools currently establishes a public, verified Resynced interface to **enumerate/spawn NPCs, force a named enemy attack, restore exact AI/RNG state, or create an editable combat arena**. Do not represent those capabilities as implemented.

## Minimal runtime harness: testable boundary

Proposed layers, with any engine-specific method **disabled until verified**:

1. **Fixture / restore:** owner-controlled save and known existing on-foot encounter (no new custom arena). Record build fingerprint and fixture identifier; save-game mutation requires an isolated backup. State precisely whether a fixture was actually reset or only reloaded.
2. **Input driver:** Python-side virtual gamepad with press/hold/release and timestamped input sequence; retain manual fallback. The input driver must avoid physical controllers fighting the virtual device. Track focus, FPS, game pause/speed, input backend and monotonic timestamps.
3. **Semantic observer:** an explicitly version-gated, read-only plugin/ASI that emits only **verified** signals: enemy attack initiation, enemy and player state transitions, parry/counter result, damage and selected native action. Each signal must have a provenance label: `ENGINE_CONFIRMED`, `INFERRED_FROM_EFFECT`, or `UNAVAILABLE`. Do **not** infer pre-action eligibility from haptic output, animation end, post-event audio or physical button presses.
4. **Coordinator:** local-only driver controlling trial ID, preconditions, event time anchor and bounded timeout. Use a deliberately small command surface (e.g. `start_capture`, `stop_capture`, `read_report`) first. Add `reset_fixture` or `trigger_enemy_attack` **only after** verifying a safe supported native seam; never invent those commands as implemented.
5. **Report & analysis:** one sanitized, machine-readable file per combined session: build hash/compatibility result, fixture and mods, input events, trusted state events, temporal reference, FPS/pause/speed, result, error cause and coverage summary. No automatic network upload or protected game data. A trial with missing event provenance is `INCONCLUSIVE`, not `PASS`.
6. **Offline CI:** deterministic tests for driver protocol, parsers, synthetic event sequences, correlation logic, restore/report schema and fail-closed behavior on unsupported builds. CI success does not prove that the game hooks or timings work.

### First concrete test: parry timing

- Precondition: existing enemy encounter and verified player state; capture baseline without other trainers.
- Anchor: **actual enemy attack-start event**, only if an observable native semantic signal has been verified; otherwise mark test **BLOCKED_SEMANTIC_ANCHOR**. Starting a timer at an animation guess or a scripted button press does not produce a valid parry-window measurement.
- Trial: send defensive action at a chosen offset relative to the anchor; log actual input receipt, state transition, damage and parry success, with observed frame/time uncertainty.
- Repeat with several offsets **only if** the enemy action and initial states can be reproduced sufficiently to compare. Otherwise report nondeterminism and stop, rather than publishing false millisecond precision.
- Acceptance: one reproducible, timestamped **game-semantic** pass/fail result with source-labelled evidence; otherwise explicit unsupported status and one identified missing hook.

### Staged development / exit criteria

| Stage | Deliverable | Go / no-go |
| --- | --- | --- |
| **0. Source-first triage** | Choose the *minimum* useful tested technique from HapticsBridge, Otis_Inf and current own diagnostic implementation; audit loader/version/license constraints | Specific source path and expected observation; no game launch for speculation |
| **1. Offline harness skeleton** | Original protocol, synthetic fixture, driver stub, event/report schema, CI tests, with all engine calls disabled | Offline repeatability; unsupported features fail closed |
| **2. Combined single-session semantic probe** | One GitHub Actions-produced opt-in diagnostic bundle, respecting [TEST_STRATEGY.md](TEST_STRATEGY.md), containing only verified safe hooks | Owner's actual loader/build supported; otherwise report exact blocker and **do not** patch |
| **3. One controlled combat scenario** | One successful repeatable test *or* evidence-backed `BLOCKED_*` result with a single highest-value missing interface | Actual outcome observed, not merely input recorded |
| **4. Optional deterministic NPC/arena API** | Version-specific entity enumeration, position/reset, trigger one AI action; later spawn/despawn/custom arena | Requires separate verified native APIs, scoped safety gate #18, rollback tests and no overwrite of protected assets |

**Stop condition:** If player-side automation works but no reliable enemy action anchor/outcome can be observed, do **not** spend more runs on button replay. Investigate one specific semantic source, and document why more automatic trials would currently be misleading.

## Decisions for the near term

1. **First investigate an engine-semantic observer**, not general-purpose bot input or scene editing. Existing project diagnostics already sample XInput; avoid another generic controller logger.
2. **Use existing in-world encounter before building arenas.** Reusable saved state is enough for first feasibility proof; deterministic NPC actions are higher-cost optional enhancement.
3. **Make time manipulation an optional observational tool** (Otis_Inf or ACBFRTools) and independently check whether it changes parry timing and physics.
4. **Keep the target gameplay goal central**: classic contextual X and predictable defense. This harness is supporting infrastructure, **not** a replacement for the playable vertical slice in issue #30.
5. Preserve issue #18 input-interception gate; observation-only hooks and controlled tests do not grant permission to replace native input or ship a gameplay change.

## Source-level follow-up (2026-11-17): actionable findings

This section refines the inventory by inspecting project-authored documentation/source, not by running game builds. **Claim scope matters**: upstream author documentation and synthetic harness checks are not independent Resynced in-game validation.

### 1. What AC4Coop *actually* gives us

**Primary:** [AC4Coop README](https://github.com/Kamzik123/AC4Coop/blob/main/README.md), [`src/phase1.cpp`](https://github.com/Kamzik123/AC4Coop/blob/main/src/phase1.cpp), [`src/netstate.h`](https://github.com/Kamzik123/AC4Coop/blob/main/src/netstate.h). Applies **only to original AC4 `AC4BFSP.exe`, x86**.

- The upstream README reports a working **extra player puppet**: a native `Human`, created/despawned on travel or disconnect, updated **on the game's main-thread Human update**, with transform via `SetGlobalMatrixInternal`. Its `CharacterController::m_DriveEntity` is disabled so ordinary physics does not undo externally supplied position.
- The **working default mode disables puppet behavior/AI**, then plays a remote player's already selected **action** via `SetTargetAction`, including alias/channel/layer, blend array (documented 46 factors), blend-in and phase. This is **animation/state replication**, *not* proof of generating genuine combat attacks, AI decisions, target selection, hit events, or a parryable strike.
- `F6` requests a manual puppet spawn with networking disabled; `F7` dumps player/human context. Those are **original-AC4 instrumentation ideas only**. Upstream also suppresses ambient spawns for co-op determinism; that is not a robust arena constructor.
- **Porting hypothesis to test:** identify Resynced counterparts to `Human`, `EntityBuilder`, `ActionComponent` and the main-thread update, **without importing x86 addresses or struct offsets**. First prove *read-only* player/NPC enumeration, identity and motion; then separately prove a harmless native action readback. Do not call a puppet replayed animation an enemy attack.
- **Decision:** study `src/phase1.cpp` and class wrappers as *conceptual guides*; postpone NPC spawn/forced attack. A minimal harness can use already-existing enemies even if their attack timing varies.

### 2. HapticsBridge state hooks: explicit boundary

**Primary:** [upstream `state_mapping108.json`](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/References/state_mapping108.json), [`test_hooks.cpp`](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/test_hooks.cpp), [non-commercial license](https://github.com/DEadDRop470/Assassins-Creed-Black-Flag-Resynced-Haptics-Bridge/blob/main/Source%20v%202.3.7.9/LICENSE.txt).

- The JSON inventories **21 enter/exit transition hook slots** for TU 1.0.6, 1.0.7 and 1.0.8, for `InAir`, jumps, falling, climbing, swimming, `HayStack`, and other **movement / traversal** states. **No listed parry, combat intent or enemy hitbox state**. Addresses in the upstream file are **research data, not safe reusable signatures for our project**.
- The read `test_hooks.cpp` specifically runs synthetic transactional installation, failure rollback and unknown-layout rejection for **1.0.6 and 1.0.7**. The separate JSON has a 1.0.8 mapping, but **do not assert that this particular synthetic test validates 1.0.8**; no game execution was performed here.
- The same test verifies a game `XInputSetState` **output/rumble** observer, not an XInput `GetState` semantic input resolver. Audible/haptic parry feedback may be a useful **post-outcome correlate**, but cannot independently timestamp **enemy attack start** or **pre-action parry eligibility**.
- **Candidate independent implementation pattern:** feature gate by exact executable build, preflight **every expected target and original pointer**, only install as an atomic transaction, rollback on any partial failure; emit signal availability separately from hook installation status. Do **not** copy hook tables/source from its non-commercial codebase.

### 3. Otis_Inf: precise tool limits

**Primary:** [developer's Resynced camera page](https://opm.fransbouma.com/Cameras/acbfresynced.htm) (v1.0.1; Ubisoft Connect and Steam *claimed*; reported executable process names `acblackflag.exe` and `acblackflag_plus.exe`).

- Author documents **Timestop**, **Frameskip** and adjustable **game speed**; a game-speed value of **1.0** means normal and **0.0** total pause. This is a possible **visual inspection aid**, not documented programmatic frame-advance RPC, frame-accurate capture clock or CPU-independent pause primitive.
- Camera-state slots **restore only camera location/orientation/FoV (and camera-path environment data)**. They do **not** reset the encounter, enemy AI, health, RNG or player animation. Do not label camera-state restore as a test fixture reset.
- Compatible with supported retail channels **according to author**, not proven with the user's build/other injected modules. Tool has separate client/injection process; do **not** make it a silent installation dependency. To measure temporal parry boundaries, establish whether game speed scales both combat state transitions and attack windows; preserve a **normal-speed baseline**.

### 4. Source-backed fail-closed implementation pattern

[AC4Tools](https://github.com/45hgv43whgf/AC4Tools) is a **different**, original-AC4 x86 mod with public source. Its README documents a **specific SHA-256-supported executable**, an image-base gate, pattern uniqueness and **expected-byte checks**, with refusal to install features when the address or hook owner is unexpected. It also documents reversible/safe installation logic and warns that save unlocks can persist or softlock progression.

- **Transferable idea:** proof of exact build + expected target bytes + unique match + hook ownership + rollback + diagnostic reason codes.
- **Not transferable:** original x86 addresses, signatures, patch bytes, DLL, or any assumption that an AC4Tools feature works on Resynced.
- **Do not use save-unlock/character-progression edits** for a disposable combat fixture, because persisted changes can outlive the test.

### 5. Additional Resynced tools: confidence and test contamination

- [ACBFRTools #661](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/661): published Nexus description confirms **F4 menu**, player/ship options, time scale, weather, noclip/free camera, save editing. **No publicly confirmed scripting protocol, NPC spawn or AI attack command** found in the accessed description. Treat version 1.0.6 support as **unknown**.
- [Resynced trainer #616](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/616): author advertises versions **1.0.6–1.0.7**, plus noclip and enemy/ship health cheats. A version-range claim is **not** a successful test on the user's exact executable.
- [Cheats Trainer #541](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/541): page claims **1.0.6–1.0.8**. No documented programmatic NPC action API identified; normal combat test must not include damage/health manipulations.
- [Cheat Happens trainer discussion](https://www.cheathappens.com/show_board2.asp?headID=1009990748&titleID=82890): author lists **Disable Enemies and NPCs**, teleport and speed adjustment; the author specifically warns the NPC-disabling feature **can break game interactions and require save reload**. It is **AI suppression, not scripted AI control**, and is inappropriate for parry trials. The page's current header states **1.0.8**, so it is not evidence of 1.0.6 compatibility.
- [Resynced Modding Workshop #197](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/197): author [describes](https://www.reddit.com/r/ACBFResynced/comments/1w7isjr/black_flag_resynced_modding_workshop/) reversible local file/asset/save editing and backup/restore. Author explicitly says **animations and damage values are not modified**; this is not a runtime action/arena editor. Archive edits only while the game is closed.
- **Test isolation:** use at most **one** optional time/camera helper in a separate reconnaissance profile, never mix `God Mode`, `Easy Parry`, invisible player, suppressed NPC AI, unlimited defense or shifted speed into the baseline test. Record all loaded modules/features and reject contaminated comparisons.

### 6. Agent-driven testing: select useful methods, not an unsupported package

**Primary:** [coop-anything fast-iteration playbook](https://github.com/cranebeaker/coop-anything/blob/main/playbook/fast-iteration.md), [automated QA](https://github.com/cranebeaker/coop-anything/blob/main/playbook/automated-qa.md), [hot-reload guide](https://github.com/cranebeaker/coop-anything/blob/main/playbook/hot-reloading-your-mod.md).

- **Adopt in our own code:** structured action responses with `ok`, `reason`, observed state; event-driven timeouts (no blind fixed sleeps), idempotent scenario setup, `NOT_RUN` vs `PASS` semantics, real process ownership, and **negative-control/mutation tests** demonstrating that the test detects an intentionally disabled feature.
- **Game RPC is not supplied by the playbook**. Its main-thread request-queue pattern requires **a verified frame callback**; it cannot conjure `spawn_enemy`/`perform_attack` methods from nonexistent Resynced hooks.
- **Hot reload only later.** A stable loaded shim + swappable core could accelerate iterations **only after** a safe frame callback and teardown/ownership protocol exist. Upstream notes that outstanding engine-thread callbacks, unregistered threads, chained hooks or pointers into unloaded code can crash a game. Our current read-only diagnostic hosts do **not** satisfy those preconditions.
- The smallest useful early deliverable is a **CI-tested protocol/report parser and synthetic semantic oracle**, not a full agent bot, remote-control server, hot-reload loader, multi-instance pool, or camera UI.

### 7. Input backend viability: correct earlier vgamepad recommendation

**Primary:** [vgamepad README](https://github.com/yannbouteiller/vgamepad), [ViGEmBus maintainer EOL](https://docs.nefarius.at/projects/ViGEm/End-of-Life/), [LizardByte libvirtualhid announcement](https://app.lizardbyte.dev/2026-08-16-introducing-libvirtualhid-and-virtual-hid-driver/).

- The `vgamepad` Python library **is an input API**, but Windows emulation uses **ViGEmBus**, officially **archived and retired in 2023**. Do **not** suggest it as a modern fully supported default or auto-install its bundled driver on the maintainer's PC.
- LizardByte's **actively developed** `libvirtualhid` (MIT) has a Windows **Virtual HID Driver** with a **separate, source-available license and paid machine-license activation**, and requires a supporting host/client integration; it is **not** an established zero-install drop-in replacement for vgamepad. A maintained driver is not automatically the best fit for a no-operator-work research task.
- **Decision:** make the input driver a **mockable interface**. Test press/hold/release report logic in CI with synthetic input. Select an actual physical-virtual input backend only after confirming game recognition and the operator's explicit consent to install any driver/license component; an independently identified native game-command seam may make device emulation unnecessary.

### 8. Hostile/unverifiable research links

The previous `riccardoponte/Reverse-Engineering-REA` link surfaced in indexed search but direct opening returned **404**, and the repository could not be fetched through the GitHub connector on 2026-11-17. **Do not claim it is presently available or installable.** Any future agent RE package must be separately checked for a live upstream, provenance, licensing, supported disassembler and actual maintenance. Existing analysis tools remain usable without this link.

### 9. Gameplay-specific repeatability caveat

[Ubisoft's official Resynced combat breakdown](https://www.ubisoft.com/en-gb/game/assassins-creed/news/1A8XkbnvONciecEZm9my1C/assassins-creed-black-flag-resynced-combat-system-breakdown) distinguishes **normal/perfect parry**, **unparryable attacks**, **dodge/perfect dodge**, and states that enemies **adapt to repeated moves**. This rules out equating an arbitrary replayed defense button with a stable attack stimulus.

Minimal trial metadata must include enemy archetype/identity (if observable), attack classification (parryable/unparryable **or unknown**), target state, distance/orientation, player equipment, speed/FPS, and whether the encounter had already seen repeated player responses. **Separate trials by attack class**, avoid treating an unparryable strike as parry timing failure, and report stochastic behavior as uncertainty, not a deterministic timing-window measurement.

## Revised highest-value sequence for issue #33

1. **No new installed tools yet.** Extract one concrete observation hypothesis from HapticsBridge source without copying patterns; record that its current mapping is *traversal*, not semantic parry.
2. In parallel, study AC4Coop's *pattern* for per-frame state readback/animation channels but keep actual Resynced x64 NPC identity and AI action pathways **UNKNOWN**.
3. Add a lightweight test-oracle schema and **negative-control synthetic tests** to CI. The oracle must reject tests that only observe physical input, haptic effects, or time-dilated frames.
4. Look for one **Resynced 1.0.6 engine-semantic** enemy attack initiation/result seam. If unavailable, produce a concise blocker; do not spend owner game sessions replaying scripted presses.
5. When a source-backed observable hook is actually identified, build an opt-in diagnostic (existing Puppet Native host if compatible) and package one sanitized single-session report. Test any optional camera/trainer separately from the baseline.
6. Pursue programmable NPC attack, reset, spawn, or arena only **after** independent native-interface evidence exists; no broad promise of unattended game control.
