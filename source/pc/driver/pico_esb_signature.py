# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Heuristics for the experimental MOCK 64-byte payload codec only.

Not a hardware radio signature, live receiver, CRC checker or validated Swift
telemetry interpreter. 0x71764129 is DTM, not the operational pairing address.
Legacy constants below are retained for compatibility and must not configure RF.
Live captures are handled by radio.swift_partial_crc; no real poses are produced
by this mock interpreter until an independently validated telemetry layout exists.
"""

from __future__ import annotations

import struct
from dataclasses import dataclass
import numpy as np

try:
    from radio.esb_packet import TrackerTelemetryPacket, decode_telemetry_payload
except ImportError:
    from esb_packet import TrackerTelemetryPacket, decode_telemetry_payload


# Known Pico 2.4 GHz Sync / Access Addresses
PICO_PAIRING_ADDRESS = 0x71764129  # Prefix 0x71, Base 0x76412900
PICO_BONDED_PREFIX = 0x71


@dataclass(slots=True)
class InterpretedTrackerState:
    """High-level interpreted tracker state ready for SteamVR tracking."""

    tracker_id: int
    serial_number: str
    sequence_number: int
    timestamp_us: int
    orientation: np.ndarray  # [x, y, z, w] unit quaternion
    acceleration: np.ndarray  # [ax, ay, az] in m/s^2
    angular_velocity: np.ndarray  # [gx, gy, gz] in rad/s
    battery_percentage: int
    is_charging: bool
    is_valid: bool
    signal_quality: float  # 0.0 to 1.0


class PicoESBSignatureInterpreter:
    """Interprets and validates Pico Motion Tracker ESB radio signatures."""

    def __init__(self, tracker_sn_map: dict[int, str] | None = None) -> None:
        # Default SN mapping from bonded devices
        self.tracker_sn_map = tracker_sn_map or {
            1: "OWNER_SUPPLIED_PRIVATE_VALUE_REQUIRED",
            2: "OWNER_SUPPLIED_PRIVATE_VALUE_REQUIRED",
            3: "OWNER_SUPPLIED_PRIVATE_VALUE_REQUIRED",
        }
        self.total_packets_inspected = 0
        self.valid_signatures_detected = 0
        self.crc_or_format_errors = 0

    def match_signature(self, raw_bytes: bytes) -> bool:
        """Heuristic check: does this byte chunk match the Pico Tracker ESB signature?"""
        if len(raw_bytes) != 64:
            return False

        pkt_type = raw_bytes[0]
        if pkt_type not in (0x02, 0x05):
            return False

        flags = raw_bytes[1]
        # bit 0 is quat_valid
        if (flags & 0x01) == 0:
            return False

        # Check reasonable battery level
        batt_byte = raw_bytes[62]
        batt_level = batt_byte & 0x7F
        if batt_level > 100:
            return False

        return True

    def interpret_packet(
        self,
        raw_bytes: bytes,
        tracker_id: int = 1,
    ) -> InterpretedTrackerState | None:
        """Decode and validate a raw ESB packet into an interpreted tracker state."""
        self.total_packets_inspected += 1

        if not self.match_signature(raw_bytes):
            self.crc_or_format_errors += 1
            return None

        # Check raw floats BEFORE the decoder normalizes the quaternion.
        # Otherwise a corrupt quaternion becomes plausible and NaNs bypass comparisons.
        values = np.asarray(struct.unpack_from("<13f", raw_bytes, 8), dtype=np.float64)
        if (not np.all(np.isfinite(values))
                or abs(float(np.linalg.norm(values[:4])) - 1.0) > 0.05):
            self.crc_or_format_errors += 1
            return None

        pkt = decode_telemetry_payload(raw_bytes, tracker_id=tracker_id)
        if pkt is None:
            self.crc_or_format_errors += 1
            return None

        # Verify physical plausibility of quaternion
        q_norm = float(np.linalg.norm(pkt.orientation_quat))
        if abs(q_norm - 1.0) > 0.05:
            self.crc_or_format_errors += 1
            return None

        # Verify physical plausibility of accelerometer (e.g. 1g +- 40 m/s^2 dynamic limit)
        a_norm = float(np.linalg.norm(pkt.acc))
        if a_norm > 150.0:  # Excessive spike / corrupted bits
            self.crc_or_format_errors += 1
            return None

        self.valid_signatures_detected += 1
        sn = self.tracker_sn_map.get(tracker_id, f"PICO_TRACKER_{tracker_id}")

        return InterpretedTrackerState(
            tracker_id=tracker_id,
            serial_number=sn,
            sequence_number=pkt.seq_num,
            timestamp_us=pkt.timestamp_us,
            orientation=pkt.orientation_quat,
            acceleration=pkt.acc,
            angular_velocity=pkt.gyro,
            battery_percentage=pkt.battery_pct,
            is_charging=pkt.is_charging,
            is_valid=True,
            signal_quality=1.0,
        )
