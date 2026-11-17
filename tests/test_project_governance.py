"""Offline project-status and feature-safety policy consistency checks.

This validates *merged documentation*, not native combat behavior or GitHub
branch protection, and uses only the Python standard library.
"""

from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CANONICAL_DOCS = (
    "README.md",
    "docs/ROADMAP.md",
    "docs/WORKFLOW.md",
    "docs/ARCHITECTURE.md",
    "docs/TESTING.md",
    "docs/TEST_STRATEGY.md",
    "docs/RESEARCH.md",
)


class ProjectGovernanceTests(unittest.TestCase):
    def test_project_status_and_knowledge_base_are_visible(self):
        readme = (ROOT / "README.md").read_text(encoding="utf-8")
        self.assertIn("docs/PROJECT_STATUS.md", readme)
        self.assertIn("docs/knowledge/README.md", readme)
        self.assertIn("research/modding_sources.json", readme)
        self.assertLess(
            readme.index("## Modding knowledge base"),
            readme.index("## Project documents"),
        )
        self.assertGreater(
            readme.index("- [Scope and non-goals]"),
            readme.index("## Project documents"),
        )
        status = (ROOT / "docs/PROJECT_STATUS.md").read_text(encoding="utf-8")
        self.assertIn("/issues/18", status)
        self.assertIn("/issues/30", status)
        self.assertIn("no playable", status.lower())

    def test_feature_scoped_gate_does_not_regress_to_global_blocker(self):
        for path in CANONICAL_DOCS:
            text = (ROOT / path).read_text(encoding="utf-8").lower()
            with self.subTest(document=path):
                self.assertIn("feature-scoped", text)
                self.assertNotIn(
                    "blocked until the full-surface audit is complete", text
                )
                self.assertNotIn(
                    "full-system control audit #18 is mandatory before any game-facing work",
                    text,
                )
                self.assertNotIn(
                    "blocked by full control audit #18", text
                )

    def test_unverified_game_features_are_not_claimed(self):
        status = (ROOT / "docs/PROJECT_STATUS.md").read_text(encoding="utf-8")
        self.assertIn("No playable Resynced mod", status)
        self.assertIn("not in-engine", status.lower() if "not in-engine" in status.lower() else "not in-engine")
        self.assertIn("synthetic", status.lower())


if __name__ == "__main__":
    unittest.main()
