"""Tag each successfully validated squash merge on main, from GitHub Actions only."""

import json
import os
import subprocess
import sys
import time
from urllib.error import HTTPError, URLError
from urllib.request import Request, urlopen

from versioning import (
    is_merged_main_pr,
    is_single_parent_commit,
    next_version,
    parse_version,
)

BASELINE_SHA = "9112706e5d12652605323a9a25745ee9609b9d8d"


def git(*args: str) -> str:
    result = subprocess.run(
        ["git", *args], check=True, capture_output=True, text=True
    )
    return result.stdout.strip()


def versions_at(commit: str) -> list[str]:
    return [
        tag for tag in git("tag", "--points-at", commit).splitlines()
        if parse_version(tag) is not None
    ]


def associated_merged_prs(repository: str, sha: str, token: str) -> list[dict]:
    url = f"https://api.github.com/repos/{repository}/commits/{sha}/pulls?per_page=100"
    request = Request(
        url,
        headers={
            "Authorization": f"Bearer {token}",
            "Accept": "application/vnd.github+json",
            "X-GitHub-Api-Version": "2022-11-28",
            "User-Agent": "acbf-resynced-classic-versioner",
        },
    )
    with urlopen(request, timeout=20) as response:
        prs = json.load(response)
    if not isinstance(prs, list):
        raise RuntimeError("Unexpected GitHub API response for commit-associated pull requests")
    return [pr for pr in prs if is_merged_main_pr(pr, sha, repository)]


def main() -> int:
    if os.environ.get("GITHUB_ACTIONS") != "true":
        raise RuntimeError("Version publishing is restricted to GitHub Actions")

    sha = os.environ["GITHUB_SHA"]
    repository = os.environ["GITHUB_REPOSITORY"]
    token = os.environ["GH_TOKEN"]
    if git("rev-parse", "HEAD") != sha:
        raise RuntimeError("Checkout SHA differs from the validated workflow SHA")

    git("fetch", "--force", "--tags", "origin")
    existing = versions_at(sha)
    if existing:
        if len(existing) != 1:
            raise RuntimeError(f"Multiple version tags already point at {sha}: {existing}")
        print(f"Already versioned: {existing[0]} -> {sha}; no changes.")
        return 0

    line = git("rev-list", "--parents", "-n", "1", sha)
    if not is_single_parent_commit(line, sha):
        raise RuntimeError("The main commit is not a one-parent squash candidate")
    parent = line.split()[1]

    # A squash merge must be associated with an actually merged PR into main.
    # A direct push can still reach main if branch rules are missing; fail closed.
    for attempt in range(4):
        prs = associated_merged_prs(repository, sha, token)
        if prs:
            break
        if attempt < 3:
            time.sleep(3)
    else:
        raise RuntimeError("No merged PR on main references this SHA; refusing to tag a direct push")

    if parent == BASELINE_SHA:
        new_tag = "v0.1.0"
    else:
        # Do not assign a later version while an earlier merge is still untagged.
        prev = []
        for attempt in range(13):
            git("fetch", "--force", "--tags", "origin")
            prev = versions_at(parent)
            if prev or attempt == 12:
                break
            time.sleep(5)
        if len(prev) != 1:
            raise RuntimeError(
                f"Previous main commit {parent} must have exactly one version tag; "
                f"found {prev}. Rerun after the previous merge's CI/version job succeeds."
            )
        new_tag = next_version(prev)

    if git("tag", "--list", new_tag):
        raise RuntimeError(f"Version {new_tag} already exists on another commit")

    git("config", "user.name", "github-actions[bot]")
    git("config", "user.email", "41898282+github-actions[bot]@users.noreply.github.com")
    git("tag", "-a", new_tag, sha, "-m", f"Project snapshot {new_tag}")
    git("push", "origin", f"refs/tags/{new_tag}")
    print(f"Created version {new_tag} for validated squash merge {sha}.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (RuntimeError, subprocess.CalledProcessError, HTTPError, URLError) as exc:
        print(f"VERSION ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
