"""Research-only action eligibility and title-update evidence contracts.

The tests ensure provenance and dangerous unknowns remain explicit. They do
NOT verify either game's runtime logic or certify a playable game hook.
"""
import json
from collections import Counter
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "research" / "combat_conditions.json"
LEDGER = ROOT / "research" / "control_system_ledger.json"

MOVES = {
    "Attack", "Combo Kill", "Kill Streak", "Break Defense",
    "Ranged Attack", "Counter Kill", "Counter Disarm", "Counter Throw",
    "Special Counter", "Parry",
}
ARCHETYPES = {"Soldier", "Scout", "Gunner", "Brute", "Captain"}
TU106 = {
    "106_RT_SPRINT_PARKOUR", "106_QUICK_ROPE_INTERRUPT",
    "106_SMOKE_POST_COMBAT_ASSASSINATION", "106_TAKEDOWN_WHILE_VAULTING",
    "106_SWEEP_AWARENESS", "106_HOOD_LEAP", "106_BENCH_BODY_AFTER_LOAD",
}
POST_TARGET = {
    "107_SUSPICIOUS_BURNING_TAKEDOWN", "107_HAYSTACK_ASSASSINATION",
    "107_AIR_ASSASSINATION_TARGET_DIES", "107_PHOTO_MODE_CROUCH",
    "108_WALK_TOGGLE_DIVE", "108_AIM_HIT_CORNER_SMOKE",
    "108_SLEEP_BUSH_ASSASSINATION", "108_NEW_TAKEDOWN_ANIMATIONS",
    "108_NIGHTMARE_PARRY",
}

class CombatConditionsContract(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.catalog = json.loads(DATA.read_text(encoding="utf-8"))
        cls.ledger = json.loads(LEDGER.read_text(encoding="utf-8"))

    def test_scope_is_offline_and_target_binary_unverified(self):
        c = self.catalog
        self.assertEqual(c["schema_version"], 1)
        self.assertEqual(c["status"], "RESEARCH_ONLY_INCOMPLETE")
        self.assertIn("1.0.6", c["intended_build"])
        self.assertIn("unverified", c["intended_build"])
        self.assertFalse(c["ACBFR"]["target_build_verified"])
        self.assertTrue((ROOT / "docs" / "COMBAT_CONDITIONS.md").is_file())
        self.assertFalse((ROOT / "packaging" / "installable.json").exists())
        self.assertEqual(self.ledger["audit_status"],
                         "INCOMPLETE_NO_GAME_HOOKS")
        self.assertEqual(self.ledger["combat_condition_catalog"],
                         "research/combat_conditions.json")

    def test_original_archetypes_have_all_moves_without_boolean_shortcuts(self):
        c = self.catalog["AC4"]
        self.assertEqual(set(c["moves"]), MOVES)
        self.assertEqual(set(c["archetypes"]), ARCHETYPES)
        self.assertIn("not an actionable", c["evidence"])
        for name, row in c["archetypes"].items():
            with self.subTest(enemy=name):
                self.assertFalse(row["gameplay_state_verified"])
                self.assertEqual(set(row["move_effectiveness"]), MOVES)
                self.assertTrue(set(row["move_effectiveness"].values()) <=
                                {"weak", "immune", "unspecified"})
                self.assertNotIn(True, row["move_effectiveness"].values())
                self.assertNotIn(False, row["move_effectiveness"].values())
        brute = c["archetypes"]["Brute"]["move_effectiveness"]
        self.assertEqual(brute["Attack"], "immune")
        self.assertEqual(brute["Counter Kill"], "immune")
        self.assertEqual(brute["Break Defense"], "weak")
        captain = c["archetypes"]["Captain"]["move_effectiveness"]
        self.assertEqual(captain["Break Defense"], "immune")
        self.assertEqual(captain["Parry"], "weak")

    def test_source_conflict_retained_not_silently_resolved(self):
        caveats = {x["id"]: x for x in self.catalog["AC4"]["caveats"]}
        self.assertIn("CAPTAIN_BREAK_DEFENSE_TEXT_CONFLICT", caveats)
        self.assertIn("SOURCE CONTRADICTION",
                      caveats["CAPTAIN_BREAK_DEFENSE_TEXT_CONFLICT"]["description"])
        self.assertIn("BRUTE_COUNTER_DODGE", caveats)
        for record in caveats.values():
            self.assertFalse(record["engine_state_verified"])

    def test_resynced_conditions_qualify_actions_not_just_button(self):
        items = self.catalog["ACBFR"]["conditional_actions"]
        ids = {x["id"] for x in items}
        self.assertEqual(len(ids), len(items))
        self.assertTrue({
            "BREAK_DEFENSE", "PERFECT_PARRY", "CHAIN_CAPTAIN_BRUTE",
            "WALL_KICK", "GROUND_SWEEP", "BRUTE_UNPARRYABLE",
        } <= ids)
        for item in items:
            self.assertTrue(item["required"])
            self.assertTrue(item["limitation"])
        self.assertFalse(self.catalog["ACBFR"]["target_build_verified"])

    def test_patches_are_version_scoped_not_runtime_successes(self):
        c = self.catalog
        fixes = c["patch_fixes"]
        ids = [r["id"] for r in fixes]
        self.assertEqual(len(ids), len(set(ids)))
        self.assertEqual(set(ids), TU106 | POST_TARGET)
        surfaces = {x["id"] for x in self.ledger["surfaces"]}
        sources = set(c["source_registry"])
        version_counts = Counter()
        for record in fixes:
            with self.subTest(patch=record["id"]):
                version = record["version"]
                self.assertIn(version, {"1.0.6", "1.0.7", "1.0.8"})
                version_counts[version] += 1
                self.assertEqual(record["source_id"],
                                 "tu" + version.replace(".", "")[-3:])
                self.assertTrue(record["surfaces"])
                self.assertTrue(set(record["surfaces"]) <= surfaces)
                self.assertIn(record["source_id"], sources)
                self.assertFalse(record["runtime_verified"])
                for key in ("precondition", "reported_behavior_before",
                            "patch_claim"):
                    self.assertTrue(record[key])
                if record["id"].startswith("106_"):
                    self.assertEqual(version, "1.0.6")
                else:
                    self.assertNotEqual(version, "1.0.6")
        self.assertEqual(version_counts, {"1.0.6": 7, "1.0.7": 4,
                                          "1.0.8": 5})

    def test_provenance_is_primary_for_release_fixes(self):
        registry = self.catalog["source_registry"]
        self.assertEqual(registry["tu106"]["kind"], "official_patch_notes")
        self.assertEqual(registry["tu107"]["kind"], "official_patch_notes")
        self.assertEqual(registry["tu108"]["kind"], "official_patch_notes")
        for key, item in registry.items():
            with self.subTest(source=key):
                self.assertTrue(item["url"].startswith("https://"))
                self.assertTrue(item["scope"])

    def test_hard_safety_limits_preserved(self):
        restrictions = self.catalog["safety_invariants"]
        self.assertGreaterEqual(len(restrictions), 6)
        self.assertTrue(any("not an executable eligibility" in x.lower()
                            for x in restrictions))
        self.assertTrue(any("1.0.7/1.0.8" in x for x in restrictions))
        self.assertTrue(any("do not request" in x.lower()
                            for x in restrictions))
        unknowns = {x["id"] for x in self.ledger["critical_unknowns"]}
        self.assertIn("AC4_TARGET_ARCHETYPE_ACTION", unknowns)
        self.assertIn("BFR_106_POST_INTERRUPT_ACTION", unknowns)

if __name__ == "__main__":
    unittest.main()
