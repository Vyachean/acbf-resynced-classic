#!/usr/bin/env python3
"""Offline, evidence-preserving analyzer for AC4 observer JSONL journals."""
from __future__ import annotations

import argparse
import json
from collections import defaultdict
from pathlib import Path
from typing import Any


def load_journals(paths: list[Path]) -> tuple[list[dict[str, Any]], list[dict[str, Any]]]:
    records: list[dict[str, Any]] = []
    issues: list[dict[str, Any]] = []
    for path in paths:
        with path.open("r", encoding="utf-8") as stream:
            for number, line in enumerate(stream, 1):
                if not line.strip():
                    continue
                try:
                    row = json.loads(line)
                except json.JSONDecodeError:
                    # A crash can leave only the final JSONL line incomplete.
                    if stream.readline() == "":
                        issues.append({"file": path.name, "line": number,
                                       "kind": "truncated_record"})
                        break
                    raise ValueError(f"Malformed non-final JSON at {path}:{number}")
                if not isinstance(row, dict) or "record_type" not in row:
                    raise ValueError(f"Missing record_type at {path}:{number}")
                row["_file"] = path.name
                row["_line"] = number
                records.append(row)
    return records, issues


def analyze(records: list[dict[str, Any]], issues: list[dict[str, Any]]) -> dict[str, Any]:
    sessions: dict[str, dict[str, Any]] = {}
    events: dict[str, list[dict[str, Any]]] = defaultdict(list)
    for row in records:
        sid = str(row.get("session_id", "unknown"))
        if row["record_type"] == "session_start":
            sessions.setdefault(sid, {"session_id": sid, "start": row,
                                      "end": None, "events": []})
        elif row["record_type"] == "session_end":
            sessions.setdefault(sid, {"session_id": sid, "start": None,
                                      "end": row, "events": []})["end"] = row
        elif row["record_type"] == "event":
            events[sid].append(row)
    for sid, rows in events.items():
        sessions.setdefault(sid, {"session_id": sid, "start": None,
                                   "end": None, "events": []})["events"] = rows

    observed_bindings: dict[tuple[str, str], set[str]] = defaultdict(set)
    inferred_bindings: dict[tuple[str, str], set[str]] = defaultdict(set)
    by_correlation: dict[str, list[dict[str, Any]]] = defaultdict(list)
    for sid, session in sessions.items():
        session["events"].sort(key=lambda e: (e.get("qpc", 0), e.get("seq", 0)))
        for event in session["events"]:
            corr = event.get("correlation_id")
            if corr is not None:
                by_correlation[f"{sid}:{corr}"].append(event)
    # Correlation is an explicit source-provided relationship, never a time-window guess.
    for key, rows in by_correlation.items():
        physical = [e for e in rows if e.get("source") in ("physical_input", "xinput_poll")
                    and e.get("control") and e.get("event") == "button_edge"]
        logical = [e for e in rows if e.get("source") in ("logical_command", "logical_edge_query")
                   and e.get("command_id") is not None]
        for source in physical:
            for command in logical:
                pair = (str(source["control"]), str(command["command_id"]))
                observed_bindings[pair].update((f"{source['_file']}:{source['_line']}",
                                                f"{command['_file']}:{command['_line']}"))
    physical_by_id = {}
    for session in sessions.values():
        for event in session["events"]:
            if event.get("source") == "xinput_poll" and event.get("event") == "button_edge":
                physical_by_id[(session["session_id"], str(event.get("physical_event_id")))] = event
    for session in sessions.values():
        sid = session["session_id"]
        for event in session["events"]:
            if event.get("source") != "logical_edge_query" or not event.get("physical_ref"):
                continue
            source = physical_by_id.get((sid, str(event["physical_ref"])))
            if source and source.get("state") == 1 and event.get("state") == 1:
                pair = (str(source.get("control")), str(event.get("command_id")))
                inferred_bindings[pair].update((f"{source['_file']}:{source['_line']}",
                                                f"{event['_file']}:{event['_line']}"))

    coverage = defaultdict(set)
    holds: list[dict[str, Any]] = []
    combinations: list[dict[str, Any]] = []
    for session in sessions.values():
        start = session["start"] or {}
        frequency = int(start.get("qpc_frequency") or 0)
        pressed: dict[tuple[str, str], dict[str, Any]] = {}
        down_events: list[dict[str, Any]] = []
        for event in session["events"]:
            context = event.get("context")
            if context:
                coverage[str(context)].add(str(event.get("event", "unknown")))
            if event.get("source") != "xinput_poll" or event.get("event") != "button_edge":
                continue
            key = (str(event.get("object_id")), str(event.get("control")))
            if event.get("state") == 1:
                pressed[key] = event
                down_events.append(event)
            elif event.get("state") == 0 and key in pressed:
                begin = pressed.pop(key)
                if frequency and event.get("qpc", 0) >= begin.get("qpc", 0):
                    holds.append({"controller_slot": key[0], "control": key[1],
                                  "duration_ms": round((event["qpc"] - begin["qpc"])
                                                       * 1000 / frequency, 3),
                                  "status": "observed_edge_duration",
                                  "evidence": [f"{begin['_file']}:{begin['_line']}",
                                               f"{event['_file']}:{event['_line']}"]})
        if frequency:
            for index, first in enumerate(down_events):
                group = [first]
                group.extend(other for other in down_events[index + 1:]
                             if other.get("object_id") == first.get("object_id")
                             and 0 <= other.get("qpc", 0) - first.get("qpc", 0)
                             <= frequency // 100)  # within one 10 ms poll interval
                if len(group) > 1:
                    combinations.append({"controller_slot": first.get("object_id"),
                        "controls": sorted({str(x.get("control")) for x in group}),
                        "status": "near_simultaneous_sample_inference",
                        "window_ms": 10,
                        "evidence": [f"{x['_file']}:{x['_line']}" for x in group]})

    return {
        "schema_version": 1,
        "sessions": [
            {"session_id": sid,
             "started_utc": (s["start"] or {}).get("timestamp_utc"),
             "ended_utc": (s["end"] or {}).get("timestamp_utc"),
             "complete": bool((s["end"] or {}).get("complete", False)),
             "dropped_events": (s["end"] or {}).get("dropped_events", "unknown"),
             "event_count": len(s["events"]),
             "sources": sorted({str(e.get("source", "unknown")) for e in s["events"]})}
            for sid, s in sorted(sessions.items())],
        "physical_to_logical": [
            {"physical_control": p, "command_id": c, "status": "observed_correlated",
             "evidence": sorted(refs)}
            for (p, c), refs in sorted(observed_bindings.items())],
        "temporal_input_associations": [
            {"physical_control": p, "logical_slot": c, "status": "inferred_temporal",
             "association_window_ms": 250, "observations": max(1, len(refs) // 2),
             "evidence": sorted(refs)}
            for (p, c), refs in sorted(inferred_bindings.items())],
        "physical_holds": holds,
        "near_simultaneous_combinations": combinations,
        "candidate_dispatch_events": [
            {"candidate_id": e.get("command_id"), "object_id": e.get("object_id"),
             "recipient_id": e.get("recipient_id"), "flags": e.get("state"),
             "correlation_id": e.get("correlation_id"), "provenance": "runtime_observed",
             "meaning": "unlabeled_internal_dispatch_result",
             "evidence": f"{e['_file']}:{e['_line']}"}
            for s in sessions.values() for e in s["events"]
            if e.get("source") == "native_dispatch"],
        "correlations": [
            {"session_correlation_id": key, "sources": sorted(
                {str(e.get("source", "unknown")) for e in rows}),
             "evidence": [f"{e['_file']}:{e['_line']}" for e in rows]}
            for key, rows in sorted(by_correlation.items())],
        "observed_contexts": [
            {"context": context, "event_types": sorted(types), "status": "observed",
             "evidence": [f"{e['_file']}:{e['_line']}"
                          for s in sessions.values() for e in s["events"]
                          if str(e.get("context")) == context]}
            for context, types in sorted(coverage.items())],
        "unknowns": ["unobserved contexts and transitions",
                     "candidate rejection reasons unless emitted by a verified source",
                     "runtime actor/component class names; pointers are session-local only",
                     "which vtable method body executed; the dispatcher result is not an action-start event",
                     "action start/end/interrupt unless emitted by a verified source",
                     "DirectInput and non-XInput physical devices"],
        "journal_issues": issues,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("journals", nargs="+", type=Path)
    parser.add_argument("-o", "--output", type=Path, default=Path("ac4-control-map.json"))
    args = parser.parse_args()
    records, issues = load_journals(args.journals)
    report = analyze(records, issues)
    args.output.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n",
                           encoding="utf-8")
    print(f"Wrote {args.output}: {len(report['sessions'])} session(s), "
          f"{sum(s['event_count'] for s in report['sessions'])} event(s), "
          f"{len(report['physical_to_logical'])} confirmed physical/logical mapping(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
