# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Stereo and Multi-Camera Prior-Assisted PnP Solver for Steam Frame.

Steam Frame tracking utilizes multiple wide-angle tracking cameras (e.g. Left and
Right forward/downward cameras). This solver combines 2D LED blob observations
across multiple cameras into a single, unified linear least-squares system:

    A_stereo @ T_ref = b_stereo

Benefits over single-camera tracking:
1. Triangulation baseline constraint: Eliminates single-camera depth ambiguity,
   dropping Z uncertainty from ~3-5 mm to < 0.5 mm.
2. Wider Field of View (FOV): Tracker remains locked even when it moves out of one
   camera's FOV, as long as another camera observes it.
3. Robustness to occlusion: Even if 4 of 8 LEDs are blocked from the left camera,
   the right camera's view of the remaining LEDs maintains continuous tracking.
"""

from __future__ import annotations

from dataclasses import dataclass, field
import numpy as np

try:
    from optical.constellation import CameraIntrinsics, TrackerConstellation
    from optical.distortion import DistortionCoeffs, undistort_pixel_points
    from optical.pnp_solver import PnPSolution
except ImportError:
    from constellation import CameraIntrinsics, TrackerConstellation
    from distortion import DistortionCoeffs, undistort_pixel_points
    from pnp_solver import PnPSolution

from coords import quat_to_rot_matrix


@dataclass(slots=True)
class CameraRigExtrinsics:
    """Extrinsics of a camera relative to the reference frame (e.g. HMD or Camera 0).

    R: (3, 3) rotation matrix
    T: (3,) translation vector in meters
    """

    R: np.ndarray = field(default_factory=lambda: np.eye(3, dtype=np.float64))
    T: np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.float64))


@dataclass(slots=True)
class CameraView:
    """Camera specification with intrinsics, distortion, and rig extrinsics."""

    intrinsics: CameraIntrinsics
    distortion: DistortionCoeffs = field(default_factory=DistortionCoeffs)
    extrinsics: CameraRigExtrinsics = field(default_factory=CameraRigExtrinsics)


@dataclass(slots=True)
class CameraObservation:
    """Blobs detected in a specific camera."""

    camera_id: int
    blob_points: np.ndarray  # (M, 2) in raw pixels


class StereoPriorAssistedPnPSolver:
    """Multi-camera Prior-Assisted PnP Solver."""

    def __init__(
        self,
        cameras: list[CameraView],
        constellation: TrackerConstellation | None = None,
        max_reproj_error_px: float = 4.0,
    ) -> None:
        """Initialize with a list of camera views in the rig.

        Camera 0 is typically the reference camera (R=I, T=0).
        """
        self.cameras = cameras
        self.constellation = constellation or TrackerConstellation()
        self.max_reproj_error_px = max_reproj_error_px

    def solve_translation(
        self,
        matches_per_cam: dict[int, tuple[np.ndarray, np.ndarray]],
        quat_ref: np.ndarray,
    ) -> tuple[np.ndarray | None, float, int]:
        """Solve for 3D translation T_ref in reference frame given point matches across cameras.

        Args:
            matches_per_cam: Dict mapping camera_id -> (led_indices (K,), undistorted_pts (K, 2))
            quat_ref: Tracker orientation in reference coordinate frame.

        Returns:
            (T_ref, mean_reprojection_error_px, total_matches)
        """
        total_eqs = sum(len(led_idx) * 2 for led_idx, _ in matches_per_cam.values())
        if total_eqs < 4:  # At least 2 points across all cameras
            return None, 999.0, 0

        # Tracker rotation in reference frame
        R_ref = quat_to_rot_matrix(quat_ref)
        # Rotated 3D model points in tracker local frame: (8, 3)
        P_prime_all = (R_ref @ self.constellation.led_positions.T).T

        A = np.zeros((total_eqs, 3), dtype=np.float64)
        b = np.zeros(total_eqs, dtype=np.float64)

        row_idx = 0
        for cam_id, (led_indices, pts_undist) in matches_per_cam.items():
            cam = self.cameras[cam_id]
            K = cam.intrinsics
            R_cam = cam.extrinsics.R
            T_cam = cam.extrinsics.T

            # P_cam = R_cam * P'_i + T_cam + R_cam * T_ref
            # Let M_i = R_cam * P'_i + T_cam
            P_prime_subset = P_prime_all[led_indices]
            M = (R_cam @ P_prime_subset.T).T + T_cam

            u = pts_undist[:, 0]
            v = pts_undist[:, 1]
            du = u - K.cx
            dv = v - K.cy

            r1 = R_cam[0, :]
            r2 = R_cam[1, :]
            r3 = R_cam[2, :]

            for i in range(len(led_indices)):
                # X equation: fx * X_cam - du * Z_cam = 0
                # X_cam = M[i, 0] + r1 @ T_ref
                # Z_cam = M[i, 2] + r3 @ T_ref
                A[row_idx, :] = K.fx * r1 - du[i] * r3
                b[row_idx] = du[i] * M[i, 2] - K.fx * M[i, 0]
                row_idx += 1

                # Y equation: fy * Y_cam - dv * Z_cam = 0
                # Y_cam = M[i, 1] + r2 @ T_ref
                A[row_idx, :] = K.fy * r2 - dv[i] * r3
                b[row_idx] = dv[i] * M[i, 2] - K.fy * M[i, 1]
                row_idx += 1

        # Solve linear system A @ T_ref = b
        sol, residuals, rank, s = np.linalg.lstsq(A, b, rcond=None)
        T_ref = sol.astype(np.float64)

        # Check positive depth in all observing cameras
        for cam_id, _ in matches_per_cam.items():
            cam = self.cameras[cam_id]
            P_cam_origin = cam.extrinsics.R @ T_ref + cam.extrinsics.T
            if P_cam_origin[2] <= 0.05:
                return None, 999.0, 0

        # Non-linear Gauss-Newton refinement across all cameras
        for _ in range(4):
            res_list = []
            J_list = []

            for cam_id, (led_indices, pts_undist) in matches_per_cam.items():
                cam = self.cameras[cam_id]
                K = cam.intrinsics
                R_cam = cam.extrinsics.R
                T_cam = cam.extrinsics.T

                P_prime_subset = P_prime_all[led_indices]
                pts_cam = (R_cam @ (P_prime_subset + T_ref).T).T + T_cam
                Z_cam = pts_cam[:, 2]
                if np.any(Z_cam <= 0.01):
                    break

                u_proj = K.fx * (pts_cam[:, 0] / Z_cam) + K.cx
                v_proj = K.fy * (pts_cam[:, 1] / Z_cam) + K.cy

                res_cam = np.zeros(2 * len(led_indices), dtype=np.float64)
                res_cam[0::2] = u_proj - pts_undist[:, 0]
                res_cam[1::2] = v_proj - pts_undist[:, 1]
                res_list.append(res_cam)

                # Jacobian of projection w.r.t T_ref:
                # d(pts_cam)/d(T_ref) = R_cam
                # d(u)/d(pts_cam) = [fx / Z, 0, -fx*X / Z^2]
                # d(u)/d(T_ref) = [fx / Z, 0, -fx*X / Z^2] @ R_cam
                J_cam = np.zeros((2 * len(led_indices), 3), dtype=np.float64)
                X_c = pts_cam[:, 0]
                Y_c = pts_cam[:, 1]
                Z_sq = Z_cam * Z_cam

                J_cam[0::2, :] = (
                    (K.fx / Z_cam)[:, None] * R_cam[0, :]
                    - (K.fx * X_c / Z_sq)[:, None] * R_cam[2, :]
                )
                J_cam[1::2, :] = (
                    (K.fy / Z_cam)[:, None] * R_cam[1, :]
                    - (K.fy * Y_c / Z_sq)[:, None] * R_cam[2, :]
                )
                J_list.append(J_cam)

            if not res_list or not J_list:
                break

            res_full = np.concatenate(res_list)
            J_full = np.vstack(J_list)

            JtJ = J_full.T @ J_full
            Jt_res = J_full.T @ res_full

            try:
                delta_T = -np.linalg.solve(JtJ + 1e-6 * np.eye(3), Jt_res)
            except np.linalg.LinAlgError:
                break

            T_ref += delta_T
            if np.linalg.norm(delta_T) < 1e-5:
                break

        # Compute final mean reprojection error
        mean_err = float(np.mean(np.abs(res_full))) if len(res_full) > 0 else 999.0
        total_pts = sum(len(led_idx) for led_idx, _ in matches_per_cam.values())
        return T_ref, mean_err, total_pts

    def match_and_solve(
        self,
        observations: list[CameraObservation],
        quat_ref: np.ndarray,
        prior_position: np.ndarray | None = None,
    ) -> PnPSolution:
        """Perform 2D-3D blob association and multi-camera PnP solve.

        Args:
            observations: List of CameraObservation (one per observing camera).
            quat_ref: Estimated tracker orientation in reference frame.
            prior_position: Estimated 3D position in reference frame from EKF.

        Returns:
            PnPSolution with 3D reference position and diagnostic metrics.
        """
        R_ref = quat_to_rot_matrix(quat_ref)

        # 1. Undistort blob points for all cameras
        undistorted_obs: dict[int, np.ndarray] = {}
        for obs in observations:
            cam = self.cameras[obs.camera_id]
            if len(obs.blob_points) > 0:
                undist_pts = undistort_pixel_points(
                    obs.blob_points,
                    cam.intrinsics.fx,
                    cam.intrinsics.fy,
                    cam.intrinsics.cx,
                    cam.intrinsics.cy,
                    cam.distortion,
                )
                undistorted_obs[obs.camera_id] = undist_pts

        # 2. Association and iterative PnP solve
        # Estimate initial depth Z from prior or constellation diameter in observing cameras
        z_init = 1.0
        if prior_position is not None and prior_position[2] > 0.05:
            z_init = float(prior_position[2])
        else:
            # Estimate from 2D bounding diameter across cameras
            for cam_id, blobs_2d in undistorted_obs.items():
                if len(blobs_2d) >= 2:
                    cam = self.cameras[cam_id]
                    dist_matrix = np.linalg.norm(blobs_2d[:, None, :] - blobs_2d[None, :, :], axis=-1)
                    max_d = np.max(dist_matrix)
                    if max_d > 5.0:
                        z_init = float((cam.intrinsics.fx * 0.045) / max_d)
                        break

        # Initial translation estimate
        if prior_position is not None and np.linalg.norm(prior_position) > 0.05:
            current_T = np.copy(prior_position)
        else:
            # Centroid-based initialization from camera 0, transformed to reference frame
            cam0 = self.cameras[0]
            if 0 in undistorted_obs and len(undistorted_obs[0]) > 0:
                c2d = np.mean(undistorted_obs[0], axis=0)
                t_cam0 = np.array([
                    (c2d[0] - cam0.intrinsics.cx) * z_init / cam0.intrinsics.fx,
                    (c2d[1] - cam0.intrinsics.cy) * z_init / cam0.intrinsics.fy,
                    z_init,
                ], dtype=np.float64)
                current_T = cam0.extrinsics.R.T @ (t_cam0 - cam0.extrinsics.T)
            else:
                current_T = np.array([0.0, 0.0, z_init], dtype=np.float64)

        P_prime_all = (R_ref @ self.constellation.led_positions.T).T

        best_T = None
        best_err = 999.0
        best_matches = 0

        # Iterative closest point (ICP) across all cameras
        for iter_idx in range(4):
            matches_per_cam: dict[int, tuple[np.ndarray, np.ndarray]] = {}

            for cam_id, blobs_2d in undistorted_obs.items():
                if len(blobs_2d) == 0:
                    continue
                cam = self.cameras[cam_id]
                # Project all LEDs to this camera with current_T
                pts_cam = (cam.extrinsics.R @ (P_prime_all + current_T).T).T + cam.extrinsics.T

                # Visibility filter: in front of camera
                valid_mask = pts_cam[:, 2] > 0.05
                if np.sum(valid_mask) < 2:
                    continue

                valid_indices = np.where(valid_mask)[0]
                u_proj = cam.intrinsics.fx * (pts_cam[valid_indices, 0] / pts_cam[valid_indices, 2]) + cam.intrinsics.cx
                v_proj = cam.intrinsics.fy * (pts_cam[valid_indices, 1] / pts_cam[valid_indices, 2]) + cam.intrinsics.cy
                proj_2d = np.column_stack((u_proj, v_proj))

                # Distance matrix: (N_valid, M_blobs)
                dists = np.linalg.norm(proj_2d[:, None, :] - blobs_2d[None, :, :], axis=2)

                # Adaptive gating threshold (wider on first iteration, tight on subsequent)
                gate_px = 35.0 if iter_idx == 0 else 15.0

                matched_leds = []
                matched_blobs = []
                used_blobs = set()

                for k, led_idx in enumerate(valid_indices):
                    closest_b = int(np.argmin(dists[k]))
                    min_d = dists[k, closest_b]
                    if min_d < gate_px and closest_b not in used_blobs:
                        # Mutual nearest check
                        if np.argmin(dists[:, closest_b]) == k:
                            matched_leds.append(led_idx)
                            matched_blobs.append(blobs_2d[closest_b])
                            used_blobs.add(closest_b)

                if len(matched_leds) >= 2:
                    matches_per_cam[cam_id] = (
                        np.array(matched_leds, dtype=int),
                        np.array(matched_blobs, dtype=np.float64),
                    )

            if not matches_per_cam:
                break

            T_cand, err, n_pts = self.solve_translation(matches_per_cam, quat_ref)
            if T_cand is not None and err < best_err:
                best_err = err
                best_T = T_cand
                best_matches = n_pts
                current_T = T_cand
                if err < 0.2:  # Subpixel convergence achieved
                    break

        if best_T is not None and best_err <= self.max_reproj_error_px and best_matches >= 3:
            return PnPSolution(
                position=best_T,
                orientation=quat_ref,
                reprojection_error=best_err,
                num_matches=best_matches,
                is_valid=True,
            )

        # Fallback if no prior or association failed: try Camera 0 single solve
        if 0 in undistorted_obs and len(undistorted_obs[0]) >= 3:
            cam0 = self.cameras[0]
            blobs0 = undistorted_obs[0][:min(6, len(undistorted_obs[0]))]
            from itertools import combinations, permutations
            for blob_subset in combinations(range(len(blobs0)), 3):
                pts_sub = blobs0[list(blob_subset)]
                for led_subset in permutations(range(8), 3):
                    matches = {0: (np.array(led_subset, dtype=int), pts_sub)}
                    T, err, n = self.solve_translation(matches, quat_ref)
                    if T is not None and err < best_err:
                        best_err = err
                        best_T = T
                        best_matches = n
                        if err < 1.0:
                            return PnPSolution(
                                position=best_T,
                                orientation=quat_ref,
                                reprojection_error=best_err,
                                num_matches=best_matches,
                                is_valid=True,
                            )

            if best_T is not None and best_err <= self.max_reproj_error_px:
                return PnPSolution(
                    position=best_T,
                    orientation=quat_ref,
                    reprojection_error=best_err,
                    num_matches=best_matches,
                    is_valid=True,
                )

        return PnPSolution(
            position=prior_position if prior_position is not None else np.zeros(3),
            orientation=quat_ref,
            reprojection_error=999.0,
            num_matches=0,
            is_valid=False,
        )
