"""Validate the research fixtures without claiming real-game coverage."""

import json
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CASES_PATH = ROOT / "research" / "reference_scenarios.json"
TESTING_DOC = ROOT / "docs" / "TESTING.md"
REFERENCE_DOC = ROOT / "docs" / "CONTROL_REFERENCE.md"

EXPECTED_IDS = {
    "X-01", "X-02", "X-03", "X-04", "X-05",
    "B-01", "B-02", "A-01", "RT-01", "M-01",
}


class ReferenceScenarioTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.fixture = json.loads(CASES_PATH.read_text(encoding="utf-8"))

    def test_fixture_is_explicitly_unverified(self):
        self.assertEqual(self.fixture["schema_version"], 1)
        self.assertIs(self.fixture["verified_game_behavior"], False)
        self.assertIn("no in-game pass/fail", self.fixture["note"].lower())

    def test_case_ids_are_unique_and_match_test_plan(self):
        cases = self.fixture["cases"]
        ids = [case["id"] for case in cases]
        self.assertEqual(len(ids), len(set(ids)))
        self.assertEqual(set(ids), EXPECTED_IDS)
        document = TESTING_DOC.read_text(encoding="utf-8")
        for case_id in ids:
            with self.subTest(case=case_id):
                self.assertIn(case_id, document)

    def test_every_case_is_a_design_target_not_a_game_result(self):
        for case in self.fixture["cases"]:
            with self.subTest(case=case["id"]):
                self.assertEqual(case["source_status"], "design_target")
                self.assertEqual(case["engine_eligibility"], "unknown")
                for key in ("surface", "input", "situation", "desired_intent"):
                    self.assertIsInstance(case[key], str)
                    self.assertTrue(case[key].strip())
                self.assertNotIn("result", case)
                self.assertNotIn("passed", case)
                self.assertNotIn("hook_offset", case)

    def test_reference_document_explains_uncertainty(self):
        doc = REFERENCE_DOC.read_text(encoding="utf-8").lower()
        for term in ("unverified", "version", "conflict", "unknown"):
            with self.subTest(term=term):
                self.assertIn(term, doc)


if __name__ == "__main__":
    unittest.main()
