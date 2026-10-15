# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Experimental serial receiver transport, NOT a stock onboard Roy interface.

Requires a receiver exporting the assumed 64-byte Pico payload format over CDC.
No SPI/HID backend or stock-firmware command interface is implemented here.
Packet layout and command IDs still require validation against live captures.
"""

from __future__ import annotations

import logging
import queue
import threading
import time
from typing import Callable
import numpy as np

from radio.esb_packet import (
    TrackerTelemetryPacket,
    decode_telemetry_payload,
    encode_command_packet,
    CMD_SET_IRLED_MODE,
)

logger = logging.getLogger(__name__)


class SteamFrameRadioBridge:
    """Streams an experimental external receiver's serial telemetry."""

    def __init__(
        self,
        port: str = "COM3",  # On Linux/SteamOS: /dev/ttyACM0 or /dev/ttyUSB0
        baudrate: int = 1000000,
        on_packet: Callable[[TrackerTelemetryPacket], None] | None = None,
    ) -> None:
        self.port = port
        self.baudrate = baudrate
        self.on_packet = on_packet

        self._running = False
        self._thread: threading.Thread | None = None
        self._serial = None
        self._latest_packets: dict[int, TrackerTelemetryPacket] = {}
        self._lock = threading.Lock()

    def start(self) -> bool:
        """Start background receiver thread."""
        try:
            import serial
            self._serial = serial.Serial(self.port, self.baudrate, timeout=0.1)
            logger.info("Connected to experimental serial receiver on %s (%d baud)", self.port, self.baudrate)
        except Exception as e:
            logger.error("Could not open serial receiver %s: %s", self.port, e)
            self._serial = None
            return False

        self._running = True
        self._thread = threading.Thread(target=self._rx_worker, daemon=True)
        self._thread.start()
        return True

    def _rx_worker(self) -> None:
        """Worker loop reading binary radio packets from serial stream."""
        buffer = bytearray()
        packet_len = 64

        while self._running:
            if self._serial is not None and self._serial.is_open:
                try:
                    chunk = self._serial.read(64)
                    if chunk:
                        buffer.extend(chunk)
                        while len(buffer) >= packet_len:
                            # Verify packet type marker (0x02 or 0x05)
                            if buffer[0] in (0x02, 0x05):
                                raw_pkt = bytes(buffer[:packet_len])
                                del buffer[:packet_len]
                                pkt = decode_telemetry_payload(raw_pkt)
                                if pkt is not None:
                                    with self._lock:
                                        self._latest_packets[pkt.tracker_id] = pkt
                                    if self.on_packet:
                                        self.on_packet(pkt)
                            else:
                                # Resynchronize on next byte
                                del buffer[0]
                    else:
                        time.sleep(0.002)
                except Exception as e:
                    logger.error("Error reading from radio SoC: %s", e)
                    time.sleep(0.05)
            else:
                time.sleep(0.01)

    def get_latest_packet(self, tracker_id: int = 1) -> TrackerTelemetryPacket | None:
        """Get the most recent telemetry packet for a specific tracker."""
        with self._lock:
            return self._latest_packets.get(tracker_id)

    def set_tracker_led_mode(self, mode: int = 0x01) -> bool:
        """Send command over radio to switch tracker IR LEDs (0x01 = Static ON)."""
        cmd_bytes = encode_command_packet(CMD_SET_IRLED_MODE, param=mode)
        if self._serial is not None and self._serial.is_open:
            try:
                self._serial.write(cmd_bytes)
                logger.info("Sent radio command to switch IR LED mode: 0x%02X", mode)
                return True
            except Exception as e:
                logger.error("Failed to send radio command: %s", e)
                return False
        return False

    def close(self) -> None:
        """Stop receiver thread and release port."""
        self._running = False
        if self._thread is not None:
            self._thread.join(timeout=1.0)
            self._thread = None
        if self._serial is not None:
            try:
                self._serial.close()
            except Exception:
                pass
            self._serial = None


class MockSteamFrameRadioBridge(SteamFrameRadioBridge):
    """Simulates 200 Hz telemetry packets from Pico Tracker for testing."""

    def __init__(
        self,
        fps: float = 200.0,
        on_packet: Callable[[TrackerTelemetryPacket], None] | None = None,
    ) -> None:
        super().__init__(on_packet=on_packet)
        self.fps = fps
        self.interval = 1.0 / fps

    def start(self) -> bool:
        """Start explicitly requested simulation without opening a physical port."""
        self._running = True
        self._thread = threading.Thread(target=self._rx_worker, daemon=True)
        self._thread.start()
        return True

    def _rx_worker(self) -> None:
        seq = 0
        start_t = time.time()
        while self._running:
            now = time.time()
            t = now - start_t

            # Generate synthetic IMU motion (subtle rotation and gravity)
            pitch = float(np.radians(10.0 * np.sin(1.5 * t)))
            cp, sp = float(np.cos(pitch * 0.5)), float(np.sin(pitch * 0.5))
            quat = np.array([sp, 0.0, 0.0, cp], dtype=np.float32)

            acc = np.array([0.0, 9.80665, 0.0], dtype=np.float32)
            gyro = np.array([0.0, 0.0, 0.0], dtype=np.float32)
            mag = np.array([0.0, 0.0, 0.0], dtype=np.float32)

            pkt = TrackerTelemetryPacket(
                tracker_id=1,
                seq_num=seq,
                timestamp_us=int(now * 1e6),
                orientation_quat=quat,
                acc=acc,
                gyro=gyro,
                mag=mag,
                temperature_c=27.2,
                battery_pct=95,
                is_charging=False,
                irled_status=0x01,
                quat_valid=True,
            )

            with self._lock:
                self._latest_packets[1] = pkt

            if self.on_packet:
                self.on_packet(pkt)

            seq += 1
            time.sleep(self.interval)
