# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Perspective-n-Point (PnP) Solver with Prior IMU Orientation.

Solves for 3D translation T = (Tx, Ty, Tz) of the Pico Motion Tracker in camera frame,
leveraging the high-rate IMU orientation quaternion to eliminate flip ambiguities and
reduce non-linear PnP to a fast, robust linear least-squares problem with RANSAC.
"""

from __future__ import annotations

from dataclasses import dataclass
import numpy as np

try:
    from optical.constellation import CameraIntrinsics, TrackerConstellation
except ImportError:
    from constellation import CameraIntrinsics, TrackerConstellation

from coords import quat_to_rot_matrix


@dataclass(slots=True)
class PnPSolution:
    """Estimated 6DoF Pose from PnP."""

    position: np.ndarray  # (3,) camera frame [X, Y, Z] in meters
    orientation: np.ndarray  # (4,) quaternion [qx, qy, qz, qw]
    reprojection_error: float  # Mean reprojection error in pixels
    num_matches: int  # Number of inlier points used
    is_valid: bool


class PriorAssistedPnPSolver:
    def __init__(
        self,
        constellation: TrackerConstellation | None = None,
        max_reproj_error_px: float = 3.5,
    ) -> None:
        self.constellation = constellation or TrackerConstellation()
        self.max_reproj_error_px = max_reproj_error_px

    def solve_translation_linear(
        self,
        led_indices: np.ndarray,
        image_points: np.ndarray,
        quat_cam: np.ndarray,
        camera: CameraIntrinsics,
    ) -> tuple[np.ndarray | None, float]:
        """Solve for 3D translation T given matched point pairs and fixed orientation.

        Linear formulation:
          fx * Tx - (u - cx) * Tz = (u - cx) * P'z - fx * P'x
          fy * Ty - (v - cy) * Tz = (v - cy) * P'z - fy * P'y

        Returns:
            (T, mean_reprojection_error)
        """
        N = len(led_indices)
        if N < 2:
            return None, 999.0

        R = quat_to_rot_matrix(quat_cam)
        # Rotated 3D model points (N, 3)
        P_prime = (R @ self.constellation.led_positions[led_indices].T).T

        u = image_points[:, 0]
        v = image_points[:, 1]
        du = u - camera.cx
        dv = v - camera.cy

        # Build linear system A @ T = b (2N x 3)
        A = np.zeros((2 * N, 3), dtype=np.float64)
        b = np.zeros(2 * N, dtype=np.float64)

        # X equations
        A[0::2, 0] = camera.fx
        A[0::2, 2] = -du
        b[0::2] = du * P_prime[:, 2] - camera.fx * P_prime[:, 0]

        # Y equations
        A[1::2, 1] = camera.fy
        A[1::2, 2] = -dv
        b[1::2] = dv * P_prime[:, 2] - camera.fy * P_prime[:, 1]

        # Solve linear system
        sol, residuals, rank, s = np.linalg.lstsq(A, b, rcond=None)
        T = sol.astype(np.float64)

        # Check positive depth
        if T[2] <= 0.05:
            return None, 999.0

        # Non-linear Gauss-Newton refinement (minimizing true pixel reprojection error)
        for _ in range(3):
            pts_c = P_prime + T
            Z_c = pts_c[:, 2]
            if np.any(Z_c <= 0.01):
                break

            u_proj = camera.fx * (pts_c[:, 0] / Z_c) + camera.cx
            v_proj = camera.fy * (pts_c[:, 1] / Z_c) + camera.cy

            # Residuals (2N,)
            res = np.zeros(2 * N, dtype=np.float64)
            res[0::2] = u_proj - u
            res[1::2] = v_proj - v

            # Jacobian J (2N x 3)
            J = np.zeros((2 * N, 3), dtype=np.float64)
            J[0::2, 0] = camera.fx / Z_c
            J[0::2, 2] = -camera.fx * pts_c[:, 0] / (Z_c * Z_c)

            J[1::2, 1] = camera.fy / Z_c
            J[1::2, 2] = -camera.fy * pts_c[:, 1] / (Z_c * Z_c)

            # Delta T = -inv(J.T @ J) @ J.T @ res
            JtJ = J.T @ J
            Jt_res = J.T @ res
            try:
                delta_T = -np.linalg.solve(JtJ + 1e-6 * np.eye(3), Jt_res)
            except np.linalg.LinAlgError:
                break

            T = T + delta_T

        T_f32 = T.astype(np.float32)

        # Compute final reprojection error
        pts_3d = P_prime + T_f32
        proj, _ = camera.project_points(pts_3d)
        errs = np.linalg.norm(proj - image_points, axis=1)
        mean_err = float(np.mean(errs))

        return T_f32, mean_err

    def match_and_solve(
        self,
        blobs_2d: np.ndarray,
        quat_cam: np.ndarray,
        camera: CameraIntrinsics,
        prior_position: np.ndarray | None = None,
    ) -> PnPSolution:
        """Associate 2D detected blobs with 3D LEDs and solve 6DoF pose.

        Args:
            blobs_2d: (M, 2) array of detected subpixel blob centers.
            quat_cam: (4,) quaternion of tracker in camera frame from IMU.
            camera: CameraIntrinsics parameters.
            prior_position: Optional prior (e.g. from previous frame / EKF) for fast tracking.

        Returns:
            PnPSolution object.
        """
        M = len(blobs_2d)
        if M < 2:
            return PnPSolution(
                position=np.zeros(3, dtype=np.float32),
                orientation=quat_cam,
                reprojection_error=999.0,
                num_matches=0,
                is_valid=False,
            )

        R = quat_to_rot_matrix(quat_cam)
        model_leds = self.constellation.led_positions  # (8, 3)
        P_prime = (R @ model_leds.T).T  # (8, 3)

        # Initial depth guess: from prior or constellation diameter
        if prior_position is not None and prior_position[2] > 0.1:
            z_init = float(prior_position[2])
            t_init = np.copy(prior_position)
        else:
            # Estimate depth from 2D bounding diameter
            dist_matrix = np.linalg.norm(blobs_2d[:, None, :] - blobs_2d[None, :, :], axis=-1)
            max_d_px = np.max(dist_matrix)
            if max_d_px < 5.0:
                max_d_px = 5.0
            d_3d = 0.045  # ~45mm tracker diameter
            z_init = float((camera.fx * d_3d) / max_d_px)
            center_2d = np.mean(blobs_2d, axis=0)
            t_init = np.array(
                [
                    (center_2d[0] - camera.cx) * z_init / camera.fx,
                    (center_2d[1] - camera.cy) * z_init / camera.fy,
                    z_init,
                ],
                dtype=np.float32,
            )

        # Iterative closest point in 2D
        best_T = None
        best_err = 999.0
        best_matches = None

        current_T = t_init
        for _ in range(4):
            # Project all LEDs with current T
            proj, in_front = camera.project_points(P_prime + current_T)
            valid_led_idx = np.where(in_front)[0]
            if len(valid_led_idx) < 2:
                break

            # Find nearest 2D blob for each projected LED
            cost_matrix = np.linalg.norm(proj[valid_led_idx, None, :] - blobs_2d[None, :, :], axis=-1)

            matched_leds = []
            matched_blobs = []
            for k, led_i in enumerate(valid_led_idx):
                closest_blob_idx = int(np.argmin(cost_matrix[k]))
                dist = cost_matrix[k, closest_blob_idx]
                if dist < 30.0:  # Gating threshold in pixels
                    matched_leds.append(led_i)
                    matched_blobs.append(closest_blob_idx)

            if len(matched_leds) < 2:
                break

            # Solve linear translation on matched pairs
            cand_T, err = self.solve_translation_linear(
                np.array(matched_leds),
                blobs_2d[matched_blobs],
                quat_cam,
                camera,
            )
            if cand_T is not None and err < best_err:
                best_err = err
                best_T = cand_T
                best_matches = len(matched_leds)
                current_T = cand_T

        is_valid = best_T is not None and best_err <= self.max_reproj_error_px and (best_matches or 0) >= 3

        return PnPSolution(
            position=best_T if best_T is not None else np.zeros(3, dtype=np.float32),
            orientation=quat_cam,
            reprojection_error=best_err,
            num_matches=best_matches or 0,
            is_valid=is_valid,
        )
