"""Deterministic early-development project snapshot versions."""

import re
from collections.abc import Iterable

VERSION_TAG = re.compile(r"^v(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)$")


def parse_version(tag: str) -> tuple[int, int, int] | None:
    match = VERSION_TAG.fullmatch(tag)
    if match is None:
        return None
    return tuple(int(part) for part in match.groups())


def next_version(tags: Iterable[str]) -> str:
    versions = [parsed for tag in tags if (parsed := parse_version(tag)) is not None]
    if not versions:
        return "v0.1.0"
    major, minor, patch = max(versions)
    return f"v{major}.{minor}.{patch + 1}"


def is_single_parent_commit(parent_line: str, sha: str) -> bool:
    parts = parent_line.strip().split()
    return len(parts) == 2 and parts[0] == sha and len(parts[1]) == 40


def is_merged_main_pr(pr: dict, sha: str, repository: str) -> bool:
    return (
        pr.get("merged_at") is not None
        and pr.get("merge_commit_sha") == sha
        and pr.get("base", {}).get("ref") == "main"
        and pr.get("base", {}).get("repo", {}).get("full_name") == repository
    )
