# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Experimental 64-byte MOCK telemetry codec, retained for bridge tests.

This layout has not been observed on the live Swift radio. Do not pass captured
payloads to it or use its enums as station commands. Live shortened captures
currently have lengths10/22/28 and an 8-bit length + 4-bit control header.
Operational firmware evidence uses CRC16/0x1021 including the address; the old
0x71764129/CRC24 pairing description belonged to DTM and was incorrect.
Use pico_swift_probe and swift_partial_crc for offline/live-capture analysis.
"""

from __future__ import annotations

import struct
from dataclasses import dataclass
import numpy as np


# Packet Type IDs in S0 header byte
PKT_TYPE_PAIR_REQ = 0x01
PKT_TYPE_TELEMETRY = 0x02
PKT_TYPE_CMD_ACK = 0x03
PKT_TYPE_IMU_RAW = 0x04
PKT_TYPE_TELEMETRY_EXT = 0x05

# Command IDs sent from Station to Tracker
CMD_SET_WORKMODE = 0x10
CMD_SET_IRLED_MODE = 0x20
CMD_SET_RADIO_INTERVAL = 0x30
CMD_REQUEST_VERSION = 0x40
CMD_UNBOND = 0xF0


@dataclass(slots=True)
class TrackerTelemetryPacket:
    """Decoded 200 Hz telemetry packet received from Pico Motion Tracker."""

    tracker_id: int
    seq_num: int
    timestamp_us: int
    orientation_quat: np.ndarray  # [x, y, z, w]
    acc: np.ndarray  # [ax, ay, az] in m/s^2 (or g)
    gyro: np.ndarray  # [gx, gy, gz] in rad/s
    mag: np.ndarray  # [mx, my, mz]
    temperature_c: float
    battery_pct: int
    is_charging: bool
    irled_status: int
    quat_valid: bool


def decode_telemetry_payload(data: bytes, tracker_id: int = 1) -> TrackerTelemetryPacket | None:
    """Decode raw radio payload bytes into TrackerTelemetryPacket.

    Payload format (little-endian):
      Offset  Size  Type      Field
      0       1     u8        Packet Type (0x02 or 0x05)
      1       1     u8        Flags (bit 0: quat_valid, bits 1-7: session index)
      2       2     u16       Sequence Number (frame_cnt)
      4       4     u32       Timestamp (microseconds)
      8       16    4xf32     Quaternion: w, x, y, z (on-board Keil RTX fusion)
      24      12    3xf32     Accelerometer: ax, ay, az
      36      12    3xf32     Gyroscope: gx, gy, gz
      48      12    3xf32     Magnetometer: mx, my, mz
      60      2     i16       Raw Temperature (T = raw / 132.48 + 25.0 C)
      62      1     u8        Battery level (0-100) & charging status (bit 7)
      63      1     u8        IR LED mode / strobe status
      Total: 64 bytes
    """
    if len(data) < 64:
        return None

    (
        pkt_type,
        flags,
        seq_num,
        timestamp_us,
        qw,
        qx,
        qy,
        qz,
        ax,
        ay,
        az,
        gx,
        gy,
        gz,
        mx,
        my,
        mz,
        raw_temp,
        batt_byte,
        irled_status,
    ) = struct.unpack_from("<BBH I 4f 3f 3f 3f hBB", data, 0)

    quat_valid = bool(flags & 0x01)
    # Convert quat from w, x, y, z to standard [qx, qy, qz, qw]
    quat = np.array([qx, qy, qz, qw], dtype=np.float32)
    norm = np.linalg.norm(quat)
    if norm > 1e-6:
        quat /= norm
    else:
        quat = np.array([0.0, 0.0, 0.0, 1.0], dtype=np.float32)

    acc = np.array([ax, ay, az], dtype=np.float32)
    gyro = np.array([gx, gy, gz], dtype=np.float32)
    mag = np.array([mx, my, mz], dtype=np.float32)

    # Formula from libpvrtrackingservice.so: raw_temp / 132.48 + 25.0
    temp_c = float(raw_temp) / 132.48 + 25.0

    battery_pct = batt_byte & 0x7F
    is_charging = bool(batt_byte & 0x80)

    return TrackerTelemetryPacket(
        tracker_id=tracker_id,
        seq_num=seq_num,
        timestamp_us=timestamp_us,
        orientation_quat=quat,
        acc=acc,
        gyro=gyro,
        mag=mag,
        temperature_c=temp_c,
        battery_pct=battery_pct,
        is_charging=is_charging,
        irled_status=irled_status,
        quat_valid=quat_valid,
    )


def encode_telemetry_payload(packet: TrackerTelemetryPacket) -> bytes:
    """Encode TrackerTelemetryPacket into 64-byte binary radio payload."""
    flags = 0x01 if packet.quat_valid else 0x00
    batt_byte = (packet.battery_pct & 0x7F) | (0x80 if packet.is_charging else 0x00)
    raw_temp = int(round((packet.temperature_c - 25.0) * 132.48))

    qx, qy, qz, qw = packet.orientation_quat

    payload = struct.pack(
        "<BBH I 4f 3f 3f 3f hBB",
        PKT_TYPE_TELEMETRY,
        flags,
        packet.seq_num & 0xFFFF,
        packet.timestamp_us & 0xFFFFFFFF,
        qw,
        qx,
        qy,
        qz,
        packet.acc[0],
        packet.acc[1],
        packet.acc[2],
        packet.gyro[0],
        packet.gyro[1],
        packet.gyro[2],
        packet.mag[0],
        packet.mag[1],
        packet.mag[2],
        raw_temp,
        batt_byte,
        packet.irled_status,
    )
    return payload


def encode_command_packet(cmd_id: int, param: int = 0) -> bytes:
    """Encode command from Station to Tracker (e.g. set_irled_onoff_mode 0x01)."""
    return struct.pack("<BBH", PKT_TYPE_CMD_ACK, cmd_id, param)
