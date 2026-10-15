# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""SteamVR OpenVR Tracker Device Provider.

Publishes Pico Motion Trackers as native SteamVR Tracker devices
(TrackedDeviceClass_GenericTracker) alongside Valve Frame Controllers (Roy)
without interfering with the onboard Roy dongle or SPI HID drivers.
"""

from __future__ import annotations

from enum import Enum
import logging
import numpy as np

try:
    from vmt_client import VMTClient
except ImportError:
    from vmt_client import VMTClient

logger = logging.getLogger(__name__)


class TrackerRole(str, Enum):
    WAIST = "Waist"
    LEFT_FOOT = "LeftFoot"
    RIGHT_FOOT = "RightFoot"
    LEFT_KNEE = "LeftKnee"
    RIGHT_KNEE = "RightKnee"
    CHEST = "Chest"
    GENERIC = "Tracker"


DEFAULT_ROLE_MAP = {
    1: TrackerRole.WAIST,
    2: TrackerRole.LEFT_FOOT,
    3: TrackerRole.RIGHT_FOOT,
}


class SteamVRTrackerDevice:
    """Manages an individual virtual SteamVR Tracker device."""

    def __init__(
        self,
        tracker_id: int,
        serial_number: str,
        role: TrackerRole = TrackerRole.WAIST,
        vmt_client: VMTClient | None = None,
    ) -> None:
        self.tracker_id = tracker_id
        self.serial_number = serial_number
        self.role = role
        self.vmt = vmt_client or VMTClient()

        self.last_pos = np.zeros(3, dtype=np.float32)
        self.last_quat = np.array([0.0, 0.0, 0.0, 1.0], dtype=np.float32)
        self.is_connected = False
        self.battery_pct = 100
        self.is_charging = False

    def update_pose(
        self,
        position: np.ndarray,
        orientation: np.ndarray,
        time_offset: float = 0.0,
    ) -> None:
        """Send 6DoF pose update to SteamVR."""
        self.last_pos = np.asarray(position, dtype=np.float32)
        self.last_quat = np.asarray(orientation, dtype=np.float32)
        self.is_connected = True

        # Send pose to SteamVR VMT via OSC
        self.vmt.send_pose(
            index=self.tracker_id,
            pos=self.last_pos,
            quat=self.last_quat,
            enabled=True,
            time_offset=time_offset,
        )

    def update_battery(self, battery_pct: int, is_charging: bool) -> None:
        """Update battery status in SteamVR."""
        self.battery_pct = battery_pct
        self.is_charging = is_charging
        # VMT support for battery status
        if hasattr(self.vmt, "send_battery"):
            self.vmt.send_battery(self.tracker_id, battery_pct / 100.0, is_charging)


class SteamVRTrackerManager:
    """Manages multiple Pico Trackers coexisting with Valve Frame Controllers."""

    def __init__(
        self,
        role_map: dict[int, TrackerRole] | None = None,
        vmt_port: int = 39570,
    ) -> None:
        self.role_map = role_map or DEFAULT_ROLE_MAP
        self.vmt = VMTClient(port=vmt_port)
        self.devices: dict[int, SteamVRTrackerDevice] = {}

    def get_or_create_device(self, tracker_id: int, serial_number: str) -> SteamVRTrackerDevice:
        if tracker_id not in self.devices:
            role = self.role_map.get(tracker_id, TrackerRole.GENERIC)
            dev = SteamVRTrackerDevice(
                tracker_id=tracker_id,
                serial_number=serial_number,
                role=role,
                vmt_client=self.vmt,
            )
            self.devices[tracker_id] = dev
            logger.info(
                "Registered SteamVR Tracker #%d (SN: %s, Role: %s)",
                tracker_id,
                serial_number,
                role.value,
            )
        return self.devices[tracker_id]

    def publish_state(
        self,
        tracker_id: int,
        serial_number: str,
        pos: np.ndarray,
        quat: np.ndarray,
        battery_pct: int,
        is_charging: bool,
    ) -> None:
        dev = self.get_or_create_device(tracker_id, serial_number)
        dev.update_battery(battery_pct, is_charging)
        dev.update_pose(pos, quat)
