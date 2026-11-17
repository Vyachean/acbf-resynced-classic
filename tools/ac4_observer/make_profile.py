"""Create a private, exact-build hook profile from verified local Ghidra notes."""
import argparse
import hashlib
import json
import struct
from pathlib import Path

REQUIRED = ("callback", "resolver", "dispatcher", "context_gate", "rising_edge",
            "timing_gate", "availability")


def image_base(path: Path) -> int:
    data = path.read_bytes()
    if len(data) < 0x40 or data[:2] != b"MZ":
        raise ValueError("target is not a valid PE image")
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    if pe_offset + 0x38 > len(data) or data[pe_offset:pe_offset + 4] != b"PE\0\0":
        raise ValueError("target PE header is missing or truncated")
    machine = struct.unpack_from("<H", data, pe_offset + 4)[0]
    opt_size = struct.unpack_from("<H", data, pe_offset + 20)[0]
    optional = pe_offset + 24
    if machine != 0x14C or opt_size < 32 or struct.unpack_from("<H", data, optional)[0] != 0x10B:
        raise ValueError("target must be a PE32/i386 executable")
    return struct.unpack_from("<I", data, optional + 28)[0]


def render(exe: Path, profile_path: Path) -> str:
    base = image_base(exe)
    if base != 0x400000:
        raise ValueError("this observer profile schema currently requires image base 0x400000")
    profile = json.loads(profile_path.read_text(encoding="utf-8"))
    sites = profile.get("sites")
    if not isinstance(sites, list) or {x.get("name") for x in sites} != set(REQUIRED) or len(sites) != len(REQUIRED):
        raise ValueError("profile must contain each of the seven verified site names exactly once")
    by_name = {x["name"]: x for x in sites}
    output = ["// Generated locally; keep this file untracked and do not share across builds."]
    names = {"callback": "Callback", "resolver": "Resolver", "dispatcher": "Dispatch",
             "context_gate": "Gate", "rising_edge": "Edge", "timing_gate": "Hold",
             "availability": "Available"}
    for name in REQUIRED:
        site = by_name[name]
        try:
            rva = int(site["rva"], 0) if isinstance(site["rva"], str) else int(site["rva"])
            raw = bytes.fromhex(site["bytes"])
        except (KeyError, TypeError, ValueError) as exc:
            raise ValueError(f"invalid profile site {name}") from exc
        if rva <= 0 or len(raw) < 16 or len(raw) > 64:
            raise ValueError(f"site {name} needs a positive RVA and 16-64 verified bytes")
        values = ",".join(f"0x{x:02x}" for x in raw)
        output.append(f"constexpr uint8_t kSite{names[name]}[] = {{{values}}};")
    output.append("constexpr Site kSites[] = {")
    for name in REQUIRED:
        site = by_name[name]
        rva = int(site["rva"], 0) if isinstance(site["rva"], str) else int(site["rva"])
        var = "kSite" + names[name]
        output.append(f"    {{{rva:#010x}, {var}, sizeof({var})}},")
    output.extend(("};", f'constexpr char kExpectedExeSha256[] = "{hashlib.sha256(exe.read_bytes()).hexdigest()}";'))
    return "\n".join(output) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", required=True, type=Path, help="the exact locally inspected AC4BFSP.exe")
    parser.add_argument("--profile", required=True, type=Path, help="private JSON of independently verified RVAs and bytes")
    parser.add_argument("--output", type=Path, default=Path("target_profile.local.h"))
    args = parser.parse_args()
    args.output.write_text(render(args.exe, args.profile), encoding="ascii", newline="\n")
    print(f"Wrote private profile header: {args.output.name}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
