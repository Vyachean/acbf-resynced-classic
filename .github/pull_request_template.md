## Summary

<!-- One logically complete, reviewable increment. Describe an actual outcome, not an intention. -->

## Linked issue

<!-- Closes #... or Related #... -->

## Evidence

- [ ] Repository-contract and offline tests completed in GitHub Actions
- [ ] Windows build/test/package job completed (or correctly marked no source yet)
- [ ] Documentation explains known limitations and unknowns
- [ ] No third-party game binaries/assets, secrets, or unauthorized code

**Actual test results / Actions run:**

## In-game testing

- [ ] No game launch needed; offline/static evidence is sufficient
- [ ] A single-session diagnostic plan and CI artifact are ready (link below)
- [ ] Already validated on a named game build; evidence is attached

**If a game launch is unavoidable:** which unknown(s) require it, how many launches, which cases will be captured in that one session, and where is the CI-generated bundle?

## Change classification

- [ ] Research/documentation-only
- [ ] Tooling/CI-only
- [ ] Source-only / not playable yet
- [ ] Diagnostic runtime build
- [ ] Playable feature verified on specified game version

## Merge checklist

- [ ] Scope is complete; this is not an incomplete runtime feature
- [ ] Branch targets `main` for **squash merge only**
- [ ] Merge will create the next automatic **version tag** after CI passes; no GitHub Release is implied

See [Development policy](../docs/WORKFLOW.md) and [Single-session test strategy](../docs/TEST_STRATEGY.md).
