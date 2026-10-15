# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Versioned loopback IPC for measured OpenVR world poses (PTP2).

Time is Linux CLOCK_MONOTONIC on Frame, never a remote PC clock. The producer
must convert calibrated optical measurements to OpenVR standing space first.
"""
from dataclasses import dataclass
import math
import socket
import struct
import time

WIRE = struct.Struct('<4sIIQQ15f')
MAX_AGE_NS = 250_000_000


@dataclass(frozen=True)
class PoseFrame:
    chip_id: int
    sequence: int
    source_time_ns: int
    position: tuple[float, float, float]
    quaternion: tuple[float, float, float, float]
    linear_velocity: tuple[float, float, float] = (0., 0., 0.)
    angular_velocity: tuple[float, float, float] = (0., 0., 0.)
    connected: bool = False
    pose_valid: bool = False
    quality: float = 0.
    battery_percent: float | None = None

    def encode(self, now_ns=None):
        now_ns = time.monotonic_ns() if now_ns is None else now_ns
        if (type(self.chip_id) is not int or not 0 < self.chip_id <= 0xffffffff
                or type(self.sequence) is not int or not 0 <= self.sequence < 2**64
                or type(self.source_time_ns) is not int or self.source_time_ns <= 0
                or not 0 <= now_ns-self.source_time_ns <= MAX_AGE_NS):
            raise ValueError('Invalid pose identity, sequence or source time')
        if (len(self.position),len(self.quaternion),len(self.linear_velocity),
                len(self.angular_velocity)) != (3,4,3,3):
            raise ValueError('Invalid pose dimensions')
        values = (*self.position,*self.quaternion,*self.linear_velocity,*self.angular_velocity)
        if not all(math.isfinite(x) and abs(x) <= 1e6 for x in values):
            raise ValueError('Invalid pose values')
        norm = sum(x*x for x in self.quaternion)
        if not .81 <= norm <= 1.21:
            raise ValueError('Invalid quaternion norm')
        if not math.isfinite(self.quality) or not 0 <= self.quality <= 1:
            raise ValueError('Invalid tracking quality')
        if self.pose_valid and (not self.connected or self.quality <= 0):
            raise ValueError('Valid pose requires connection and measurement quality')
        if self.battery_percent is not None and (
                not math.isfinite(self.battery_percent) or not 0 <= self.battery_percent <= 100):
            raise ValueError('Invalid verified battery percent')
        flags = int(self.connected) | (int(self.pose_valid)<<1) | (int(self.battery_percent is not None)<<2)
        return WIRE.pack(b'PTP2',self.chip_id,flags,self.sequence,self.source_time_ns,
                         *values,-1 if self.battery_percent is None else self.battery_percent,self.quality)


class NativePosePublisher:
    def __init__(self):
        self.socket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

    def send(self, frame: PoseFrame):
        self.socket.sendto(frame.encode(),('127.0.0.1',39571))

    def close(self):
        self.socket.close()
