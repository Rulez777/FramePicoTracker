# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Brown-Conrady Camera Lens Distortion Model.

Provides fast forward distortion and inverse undistortion for 2D subpixel blob
centroids detected on wide-angle tracking cameras (e.g. Steam Frame tracking cameras).

Undistorting blob centers directly in O(N) is orders of magnitude faster (< 1 us)
than undistorting entire high-resolution camera frames (5-10 ms).
"""

from __future__ import annotations

from dataclasses import dataclass
import numpy as np


@dataclass(slots=True)
class DistortionCoeffs:
    """Radial and tangential lens distortion coefficients.

    Model:
      r^2 = x^2 + y^2
      radial = 1 + k1*r^2 + k2*r^4 + k3*r^6
      x_dist = x * radial + 2*p1*x*y + p2*(r^2 + 2*x^2)
      y_dist = y * radial + p1*(r^2 + 2*y^2) + 2*p2*x*y
    """

    k1: float = 0.0
    k2: float = 0.0
    p1: float = 0.0
    p2: float = 0.0
    k3: float = 0.0

    @property
    def is_zero(self) -> bool:
        return (
            self.k1 == 0.0
            and self.k2 == 0.0
            and self.p1 == 0.0
            and self.p2 == 0.0
            and self.k3 == 0.0
        )

    def as_array(self) -> np.ndarray:
        return np.array([self.k1, self.k2, self.p1, self.p2, self.k3], dtype=np.float64)


def distort_normalized_points(
    pts_norm: np.ndarray,
    coeffs: DistortionCoeffs,
) -> np.ndarray:
    """Apply radial and tangential distortion to normalized points (x, y) = (X/Z, Y/Z).

    Args:
        pts_norm: (N, 2) array of normalized coordinates.
        coeffs: DistortionCoeffs.

    Returns:
        (N, 2) array of distorted normalized coordinates (xd, yd).
    """
    if coeffs.is_zero or len(pts_norm) == 0:
        return np.copy(pts_norm)

    pts = np.asarray(pts_norm, dtype=np.float64)
    x = pts[:, 0]
    y = pts[:, 1]

    r2 = x * x + y * y
    r4 = r2 * r2
    r6 = r4 * r2

    radial = 1.0 + coeffs.k1 * r2 + coeffs.k2 * r4 + coeffs.k3 * r6

    xd = x * radial + 2.0 * coeffs.p1 * x * y + coeffs.p2 * (r2 + 2.0 * x * x)
    yd = y * radial + coeffs.p1 * (r2 + 2.0 * y * y) + 2.0 * coeffs.p2 * x * y

    return np.column_stack((xd, yd))


def undistort_normalized_points(
    pts_dist_norm: np.ndarray,
    coeffs: DistortionCoeffs,
    max_iters: int = 5,
    tol: float = 1e-6,
) -> np.ndarray:
    """Invert distortion using Newton-Raphson iteration.

    Given distorted normalized points (xd, yd), finds undistorted normalized points (x, y).

    Args:
        pts_dist_norm: (N, 2) distorted normalized points.
        coeffs: DistortionCoeffs.
        max_iters: Max Newton-Raphson iterations (typically converges in 3-4 steps).
        tol: Convergence tolerance.

    Returns:
        (N, 2) undistorted normalized coordinates (x, y).
    """
    if coeffs.is_zero or len(pts_dist_norm) == 0:
        return np.copy(pts_dist_norm)

    pts_d = np.asarray(pts_dist_norm, dtype=np.float64)
    # Initial guess is the distorted point
    x = np.copy(pts_d[:, 0])
    y = np.copy(pts_d[:, 1])
    xd_target = pts_d[:, 0]
    yd_target = pts_d[:, 1]

    for _ in range(max_iters):
        r2 = x * x + y * y
        r4 = r2 * r2
        r6 = r4 * r2

        radial = 1.0 + coeffs.k1 * r2 + coeffs.k2 * r4 + coeffs.k3 * r6

        fx = x * radial + 2.0 * coeffs.p1 * x * y + coeffs.p2 * (r2 + 2.0 * x * x) - xd_target
        fy = y * radial + coeffs.p1 * (r2 + 2.0 * y * y) + 2.0 * coeffs.p2 * x * y - yd_target

        if np.max(np.abs(fx)) < tol and np.max(np.abs(fy)) < tol:
            break

        # Derivative of radial w.r.t r2
        d_radial_dr2 = coeffs.k1 + 2.0 * coeffs.k2 * r2 + 3.0 * coeffs.k3 * r4

        # Partial derivatives
        dfx_dx = radial + x * d_radial_dr2 * (2.0 * x) + 2.0 * coeffs.p1 * y + 6.0 * coeffs.p2 * x
        dfx_dy = x * d_radial_dr2 * (2.0 * y) + 2.0 * coeffs.p1 * x + 2.0 * coeffs.p2 * y

        dfy_dx = y * d_radial_dr2 * (2.0 * x) + 2.0 * coeffs.p1 * x + 2.0 * coeffs.p2 * y
        dfy_dy = radial + y * d_radial_dr2 * (2.0 * y) + 6.0 * coeffs.p1 * y + 2.0 * coeffs.p2 * x

        # 2x2 determinant per point: det = dfx_dx * dfy_dy - dfx_dy * dfy_dx
        det = dfx_dx * dfy_dy - dfx_dy * dfy_dx
        det = np.where(np.abs(det) < 1e-12, 1e-12, det)

        # Newton step: delta = -inv(J) * f
        dx = -(dfy_dy * fx - dfx_dy * fy) / det
        dy = -(-dfy_dx * fx + dfx_dx * fy) / det

        x += dx
        y += dy

    return np.column_stack((x, y))


def undistort_pixel_points(
    pixel_pts: np.ndarray,
    fx: float,
    fy: float,
    cx: float,
    cy: float,
    coeffs: DistortionCoeffs,
) -> np.ndarray:
    """Convert distorted pixel points (u_d, v_d) to undistorted pixel points (u, v).

    Args:
        pixel_pts: (N, 2) distorted pixel coordinates.
        fx, fy, cx, cy: Camera intrinsics.
        coeffs: Distortion coefficients.

    Returns:
        (N, 2) undistorted pixel coordinates.
    """
    if coeffs.is_zero or len(pixel_pts) == 0:
        return np.copy(pixel_pts)

    pts = np.asarray(pixel_pts, dtype=np.float64)
    # 1. Pixel -> Distorted Normalized
    xd = (pts[:, 0] - cx) / fx
    yd = (pts[:, 1] - cy) / fy
    pts_d_norm = np.column_stack((xd, yd))

    # 2. Undistort normalized
    pts_norm = undistort_normalized_points(pts_d_norm, coeffs)

    # 3. Normalized -> Undistorted Pixel
    u = pts_norm[:, 0] * fx + cx
    v = pts_norm[:, 1] * fy + cy

    return np.column_stack((u, v))
