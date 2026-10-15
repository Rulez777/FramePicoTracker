# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Space calibration between Pico 4 tracking space and Steam Frame (SteamVR) space.

Implements Kabsch (orthogonal Procrustes) algorithm to estimate the optimal
rigid transformation (Rotation R, Translation T) from paired 3D point samples.
"""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
import numpy as np

from coords import quat_to_rot_matrix, rot_matrix_to_quat, quat_multiply


@dataclass
class SpaceTransform:
    """Rigid transform from source space (e.g. Pico) to target space (SteamVR).

    P_target = R @ P_source + T
    Q_target = Q_rot * Q_source
    """

    rotation_matrix: np.ndarray  # 3x3 float32
    translation: np.ndarray  # 3, float32
    rotation_quat: np.ndarray  # 4, float32 (qx, qy, qz, qw)

    @classmethod
    def identity(cls) -> SpaceTransform:
        R = np.eye(3, dtype=np.float32)
        T = np.zeros(3, dtype=np.float32)
        Q = np.array([0.0, 0.0, 0.0, 1.0], dtype=np.float32)
        return cls(rotation_matrix=R, translation=T, rotation_quat=Q)

    def apply_pos(self, pos: np.ndarray) -> np.ndarray:
        p = np.asarray(pos, dtype=np.float32)
        return self.rotation_matrix @ p + self.translation

    def apply_rot(self, quat: np.ndarray) -> np.ndarray:
        q = np.asarray(quat, dtype=np.float32)
        return quat_multiply(self.rotation_quat, q)

    def save(self, path: str | Path) -> None:
        data = {
            "rotation_matrix": self.rotation_matrix.tolist(),
            "translation": self.translation.tolist(),
            "rotation_quat": self.rotation_quat.tolist(),
        }
        with open(path, "w", encoding="utf-8") as f:
            json.dump(data, f, indent=2)

    @classmethod
    def load(cls, path: str | Path) -> SpaceTransform:
        with open(path, "r", encoding="utf-8") as f:
            data = json.load(f)
        R = np.array(data["rotation_matrix"], dtype=np.float32)
        T = np.array(data["translation"], dtype=np.float32)
        Q = np.array(data["rotation_quat"], dtype=np.float32)
        return cls(rotation_matrix=R, translation=T, rotation_quat=Q)


def estimate_rigid_transform(
    source_points: np.ndarray,
    target_points: np.ndarray,
) -> SpaceTransform:
    """Compute optimal R, T mapping source_points to target_points using Kabsch algorithm.

    Args:
        source_points: (N, 3) float array of 3D positions in source space.
        target_points: (N, 3) float array of 3D positions in target space.

    Returns:
        SpaceTransform object.
    """
    P = np.asarray(source_points, dtype=np.float64)
    Q = np.asarray(target_points, dtype=np.float64)

    if P.shape != Q.shape or P.ndim != 2 or P.shape[1] != 3:
        raise ValueError(f"Invalid shapes: P={P.shape}, Q={Q.shape}. Expected (N, 3)")

    N = P.shape[0]
    if N < 3:
        raise ValueError(f"Need at least 3 non-collinear point pairs, got {N}")

    centroid_P = np.mean(P, axis=0)
    centroid_Q = np.mean(Q, axis=0)

    P_centered = P - centroid_P
    Q_centered = Q - centroid_Q

    # Covariance matrix H = P_centered.T @ Q_centered
    H = P_centered.T @ Q_centered

    U, S, Vt = np.linalg.svd(H)
    V = Vt.T

    # R = V @ diag(1, 1, det(V @ U.T)) @ U.T
    d = np.linalg.det(V @ U.T)
    diag_sign = np.array([1.0, 1.0, np.sign(d)])
    R = V @ np.diag(diag_sign) @ U.T

    # T = centroid_Q - R @ centroid_P
    T = centroid_Q - R @ centroid_P

    R_f32 = R.astype(np.float32)
    T_f32 = T.astype(np.float32)
    Q_f32 = rot_matrix_to_quat(R_f32)

    return SpaceTransform(rotation_matrix=R_f32, translation=T_f32, rotation_quat=Q_f32)
