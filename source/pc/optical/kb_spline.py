# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Kannala-Brandt Fisheye Lens Model with Spline Grid Residuals (kb_spline).

Implements the official camera model used by Steam Frame tracking cameras
(e.g. Sunny AW01D04-12085, 1056x1024, kb_spline).

Model:
  1. Kannala-Brandt Fisheye Projection:
     r = sqrt(X^2 + Y^2)
     theta = atan2(r, Z)
     theta_d = theta + k1*theta^3 + k2*theta^5 + k3*theta^7 + k4*theta^9
     u_base = fx * (theta_d * X / r) + cx
     v_base = fy * (theta_d * Y / r) + cy

  2. 2D Spline Residual Grid (sU, sV):
     Evaluates residual offsets (delta_u, delta_v) from a 5x5 grid of control points
     with node spacing (nodeSpacing_x, nodeSpacing_y) and crop offset (crop_x, crop_y).
     u = u_base + delta_u - crop_x
     v = v_base + delta_v - crop_y

  3. Inverse Unprojection:
     Given distorted pixel (u, v), inverts spline residual and solves for theta
     via Newton-Raphson to produce a metric 3D direction unit ray in camera frame.
"""

from __future__ import annotations

import json
import math
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any
import numpy as np


def _require_camera_calibration():
    raise ValueError("Supply explicit recipient camera calibration; bundled defaults were removed")


@dataclass
class KBSplineCamera:
    """Steam Frame kb_spline camera model."""

    name: str = "slam_right"
    width: int = 1056
    height: int = 1024
    crop_x: float = field(default_factory=_require_camera_calibration)
    crop_y: float = field(default_factory=_require_camera_calibration)

    fx: float = field(default_factory=_require_camera_calibration)
    fy: float = field(default_factory=_require_camera_calibration)
    cx: float = field(default_factory=_require_camera_calibration)
    cy: float = field(default_factory=_require_camera_calibration)

    k1: float = field(default_factory=_require_camera_calibration)
    k2: float = field(default_factory=_require_camera_calibration)
    k3: float = field(default_factory=_require_camera_calibration)
    k4: float = field(default_factory=_require_camera_calibration)

    grid_size: tuple[int, int] = (5, 5)
    node_spacing: tuple[float, float] = field(default_factory=_require_camera_calibration)

    sU: np.ndarray = field(default_factory=lambda: np.zeros((5, 5), dtype=np.float32))
    sV: np.ndarray = field(default_factory=lambda: np.zeros((5, 5), dtype=np.float32))

    position_m: np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.float32))
    plus_x: np.ndarray = field(default_factory=lambda: np.array([1.0, 0.0, 0.0], dtype=np.float32))
    plus_z: np.ndarray = field(default_factory=lambda: np.array([0.0, 0.0, 1.0], dtype=np.float32))

    def eval_spline_residual(
        self,
        u_base: np.ndarray | float,
        v_base: np.ndarray | float,
    ) -> tuple[np.ndarray, np.ndarray]:
        """Bilinear evaluation of the 5x5 spline grid residual offsets."""
        u_arr = np.asarray(u_base, dtype=np.float32)
        v_arr = np.asarray(v_base, dtype=np.float32)

        # Center grid around principal point (cx, cy)
        # Grid center is at index 2 (for 5x5 grid)
        col = (u_arr - self.cx) / self.node_spacing[0] + 2.0
        row = (v_arr - self.cy) / self.node_spacing[1] + 2.0

        col_c = np.clip(col, 0.0, 3.999)
        row_c = np.clip(row, 0.0, 3.999)

        c0 = np.floor(col_c).astype(int)
        r0 = np.floor(row_c).astype(int)
        c1 = c0 + 1
        r1 = r0 + 1

        fc = col_c - c0
        fr = row_c - r0

        # Bilinear interpolation weights
        w00 = (1.0 - fc) * (1.0 - fr)
        w10 = fc * (1.0 - fr)
        w01 = (1.0 - fc) * fr
        w11 = fc * fr

        delta_u = (
            w00 * self.sU[r0, c0]
            + w10 * self.sU[r0, c1]
            + w01 * self.sU[r1, c0]
            + w11 * self.sU[r1, c1]
        )
        delta_v = (
            w00 * self.sV[r0, c0]
            + w10 * self.sV[r0, c1]
            + w01 * self.sV[r1, c0]
            + w11 * self.sV[r1, c1]
        )

        return delta_u, delta_v

    def project_points(
        self,
        points_cam: np.ndarray,
    ) -> tuple[np.ndarray, np.ndarray]:
        """Project 3D camera-frame points (N, 3) to 2D distorted pixel coordinates (N, 2).

        Args:
            points_cam: (N, 3) array [X, Y, Z] in meters.

        Returns:
            (pixels_2d, valid_mask): (N, 2) pixel coords and boolean mask.
        """
        pts = np.asarray(points_cam, dtype=np.float32)
        if pts.ndim == 1:
            pts = pts.reshape(1, 3)

        x, y, z = pts[:, 0], pts[:, 1], pts[:, 2]
        r = np.sqrt(x * x + y * y)

        # Points must be in front of the camera (Z > 0.05m)
        valid = z > 0.05

        safe_r = np.where(r > 1e-12, r, 1.0)
        theta = np.arctan2(r, np.where(valid, z, 1.0))

        # Kannala-Brandt distortion polynomial
        th2 = theta * theta
        th3 = th2 * theta
        th5 = th3 * th2
        th7 = th5 * th2
        th9 = th7 * th2

        theta_d = theta + self.k1 * th3 + self.k2 * th5 + self.k3 * th7 + self.k4 * th9

        scale = np.where(r > 1e-12, theta_d / safe_r, 0.0)
        u_base = self.fx * (x * scale) + self.cx
        v_base = self.fy * (y * scale) + self.cy

        delta_u, delta_v = self.eval_spline_residual(u_base, v_base)

        u = u_base + delta_u - self.crop_x
        v = v_base + delta_v - self.crop_y

        in_bounds = (u >= 0) & (u < self.width) & (v >= 0) & (v < self.height)
        valid = valid & in_bounds

        return np.column_stack((u, v)), valid

    def unproject_pixels(
        self,
        pixels_2d: np.ndarray,
        max_iters: int = 8,
        tol: float = 1e-7,
    ) -> np.ndarray:
        """Unproject 2D distorted pixels to 3D unit ray directions [X, Y, Z] in camera frame.

        Args:
            pixels_2d: (N, 2) distorted pixel coordinates.
            max_iters: Max Newton-Raphson iterations for polynomial inversion.
            tol: Inversion tolerance in radians.

        Returns:
            (N, 3) array of unit ray direction vectors [X, Y, Z].
        """
        pts = np.asarray(pixels_2d, dtype=np.float32)
        if pts.ndim == 1:
            pts = pts.reshape(1, 2)

        u_raw = pts[:, 0] + self.crop_x
        v_raw = pts[:, 1] + self.crop_y

        # Subtract spline residual
        delta_u, delta_v = self.eval_spline_residual(u_raw, v_raw)
        u_base = u_raw - delta_u
        v_base = v_raw - delta_v

        # Distorted normalized coords
        xd = (u_base - self.cx) / self.fx
        yd = (v_base - self.cy) / self.fy
        theta_d = np.sqrt(xd * xd + yd * yd)

        # Invert Kannala-Brandt polynomial theta_d = f(theta) via Newton-Raphson
        theta = np.copy(theta_d)
        for _ in range(max_iters):
            th2 = theta * theta
            th3 = th2 * theta
            th5 = th3 * th2
            th7 = th5 * th2
            th9 = th7 * th2

            f_val = theta + self.k1 * th3 + self.k2 * th5 + self.k3 * th7 + self.k4 * th9 - theta_d
            if np.max(np.abs(f_val)) < tol:
                break

            f_prime = (
                1.0
                + 3.0 * self.k1 * th2
                + 5.0 * self.k2 * (th2 * th2)
                + 7.0 * self.k3 * (th3 * th3)
                + 9.0 * self.k4 * (th2 * th2 * th2 * th2)
            )
            theta -= f_val / np.where(np.abs(f_prime) > 1e-12, f_prime, 1.0)

        # Build 3D ray direction: [sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)]
        safe_thd = np.where(theta_d > 1e-12, theta_d, 1.0)
        sin_th = np.sin(theta)
        cos_th = np.cos(theta)

        dir_x = np.where(theta_d > 1e-12, sin_th * (xd / safe_thd), 0.0)
        dir_y = np.where(theta_d > 1e-12, sin_th * (yd / safe_thd), 0.0)
        dir_z = np.where(theta_d > 1e-12, cos_th, 1.0)

        rays = np.column_stack((dir_x, dir_y, dir_z))
        norms = np.linalg.norm(rays, axis=1, keepdims=True)
        return rays / np.where(norms > 1e-12, norms, 1.0)

    def unproject_normalized(
        self,
        pixels_2d: np.ndarray,
    ) -> np.ndarray:
        """Convert distorted pixels to normalized pinhole coordinates (x, y) = (X/Z, Y/Z).

        For direct feeding into linear PnP equations:
          Tx - x * Tz = x * P'z - P'x
          Ty - y * Tz = y * P'z - P'y
        """
        rays = self.unproject_pixels(pixels_2d)
        safe_z = np.where(np.abs(rays[:, 2]) > 1e-6, rays[:, 2], 1.0)
        return np.column_stack((rays[:, 0] / safe_z, rays[:, 1] / safe_z))


def load_camera_from_calibration(
    calibration_file: Path | str,
    camera_name: str = "slam_right",
) -> KBSplineCamera:
    """Load camera parameters directly from Steam Frame sensorRigCalibration.json."""
    with open(calibration_file, "r", encoding="utf-8") as f:
        data = json.load(f)

    # Find matching camera in calibrations
    for cal in data.get("alternate_calibrations", []):
        for cam in cal.get("cameras", []):
            if cam.get("sourceCamera") == camera_name:
                intr = cam.get("intrinsics", {})
                crop = cam.get("cropRegion") or {}
                ext = cam.get("extrinsics", {})
                pos = np.array(ext.get("position", [0, 0, 0]), dtype=np.float32) / 1000.0  # mm -> m

                su_raw = intr.get("sU", [])
                sv_raw = intr.get("sV", [])
                sU = np.array(su_raw, dtype=np.float32).reshape((5, 5)) if len(su_raw) == 25 else np.zeros((5, 5), dtype=np.float32)
                sV = np.array(sv_raw, dtype=np.float32).reshape((5, 5)) if len(sv_raw) == 25 else np.zeros((5, 5), dtype=np.float32)

                return KBSplineCamera(
                    name=camera_name,
                    width=int(cam.get("width", 1056)),
                    height=int(cam.get("height", 1024)),
                    crop_x=float(crop.get("x", 0.0)),
                    crop_y=float(crop.get("y", 0.0)),
                    fx=float(intr.get("fx", 329.6)),
                    fy=float(intr.get("fy", 328.9)),
                    cx=float(intr.get("cx", 533.9)),
                    cy=float(intr.get("cy", 512.3)),
                    k1=float(intr.get("k1", 0.0)),
                    k2=float(intr.get("k2", 0.0)),
                    k3=float(intr.get("k3", 0.0)),
                    k4=float(intr.get("k4", 0.0)),
                    grid_size=tuple(intr.get("gridSize", [5, 5])),
                    node_spacing=tuple(intr.get("nodeSpacing", [228.8, 221.87])),
                    sU=sU,
                    sV=sV,
                    position_m=pos,
                    plus_x=np.array(ext.get("plus_x", [1, 0, 0]), dtype=np.float32),
                    plus_z=np.array(ext.get("plus_z", [0, 0, 1]), dtype=np.float32),
                )

    raise ValueError(f"Camera '{camera_name}' not found in calibration file")
