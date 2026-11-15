"""Structural guard for the game-control audit; does NOT validate gameplay."""
import json
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
LEDGER = ROOT / "research" / "control_system_ledger.json"
GAME_IDS = {"AC4", "ACBFR"}
EVIDENCE = {"source_described_not_runtime_verified",
            "feature_described_binding_unverified"}
SOURCE_KINDS = {
    "official_manual", "original_game_guide", "secondary_reference",
    "secondary_launch_keymap", "official_design_document",
    "official_features", "official_controls_help", "community_report",
}
REQUIRED_SURFACES = {
    "ground_idle", "high_profile_travel", "manual_jump", "climb_and_ledge",
    "combat_neutral", "combat_incoming_attack", "combat_counter_followup",
    "combat_finisher", "combat_lockon_and_camera", "stealth_awareness",
    "stealth_assassination", "stealth_nonlethal", "tools_aim_and_loadout",
    "interactions_context", "surface_swimming", "underwater", "ship_helm",
    "ship_weapons", "ship_spyglass", "ship_boarding", "ship_shanties",
    "menu_map_ui", "modern_day_ac4", "hunting_harpooning",
    "original_multiplayer",
}
CRITICAL_HAZARDS = {
    "AC4_B_TAP_HOLD_COUNTER", "AC4_B_COUNTER_FOLLOWUP",
    "AC4_RT_PROFILE", "AC4_A_GUARDBREAK_JUMP", "AC4_X_ASSASSINATE_KILLCHAIN",
    "BFR_RT_A_B_ABILITIES", "BFR_Y_OVERLAP", "BFR_UNPARRYABLE_ATTACK",
    "BFR_SHARED_BUTTON_OBSERVED", "BFR_WATER_SHIP",
    "BFR_PARKOUR_SETTINGS", "BFR_VERSION_PROFILE",
}
CRITICAL_UNKNOWN_IDS = {
    "AC4_COUNTER_TIMING_INTERRUPTS", "AC4_X_PRIORITY",
    "BFR_NATIVE_ELIGIBILITY", "BFR_NATIVE_DISPATCH",
    "BFR_MANUAL_JUMP_MODES", "BFR_TOOL_MODIFIERS",
    "GAMEPAD_VERSION_OPTIONS",
}


class ControlSystemAuditContract(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.ledger = json.loads(LEDGER.read_text(encoding="utf-8"))

    def test_audit_explicitly_incomplete_and_gameplay_blocked(self):
        l = self.ledger
        self.assertEqual(l["schema_version"], 1)
        self.assertEqual(l["audit_status"], "INCOMPLETE_NO_GAME_HOOKS")
        self.assertFalse((ROOT / "packaging" / "installable.json").exists())
        self.assertIn("all_runtime_overrides", {
            b for unknown in l["critical_unknowns"]
            for b in unknown["blocked_features"]
        })
        self.assertTrue((ROOT / "docs" / "CONTROL_SYSTEM_AUDIT.md").exists())

    def test_source_provenance(self):
        sources = self.ledger["source_registry"]
        self.assertGreaterEqual(len(sources), 12)
        for source_id, source in sources.items():
            with self.subTest(source=source_id):
                self.assertIn(source["kind"], SOURCE_KINDS)
                self.assertTrue(source["url"].startswith("https://"))
                self.assertTrue(source["scope"])
        for kind in ("official_manual", "secondary_launch_keymap",
                     "official_design_document", "original_game_guide"):
            self.assertIn(kind, {source["kind"] for source in sources.values()})

    def test_every_control_surface_has_scoped_actions_and_unknowns(self):
        l = self.ledger
        surfaces = l["surfaces"]
        ids = [surface["id"] for surface in surfaces]
        self.assertEqual(len(ids), len(set(ids)))
        self.assertEqual(set(ids), REQUIRED_SURFACES)
        count = 0
        for surface in surfaces:
            with self.subTest(surface=surface["id"]):
                self.assertTrue(surface["label"])
                self.assertTrue(surface["unresolved"])
                for game in GAME_IDS:
                    self.assertEqual(surface[game]["engine_priority"], "UNKNOWN")
                    actions = surface[game]["actions"]
                    count += len(actions)
                    for action in actions:
                        self.assertTrue(action["action"])
                        self.assertTrue(action["when"])
                        self.assertTrue(action["gesture"])
                        self.assertIn(action["evidence"], EVIDENCE)
                        self.assertTrue(action["sources"])
                        self.assertTrue(set(action["sources"]) <= set(l["source_registry"]))
                        if ("unknown" in action["input"].lower() or
                                action["input"] == "unverified" or
                                "?" in action["input"]):
                            self.assertEqual(
                                action["evidence"],
                                "feature_described_binding_unverified")
                self.assertEqual(
                    surface["ACBFR"]["version_specific_priority"], "UNKNOWN")
        self.assertGreaterEqual(count, 130)

    def test_hazards_reference_real_surfaces_and_do_not_claim_priority(self):
        l = self.ledger
        known_surfaces = {surface["id"] for surface in l["surfaces"]}
        known_sources = set(l["source_registry"])
        hazards = l["hazards"]
        ids = [hazard["id"] for hazard in hazards]
        self.assertEqual(len(ids), len(set(ids)))
        self.assertTrue(CRITICAL_HAZARDS <= set(ids))
        for hazard in hazards:
            with self.subTest(hazard=hazard["id"]):
                self.assertIn(hazard["priority"], {"P0", "P1"})
                self.assertIn(hazard["game"], {"AC4", "ACBFR", "BOTH"})
                self.assertTrue(set(hazard["surface_ids"]) <= known_surfaces)
                self.assertTrue(set(hazard["source_ids"]) <= known_sources)
                self.assertTrue(hazard["risk"])
                self.assertIs(hazard["engine_arbitration_verified"], False)

    def test_transitions_distinguish_inferred_vs_described(self):
        l = self.ledger
        valid_surfaces = {surface["id"] for surface in l["surfaces"]}
        transitions = l["transitions"]
        self.assertGreaterEqual(len(transitions), 35)
        ids = [transition["id"] for transition in transitions]
        self.assertEqual(len(ids), len(set(ids)))
        self.assertEqual({t["game"] for t in transitions}, GAME_IDS)
        for t in transitions:
            with self.subTest(transition=t["id"]):
                self.assertIn(t["from"], valid_surfaces)
                self.assertIn(t["to"], valid_surfaces)
                self.assertIn(t["evidence"], {
                    "guide_described", "binding_not_verified",
                    "inferred_not_verified"})
                self.assertTrue(t["sources"])
                self.assertTrue(set(t["sources"]) <= set(l["source_registry"]))
                self.assertFalse(t["engine_verified"])
                self.assertEqual(t["interrupt_priority"], "UNKNOWN")
                self.assertEqual(t["release_cancellation"], "UNKNOWN")

    def test_critical_unknowns_block_feature_assumptions(self):
        unknowns = self.ledger["critical_unknowns"]
        ids = {entry["id"] for entry in unknowns}
        self.assertTrue(CRITICAL_UNKNOWN_IDS <= ids)
        for entry in unknowns:
            self.assertEqual(entry["priority"], "P0")
            self.assertTrue(entry["blocked_features"])
            self.assertTrue(entry["needs"])

    def test_counter_followup_buttons_are_documented_but_not_engine_verified(self):
        surfaces = {s["id"]: s for s in self.ledger["surfaces"]}
        actions = {
            a["action"]: a
            for a in surfaces["combat_counter_followup"]["AC4"]["actions"]
        }
        for name, button in {
            "counter_kill": "X", "counter_throw": "B",
            "counter_disarm": "A", "tool_counter": "Y",
        }.items():
            with self.subTest(name=name):
                self.assertEqual(actions[name]["input"], button)
                self.assertEqual(actions[name]["evidence"],
                                 "source_described_not_runtime_verified")
                self.assertIn("ac4_prima_advanced_tips", actions[name]["sources"])

    def test_original_counter_and_profiles_not_equated_to_new_system(self):
        l = self.ledger
        hazards = {item["id"]: item for item in l["hazards"]}
        self.assertIn("combat_counter_followup",
                      hazards["AC4_B_COUNTER_FOLLOWUP"]["surface_ids"])
        self.assertIn("high_profile_travel",
                      hazards["AC4_RT_PROFILE"]["surface_ids"])
        self.assertIn("manual_jump",
                      hazards["BFR_PARKOUR_SETTINGS"]["surface_ids"])
        self.assertTrue(any("user" in note.lower() for note in l["research_notes"]))
        self.assertNotEqual(
            l["surfaces"][0]["AC4"]["engine_priority"],
            "VERIFIED")


if __name__ == "__main__":
    unittest.main()
