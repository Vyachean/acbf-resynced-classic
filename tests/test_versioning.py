"""Offline tests for repository versioning policy."""

import unittest

from scripts.versioning import (
    is_merged_main_pr,
    is_single_parent_commit,
    next_version,
    parse_version,
)


class VersioningTests(unittest.TestCase):
    def test_initial_and_sequential_versions(self):
        self.assertEqual(next_version([]), "v0.1.0")
        self.assertEqual(next_version(["unrelated", "v0.1.0"]), "v0.1.1")
        self.assertEqual(next_version(["v0.1.12", "v0.1.9"]), "v0.1.13")

    def test_strict_version_format(self):
        for tag in ("v01.2.3", "1.0.0", "v1.2", "v1.2.3-rc1", "v1.02.3"):
            self.assertIsNone(parse_version(tag))
        self.assertEqual(parse_version("v1.20.3"), (1, 20, 3))

    def test_single_parent_squash_candidate(self):
        sha = "a" * 40
        parent = "b" * 40
        self.assertTrue(is_single_parent_commit(f"{sha} {parent}", sha))
        self.assertFalse(is_single_parent_commit(f"{sha} {parent} {'c' * 40}", sha))
        self.assertFalse(is_single_parent_commit(sha, sha))
        self.assertFalse(is_single_parent_commit(f"{'d' * 40} {parent}", sha))

    def test_completed_pr_required(self):
        sha = "a" * 40
        repo = "Vyachean/acbf-resynced-classic"
        pr = {
            "merged_at": "2026-11-15T12:00:00Z",
            "merge_commit_sha": sha,
            "base": {"ref": "main", "repo": {"full_name": repo}},
        }
        self.assertTrue(is_merged_main_pr(pr, sha, repo))
        self.assertFalse(is_merged_main_pr({**pr, "merged_at": None}, sha, repo))
        self.assertFalse(is_merged_main_pr({**pr, "merge_commit_sha": "b" * 40}, sha, repo))
        self.assertFalse(is_merged_main_pr({**pr, "base": {"ref": "dev", "repo": {"full_name": repo}}}, sha, repo))


if __name__ == "__main__":
    unittest.main()
