"""Offline consistency checks for the dual-game modding source catalog.

No network access, external dependencies or game files are required.
"""

import json
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
SOURCE_MD = ROOT / "docs" / "knowledge" / "SOURCES.md"
SOURCE_JSON = ROOT / "research" / "modding_sources.json"
SOURCE_LINE = re.compile(
    r"^\|\s*\[([^]]+)\]\((https?://[^)]+)\)\s*\|\s*([^|]+)\|\s*([^|]+)\|"
)


def read_source_table():
    section = ""
    rows = []
    for line in SOURCE_MD.read_text(encoding="utf-8").splitlines():
        if line.startswith("## "):
            section = line[3:].strip()
        if section == "Source addition template":
            continue
        found = SOURCE_LINE.match(line)
        if found:
            name, url, scope, desc = found.groups()
            rows.append((section, name, url, scope.strip(), desc.strip()))
    return rows


class KnowledgeSourceCatalogTests(unittest.TestCase):
    def test_catalog_synchronized_with_human_index(self):
        catalog = json.loads(SOURCE_JSON.read_text(encoding="utf-8"))
        rows = read_source_table()
        entries = catalog["entries"]
        self.assertEqual(catalog["schema_version"], 1)
        self.assertEqual(len(rows), catalog["count"])
        self.assertEqual(len(entries), len(rows))
        self.assertGreater(len(rows), 0, "A vacuous empty source index must not pass")

        for idx, (entry, (section, name, url, scope, desc)) in enumerate(zip(entries, rows), 1):
            with self.subTest(source_id=entry.get("id")):
                self.assertEqual(entry["id"], f"S{idx:03d}")
                self.assertEqual(entry["category"], section)
                self.assertEqual(entry["title"], name)
                self.assertEqual(entry["url"], url)
                self.assertEqual(entry["annotation"], desc)
                self.assertTrue(entry["game_scope"])
                self.assertIn(entry["evidence_grade"], {"A", "B", "C", "U"})

    def test_no_duplicate_or_template_sources(self):
        sources = json.loads(SOURCE_JSON.read_text(encoding="utf-8"))["entries"]
        urls = [source["url"] for source in sources]
        self.assertEqual(len(urls), len(set(urls)))
        self.assertTrue(all("original-author-url" not in url for url in urls))
        self.assertTrue(all(source["url"].startswith(("https://", "http://")) for source in sources))


if __name__ == "__main__":
    unittest.main()
