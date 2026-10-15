# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Extended Kalman Filter (EKF) for 6DoF Optical-Inertial Fusion.

Fuses high-rate (200 Hz) IMU acceleration with lower-rate (60-90 Hz)
optical camera PnP position measurements.
Handles occlusion gracefully by predicting on inertial integration.
"""

from __future__ import annotations

import numpy as np
try:
    from ..coords import quat_to_rot_matrix
except ImportError:
    from coords import quat_to_rot_matrix


class OpticalInertialEKF:
    """9-state Extended Kalman Filter: [Position(3), Velocity(3), AccelBias(3)]."""

    def __init__(
        self,
        initial_pos: np.ndarray | None = None,
        acc_noise: float = 0.05,
        bias_noise: float = 0.001,
        cam_pos_noise: float = 0.003,  # ~3mm optical accuracy
        gravity: np.ndarray | None = None,
    ) -> None:
        # State vector: [px, py, pz, vx, vy, vz, bx, by, bz]
        self.x = np.zeros(9, dtype=np.float64)
        if initial_pos is not None:
            self.x[:3] = np.asarray(initial_pos, dtype=np.float64)

        # Covariance matrix P (9x9)
        self.P = np.eye(9, dtype=np.float64) * 0.1
        self.P[6:9, 6:9] = np.eye(3) * 0.01  # Lower initial uncertainty on bias

        # Process noise parameters
        self.acc_noise = acc_noise
        self.bias_noise = bias_noise
        self.cam_pos_noise = cam_pos_noise

        # Gravity vector in world frame (default OpenVR: +Y is Up, gravity is -9.81 on Y)
        self.gravity = (
            np.asarray(gravity, dtype=np.float64)
            if gravity is not None
            else np.array([0.0, -9.80665, 0.0], dtype=np.float64)
        )

        self.last_predict_t: float | None = None

    @property
    def position(self) -> np.ndarray:
        return self.x[:3].astype(np.float32)

    @property
    def velocity(self) -> np.ndarray:
        return self.x[3:6].astype(np.float32)

    @property
    def accel_bias(self) -> np.ndarray:
        return self.x[6:9].astype(np.float32)

    def predict(self, dt: float, raw_acc_body: np.ndarray, quat: np.ndarray) -> np.ndarray:
        """Propagate state forward using accelerometer and orientation.

        Args:
            dt: Time step in seconds.
            raw_acc_body: (3,) accelerometer reading in body frame (m/s^2).
            quat: (4,) orientation quaternion [qx, qy, qz, qw].

        Returns:
            Updated 3D position.
        """
        if not np.isfinite(dt) or dt <= 0.0 or dt > 0.5:
            # Missing samples cannot be reconstructed by inventing a200Hz step.
            # Caller must invalidate/reset prediction or obtain a new anchor.
            raise ValueError('IMU interval must be finite and within (0, 0.5] seconds')

        R = quat_to_rot_matrix(quat).astype(np.float64)
        acc_bias = self.x[6:9]
        acc_corrected = np.asarray(raw_acc_body, dtype=np.float64) - acc_bias

        # Transform acceleration to world frame and subtract gravity
        # In free fall, sensor reads 0 -> a_world = 0 + gravity = gravity (downward)
        # Sitting on table, sensor reads +1g upward -> a_world = +1g - 1g = 0
        a_world = R @ acc_corrected + self.gravity

        # State transition: x_{k+1} = F @ x_k + B @ a_world
        p = self.x[:3]
        v = self.x[3:6]

        p_new = p + v * dt + 0.5 * a_world * (dt * dt)
        v_new = v + a_world * dt

        self.x[:3] = p_new
        self.x[3:6] = v_new

        # State transition Jacobian F (9x9)
        F = np.eye(9, dtype=np.float64)
        F[0:3, 3:6] = np.eye(3) * dt
        F[0:3, 6:9] = -0.5 * (dt * dt) * R
        F[3:6, 6:9] = -dt * R

        # Process noise covariance Q (9x9)
        Q = np.zeros((9, 9), dtype=np.float64)
        q_pos = (self.acc_noise**2) * (dt**3) / 3.0
        q_vel = (self.acc_noise**2) * dt
        q_bias = (self.bias_noise**2) * dt

        Q[0:3, 0:3] = np.eye(3) * q_pos
        Q[3:6, 3:6] = np.eye(3) * q_vel
        Q[6:9, 6:9] = np.eye(3) * q_bias

        self.P = F @ self.P @ F.T + Q
        return self.position

    def update_optical(self, optical_pos: np.ndarray, optical_noise: float | None = None) -> None:
        """Correct state with 3D optical position measurement from camera PnP.

        Args:
            optical_pos: (3,) observed 3D position in world/camera frame.
            optical_noise: Optional standard deviation of optical measurement in meters.
        """
        z = np.asarray(optical_pos, dtype=np.float64)

        # Measurement matrix H: z = H @ x = position
        H = np.zeros((3, 9), dtype=np.float64)
        H[0:3, 0:3] = np.eye(3)

        # Measurement noise covariance R (3x3)
        sigma = optical_noise if optical_noise is not None else self.cam_pos_noise
        R_cov = np.eye(3, dtype=np.float64) * (sigma**2)

        # Innovation residual y = z - H @ x
        y = z - self.x[:3]

        # Innovation covariance S = H @ P @ H.T + R
        S = H @ self.P @ H.T + R_cov

        # Kalman gain K = P @ H.T @ inv(S)
        K = self.P @ H.T @ np.linalg.inv(S)

        # State and covariance update
        self.x = self.x + K @ y
        I_KH = np.eye(9, dtype=np.float64) - K @ H
        self.P = I_KH @ self.P @ I_KH.T + K @ R_cov @ K.T
