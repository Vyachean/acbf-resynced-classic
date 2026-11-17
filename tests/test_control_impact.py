"""Controller impact analysis must never silently discard documented actions."""
import json
import sys
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
from control_impact import BUTTONS, analyze, physical_buttons, ambiguous_binding

class ControlImpactTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.ledger = json.loads(
            (ROOT / "research" / "control_system_ledger.json").read_text(encoding="utf-8"))

    def test_lexer_reads_chords_without_guessing_actions(self):
        self.assertEqual(physical_buttons("RT + X"), ("X", "RT"))
        self.assertEqual(physical_buttons("LT + DPadLeft"), ("LT", "DPAD"))
        self.assertEqual(physical_buttons("L3 + R3"), ("L3", "R3"))
        self.assertEqual(physical_buttons("RT_released"), ("RT",))
        self.assertEqual(physical_buttons("one_of_four_face_buttons"),
                         ("X", "Y", "A", "B"))
        self.assertEqual(physical_buttons("unverified"), ())
        self.assertEqual(physical_buttons("left_stick_direction"), ())
        self.assertTrue(ambiguous_binding("Y? / context",
                                          "feature_described_binding_unverified"))
        self.assertFalse(ambiguous_binding("X", "source_described_not_runtime_verified"))

    def test_both_games_and_all_buttons_produce_impact_only_report(self):
        for game in ("AC4", "ACBFR"):
            for button in BUTTONS:
                with self.subTest(game=game, button=button):
                    result = analyze(self.ledger, game, button)
                    self.assertEqual(result["override_permission"],
                                     "IMPACT_REPORT_ONLY")
                    self.assertEqual(result["audit_status"],
                                     "INCOMPLETE_FEATURE_SCOPED")
                    self.assertTrue(result["scope"].startswith("ALL_"))
                    self.assertTrue(all(row["runtime_verified"] is False
                                       for row in result["exact_binding_actions"]+
                                       result["unknown_binding_actions"]))
                    self.assertTrue(all(row["native_action_priority"] == "UNKNOWN"
                                       for row in result["exact_binding_actions"]))

    def test_ground_and_ship_actions_not_lost(self):
        expectations = {
            ("ACBFR", "X"): {"crouch", "kick", "brace"},
            ("ACBFR", "B"): {"dodge", "parkour_down", "dive_down",
                               "lower_sails", "exit_spyglass", "quick_use_rope"},
            ("ACBFR", "Y"): {"takedown", "assassinate", "interact",
                               "human_shield", "follow_sea", "board_ship"},
            ("ACBFR", "A"): {"parkour_jump", "swim_up", "raise_sails",
                               "mark_target"},
            ("ACBFR", "RB"): {"basic_attack", "heavy_strike",
                                "hang_enemy_below", "swivel"},
            ("AC4", "B"): {"guard_block", "counter_entry", "counter_throw",
                              "interact", "stealth_dive", "decelerate"},
            ("AC4", "A"): {"break_defense", "human_shield",
                              "manual_jump", "accelerate"},
            ("AC4", "RT"): {"sprint_free_run", "high_profile_assassinate",
                               "fire_aimed", "shoot"},
        }
        for (game, button), required in expectations.items():
            with self.subTest(game=game, button=button):
                actions = {r["action"] for r in
                           analyze(self.ledger, game, button)["exact_binding_actions"]}
                self.assertTrue(required <= actions, required-actions)

    def test_ambiguous_sources_never_claim_unaffected(self):
        for game, button, expected in (
            ("ACBFR", "X", "stealth_position_selection"),
            ("AC4", "X", "tablet"),
            ("AC4", "B", "special_mode"),
        ):
            with self.subTest(game=game, button=button):
                unknown = {r["action"] for r in
                    analyze(self.ledger, game, button)["unknown_binding_actions"]}
                self.assertIn(expected, unknown)

    def test_every_catalogued_action_sharing_key_is_reported(self):
        for game in ("AC4", "ACBFR"):
            for button in BUTTONS:
                with self.subTest(game=game, button=button):
                    discovered = {(r["surface"], r["action"], r["input"])
                        for r in analyze(self.ledger, game, button)["exact_binding_actions"]}
                    expected = {(s["id"], a["action"], a["input"])
                        for s in self.ledger["surfaces"]
                        for a in s[game]["actions"]
                        if button in physical_buttons(a["input"])}
                    self.assertEqual(discovered, expected)

    def test_reject_nonexistent_buttons_or_game_modes(self):
        for game, button in (("AC4", "Q"), ("ACBFR", "RB2"), ("OTHER", "X")):
            with self.subTest(game=game, button=button):
                with self.assertRaises(ValueError):
                    analyze(self.ledger, game, button)

if __name__ == "__main__":
    unittest.main()
