import tempfile
import unittest
from pathlib import Path

from watch import read_appended


class WatchTests(unittest.TestCase):
    def test_partial_record_is_completed_on_next_poll(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "session.jsonl"
            path.write_bytes(b'{"record_type":"session_start",')
            rows, offset, pending = read_appended(path, 0, b"")
            self.assertEqual(rows, [])
            self.assertNotEqual(pending, b"")

            with path.open("ab") as stream:
                stream.write(b'"session_id":"s1"}\n')
            rows, offset, pending = read_appended(path, offset, pending)
            self.assertEqual(rows[0]["session_id"], "s1")
            self.assertEqual(pending, b"")
            self.assertEqual(offset, path.stat().st_size)

    def test_truncation_resets_reader_offset(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "session.jsonl"
            path.write_text('{"record_type":"session_start","session_id":"old-session-long"}\n',
                            encoding="utf-8")
            _, offset, _ = read_appended(path, 0, b"")
            path.write_text('{"record_type":"session_start","session_id":"new"}\n',
                            encoding="utf-8")
            rows, _, _ = read_appended(path, offset, b"")
            self.assertEqual(rows[0]["session_id"], "new")


if __name__ == "__main__":
    unittest.main()

