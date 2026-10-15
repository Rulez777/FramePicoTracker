# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Optical-Inertial Tracker Engine for Steam Frame.

Combines Subpixel Blob Detection on Steam Frame cameras, Prior-assisted PnP
using the 8-LED Pico Motion Tracker constellation, and Extended Kalman Filter (EKF)
fusion with 200 Hz IMU telemetry.
"""

from __future__ import annotations

import logging
import time
import numpy as np

try:
    from optical.blob_detector import BlobDetector
    from optical.constellation import CameraIntrinsics, TrackerConstellation
    from optical.fusion_ekf import OpticalInertialEKF
    from optical.pnp_solver import PnPSolution, PriorAssistedPnPSolver
except ImportError:
    from blob_detector import BlobDetector
    from constellation import CameraIntrinsics, TrackerConstellation
    from fusion_ekf import OpticalInertialEKF
    from pnp_solver import PnPSolution, PriorAssistedPnPSolver

from vmt_client import VMTClient

logger = logging.getLogger(__name__)


class SteamFrameOpticalTracker:
    def __init__(
        self,
        camera: CameraIntrinsics,
        constellation: TrackerConstellation | None = None,
        tracker_index: int = 0,
        vmt_client: VMTClient | None = None,
        anchor_timeout: float = 0.1,
    ) -> None:
        self.camera = camera
        self.constellation = constellation or TrackerConstellation()
        self.tracker_index = tracker_index
        self.vmt = vmt_client
        if not 0 < anchor_timeout <= 0.25:
            raise ValueError('Anchor timeout must be in (0, 0.25] seconds')
        self.anchor_timeout = anchor_timeout
        self.last_anchor = None

        self.blob_detector = BlobDetector()
        self.pnp_solver = PriorAssistedPnPSolver(constellation=self.constellation)
        self.ekf = OpticalInertialEKF()

        self.latest_pnp: PnPSolution | None = None
        self.total_frames = 0
        self.optical_inliers = 0

    def process_imu(self, dt: float, raw_acc: np.ndarray, quat: np.ndarray) -> np.ndarray:
        """High-frequency IMU predict step (e.g. 200 Hz).

        Returns current filtered 3D position.
        """
        # Camera-space estimates have no validated world transform here.
        # Only the timestamped world-pose pipeline may publish to native IPC.
        if self.last_anchor is None or time.monotonic()-self.last_anchor > self.anchor_timeout:
            self.last_anchor = None
            return self.ekf.position
        if (not np.isfinite(dt) or not 0 < dt <= self.anchor_timeout
                or not np.all(np.isfinite(raw_acc)) or not np.all(np.isfinite(quat))):
            self.last_anchor = None
            raise ValueError('Invalid inertial measurement; optical reacquisition required')
        return self.ekf.predict(dt=dt, raw_acc_body=raw_acc, quat=quat)

    def process_camera_frame(
        self,
        image_gray: np.ndarray,
        current_quat: np.ndarray,
    ) -> PnPSolution:
        """Optical measurement step from Steam Frame camera (60-90 Hz).

        Args:
            image_gray: 2D uint8/float grayscale image from tracking camera.
            current_quat: Current orientation quaternion in camera frame.

        Returns:
            PnPSolution with estimated 3D position and diagnostics.
        """
        self.total_frames += 1

        # 1. Detect subpixel IR blobs
        blobs = self.blob_detector.detect(image_gray)
        if len(blobs) < 2:
            return PnPSolution(
                position=self.ekf.position,
                orientation=current_quat,
                reprojection_error=999.0,
                num_matches=0,
                is_valid=False,
            )

        blob_pts = np.array([[b.x, b.y] for b in blobs], dtype=np.float32)

        # 2. Solve PnP with prior orientation and previous position estimate
        prior_pos = self.ekf.position
        sol = self.pnp_solver.match_and_solve(
            blobs_2d=blob_pts,
            quat_cam=current_quat,
            camera=self.camera,
            prior_position=prior_pos if np.linalg.norm(prior_pos) > 0.05 else None,
        )

        self.latest_pnp = sol

        # 3. If optical measurement is valid, update EKF
        if sol.is_valid:
            self.optical_inliers += 1
            if self.last_anchor is None:
                self.ekf = OpticalInertialEKF(initial_pos=sol.position)
            self.ekf.update_optical(sol.position)
            self.last_anchor = time.monotonic()

        return sol
