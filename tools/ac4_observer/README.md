# Original AC4 control observer

This is a separate, diagnostic-only x86 ASI for the **original** Windows version of Assassin's Creed IV: Black Flag. It does not share the Resynced plugin's x64 target. It samples controller state and passively observes selected native input/dispatch functions; it never changes arguments, return values, input, or game conditions.

## Evidence boundary

The local Ghidra project and research PR #27 statically establish a candidate path through the assassin-behavior callback, logical input edge/context checks, resolver, and conditional virtual dispatch. The observer records calls to those statically validated sites and their object pointers, logical slot results, flags, and returned internal candidate discriminator. This does **not** prove Edward owns every observed callback, identify candidate discriminator names, identify a runtime class name from a raw pointer, or prove that a selected candidate became a played action.

XInput is polled every 8 ms. A physical-button press may be associated with a nearby logical edge when the edge occurs within 250 ms. The analyzer labels that relationship `inferred_temporal`; it is not a native causal link. The inspected installation imports DirectInput and has a proxy, so XInput polling may not represent the game's actual input provider. Non-XInput controllers, keyboard input, High/Low Profile, mode ownership, named contexts, candidate rejection reasons, concrete virtual method execution, and action start/end/cancel are not currently established by this source.

Runtime pointers are process-local IDs. They are not stable actor identifiers and are not dereferenced for reports. A resolver return is an unlabeled internal value, never an Attack/Counter/Finisher label.

## Exact-build safety

Native hooks are armed only when a **private, untracked profile** matches the executable SHA-256, PE32/i386 image metadata, and all expected function-entry bytes. A missing profile, another executable, any byte mismatch, or any MinHook error disables the whole native source set. The default public/CI build has no profile and stays disarmed. Do not copy a profile between installations or game builds.

The maintainer's local profile is in the ignored `target_profile.local.h`; its source JSON is also ignored. Both are intentionally absent from Git. The local ASI built with that profile must not be distributed. CI builds the same x86 source without a profile and tests safe refusal in a synthetic process. The only third-party hook dependency is MinHook v1.3.4, fetched at build time; see `../../THIRD_PARTY_NOTICES/MINHOOK.txt`.

## Build

Use Visual Studio 2022 Build Tools with the C++ x86 workload and CMake:

```powershell
cmake -S tools/ac4_observer -B build/ac4-observer -A Win32 `
  -DAC4_LOCAL_PROFILE_DIR=tools/ac4_observer
cmake --build build/ac4-observer --config Release --target AC4ControlObserver observer_synthetic_host_test observer_guard_tests
ctest --test-dir build/ac4-observer -C Release --output-on-failure
python -m unittest discover -s tools/ac4_observer -p 'test_*.py' -v
```

For another installation, prepare a new local `target_profile.local.json` only after separately validating its executable in Ghidra. It must list the seven source names in `make_profile.py`, each with an RVA relative to the PE image base and exact bytes read from the function entry. Then generate the private header with `python tools/ac4_observer/make_profile.py --exe "<local AC4BFSP.exe>" --profile tools/ac4_observer/target_profile.local.json`. Do not use addresses from another build. Without a verified private profile, build without `AC4_LOCAL_PROFILE_DIR`; the ASI will still log XInput but will report native hooks disabled and is **not ready for the intended control study**.

## One short readiness check, then the main session

1. Close AC4 if it is running. Copy only `build/ac4-observer/Release/AC4ControlObserver.asi` into the existing `scripts` folder beside the current ASIs. Do not replace `dinput8.dll`, existing ASIs, proxies, or settings.
2. Launch AC4 using the existing launcher. Confirm a new file appears under `%LOCALAPPDATA%\AC4ControlObserver` and contains `native_hook_status` with state `1`. State `0` means do not proceed to the main session.
3. For the short smoke check, use an ordinary, unprepared gameplay interval. Verify the journal includes an XInput edge, a logical rising-edge query, and a candidate dispatch under one `correlation_id`. The physical association, if present, remains temporal inference. Confirm the game accepts normal input and stays stable.
4. If these records are present and behavior is stable, play normally for the main session. No special sequence is needed. The report will list observed and unobserved contexts; one session is not a completeness guarantee.
5. After playing, stop logging without closing the game by running `powershell -ExecutionPolicy Bypass -File tools/ac4_observer/stop_observer.ps1`. The journal is finalized locally. Then close the game and remove only this new ASI.
6. Analyze one or more journals:

```powershell
python tools/ac4_observer/analyze.py `
  "$env:LOCALAPPDATA\AC4ControlObserver\session-*.jsonl" `
  -o ac4-control-map.json
```

PowerShell does not expand quoted wildcards for Python. Pass multiple file paths as separate arguments or use:

```powershell
$logs = Get-ChildItem "$env:LOCALAPPDATA\AC4ControlObserver\session-*.jsonl" | ForEach-Object FullName
python tools/ac4_observer/analyze.py $logs -o ac4-control-map.json
```

If AC4 exits unexpectedly before the stop signal, the final line may be missing. The analyzer recovers complete JSONL records and marks the session incomplete.

## Limits and failure behavior

- Hook installation is all-or-none across the validated native sites. A mismatch logs a disabled status and leaves every game function untouched.
- Hook callbacks perform bounded in-memory queue writes only. A background worker polls and writes JSONL once per second. Queue overflow and journal-size cutoff are recorded.
- Journals rotate per process/session, remain under `%LOCALAPPDATA%`, and are never uploaded. Each file is capped at 128 MiB.
- The ASI does not capture frames, sound, arbitrary keyboard input, save files, or memory dumps.
- The synthetic Windows test verifies DLL loading, profile-mismatch refusal, queue/report startup, and clean stop. It is not evidence that AC4 loads the ASI or that a logged candidate is a particular action.
- Do not begin the main session unless the short readiness check links physical input, logical edge, and candidate-dispatch evidence while ordinary control remains stable.
