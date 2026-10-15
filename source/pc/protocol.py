# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Wire protocol v1 implementation for Pico Motion Tracker bridge.

Specification per AGENTS.md:
Header (12 bytes, little-endian):
  magic:   u32 = 0x31425450 ('PTB1')
  version: u16 = 1
  type:    u16 = 1 (Frame) | 2 (Hello)
  length:  u32 = payload length in bytes

Hello payload (type 2):
  UTF-8 JSON string.

Frame payload (type 1):
  t:       f64 (headset monotonic seconds from Stopwatch)
  frame:   u32 (sequence counter)
  hmd:     7×f32 (px py pz qx qy qz qw)
  count:   u8 (number of trackers)
  per tracker (65 bytes):
    id:      i64 (SDK tracker ID)
    valid:   u8 (1 if valid, 0 otherwise)
    pose:    7×f32 (px py pz qx qy qz qw)
    linVel:  3×f32 (vx vy vz)
    angVel:  3×f32 (wx wy wz)
    battery: f32 (0..1, -1 if unknown)
"""

from __future__ import annotations

import json
import struct
from dataclasses import dataclass, field
import numpy as np

MAGIC = 0x31425450  # 'PTB1' in little-endian (<I)
VERSION = 1
TYPE_FRAME = 1
TYPE_HELLO = 2

HEADER_FORMAT = "<IHHI"
HEADER_SIZE = struct.calcsize(HEADER_FORMAT)  # 12 bytes

FRAME_FIXED_FORMAT = "<dI7fB"
FRAME_FIXED_SIZE = struct.calcsize(FRAME_FIXED_FORMAT)  # 8 + 4 + 28 + 1 = 41 bytes

TRACKER_RECORD_FORMAT = "<qB7f3f3ff"
TRACKER_RECORD_SIZE = struct.calcsize(TRACKER_RECORD_FORMAT)  # 8 + 1 + 28 + 12 + 12 + 4 = 65 bytes


@dataclass(slots=True)
class Header:
    magic: int
    version: int
    msg_type: int
    length: int

    def validate(self) -> None:
        if self.magic != MAGIC:
            raise ValueError(f"Invalid magic: 0x{self.magic:08X} (expected 0x{MAGIC:08X})")
        if self.version != VERSION:
            raise ValueError(f"Unsupported protocol version: {self.version} (expected {VERSION})")
        if self.msg_type not in (TYPE_FRAME, TYPE_HELLO):
            raise ValueError(f"Unknown message type: {self.msg_type}")


@dataclass(slots=True)
class HelloPayload:
    device: str = ""
    sdk: str = ""
    app: str = ""
    extra: dict = field(default_factory=dict)

    def to_dict(self) -> dict:
        d = {"device": self.device, "sdk": self.sdk, "app": self.app}
        d.update(self.extra)
        return d


@dataclass(slots=True)
class TrackerData:
    tracker_id: int
    valid: bool
    pos: np.ndarray  # shape (3,), float32 (px, py, pz)
    rot: np.ndarray  # shape (4,), float32 (qx, qy, qz, qw)
    lin_vel: np.ndarray  # shape (3,), float32
    ang_vel: np.ndarray  # shape (3,), float32
    battery: float  # 0..1, -1 if unknown


@dataclass(slots=True)
class FramePayload:
    t: float
    frame: int
    hmd_pos: np.ndarray  # shape (3,), float32
    hmd_rot: np.ndarray  # shape (4,), float32
    trackers: list[TrackerData] = field(default_factory=list)


def pack_header(msg_type: int, length: int) -> bytes:
    return struct.pack(HEADER_FORMAT, MAGIC, VERSION, msg_type, length)


def unpack_header(data: bytes) -> Header:
    if len(data) < HEADER_SIZE:
        raise ValueError(f"Buffer too short for header: {len(data)} < {HEADER_SIZE}")
    magic, version, msg_type, length = struct.unpack(HEADER_FORMAT, data[:HEADER_SIZE])
    header = Header(magic=magic, version=version, msg_type=msg_type, length=length)
    header.validate()
    return header


def pack_hello(payload: HelloPayload) -> bytes:
    encoded = json.dumps(payload.to_dict()).encode("utf-8")
    header = pack_header(TYPE_HELLO, len(encoded))
    return header + encoded


def unpack_hello(payload_bytes: bytes) -> HelloPayload:
    data = json.loads(payload_bytes.decode("utf-8"))
    device = data.pop("device", "")
    sdk = data.pop("sdk", "")
    app = data.pop("app", "")
    return HelloPayload(device=device, sdk=sdk, app=app, extra=data)


def pack_frame(frame: FramePayload) -> bytes:
    tracker_count = len(frame.trackers)
    if tracker_count > 255:
        raise ValueError("Cannot pack more than 255 trackers in one frame")

    payload_size = FRAME_FIXED_SIZE + tracker_count * TRACKER_RECORD_SIZE
    header = pack_header(TYPE_FRAME, payload_size)

    hmd_px, hmd_py, hmd_pz = frame.hmd_pos[:3]
    hmd_qx, hmd_qy, hmd_qz, hmd_qw = frame.hmd_rot[:4]

    buf = bytearray(header)
    buf.extend(
        struct.pack(
            FRAME_FIXED_FORMAT,
            frame.t,
            frame.frame,
            hmd_px,
            hmd_py,
            hmd_pz,
            hmd_qx,
            hmd_qy,
            hmd_qz,
            hmd_qw,
            tracker_count,
        )
    )

    for tr in frame.trackers:
        px, py, pz = tr.pos[:3]
        qx, qy, qz, qw = tr.rot[:4]
        vx, vy, vz = tr.lin_vel[:3]
        wx, wy, wz = tr.ang_vel[:3]
        buf.extend(
            struct.pack(
                TRACKER_RECORD_FORMAT,
                tr.tracker_id,
                1 if tr.valid else 0,
                px,
                py,
                pz,
                qx,
                qy,
                qz,
                qw,
                vx,
                vy,
                vz,
                wx,
                wy,
                wz,
                tr.battery,
            )
        )

    return bytes(buf)


def unpack_frame(payload_bytes: bytes) -> FramePayload:
    if len(payload_bytes) < FRAME_FIXED_SIZE:
        raise ValueError(f"Payload too short for frame: {len(payload_bytes)} < {FRAME_FIXED_SIZE}")

    (
        t,
        frame_idx,
        hmd_px,
        hmd_py,
        hmd_pz,
        hmd_qx,
        hmd_qy,
        hmd_qz,
        hmd_qw,
        tracker_count,
    ) = struct.unpack_from(FRAME_FIXED_FORMAT, payload_bytes, 0)

    expected_len = FRAME_FIXED_SIZE + tracker_count * TRACKER_RECORD_SIZE
    if len(payload_bytes) != expected_len:
        raise ValueError(
            f"Payload size mismatch: expected {expected_len} for {tracker_count} trackers, got {len(payload_bytes)}"
        )

    hmd_pos = np.array([hmd_px, hmd_py, hmd_pz], dtype=np.float32)
    hmd_rot = np.array([hmd_qx, hmd_qy, hmd_qz, hmd_qw], dtype=np.float32)

    offset = FRAME_FIXED_SIZE
    trackers: list[TrackerData] = []
    for _ in range(tracker_count):
        (
            tid,
            valid_u8,
            px,
            py,
            pz,
            qx,
            qy,
            qz,
            qw,
            vx,
            vy,
            vz,
            wx,
            wy,
            wz,
            battery,
        ) = struct.unpack_from(TRACKER_RECORD_FORMAT, payload_bytes, offset)
        offset += TRACKER_RECORD_SIZE

        trackers.append(
            TrackerData(
                tracker_id=tid,
                valid=bool(valid_u8),
                pos=np.array([px, py, pz], dtype=np.float32),
                rot=np.array([qx, qy, qz, qw], dtype=np.float32),
                lin_vel=np.array([vx, vy, vz], dtype=np.float32),
                ang_vel=np.array([wx, wy, wz], dtype=np.float32),
                battery=battery,
            )
        )

    return FramePayload(
        t=t,
        frame=frame_idx,
        hmd_pos=hmd_pos,
        hmd_rot=hmd_rot,
        trackers=trackers,
    )
