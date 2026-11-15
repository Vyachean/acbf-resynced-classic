import json
import tempfile
import unittest
from pathlib import Path

from analyze import analyze, load_journals


class AnalyzerTests(unittest.TestCase):
    def test_only_explicit_correlation_maps_physical_to_logical(self):
        rows = [
            {"record_type": "session_start", "session_id": "s1", "timestamp_utc": "t0"},
            {"record_type": "event", "session_id": "s1", "seq": 1, "qpc": 10,
             "source": "physical_input", "event": "button_edge", "control": "x", "state": 1,
             "correlation_id": "c7"},
            {"record_type": "event", "session_id": "s1", "seq": 2, "qpc": 12,
             "source": "logical_command", "event": "selected", "command_id": 31,
             "correlation_id": "c7"},
            {"record_type": "event", "session_id": "s1", "seq": 3, "qpc": 13,
             "source": "logical_command", "event": "selected", "command_id": 32},
        ]
        for i, row in enumerate(rows, 1):
            row.update(_file="sample.jsonl", _line=i)
        report = analyze(rows, [])
        self.assertEqual(len(report["physical_to_logical"]), 1)
        self.assertEqual(report["physical_to_logical"][0]["command_id"], "31")
        self.assertEqual(report["physical_to_logical"][0]["evidence"],
                         ["sample.jsonl:2", "sample.jsonl:3"])

    def test_temporal_association_stays_an_inference_and_holds_are_measured(self):
        rows = [
            {"record_type": "session_start", "session_id": "s2", "qpc_frequency": 1000},
            {"record_type": "event", "session_id": "s2", "seq": 1, "qpc": 100,
             "source": "xinput_poll", "event": "button_edge", "control": "x",
             "object_id": "0x00000000", "state": 1, "physical_event_id": 9},
            {"record_type": "event", "session_id": "s2", "seq": 2, "qpc": 110,
             "source": "logical_edge_query", "event": "rising_edge_result",
             "command_id": 3, "state": 1, "physical_ref": 9, "correlation_id": 4},
            {"record_type": "event", "session_id": "s2", "seq": 3, "qpc": 120,
             "source": "native_dispatch", "event": "candidate_dispatch",
             "command_id": 31, "correlation_id": 4},
            {"record_type": "event", "session_id": "s2", "seq": 4, "qpc": 600,
             "source": "xinput_poll", "event": "button_edge", "control": "x",
             "object_id": "0x00000000", "state": 0, "physical_event_id": 10},
            {"record_type": "session_end", "session_id": "s2", "complete": True,
             "dropped_events": 0},
        ]
        for i, row in enumerate(rows, 1):
            row.update(_file="session.jsonl", _line=i)
        report = analyze(rows, [])
        self.assertEqual(report["physical_to_logical"], [])
        association = report["temporal_input_associations"][0]
        self.assertEqual((association["physical_control"], association["logical_slot"]), ("x", "3"))
        self.assertEqual(association["status"], "inferred_temporal")
        self.assertEqual(report["physical_holds"][0]["duration_ms"], 500)
        self.assertEqual(report["candidate_dispatch_events"][0]["candidate_id"], 31)
        self.assertTrue(any("action start/end" in x for x in report["unknowns"]))

    def test_truncated_final_record_is_recovered(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "session.jsonl"
            path.write_text(json.dumps({"record_type": "session_start", "session_id": "s"})
                            + "\n{incomplete", encoding="utf-8")
            records, issues = load_journals([path])
            self.assertEqual(len(records), 1)
            self.assertEqual(issues[0]["kind"], "truncated_record")

    def test_nonfinal_corruption_fails_loudly(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "session.jsonl"
            path.write_text("{bad}\n{}\n", encoding="utf-8")
            with self.assertRaises(ValueError):
                load_journals([path])


if __name__ == "__main__":
    unittest.main()
