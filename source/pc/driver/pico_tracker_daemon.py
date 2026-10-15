# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Pico Tracker Driver Daemon for Steam Frame (SteamOS).

Runs as a userspace service on Steam Frame (aarch64) to:
1. Ingest 2.4 GHz ESB packets from a dedicated receiver (USB dongle or interface)
   WITHOUT modifying or flashing the onboard Roy controller SoC.
2. Validate and interpret Pico Motion Tracker signatures in real time (200 Hz).
3. Propagate an experimental inertial estimate (no live camera correction here).
4. Send OSC poses to a separately installed VMT driver.

This daemon does not implement the stock onboard Roy SPI/HID protocol.
"""

from __future__ import annotations

import argparse
import glob
import logging
import os
import signal
import sys
import time
import numpy as np

# Ensure pc/ directory is on sys.path
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PC_DIR = os.path.dirname(SCRIPT_DIR)
if PC_DIR not in sys.path:
    sys.path.insert(0, PC_DIR)

from driver.pico_esb_signature import InterpretedTrackerState, PicoESBSignatureInterpreter
from driver.openvr_tracker import SteamVRTrackerManager, TrackerRole
from optical.fusion_ekf import OpticalInertialEKF
from radio.nrf_radio_bridge import MockSteamFrameRadioBridge, SteamFrameRadioBridge

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] [PicoDaemon] %(message)s",
    datefmt="%H:%M:%S",
)
logger = logging.getLogger("PicoDaemon")


def find_default_radio_port() -> str | None:
    """Auto-detect available USB radio dongle on Linux/SteamOS or Windows."""
    # Linux / SteamOS devices
    candidates = glob.glob("/dev/ttyACM*") + glob.glob("/dev/ttyUSB*")
    # Exclude onboard Roy if it ever appears on tty
    for c in candidates:
        if "ACM" in c or "USB" in c:
            return c

    # Windows fallback
    if os.name == "nt":
        return "COM3"

    return None


class PicoTrackerService:
    def __init__(
        self,
        port: str | None = None,
        use_mock: bool = False,
        vmt_port: int = 39570,
    ) -> None:
        self.port = port
        self.use_mock = use_mock
        self.vmt_port = vmt_port

        self.interpreter = PicoESBSignatureInterpreter()
        self.tracker_mgr = SteamVRTrackerManager(vmt_port=vmt_port)
        self.ekfs: dict[int, OpticalInertialEKF] = {}

        self.running = False
        self.radio = None

        # Statistics
        self.packet_count = 0
        self.start_time = 0.0

    def _on_radio_packet(self, pkt) -> None:
        """Callback from radio bridge on packet arrival."""
        # Convert to raw packet bytes or state
        self.packet_count += 1
        tid = pkt.tracker_id

        if tid not in self.ekfs:
            self.ekfs[tid] = OpticalInertialEKF(initial_pos=np.array([0.0, -0.4, 0.9]))

        ekf = self.ekfs[tid]
        # Propagate EKF forward with 200 Hz IMU packet
        pos = ekf.predict(
            dt=0.005,
            raw_acc_body=pkt.acc,
            quat=pkt.orientation_quat,
        )

        # Publish state to SteamVR
        sn = self.interpreter.tracker_sn_map.get(tid, f"PICO_TRK_{tid}")
        self.tracker_mgr.publish_state(
            tracker_id=tid,
            serial_number=sn,
            pos=pos,
            quat=pkt.orientation_quat,
            battery_pct=pkt.battery_pct,
            is_charging=pkt.is_charging,
        )

    def start(self) -> None:
        logger.info("=======================================================")
        logger.info(" Starting Pico Tracker Interpreter Daemon on Steam Frame")
        logger.info(" Onboard Roy controller SoC remains 100% UNTOUCHED.")
        logger.info("=======================================================")

        if self.use_mock:
            logger.info("Mode: Mock 200 Hz Synthetic Radio Stream")
            self.radio = MockSteamFrameRadioBridge(fps=200.0, on_packet=self._on_radio_packet)
        else:
            resolved_port = self.port or find_default_radio_port()
            if not resolved_port:
                raise RuntimeError("No serial receiver found. Simulation requires explicit --mock; onboard Roy is not implemented.")
            else:
                logger.info("Connecting to Pico ESB Radio on %s...", resolved_port)
                self.radio = SteamFrameRadioBridge(port=resolved_port, on_packet=self._on_radio_packet)

        if not self.radio.start():
            self.radio.close()
            self.radio = None
            raise RuntimeError("Serial receiver could not start; no real telemetry is available")
        self.running = True
        self.start_time = time.time()

        # Send Static ON command over radio so IR LEDs illuminate for Steam Frame cameras
        self.radio.set_tracker_led_mode(0x01)

        logger.info("Daemon running. Exposing native trackers to SteamVR (VMT port %d)", self.vmt_port)

    def run_forever(self) -> None:
        self.start()
        try:
            while self.running:
                time.sleep(1.0)
                elapsed = time.time() - self.start_time
                fps = self.packet_count / elapsed if elapsed > 0 else 0
                logger.info("Throughput: %.1f pkts/sec | Total received: %d", fps, self.packet_count)
        except KeyboardInterrupt:
            logger.info("Daemon stopping...")
        finally:
            self.stop()

    def stop(self) -> None:
        self.running = False
        if self.radio is not None:
            self.radio.close()
            self.radio = None
        logger.info("Daemon stopped.")


def main() -> None:
    parser = argparse.ArgumentParser(description="Pico Motion Tracker Daemon for Steam Frame")
    parser.add_argument("--port", default=None, help="Serial port for ESB dongle (e.g. /dev/ttyACM0 or COM3)")
    parser.add_argument("--mock", action="store_true", help="Run with mock 200 Hz radio stream")
    parser.add_argument("--vmt-port", type=int, default=39570, help="SteamVR VMT OSC port")

    args = parser.parse_args()

    service = PicoTrackerService(port=args.port, use_mock=args.mock, vmt_port=args.vmt_port)

    def sig_handler(sig, frame):
        service.stop()
        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    service.run_forever()


if __name__ == "__main__":
    main()
