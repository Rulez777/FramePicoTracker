#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
# Install release-20261006b on Frame, keeping the previous installation.
# Does NOT enable or start the radio unit and does NOT restart SteamVR.
# Run on Frame after the release tarball was extracted and built:
#   release-20261006b/pc            current Python/C sources
#   release-20261006b/build/abi4    roy_station.elf + deployment.json (ABI4)
#   release-20261006b/build         FramePicoTracker, tracking_watch, driver_pico_tracker.so
# Private calibration files (frame-factory-20261006.json, pico-calibration-root.json)
# must already be in release-20261006b/ (copied from K:\AI\pico\outputs).
set -euo pipefail
W=/home/steamos/pico-ram-work
R=$W/release-20261006b
B=$W/backup-before-20261006b
mkdir -p "$B" "$W/private" "$W/state"

# 1. Back up everything this script replaces.
cp -a ~/.config/systemd/user/frame-pico-radio.service "$B/"
cp -a "$W/frame_debug_agent.py" "$B/"
cp -a ~/.local/bin/FramePicoTracker "$B/"
cp -a "$W/steamvr-driver/bin/linuxarm64/driver_pico_tracker.so" "$B/"
[ -e "$W/station-config.json" ] && cp -a "$W/station-config.json" "$B/"
[ -e "$W/release" ] && mv "$W/release" "$B/release-previous"

# 2. Private calibration stays outside the release tree.
for f in frame-factory-20261006.json pico-calibration-root.json; do
  [ -e "$R/$f" ] && mv "$R/$f" "$W/private/"
done
chmod 600 "$W"/private/*

# 3. One release layout, referenced by the unit through the release symlink.
cp "$R/build/abi4/roy_station.elf" "$R/pc/roy_station.elf"
cp "$R/build/abi4/deployment.json" "$R/pc/deployment.json"
cp "$R/build/tracking_watch" "$R/pc/tracking_watch"
ln -sfn "$R/pc" "$W/release"

# 4. Station config: seed from the private registry history, phase measured in-session.
cat > "$W/station-config.json" <<JSON
{"version": 1, "seed_le": "74735415",
 "optics": {"camera_clock_file": "$R/build/camera-clock-evidence.json",
            "factory_camera_file": "$W/private/frame-factory-20261006.json",
            "factory_led_file": "$W/private/pico-calibration-root.json",
            "dataset_root": "/home/steamos/.local/share/Steam/config/cv/xrservice/datasets",
            "phase_at_exposure_ticks": "search",
            "phase_file": "$W/state/led-phase.json",
            "optical_log": "$W/state/optical-live.jsonl",
            "imu_timebase": "envelope"}}
JSON

# 5. Binaries: atomic replace; SteamVR keeps the old driver until its next restart.
install -m 755 "$R/build/FramePicoTracker" ~/.local/bin/FramePicoTracker.new
mv ~/.local/bin/FramePicoTracker.new ~/.local/bin/FramePicoTracker
install -m 755 "$R/build/driver_pico_tracker.so" "$W/steamvr-driver/bin/linuxarm64/driver_pico_tracker.so.new"
mv "$W/steamvr-driver/bin/linuxarm64/driver_pico_tracker.so.new" "$W/steamvr-driver/bin/linuxarm64/driver_pico_tracker.so"
install -m 644 "$R/pc/radio/frame_debug_agent.py" "$W/frame_debug_agent.py"

# 6. Units: radio unit installed but neither enabled nor started.
install -m 644 "$R/pc/frame_app/frame-pico-radio.service" ~/.config/systemd/user/frame-pico-radio.service
systemctl --user daemon-reload
systemctl --user restart frame-pico-api.service
systemctl --user is-active frame-pico-api.service
systemctl --user is-enabled frame-pico-radio.service || true
echo "Installed. Backup: $B. Rollback: restore files from $B and re-point $W/release."
