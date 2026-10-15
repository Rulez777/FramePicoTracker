# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K03441
Source `D0478`, original line61, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

`station_runtime.py`, `swift_records.py`: only explicit hardware-CRC-valid,
complete producer records can enroll identities. Strict 12-byte bootstrap decoder,
complete working-record run decoder, multi-command IMU extraction, counter dedup,
sample gaps and counter reset after stale reception. `crc` is raw Nordic RXCRC;
new `hardware_crc_valid` is a separate producer assertion. Old capture files
without this assertion are not treated as live runtime enrollment evidence.

## K03442
Source `D0478`, original line68, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

`frame_occ_live.py`, `frame_live_camera.py`, `optical/live_decode.py`: streaming
record iterator, per-poll/record/disk limits, partial EOF and replacement detection,
skip complete history without allocating payloads, persistent ffmpeg decoder,
two-frame grayscale queue and compressed backlog failure. Decoder source-time
verification remains false. Frames are not wired into a metric world-pose solver.

## K03443
Source `D0478`, original line74, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

`steam_frame_tracker.py`: no VMT publication of camera-space/unanchored estimates;
no IMU prediction before optical initialization or beyond the anchor timeout;
reinitialize on optical reacquisition. This is a research engine, not world fusion.

## K03444
Source `D0478`, original line78, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

`pose_frame.py`, `pose_wire.h`, native driver: PTP2 chipID identity, monotonic source
time, finite values, quaternion norm, velocities, quality and optional verified
battery; separate connected/valid; stable `PicoSwift<chipID>` serials; retain
registered devices after disconnect; reject reordered/stale packets; 250 ms
source-time stale policy. No measured-world-pose producer feeds it yet. PTP1 is
no longer accepted. Battery unknown clears the provides-battery property.

## K03445
Source `D0478`, original line85, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

`frame_debug_agent.py`, native UI: diagnostics copy the existing live station log
instead of spawning a passive loader or controlling somebody else's camera
recording. Idle station reports a clear error. Restart button is explicitly
Restart API; stop response no longer asserts restoration merely from systemctl.
Fresh RF-connected peers can be displayed as awaiting optics, never falsely
marked tracked. Closing the UI still does not stop the background service.

## K03446
Source `D0478`, original line92, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

## Remaining before the authorized single hardware session

## K03447
Source `D0478`, original line94, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

1. Replace ABI3's repeated seq0 configuration with a per-peer ACK/retry/sequence
   engine and safe TX queue. Enrollment needs a new ABI; current ABI requires
   three known peers. Handshake timing and reconnect are not certified.
2. Establish scheduler budget and measured controller-validity watchdog. Current
   low-priority build does not resolve the historical denied-slots issue. Do not
   enable persistence/autostart just because it compiles.
3. Establish the camera stream/sourceCamera mapping, active calibration/crop,
   exposure time units and cross-clock transform. A decode timestamp is not an
   exposure timestamp; SWD read time is not RF arrival time.
4. Wire command14 and periodic10 to validated exposure times, including skipped
   pulses and three-device temporal association. Existing builders alone are
   insufficient.
5. Obtain tracker LED associations and metric calibrated optical poses; initialize
   world fusion from timestamped HMD transforms; validate sensor scale/axes and
   optical yaw correction. No synthetic position or unbounded IMU integration.
6. Feed measured PoseFrame into the native driver, unify actual status, then
   install the consistent release and run the single 10-minute end-to-end session
   specified in HANDOFF_FULL_DEVELOPMENT_PLAN.md.

## K03448
Source `D0478`, original line113, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

There has been no new measurement proving three simultaneous 6DoF trackers,
ordinary power-on reconnect, sustained controller coexistence, live exposure
alignment, or loaded PTP2 driver behavior in SteamVR.

## K03449
Source `D0479`, original line1, SHA256 `5bfc2aebd0408929a1921d7cee7111ee79145d2397c03a94b9035c3cfa743789`. Status: historical/unadjudicated narrative; English source prose.

# Negotiation persistence audit 212

## K03450
Source `D0479`, original line3, SHA256 `5bfc2aebd0408929a1921d7cee7111ee79145d2397c03a94b9035c3cfa743789`. Status: historical/unadjudicated narrative; English source prose.

Read-only audit of the extracted Swift application image, base 0x1000. No radio transmission or device mutation in this investigation.

## K03451
Source `D0479`, original line5, SHA256 `5bfc2aebd0408929a1921d7cee7111ee79145d2397c03a94b9035c3cfa743789`. Status: historical/unadjudicated narrative; English source prose.

The 605C connection transition sets RAM byte 0x20000E49 when the paired marker at 0x20000FD4 is not 0xAAAA. The radio scheduler consumes that flag at 6CF6..6D20 and calls context 0x20001050 method +0x20. Initialization 8C8C binds that method to Thumb 0x9DDD.

## K03452
Source `D0479`, original line7, SHA256 `5bfc2aebd0408929a1921d7cee7111ee79145d2397c03a94b9035c3cfa743789`. Status: historical/unadjudicated narrative; English source prose.

9DDC writes the paired marker 0xAAAA, slot value and four peer-seed bytes into the eight-byte record at 0x20000FD4, then invokes context method +0x14 on its storage cursor. Initialization binds that writer to 0x9D7D. 9D7C calls storage dispatch 1EFE and waits for completion through 1FA8. 1EFE validates alignment and storage address bounds before dispatching its backend write function. The concrete flash backend still needs tracing; this is nevertheless a persistent-storage request, not merely a volatile radio configuration.

## K03453
Source `D0479`, original line9, SHA256 `5bfc2aebd0408929a1921d7cee7111ee79145d2397c03a94b9035c3cfa743789`. Status: historical/unadjudicated narrative; English source prose.

Consequently a station negotiation reply to an unpaired tracker cannot currently be described as RAM-only. Do not deploy such a reply under the no-flash constraint. A tracker whose paired marker remains 0xAAAA skips the pending-store flag in this transition; that is a candidate path for reconnecting to its existing saved station identity, but current tracker state and other persistence callers must be checked before hardware use.

## K03454
Source `D0479`, original line11, SHA256 `5bfc2aebd0408929a1921d7cee7111ee79145d2397c03a94b9035c3cfa743789`. Status: historical/unadjudicated narrative; English source prose.

First tracker ID remains independently confirmed [TRACKER_ID] (air bytes [TRACKER_ID]). Autonomous connection and 6DoF remain unproven. Evidence: outputs/firmware-standalone/swift-persist*212.txt.

## K03455
Source `D0480`, original line397, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Primary references:
- https://raw.githubusercontent.com/nrfconnect/sdk-nrfxlib/main/mpsl/include/mpsl_timeslot.h
- https://docs.nordicsemi.com/r/bundle/nrf5_sdk_v17.1.0/page/structnrf_esb_config_t.html
- https://github.com/nrfconnect/sdk-nrf/blob/main/include/esb.h

## K03456
Source `D0480`, original line411, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

# Swift: startup channels and negotiated channel mask

## K03457
Source `D0480`, original line413, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

2026-10-05. This is analysis of the extracted Swift2.0 v1.89 update image,
LOAD_BASE=0x1000, not a radio capture or proof of installed tracker firmware.

## K03458
Source `D0480`, original line416, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

## Firmware evidence

## K03459
Source `D0480`, original line418, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

- Initializer VA0x6658 reads three bytes at VA0x15fdc (01 25 4d)
  and VA0x15fe0 (04 4e 4e). VA0x6726/0x671a copy them into
  state 0x20000e10 +0x2d/+0x30, respectively. Thus pipe0 group is
  1/37/77 and pipe1 group is 4/78/78; the repeated 78 is preserved.
- VA0x75cc configures pipe0, mask1, random index modulo3, channel
  state+0x2d+index. VA0x7574 configures pipe1, mask2, index modulo3,
  channel state+0x30+index. These are startup groups, not all active channels.
- Pipe1 BASE1 derives from byte-wise addition modulo256 of seeds at
  RAM0x20000fd8 and RAM0x20000efe via VA0x6124, then RBIT at VA0x58f8.
  Prefix is read from RAM0x20000fd0. Actual seed/prefix values remain unknown.
- VA0x69c0 handles event2 and reads RX FIFO through VA0x56b8.
  That read copies five metadata bytes followed by payload. In VA0x68f0
  the tested first byte is LENGTH, not a packet opcode: paths accept length9
  or (length & 0xfb)==17, i.e. lengths17 and21.
- The 17/21 path copies six payload bytes at offsets6..11 to a temporary
  buffer. VA0x7508 skips its first byte, processes the remaining40 bits,
  and emits channel 2*(bit_index+1) for each set bit: channels2..80.
- Payload offsets12..15 and16 are copied into peer seed/prefix fields
  conditionally on existing pairing state. Their names are inferred from
  later BASE1/prefix setters; they are not live measured values.
- Working channel list is stored at state+5, length state+2, index state+0x36.
  VA0x6ec4 selects that list and advances its index; VA0x6e74 can replace
  the list from state+0x19. Startup triplets alone are insufficient for pairing.

## K03460
Source `D0480`, original line442, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

`pc/radio/pico_swift_probe.py` now reports startup groups and negotiation
offsets and offers offline channel-mask/payload decoding. It never opens
devices or generates a validated radio profile. 51 offline tests pass.

## K03461
Source `D0480`, original line446, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

## Frame revalidation

## K03462
Source `D0480`, original line448, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Final reserved-close MPU implementation passed full timer/MPSL revalidation,
with both controllers awake: 222 total START callbacks, 337 pose rows each
for HMD/Left/Right. Total START count includes activity before sampled interval.
Session closed, static MPU count restored, RAM/RTT checkpoint matched,
MPU CTRL restored. No RF was enabled and no Pico packet was received.

## K03463
Source `D0480`, original line454, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Evidence copied to workspace `work/frame-stock/ram-experiments/wifi-latest/`
as `timer-with-poses23.json`, `.recovery.json`, `.summary.json`,
`timer-poses23.csv`. The harness used remote names containing21; these copies
identify the final implementation run without replacing previous local21 data.

## K03464
Source `D0480`, original line459, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Frame is accessible via SSH with USB unplugged. Pico remains on USB.
No module remains installed after that bounded test. Next implementation step:
review raw RX slot cleanup, capture a real CRC-valid packet, then recover
the polling/pairing exchange before exposing real poses in SteamVR.

## K03465
Source `D0480`, original line467, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

# First passive Roy RX experiment

## K03466
Source `D0480`, original line469, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

2026-10-05. User-authorized temporary SRAM module; no flash/pairing changes.

## K03467
Source `D0480`, original line471, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

- Raw receiver now ignores RADIO events outside active phase2, preventing
  rearming RX after TIMER0 closed a slot. Rearm additionally requires DISABLED.
- Explicit MODECNF0=0 and ADDRESS_RSSISTART added. RXEN only; no TXEN or ACK
  generation. The fixed-build loader supports --rx-only with native shell,
  MPU reservation, early hardware fault traps and normal checkpoint cleanup.
- Candidate profile: frequency37, MODE4, PCNF0=0x40008,
  PCNF1=0x01040096 (maximum150 bytes, big endian, BALEN4),
  BASE0=0x552c6a1e, PREFIX0=0xc0, CRC16/poly0x11021/initFFFF,
  whitening off. Firmware-derived candidate, not a validated paired profile.
- passive-rx24 requested1500us slots, duration0.5s, interval10000us.
  One START, then BLOCKED/IDLE state; zero CRC-valid records. Loader exited0.
  Session closed, hook cleared, static MPU count restored, RAM/RTT checkpoint
  matched, MPU restored. No target-restarted/lost-RAM or captured fatal fault.
- Before test vrcmd listed HMD and both controllers; after it listed HMD only.
  Concurrent pose output was empty. This is NOT coexistence proof; cause may
  be controller sleep or radio interference and has not been established.
  User asked to wake controllers to distinguish this before further RF tests.
- 51 offline tests pass. This does not validate radio reception or standalone poses.

## K03468
Source `D0480`, original line490, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Evidence: workspace work/frame-stock/ram-experiments/wifi-latest/passive-rx24.*.
Next: verify controller reconnection, measure concurrent poses reliably, and
resolve any RX/coexistence issue before broader scans or active pairing.

## K03469
Source `D0480`, original line500, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

MPSL reference: https://raw.githubusercontent.com/nrfconnect/sdk-nrfxlib/main/mpsl/include/mpsl_timeslot.h
describes exclusive RADIO/TIMER0 access inside granted slots and guaranteed XTAL
for radio use. Current upstream context size differs from Roy's stock build;
continue using verified fixed-build addresses/ABI, never substitute upstream ABI.

## K03470
Source `D0480`, original line508, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

After user follow-up both controllers were again listed without Roy restart or
firmware recovery. Retest used500us slots (compare300us), same channel37/profile,
duration0.5s. 73 total START, zero CRC-valid records, all unload/restoration checks
passed. Both controllers remained listed after the experiment. No lost-RAM/fatal
fault recorded. This supports bounded RX execution, not sustained tracking proof.

## K03471
Source `D0480`, original line514, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Pose file remained empty even with stdbuf; an independent 3s --pollposes read
also produced no rows without any SRAM module. Cause is unresolved; do not infer
pose continuity from the device list. Pico dumpsys power explicitly reported
mWakefulness=Asleep, mIsVrModeEnabled=false. User asked to wake Pico and confirm
tracker connection for an actual reference radio exchange. This temporary use
of Pico does not change the standalone final requirement.

## K03472
Source `D0480`, original line521, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Loader now rejects unreviewed PHY/framing/CRC/DMA capacity before opening the
debugger for --rx-only. 52 offline tests pass. No validated packet yet.

## K03473
Source `D0480`, original line527, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

# Awake Pico reference, station getter and initial-channel scan

## K03474
Source `D0480`, original line529, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

2026-10-06 (user client date). No firmware writes, no unbond/pairing reset.

## K03475
Source `D0480`, original line531, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

## Actual observations

## K03476
Source `D0480`, original line533, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

- Pico dumpsys power changed to Awake. Fresh tracking-service logs contained
  IMU records for tracker IDs3 and5 with changing timestamps/quaternions.
  This verifies Pico receives tracker telemetry; not Frame reception or 6DoF.
- awake-rx26/channel37:72 START; awake-rx27/channel1:73;
  awake-rx28/channel77:73. Zero CRC-valid frames, full cleanup in each run.
- awake-scan29 rotated1/37/77 for3s:323 total START; ADDRESS events0,
  END events0, CRC errors0. HMD pose rows168, no controller pose rows.
- reconnect-scan33 rotated1/37/77 for10s:1023 total START, same zero RX events,
  full session/RAM/RTT/MPU restoration. HMD rows176, no controller pose rows.
  These counts include callbacks before the first sampled interval.
- Device lists changed between both controllers and only Left during the work.
  Sleep vs RF interference remains unresolved; do not call this sustained
  controller coexistence or pose continuity proof. No lost module/reset/fatal
  fault was reported by loaders. Modules were unloaded after every run.

## K03477
Source `D0480`, original line548, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

## Station service and ABI

## K03478
Source `D0480`, original line550, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

`/system/bin/stationclient_test stationchipid` read ID74:73:54:15.
`channel get` returned failure. StationService executable is at
`/system/bin/stationservice` but inaccessible to shell (Permission denied).
Its init config and client libraries are readable; Pico ADB is not root.

## K03479
Source `D0480`, original line555, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

New native helper `pc/radio/pico_station_read.c`, built with the existing
Unity Android NDK, runs from `/data/local/tmp`. By default it only reads
GetTrackerStatus through libtrackerclient.pxr.so. No SDK edits required.

## K03480
Source `D0480`, original line559, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Important ABI: exported C wrappers require the client handle as first argument,
followed by method arguments. They branch directly to C++ StationClient methods.
First helper attempts omitted that handle and returned-2; this was an ABI mistake,
not evidence of Binder permission denial. Corrected helper returns0.

## K03481
Source `D0480`, original line564, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

The GetTrackerStatus implementation writes three64-byte records:
raw status at0, type at4, version string at8..14, serial at18..35.
Read registration records:

## K03482
Source `D0480`, original line568, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

| Type | Serial | Reported version |
|---|---|---|
|1|[TRACKER_SERIAL]|TR0189|
|2|[TRACKER_SERIAL]|TR0189|
|3|[TRACKER_SERIAL]|TR0189|

## K03483
Source `D0480`, original line574, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

These are registration/cache data, not proof that all three are currently awake,
nor proof of exact installed build hash. Status enum not yet interpreted.
Uninterpreted record bytes36..41 are313233330000 in all three; do not label
them MAC/FICR without evidence. No verified tracker radio ID was exposed here.
GetTrackerInfoForAlg copied612 bytes, including pointer-like values in its tail;
that dump is not a valid source of paired radio seeds and is not used for RF.

## K03484
Source `D0480`, original line581, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

User subsequently authorized restarting a tracker. Optional helper flag
--reset-known-tracker1 verifies the registered serial [TRACKER_SERIAL],
then invokes SetTrackerReset(handle,1). Attempt33 returned-1; reset not confirmed.
No repeated reset attempts or pairing/flash operations. User asked to turn off
one tracker physically and wait for the capture cue before turning it on.

## K03485
Source `D0480`, original line587, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

## Firmware address derivation

## K03486
Source `D0480`, original line589, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

VA0x7d18 reads FICR DEVICEID[1] at0x10000064, stores four LE bytes.
Initializer VA0x66c0 stores this local ID atRAM0x20000efe.
Peer seed isRAM0x20000fd8, prefixRAM0x20000fd0. VA0x6124 adds each pair of
seed bytes modulo256 independently, then BASE1 setter VA0x58f8 uses RBIT.
Identity response prepared by VA0x6844 has payload length13, leading LE16
values2 and10, and local FICR bytes at payload5..8 (callVA0x689e).
Parser now decodes that identity layout and paired-base arithmetic offline.
Station ID74:73:54:15 is a candidate peer seed, not yet confirmed by capture.

## K03487
Source `D0480`, original line598, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Raw receiver adds ADDRESS-slot/END/CRC-error counters and stores actual slot
channel to avoid mislabeling records while host rotates frequency. Host scan
only changes the next-slot channel word, not the running RADIO register.
54 offline tests pass. Actual Pico RF/standalone SteamVR tracker poses remain absent.

## K03488
Source `D0480`, original line603, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Evidence: workspace work/frame-stock/pico-awake-reference26.log,
pico-tracker-status32.txt, pico-reset33.txt, and ram-experiments/wifi-latest/
awake-rx26..28.json, awake-scan29.*, reconnect-scan33.*.

## K03489
Source `D0480`, original line610, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

This section supersedes earlier pending wake/restart instructions. The user wore
PICO and confirmed Frame controllers awake. Both headsets are available:
Frame via SSH/Wi-Fi ([DEVICE_IP]), PICO via USB ADB ([HEADSET_ADB_SERIAL]).
PICO is only a reference for protocol investigation; it is not an acceptable
dependency of the final standalone implementation. No dongle is used.

## K03490
Source `D0480`, original line616, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

### Reference timing and power

## K03491
Source `D0480`, original line618, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

The bounded poweron-scan34 capture ended before the user's physical tracker
power-on. Therefore it did NOT capture the initial handshake. Its 1021 slots
had zero ADDRESS/END events. PICO station powered off while the headset was
not worn, despite ADB waking the OS. Later logs show all three registered
trackers connected and live tracking; this is separate evidence, not overlap
with scan34. The user subsequently wore PICO to maintain presence/awake state.
Remote SetTrackerReset returned -1 and did not establish a reset. No pairing
reset, OTA, flash write, or successful remote tracker restart occurred.

## K03492
Source `D0480`, original line627, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

### MPSL control and raw receiver

## K03493
Source `D0480`, original line629, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Tests35/36 granted one slot then BLOCKED. Host-retry37 granted five with five
retries. Timer-only38, with RADIO disabled, also granted one then BLOCKED.
After the user woke Frame controllers, raw39 granted219 slots and recorded
six ADDRESS events. This associates scheduling behaviour with controller
state; continuous BLE scanning is a hypothesis, not a proven cause.

## K03494
Source `D0480`, original line635, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

The experimental receiver uses a preamble-like address (BASE0=0, PREFIX=55),
PCNF0=0, no hardware CRC, fixed raw DMA length24 or40. The register recipe is
inspired by radiobit source (original reference: `https://raw.githubusercontent.com/virtualabs/radiobit/master/micropython/source/microbit/modradio.cpp`).
That source targets nRF51. Its BALEN=1 is outside the documented nRF52833
range2..4, so this experiment is NOT a validated promiscuous receiver on Roy.
ADDRESS events can be noise or other devices. MODE4 is the researched Swift
PHY setting; it does not establish a BLE transport or Valve ESB compatibility.

## K03495
Source `D0480`, original line643, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

RADIO is receive-only, with no TX/ACK. At slot end it is disabled before
partial DMA is copied. New records carry partial=true and the actual channel
latched at START; buffers are cleared before receiving. The packets counter
counts saved raw fragments in this mode, not verified protocol packets.
END event count remains zero: increasing buffer length did not yield a
complete hardware frame. Address matching, bit order, alignment and raw
capture validity remain unresolved.

## K03496
Source `D0480`, original line651, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

| Experiment | Slot / duration | Channels | Granted slots | Raw fragments | END | Software CRC candidates |
|---|---|---|---:|---:|---:|---:|
|partial-preamble40|500us /2s|1,37,77|219|7|0|0|
|working-channel41|500us /8s|even2..80|821|19|0|0|
|raw-long42|750us /1s|1,37,77,4,78|119|7 (40 bytes)|0|0|

## K03497
Source `D0480`, original line657, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

The even-channel sweep is derived from the firmware's negotiation-mask
mapping; the actual negotiated channel mask is unknown. Initial host channel
37 may appear before the first sweep update. Strongest raw41 event was RSSI
magnitude51 on78, only four nonzero bytes; it is not attributed to PICO.
Software CRC decoder searches a candidate 5-byte address, 8-bit length,
4-bit S1 and CRC16/CCITT (initFFFF, polynomial1021). Its alignment has not
been established by a captured golden frame. No CRC match was found in
these records; no successful reception, pairing or tracker pose is claimed.

## K03498
Source `D0480`, original line666, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

### Coexistence, restoration and next implementation step

## K03499
Source `D0480`, original line668, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Tests40/41 recorded HMD and Right pose rows (199/193 and384/368), with no
Left pose rows. Test42's pose file was empty. Both controllers were listed
after42, but this is not proof of continuous tracking for both controllers.
All three tests report session closed, static MPU count restored, RAM
readback restored, and MPU restored. Cleanup is bounded and restores the
stock shell hook/RTT allocation; no persistent SRAM module is left active.
Stock flash anchors are checked, not a complete 512KiB flash comparison.

## K03500
Source `D0480`, original line676, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Next: establish a valid raw receiver configuration on nRF52833, validate
DMA bit order/alignment against a known frame and its CRC, then identify
repeated Swift identity/negotiation content. Do not implement active polling
or label arbitrary raw data as PICO. Once addresses and channel hopping are
confirmed, implement bounded receive/polling coexistence, decode telemetry,
and demonstrate standalone poses without PICO before SteamVR integration
and optical correction/calibration can be considered complete.

## K03501
Source `D0480`, original line684, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Evidence: work/frame-stock/ram-experiments/wifi-latest/{partial-preamble40,
working-channel41,raw-long42}.json; outputs/roy-raw-capture-evidence.json.
Updated code: pc/radio/ram/roy_ram_rx.c and roy_ram_loader.py;
offline CRC decoder: pc/radio/pico_swift_probe.py. No SDK changes.

## K03502
Source `D0480`, original line692, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Tests43-45 supersede the interpretation that zero callback END counts meant
the radio never finished receiving. A new independent read of EVENTS_END at
the TIMER deadline found seven hardware END events in end-check44, while
the callback counter remained0. MPSL signals observed were START and TIMER;
no RADIO signal. Exact reason for missing forwarding is unresolved.

## K03503
Source `D0480`, original line698, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

The SRAM receiver now checks END at slot teardown before recording a raw
buffer, distinguishing completed fixed-length DMA from truncated DMA. It
does not change stock Valve IRQ handlers or NVIC routing. polled-end45:
218 slots,13 raw records,9 completed DMA buffers,4 partial, all restoration
checks passed. Software Swift CRC candidates:0. A completed raw DMA buffer
is NOT a validated Swift packet. Earlier raw40/41/42 END=0 was the callback
counter, not reliable proof of absent hardware completion; historical reports
are preserved, not rewritten as measurements they did not take.

## K03504
Source `D0480`, original line707, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

supported-raw43 tested PCNF0=01000000 (16-bit preamble), PCNF1=01022828
(BALEN2,40-byte static length), BASE0=55555500/PREFIX55, MODE4, CRC disabled.
120 slots,1 partial raw record,0 directly observed END. These individual
field values conform to documented ranges, but arbitrary-address reception
and PICO compatibility are not established. The BLE2M preamble requirement
is in Nordic's specification (original reference: `https://docs.nordicsemi.com/r/bundle/ps_nrf52833/page/radio.html`).
Swift's extracted configuration used PCNF0=40008; do not substitute a BLE
framing rule and claim it represents the proprietary Swift wire protocol.
Legacy BALEN1 capture remains explicitly experimental, outside documented
range. The experiment is a comparison, not a validated receiver profile.

## K03505
Source `D0480`, original line718, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

56 offline tests pass. Both firmware images remain unmodified; SDK untouched.
No OpenOCD process remains after cleanup. Test45 has HMD pose rows only;
both-controller continuity remains unproven. Frame/PICO connection facts and
standalone requirements from the preceding section still apply.

## K03506
Source `D0480`, original line723, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Next: offline bit-order/alignment analysis of completed DMA, then establish
repeatable identity/negotiation frames with CRC and attribution. Investigate
the stock RADIO forwarding path read-only before changing IRQ routing.
No active polling or standalone tracker poses are implemented yet.

## K03507
Source `D0480`, original line731, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Analysed55 raw records from40,41,42,44,45. Exploratory search tries MSB/LSB
bit order within each byte, inversion, every offset with sufficient remaining
bits, 5-byte address, 8-bit length up to32, 4-bit S1, and CRC16 both including
and excluding address. 14,902 eligible CRC comparisons: zero matches.
This covers these framing hypotheses only; it does not prove PICO absent,
and does not validate the experimental raw receiver or eliminate longer frames,
other address/header lengths, whitening, or incorrect DMA capture.
Results: outputs/roy-alignment-search.json, script work/analyze_raw_alignment.py.
No new hardware experiment was performed for this search.

## K03508
Source `D0480`, original line741, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Rechecked Swift register writes in the extracted update image (LOAD_BASE1000):
VA50e8 loads literalVA5108=00040008 and writes RADIO.PCNF0 atVA50ec;
VA50f2 reads address length from RAM20000080+11, subtracts1, shifts16,
and sets PCNF1.ENDIAN bit24. Thus researched firmware explicitly uses
8-bit LENGTH, 4-bit S1, PLEN0, no S0, big-endian packet fields. A generic
BLE2M 16-bit-preamble requirement is not proof of Swift framing: the image
uses PLEN0 despite MODE4. Preserve this discrepancy as an observed fact.

## K03509
Source `D0480`, original line749, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

CRC configuration atVA544e selects config+9. Value2 followsVA54d6 (INITFFFF),
literalVA5538=00011021 (POLY), andVA547a writes CRCCNF=2. SKIPADDR is not
set in this path. This supports CRC over the address plus header/payload;
an over-air captured golden frame is still required to establish bit order.

## K03510
Source `D0480`, original line754, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Read-only stock Roy disassembly: RADIO vector word at44=33023, handler
VA33022 callsVA10644 before kernel wrappers203cc/205ac. VA10644 dispatches
an internal callback with argument4 through a radio-owner table. This is
an internal dispatch code, NOT evidence that the public MPSL RADIO signal
equals4. Exact forwarding to the user timeslot callback remains unresolved;
do not change stock vectors/handlers based on this alone. TIMER polling
remains the bounded fallback from test45. No IRQ/flash changes made.

## K03511
Source `D0480`, original line762, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Current outcome remains: zero attributed Swift packets, no standalone poses.
Next useful evidence is a repeatable correctly addressed capture or a raw
receiver validated against known transmitted bytes, rather than treating
completed raw DMA/noise as decoded telemetry. PICO live presence and Frame
controller wake state must be checked again before further RF experiments;
their earlier user confirmations do not prove they remain awake indefinitely.

## K03512
Source `D0480`, original line772, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Before tests, live ADB showed PICO Asleep. The user subsequently confirmed
headset/tracker ready; ADB then showed Awake. Frame device list contained
HMD and both controllers. No assumption that PICO remains awake indefinitely.

## K03513
Source `D0480`, original line776, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Read-only running RADIO snapshots46 showed controller connection traffic:
MODE4, PCNF0=01100108, CRC24 polynomial65b, CRCCNF103, changing channels,
addresses/CRCINIT/whitening seeds. Fields were read sequentially while CPU
ran: individual snapshots may mix connections/events and are NOT valid
atomic radio profiles. Earlier stock MODE3 advertising snapshot remains
historical, not the only operating mode of Roy. No register writes or halts
in read-only snapshot46; OpenOCD closed afterwards.

## K03514
Source `D0480`, original line784, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Added narrowly checked known BLE advertising reference profiles, with address
BASE0=89bed600/PREFIX8e, MODE3, PCNF0=100108, PCNF1=02030040,
CRCCNF103 (CRC24, address skipped), poly65b/init555555, whitening channel
37/38/39 paired to frequency2/26/80. Receiver remains RX-only.
TIMER END polling now accepts CRC-enabled frames only when CRCSTATUS is
valid; previously the fallback only recorded raw mode, so CRC-enabled
reception could be missed when no RADIO callback was forwarded.

## K03515
Source `D0480`, original line792, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

| Experiment | Slot/duration | Frequency | Slots | END | CRC errors | Saved CRC-valid frames |
|---|---|---:|---:|---:|---:|---:|
|ble-reference47|750us/2s|2|220|1|1|0|
|ble-reference48|1000us/2s|26|219|2|1|1|
|ble-reference49|1000us/2s|80|222|1|0|1|
|swift-address50|750us/3s|1,37,77|322|0|0|0|

## K03516
Source `D0480`, original line799, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Both accepted BLE frames contain structurally plausible advertising PDUs.
Independent CRC calculation matches RXCRC for BOTH: reflected polynomial
DA6000, input bytes LSB-first, initialAAAAAA (bit-reversed configured555555),
and final24-bit reversal gives RXCRC1117292 /6981641. PCNF0.S1INCL inserts
a zero S1 byte in DMA after the first two bytes; it must be omitted from
the CRC input. This establishes that hardware END/CRC and DMA extraction
work for known-address BLE; it does not attribute these advertisers to PICO.

## K03517
Source `D0480`, original line807, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Raw control51 uses the SAME known advertising address, PCNF0=0,
PCNF1=02032828 (40 bytes, whitening enabled), hardware CRC disabled,
1000us slots/4s:422 slots,5 raw records,3 completed DMA/2 partial.
Two completed buffers contain the same29-byte advertising payload as test49,
now with the actual three CRC bytes561190 after the PDU. Independent
CRC over each PDU equals9441622, the little-endian stored CRC561190.
Therefore BOTH raw control buffers validate. A third completed buffer fails
CRC; completion alone still does not imply a valid received protocol frame.
Bytes after the protocol CRC in the fixed40-byte buffer are outside the
validated PDU and must not be decoded as telemetry.

## K03518
Source `D0480`, original line818, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Important boundary: this proves raw capture with a KNOWN correct address,
documented address length and BLE whitening. It does NOT validate the
BALEN1 preamble trick, arbitrary address recovery, Swift MODE4 framing, or
the earlier55 raw fragments. They remain unverified. Swift-address50 with
confirmed PICO Awake found no ADDRESS events; the candidate startup address
does not identify current paired traffic. No pairing or active TX attempted.

## K03519
Source `D0480`, original line825, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

All tests47..51 report session closed, RAM readback restored, MPU count and
permissions restored. Pose files have HMD/Right rows, not continuous Left
proof. No flash/IRQ changes; final OpenOCD process check empty.57 tests pass,
including rejection of mismatched advertising frequency/whitening profile.

## K03520
Source `D0480`, original line830, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Next: use these validated reference PDUs as golden data to evaluate any
proposed unknown-address/raw capture. Recover actual Swift pairing address
and channel parameters from repeatable evidence before polling. Standalone
PICO poses and SteamVR integration through Roy remain unimplemented.
