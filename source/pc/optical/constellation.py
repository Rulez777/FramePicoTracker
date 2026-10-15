# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""3D Constellation Model for Pico Motion Tracker (Swift 2.0) IR LEDs.

Factory coordinates are excluded from this export; supply your own calibration:
8 non-coplanar IR LEDs arranged in a perimeter ring and crown.
Units: meters.
"""

from __future__ import annotations

from dataclasses import dataclass, field
import numpy as np

from coords import quat_to_rot_matrix

# Private factory defaults are deliberately absent from this source export.
def _require_private_calibration():
    raise ValueError("Supply your own device-calibrated led_positions and led_normals explicitly; no factory defaults are bundled")


@dataclass(slots=True)
class CameraIntrinsics:
    """Pinhole camera intrinsic parameters."""

    fx: float
    fy: float
    cx: float
    cy: float
    width: int = 640
    height: int = 480

    @property
    def matrix(self) -> np.ndarray:
        return np.array(
            [
                [self.fx, 0.0, self.cx],
                [0.0, self.fy, self.cy],
                [0.0, 0.0, 1.0],
            ],
            dtype=np.float32,
        )

    def project_points(self, points_cam: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
        """Project 3D camera-frame points (N, 3) to 2D image coordinates (N, 2).

        Returns:
            (projected_2d, valid_mask): 2D pixels and boolean mask of points in front of camera (Z > 0.05m).
        """
        pts = np.asarray(points_cam, dtype=np.float32)
        z = pts[:, 2]
        valid = z > 0.05  # At least 5cm in front of camera

        u = np.zeros_like(z)
        v = np.zeros_like(z)
        safe_z = np.where(valid, z, 1.0)
        u = self.fx * (pts[:, 0] / safe_z) + self.cx
        v = self.fy * (pts[:, 1] / safe_z) + self.cy

        in_bounds = (u >= 0) & (u < self.width) & (v >= 0) & (v < self.height)
        valid = valid & in_bounds

        return np.stack([u, v], axis=1), valid


@dataclass
class TrackerConstellation:
    """3D Model of the Pico Motion Tracker."""

    led_positions: np.ndarray = field(default_factory=_require_private_calibration)  # (8, 3)
    led_normals: np.ndarray = field(default_factory=_require_private_calibration)  # (8, 3)

    @property
    def num_leds(self) -> int:
        return len(self.led_positions)

    def get_world_leds(self, pos: np.ndarray, quat: np.ndarray) -> np.ndarray:
        """Transform LED positions to world/camera frame given tracker position and orientation."""
        R = quat_to_rot_matrix(quat)
        p = np.asarray(pos, dtype=np.float32)
        return (R @ self.led_positions.T).T + p

    def get_visible_leds(
        self,
        pos_cam: np.ndarray,
        quat_cam: np.ndarray,
        camera: CameraIntrinsics,
    ) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
        """Get 2D projections and indices of visible LEDs.

        Returns:
            (projected_2d, led_indices, points_cam_3d)
        """
        R = quat_to_rot_matrix(quat_cam)
        pts_cam = (R @ self.led_positions.T).T + np.asarray(pos_cam, dtype=np.float32)

        # Visibility check: LED normal should face towards the camera (dot product < 0)
        normals_cam = (R @ self.led_normals.T).T
        view_dirs = -pts_cam / np.linalg.norm(pts_cam, axis=1, keepdims=True)
        facing = np.sum(normals_cam * view_dirs, axis=1) > -0.2  # Allow up to ~100 deg view angle

        proj_2d, in_fov = camera.project_points(pts_cam)
        visible = facing & in_fov

        indices = np.where(visible)[0]
        return proj_2d[visible], indices, pts_cam[visible]
