# Status at publication

Snapshot prepared October 15, 2026. The most recent saved results are described below. No device was queried or changed to publish this repository.

## What has been demonstrated

The native Frame interface and diagnostic API exist, including separate Game and Service flows. The protected single tracker baseline uses diagnostic10 on the PICO side and original210 on Roy. Both are exact version dependent research candidates.

The saved 393-v2 Game check lasted 150 seconds. It recorded 16 request matched fresh nonce replies, including 15 distinct replies confirming mode1, N2 and pending0 over about 140.1 seconds. The process exited successfully and all four restoration barriers were recorded true. IMU and optical output were outside that test's scope, and pose_valid remained false.

Earlier phase235 and phase236 pilots received actual IMU data and published sparse optical measurements. They do not establish that the present integrated stack supplies stable calibrated optical and inertial tracking.

## The current blocker

The latest passive stereo probe processed 900 matched pairs. Every pair had too few stereo consistent points, with zero accepted fits. A sample of 39 frames per upper camera at about 37.23 microseconds of exposure had no frame with four detected markers.

The median completion age was about 77.6 ms and the maximum about 99.4 ms, below the 250 ms guard for all 900 pairs. That result does not support calling this merely another latency failure. It also does not prove that there were no photons or that exposure is the only cause.

A bounded longer exposure candidate was prepared, targeting a measured 750 to 850 microseconds and then restoring the native settings. No completed result for it is in this snapshot. It needs fresh readiness, exact input checks and an approved procedure before execution.

## How far from full body tracking?

Several major milestones remain. A percentage would hide the unresolved technical risk.

| Milestone | State | Evidence needed to finish |
| --- | --- | --- |
| One tracker radio mode control | Bounded hardware result | Longer independent repetition, reconnect and power cycle behavior. |
| Reliable visible optical markers | Blocked | Actual exposure and sufficient measured stereo marker observations under controlled conditions. |
| A fresh pose from the correct tracker | Unfinished | Causal identity, correct camera and world frames, age and uncertainty checks. |
| Stable optical and IMU fusion | Unfinished | Verified sensor timing, per device axes and bias, stable quality and honest loss handling. |
| Waist and both legs | Unfinished | Individually admitted peers, role assignment and measured three tracker quality. |
| Normal controllers working alongside tracking | Unfinished | Sustained simultaneous coexistence within the controller guard. |
| A user installable release | Unfinished | Reproducible builds, safe setup and recovery, reconnect testing and an actual application session. |

This repository is for research continuation. It is not currently a way for a VRChat user to install full body tracking and start playing.

## Verification limits

Package integrity, source syntax, sparse diff checks and isolated calibration guards were checked. The source provenance review was bounded. There was no clean machine full build, full dependency import test or hardware validation of this exported snapshot. Three older identity bound C candidates are intentionally blocked at compile time; current game and service source variants are recorded separately.

Read [CURRENT_STATE.md](CURRENT_STATE.md) for exact hashes and measurements, [SOURCE_REVIEW.md](../verification/SOURCE_REVIEW.md) for review limits, and [CREDITS.md](../CREDITS.md) for external reference attribution.
