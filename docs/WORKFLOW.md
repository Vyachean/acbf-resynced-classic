# Development and repository policy

This document is **normative** for all future work in this repository.

## Main branch: completed versions only

1. All work happens in a short-lived **feature, fix, research, or chore branch**.
2. A pull request must represent **one logically complete, reviewable increment** with an explicit outcome: verified result, completed research decision, or a self-contained infrastructure change. Do not merge partially implemented runtime features.
3. Merge pull requests to `main` using **Squash and merge only**. No direct pushes, merge commits, or rebase merges. The single squash commit is the unit of project history and versioning.
4. Before each squash merge, the maintainer **manually verifies that all applicable GitHub Actions PR jobs finished successfully** (`repository-contract`, `unit-tests`, `windows-build`). A skipped `version-main` job on PRs is expected; it runs only after merging. **Never merge when CI is queued, running, failed, canceled, or unavailable.**
5. Close or update the linked issue with actual evidence and any unresolved constraints. A failed hypothesis is a valid completed research increment if the outcome is documented.
6. Do not add speculative game compatibility claims, guessed offsets, or copied copyrighted game code.

## Version policy

**Each new squash merge into `main` is a new project version**, regardless of whether it is a playable release.

- Version identifiers are Git tags, not necessarily GitHub Releases or distributable binaries.
- During early development, tags start at **v0.1.0**, then increment the patch component per subsequent successful merge: **v0.1.1**, **v0.1.2**, etc.
- Tags identify exact committed source snapshots; no automatic public release is created.
- The `CI` GitHub Actions workflow validates the merged commit and creates the tag **only after its jobs pass**. It checks that the commit originates from a merged PR and has exactly one parent, which is the expected shape of a squash commit.
- Tagging is idempotent on rerun. It fails closed if the previous version is missing, a commit is not a validated squash merge, or GitHub token permissions are insufficient. A failed tagging job does **not** mean a version was created.
- If we later want formal minor/major SemVer changes, that policy must be intentionally revised. These pre-1.0 numbers currently represent sequential project snapshots.

The repository was initialized by two direct documentation commits **before** this policy was adopted. The baseline is `9112706e5d12652605323a9a25745ee9609b9d8d`; the first compliant PR merge gets v0.1.0.

## Build and automation boundary

**GitHub Actions is the sole build, test-automation, packaging, version-tagging, and artifact-generation environment.**

- Do not require maintainers/operators to install local compilers, run test scripts, prepare archives, execute command lines, or manually calculate versions.
- PR validation runs documentation consistency checks, deterministic unit tests, and (once a Windows CMake project is introduced) a Windows build, tests, and install/staging bundle.
- All generated binaries and diagnostic distributions must come from GitHub Actions artifacts. Never require a local release build.
- A `main` tag is **not** a statement that a compiled binary is stable, nor that a public Release exists.
- Never upload game binaries, encrypted assets, DRM-related files, private dumps or unauthorized third-party content to Actions or artifacts.
- Avoid release artifacts until their game version and supported loader are established.

## Operator / player interaction budget

**Default: zero game launches for research and development.** First exhaust public documentation, source code, static analysis of legally obtained materials, synthetic test fixtures, and CI tests.

For a **pure built-in control experiment** needing no mod (see [X-001](X_INPUT_EXPERIMENT.md)), an earlier owner observation already found a same-button Attack/Takedown collision. **X-001 is deferred** unless a new, materially different hypothesis justifies a bounded test. A no-mod experiment does not require a CI artifact and cannot establish engine eligibility or verify a game mod.

When a **mod-based** real game launch is unavoidable, prepare **a comprehensive, version-safe, opt-in diagnostic build**:

1. One CI-generated archive containing everything legally distributable; no manual compilation or configuration.
2. User copies files into the documented location and launches the game **once**. Any unavoidable gameplay interactions are grouped into the same session.
3. The mod performs safe, noninteractive diagnostics automatically and records a minimal sanitized report locally; all tests that truly require player-controlled context are clearly identified.
4. The report should be a single self-contained file ready to attach; no terminal commands, screenshots of logs, or manual parsing.
5. Stop when sufficient evidence is gathered; batch unknowns so one launch answers multiple questions.
6. A second launch must be justified in a PR/issue by a concrete blocker that cannot be answered from existing evidence.

See [Single-session testing strategy](TEST_STRATEGY.md) for the detailed contract.

## Mandatory pre-merge evidence

A PR description must state: goal, implementation/research summary, linked issue, what automation actually executed and passed, changes requiring game execution (if any), compatibility limitations, and whether this is source-only, diagnostic, or playable.

No unchecked TODO is silently represented as a completed game feature.

## Settings that must be enabled in GitHub

**Documentation and CI cannot enforce branch protection by themselves.** The repository administrator must configure the following once:

- **Settings → General → Pull Requests**: enable **Allow squash merging**; disable **Allow merge commits** and **Allow rebase merging**. Optional: automatically delete head branches.
- **Settings → Rules → Rulesets** (or applicable branch protection): target `main`, require a pull request, block force pushes and branch deletion, and avoid bypasses. The repository currently uses **manual verification of CI before merging**, rather than mandatory status checks in the ruleset (by owner decision). This is a process guard, not an enforceable GitHub merge restriction; a careless or unauthorized merger could still merge failing checks. A future change may enable required status checks when practical.
- **Settings → Actions → General**: permit workflows and ensure `GITHUB_TOKEN` has permission to create tags as needed. The versioning job explicitly requests `contents: write`.
- The owner configured squash-only repository merge methods and an active PR-only ruleset. **Required CI status checks are intentionally not configured**; inspect the actual workflow jobs in Actions before every merge, and verify version tagging after.

## Mandatory pre-implementation audit gate

**Issue [#18](https://github.com/Vyachean/acbf-resynced-classic/issues/18) governs gameplay-facing input changes through a feature-scoped gate.** The whole-system audit is an ongoing reference and is mandatory for a **global** profile, not an independent prerequisite for every narrow feature. Before accepting a PR that installs an input hook, disables native input, remaps player actions, or marks a version playable, require:

- [Source-labelled control-system audit](CONTROL_SYSTEM_AUDIT.md) updated for **every affected context and all uses of consumed input in that context**. All unrelated or unknown ship, water, menus and special states must stay **native pass-through**, not be guessed or intercepted.
- All input gestures, RT/LT modifiers, tap/hold/release, secondary counter commands and mode transitions that share the button inventoried.
- All **relevant** candidate AC4-vs-Resynced semantic equivalences verified for the enabled feature, *especially* B hold-block / tap-counter submenu whenever B is affected, and separate Resynced parry/dodge when defense inputs are affected.
- Exact executable/version gates and native fallback when current game mode or action eligibility is unknown.
- No unexplained behavior changes to unrelated original actions.

The current offline C++ resolver is a **hypothesis**, not a reference implementation. Passing synthetic CTest does not satisfy this gate. A research PR may add evidence and coverage without opening the gate. A narrow feature can pass its own scoped gate while the broader catalog remains open; do not close #18 prematurely.

## Security and compatibility

Unrecognized game builds must fail closed. Any future runtime module must provide an easy disable/uninstall path and must not enable diagnostic collection without an explicit testing build/configuration. No unsolicited network reporting.
