"""Audit the effect of consuming a controller button across both games.

This is a conservative, READ-ONLY research tool. It does not hook any game or
compute action eligibility: it lists every documented action using a physical
input, plus unbound/contextual actions that might also use that input.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
LEDGER = ROOT / "research" / "control_system_ledger.json"

# Explicitly bounded lexical normalization: this is not a game input API.
# D-pad directions are grouped because the first task is auditing whole buttons.
_BUTTON = re.compile(
    r"(?<![A-Za-z0-9_])(?:DPad(?:Up|Down|Left|Right)?|"
    r"LB|RB|LT|RT|L3|R3|Menu|View|X|Y|A|B)(?![A-Za-z0-9_])",
    re.IGNORECASE,
)
BUTTONS = ("X", "Y", "A", "B", "LB", "RB", "LT", "RT",
           "L3", "R3", "DPAD", "MENU", "VIEW")
_FACE_BUTTONS = ("X", "Y", "A", "B")

def physical_buttons(raw_input: str) -> tuple[str, ...]:
    """Find literal physical buttons; never infer a game action or context."""
    # e.g. a historical "RT_released" is a physical RT gesture, not a new key.
    normalized = raw_input.replace("_released", "")
    matched = {
        ("DPAD" if m.group(0).lower().startswith("dpad")
         else m.group(0).upper())
        for m in _BUTTON.finditer(normalized)
    }
    if "one_of_four_face_buttons" in normalized.lower():
        matched.update(_FACE_BUTTONS)
    return tuple(sorted(matched, key=BUTTONS.index))

def ambiguous_binding(raw_input: str, evidence: str) -> bool:
    """Unknown button identity must not be silently excluded from a query."""
    lowered = raw_input.lower()
    return (evidence == "feature_described_binding_unverified" or
            any(part in lowered for part in
                ("unknown", "unverified", "context", "?", "one_of_four")))

def analyze(ledger: dict, game: str, button: str) -> dict:
    """Return all *possible* input collisions for one game and physical key."""
    if game not in ("AC4", "ACBFR"):
        raise ValueError("game must be AC4 or ACBFR")
    if button not in BUTTONS:
        raise ValueError("unsupported physical button")

    definite, uncertain = [], []
    for surface in ledger["surfaces"]:
        for action in surface[game]["actions"]:
            raw = action["input"]
            buttons = physical_buttons(raw)
            unknown = ambiguous_binding(raw, action["evidence"])
            row = {
                "surface": surface["id"],
                "action": action["action"],
                "input": raw,
                "gesture": action["gesture"],
                "when": action["when"],
                "source_ids": action["sources"],
                "physical_buttons": list(buttons),
                "binding_uncertain": unknown,
                "native_action_priority": surface[game]["engine_priority"],
                "runtime_verified": False,
            }
            if button in buttons:
                definite.append(row)
            elif unknown and not buttons:
                # The source does not specify a binding. An overriding plugin
                # cannot safely claim it is unaffected by the chosen key.
                uncertain.append(row)
    key = lambda row: (row["surface"], row["action"], row["input"])
    return {
        "game": game,
        "physical_button": button,
        "audit_status": ledger["audit_status"],
        "override_permission": "IMPACT_REPORT_ONLY",
        "scope": "ALL_CATALOGUED_SURFACES_NOT_AN_ENGINE_ACTION_GRAPH",
        "exact_binding_actions": sorted(definite, key=key),
        "unknown_binding_actions": sorted(uncertain, key=key),
        "rule": ("Use this inventory to review collisions inside the feature's "
                 "declared context. Keep unsupported and unknown contexts native "
                 "pass-through; this report does not authorize or deny a feature."),
    }

def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--game", choices=("AC4", "ACBFR"), required=True)
    parser.add_argument("--button", choices=BUTTONS, required=True)
    opts = parser.parse_args()
    ledger = json.loads(LEDGER.read_text(encoding="utf-8"))
    print(json.dumps(analyze(ledger, opts.game, opts.button),
                     ensure_ascii=False, indent=2))

if __name__ == "__main__":
    main()
