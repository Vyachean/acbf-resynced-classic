#!/usr/bin/env python3
"""Read-only live summary of the AC4 observer's growing local JSONL journal."""
from __future__ import annotations

import argparse
import json
import time
from collections import Counter
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


def read_appended(path: Path, offset: int, pending: bytes
                  ) -> tuple[list[dict[str, Any]], int, bytes]:
    """Read newly appended complete JSONL records, retaining a partial final line."""
    size = path.stat().st_size
    if size < offset:
        offset = 0
        pending = b""
    with path.open("rb") as stream:
        stream.seek(offset)
        chunk = stream.read()
        offset = stream.tell()

    pieces = (pending + chunk).split(b"\n")
    pending = pieces.pop()
    rows: list[dict[str, Any]] = []
    for piece in pieces:
        if not piece.strip():
            continue
        row = json.loads(piece.decode("utf-8"))
        if not isinstance(row, dict) or "record_type" not in row:
            raise ValueError(f"Malformed journal record in {path.name}")
        rows.append(row)
    return rows, offset, pending


def watch(directory: Path, interval: float, report_interval: float) -> None:
    active_path: Path | None = None
    offset = 0
    pending = b""
    session_id = "unknown"
    sources: Counter[str] = Counter()
    event_count = 0
    input_edges = 0
    dispatches = 0
    hook_status: int | None = None
    last_report = 0.0
    waiting_reported = False

    while True:
        try:
            candidates = list(directory.glob("session-*.jsonl"))
            newest = max(candidates, key=lambda item: item.stat().st_mtime_ns,
                         default=None)
            if newest is None:
                if not waiting_reported:
                    print(f"Waiting for observer journal in {directory}", flush=True)
                    waiting_reported = True
            elif newest != active_path:
                active_path = newest
                offset = 0
                pending = b""
                session_id = "unknown"
                sources.clear()
                event_count = input_edges = dispatches = 0
                hook_status = None
                last_report = 0.0
                waiting_reported = False

            if active_path is not None and active_path.exists():
                rows, offset, pending = read_appended(active_path, offset, pending)
                for row in rows:
                    kind = row["record_type"]
                    if kind == "session_start":
                        session_id = str(row.get("session_id", session_id))
                    elif kind == "event":
                        event_count += 1
                        source = str(row.get("source", "unknown"))
                        sources[source] += 1
                        if source == "xinput_poll" and row.get("event") == "button_edge":
                            input_edges += 1
                        if source == "native_dispatch":
                            dispatches += 1
                        if row.get("event") == "native_hook_status":
                            hook_status = int(row.get("state", 0))

                now = time.monotonic()
                if event_count and now - last_report >= report_interval:
                    stamp = datetime.now(timezone.utc).isoformat(timespec="seconds")
                    source_text = ",".join(
                        f"{name}:{count}" for name, count in sorted(sources.items()))
                    print(f"{stamp} session={session_id} hooks={hook_status} "
                          f"events={event_count} xinput_edges={input_edges} "
                          f"dispatches={dispatches} sources=[{source_text}]",
                          flush=True)
                    last_report = now
        except (OSError, json.JSONDecodeError, UnicodeDecodeError, ValueError) as error:
            # A briefly locked file or incomplete final write is retried next poll.
            print(f"Journal read pending: {error}", flush=True)
            if active_path is not None and active_path.exists():
                try:
                    size = active_path.stat().st_size
                    if size < offset:
                        offset = 0
                        pending = b""
                except OSError:
                    pass
        time.sleep(interval)


def main() -> int:
    default_dir = Path.home() / "AppData" / "Local" / "AC4ControlObserver"
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--directory", type=Path, default=default_dir)
    parser.add_argument("--interval", type=float, default=1.0)
    parser.add_argument("--report-interval", type=float, default=10.0)
    args = parser.parse_args()
    if args.interval <= 0 or args.report_interval <= 0:
        parser.error("intervals must be positive")
    try:
        watch(args.directory, args.interval, args.report_interval)
    except KeyboardInterrupt:
        print("Stopped read-only journal watch.", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

