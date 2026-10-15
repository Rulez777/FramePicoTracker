# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Host-side IMU Sensor Scale Conversion and AHRS Orientation Fusion.

Converts raw ADC integer readings (ICM-42688) into metric SI units and estimates
real 3D orientation quaternions [qx, qy, qz, qw] (OpenVR coordinate convention)
using a low-latency 200 Hz Madgwick AHRS filter with static gyro bias calibration.

Physical Scales:
  Accelerometer (ICM-42688 in +/-32g or shifted +/-16g mode):
    1024 LSB = 1.0 g = 9.80665 m/s^2  -->  scale = 9.80665 / 1024.0 m/s^2 per LSB
  Gyroscope (ICM-42688 in +/-2000 dps mode):
    16.384 LSB = 1.0 deg/s             -->  scale = (pi / 180.0) / 16.384 rad/s per LSB
  Magnetometer:
    Centered unsigned 16-bit           -->  centered = raw - 32768
"""

from __future__ import annotations

import math
from dataclasses import dataclass, field
import numpy as np

# Physical conversion scale constants for ICM-42688
ACCEL_LSB_PER_G: float = 1024.0
GRAVITY_MSS: float = 9.80665
ACCEL_SCALE: float = GRAVITY_MSS / ACCEL_LSB_PER_G  # ~0.0095768 m/s^2 per LSB

GYRO_LSB_PER_DPS: float = 16.384
GYRO_SCALE: float = (math.pi / 180.0) / GYRO_LSB_PER_DPS  # ~0.00106526 rad/s per LSB

MAG_CENTER_OFFSET: int = 32768


def raw_to_si_accel(accel_raw: list[int] | tuple[int, int, int] | np.ndarray) -> np.ndarray:
    """Convert raw 16-bit accelerometer counts to m/s^2."""
    return np.asarray(accel_raw, dtype=np.float32) * ACCEL_SCALE


def raw_to_si_gyro(gyro_raw: list[int] | tuple[int, int, int] | np.ndarray) -> np.ndarray:
    """Convert raw 16-bit gyroscope counts to rad/s."""
    return np.asarray(gyro_raw, dtype=np.float32) * GYRO_SCALE


@dataclass
class IMUBiasCalibrator:
    """Online static zero-rate gyro bias estimator."""

    stationary_samples_required: int = 40  # ~0.2 s at 200 Hz
    gyro_stationary_thresh_rads: float = 0.08  # ~4.5 deg/s
    accel_stationary_thresh_mss: float = 1.2  # Deviation from ~9.8 m/s^2

    _sample_buffer: list[np.ndarray] = field(default_factory=list)
    gyro_bias: np.ndarray = field(default_factory=lambda: np.zeros(3, dtype=np.float32))
    is_calibrated: bool = False

    def update(self, gyro_si: np.ndarray, accel_si: np.ndarray) -> np.ndarray:
        """Update calibrator with new sample; returns bias-corrected gyro in rad/s."""
        g_mag = float(np.linalg.norm(gyro_si))
        a_mag = float(np.linalg.norm(accel_si))

        is_still = (g_mag < self.gyro_stationary_thresh_rads and
                    abs(a_mag - GRAVITY_MSS) < self.accel_stationary_thresh_mss)

        if is_still:
            self._sample_buffer.append(gyro_si.astype(np.float32))
            if len(self._sample_buffer) >= self.stationary_samples_required:
                self.gyro_bias = np.mean(self._sample_buffer, axis=0)
                self.is_calibrated = True
                if len(self._sample_buffer) > 200:
                    self._sample_buffer = self._sample_buffer[-100:]
        else:
            if not self.is_calibrated and len(self._sample_buffer) > 0:
                self._sample_buffer.clear()

        return (gyro_si - self.gyro_bias).astype(np.float32)


class MadgwickAHRS:
    """Madgwick AHRS Orientation Filter (6DoF IMU and 9DoF MARG).

    Maintains a normalized orientation quaternion [qx, qy, qz, qw]
    in a local +Z-up reference. OpenVR alignment is not implemented here.
    """

    def __init__(
        self,
        beta: float = 0.1,
        initial_quat: np.ndarray | None = None,
    ) -> None:
        self.beta = beta
        if initial_quat is not None:
            q = np.asarray(initial_quat, dtype=np.float32)
            self.q = q / np.linalg.norm(q)
        else:
            # Identity quaternion: [qx, qy, qz, qw] = [0, 0, 0, 1]
            self.q = np.array([0.0, 0.0, 0.0, 1.0], dtype=np.float32)

    @property
    def quaternion(self) -> np.ndarray:
        """Returns [qx, qy, qz, qw] unit quaternion."""
        return self.q.copy()

    def update_imu(
        self,
        gyro_rads: np.ndarray,
        accel_mss: np.ndarray,
        dt: float = 0.005,
    ) -> np.ndarray:
        """Update orientation from 3-axis gyro (rad/s) and 3-axis accel (m/s^2).

        Args:
            gyro_rads: (3,) angular rates [gx, gy, gz] in rad/s.
            accel_mss: (3,) acceleration [ax, ay, az] in m/s^2.
            dt: Time step in seconds (~0.005 for 200 Hz).

        Returns:
            Updated unit quaternion [qx, qy, qz, qw].
        """
        if not math.isfinite(dt) or not 0 < dt <= 0.05:
            raise ValueError('Missing, invalid or sparse IMU timing; do not synthesize 200 Hz')
        if np.shape(gyro_rads) != (3,) or np.shape(accel_mss) != (3,) or not np.isfinite(gyro_rads).all() or not np.isfinite(accel_mss).all():
            raise ValueError('Expected finite three-axis sensor readings')
        qx, qy, qz, qw = self.q

        # Rate of change of quaternion from gyroscope: 0.5 * q (x) [gx, gy, gz, 0]
        gx, gy, gz = gyro_rads
        qDot1 = 0.5 * (qw * gx + qy * gz - qz * gy)
        qDot2 = 0.5 * (qw * gy - qx * gz + qz * gx)
        qDot3 = 0.5 * (qw * gz + qx * gy - qy * gx)
        qDot4 = 0.5 * (-qx * gx - qy * gy - qz * gz)

        a_norm = float(np.linalg.norm(accel_mss))
        if a_norm > 1e-6:
            ax = accel_mss[0] / a_norm
            ay = accel_mss[1] / a_norm
            az = accel_mss[2] / a_norm

            # In body coordinates, gravity points along +Z when tracker is flat
            # Reference direction of gravity in world: [0, 0, 1]
            # Objective function: f(q) = 2*(qx*qz - qw*qy) - ax ...
            f1 = 2.0 * (qx * qz - qw * qy) - ax
            f2 = 2.0 * (qw * qx + qy * qz) - ay
            f3 = 2.0 * (0.5 - qx * qx - qy * qy) - az

            # Jacobian J^T * f
            j1 = 2.0 * qz * f1 + 2.0 * qw * f2 - 4.0 * qx * f3
            j2 = -2.0 * qw * f1 + 2.0 * qz * f2 - 4.0 * qy * f3
            j3 = 2.0 * qx * f1 + 2.0 * qy * f2
            j4 = -2.0 * qy * f1 + 2.0 * qx * f2

            j_norm = math.sqrt(j1 * j1 + j2 * j2 + j3 * j3 + j4 * j4)
            if j_norm > 1e-6:
                # Apply feedback step
                qDot1 -= self.beta * (j1 / j_norm)
                qDot2 -= self.beta * (j2 / j_norm)
                qDot3 -= self.beta * (j3 / j_norm)
                qDot4 -= self.beta * (j4 / j_norm)

        # Integrate rate of change
        self.q[0] += qDot1 * dt
        self.q[1] += qDot2 * dt
        self.q[2] += qDot3 * dt
        self.q[3] += qDot4 * dt

        # Normalize quaternion
        norm = float(np.linalg.norm(self.q))
        if norm > 1e-6:
            self.q /= norm

        return self.q.copy()


class PicoTrackerOrientationFilter:
    """Manages online orientation estimation for all 3 Pico Motion Tracker slots."""

    def __init__(self, beta: float = 0.08) -> None:
        self.filters: dict[int, MadgwickAHRS] = {
            slot: MadgwickAHRS(beta=beta) for slot in (0, 1, 2)
        }
        self.calibrators: dict[int, IMUBiasCalibrator] = {
            slot: IMUBiasCalibrator() for slot in (0, 1, 2)
        }

    def process_sensor_dict(
        self,
        slot_index: int,
        sensors: dict,
        dt: float | None = None,
    ) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
        """Process decoded sensor block dictionary.

        Args:
            slot_index: Slot 0, 1, or 2.
            sensors: Dict returned by `decode_sensor_payload`.
            dt: Sampling period in seconds (default 5 ms = 200 Hz).

        Returns:
            (quat, accel_si, gyro_si):
              quat: (4,) unit quaternion [qx, qy, qz, qw]
              accel_si: (3,) acceleration in m/s^2
              gyro_si: (3,) bias-corrected angular rates in rad/s
        """
        if slot_index not in self.filters:
            raise ValueError('Unknown tracker profile')
        if dt is None or not math.isfinite(dt) or not 0 < dt <= 0.05:
            raise ValueError('An independently established dense sample interval is required')
        if 'gyro_raw' not in sensors or 'accel_raw' not in sensors:
            raise ValueError('This packet does not contain an IMU sample')
        gyro_raw = sensors['gyro_raw']
        accel_raw = sensors['accel_raw']

        accel_si = raw_to_si_accel(accel_raw)
        gyro_si_raw = raw_to_si_gyro(gyro_raw)

        # Apply static bias calibration
        gyro_si = self.calibrators[slot_index].update(gyro_si_raw, accel_si)

        # Update AHRS filter
        quat = self.filters[slot_index].update_imu(gyro_si, accel_si, dt=dt)

        return quat, accel_si, gyro_si
