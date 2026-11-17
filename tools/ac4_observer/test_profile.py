import json
import tempfile
import unittest
from pathlib import Path

from make_profile import REQUIRED, render


def fake_pe(path: Path) -> None:
    data = bytearray(512)
    data[:2] = b"MZ"
    data[0x3C:0x40] = (0x80).to_bytes(4, "little")
    data[0x80:0x84] = b"PE\0\0"
    data[0x84:0x86] = (0x14C).to_bytes(2, "little")
    data[0x94:0x96] = (0xE0).to_bytes(2, "little")
    optional = 0x98
    data[optional:optional + 2] = (0x10B).to_bytes(2, "little")
    data[optional + 28:optional + 32] = (0x400000).to_bytes(4, "little")
    path.write_bytes(data)


class ProfileTests(unittest.TestCase):
    def test_private_header_requires_x86_profile_and_hashes_exact_exe(self):
        with tempfile.TemporaryDirectory() as directory:
            exe = Path(directory) / "AC4BFSP.exe"
            profile = Path(directory) / "target_profile.local.json"
            fake_pe(exe)
            profile.write_text(json.dumps({"sites": [
                {"name": name, "rva": hex(0x1000 + i * 0x100), "bytes": "55 8B EC 90 90 90 90 90 90 90 90 90 90 90 90 90"}
                for i, name in enumerate(REQUIRED)]}), encoding="utf-8")
            header = render(exe, profile)
            self.assertEqual(header.count("constexpr uint8_t kSite"), 7)
            self.assertIn("kExpectedExeSha256", header)
            self.assertNotIn(str(exe), header)

    def test_missing_site_fails(self):
        with tempfile.TemporaryDirectory() as directory:
            exe = Path(directory) / "AC4BFSP.exe"
            profile = Path(directory) / "profile.json"
            fake_pe(exe)
            profile.write_text('{"sites": []}', encoding="utf-8")
            with self.assertRaises(ValueError):
                render(exe, profile)


if __name__ == "__main__":
    unittest.main()
