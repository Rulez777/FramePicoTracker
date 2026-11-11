# Development Roadmap

## Current blocker: optical tracking

The SteamVR update on 2026-11-06 changed the IR camera cadence from 60 fps (33.333 ms) to 66.67 fps (30 ms). Alpha 1.3 still flashes at 33.333 ms. The producer expects 32–35 ms, so its exposure planner produces no plan and reports `camera_period_mismatch`, `plan_wait`, and zero optical fits. Check `Camera recording rate` in `~/.local/share/Steam/logs/xrservice.txt`. Keep the headset display mode as-is.

The planned Alpha 1.4 work is to align the tracker LED timer with the measured IR cadence. For 30 ms at 16 MHz the period is 480000 ticks, compared with 533333 in Alpha 1.3. Update the period and admissible frame interval in `source/frame-native/native/optical_timing.cpp` together with the tracker firmware. Validate exposure planning, reacquisition, and sustained optical poses on hardware. This work is pending.

## Verified baseline

- On 2026-11-10, single- and three-tracker sessions ran without trackers on the updated Roy firmware. They exited successfully and passed all four cleanup barriers. Pairing modules loaded and released Roy without trackers. The OTA service module built successfully.
- On 2026-11-11, a live Alpha 1.3 tracker on the updated Roy firmware reported status, battery and magnetometer data. IMU reached the driver at roughly 50 radio packets per second with one controller connected. Optical fits were zero.
- Earlier tests at 60 fps measured about 26–28 optical poses per second and about 200 IMU samples per second into the driver for one tracker with both controllers connected. Those figures describe the earlier configuration.
- Live discovery pairing, OTA, two-controller throughput and worn multi-tracker tracking still need verification on the updated firmware. The PC bridge passed local loopback. SteamVR role assignment and VRChat integration still need a live test.

## Development order

1. Restore optical timing and validate one tracker.
2. Measure IMU throughput with both controllers connected. Evaluate `PICO_MULTI_N` for two and three trackers and measure each peer independently.
3. Improve position continuity during occlusion and return to optics. Evaluate against captures with deliberate optical gaps and ground-truth poses.
4. Test small LED constellations at foot and waist distances, then validate two-step body calibration and role assignment with two and three worn trackers.
5. Test the Windows SteamVR bridge and VRChat integration.
6. Investigate intermittent tracker hangs and evaluate strap-class data for automatic waist assignment.

## Build and hardware constraints

- [Start Here](START_HERE.md) lists build commands, deployment paths and smoke tests. The launcher reproduces the shipped Alpha 1.3 image. Rebuilding changed firmware from source requires Zig and the development inputs from `pico-fw-release-20261101`; those inputs are separate from this handoff.
- Live sessions detect the Roy layout before loading a module. If detection fails, investigate the locator and run `roy_locator_tests` and `roy_relocation_tests` with matching local fixtures before retrying.
- Keep runtime tracker and driver code in C/C++. Preserve Roy cleanup and the four recovery checks. Keep station modules in SRAM.
- Tracker firmware changes must preserve the bootloader, vector table, metadata, OTA handlers and default stock-compatible boot. Frame mode is runtime-only. Keep the stock recovery path available.
- Radio polling intervals must be multiples of the 3000 us Frame-mode tracker period. Verify configured values by reading them back.
- Keep owner-assisted checks within one minute. Run headset tests with the game closed.
