# AC4 offline combat sandbox (no game integration)

`acbf_combat_arena` is an **interactive terminal demonstration** built from the same source-backed synthetic `acbf::legacy::resolve` policy used by unit tests. It **does not** inspect, load, inject into, reset or control Assassin's Creed IV or Black Flag Resynced; it has no NPC entity management and no timing/physics oracle.

Illustrative console commands:

```text
set phase melee
set enemy soldier
press b tap
press x tap
```

This models entrance into an **assumed** counter follow-up and selection of a kill candidate. `set enemy brute` illustrates the evasive exception, `set warnings 2` a double-counter candidate; `set phase unknown` must remain unresolved. `--demo` runs a deterministic smoke test.

**Build/test channel:** only GitHub Actions Windows CI (see `combat_arena_demo` CTest). The tool is neither a distributable game mod nor a standalone owner setup requirement; there is no reason to install or run it with the game.

See [offline legacy model](LEGACY_COMBAT_MODEL.md), [reference mechanics](CONTROL_SYSTEM_AUDIT.md), [project status](PROJECT_STATUS.md), and live gameplay research [issue #30](https://github.com/Vyachean/acbf-resynced-classic/issues/30).
