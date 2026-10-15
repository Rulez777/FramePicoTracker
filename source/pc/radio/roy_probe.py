# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Read-only inventory of stock Steam Frame radio interfaces; Python stdlib only.

Run on Frame, or via SSH: ssh USER@FRAME python3 - --stdout < roy_probe.py
Never opens device nodes, issues ioctls, flashes, rebinds drivers or stops services.
The presence of spidev/hidraw is NOT evidence of a raw-radio API.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import platform
import re
from pathlib import Path


def read_file(path: Path, limit: int = 131072) -> dict:
    try:
        data = path.read_bytes()
        return {"path": str(path), "text": data[:limit].decode("utf-8", "replace"),
                "truncated": len(data) > limit}
    except OSError as exc:
        return {"path": str(path), "error": str(exc)}


def link_target(path: Path) -> str | None:
    return str(path.resolve()) if path.is_symlink() else None


def parse_ihex(text: str) -> dict[int, int]:
    """Validate Intel HEX and preserve addresses without allocating sparse gaps."""
    memory: dict[int, int] = {}
    base = 0
    eof = False
    for line_no, line in enumerate(text.splitlines(), 1):
        line = line.strip()
        if not line:
            continue
        if eof or not line.startswith(":"):
            raise ValueError(f"Invalid record at line {line_no}")
        try:
            record = bytes.fromhex(line[1:])
        except ValueError as exc:
            raise ValueError(f"Invalid hex at line {line_no}") from exc
        if len(record) < 5 or len(record) != record[0] + 5 or sum(record) & 255:
            raise ValueError(f"Bad length/checksum at line {line_no}")
        length, kind = record[0], record[3]
        offset = int.from_bytes(record[1:3], "big")
        payload = record[4:-1]
        if kind == 0:
            for index, value in enumerate(payload):
                address = base + offset + index
                if address in memory and memory[address] != value:
                    raise ValueError(f"Conflicting data at 0x{address:x}")
                memory[address] = value
        elif kind == 1:
            if length or offset:
                raise ValueError("Malformed EOF")
            eof = True
        elif kind in (2, 4):
            if length != 2 or offset:
                raise ValueError("Malformed address record")
            base = int.from_bytes(payload, "big") << (4 if kind == 2 else 16)
        elif kind in (3, 5):
            if length != 4 or offset:
                raise ValueError("Malformed start address record")
        else:
            raise ValueError(f"Unknown record type {kind}")
    if not eof:
        raise ValueError("Missing EOF")
    return memory


def contiguous_regions(memory: dict[int, int]) -> list[tuple[int, bytes]]:
    regions: list[tuple[int, bytes]] = []
    start = previous = -1
    data = bytearray()
    for address in sorted(memory):
        if address != previous + 1 and data:
            regions.append((start, bytes(data)))
            data.clear()
        if not data:
            start = address
        data.append(memory[address])
        previous = address
    if data:
        regions.append((start, bytes(data)))
    return regions


def inspect_firmware(path: Path) -> dict:
    result = {"path": str(path)}
    try:
        data = path.read_bytes()
        result.update(size=len(data), sha256=hashlib.sha256(data).hexdigest())
        regions = (contiguous_regions(parse_ihex(data.decode("ascii")))
                   if path.suffix.lower() == ".hex" else [(0, data)])
        # Strings are investigation leads, never proof of an accessible API.
        pattern = re.compile(rb"radio|esb|gazell|raw|channel|crc|timesync|spi|hid|debug", re.I)
        strings = []
        for start, content in regions:
            for match in re.finditer(rb"[ -~]{6,}", content):
                if pattern.search(match.group()):
                    strings.append({"address": hex(start + match.start()),
                                    "text": match.group()[:300].decode("ascii")})
        result.update(regions=[{"address": hex(start), "size": len(content)}
                               for start, content in regions], string_leads=strings[:500],
                      string_leads_truncated=len(strings) > 500)
    except (OSError, UnicodeError, ValueError) as exc:
        result["error"] = str(exc)
    return result


def collect(root: Path = Path("/"), steamvr: str = "/opt/steamvr") -> dict:
    """Root override supports offline sysfs snapshots and deterministic tests."""
    def at(path: str) -> Path:
        return root / path.lstrip("/")

    report: dict = {
        "schema": "pico-roy-probe-v1", "machine": platform.machine(),
        "kernel": platform.release(), "device_nodes_opened": False,
        "radio_commands_sent": False, "onboard_pico_rx_verified": False,
        "assessment": "unverified: requires a documented or traced stock-firmware host command and live Pico packets",
        "os": read_file(at("/etc/os-release")),
        "spi": [], "hidraw": [], "device_nodes": [], "firmware": [], "scripts": [],
        "driver_manifests": [],
    }
    for path in sorted(at("/sys/bus/spi/devices").glob("*")):
        report["spi"].append({"name": path.name, "driver": link_target(path / "driver"),
                              "of_node": link_target(path / "of_node"),
                              "modalias": read_file(path / "modalias"),
                              "uevent": read_file(path / "uevent")})
    for path in sorted(at("/sys/class/hidraw").glob("hidraw*")):
        report["hidraw"].append({"name": path.name,
                                 "device": link_target(path / "device"),
                                 "uevent": read_file(path / "device/uevent")})
    for pattern in ("spidev*", "hidraw*", "ttyACM*", "ttyUSB*"):
        report["device_nodes"].extend(str(path) for path in sorted(at("/dev").glob(pattern)))
    drivers = at(steamvr) / "drivers"
    roy = drivers / "frame_hmd/resources/firmware/roy"
    report["firmware_directory_exists"] = roy.is_dir()
    for path in sorted(roy.glob("*.hex")):
        report["firmware"].append(inspect_firmware(path))
    for pattern in ("*.sh", "*.cfg"):
        report["scripts"].extend(read_file(path) for path in sorted(roy.glob(pattern)))
    for driver in ("frame_hmd", "frame_controller", "deckard", "cv"):
        report["driver_manifests"].append(read_file(drivers / driver / "driver.vrdrivermanifest"))
    report["driver_binaries"] = [str(path) for driver in ("frame_hmd", "deckard", "cv")
                                  for path in sorted((drivers / driver / "bin").glob("**/*.so"))]
    report["gpio_names"] = [read_file(path) for path in sorted(
        at("/sys/firmware/devicetree/base").glob("**/gpio-line-names"))]
    return report


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--stdout", action="store_true", help="Print JSON; create no files")
    parser.add_argument("--out", type=Path, help="Write report JSON to this new file")
    parser.add_argument("--root", type=Path, default=Path("/"), help="Offline snapshot root")
    parser.add_argument("--steamvr", default="/opt/steamvr")
    parser.add_argument("--firmware", type=Path, help="Analyze a local HEX/BIN instead of inventory")
    args = parser.parse_args()
    if not args.stdout and args.out is None:
        parser.error("Specify --stdout or --out")
    report = inspect_firmware(args.firmware) if args.firmware else collect(args.root, args.steamvr)
    text = json.dumps(report, indent=2, ensure_ascii=False)
    if args.out:
        with args.out.open("x", encoding="utf-8") as handle:
            handle.write(text + "\n")
    if args.stdout:
        print(text)


if __name__ == "__main__":
    main()
