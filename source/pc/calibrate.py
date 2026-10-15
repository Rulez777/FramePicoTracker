# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Interactive Space Calibration tool between Pico 4 and SteamVR (Steam Frame).

Collects simultaneous poses from Pico (via TCP Wire Protocol v1) and SteamVR HMD
(via openvr API), estimates the rigid 3D transformation using Kabsch algorithm,
and saves the calibration to JSON.
"""

from __future__ import annotations

import argparse
import logging
import sys
import time
from pathlib import Path
import numpy as np

try:
    import openvr
except ImportError:
    openvr = None

from calibration import estimate_rigid_transform
from coords import unity_to_openvr_pos
from protocol import FramePayload
from receiver import StreamReceiver

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
logger = logging.getLogger("calibrate")


class SpaceCalibratorTool:
    def __init__(self, tcp_port: int = 39500, output_file: Path = Path("calibration.json")) -> None:
        self.tcp_port = tcp_port
        self.output_file = output_file

        self.pico_points: list[np.ndarray] = []
        self.steam_points: list[np.ndarray] = []

        self._latest_pico_hmd: np.ndarray | None = None
        self._vr_system = None

    def init_steamvr(self) -> bool:
        if openvr is None:
            logger.error("openvr package is not installed.")
            return False
        try:
            self._vr_system = openvr.init(openvr.VRApplication_Utility)
            logger.info("Connected to SteamVR runtime.")
            return True
        except Exception as e:
            logger.error("Failed to connect to SteamVR: %s", e)
            return False

    def get_steamvr_hmd_pos(self) -> np.ndarray | None:
        if not self._vr_system:
            return None
        poses = self._vr_system.getDeviceToAbsoluteTrackingPose(
            openvr.TrackingUniverseStanding, 0, openvr.k_unMaxTrackedDeviceCount
        )
        hmd_pose = poses[openvr.k_unTrackedDeviceIndex_Hmd]
        if not hmd_pose.bPoseIsValid:
            return None

        m = hmd_pose.mDeviceToAbsoluteTracking
        # Matrix is 3x4 [R | T]
        pos = np.array([m[0][3], m[1][3], m[2][3]], dtype=np.float32)
        return pos

    def on_pico_frame(self, frame: FramePayload) -> None:
        # Convert Pico HMD pos to OpenVR convention
        pico_hmd_ovr = unity_to_openvr_pos(frame.hmd_pos)
        self._latest_pico_hmd = pico_hmd_ovr

        steam_pos = self.get_steamvr_hmd_pos()
        if steam_pos is not None and self._latest_pico_hmd is not None:
            self.pico_points.append(self._latest_pico_hmd)
            self.steam_points.append(steam_pos)

            if len(self.pico_points) % 50 == 0:
                logger.info("Collected %d point pairs...", len(self.pico_points))

    def run(self, target_samples: int = 300) -> None:
        if not self.init_steamvr():
            logger.warning("SteamVR not available. You can also run offline calibration with recorded logs.")

        logger.info("Starting calibration capture...")
        logger.info("Move both headsets smoothly together in a figure-8 / circle pattern.")

        receiver = StreamReceiver(
            port=self.tcp_port,
            on_frame=self.on_pico_frame,
        )

        try:
            receiver.start()
        except KeyboardInterrupt:
            pass
        finally:
            receiver.stop()

        if len(self.pico_points) < 10:
            logger.error("Not enough samples collected (%d < 10). Calibration aborted.", len(self.pico_points))
            return

        logger.info("Estimating transform from %d samples...", len(self.pico_points))
        P = np.array(self.pico_points)
        Q = np.array(self.steam_points)

        transform = estimate_rigid_transform(P, Q)
        transform.save(self.output_file)
        logger.info("Calibration saved to %s", self.output_file)
        logger.info("Translation: %s", transform.translation)
        logger.info("Rotation Quat: %s", transform.rotation_quat)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Space Calibration between Pico 4 and SteamVR")
    parser.add_argument("--port", type=int, default=39500, help="TCP port from Pico")
    parser.add_argument("--out", type=Path, default=Path("calibration.json"), help="Output JSON path")
    args = parser.parse_args()

    tool = SpaceCalibratorTool(tcp_port=args.port, output_file=args.out)
    tool.run()
