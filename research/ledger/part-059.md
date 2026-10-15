# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K04641
Source `D0166`, original line889, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Currently, coupling of stages B, C, and D is underway: ABI4 radio runtime, exposure synchronization, optical chipID identification, and measured world pose. Source files for the overall pipeline have been created, but its integration audit and hardware verification are not complete. Especially important: the presence of a fusion method in code does not mean that IMU processing is already connected to it in the production runtime.

## K04642
Source `D0166`, original line891, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Verified on Frame: API active, radio service inactive, OpenOCD not running. The installed radio unit still references the old `roy_station_service250.elf` with duration of 10 seconds. The new driver, UI, API, and unit from this work are not installed. Candidate directory: `/home/steamos/pico-ram-work/integration-20261006`.

## K04643
Source `D0166`, original line893, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The user turned on the headset and both Valve controllers. The read-only monitor confirmed valid poses for all three devices. This is a verification of the baseline state, **not compatibility with the new radio**, which was not run. Pressing search/pairing on the PICO did not lead to a connection, since the new station is not yet working. Inviting the user to verify was premature.

## K04644
Source `D0166`, original line895, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Status of stages A–F

## K04645
Source `D0166`, original line897, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Status as of evening 2026-10-06. Legend: ✅ done, 🔁 redone after audit, ⏳ remaining.

## K04646
Source `D0166`, original line899, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Stage | Done | Redone (evening 06.10) | Remaining |
|---|---|---|---|
| A. Build and Roy ownership | ✅ Manifest with SHA, reproducible build, flock, callback/MPU/RAM checks, checkpoint, trace controls restoration. ✅ Release `release-20261006b` built and **installed** with backup | 🔁 Cleanup of each resource in its own finally; OpenOCD terminate→kill | ⏳ Startup/shutdown sequence on hardware not verified |
| B. Three radio devices | ✅ ABI4 (1956/2045 B), three profiles, seqlock-mailbox, discovery/enrollment, paired reconnect, ACK/seq-engine, controller watchdog | 🔁 Mailbox timing inside seqlock; timeouts in DWT cycles (were in slots, broke on fast interval); new RF tag on each attempt and correct ACK attribution; unified reset on reconnect; enrollment with empty mailbox and registry preservation before unpausing | ⏳ ABI4 was not executed on Roy; three-device stability, polling rate, controller compatibility |
| C. Cameras and LEDs | ✅ Live frames, PTS↔OCC, serial↔sourceCamera, KB calibration, extrinsics, RAW↔MONOTONIC, timed command10/14, temporal identity. ✅ **New:** automatic LED phase search (`phase_search.py`) | 🔁 Frame index incremental with anchors (was drift); schedule caches by physical slot; plan stores chip; change in device count resets queue 10/14; temporal identity allows smooth movement | ⏳ Phase search, dark-pulse identity, and LED visibility on real trackers not verified |
| D. World pose and fusion | ✅ Metric PnP/RANSAC, camera→head→world, publication barriers, EKF. ✅ **New:** IMU axis/bias/time-offset calibration from optics (`imu_calibration.py`), IMU connected to runtime | 🔁 Atomic IMU propagation with rollback (previously double rotation); IMU buffer replay after optical anchor; bias and offset in `configure_imu` | ⏳ Real poses of three trackers; real IMU calibration by movement; meters/orientation/world stability |
| E. SteamVR | ✅ PTP2 and native driver, chipID identity, stale/reorder/finite checks. ✅ New driver **installed** (atomic replacement) | Without source changes | ⏳ Driver loading by restarting SteamVR; three GenericTrackers in SteamVR and game |
| F. Service and application | ✅ New UI/API **installed**, API restarted; radio unit ABI4 installed (disabled, not started) | UI rebuilt from new main.cpp | ⏳ Autostart, pairing/reconnect, UI closing, restarts; camera clock evidence tied to current boot |

## K04647
Source `D0166`, original line908, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Not a single stage can be declared fully completed based solely on sources or unit tests.**

## K04648
Source `D0166`, original line910, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What is confirmed by verifications

## K04649
Source `D0166`, original line912, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **220 offline tests pass** (06.10 evening; was 177). **ABI4 after fixes: 1956 bytes, code SHA256 `686794f8…`, ELF `30eedf64…`; item 2 below refers to the previous build.** Baseline: Python modules optical and radio/ram pass compileall. Full rotation/position math, outlier rejection, world pose transformation under head movement, clock/counter bounds, timed mailbox, watchdog, and temporal identity rejection on dropped frames/movement verified. Synthetic poses were not sent to SteamVR.
2. **ABI4 builds into 1928 bytes**, limit 2045, aligned size 1928. Code SHA256: `cdc769d142071851c458b0b6000f0abad89397cbbc9e4045fa5a44a235725e08`. ELF SHA256: `12103d05ee37c9a058d572dc9228e6fcd65e2ee716e5d3f34dcb0903347a90da`. The build was not loaded into SRAM.
3. New native driver and PTP2 codec built on Frame. Native read-only `tracking_watch` also built, including GenericTracker reading. This is not proof of receiving real tracker poses.
4. **One camera-only verification, 15 seconds:** 1800 metadata sets, 1372 decoded IR frames from four cameras, 1056×1024, exact PTS to OCC mapping. Dataset: `Deckard_2026-10-06_18-24-27.dataset`. The recording created by us has been stopped. RF/SWD/pose publication were not run.
5. For this fresh recording, the clock domain is distinguished by causality: source timestamp corresponds to `CLOCK_MONOTONIC_RAW`; by the read time, RAW age was 26.849–32.269 ms. In `CLOCK_MONOTONIC`, the same timestamps were in the future by 113.730–119.151 ms; BOOTTIME diverged by approximately 501 second. RAW→MONOTONIC conversion is now measured on each translation, because constant offset drifts. Evidence is tied to current boot; behavior after next boot still needs to be implemented and verified.
6. Factory serial links each IR camera to its active named calibration. Camera→head transform matches recorded SteamVR `sensorsFromHead` for slam_right with numerical discrepancy up to 1e-6. Mathematical agreement does not replace verification of world stability on a real tracker.

## K04650
Source `D0166`, original line919, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Created/modified main components

## K04651
Source `D0166`, original line921, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- `pc/radio/ram/roy_ram_station_v4.c`: SRAM ABI4, ring 10×112B, three peer profiles, seqlock-mailbox, phase patch on TX, time windows, trace controls.
- `pc/radio/ram/peer_engine.py`, `station_v4_runtime.py`: product/registry, commands, ACK, sequence, reconnect, and live state.
- `pc/radio/ram/roy_ram_loader.py`, `ownership.py`, `roy_ram_recover.py`, `build_station.py`: lifecycle, watchdog, build, and recovery.
- `pc/optical/live_source.py`, `live_decode.py`, `h264_timestamp_mux.py`: bounded live transport, SPS/PPS priming without historical pictures, timestamped decode.
- `pc/optical/frame_camera_rig.py`, `measurement_clock.py`, `exposure_schedule.py`: calibration, clock, and command10/14 schedule.
- `pc/optical/ray_pose_solver.py`, `measured_pipeline.py`, `station_optics.py`, `temporal_identity.py`: measured pose and overall pipeline. Temporal identity is currently experimental and not hardware-certified.
- `pc/driver/pose_frame.py`, native driver, `pc/frame_app/tracking_watch.cpp`: PTP2 and OpenVR evidence.
- `pc/frame_app/frame-pico-radio.service`, `main.cpp`, `pc/radio/frame_debug_agent.py`: updated sources only; installation pending.

## K04652
Source `D0166`, original line930, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What to do next, in strict order

## K04653
Source `D0166`, original line932, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. ✅ **DONE 06.10 evening** (Codex+Gemini audit, 11+6 remarks, confirmed ones fixed, +12 integration tests). Not verified on hardware: actual phase value (now searched automatically). **Finish integration audit before RF.** Verify timed command10/14 with dropped slots, ACK during retiming, change in device count, reconnect with MCU reset, sequence transmission, and dark-frame calculation. The value of `phase_at_exposure_ticks` has not yet been measured on a real PICO; the existing schedule does not prove that the LED pulse falls within the short exposure. Check cleanup on optical flow error, guard, and worker stop. Add necessary integration tests rather than repeating already passed checks without changes.
2. ✅ **DONE offline 06.10 evening** (`imu_calibration.py`, wiring in `station_optics.py`/`measured_pipeline.py`; inertial prediction is enabled only after verified calibration). ⏳ The calibration itself is performed in item 4 by moving the trackers. **Complete inertial path.** Implement measurement of body axes/bias and sample-time offset from optical observations; connect real decoded IMU samples to `MeasuredPosePipeline.imu`. Until this is in place, only confirmed optical poses are permitted; do not enable inertial prediction with default axes.
3. ✅ **DONE 06.10 19:36**: `release-20261006b` built and installed via `install_release.sh`, backup copy `backup-before-20261006b`, autostart not enabled. **Prepare one consistent release build.** Update candidate Python to latest local sources, build native driver/UI/watch, generate config/registry/manifest of a single version. Do not mix ABI3, ABI4, and legacy PTP1. Install release and driver, preserving previous installation. Do not enable autostart before lifecycle verification.
4. ⏳ **NEXT STEP**, requires user in headset with two controllers and three trackers. Order: station start → LED phase search → temporal identity → IMU calibration (rotation along all axes, two pauses) → SteamVR restart for new driver. **Run one joint hardware session, 10 minutes.** Only after readiness of items 1–3 and fresh confirmation of HMD + two Valve controllers. Verify three PICOs, live LED visibility, command10/14 phase, temporal identity without swaps, measured world poses, and real streaming to GenericTracker. Collect latency, sample loss, optical inliers/residuals, controller validity, CPU/RAM/disk. Short isolated RF trials are prohibited.
5. ⏳ Based on the results of item 4. **Fix only specifically detected defects.** If the joint session does not pass, save exact data; next RF launch only after fixing the identified defect. Do not declare "done" upon RF connection or icon appearance.
6. ⏳ Not started. Known blocker: camera clock evidence is tied to boot `a7b75755…`, after reboot an automatic clock re-audit is needed. **Complete commissioning and gameplay.** OFF/ON of each PICO, UI closing, API restart, SteamVR restart, boot, safe recovery of Roy; current-boot clock evidence must not permanently block the next boot. Check orientations/meters/world stability and leg occlusion. Then enable stable service and test FBT in a consuming game with assigned roles.

## K04654
Source `D0166`, original line939, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## When the user will be able to play

## K04655
Source `D0166`, original line941, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

After items 1–6, when three moving GenericTrackers are genuinely confirmed and normal tracking of both Valve controllers is preserved. There is currently no reliable timeframe: hardware testing of the new loop has not yet begun. The test build is installed (06.10 19:36), so the next step already requires the user: session of item 4 upon their readiness.

## K04656
Source `D0166`, original line943, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Private evidence files are located in `outputs/live-camera-clock-audit-20261006.json`, `outputs/camera-clock-evidence-20261006.json`, and isolated remote `build/`. Do not publish firmware, calibrations, or raw captures.

## K04657
Source `D0168`, original line1, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# FW189 GRID3000: Codex verdict on Opus and Grok, 2026-10-07

## K04658
Source `D0168`, original line3, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Status: candidate encoding confirmed; functional completeness of the patch and OTA readiness not confirmed. User hardware hold remains in effect. Original reports, production sources, and images are unchanged.

## K04659
Source `D0168`, original line5, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 1. Main fix: TIMER0 has a tick of 1 µs

## K04660
Source `D0168`, original line7, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Grok's latest remarks mistakenly call writing 3 to `0x40008508` a PRESCALER configuration. This is BITMODE. According to the Nordic specification, BITMODE is located at +0x508, PRESCALER at +0x510.

## K04661
Source `D0168`, original line9, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The actual init `0xbc06 → 0x8492 → 0x6184` writes:

## K04662
Source `D0168`, original line11, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Address | Register | Value |
| --- | --- | --- |
| 0x40008504 | MODE | 0, timer |
| 0x40008508 | BITMODE | 3 |
| 0x40008510 | PRESCALER | 4 |

## K04663
Source `D0168`, original line17, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is additionally reproduced by executing stock init in Unicorn. Frequency of TIMER0 = 16 MHz / 2^4 = 1 MHz. Consequently, 18910 ticks correspond to 18910 µs, 12000 to 12000 µs, 3000 to 3000 µs. Grok's conclusion of 6000 µs and 1500 µs is rejected. The init check is limited, is not an execution of the device's entire boot process, and does not rule out subsequent register modifications in other modes.

## K04664
Source `D0168`, original line19, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Source of register map and formula: Nordic TIMER specification (original reference: `https://docs.nordicsemi.com/r/bundle/ps_nrf52833/page/timer.html`).

## K04665
Source `D0168`, original line21, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the intermediate offline JSON of this verification, register +0x508 was also initially misnamed; the entry has been corrected, and the error is explicitly noted in the correction field. This intermediate interpretation cannot be used as confirmation of Grok.

## K04666
Source `D0168`, original line23, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2. RTC: remark is substantial and confirmed

## K04667
Source `D0168`, original line25, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

When `0x20000f02` is non-zero, function 0x641c converts the argument via a double divider 30.5 and writes the result into RTC CC. When the flag is zero, a different path is used. The conclusion from one branch cannot be generalized to all modes without investigating the sources of the flag.

## K04668
Source `D0168`, original line27, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

RX arithmetic and the RTC branch itself were executed offline. For candidate N=13, an integer argument 40810 is obtained first, then CC=1338. With RTC at 32768 Hz and PRESCALER=0, this is 40832.51953125 µs. N=9 yields argument 28810 and CC=944, which is 28808.59375 µs under the same assumptions. Source of results: `outputs/tracker-grid3000-20261007/timer-units-countercheck.json`.

## K04669
Source `D0168`, original line29, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Thus, previous checks of RX arguments did not prove physical window time. However, a non-integer number of RTC ticks for 3000 µs does not in itself prove the grid is impossible: phase, rounding, and possible correction must be taken into account. The proposal to change the period to 2989 is also not a ready solution: 98 physical RTC ticks at the specified frequency amount to 2990.72265625 µs.

## K04670
Source `D0168`, original line31, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. Constants 9922 and 10980

## K04671
Source `D0168`, original line33, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Five MOVW #9922 indeed remain at 0x6538, 0x65a2, 0x67de, 0x6c42, 0x6c5c and call 0x641c. The real bytes are 42f2c260; the Opus reproducer with 42f6c26c is incorrect.

## K04672
Source `D0168`, original line35, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Replacing 9922 → 6467 is unjustified. The equality 9922 = 2×4727.5+467 does not establish the origin of the constant. The number 467 in another handler has a different unit and does not prove such a dependency. These locations require analysis of purpose and call conditions.

## K04673
Source `D0168`, original line37, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The unchanged MOVW #10980 at 0x6eac is related to quanta: 10980 = 24×457.5 = 360×30.5. This is sufficient reason to include the branch in the completeness audit, but insufficient for automatic scaling: its gate and purpose are important.

## K04674
Source `D0168`, original line39, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 4. What to accept from the reports

## K04675
Source `D0168`, original line41, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Ten sites and 30 differing bytes: confirmed in narrow byte-by-byte scope. Completeness of schedule translation is not established by this.
- OTA start/data/finish format, SPI layout, and CRC: confirmed in offline model. This does not imply sender readiness or successful activation.
- waiting_target: reproduced defect in recording the result upon timeout before OTA start.
- ACK: start/data/finish do not provide necessary correlation with transmission/session; offline reception of foreign generation is reproduced. Specific delivery of an old ACK through live queues remains a hypothesis. A check of the numeric tag alone is insufficient without considering its reuse.
- Inevitable collision at 12 ms: not proven. The correct unit of TIMER0 restores the nominal 12 ms, but does not replace the analysis of ADDRESS, COMPARE0_STOP, gates, phases, and on-air time.
- Bootloader/activation/recovery: insufficient data. The presence of a stock file does not prove recovery capability after any failure.

## K04676
Source `D0168`, original line48, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Grok's additional address corrections to Opus regarding 25.0 and 299815 are useful: one should distinguish the 32-bit high literal of a double from the finished 64-bit double, and the load instruction from the address of its pool.

## K04677
Source `D0168`, original line50, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 5. Summary and future work

## K04678
Source `D0168`, original line52, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The Opus report is accepted as a limited verification of bytes and the OTA model. Its A-1 is not accepted as a recipe for a fix. Grok's latest remarks are accepted regarding RTC and ACK limitations; the central argument of PRESCALER=3 is rejected based on register map and init execution.

## K04679
Source `D0168`, original line54, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The current candidate cannot be considered a finished solution for coexistence with controllers and cannot be considered ready for flashing. Nor is it disproven by the argument that "TIMER0 runs twice as fast". Before a new candidate, a full audit of timer units and branches, a decision on 9922/10980, verification of the actual RX phase, and elimination of sender defects are needed. Prior to hardware OTA, a confirmed activation/recovery path is separately required.

## K04680
Source `D0168`, original line56, SHA256 `76abf384e5a4d285ac5925dabcdf46dbb9b4ad6d569766578041d29469d700bf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

79 previous tests remain evidence of their limited scope, rather than proof of a correct physical schedule. Firmware/sender fixes and hardware actions were not performed as part of this verdict.

## K04681
Source `D0169`, original line1, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Independent validation report of PICO FW189 patch (GRID3000)

## K04682
Source `D0169`, original line3, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Validation date**: 2026-10-07  
**Validation object**: Document FW189_GRID3000_INDEPENDENT_VALIDATION.md (original reference: `[PRIVATE_SOURCE_REFERENCE]`) and artifacts in directory `K:\AI\pico\outputs\tracker-grid3000-20261007/`  
**Status**: Strict audit without modification of source file and without use of physical hardware.

## K04683
Source `D0169`, original line9, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 1. Official verdicts

## K04684
Source `D0169`, original line20, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2. Boundaries and justification of verdicts

## K04685
Source `D0169`, original line22, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 2.1. Binary patch (`correct within specified scope`)
- **Verified byte-by-byte**: Exactly 10 regions are affected (56 range bytes, **30 actually modified bytes**).
- **ARMv7-M Encoding**: All 6 `MOVW T3` instructions (`0x6214`, `0x6218`, `0x630a`, `0x630e`, `0x6cde`, `0x6ce4`) are encoded in strict compliance with the ARMv7-M architectural specification (validated via Capstone disassembler and bitwise parsing).
- **LE Double Floating Point**: Constants `0x6d40` (3000.0), `0x6d48` (1810.5), `0x6d50` (2207.0), `0x6f80` (3000.0) are aligned to 8 bytes and correctly encoded in little-endian IEEE754.
- **RX Offset Math**: Offset $\Delta = 1727.5$ µs is subtracted from the receive window reopening intervals (`3538 - 1727.5 = 1810.5`, `3934.5 - 1727.5 = 2207.0`), which preserves the nominal width of the receive aperture window relative to the end of the period ($1189.5$ µs and $793$ µs respectively).
- **Compensation −25 µs**: Calling function `0x13754` in the miss branch inverts the high sign bit of the second double operand (`0x40390000_00000000 = 25.0`) via `eor r3, r3, #0x80000000` and branches to `0x13508` (`__aeabi_dadd`). This is a proven mathematical **subtraction** of 25.0 µs, not an addition.
- **Reversibility**: The `reverse(candidate, manifest)` function bitwise restores the original `STOCK_SHA256` (`477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89`).
- **Immutability of critical regions**: Vector table, stack (`0x20006000`), interrupt handlers, pairing, and service structures are not modified.

## K04686
Source `D0169`, original line31, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 2.2. OTA sender (`correct within specified scope`)
- **Command 1 / Subcommand 0x25 Protocol**: Native dispatching `0x8c10 -> 0x16558 -> 0x88cc -> 0xfbcc` confirms packet structure:
  - Start (9 bytes): `25 01 [len:4 BE] [crc8:1] [ver:2 BE]`
  - Data (29 bytes): `25 02 [idx:2 BE] [len:1] [data:24]`
  - Finish (2 bytes): `25 03`
- **Acknowledgement Decoding (ACK)**: Processing of positive/negative ACKs, block index matching, and support for 6-byte duplicate ACK with suffix `0xaa` (`0xfcca..0xfce4`) are correctly implemented.
- **Queue draining (drain_ack)**: Transition to `drain_ack` state with $S_1 = 3$ and no-op payload `b'\0'` ensures that the next payload packet will not acknowledge itself before the host reads the response.
- **SPI Layout**: SPI offset formula for 24-byte blocks on a 256-byte page (`floor(j/10)*256 + (j%10)*24`) fully matches code in `0xf7d8` and `0xd130`.

## K04687
Source `D0169`, original line40, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 2.3. Radio compatibility with Steam Frame (`disproven`)
- The claim of resolving the coexistence issue of trackers and controllers on the headset's radio grid is **disproven**.
- The patch changes the autonomous transmission timer constant from 18910 to 12000 µs when $N \neq 0$, but **does not disable or coordinate autonomous TX**.
- On the station polling grid with a step of 6000 µs (42000 µs cycle), the 12000 µs interval is a multiple ($12 \equiv 0 \pmod 6$), leading to inevitable periodic on-air collisions and interference with Valve controllers.

## K04688
Source `D0169`, original line45, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 2.4. Hardware activation and recovery (`insufficient data`)
- The repository lacks a dump of tracker Bootloader Flash memory.
- It is impossible to confirm the absence of digital signature verification (RSA/ECDSA) in the bootloader prior to flashing Bank 0.
- Dual-bank rollback or recovery mode upon failed application launch is not confirmed. In case of a hang or HardFault, a sealed tracker without an external USB port turns into an unrecoverable "brick".

## K04689
Source `D0169`, original line52, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. Detailed findings registry (Findings)

## K04690
Source `D0169`, original line54, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Finding B-1: Deterministic collisions of autonomous TX in multi-user configuration
- **Severity**: **Blocker**
- **Location**: `fw189-stock.bin` VA `0x6214`, `0x630a`; tracker_grid_patch.py (original reference: `[PRIVATE_SOURCE_REFERENCE]`)
- **Trigger and mechanism**:
  1. When $N \neq 0$ ($N=13$), native code in `0x6222` and `0x6318` sets `TIMER0 CC0 = 12000` µs.
  2. This timer initiates autonomous transmission of IMU samples.
  3. Roy MPSL-based station operates with a slot step of 6000 µs (cycle of 7 slots, 42 ms):
     - $t = 0$ ms: polling Peer 0
     - $t = 6$ ms: polling Peer 1
     - $t = 12$ ms: polling Peer 2
     - $t = 18, 24, 30, 36$ ms: reserved for two Valve controllers.
  4. Since the autonomous transmission interval is 12 ms ($12 = 2 \times 6$ ms):
     - **Collision 1**: Peer 0 polled at $t = 0$. Its autonomous TX will trigger at $t = 12$ ms. At that exact moment, the station polls Peer 2 on channel 2 $\implies$ **direct radio collision**.
     - **Collision 2**: Peer 1 polled at $t = 6$. Its autonomous TX will trigger at $t = 6 + 12 \times 3 = 42$ ms. At that moment, the station polls Peer 0 again $\implies$ **direct radio collision**.
     - **Interference with controllers**: At times $t = 18, 24, 30, 36$ ms, autonomous TX of trackers fall into "quiet" slots, causing latency spikes and freezes of Valve controllers for 200–484 ms.
- **Status**: Mathematical and algorithmic fact.
- **Recommendation**: Modifying 30 bytes does not solve the problem. It is necessary to either completely suppress launching `TIMER0 CC0` when $N \neq 0$ (NOP CC0 write branch), or synchronize the timer with the station.

## K04691
Source `D0169`, original line74, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Finding B-2: Tracker response duration exceeds slot limit (400 µs)
- **Severity**: **Major**
- **Location**: roy_ram_station_v4.c (original reference: `[PRIVATE_SOURCE_REFERENCE]`); section 7.3 of document
- **Trigger and mechanism**:
  HIGH-slot allocates 600 µs, of which useful reception is bounded by threshold 400 µs (`steady_slot_us - end_margin_us = 600 - 200`). When transmitting multiple IMU samples or command queue, total on-air time including preamble, address, S1, payload, CRC, hardware ramp-up, and turnaround reaches 227–528 µs. Packets completing after 400 µs are truncated or delay return of radio to the controller stack.
- **Status**: Confirmed by logs of real radio captures.

## K04692
Source `D0169`, original line83, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Finding B-3: Synchronization breakdown upon reconnection due to unchanged version number (FW189)
- **Severity**: **Major**
- **Location**: station_v4_runtime.py (original reference: `[PRIVATE_SOURCE_REFERENCE]`); tracker_ota_runtime.py (original reference: `[PRIVATE_SOURCE_REFERENCE]`)
- **Trigger and mechanism**:
  The firmware version remains 189. Stock requires $N=9$ (period of 4727.5 µs), while the patch requires $N=13$ (period of 3000 µs). If the tracker reconnects to the station without a predefined `tracker_poll_decimation_by_id` table, the host configures it with the default $N=9$. The patched tracker will open the reception window at $\operatorname{int}(9 \times 3000 + 1810.5) = 28810$ µs and close it by 30 ms, while the station poll will only arrive at 42 ms. A complete loss of connection occurs.
- **Status**: Fact. A software indicator to distinguish the patched tracker is required.

## K04693
Source `D0169`, original line92, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Finding D-1: The `result.json` artifact hangs in `waiting_target` on error before OTA start
- **Severity**: **Major**
- **Location**: tracker_ota_runtime.py (original reference: `[PRIVATE_SOURCE_REFERENCE]`), L124 (original reference: `[PRIVATE_SOURCE_REFERENCE]`)
- **Trigger and mechanism**:
  In `__init__`, `result.json` is created with the `waiting_target` status. If the tracker fails to connect within 120 s or reports an unsupported version, `poll()` raises `RuntimeError`. Since `self.ota` is `None`, `_write_status()` is not called, and `result.json` permanently remains with the `waiting_target` status. On a subsequent call, `load_job()` terminates with `ValueError: OTA result already exists; do not replay a completed/interrupted job`, blocking operation without saving the failure cause.
- **Recommendation**: Add exception handling prior to OTA start, saving the error to `result.json`.

## K04694
Source `D0169`, original line101, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Finding D-2: Risk of MCU watchdog timeout due to synchronous disk I/O
- **Severity**: **Minor**
- **Location**: tracker_ota_runtime.py (original reference: `[PRIVATE_SOURCE_REFERENCE]`), L119-L120 (original reference: `[PRIVATE_SOURCE_REFERENCE]`)
- **Trigger and mechanism**:
  On each of the 4023 blocks, atomic rewriting of the `.tmp` file and appending to `.events.jsonl` are performed. On the Steam Frame internal memory (eMMC), disk delays may exceed 1 second, which, given an MCU heartbeat transmission delay of 6 seconds, will cause an emergency shutdown of the service.
- **Recommendation**: Flush status to disk in batches (once every 50–100 blocks or at an interval of 2–3 seconds).

## K04695
Source `D0169`, original line110, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Finding C-1: Absence of Bootloader dump and recovery guarantees
- **Severity**: **Blocker (for hardware flashing)**
- **Location**: Section 9 and Appendix G of the document
- **Trigger and mechanism**:
  Following successful writing to SPI flash, the native `0xfa98` code writes an activation marker to flash `0x2f000` and performs a reset. Further actions are performed by the Bootloader, whose code is absent from the project. It is unknown whether it verifies RSA/ECDSA signatures, how it handles power failures during copying to internal flash, and whether an emergency recovery mode exists.
- **Recommendation**: Prohibit flashing real devices until a Bootloader dump is obtained.

## K04696
Source `D0169`, original line119, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 4. List of missing input data

## K04697
Source `D0169`, original line121, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Tracker Bootloader memory dump (`0x00000000..0x00001000` and upper region)**: Required to prove the absence of digital signature verification and clarify rollback logic on firmware crash.
2. **Hardware radio capture (RF Capture)**: Required to measure the actual behavior of tracker `TIMER0` during packet reception (reset vs autonomous TX overlap).
3. **SWD pinout and tracker NVMC dump**: Required to ensure wired recovery of the test unit before the first experimental OTA flashing.

## K04698
Source `D0169`, original line127, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

*The report was prepared in the strict mode of an independent technical audit. The original file `FW189_GRID3000_INDEPENDENT_VALIDATION.md` was left unchanged.*

## K04699
Source `D0170`, original line1, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Independent validation of the PICO FW189 patch for Steam Frame radio

## K04700
Source `D0170`, original line3, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-07. Status: **experimental candidate, prepared offline; not flashed and not approved for hardware launch**.

## K04701
Source `D0170`, original line5, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This document is intended for transfer to independent reviewers, including other neural networks. It contains context, precise edits, sender design, verification boundaries, and questions that could refute the chosen solution. The reviewer's task is to find specific errors and missing evidence, rather than confirming the author's conclusion based on the number of passing tests.

## K04702
Source `D0170`, original line7, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The user requested a pause after writing. **Do not run SSH/SWD/RF/OTA, modify services, or flash devices within the scope of this review.** Source code reading, calculations, and offline models are permitted. This document does not constitute permission for hardware launch.

## K04703
Source `D0170`, original line9, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 1. How to use the document

## K04704
Source `D0170`, original line11, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

If only this MD is available, start with sections 2–12, then examine the embedded snapshots of source code and disassembly in the appendices. Do not assume access to the author's local paths. When an answer requires a missing bootloader, RF trace, or hardware, explicitly state this deficiency.

## K04705
Source `D0170`, original line13, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

When accessing the repository, the project root is `K:\AI\pico`. First read its `AGENTS.md`. The primary log is `docs/EXECUTION_STATUS_2026-10-06.md`. Precise source snapshots in the appendices take priority over modified files from a future session.

## K04706
Source `D0170`, original line15, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Three separate subjects of review:

## K04707
Source `D0170`, original line17, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Correctness of the binary patch:** proper locations, encoding, values, completeness of dependent constants, absence of unwanted changes.
2. **Correctness of OTA:** byte protocol, ACK, packet retries, SPI staging, CRC, activation, and recovery.
3. **Efficiency over radio:** not only the 3000 µs period, but also phase, autonomous TX, RX availability, response length, and coexistence of three trackers with controllers.

## K04708
Source `D0170`, original line21, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Do not combine these three verdicts. A correct 30 bytes does not prove safe OTA or a compatibility fix.

## K04709
Source `D0170`, original line23, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2. Project goal and constraints

## K04710
Source `D0170`, original line25, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The goal is autonomous tracking of three PICO Motion Trackers by the Steam Frame headset using onboard Nordic Roy, without an external dongle and without a permanent PICO/Windows pose bridge.

## K04711
Source `D0170`, original line27, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Roy retains stock flash and normal operation of two Valve controllers. The user previously permitted a temporary SRAM/SWD module operating within granted MPSL timeslots. The user has now separately permitted the preparation of a small **tracker firmware** patch. This is not permission to modify Roy's flash.

## K04712
Source `D0170`, original line29, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The patch must preserve native code for IMU, LED, camera synchronization, commands, pairing, and boot vectors. "Code is unchanged" and "dynamic behavior is preserved" are different assertions: the new cadence may alter IMU batching, queues, power consumption, and timeliness of command processing without modifying driver instructions.

## K04713
Source `D0170`, original line31, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. Prior observations and their reliability

## K04714
Source `D0170`, original line33, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The following hardware details are taken from the project log and existing receiver sources, rather than re-measured during patch preparation:

## K04715
Source `D0170`, original line35, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Roy is identified as nRF52833; stock application build `1788551713`, Zephyr/MPSL/BLE controller stack.
- Two controller links operate on a measured grid of approximately 3000 µs. In the phase-locking code, this is `PERIOD_S=3000e-6`, RTC 32768 Hz.
- In one set of observations, controller radio occupied roughly phases 500–1200 and 2200–3100 µs; the quiet region was approximately 1300–2200 µs. This is a historical measurement in relative phase, not universal absolute times for all boot/connection states.
- Roy MPSL did not provide a stable chain of slots with an interval under approximately 6 ms. Therefore, the chosen station slot is every **6000 µs**, rather than every 3000 µs.
- The HIGH timeslot takes 600 µs; custom RF must complete by 400 µs after the start of the slot, leaving 200 µs to return the radio to the controllers.
- There were periods of clean coexistence lasting several minutes, but there were also controller freezes of 200–484 ms. Therefore, 600/6000 µs cannot be considered a universally hardware-certified mode.
- With stock FW189, reply ratios were approximately 3–30%; responses arrived in bursts with long pauses. Other saved measurements with short RX showed around 7% intercepted responses. These numbers pertain to different modes and do not form a single precise loss probability.
- The duration/end of a received response varied approximately 227–528 µs from the start of the slot. Responses ending after 400 µs no longer fit into the current RX regardless of period matching.
- `MODECNF0=0x201` should be preserved: writing 1 previously disrupted the controllers until Roy rebooted.

## K04716
Source `D0170`, original line45, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**The new candidate has not been tested on hardware.** Neither an increased reply ratio, nor the absence of controller freezes, nor successful boot, nor full FBT are claimed by this document.

## K04717
Source `D0170`, original line47, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The exact chip/flash layout of the tracker should not be automatically equated to Roy. The stack pointer `0x20006000` and metadata page `0x2f000` do not in themselves prove the MCU model or the absence of bootloader security.

## K04718
Source `D0170`, original line49, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 4. Identity of inputs and outputs

## K04719
Source `D0170`, original line51, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Object | Size | SHA256 |
|---|---:|---|
| Vendor container `Swift2.0_sv1.89_20241213_b189_app.bin` | 193385 | `41bd2d24dd9293e7d17c62ab18acd1001d6085da393123a67d226b28d2132e54` |
| Raw stock application image | 96532 (`0x17914`) | `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89` |
| Raw patched application image | 96532 | `6a82a89206a609fdce92a3de764882ce53f79f0360b925bebe1ce7853cde6a0d` |

## K04720
Source `D0170`, original line57, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Verified container: header of 321 bytes (`0x141`), followed by two identical copies of the raw image of 96532 bytes each. Both match the separate extracted stock image. The signed container was not rewritten and no new RSA signature was created.
