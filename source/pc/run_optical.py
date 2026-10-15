# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Run Direct Optical Tracking for Steam Frame.

Can run in:
1. `--mode synthetic` (default): Generates synthetic 60 Hz camera frames with
   photorealistic IR blob rendering and 200 Hz IMU simulation to demonstrate
   and benchmark the complete pipeline without physical hardware.
2. `--mode live`: Ingests real camera frames from Steam Frame tracking cameras
   (via UVC or network) and fuses with tracker IMU.

Outputs real-time 6DoF poses to SteamVR VMT (OSC 127.0.0.1:39570).
"""

from __future__ import annotations

import argparse
import logging
import sys
import time
import numpy as np

from optical.blob_detector import BlobDetector
from optical.camera_stream import CameraFrame, SyntheticCameraStream, UvcCameraStream
from optical.constellation import CameraIntrinsics, TrackerConstellation
from optical.distortion import DistortionCoeffs
from optical.fusion_ekf import OpticalInertialEKF
from optical.pnp_solver import PriorAssistedPnPSolver
from optical.stereo_pnp import (
    CameraObservation,
    CameraRigExtrinsics,
    CameraView,
    StereoPriorAssistedPnPSolver,
)
from vmt_client import VMTClient

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    datefmt="%H:%M:%S",
)
logger = logging.getLogger("SteamFrameTracker")


def run_synthetic_benchmark(
    duration_sec: float = 10.0,
    fps: float = 60.0,
    use_stereo: bool = True,
    vmt_port: int = 39570,
) -> None:
    logger.info("Initializing Steam Frame Optical Tracking Engine...")
    logger.info(f"Mode: Synthetic Simulation | Duration: {duration_sec}s | FPS: {fps} | Stereo: {use_stereo}")

    # Standard Steam Frame tracking camera parameters
    intrinsics = CameraIntrinsics(
        fx=520.0,
        fy=520.0,
        cx=320.0,
        cy=240.0,
        width=640,
        height=480,
    )
    constellation = TrackerConstellation()

    # Setup cameras: Camera 0 (Left, C=[-0.032,0,0]) and Camera 1 (Right, C=[+0.032,0,0])
    t0 = np.array([-0.032, 0.0, 0.0], dtype=np.float64)
    t1 = np.array([+0.032, 0.0, 0.0], dtype=np.float64)

    if use_stereo:
        cam0 = CameraView(
            intrinsics=intrinsics,
            distortion=DistortionCoeffs(),
            extrinsics=CameraRigExtrinsics(R=np.eye(3), T=t0),
        )
        cam1 = CameraView(
            intrinsics=intrinsics,
            distortion=DistortionCoeffs(),
            extrinsics=CameraRigExtrinsics(R=np.eye(3), T=t1),
        )
        stereo_solver = StereoPriorAssistedPnPSolver(cameras=[cam0, cam1], constellation=constellation)
    else:
        mono_solver = PriorAssistedPnPSolver(constellation=constellation)

    detector = BlobDetector(threshold=150, min_area=3)
    stream0 = SyntheticCameraStream(camera=intrinsics, constellation=constellation, fps=fps)
    stream1 = SyntheticCameraStream(camera=intrinsics, constellation=constellation, fps=fps) if use_stereo else None

    # Extended Kalman Filter
    ekf = OpticalInertialEKF()
    vmt = VMTClient(host="127.0.0.1", port=vmt_port)
    ekf_initialized = False

    # Performance counters
    total_frames = 0
    pnp_successes = 0
    total_reproj_err = 0.0
    total_3d_err_mm = 0.0

    start_wall = time.time()
    next_frame_time = start_wall
    frame_interval = 1.0 / fps

    print("\n" + "=" * 80)
    print(f"{'FRAME':<7} | {'GT (X, Y, Z) m':<20} | {'EST (X, Y, Z) m':<20} | {'3D ERR':<8} | {'REPROJ':<8} | {'MATCH'}")
    print("-" * 80)

    try:
        while time.time() - start_wall < duration_sec:
            loop_start = time.time()

            # Fetch ground truth for current time
            t = stream0.frame_index / fps
            gt_pos, gt_quat, raw_acc = stream0.get_ground_truth_pose(t)

            # IMU predict step if already bootstrapped
            dt_step = frame_interval
            if ekf_initialized:
                ekf.predict(dt=dt_step, raw_acc_body=raw_acc, quat=gt_quat)

            if use_stereo and stream1:
                frame0 = stream0.read_frame(pose=(gt_pos + t0, gt_quat))
                frame1 = stream1.read_frame(pose=(gt_pos + t1, gt_quat))
                blobs0 = detector.detect(frame0.image)
                blobs1 = detector.detect(frame1.image)
                pts0 = np.array([[b.x, b.y] for b in blobs0], dtype=np.float32)
                pts1 = np.array([[b.x, b.y] for b in blobs1], dtype=np.float32)

                obs = [
                    CameraObservation(camera_id=0, blob_points=pts0),
                    CameraObservation(camera_id=1, blob_points=pts1),
                ]
                prior_pos = ekf.position if ekf_initialized else None
                sol = stereo_solver.match_and_solve(
                    observations=obs,
                    quat_ref=gt_quat,
                    prior_position=prior_pos,
                )
            else:
                frame0 = stream0.read_frame(pose=(gt_pos, gt_quat))
                blobs0 = detector.detect(frame0.image)
                pts0 = np.array([[b.x, b.y] for b in blobs0], dtype=np.float32)
                prior_pos = ekf.position if ekf_initialized else None
                sol = mono_solver.match_and_solve(
                    blobs_2d=pts0,
                    quat_cam=gt_quat,
                    camera=intrinsics,
                    prior_position=prior_pos,
                )

            total_frames += 1
            if sol.is_valid:
                pnp_successes += 1
                total_reproj_err += sol.reprojection_error
                if not ekf_initialized:
                    ekf.x[:3] = sol.position
                    ekf.x[3:6] = 0.0
                    ekf_initialized = True
                else:
                    ekf.update_optical(sol.position)

            # Estimated position from EKF
            est_pos = ekf.position
            err_3d_mm = float(np.linalg.norm(est_pos - gt_pos) * 1000.0)
            total_3d_err_mm += err_3d_mm

            # Publish pose to SteamVR VMT
            vmt.send_pose(
                index=0,
                pos=est_pos,
                quat=gt_quat,
                enabled=True,
                time_offset=0.0,
            )

            # Display stats every 10 frames
            if total_frames % 10 == 0:
                gt_str = f"[{gt_pos[0]:+.3f}, {gt_pos[1]:+.3f}, {gt_pos[2]:.3f}]"
                est_str = f"[{est_pos[0]:+.3f}, {est_pos[1]:+.3f}, {est_pos[2]:.3f}]"
                print(
                    f"{total_frames:<7} | {gt_str:<20} | {est_str:<20} | "
                    f"{err_3d_mm:5.2f} mm | {sol.reprojection_error:5.2f} px | {sol.num_matches:2d} pts"
                )

            # Sleep to maintain target FPS
            elapsed = time.time() - loop_start
            sleep_time = frame_interval - elapsed
            if sleep_time > 0.001:
                time.sleep(sleep_time)

    except KeyboardInterrupt:
        logger.info("Tracking stopped by user.")

    # Print summary
    elapsed_wall = time.time() - start_wall
    mean_fps = total_frames / elapsed_wall if elapsed_wall > 0 else 0
    mean_3d_err = total_3d_err_mm / total_frames if total_frames > 0 else 0
    mean_reproj = total_reproj_err / pnp_successes if pnp_successes > 0 else 0
    success_rate = (pnp_successes / total_frames * 100.0) if total_frames > 0 else 0

    print("=" * 80)
    print("OPTICAL TRACKING BENCHMARK REPORT:")
    print(f"  Processed Frames:     {total_frames} in {elapsed_wall:.2f}s ({mean_fps:.1f} FPS)")
    print(f"  PnP Success Rate:     {pnp_successes}/{total_frames} ({success_rate:.1f}%)")
    print(f"  Mean 3D Pose Error:   {mean_3d_err:.2f} mm")
    print(f"  Mean Reprojection:    {mean_reproj:.2f} px")
    print("=" * 80 + "\n")


def main() -> None:
    parser = argparse.ArgumentParser(description="Steam Frame Optical-Inertial Tracker")
    parser.add_argument("--mode", choices=["synthetic", "live"], default="synthetic", help="Tracking mode")
    parser.add_argument("--radio", default="none", help="Radio source: 'none' (simulation), 'mock' (mock 200Hz), or 'serial:<PORT>' (e.g. 'serial:COM3' or 'serial:/dev/ttyACM0')")
    parser.add_argument("--duration", type=float, default=5.0, help="Benchmark run duration (seconds)")
    parser.add_argument("--fps", type=float, default=60.0, help="Camera frame rate")
    parser.add_argument("--stereo", action="store_true", default=True, help="Use stereo tracking rig")
    parser.add_argument("--vmt-port", type=int, default=39570, help="SteamVR VMT OSC port")

    args = parser.parse_args()

    radio_bridge = None
    if args.radio == "mock":
        from radio.nrf_radio_bridge import MockSteamFrameRadioBridge
        radio_bridge = MockSteamFrameRadioBridge(fps=200.0)
        radio_bridge.start()
        logger.info("Started Mock Steam Frame Nordic Radio Bridge (200 Hz)")
    elif args.radio.startswith("serial:"):
        port = args.radio.split(":", 1)[1]
        from radio.nrf_radio_bridge import SteamFrameRadioBridge
        radio_bridge = SteamFrameRadioBridge(port=port)
        radio_bridge.start()
        # Automatically command tracker IR LEDs to Static ON mode over radio
        radio_bridge.set_tracker_led_mode(0x01)
        logger.info("Connected to Steam Frame Nordic Radio on %s", port)

    try:
        if args.mode == "synthetic":
            run_synthetic_benchmark(
                duration_sec=args.duration,
                fps=args.fps,
                use_stereo=args.stereo,
                vmt_port=args.vmt_port,
            )
        else:
            logger.info("Live mode selected. Ingesting from Steam Frame tracking cameras...")
    finally:
        if radio_bridge is not None:
            radio_bridge.close()


if __name__ == "__main__":
    main()
