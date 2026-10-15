# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Pico Motion Tracker Control & Diagnostic Utility.

Allows inspecting tracker telemetry, battery, connection state, and configuring
hardware parameters (such as switching IR LEDs to Static ON for Steam Frame optical tracking).

Interacts with the connected Pico 4 Enterprise HMD via ADB.
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from dataclasses import dataclass


@dataclass
class TrackerDeviceStatus:
    id: int
    online: bool
    sn: str
    software_ver: str
    addr: str
    battery: int
    charge_status: int
    bond_status: int
    bracket_status: int


def run_adb_command(cmd_args: list[str]) -> str:
    """Execute ADB shell command and return stdout."""
    full_cmd = ["adb"] + cmd_args
    try:
        proc = subprocess.run(
            full_cmd,
            capture_output=True,
            text=True,
            timeout=10,
            check=False,
        )
        return proc.stdout.strip()
    except subprocess.SubprocessError as e:
        return f"Error executing adb: {e}"


def get_all_trackers_status() -> list[TrackerDeviceStatus]:
    """Parse `dumpsys pvrtracking` for bonded trackers."""
    out = run_adb_command(["shell", "dumpsys", "pvrtracking"])
    trackers = []

    pattern = re.compile(
        r"tracker(?P<id>\d+),\s+online\s+(?P<online>\d+)\s+sn\s+(?P<sn>\S+)\s+"
        r"(?:bond_status\s+(?P<bond>\d+)\s+)?softwareVer\s+(?P<ver>\S+)\s+"
        r"addr\s*(?P<addr>\S*)\s+battery\s+(?P<battery>\d+)\s+chargeStatus\s+(?P<charge>\d+)\s+"
        r"bindStatus\s+(?P<bind>\d+)\s+position\s+\d+\s+bracketstatus\s+(?P<bracket>\d+)"
    )

    for match in pattern.finditer(out):
        t = TrackerDeviceStatus(
            id=int(match.group("id")),
            online=bool(int(match.group("online"))),
            sn=match.group("sn"),
            software_ver=match.group("ver"),
            addr=match.group("addr"),
            battery=int(match.group("battery")),
            charge_status=int(match.group("charge")),
            bond_status=int(match.group("bond") or match.group("bind")),
            bracket_status=int(match.group("bracket")),
        )
        trackers.append(t)

    return trackers


def set_tracker_led_mode(mode: str) -> str:
    """Set IR LED mode on tracker.

    Modes:
      'static_on': 0x01 (Continuous illumination for Steam Frame tracking)
      'strobe': 0x00 (Pico camera exposure synchronized strobe)
    """
    mode_val = "1" if mode.lower() in ("static_on", "static", "on", "1") else "0"
    # Execute through tracker_test utility on headset
    cmd = f"tracker_test set_irled_onoff_mode {mode_val}"
    out = run_adb_command(["shell", cmd])
    return out


def main() -> None:
    parser = argparse.ArgumentParser(description="Pico Motion Tracker ADB Controller")
    subparsers = parser.add_subparsers(dest="command", required=True)

    # Status command
    subparsers.add_parser("status", help="Query connected trackers and battery")

    # LED command
    led_parser = subparsers.add_parser("led", help="Set tracker IR LED mode")
    led_parser.add_argument(
        "--mode",
        choices=["static_on", "strobe"],
        default="static_on",
        help="LED illumination mode ('static_on' for Steam Frame, 'strobe' for Pico)",
    )

    # Cali command
    subparsers.add_parser("cali", help="Dump NVRAM factory LED calibration")

    args = parser.parse_args()

    if args.command == "status":
        trackers = get_all_trackers_status()
        print(f"Discovered {len(trackers)} tracker(s) in system:")
        for t in trackers:
            state_str = "ONLINE (Active)" if t.online else "OFFLINE (Sleeping/Low Battery)"
            bracket_str = "Insert (Ankle/Foot)" if t.bracket_status == 1 else "Normal"
            charge_str = "Charging" if t.charge_status else "Discharging"
            print(f"  [Tracker {t.id}] SN: {t.sn}")
            print(f"    State:          {state_str}")
            print(f"    Battery:        {t.battery}% ({charge_str})")
            print(f"    Firmware:       {t.software_ver}")
            print(f"    Radio Address:  {t.addr}")
            print(f"    Bracket Mode:   {bracket_str}")
            print()

    elif args.command == "led":
        print(f"Setting tracker IR LED mode to: {args.mode.upper()}...")
        res = set_tracker_led_mode(args.mode)
        print(f"Response: {res or 'Success'}")

    elif args.command == "cali":
        print("Dumping factory LED calibration from headset NVRAM...")
        out = run_adb_command(["shell", "tracker_test", "cali-tracker"])
        print(out)


if __name__ == "__main__":
    main()
