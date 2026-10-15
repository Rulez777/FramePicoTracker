# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K02401
Source `D0363`, original line74, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

21 cases pass: state1 rejection at three elapsed times, state2 acceptance,
EARLIEST eligibility in both states, and suppression of requests after stop.
Low-priority action remains NONE. State2 requests retain900us length and a
4727us multiple with at least300us lead in the tested cases.

## K02402
Source `D0363`, original line79, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

Command:
`.venv/Scripts/python.exe -m pytest pc/tests/test_service_grid_native_request.py -q -p no:faulthandler`

## K02403
Source `D0363`, original line82, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

The default pytest faulthandler emitted Windows access-violation notifications
inside Unicorn mem_map despite all cases completing. Disabling that plugin
produced a clean21-pass run in0.13s; emulator assertions were unchanged. This
runner caveat must not be interpreted as a target firmware fault.

## K02404
Source `D0363`, original line87, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

DWT subtraction has a67.108864s modulo horizon at64MHz. A wrap across a short
interval is handled; a full counter revolution without START is ambiguous.
The existing grid test checks only short elapsed intervals around one wrap.

## K02405
Source `D0363`, original line91, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7-medium was requested through agent-intern Cursor in read-only mode.
The bridge reported a210s process timeout and returned no review. No independent
reviewer verdict is available from that call; this is not evidence of quota
exhaustion. The conclusions above are based on the local native execution.

## K02406
Source `D0363`, original line96, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

## Next implementation criteria

## K02407
Source `D0363`, original line98, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

Before building a successor, explicitly handle state1 recovery: either a
verified eligible request strategy or a bounded fallback when NORMAL returns
-35. EARLIEST changes cadence and must be recorded as a re-anchor, not described
as preserving the old grid. Never silently ignore errors or remove fault/stop
guards. Execute the replacement through the real API in both states, include
stop/API-failure cases and the DWT horizon policy, then verify build size and
source/ELF/capture provenance. Only then prepare one bounded hardware session
under the applicable authorization. No new period/N sweep is justified here.

## K02408
Source `D0363`, original line107, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

Production source, saved binaries, firmware whitelist, OTA readiness and pose
gates are unchanged. Identity/optical6DoF and controller coexistence remain
unproven. The complete machine-readable audit is
outputs/service-fast-poll26/evidence.json.

## K02409
Source `D0364`, original line1, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

# Iteration27: service radio exceeds the60% target on hardware

## K02410
Source `D0364`, original line3, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. The user enabled Frame and the stock tracker, disabled the Valve
controllers, and explicitly authorized continued SRAM service development and
hardware measurements until at least60% radio delivery was demonstrated.
No tracker/Roy flash, reset, unlock or OTA operation was performed.

## K02411
Source `D0364`, original line10, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

One stock FW189 tracker, slot0/chipLE3eed1979, serial[TRACKER_SERIAL];
Frame Roy stock SHA0375ab90…dd50. Both controllers were absent from live
tracking_watch before the runs; controllers_off guard stayed enabled.
One active peer, other peers paused, cameras/optics/phase10 disabled.

## K02412
Source `D0364`, original line15, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

| Capture | Counter span | Completed TX delta | CRC-valid RX delta | RX/TX-END | Observed unique target answers / total TX |
|---|---:|---:|---:|---:|---:|
| service-measure27 |59.986s|3004|2728|90.812%|Only last1000 raw records retained; do not use this subset to claim full unique coverage|
| service-confirm27 |119.930s|5862|5855|99.881%|5491/5876=93.448% conservative lower bound|

## K02413
Source `D0364`, original line20, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

Confirmation retained5492 records;5491 were target/channel2 replies in5491
different slot numbers, all with valid payload checksum. The denominator5876
is the cumulative completed-TX counter, including startup, whereas the delta
ratio excludes transmissions before the first sample. This lower bound remains
above60% even after364 host ring drops. It is not an inflated count of several
responses to a single poll. No slot-number wrap occurred in these captures.

## K02414
Source `D0364`, original line27, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

RF completion rate in confirmation was48.879TX/s and48.820RX/s. Ratio improved
substantially while grant rate decreased from the older~126slots/s. Do not call
this200Hz tracking or full native sample delivery. MPSL refused many grid-aligned
requests; service does not exclusively own all radio scheduling merely because
the controllers are disconnected. Both runs were fault-free, host_retries0,
session_closed_verified/ram_restored_verified/mpu_restored/unloaded true.
Host module/config restoration and combat preservation were also verified.

## K02415
Source `D0364`, original line35, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

Primary evidence: outputs/service-fast-poll27/service-confirm27/analysis27.json,
N0-grid-confirmation.json, manifest/recovery/run-summary/host-restoration and
before-run backups. The60s capture is retained separately in service-measure27.

## K02416
Source `D0364`, original line39, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

## Implementation

## K02417
Source `D0364`, original line41, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

The new service source is pc/radio/ram/roy_ram_station_service27.c, copied
from the exact tested candidate. Combat source and combat ELF were not replaced.
build_station.py selects this dedicated source for ABI4 --service when present;
older explicitly supplied source directories continue to work.

## K02418
Source `D0364`, original line46, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

1. In low-priority callbacks, NORMAL is retained only when the native session
   state byte0x20010592 is2. Otherwise request.type is explicitly changed to
   EARLIEST. NORMAL retry skips missed4727us grid points with300us minimum lead.
   EARLIEST is a re-anchor, not preservation of the previous phase. API errors
   still stop the module. Low-priority return action stays NONE.
2. TX-END waiting is bounded by elapsed DWT time, before RX deadline minus100us,
   rather than a20000-iteration loop. A missing END ends that slot. This is not
   proof that the old assert had this sole cause; fault guards remain installed.
3. DATA+1912 counts observed TX-END events. The original software TXEN history,
   identity, checksum, RX deadline and restoration mechanisms remain intact.
4. In service_poll configuration the production loader disables --repeat-host,
   preventing a host request racing the MCU callback's retry.

## K02419
Source `D0364`, original line59, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

Measured ELF SHA256:
65d217e5e47e8028a6d58fe46c17923fcd20877dff27ffed64bc4dbf71f64a44.
Code2008/2045B, SHA256
624e6ab6b1b951aab909214105ddea2876bc2946ac69b7f67a0978eed8ded120.
Candidate source SHA256
f1b51111114e5bf8f41ec9ad525c6d5be669441b0ed05f53633c7b1150dec642.
Build command/compiler/linker hashes are preserved in deployment.json.

## K02420
Source `D0364`, original line67, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

## Why this iteration succeeded where the earlier service did not

## K02421
Source `D0364`, original line69, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

The older low-priority recovery always requested EARLIEST. After a refusal,
that abandons the previous poll phase. Capture24's median software TXEN gap was
1256.4375us, with a31.5ms cluster, despite a requested4727us interval. FW189 has
post-packet radio-off intervals, so many rapid polls can occur while it is not
listening. This is a source-based explanation supported by the old observed
schedule, not direct measurement of the tracker's RX gate.

## K02422
Source `D0364`, original line76, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

Service27 retains NORMAL grid recovery in eligible state2 and skips missed grid
points; state1 explicitly re-anchors with EARLIEST. Earlier build25 tried NORMAL
in state1 and stopped with-35, a failure reproduced with native API execution.
The host retry owner was also removed, and TX waiting was time-bounded. The
combined replacement delivered~48.8 actual RF replies/s versus the earlier
~14.4RX/s, although granted TX fell from~126/s to~48.9/s. This is a real increase
in received data as well as a better percentage, not just a dashboard correction.

## K02423
Source `D0364`, original line84, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

Changes were tested together. No separate hardware ablation apportions the gain
between phase-preserving retries, removal of the host race and bounded waiting.
Neither the exclusive cause of old losses nor the root cause of assert106/501
is proven. The display fix cannot explain the measured raw counter improvement;
it only corrects the previously misleading ACK-tag-change percentage.

## K02424
Source `D0364`, original line90, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

## Live metric correction and installation

## K02425
Source `D0364`, original line92, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

Previous reply_ratio counted ACK-tag changes. Repeated valid idle-poll replies
under the same tag were invisible to that metric. station_v4_runtime.py now
counts retained checksum-valid, identity-matched target replies once per slot;
ack_tag_changes remains separate. Ring loss lowers this visible ratio rather
than being hidden. The exact tested code SHA plus single-peer configuration
enables the TX-END denominator and a separate cumulative radio_measurement
object. Other binaries never interpret this diagnostic offset as TX-END.
The dashboard shows RF replies retained rather than only connection state.

## K02426
Source `D0364`, original line101, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

After installation the running production service published4173 unique retained
answers /4394 completed TX=94.970%, raw4354/4394=99.090%, with no live error.
The opened SteamVR PICO dashboard displayed95.5% retained replies. Its overlay
probe confirmed registration, visible panel and1200x700 texture. The application's
actual rendered preview is outputs/service-fast-poll27/installed-dashboard.png;
installed-live.json preserves the first saved production-state verification.
No additional source/binary tuning occurred after the successful confirmation.

## K02427
Source `D0364`, original line109, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

Installation is backed up under /home/steamos/pico-ram-work/service-release27/
before-install; installation.json records before/after hashes. Installed active
and service ELF are the exact measured file. Combat SHAa911666f…5ef is preserved;
station-config is unchanged. The radio unit was started explicitly, without
enabling boot autostart. The final live verification is saved beside this report's
artifacts. Controllers_off guard stops custom RF if a controller connects.

## K02428
Source `D0364`, original line116, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

## Validation and review limits

## K02429
Source `D0364`, original line118, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

74 focused tests passed: native request eligibility in both states, stop
suppression, bounded stuck-END model, loader/controller guards, transport and
runtime integration. The display regression includes idle replies with unchanged
tags, same-slot duplicate suppression, wrong identity/CRC/checksum rejection and
separate retained versus raw counter ratios. Native scheduler submission and
session lookup remain mocked in offline tests; hardware captures establish the
radio result. The ARM64 dashboard also compiled successfully on Frame.

## K02430
Source `D0364`, original line126, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7-medium completed a read-only review with1200s allowance; its original
answer is outputs/service-fast-poll27/grok-review.md. It independently supported
state-aware retry and single-owner recovery. Its additional half-microsecond,
SHORTS and tracker-timer claims are not all established and were not applied
as extra changes after the measured success. It did not know the new hardware
results when forming its causal explanation. Do not promote its opening
phase/turnaround diagnosis into exclusive measured causality.

## K02431
Source `D0364`, original line134, SHA256 `65c68ae31b55f177531e49bb0871969c7ff00ff775ba26141c20db6ff856d27c`. Status: historical/unadjudicated narrative; English source prose.

This iteration proves the requested service radio percentage for one tracker
under the recorded conditions. It does not prove optical6DoF, three-peer throughput,
active-controller coexistence, an unrestricted all-day run or OTA readiness.

## K02432
Source `D0365`, original line1, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

# Iteration28: 99% retained service replies and a negative coexistence experiment

## K02433
Source `D0365`, original line3, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

## Measured outcome

## K02434
Source `D0365`, original line5, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

User authorized a 20-minute optimization window and a separate two-controller
experiment. Work started at16:18:25 UTC. The service target was achieved before
the deadline of16:38:25 UTC, without flashing either device.

## K02435
Source `D0365`, original line9, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

The unchanged, measured service27 ELF produced the following 150-second capture:

## K02436
Source `D0365`, original line11, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

| Measurement | Result |
|---|---:|
| Completed TX-END counter |7238|
| CRC-valid RX counter |7190|
| Retained unique target replies |7187|
| Retained replies / all completed transmissions |99.295385%|
| MCU ring dropped records |3|
| Independently decoded invalid target records |0|

## K02437
Source `D0365`, original line20, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

The saved raw logs contain7188 records: one bootstrap record and7187 target
records. All target records pass the application decoder/checksum and hardware
CRC flag checks, with7187 distinct slot numbers. This independently matches the
runtime reply count. See `outputs/service-fast-poll28/record-validation.json`
and `service-host28-final/final-run-records.jsonl`.

## K02438
Source `D0365`, original line26, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

An explicitly separate stable interval, after30 seconds of startup, lasted
118.097507 seconds:5677 completed transmissions,5676 retained replies,
99.982385% retained,5677 CRC-valid replies, and one ring drop. Counter reads are
sequential; equality of aggregate counters is not an atomic per-packet delivery
proof. Do not present the stable interval as the whole-run percentage.

## K02439
Source `D0365`, original line32, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

## Changes and causal limits

## K02440
Source `D0365`, original line34, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

The service27 radio binary and its SRAM reservation remain unchanged:
ELF SHA256 `65d217e5e47e8028a6d58fe46c17923fcd20877dff27ffed64bc4dbf71f64a44`.
The improvement concerns host consumption of the ten-record SRAM ring.

## K02441
Source `D0365`, original line38, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

Previously the host held records while reading peer state, publishing status,
decoding packets and writing mailboxes. It now copies only the published records
and acknowledges the captured write counter before that processing. Wrapped
reads include only pending records, rather than the entire1120-byte ring.
The copied bytes are private before slots become available to the producer.
Later producer publications are excluded from the acknowledged counter.

## K02442
Source `D0365`, original line45, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

Runtime polling now reads96 peer bytes and40 diagnostic bytes in one ordered
OpenOCD request, instead of transferring the unused bytes between those regions
in a416-byte contiguous read. Peer processing, controller/fault checks,
seqlocked TX mailbox publication and heartbeat checks remain active.

## K02443
Source `D0365`, original line50, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

Measured intermediate results are preserved:

## K02444
Source `D0365`, original line52, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

| Host variant | TX | Retained | Whole-run retained | Ring dropped |
|---|---:|---:|---:|---:|
| Early acknowledgment, full-ring read |8345|7596|91.024566%|697|
| Early acknowledgment, pending records only |5986|5862|97.928500%|115|
| Pending records and compact batched state read |7238|7187|99.295385%|3|

## K02445
Source `D0365`, original line58, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

These sequential runs have different startup conditions and RF grant histories;
they are not a controlled timing ablation. The drop-counter reduction and raw
record validation establish the host retention improvement. An earlier attempted
batched measurement overlapped the end of the adaptive runner and did not start
a fresh radio session; it was stopped and excluded. The final capture explicitly
required the service inactive before starting a fresh session.

## K02446
Source `D0365`, original line65, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7 medium reviewed ring publication and coexistence constraints in read-only
mode. Its review is saved in `outputs/service-fast-poll28/grok-review.json`.
The suggested whole1500-byte read was considered but not hardware-tested;
the implemented pending-record reads avoid unnecessary SWD traffic. Suggested
extra coexistence retry changes were not applied without sufficient code space
and independent validation.

## K02447
Source `D0365`, original line72, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

## Two controllers: negative tracker result

## K02448
Source `D0365`, original line74, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

After service cleanup, the user enabled both controllers and physically moved
them. A20-second custom-radio-off baseline preceded the bounded90-second RF
experiment. Both controllers and HMD were valid before RF. The101-second
controller monitoring log includes setup/quiet-window sampling and the RF run.

## K02449
Source `D0365`, original line79, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

The experimental module preserves stock BLE phase correction, MODECNF0=0x201,
600us slots,6000us interval, a7-slot cycle,200us end margin, and pauses peers1/2.
TX-END waiting now has a DWT deadline inside the fixed400us radio deadline.
Two healthy controllers are required;150ms frozen-pose and continuous tracking
loss guards remain enabled. The measured BLE quiet window was1200us, exceeding
the800us preflight requirement. The combat callback retains the existing host
blocked-recovery owner. This is not the controllers-off service27 image.

## K02450
Source `D0365`, original line87, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

Candidate code2036/2045 bytes; ELF SHA256
`c9b4972bfe684f519066c835f8f152c5daad96e7ead4c7382fcdc05b90a3ac10`.
Build/source are under `outputs/service-fast-poll28/coexist-source/`.
The initial attempt to add an actual TX counter exceeded the2045-byte reservation;
it was removed rather than expanding the reservation or removing safeguards.

## K02451
Source `D0365`, original line93, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

| Controller measurement | Baseline, each controller | Experiment monitor, each controller |
|---|---:|---:|
| OpenVR samples |1982|10004|
| Invalid samples / episodes |0 /0|0 /0|
| Pose matrix changes |1982|10004|
| Maximum unchanged-pose interval |0ms|0ms|

## K02452
Source `D0365`, original line100, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

These are OpenVR pose-health observations, not the controllers' RF packet-loss
rate; that rate was not exposed by this instrumentation.

## K02453
Source `D0365`, original line103, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

The tracker produced only one retained target response in15036 granted slots,
never completed host confirmation and produced zero decoded IMU samples.
The nominal single-peer scheduled opportunities are15036//7=2148, giving
1/2148=0.046555% saved replies per nominal opportunity. Actual completed tracker
TX count was not instrumented, and early confirmation may use additional slots;
therefore this figure must not be called an exact RF loss percentage.
Controller pose continuity was preserved, but useful tracker coexistence failed.
This negative result does not establish the cause: RX phase, enrollment and
host command acceptance remain separate hypotheses needing a new audit.

## K02454
Source `D0365`, original line113, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

The90-second run exited normally. Session closure, SRAM, MPU and trace restoration
were verified. The measured service27 ELF/config were restored; the original
combat ELF remained unchanged. Both controllers and HMD remained valid afterward.
Custom radio is left inactive while the controllers are connected.

## K02455
Source `D0365`, original line118, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

## Firmware delivery and bootloader readback

## K02456
Source `D0365`, original line120, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

99.295% or99.982% reply retention is not guaranteed lossless firmware delivery.
Future transfer requires indexed blocks, retry/timeout handling, fresh
session-specific application acknowledgments and complete-image integrity
verification. The existing OTA readiness/recovery gates remain closed. No OTA,
flash write, tracker reset, unlock or bootloader-entry command was sent.

## K02457
Source `D0365`, original line126, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

Current stock FW189 handlers do not provide a verified arbitrary flash or
bootloader-read primitive over this connection. Original iteration15 established
fixed RAM getters; reset subcommand0c changes execution and is not a readback.
The vendor OTA container contains application copies, not the bootloader area.
Iteration18 found BLE READ_DATA/READ_MEM names but their builders return empty
commands; compatibility with SwiftFW189 is unproven. Its GET_BASE_ADDR state
machine can continue to page erase and must not be used as a harmless read probe.
See readback audit15 (original reference: `FW189_ITERATION_15_COMMAND_READBACK_AUDIT_2026-10-08.md`),
vendor audit18 (original reference: `FW189_ITERATION_18_OFFLINE_VENDOR_APK_AUDIT_2026-10-08.md`), and
JNI audit19 (original reference: `FW189_ITERATION_19_JNI_DEX_AUDIT_2026-10-08.md`).

## K02458
Source `D0365`, original line137, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

## Validation, provenance and installed state

## K02459
Source `D0365`, original line139, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

83 focused tests pass, including actual ARM bounded-TX execution for the
coexistence candidate, service27 native request/deadline checks, captured-count
acknowledgment, private copies, ring index wrap and uint32 rollover rejection, batched read ordering/short-read
rejection, runtime decoding and controller guards. These offline checks are
supporting evidence and do not replace the hardware captures above.

## K02460
Source `D0365`, original line145, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

Snapshot artifacts were located through `provenance/artifact-catalog.csv` and
checked against `provenance/manifest.csv`;22 selected historical copies match
their SHA256 values in `outputs/service-fast-poll28/provenance-check.json`.
Earlier reports20–27 and referenced negative timing/controller results were
consulted. Historical sources remain preserved. New evidence is English.

## K02461
Source `D0365`, original line151, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

Installed optimized loader SHA256
`615e379082cbbaebf766c7e0b3b076db865b296b82d08ed3805ee9b1b95c48d2`;
runtime SHA256 `770b27d127ad2cce9ddee110c9c815b341440d3d48a936a561cdea333defe106`.
Before-change loader/runtime copies accompany the captures. Restored combat ELF
SHA256 `a911666fb384972639026d4c9fb828ee7302066d0a7b9076811d4f73a4a325ef`.
All local new research artifacts are indexed in
`outputs/service-fast-poll28/artifact-manifest.json`.

## K02462
Source `D0365`, original line159, SHA256 `a81e1fe4ae58e21020dd64483d76d18594aa3fbb1ea5be03bbf15897f32fd599`. Status: historical/unadjudicated narrative; English source prose.

After measurement, the loader was conservatively hardened to reject a uint32
record-counter rollover without acknowledging records. Native modulo10 indexing
is discontinuous across that rollover. No measured run approached it; the
measured loader is preserved in service-host28-final/roy_ram_loader.py.
The installed loader differs only by this unexercised rollover rejection.

## K02463
Source `D0366`, original line1, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

# Service revalidation and separate game-mode status

## K02464
Source `D0366`, original line3, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

Hardware measurements2026-10-09. Target [TRACKER_SERIAL], FW189/revision07, mode1 freshly verified. No OTA or persistent Frame firmware changes. Native UART was not toggled during these controls.

## K02465
Source `D0366`, original line7, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

The current staged Phase106-derived OFF stream previously measured1052CRC/4330TX=24.2956% in streaming,582IMU/33.9603s. A newly isolated copy of the historically successful Phase82 full stack and exact Service52 ELF was run to test whether high reception remains reproducible today.

## K02466
Source `D0366`, original line9, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

First service recheck60s:3447CRC/4355TX=79.1504%;3351retained=76.9460%;2015uniqueIMU;95host ring drops. Exit0 and all four restoration checks true. Early cumulative observation was20.44%, increasing during the run. This comparison changes both host stack and module; it does not isolate one instruction.

## K02467
Source `D0366`, original line11, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

Second service control requested120s with independent counter snapshots. It was interrupted by the controllers-off guard when controller1 connected. Final2116CRC/2212TX=95.6600%;2044retained=92.4051%;1222IMU;71ring drops. Exit1 reflects the guard rejection; all four restoration checks true. Snapshot span29.4809s; do not report the requested120s as completed measurement time. One local snapshot delta exceeds100% because TX and CRC counters are sampled sequentially; no exact interval packet-loss claim is supported. The uninterrupted controller-OFF99% target remains unmet.

## K02468
Source `D0366`, original line13, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

The higher reception was obtained without disabling UART, so UART enable alone does not establish a24% ceiling. Firmware flag side effects remain a separate static concern. No matched UART ON/OFF causal test was performed.

## K02469
Source `D0366`, original line15, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

## Controller baseline and game mode

## K02470
Source `D0366`, original line17, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

After both hands became visible, a15s stock-only stride3 native sampler control completed:50calls,1600raw rows, no RF session opened, no flash writes. RAM/MPU restoration and hook-clear checks passed; both controllers valid after cleanup. This proves baseline availability, not simultaneous tracker delivery.

## K02471
Source `D0366`, original line19, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

Service52 is the controller-OFF diagnostic/OTA radio profile. The experimental game/coexistence coordinator began at Phase99 and continued through103–110. Phase99 is not a tracker firmware revision; installed tracker revision remains07. Do not present service reception as game-mode success.

## K02472
Source `D0366`, original line21, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

No new joint RF run was started here. Phase109 already failed the admitted quiet-window test; repeating it unchanged or relaxing the guard is rejected. Phase110 compared an approximately650us conservative stock pause with300us in an active joint session, but differing captures do not establish causation. Next candidate must isolate the active session's scheduling burden from actual tracker RF with a bounded pause below the observed0.6–1s link sensitivity and preserve strict final controller admission. That implementation and its validation remain outstanding.98–100% with both healthy controllers is NOT achieved.

## K02473
Source `D0366`, original line23, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

## Reproducibility and provenance

## K02474
Source `D0366`, original line25, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

New evidence directories: `outputs/service-recheck-after-uart`, `outputs/service-steady-after-uart`, `outputs/controller-baseline-after-service`. Preparation scripts: `outputs/imu-after-uart/prepare_service_recheck.py`, `prepare_service_steady.py`. Existing historical attempts remain unchanged.

## K02475
Source `D0366`, original line27, SHA256 `ca9d70bf50c5350fc003631ae1c249e6a42846b38a5f74fd1e44df9cfe50168a`. Status: historical/unadjudicated narrative; English source prose.

Exact Service52 ELF SHA25644f4e52b07bbba4c459256c910228d440e25ff0d288238e76047fd21000c4c7c; loaded code SHA256b14a214100bbf00c5bd25592c753eedaa4326375343699d6748933c35be2b11a,2036bytes. The recheck wrapper verifies source invocation hashes and ELF before running, target proof is acquired live. These Oct9 artifacts postdate the Oct8 snapshot provenance catalog; use their local invocation/deployment/raw manifests rather than inventing a snapshot catalog entry. Copied release C files can differ from the historically compiled ELF: never infer exact compiled behavior from a neighboring source alone.

## K02476
Source `D0367`, original line1, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

# PICO tracker + Steam Frame coexistence: state after Phase91–99 (for AI readers)

## K02477
Source `D0367`, original line3, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09, written by Claude Opus 5.5. English, numbers first. Chronological detail with
per-step evidence: WORKLOG_CLAUDE_2026-10-09.md (original reference: `WORKLOG_CLAUDE_2026-10-09.md`) (E1–E19).
Earlier full handoff: RESUME_COEXISTENCE_98_100_2026-10-09.md (original reference: `RESUME_COEXISTENCE_98_100_2026-10-09.md`).

## K02478
Source `D0367`, original line7, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

## Goal
98–100% tracker reception (CRC RX / completed TX, IMU continuity) while BOTH Frame
Valve controllers stay healthy (guard: 2 controllers valid, unchanged pose ≤150 ms).
**Not achieved yet.** No OTA, no Frame flash in any step below. Every hardware run
ended with session closed, RAM/MPU/trace restored and verified by readback.

## K02479
Source `D0367`, original line13, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

## Hardware and identities
Frame SSH steamos@[DEVICE_IP], work dir /home/steamos/pico-ram-work. Tracker
[TRACKER_SERIAL], chip LE [TRACKER_ID], firmware 189 with fork revision07 installed,
**mode1 (Frame cadence) verified fresh in every run since Phase92**. ESB 2 Mbit/s on
channel 2 (working address).

## K02480
Source `D0367`, original line19, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

## Established facts (OBSERVED or CONFIRMED)
