# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Virtual Motion Tracker (VMT) OSC Client.

Sends OpenVR tracker poses to SteamVR via VMT's OSC UDP interface.
Default VMT port: 39570 (UDP).
"""

from __future__ import annotations

import logging
from pythonosc.udp_client import SimpleUDPClient
import numpy as np

logger = logging.getLogger(__name__)

DEFAULT_VMT_HOST = "127.0.0.1"
DEFAULT_VMT_PORT = 39570


class VMTClient:
    """Client for Virtual Motion Tracker (VMT) in SteamVR."""

    def __init__(self, host: str = DEFAULT_VMT_HOST, port: int = DEFAULT_VMT_PORT) -> None:
        self.host = host
        self.port = port
        self.client = SimpleUDPClient(host, port)
        logger.info("VMTClient initialized targeting %s:%d", host, port)

    def send_pose(
        self,
        index: int,
        pos: np.ndarray,
        quat: np.ndarray,
        enabled: bool = True,
        time_offset: float = 0.0,
        room_space: bool = True,
    ) -> None:
        """Send tracker pose to SteamVR.

        Args:
            index: Tracker index (0, 1, 2, ...).
            pos: 3D position [x, y, z] in meters (OpenVR right-handed coordinate system).
            quat: Quaternion [qx, qy, qz, qw] (OpenVR right-handed).
            enabled: Whether tracker is active and tracked.
            time_offset: Prediction or delay compensation in seconds (0.0 = current).
            room_space: True to use /vmt/set/room/pose, False to use /vmt/set/raw/pose.
        """
        address = "/vmt/set/room/pose" if room_space else "/vmt/set/raw/pose"
        px, py, pz = float(pos[0]), float(pos[1]), float(pos[2])
        qx, qy, qz, qw = float(quat[0]), float(quat[1]), float(quat[2]), float(quat[3])

        args = [
            int(index),
            1 if enabled else 0,
            float(time_offset),
            px,
            py,
            pz,
            qx,
            qy,
            qz,
            qw,
        ]
        self.client.send_message(address, args)

    def send_diag(self, index: int, message: str) -> None:
        """Send diagnostic/battery message for tracker."""
        self.client.send_message("/vmt/set/diag", [int(index), str(message)])

    def disable_tracker(self, index: int) -> None:
        """Disable tracker in SteamVR."""
        self.client.send_message(
            "/vmt/set/room/pose",
            [int(index), 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
        )
