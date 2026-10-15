# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Camera Stream Ingestion for Steam Frame Optical Tracking.

Provides unified interface for tracking camera frames:
- `SyntheticCameraStream`: Simulates 60-90 Hz camera frames with tracker moving on
  3D trajectories, rendering Gaussian IR LED spots, subpixel noise, and occlusion.
- `UvcCameraStream`: Ingests from USB / UVC tracking camera hardware (via OpenCV/pyuvc).
- `NetworkCameraStream`: Receives raw or JPEG camera frames over UDP/TCP from Steam Frame HMD.
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from dataclasses import dataclass
import logging
import math
import time
import numpy as np

try:
    from optical.constellation import CameraIntrinsics, TrackerConstellation
except ImportError:
    from constellation import CameraIntrinsics, TrackerConstellation

from coords import quat_to_rot_matrix

logger = logging.getLogger(__name__)


@dataclass(slots=True)
class CameraFrame:
    """Captured frame metadata and image buffer."""

    image: np.ndarray  # Grayscale (H, W) uint8
    timestamp_ns: int  # Hardware timestamp in nanoseconds
    frame_index: int
    camera_id: int = 0


class CameraStream(ABC):
    """Abstract base class for tracking camera streams."""

    @abstractmethod
    def read_frame(self) -> CameraFrame | None:
        """Fetch next frame. Blocks until frame is available or returns None on EOF."""
        pass

    @abstractmethod
    def close(self) -> None:
        """Release camera resources."""
        pass


class SyntheticCameraStream(CameraStream):
    """Generates synthetic 60-90 Hz tracking camera frames with moving tracker."""

    def __init__(
        self,
        camera: CameraIntrinsics,
        constellation: TrackerConstellation | None = None,
        fps: float = 60.0,
        spot_sigma: float = 1.8,
        noise_std: float = 2.0,
    ) -> None:
        self.camera = camera
        self.constellation = constellation or TrackerConstellation()
        self.fps = fps
        self.spot_sigma = spot_sigma
        self.noise_std = noise_std

        self.frame_index = 0
        self.start_time = time.time()
        self.rng = np.random.default_rng(12345)

    def get_ground_truth_pose(self, t: float) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
        """Compute synthetic ground truth tracker position, orientation, and body acceleration.

        Simulates ankle/foot motion (figure-8 / circular trajectory).
        Returns:
            (position_m, quat_xyzw, raw_acc_body_mps2)
        """
        # Translation: 0.9m distance, subtle leg swings within FOV
        wx, wy, wz = 1.2, 2.0, 0.8
        x = 0.12 * math.sin(wx * t)
        y = -0.32 + 0.05 * math.cos(wy * t)
        z = 0.90 + 0.08 * math.sin(wz * t)
        pos = np.array([x, y, z], dtype=np.float32)

        # True kinematic linear acceleration: d^2(pos)/dt^2
        ax = -0.12 * (wx**2) * math.sin(wx * t)
        ay = -0.05 * (wy**2) * math.cos(wy * t)
        az = -0.08 * (wz**2) * math.sin(wz * t)
        acc_world = np.array([ax, ay, az], dtype=np.float32)

        # Orientation: pitch oscillation (-12 to +12 deg)
        pitch = math.radians(12.0 * math.sin(1.2 * t))
        yaw = math.radians(8.0 * math.cos(0.9 * t))
        cy, sy = math.cos(yaw * 0.5), math.sin(yaw * 0.5)
        cp, sp = math.cos(pitch * 0.5), math.sin(pitch * 0.5)
        quat = np.array([sp * cy, cp * sy, -sp * sy, cp * cy], dtype=np.float32)
        quat /= np.linalg.norm(quat)

        # Specific force measured by IMU in body frame: a_body = R.T @ (a_world + [0, 9.80665, 0])
        R_mat = quat_to_rot_matrix(quat)
        g_world = np.array([0.0, 9.80665, 0.0], dtype=np.float32)
        raw_acc_body = (R_mat.T @ (acc_world + g_world)).astype(np.float32)

        return pos, quat, raw_acc_body

    def read_frame(
        self,
        pose: tuple[np.ndarray, np.ndarray] | None = None,
    ) -> CameraFrame:
        """Generate and render next synthetic camera frame.

        Args:
            pose: Optional (pos_cam, quat_cam) to render in a specific camera frame.
        """
        t = self.frame_index / self.fps
        if pose is not None:
            pos, quat = pose
        else:
            pos, quat, _ = self.get_ground_truth_pose(t)

        # Initialize dark background with sensor noise
        H, W = self.camera.height, self.camera.width
        img = self.rng.normal(5.0, self.noise_std, (H, W)).astype(np.float32)

        # Project visible LEDs
        proj_2d, visible_idx, _ = self.constellation.get_visible_leds(pos, quat, self.camera)

        # Render Gaussian intensity spots for each visible LED
        y_coords, x_coords = np.indices((H, W))
        two_sigma_sq = 2.0 * (self.spot_sigma**2)

        for u, v in proj_2d:
            if 0 <= u < W and 0 <= v < H:
                # Local window around spot (radius 5 px for speed)
                r = int(math.ceil(3.0 * self.spot_sigma))
                x_min, x_max = max(0, int(u) - r), min(W, int(u) + r + 1)
                y_min, y_max = max(0, int(v) - r), min(H, int(v) + r + 1)

                sub_x = x_coords[y_min:y_max, x_min:x_max]
                sub_y = y_coords[y_min:y_max, x_min:x_max]
                dist_sq = (sub_x - u) ** 2 + (sub_y - v) ** 2
                spot = 240.0 * np.exp(-dist_sq / two_sigma_sq)
                img[y_min:y_max, x_min:x_max] += spot

        img_uint8 = np.clip(img, 0, 255).astype(np.uint8)

        now_ns = int(time.time() * 1e9)
        frame = CameraFrame(
            image=img_uint8,
            timestamp_ns=now_ns,
            frame_index=self.frame_index,
            camera_id=0,
        )
        self.frame_index += 1
        return frame

    def close(self) -> None:
        pass


class UvcCameraStream(CameraStream):
    """Reads frames from physical UVC camera using OpenCV if available."""

    def __init__(self, camera_index: int = 0, width: int = 640, height: int = 480) -> None:
        self.camera_index = camera_index
        self.cap = None
        self.frame_index = 0

        try:
            import cv2
            self.cap = cv2.VideoCapture(camera_index)
            self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
            self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
            if not self.cap.isOpened():
                logger.warning(f"Could not open camera device {camera_index}")
                self.cap = None
        except ImportError:
            logger.info("OpenCV not installed; UvcCameraStream unavailable without cv2.")

    def read_frame(self) -> CameraFrame | None:
        if self.cap is None:
            return None

        import cv2
        ret, frame_bgr = self.cap.read()
        if not ret or frame_bgr is None:
            return None

        gray = cv2.cvtColor(frame_bgr, cv2.COLOR_BGR2GRAY)
        now_ns = int(time.time() * 1e9)
        frame = CameraFrame(
            image=gray,
            timestamp_ns=now_ns,
            frame_index=self.frame_index,
            camera_id=self.camera_index,
        )
        self.frame_index += 1
        return frame

    def close(self) -> None:
        if self.cap is not None:
            self.cap.release()
            self.cap = None
