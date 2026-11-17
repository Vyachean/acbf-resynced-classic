# Test automation, controlled combat scenarios and runtime diagnostics

**Both games:** original AC4 **x86** and Resynced **x64**. **Owner's Resynced target:** reportedly 1.0.6, game-facing compatibility not independently proven. Reviewed 2026-11-17. The research target is controlled **game-semantic evidence** (not fabricated “passed” controller events).

## What is actually possible with public tools

| Technique | Original AC4 | Resynced | Claim boundary |
| --- | --- | --- | --- |
| Save-based scenario | Native game saves; author mods may alter them | Native saves; [QuickSaveF5 #536](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/536) author claims native save routine | Save/load may not reset RNG, NPC AI memory, mission scripting, animation phase or attack selection |
| Camera/world pause | [LionAG CameraTool](https://github.com/LionAG/ACBlackFlag-CameraTool) for *Uplay 1.07*; [AC4Tools](https://github.com/45hgv43whgf/AC4Tools) supported Steam fingerprint | [Otis_Inf](https://opm.fransbouma.com/Cameras/acbfresynced.htm) Timestop/Frameskip and [ACBFRTools](https://www.nexusmods.com/assassinscreedblackflagresynced/mods/661) time scale | Camera-state save only saves camera. Sim time changes can alter physics, timing and game behavior |
| Repeat controller edges | Keyboard APIs or virtual controller possible, not currently game-proven | Same; project has synthetic XInput diagnostics | Input sent ≠ input accepted, and player button cannot deterministically trigger enemy |
| Virtual gamepad | [vgamepad](https://github.com/yannbouteiller/vgamepad) supports X360/DS4 Python API | Same physical report path only | Windows backend **ViGEmBus is retired**. Installing any kernel driver requires explicit approval; don't silently install |
| Semantic game event observer | AC4Coop reads actor animation/action snapshots for its build | HapticsBridge movement enter/exit and effect correlation | No authoritative Resynced enemy attack initiation, pre-parry window or true finisher eligibility established |
| Reset/trigger NPC action | Original AC4Coop can spawn an **AI-disabled puppet**, not force a true enemy attack | No verified native method | Neither game has a confirmed drop-in arena scripting SDK |
| Real automated QA architecture | [coop-anything](https://github.com/cranebeaker/coop-anything) includes examples across other engines | Can adapt patterns with new engine-specific bridge | Examples are not an AC4/Resynced plugin |

## Separate the testing layers

1. **Synthetic offline logic:** deterministic input/event/state oracle, hook rollback checks, mock executable fingerprint, command protocol; GitHub Actions is the sole automated build and CI environment for this repository.
2. **Read-only native integration:** single opt-in plugin with source-labelled events and feature-detection statuses; only on exact executable fingerprint and compatible existing loader.
3. **In-game scenario:** the real opponent, a visible attack, player response, engine-semantic result and sanity checks of context; owner starts game **once** when needed.
4. **Game control API (not available yet):** reset fixture, enumerate NPC, set AI task, spawn/destroy enemy, seed RNG, advance frames. These are **investigation goals**, not fake methods to expose in a premature RPC server.

## Proposed minimal test protocol

### Event schema to implement independently

```text
trial_id; build_sha256_local; game; mod_stack_hash
fixture_id; enemy_archetype; enemy_id_if_observable; environment
fps_samples; game_timescale; pause_state
event{source, monotonic_time, optional_frame, name, entity_id, confidence}
input{controller_type, attempted_time, observed_game_input_time_if_available, button, edge}
outcome{PASS|FAIL|INCONCLUSIVE|NOT_RUN|UNSUPPORTED, oracle_source, reason, uncertainty_ms}
```

**Source labels:** `ENGINE_CONFIRMED` (known native state), `INFERRED_FROM_EFFECT` (animation, damage, haptic), `INPUT_ONLY` (button delivered, no semantic acceptance), `UNAVAILABLE`. A trace consisting only of `INPUT_ONLY` **can never be PASS for game behavior**.

### First parry timing case

1. Identify one existing enemy/encounter and a reversible save fixture. Record game build, enemy type, equipment, combat state and baseline without time/health/invisibility cheats.
2. Observe **enemy attack start** from a verified engine event; if unavailable, return `BLOCKED_SEMANTIC_ANCHOR`. Screen cue/rumble timing can provide a **qualitative observation**, not precision millisecond ground truth.
3. Emit a defense input with a recorded delay **relative to a semantic anchor**. An input-only `sleep(100)` is not deterministic game-time scheduling.
4. Validate actual reaction by verified parry event or game-authoritative health/state update, not merely an animation/no-health-change. **Classify parryable vs unparryable**; enemy adaptation and other NPC attacks are confounders.
5. Repeat only when initial conditions are independently comparable. Positive **and negative** controls must be able to fail. If one combat event cannot be observed, stop and specify exactly which interface is missing.

### Contextual X finisher case

- Precondition **before X**: enemy target/position, eligible action type and player mode must be observed, otherwise `UNKNOWN_ELIGIBILITY`.
- Action requested: one semantic Attack vs Takedown, with an explicit decision recorded before any native input suppression.
- Outcome: native engine dispatch and game-accepted finisher, not a modified finishing animation after lethal normal attack. No double action; unknown/ship/swim/menu/parkour context native pass-through.
- Governed by [whole input audit](../CONTROL_SYSTEM_AUDIT.md) and [#18 safety gate](https://github.com/Vyachean/acbf-resynced-classic/issues/18). Passing synthetic C++ tests alone cannot open this gate.

## External automation tooling: details / limitations

- [vgamepad](https://github.com/yannbouteiller/vgamepad) `VX360Gamepad`/`VDS4Gamepad`, `press_button`, `release_button`, `update` are published APIs. **Windows ViGEmBus**, its default virtual device backend, is [end-of-life](https://docs.nefarius.at/projects/ViGEm/End-of-Life/); do not default to a discontinued kernel-driver stack.
- [LizardByte libvirtualhid](https://app.lizardbyte.dev/2026-08-16-introducing-libvirtualhid-and-virtual-hid-driver/) is an alternative under development but has a separately licensed/activated Windows driver and requires custom integration. **No zero-install replacement** proven.
- [Frida](https://frida.re/) dynamic instrumentation and [Ghidra](https://github.com/NationalSecurityAgency/ghidra) static analysis are valuable **once** a legal/fingerprinted executable and suitable feature boundary exist; tools do not discover semantic functions by themselves.
- [coop-anything automated QA](https://github.com/cranebeaker/coop-anything/blob/main/playbook/automated-qa.md): test bot inside native engine, truthful oracle, cleanup/saves hashed, no hollow passes, real process ownership, event-driven waits, negative/mutation checks. **Architecture only**, not a game-ready Black Flag mod.
- [coop-anything fast iteration](https://github.com/cranebeaker/coop-anything/blob/main/playbook/fast-iteration.md): stable in-game bridge, RPC queue drained on main thread, fixture reset and explainable commands; these parts require a **verified game update callback** to implement.
- [Hot reload guide](https://github.com/cranebeaker/coop-anything/blob/main/playbook/hot-reloading-your-mod.md): only after safe callbacks/unhook exists. Engine callback or borrowed pointer into unloaded code may crash the entire game. Not a P0 feature.
- [LionAG AC4 CameraTool](https://github.com/LionAG/ACBlackFlag-CameraTool) says activating first person while sailing may crash and world-stop may leave NPCs posed incorrectly. Never use it as a “fully deterministic freeze” without comparing native behavior.
- [Otis_Inf camera](https://opm.fransbouma.com/Cameras/acbfresynced.htm) exposes camera/time controls for BFR, but published docs do **not** state an RPC interface for scripted frameskip, AI, scene state.

## Automated test oracle failure codes

| Code | Interpretation |
| --- | --- |
| `UNSUPPORTED_BUILD` | Executable hash/bitness or expected bytes do not match; no invasive hooks permitted |
| `LOADER_CONFLICT` | Proxy/module ownership unclear or another plugin owns hook |
| `HOOK_UNAVAILABLE` | Source-backed semantic hook absent / read-only observability incomplete |
| `BLOCKED_SEMANTIC_ANCHOR` | Cannot anchor attack to a verified start event |
| `INCONCLUSIVE_OUTCOME` | Input or animation seen but not enough native evidence for pass/fail |
| `INCOMPARABLE_FIXTURE` | Different enemy attack class, AI history, speed, mods or RNG |
| `TIMEOUT` | No progress within monotonic wall-clock bound |
| `NOT_RUN` | Planned step was not executed; **not** passing |
| `PASS` | Exact stated semantic behavior proven for the recorded game build and context |

**No false precision:** frame count and monotonic clocks, presentation latency, game time and audio output are different domains. Timestamp source/uncertainty must travel with every event.

## Minimal operator interaction / privacy

- No owner compilation, local terminal commands, manual log parsing or special repeated combat sequences if tests can be performed offline.
- Actual game tests use one version-safe CI-built diagnostic, one run and local single sanitized report, with optional short naturally played encounter when automatic scene control does not exist.
- Do not upload `.exe`, game binaries, copyrighted `.data`/`.forge`, memory dumps, machine paths, secrets or unrelated keystrokes; no remote telemetry.
- Keep untouched save backup, rollback/uninstall, strict opt-in and off-by-default instrumentation; do not overwrite `version.dll` or another installed proxy.
- See [the normative project testing policy](../TEST_STRATEGY.md), [diagnostics](../DIAGNOSTICS.md) and [expanded harness research](../RUNTIME_TEST_HARNESS_RESEARCH.md).

## What would materially unlock unattended tests?

**One authentic game update callback, semantic attack-start/event source, and read-only combat outcome signal**. With only these, a meaningful *observational* test and good failure report becomes possible, even without forced AI attacks. A full deterministic arena further requires independently proven AI action / spawn / reset controls. Invest in these game-specific seams before adding a broad bot framework or driver.
