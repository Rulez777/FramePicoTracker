# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Coordinate system transformations between Unity and SteamVR/OpenVR.

Unity:
  - Left-handed coordinate system.
  - +X = Right, +Y = Up, +Z = Forward.
  - Quaternions: (x, y, z, w)

SteamVR / OpenVR:
  - Right-handed coordinate system.
  - +X = Right, +Y = Up, -Z = Forward (+Z = Backward).
  - Quaternions: (x, y, z, w)
"""

from __future__ import annotations
import numpy as np


def unity_to_openvr_pos(pos: np.ndarray) -> np.ndarray:
    """Convert position from Unity left-handed to OpenVR right-handed.

    (x, y, z) -> (x, y, -z)
    """
    p = np.asarray(pos, dtype=np.float32)
    return np.array([p[0], p[1], -p[2]], dtype=np.float32)


def openvr_to_unity_pos(pos: np.ndarray) -> np.ndarray:
    """Convert position from OpenVR right-handed to Unity left-handed."""
    return unity_to_openvr_pos(pos)


def unity_to_openvr_quat(quat: np.ndarray) -> np.ndarray:
    """Convert orientation quaternion from Unity to OpenVR.

    Reflection across XY-plane (z -> -z):
    (qx, qy, qz, qw) -> (-qx, -qy, qz, qw)
    """
    q = np.asarray(quat, dtype=np.float32)
    norm = np.linalg.norm(q)
    if norm > 1e-8:
        q = q / norm
    return np.array([-q[0], -q[1], q[2], q[3]], dtype=np.float32)


def openvr_to_unity_quat(quat: np.ndarray) -> np.ndarray:
    """Convert orientation quaternion from OpenVR to Unity."""
    return unity_to_openvr_quat(quat)


def unity_to_openvr_vel(vel: np.ndarray) -> np.ndarray:
    """Convert linear velocity from Unity to OpenVR."""
    return unity_to_openvr_pos(vel)


def unity_to_openvr_angvel(angvel: np.ndarray) -> np.ndarray:
    """Convert angular velocity from Unity to OpenVR."""
    w = np.asarray(angvel, dtype=np.float32)
    return np.array([-w[0], -w[1], w[2]], dtype=np.float32)


def quat_to_rot_matrix(q: np.ndarray) -> np.ndarray:
    """Convert quaternion (qx, qy, qz, qw) to 3x3 rotation matrix."""
    x, y, z, w = q
    norm = np.sqrt(x * x + y * y + z * z + w * w)
    if norm > 1e-8:
        x, y, z, w = x / norm, y / norm, z / norm, w / norm

    xx, yy, zz = x * x, y * y, z * z
    xy, xz, yz = x * y, x * z, y * z
    wx, wy, wz = w * x, w * y, w * z

    return np.array(
        [
            [1.0 - 2.0 * (yy + zz), 2.0 * (xy - wz), 2.0 * (xz + wy)],
            [2.0 * (xy + wz), 1.0 - 2.0 * (xx + zz), 2.0 * (yz - wx)],
            [2.0 * (xz - wy), 2.0 * (yz + wx), 1.0 - 2.0 * (xx + yy)],
        ],
        dtype=np.float32,
    )


def rot_matrix_to_quat(R: np.ndarray) -> np.ndarray:
    """Convert 3x3 rotation matrix to quaternion (qx, qy, qz, qw)."""
    tr = R[0, 0] + R[1, 1] + R[2, 2]
    if tr > 0:
        s = 0.5 / np.sqrt(tr + 1.0)
        w = 0.25 / s
        x = (R[2, 1] - R[1, 2]) * s
        y = (R[0, 2] - R[2, 0]) * s
        z = (R[1, 0] - R[0, 1]) * s
    elif (R[0, 0] > R[1, 1]) and (R[0, 0] > R[2, 2]):
        s = 2.0 * np.sqrt(1.0 + R[0, 0] - R[1, 1] - R[2, 2])
        w = (R[2, 1] - R[1, 2]) / s
        x = 0.25 * s
        y = (R[0, 1] + R[1, 0]) / s
        z = (R[0, 2] + R[2, 0]) / s
    elif R[1, 1] > R[2, 2]:
        s = 2.0 * np.sqrt(1.0 + R[1, 1] - R[0, 0] - R[2, 2])
        w = (R[0, 2] - R[2, 0]) / s
        x = (R[0, 1] + R[1, 0]) / s
        y = 0.25 * s
        z = (R[1, 2] + R[2, 1]) / s
    else:
        s = 2.0 * np.sqrt(1.0 + R[2, 2] - R[0, 0] - R[1, 1])
        w = (R[1, 0] - R[0, 1]) / s
        x = (R[0, 2] + R[2, 0]) / s
        y = (R[1, 2] + R[2, 1]) / s
        z = 0.25 * s

    q = np.array([x, y, z, w], dtype=np.float32)
    return q / np.linalg.norm(q)


def quat_multiply(q1: np.ndarray, q2: np.ndarray) -> np.ndarray:
    """Hamilton product of two quaternions (x, y, z, w): q = q1 * q2."""
    x1, y1, z1, w1 = q1
    x2, y2, z2, w2 = q2
    return np.array(
        [
            w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2,
            w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2,
            w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2,
            w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2,
        ],
        dtype=np.float32,
    )
