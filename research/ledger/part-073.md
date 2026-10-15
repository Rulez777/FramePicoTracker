# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05761
Source `D0362`, original line20, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Parameters: service mode, 4727 µs requested interval, 900 µs slot, RX deadline 800 µs, slot_cycle=1, N=0, C=2, command16 fresh sequence3, controllers_off guard, remaining peers paused, BLE phase lock turned off, optics turned off. Optical worker, cameras, command14, and phase-window gating were not run.

## K05762
Source `D0362`, original line22, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The probe waits 20 seconds from the first mailbox publication. After configuration completes, it queues command10 approximately every 0,8 s if there is no pending command. The time reference is the read DWT CYCCNT (64MHz); the corresponding 16MHz tick is calculated relative to an arbitrary origin modulo 533333. Using the existing ABI4 bit16, the MCU recalculates the tick before transmission and updates the checksum. This is an arbitrary 30Hz time grid, not the measured exposure phase of PICO/Frame. For delays up to 60 s, the DWT difference is correct modulo 32; usual timeout/retry and seqlock remain active.

## K05763
Source `D0362`, original line24, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The log now contains all _begin calls for 16/17/25/19/10, transport ACKs, and additional response records. It is not a complete trace of all actual TXs, and recording the observed RX does not bypass the standard identity check.

## K05764
Source `D0362`, original line26, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Confirmed sequence

## K05765
Source `D0362`, original line28, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

16 [0002] seq3 -> 17 [07] seq0 -> 25 [02] seq0 -> 19 [00] seq0. Queuing time 2130,127 / 2130,255 / 2130,347 / 2130,446 by monotonic. All four received transport ACK before 2130,551. The first command10 was queued at 2149,283, its ACK at 2149,387. Total of 15 attempts of command10,14 transport ACK.

## K05766
Source `D0362`, original line30, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The engine transitioned to streaming after transport ACK of command10 and real IMU samples. In this diagnostic mode, this is an internal transport state; pose_valid and poses_valid remained false. It does NOT mean optical synchronization, radio RX phase lock, or 6DoF.

## K05767
Source `D0362`, original line32, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Command22 was observed 89 times in the decoded-record log, including 63 prior to the first command10. Consequently, the mere appearance of command22 cannot be considered its causal confirmation. The transport ACK response also does not prove that the firmware processed every command10: FW189 handler0x8df0 has guard0x59cc and additional conditions in 0x79b8.

## K05768
Source `D0362`, original line34, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Clarification of the meaning of command 10

## K05769
Source `D0362`, original line36, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the previous report 22, the phrase 'phase synchronization' might have created the impression of correcting the radio rhythm. According to FW189 0x8df0 -> 0x79b8, the command adjusts a separate synchronization timer (LED/camera subsystem), uses ADDRESS capture and an offset of 467 TIMER1 ticks. There is no proof that it realigns the RX cadence. A positive transport ACK is sufficient for the old host phase_verified, but insufficient for an assertion about the physical phase.

## K05770
Source `D0362`, original line38, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This run tests the narrow hypothesis that 'the completion of command10 alone will noticeably raise the reception percentage.' The observed section does not support it. It does not test the complete native PICO camera/TIMER4 reference chain.

## K05771
Source `D0362`, original line40, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Assert and recovery

## K05772
Source `D0362`, original line42, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Stock assert r0=106,r1=501,PC=0x13e54,LR=0x10507,CFSR/HFSR=0. It matches the one observed in optics-on test20. Here optics_enabled=false and the optical worker is not created: cameras are not a necessary condition for reproduction. One cannot declare command10, its MCU retiming, host latency, or overruns as the root cause from a single run — this requires checking the scheduler/callback.

## K05773
Source `D0362`, original line44, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The existing check_fault stopped the test. session_closed_verified,ram_restored_verified,mpu_restored,trace_controls_restored_verified have been confirmed. The host runner restored the module, config, and saved the combat ELF. After the session: radio service inactive, no loader/OpenOCD processes. The active ELF is again SHA8f784fc334c1e24a455513ff5565385b63a309427994b10d89ed20b7ceca44e4; config SHA2e2cf170eb7ea53e840673d61cbf56075997b7d8393289d7ec9784572bbf1b90. The diagnostic module does not start automatically/persistently. FW189 and the OTA gate are unchanged.

## K05774
Source `D0362`, original line46, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Checks and next step

## K05775
Source `D0362`, original line48, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

13 peer-engine/measurement-clock tests passed. A separate test_native_phase.py executes the real compiled callback in Unicorn: command10 body recalculated116 ticks out of100 and64 DWT cycles, checksum is correct, identity and capture are preserved. test_probe.py checks baseline delay, absence of command10 up to20s, ABI4 bit16 without windows, ACK and retry. One initial test check at exactly0,8 seconds hit a float boundary; check was performed at0,81s, schedule code was not changed.

## K05776
Source `D0362`, original line50, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The next step is an offline analysis of assert106/501 and callback/MPSL lifecycle timing. Until the error is resolved, the service mode cannot be declared stable or a long/persistent run repeated. RX is still ~11,6%; the firmware patch has not become any more ready for OTA.

## K05777
Source `D0362`, original line52, SHA256 `6c9856a00e3f9d241f8900fb4f84ae98410174494b10d6a61f16e1a1cb310e70`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Artifacts: outputs/service-fast-poll23/phase_probe.py, prepare.py, run.py, test_probe.py, test_native_phase.py, analyze.py; the complete saved service-measure23 with raw records, command log, fault snapshot, recovery checkpoint, and before-run backups. manifest.json records SHA256 of the files. The documents contain a public tracker ID; keep raw backups locally.

## K05778
Source `D0381`, original line82, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Continuation of tracker firmware modification

## K05779
Source `D0381`, original line84, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Last iteration: FW189_ITERATION_02_2026-10-07.md; RC1 snapshot and145tests.
The new SRAM deadline ELF was built locally2040/2045, not uploaded.
Native pending-OTA race after empty queue is confirmed. Hardware pause
remains in effect; RC1 approved_for_ota=false.

## K05780
Source `D0381`, original line89, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Current clarification: read FW189_OFFLINE_REVISION_2026-10-07.md. Host code
has been refined, 104 tests; queued ACK freshness, completeness of the timer patch, and recovery
remain unaddressed. The old words "preparation complete" below are historical
status, not permission for a hardware run. The pause remains in effect.

## K05781
Source `D0381`, original line94, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Current stopping point: preparation complete, pause

## K05782
Source `D0381`, original line96, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The user clarified the patch: only radio FW189 to a 3000µs grid; preserve
native IMU/LED/cameras/commands. Then requested a pause after writing.
**Do not upload to Frame, do not run RF/OTA/services until their signal.**
The timeline and full results are now recorded at the top of
`EXECUTION_STATUS_2026-10-06.md`; this document serves as the continuation point.

## K05783
Source `D0381`, original line102, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Ready local artifacts

## K05784
Source `D0381`, original line104, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- `outputs/tracker-grid3000-20261007/fw189-valve-grid-3000.bin`:96532 bytes,
 30 modified bytes in10 places; SHA256
 `6a82a89206a609fdce92a3de764882ce53f79f0360b925bebe1ce7853cde6a0d`.
- Nearby frozen `fw189-stock.bin`, `patch-manifest.json` with exact addresses/
 bytes/hash and `ota-wire-example.json` (candidate CRC0xe6,4023 blocks).
- `pc/radio/tracker_grid_patch.py`: reproducible patch/reverse, exact
 hash/byte guards; signed vendor container is not modified. Reverse byte-for-byte
 restores stock. Native version remains189.
- `tracker_ota.py`: native command1/subcommand0x25, start/data/finish,
 application ACK, CRC8/MAXIM,24-byte blocks, BE16 indices with1 and sentinel
 for a multiple of24 size. ACK-only drain is separated from the next block so that
 repeated S1 bit0 does not delete the new ACK before reading by host. Bounded timeouts;
 transport ACK is insufficient, auto-resume after disconnect is forbidden.
- `ram/tracker_ota_runtime.py` + explicit loader `--tracker-ota-job`: one
 verified registry identity/slot, other peers paused, optics off,
 HIGH600/6000µs, RX end400µs, phase lock, two controllers, freeze≤150ms,
 ownership/heartbeat/cleanup.40s MCU timeout only for SPI erase/CRC.
- `tracker_ota_prepare.py`: separate one-shot job/config with image/hash/
 manifest/result; original station config is preserved, hardware is not started.
- 79 focused tests passed: real ARM TX/RX branches, native subcommand
 dispatch and full-size96532 SPI staging/whole-image CRC in Unicorn;
 boundaries24/240, corruptions, ACK/timeout, adapter/peer/ABI4/controller guard.
 SPI/RF side effects replaced with a model: bootloader/timing/over-the-air were not tested.

## K05785
Source `D0381`, original line128, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Clarifications superseding old conclusions below

## K05786
Source `D0381`, original line130, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

OTA staging is **external SPI memory**,0xd130/0xd1f0,10×24bytes per256-byte
page; start erase4×64KB.0xf7d8 checks the CRC of the entire image. Finish ACK
confirms staging CRC. Metadata in0x2f000 and activation is a separate native path
0xfa98; bootloader and recovery are not verified. Finish should not be considered a harmless
download: native disconnect/power event may activate the update.

## K05787
Source `D0381`, original line136, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

RX miss formula is indeed `(N+1)*period - 25`, not `+25`; native
compensation−25 is preserved. RX offsets are shifted by1727.5µs, integer hunt
is rounded up0.5µs. Matching period does not prove phase or coexistence.

## K05788
Source `D0381`, original line140, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Next steps after user signal

## K05789
Source `D0381`, original line142, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Verify Frame/Roy/registry and activation/recovery for one selected
 tracker. Having a stock file does not guarantee recovery from any state.
2. Transfer current `pc/radio/` sources and bundle to a separate candidate
 directory on Frame; compare existing SRAM ABI4 module/tag/hash. SRAM C has not changed.
 Preserve old release/persistent service and flash Roy.
3. Create a fresh one-shot job from the **real private config/seed**. Example
 command format (replace SLOT/CHIP_ID_LE with verified ones, PYTHONPATH=candidate pc):

## K05790
Source `D0381`, original line154, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is only file preparation. For rollback, a new job with `--action stock`
 and `--source-grid-us 3000` is needed, provided the native OTA path remains functional.
4. Explicit one-shot invocation guarded loader: existing ABI4 flags from
 `pc/frame_app/frame-pico-radio.service`, add `--tracker-ota-job`
 `FRESH_JOB/job.json`, use `FRESH_JOB/station-config.json` and
 `--slot-us 600`. Do not run a persistent service with job.
5. Verify application finish ACK, activation/boot, measured rhythm/phase/RF
 and controllers. Version189 by itself does not confirm patch installation.
6. For normal42ms polls: patched peer N13, stock peer former N9.
 Explicit `tracker_poll_decimation_by_id` allows a mixed set, without
 automatic assumptions based on the unchanged version field. Separately
 verify optics/FBT; OTA does not mean ready full body tracking.

## K05791
Source `D0381`, original line167, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Historical snapshot of the beginning of continuation (prior to preparation)

## K05792
Source `D0381`, original line169, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

User request: modify the PICO tracker firmware and transfer it via SSH
to Steam Frame, then via onboard Roy over radio to the tracker. Modifying flash Roy
is not required for this. The exact change in tracker behavior is currently being clarified.

## K05793
Source `D0381`, original line173, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Verified in this session

## K05794
Source `D0381`, original line175, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- SSH `steamos@[DEVICE_IP]` responds, hostname `frame`. RF was not started.
- Ready-made OTA sender/state machine not found in current `pc/radio/`.
- `pc/radio/tracker_image_audit.py` compares the saved container with the extracted
  image without modifying firmware files. Result: `outputs/tracker-image-audit-20261007.json`.
- Container 193385 bytes: header 321 and two identical copies of 96532 bytes each.
  Both matched `outputs/swift189-image.bin`, base 0x1000, SP 0x20006000,
  reset 0x13a5. For 24-byte blocks: 4023 blocks, the last 4 bytes.
- Table at VA 0x16200 completely matched CRC-8/MAXIM (init 0,
  reflected polynomial 0x8c); check vector `123456789` = 0xa1.
- Handler 0xfbcc: type 1 start, type 2 data, type 3 finish.
  Start reads BE32 at +1, limits size to 24 bits, separate byte +5
  and BE16 at +6. The semantics of the last fields are not yet established.
  Data reads BE16 at +1, byte +3 and 24 bytes at +4.
  ACK is formed in 0xfa30..0xfa72; transport ACK match is insufficient.
- 0xfa98..0xfafe writes 24 bytes of metadata into 0x2f000 and compares the CRC of these
  24 bytes, rather than the entire page. Magic depends on function argument.

## K05795
Source `D0381`, original line192, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What cannot be considered proven

## K05796
Source `D0381`, original line194, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`outputs/tracker-ota-gemini.md` contains unconfirmed conclusions: address of the second
bank above 0x30000, absence of checks in bootloader, guaranteed safe
interruption, and guaranteed launch of modified image. They are not confirmed
by a single application image disassembly alone. Direct arithmetic of free space does not determine
the real update map. Until the bootloader is analyzed, do not use these conclusions
as a basis for launching OTA.

## K05797
Source `D0381`, original line201, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Next dependent steps

## K05798
Source `D0381`, original line203, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Fix the exact desired RX/TX schedule patch, expected original bytes,
   SHA of original image, and rollback method to stock on one selected tracker.
2. Trace the complete path of command 1/subcommand 0x25 to 0xfbcc, writer 0xd130,
   reader 0xd1f0, staging addresses, and bootloader activation. Establish values
   of start fields, index of first/last block, padding, and application ACK.
3. Create a separate modified image, manifest, and diff, leaving stock untouched.
4. Implement offline packet builders and boundary tests, then sender with bounded
   retry, ACK for a specific block, controller guard, and prohibiting finish on error.
   Resumption after disconnect must not be enabled until tracker behavior is confirmed.
5. Prepare one consistent build on Frame and only then test OTA
   on one tracker; save the transmission log and check version/behavior after
   reboot. Successful OTA by itself does not mean ready full body tracking.

## K05799
Source `D0381`, original line216, SHA256 `adb1a3f800af53ca11b18c278012c26d38c8ee905252639ab23db40f934c3bd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The latest RF and optical results remain in `EXECUTION_STATUS_2026-10-06.md`.

## K05800
Source `D0387`, original line186, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Current status of PICO Motion Tracker → Steam Frame

## K05801
Source `D0387`, original line188, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Updated: **2026-10-06, Europe/Warsaw**, after investigation 180.
This document contains the current summary. Old sections of the investigation are a measurement
log; their former conclusions do not replace this summary.

## K05802
Source `D0387`, original line192, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Working standalone 6DoF has not yet been achieved.** Real PICO radio packets have been received
on the built-in Nordic Frame, sensors have been decoded, the FramePicoTracker application
and remote camera recording along with radio diagnostics have been implemented.
A persistent standalone Frame ↔ tracker connection does not exist yet.

## K05803
Source `D0387`, original line197, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Requirements and boundaries

## K05804
Source `D0387`, original line199, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- The end result runs on the Frame itself, without a dongle, Windows, or permanent PICO headset.
- Flash, stock firmware of Frame/Roy/trackers are not modified.
- The user permitted a temporary SRAM module and investigation via SWD.
- Valve stock controllers must remain functional. This is verified by live
  poses during the test, not by the presence of recording files after it.
- PICO is used for research and reference communication. A pose bridge from PICO
  was rejected by the user even for a temporary demo.

## K05805
Source `D0387`, original line207, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Access to two headsets

## K05806
Source `D0387`, original line209, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Device | Access and limitations |
|---|---|
| Steam Frame | `ssh steamos@[DEVICE_IP]`, no password; Wi-Fi operation confirmed. Directory `/home/steamos/pico-ram-work`. SSH user is not root, sudo requires password. USB ADB `frame` was available earlier; TCP ADB is not. |
| PICO 4 Enterprise | USB ADB `[HEADSET_ADB_SERIAL]`, A8110, Android 10, 5.13.7. Regular shell uid=2000; root separately confirmed via existing `/sdcard/hashsu.sh` (uid=0, context pvrtrackingservice). This is not `adb root`. |

## K05807
Source `D0387`, original line214, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`outputs/pico-tracker-info-root.json` and
`outputs/pico-calibration-root.json` were saved via PICO root. Android root does not prove access to Nordic
station RAM: debug command for reading MCU via stationdev was not found.
In the latest check PICO is Awake/ON, the first tracker is online, the other two are offline.
This is a state snapshot, not a permanent guarantee of availability.

## K05808
Source `D0387`, original line220, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What has been implemented

## K05809
Source `D0387`, original line222, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Temporary Roy receiver in SRAM.** Native shell → MPSL; MPU remains enabled.
   Code is loaded for verified stock build 1788551713. Radio is used
   only in dedicated slots. Session closing, RAM, and MPU are verified on exit.
   Diagnostic sessions are limited to 10 seconds; this is a loader limit.
2. **Reception with CRC.** Full profile has been hardware-confirmed for the first tracker:
   `BASE0=[PRIVATE_LINK_VALUE]`, `PREFIX0=0`, `MODE=4`, `PCNF0=0x40008`,
   `PCNF1=0x01040096`, CRC16 `poly=0x11021`, `init=0xFFFF`.
   CRC-checked frames include IMU, magnetometer, and other/null frames.
3. **Passive channel scan.** A separate `ROY_SWIFT_SWEEP` visits even
   channels 2..80 inside SRAM. Module tag and explicit loader flag are required.
   This is channel investigation, not a reconstructed hopping sequence.
4. **Sensor parser.** Radio format is separated from SLPI V2. Gyroscope, acceleration,
   magnetometer, temperature, and counter are decoded without outputting dummy poses.
   CSV/JSON contain SWD host read time, which does not equal RF reception time.
5. **FramePicoTracker on headset.** Native ARM64 OpenVR dashboard overlay `PICO`.
   Launch via `+ → FramePicoTracker`; binary
   `/home/steamos/.local/bin/FramePicoTracker`. UI shows completed IR frame
   and direction of measured acceleration in sensor axes. This is not live video,
   not orientation in SteamVR space, and not axes bound to the tracker.
6. **Direct remote access.** Diagnostic HTTP API listens only on
   `127.0.0.1:18741` on Frame; client works via SSH. Record button starts
   cameras and passive RF test, stops cameras, collects archive with OCC,
   calibration, Frame poses, radio log, and IMU CSV/JSON. There are no arbitrary shell/TX
   commands via API. External SWD processes must be run sequentially.
7. **Cameras.** HMD commands
   `start_tracking_recording 1 0 0` and `stop_tracking_recording` are confirmed.
   Cameras are recorded without screen and audio. OCC is unpacked into real H.264
   streams: two 1056×1024 and two 640×480 IR streams plus SLAM images.
8. **Optical data and prototypes.** Factory Frame camera calibrations
   and coordinates of eight LEDs by tracker serial number are extracted. There is IMU filter code,
   camera geometry, and a SteamVR driver prototype. They are not yet connected into
   a verified stream of real 6DoF poses. Driver is not registered/not loaded.

## K05810
Source `D0387`, original line255, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Latest hardware results

## K05811
Source `D0387`, original line257, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Test | Result and conclusion limit |
|---|---|
| 155–156 | Full profile of the first tracker: 7 and 19 CRC frames; both stock controller poses were present during these tests. |
| 157 | Bounded TX probe: 40 transmissions completed in hardware. Delivery of LED enable command and full ID match are unconfirmed. |
| 167 | 19 CRC frames, 15 gyro/accel, and one magnetometer. Real motion data, not continuous orientation. |
| 169 | 30 CRC frames, 27 gyro/accel, maximum read interval 3,353 s. Camera 65,7 MB. |
| 170 | 30 CRC frames, 18 gyro/accel, maximum interval 3,249 s. All four IR streams parsed into 242 frames each; Motion Tracker markers not identified. Automatic preview was not created, although manual OCC decode succeeded. |
| 171 | Sweep: 1021 slot, two CRC frames — magnetometer on 34 and null frame on 28. |
| 172 | Fixed 80: 1019 slots, zero frames. |
| 173 | Fixed 34, interval 10007 µs: 1018 slots, seven CRC frames, two of them magnetometer; no gyro/accel. |
| 175 | Active empty poll probe: 121 slot, 40 TXEND, zero response frames. Recovery confirmed; poll delivery not proven. |
| 176 | PICO Asleep/OFF: empty poll, 124 slots, 40 TXEND, zero responses. Sleep does not prove PICO radio station shutdown. |
| 177 | Full three-byte header with opcode0: 121 slot, 40 TXEND, zero responses. Neither settings nor pairing commands. |
| 178 | Passive bootstrap BASE552C6A1E/prefixC0, channels1/37/77:1021slot, zero frames. Recovery confirmed. |
| 179 | Paired initial channel78:1020slots, zero frames; all restored. PICO host reports currentFreq1, actualTrackerFrequency1 — mode enum, not Hz. |
| 180 | ABI GetTrackerWorkMode/SetTrackerWorkMode read; read-only helper on PICO returned-1. Service log: mcu send fail,ret0. Setter was not called. |

## K05812
Source `D0387`, original line274, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In 169–173 SRAM/MPU/session restoration is confirmed; flash_written=false.
In 171–173 OpenVR poll showed only HMD: concurrent operation with awake
controllers was not tested in these runs. Controller pose files in the dataset
do not by themselves confirm valid poses.

## K05813
Source `D0387`, original line279, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Archives and outputs: `outputs/motion-check169.tar.gz`, `motion-check170.tar.gz`,
`sweep171.json`, `sweep171-decoded.json`, `fixed172.json`,
`channel34-173.json`, `channel34-173-decoded.json`.

## K05814
Source `D0387`, original line283, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Identifiers and corrections to former statements

## K05815
Source `D0387`, original line285, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Physically, low-order bytes `3E ED`, `C3 BF`, `AD 1D` were observed:
  words `ED3E`, **BFC3**, `1DAD`. `DFC3` is an error.
- Store station seed as bytes `74 73 54 15`; LE word `0x15547374`.
  The number `[PRIVATE_LINK_VALUE]` without specifying byte order is ambiguous.
- Full FICR IDs were not read directly. The values `EEACED3E/EEADDFC3/EEAE1DAD`
  were a hypothesis; they cannot be used as confirmed IDs.
- Reverse calculation from the fully confirmed RF BASE yields a tentative candidate
  of the first ID `[TRACKER_ID]` (bytes `[TRACKER_ID]`). A match of the low word
  does not replace direct verification of FICR or an addressed response.
- `tracker_info_data.json` contains slots, SN, software, bond_status, but not RF keys.
  The string `1233` is not a hardware radio address.
- Sync word `0x71764129` refers to DTM; operational exchange uses ESB-like
  framing. The stock Valve BLE/MPSL radio stack is not proven to be "Valve ESB".
- The Swift command table associates IR-handler `8D88` with opcode **09**;
  the earlier claim about opcode 1A is not confirmed by this table.
- `595C(2)` sets RXADDRESSES mask pipe1, **not TX pipe2**. Callback
  `5B0E..5B12` selects TXADDRESS by RXMATCH, response is triggered before command parser.
  Detailed evidence and limits: swift-station-cycle-2026-10-06.md (original reference: `swift-station-cycle-2026-10-06.md`).
- Channel switching is confirmed by code, but the live mask of all 40 channels and
  a hop on each IMU packet are not proven. `6E20..6E38` has threshold
  `200/(config_byte+1)` and a counter before channel transition.
- A separate fallback/bootstrap path `75CC` uses **pipe0/prefixC0** and
  a table of three channels **1,37,77** (ROM15FDC). Paired initial path `7574`
  uses pipe1 and ROM15FE0 **4,78,78**. This is not the operational mask2..80.
  In init6658..672A both tables are copied to RAM, BASE0=RBIT32(785634AA)
  =552C6A1E. The current mode of the live tracker is not directly confirmed.
- SLPI quaternion is not proven in observed RF packets. The IMU filter must not
  be fed an invented dt=5 ms with gaps of several seconds.
- Long exposure increases the chance of seeing a flash, but does not guarantee it.
  Bright spots of the window/controllers are not a proven tracker constellation.

## K05816
Source `D0387`, original line316, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What else is needed

## K05817
Source `D0387`, original line318, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Available data and source code

## K05818
Source `D0387`, original line320, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Three SNs: `[TRACKER_SERIAL]`, `[TRACKER_SERIAL]`, `[TRACKER_SERIAL]`.
  Root JSON contains version `TR018903`; individual host APIs show `TR0189`.
- Tracker factory block: 660 bytes, eight LED XYZ at offset 212..307.
  There are old entries in the file: select by SN, not by array index.
- Radio IMU: gyro scale ≈0,001065264 rad/s/count; accel 1024 count/g
  is consistent with rest observation, full calibration pipeline remains to be verified.
  Temperature `25 + raw/132,48`. Magnetometer is unsigned centered at 32768.
- Native RX sources: `pc/radio/ram/roy_ram_rx.c`, `roy_ram_loader.py`,
  `roy_passive_probe.py`. Code SRAM `0x2001151C`, data `0x20010C18`;
  executable MPU region2 is reserved, stock stack guard is moved to region3.
  Four ring entries; actual END fallback saves frame upon slot
  completion because stock IRQ does not always pass RADIO callback to module.
- Diagnostics: `pc/radio/frame_debug_agent.py`, `frame_debug_client.py`,
  `frame_telemetry_trace.py`, `frame_occ_tail.py`, `frame_ir_scan.py`.
- UI: `pc/frame_app/main.cpp`, README and MIT vendor overlay/texture transport.
  `outputs/frame-pico-vectors168.png` is a verified render UI, not an eye screenshot.
- Optics/IMU: `pc/optical/imu_orientation.py`, `kb_spline.py`,
  `steam_frame_tracker.py`; their presence and unit tests do not confirm exact
  factory camera projection, world alignment, or working 6DoF.
- Access to raw live cameras via OpenVR is not established. Stock
  XRService recording and subsequent decode are used; sleep/presence should be accounted for
  by actual frames and service state, not only by turned off screens.
- The last 24 loader/probe unit tests passed. They test code/constraints,
  while hardware results are given separately above.
- In `fusion_ekf.py`, replacement of invalid dt with5ms was removed. Undefined,
  negative, or >0,5s interval is now rejected before state change.
  8 optical/gap tests passed; this is a correctness fix, not ready tracking.
- A persistent Codex goal has been launched by direct user demand
  to continue until hardware-verified standalone6DoF. It is active and not
  completed. Backend agent-intern was not found in the current set of callable tools.

## K05819
Source `D0387`, original line351, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Restore RF schedule, RX/ACK windows, and live channel configuration;
   achieve frequent gyro/accel with measured arrival time.
2. Confirm full ID/addressed delivery and implement required station
   commands, ACK, LED turn-on and phase without PICO as a permanent station.
3. Identify eight markers on Frame, bind RF/camera clocks, and
   obtain optical position with correct geometry.
4. Combine optics and IMU, align to Frame world, output real valid
   poses via SteamVR driver; mark pose as invalid upon data loss.
5. Verify the entire pipeline without PICO and with both Valve controllers.

## K05820
Source `D0387`, original line361, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Repeating identical motion recordings is not currently required: the main blockers are
radio schedule, station command delivery, and optical synchronization.

## K05821
Source `D0395`, original line3, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Relevance — 2026-10-06, after test 173

## K05822
Source `D0395`, original line5, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Current summary: current-status.md (original reference: `K:/AI/pico/docs/current-status.md`).
The original plan/historical log is preserved below; assumptions about full ID,
ready 6DoF, frequency of 200 Hz, and need for root require verification against the summary.
Real CRC reception of PICO on Roy, sensor decoding, native FramePicoTracker UI,
and automated remote recording of cameras + RF are confirmed. PICO root is
confirmed via hashsu.sh. Standalone 6DoF and custom station/ACK/LED
synchronization loop are not yet implemented. Old "no RF received"/"no root" are obsolete.
Full FICRs are not confirmed; the low word of the second ID is BFC3, not DFC3;
seed 74 73 54 15 is LE 15547374. Tests171–173 confirmed channel34 for
magnetometer, but not a frequent gyro/accel stream. Flash was not modified.

## K05823
Source `D0398`, original line626, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

User explicitly confirmed immediate start ("let's go") after the pre-start pause.
Single run45 invoked; monitor existing attempt only, no replay/restart.
Outcome pending; do not infer completion or activation from invocation.

## K05824
Source `D0398`, original line1287, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Full project handoff
At the user's request, docs/HANDOFF_FULL_DEVELOPMENT_PLAN.md,2026-10-06 was created.
This is an inventory of evidence, defects, and stages A-F, not a declaration of readiness.
Read this document before continuing; do not use previous readiness messages
as evidence. UserprohibitsrepeatedshortRFprobes. Full6DoFincomplete.

## K05825
Source `D0480`, original line1, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Pico → stock Nordic Steam Frame: status 2026-10-05

## K05826
Source `D0480`, original line3, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Goal and actual result

## K05827
Source `D0480`, original line5, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Receive Pico Swift with the built-in Nordic/Roy of the Frame headset; implement the shim layer,
preserving firmware and operation of Valve controllers. An external dongle does not replace this result.
Pico reception via stock Roy is NOT yet implemented and NOT verified.

## K05828
Source `D0480`, original line9, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The plan from Desktop, the Antigravity study, and the attached hardware summary were read.
Claims in the documents regarding completed implementation were verified against the code:

## K05829
Source `D0480`, original line12, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Claim | What is in the code |
|---|---|
| Reception via on-board Nordic over SPI | Experimental CDC/serial only, no Roy SPI/HID backend |
| Ready native SteamVR driver | C++ skeleton: `RunFrame()` is empty, IPC receiver is absent |
| Roles/serial numbers registered via Python | Python stores them locally; OSC client does not transfer these properties to SteamVR |
| Over-the-air signature/CRC verification | Verification of assumed decoded payload; no address, channel, or over-the-air CRC |
| 0,42 mm on Frame | Synthetic benchmark; real accuracy is not measured |
| Tests confirm radio operation | Synthetic data tests; no recorded real RF/Roy traffic |

## K05830
Source `D0480`, original line21, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

After clarifying USB serial `frame`, access to Frame was confirmed. An immutable
`/system/etc/trackerbin/Swift2.0_sv1.89_20241213_b189_app.bin`, 193385 bytes,
SHA256 `41bd2d24dd9293e7d17c62ab18acd1001d6085da393123a67d226b28d2132e54` was read from Pico.
This is a copy of the shipped firmware, not a flash dump of the connected tracker.

## K05831
Source `D0480`, original line26, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Where the shim layer should be located

## K05832
Source `D0480`, original line33, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The first arrow cannot be replaced by a Python decoder. Nordic RADIO processes
PHY, addressing, PCNF, and CRC before passing data to the host. In order for the existing firmware
to receive Pico, a confirmed mechanism for configuring the RX profile and outputting data
or an already implemented compatible profile is needed. For concurrent Valve/Pico reception,
a mechanism for shared radio servicing inside the stock firmware is also needed.
Direct presence of SPI does not give host access to Nordic RADIO registers.

## K05833
Source `D0480`, original line40, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

However, the absence of an interface is not yet proven by analysis of a real Roy image:
the `BLE` name, someone else's ESB clone, and sysfs inventory do not replace this verification.
OpenPuck radio parameters pertain to its investigated devices; transferring them to
Frame Roy without measurement/disassembly is not permissible.

## K05834
Source `D0480`, original line45, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Primary sources:
- Nordic nRF52840 RADIO (original reference: `https://docs.nordicsemi.com/r/bundle/ps_nrf52840/page/radio.html`)
- Nordic ESB protocol (original reference: `https://github.com/nrfconnect/sdk-nrf/blob/main/doc/nrf/protocols/esb/index.rst`)
- OpenPuck architecture (original reference: `https://github.com/safijari/openpuck/blob/main/ARCHITECTURE.md`)

## K05835
Source `D0480`, original line50, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Prepared tool

## K05836
Source `D0480`, original line59, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

On the Frame itself:

## K05837
Source `D0480`, original line65, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

With a different SteamVR path:

## K05838
Source `D0480`, original line71, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Via SSH from PowerShell (substitute known address/user):

## K05839
Source `D0480`, original line79, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For a complete analysis, `roy_dongle_fw.hex`,
`roy_dongle_timesync_client_fw.hex`, associated `.sh/.cfg` scripts, manifest, and
the `frame_hmd` driver binary are additionally needed. Copying them does not require executing scripts.
Images from SteamVR resources are not guaranteed to match the firmware in flash;
the version of the running Roy must be matched separately via stock diagnostics.

## K05840
Source `D0480`, original line85, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Confirmed Frame data and SRAM experiment
