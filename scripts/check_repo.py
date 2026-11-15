"""Standard-library-only, offline repository contract check for GitHub Actions."""

from pathlib import Path
import re
import sys
from urllib.parse import unquote

ROOT = Path(__file__).resolve().parents[1]
REQUIRED = [
    "README.md",
    "LICENSE.md",
    "CONTRIBUTING.md",
    "docs/SCOPE.md",
    "docs/ROADMAP.md",
    "docs/ARCHITECTURE.md",
    "docs/TESTING.md",
    "docs/TEST_STRATEGY.md",
    "docs/RESEARCH.md",
    "docs/WORKFLOW.md",
    ".github/pull_request_template.md",
    ".github/workflows/ci.yml",
    ".github/ISSUE_TEMPLATE/bug_report.yml",
    ".github/ISSUE_TEMPLATE/research.yml",
    "scripts/versioning.py",
    "scripts/publish_version.py",
]
MARKDOWN_LINK = re.compile(r"\[[^\]]*\]\(([^)]+)\)")


def check_local_links(path: Path) -> list[str]:
    problems = []
    for line_number, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        for value in MARKDOWN_LINK.findall(line):
            target = value.split("#", 1)[0].split("?", 1)[0]
            if not target or "://" in target or target.startswith(("mailto:", "data:", "#")):
                continue
            if target.startswith("/"):
                candidate = ROOT / target.lstrip("/")
            else:
                candidate = path.parent / unquote(target)
            if not candidate.exists():
                problems.append(f"{path.relative_to(ROOT)}:{line_number}: broken link: {value}")
    return problems


def main() -> int:
    errors = [f"missing required file: {name}" for name in REQUIRED if not (ROOT / name).is_file()]
    for path in ROOT.rglob("*.md"):
        if any(segment in {".git", "build", "out"} for segment in path.parts):
            continue
        errors.extend(check_local_links(path))
    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        return 1
    print(f"Repository contract passed: {len(REQUIRED)} required files; local Markdown links resolve.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
