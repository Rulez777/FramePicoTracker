# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Main bridge server: receives Pico Motion Tracker frames and publishes to SteamVR VMT.

Usage:
  python main.py [--port 39500] [--vmt-port 39570] [--calib calibration.json]
"""

from __future__ import annotations

import argparse
import logging
import sys
import time
from pathlib import Path
from typing import Optional

from calibration import SpaceTransform
from coords import unity_to_openvr_pos, unity_to_openvr_quat
from protocol import FramePayload, HelloPayload
from receiver import StreamReceiver
from vmt_client import VMTClient

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(name)s: %(message)s",
    datefmt="%H:%M:%S",
)
logger = logging.getLogger("pico_bridge")


class BridgeApp:
    def __init__(
        self,
        tcp_port: int = 39500,
        vmt_port: int = 39570,
        calib_file: Optional[Path] = None,
    ) -> None:
        self.vmt = VMTClient(port=vmt_port)
        self.calib = (
            SpaceTransform.load(calib_file)
            if calib_file and calib_file.exists()
            else SpaceTransform.identity()
        )
        if calib_file and calib_file.exists():
            logger.info("Loaded space calibration from %s", calib_file)
        else:
            logger.info("Using identity space transform (no calibration file loaded)")

        self.receiver = StreamReceiver(
            port=tcp_port,
            on_hello=self.handle_hello,
            on_frame=self.handle_frame,
        )

        self._frame_count = 0
        self._last_report_time = time.monotonic()
        self._last_reported_frame = 0

    def handle_hello(self, hello: HelloPayload) -> None:
        logger.info("Pico Headset Connected: Device=%s, SDK=%s, App=%s", hello.device, hello.sdk, hello.app)

    def handle_frame(self, frame: FramePayload) -> None:
        self._frame_count += 1
        now = time.monotonic()

        # Update FPS statistics every 2 seconds
        dt = now - self._last_report_time
        if dt >= 2.0:
            fps = (self._frame_count - self._last_reported_frame) / dt
            valid_count = sum(1 for t in frame.trackers if t.valid)
            logger.info(
                "Streaming @ %.1f Hz | Trackers active: %d/%d | Frame ID: %d",
                fps,
                valid_count,
                len(frame.trackers),
                frame.frame,
            )
            self._last_report_time = now
            self._last_reported_frame = self._frame_count

        # Map each tracker to a VMT device index
        for idx, tr in enumerate(frame.trackers):
            if not tr.valid:
                continue

            # 1. Convert Unity (left-handed) to OpenVR (right-handed)
            vr_pos = unity_to_openvr_pos(tr.pos)
            vr_rot = unity_to_openvr_quat(tr.rot)

            # 2. Apply space calibration (Pico space -> Steam Frame space)
            calib_pos = self.calib.apply_pos(vr_pos)
            calib_rot = self.calib.apply_rot(vr_rot)

            # 3. Publish to SteamVR via VMT
            self.vmt.send_pose(
                index=idx,
                pos=calib_pos,
                quat=calib_rot,
                enabled=True,
                time_offset=0.0,
            )

            # 4. Optional battery report
            if tr.battery >= 0.0:
                self.vmt.send_diag(idx, f"bat: {int(tr.battery * 100)}%")

    def run(self) -> None:
        try:
            self.receiver.start()
        except KeyboardInterrupt:
            logger.info("Shutting down bridge...")
            self.receiver.stop()


def main() -> None:
    parser = argparse.ArgumentParser(description="Pico Motion Tracker -> SteamVR Bridge")
    parser.add_argument("--port", type=int, default=39500, help="TCP listen port (default 39500)")
    parser.add_argument("--vmt-port", type=int, default=39570, help="VMT OSC port (default 39570)")
    parser.add_argument("--calib", type=Path, default=None, help="Path to space calibration JSON file")
    args = parser.parse_args()

    app = BridgeApp(tcp_port=args.port, vmt_port=args.vmt_port, calib_file=args.calib)
    app.run()


if __name__ == "__main__":
    main()
