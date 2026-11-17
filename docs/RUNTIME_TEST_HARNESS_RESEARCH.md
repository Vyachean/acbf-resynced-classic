# Runtime test harness and controlled combat testing — external-tool leads

**Research date:** 2026-11-17. **Status:** public-source inventory and proposed architecture; **no in-game tests, engine NPC-control API, or Resynced 1.0.6 compatibility confirmed**. This document records tools worth evaluating, not dependencies or permission to install them. Related: [integration findings](INTEGRATION_FINDINGS.md), [single-session strategy](TEST_STRATEGY.md), [Puppet Native ABI](PUPPET_NATIVE.md), and issues [#30](https://github.com/Vyachean/acbf-resynced-classic/issues/30), [#10](https://github.com/Vyachean/acbf-resynced-classic/issues/10), [#18](https://github.com/Vyachean/acbf-resynced-classic/issues/18).

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
| [AC4Coop — original AC4 research](https://github.com/Kamzik123/AC4Coop) | Original AC4 co-op project and possible reference for character/entity synchronization | **Exploratory** only. **Do not assert** that it offers a ready-to-use Resynced NPC spawn/control path, or transplant x86/original AC4 addresses into x64 Resynced. Earlier reports of EntityBuilder/transform experiments require source-level independent validation before use as facts. |
| [Universal Modder AC4 field notes](https://universal-modder.dev/games/assassin-s-creed-iv-black-flag/) | Public reverse-engineering/workflow leads for **original AC4** | **Exploratory** and version-separated. Verify individual claims and legal terms; no direct compatibility inference for Resynced. |
| [vgamepad — upstream source](https://github.com/yannbouteiller/vgamepad) | Python virtual Xbox 360/DS4 controller on Windows via a virtual gamepad backend | **High** for *repeatable physical input* sequences only. Confirm backend installation/maintenance, game recognition, input focus and timing; controller events alone cannot establish semantic combat success. Synthetic input cannot create a deterministic enemy attack. |
| [Frida](https://frida.re/) | Dynamic runtime instrumentation and event hooks | **Medium**, only once a safe, legal and version-guarded process boundary is identified. Attaching changes the environment, may be blocked, and is not itself a game API. No guessed offsets or bypass. |
| [REA / Ghidra agent workflow](https://github.com/riccardoponte/Reverse-Engineering-REA) | Agent-assisted native binary inspection through documented reverse-engineering backends | **Medium** for bounded static investigations; no claim of autonomous in-game control. Respect original-game-source/asset restrictions and reproducible findings. |
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
