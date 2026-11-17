# Offline combat sandbox

`acbf_combat_arena` is an interactive console sandbox for the documented AC4
policy model. It lets us exercise the counter follow-up sequence and enemy
exceptions quickly, using the same C++ resolver that the offline tests call.

Build with CMake, then run `acbf_combat_arena` (or `acbf_combat_arena.exe` on
Windows). The basic flow is:

```text
set phase melee
set enemy soldier
press b tap
press x tap
```

The B tap enters the counter follow-up phase; X selects a counter kill. In that
phase, B, A, and Y select throw, disarm/stun, and the selected tool. Try
`set enemy brute` before the B tap to exercise the evade exception, or set
`set warnings 2` for a simultaneous double-counter.

Run `acbf_combat_arena --demo` for a deterministic smoke scenario. CTest runs
that scenario as `combat_arena_demo`.

This sandbox feeds explicit scenario labels into the policy model. It does not
observe a live game, infer prompt timing, inject controller input, or prove
that the Resynced game uses these rules. The target-game loader and native
action route remain separate work.
