# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K00561
Source `D0211`, original line36, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

Prepared an isolated Frame release at `/home/steamos/pico-ram-work/ota45/release`:
copied measured OTA39 runtime, changed only exact-image admission modules.
Normal release symlink, automatic mode policy and radio service are unchanged.
The firmware binary itself is unchanged; approved_for_ota=false is preserved.

## K00562
Source `D0211`, original line41, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

## Hardware preparation results

## K00563
Source `D0211`, original line43, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

Initial45s no-OTA query (`outputs/ota45/ota45-check/`): fresh exact serial/FW189;
2193 TX-END,2156 CRC-valid RX (~98.31%),2147 retained answers,9 ring drops.
No fresh mode query proof; do not infer exact mode from firmware version189.
RF responsiveness under the stock service cadence is measured, not a new
cryptographic firmware readback or profile-transition proof.

## K00564
Source `D0211`, original line49, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

Separate45s guarded OTA-adapter preparation (`outputs/ota45/pause/`):
`--pause-before-ota-start` enabled throughout. Result:
`state=paused_before_ota_start`, `ota_start_sent=false`, no error,
identity serial[TRACKER_SERIAL]/FW189/chip[TRACKER_ID].
No OtaSession was instantiated, so no prestart/start/data/finish OTA was sent.
Controller guard passed. Both sessions closed and restored SRAM/MPU/trace;
their cleanup.json files record all four checks true.

## K00565
Source `D0211`, original line57, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

## Boundary for the next action

## K00566
Source `D0211`, original line59, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

`outputs/ota45/attempt/` contains a fresh unexecuted job, image, manifest,
registry, isolated controller-free config and frozen invocation/source hashes.
`outputs/ota45/run45.py` requires the explicit confirmed-start argument,
checks no previous result exists and refuses concurrent loader/OpenOCD/service.
The runtime rechecks live identity before creating the sender; no automatic
retry or resume. This script has NOT been executed.

## K00567
Source `D0211`, original line66, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

Final human confirmation at this exact pre-OTA boundary is still required by
the user's earlier instruction. Grok4.7-medium admission/pause review (original reference: `OTA45_REVISION07_GROK_REVIEW_2026-10-08.md`)
completed with no concrete code blocker; no hardware promotion is inferred.
The frozen isolated sender hash matches measured OTA39
`08d4fc51a2319f73808af18c23156a1097e9fe37164e82ac79aaf72f0e7abca8`;
run45 verifies the frozen hashes before invoking the loader. Normal release
sender is not used by this isolated invocation. The broader existing profile
allowlist was retained; this exact job selects only measured service38.
Bootloader/recovery, actual07 activation, switching/coexistence and A1 remain
unverified or explicitly limited. Installed05 has not been replaced.

## K00568
Source `D0212`, original line1, SHA256 `5b6607ae8b6edd0db6e20fcad05a9b0f7396850afcb5fc3e4cf4006896492bcc`. Status: historical/unadjudicated narrative; English source prose.

# OTA46: phone-off repeat preparation, paused before start

## K00569
Source `D0212`, original line3, SHA256 `5b6607ae8b6edd0db6e20fcad05a9b0f7396850afcb5fc3e4cf4006896492bcc`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User requests another attempt and confirms phone OFF, both Frame
controllers OFF, target [TRACKER_SERIAL] ON. Phone interference is a hypothesis,
not an established explanation of OTA45 failure (original reference: `OTA45_REVISION07_FAILED_2026-10-08.md`).
The failed job and all original evidence remain preserved.

## K00570
Source `D0212`, original line8, SHA256 `5b6607ae8b6edd0db6e20fcad05a9b0f7396850afcb5fc3e4cf4006896492bcc`. Status: historical/unadjudicated narrative; English source prose.

No firmware or sender changes. Same exact revision07 image97832B,
SHA40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5,
same measured service38 and isolated `/home/steamos/pico-ram-work/ota45/release`.
Frozen source hashes and image hash checked. New directory/job prevents replay.

## K00571
Source `D0212`, original line13, SHA256 `5b6607ae8b6edd0db6e20fcad05a9b0f7396850afcb5fc3e4cf4006896492bcc`. Status: historical/unadjudicated narrative; English source prose.

45-second guarded live preparation completed with exact serial/FW189/chip[TRACKER_ID],
`state=paused_before_ota_start`, `ota_start_sent=false`, no error.
This proves a responsive target and adapter readiness, not native OTA-idle state
or exact running firmware. Internal interrupted-OTA state remains unknown;
no reset, finish, resume, or data command was sent in this preparation.
Controller guard passed. Session/SRAM/MPU/trace cleanup all true.
Evidence `outputs/ota46/pause/`; staged unexecuted job `outputs/ota46/attempt/`.

## K00572
Source `D0212`, original line21, SHA256 `5b6607ae8b6edd0db6e20fcad05a9b0f7396850afcb5fc3e4cf4006896492bcc`. Status: historical/unadjudicated narrative; English source prose.

The earlier instruction to pause immediately before OTA remains active.
Final immediate-start confirmation is required before invoking run46.
A future attempt starts from a new start operation/block1, never block2560;
its start ACK must establish native acceptance. No automatic retry permitted.
No claim that phone removal fixes the failure, hardware recovery, or A1.
`approved_for_ota=false` remains.

## K00573
Source `D0213`, original line1, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

# OTA46 revision07: complete physical transfer, activation pending

## K00574
Source `D0213`, original line8, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

## Measured result

## K00575
Source `D0213`, original line10, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

Exact revision07 image97,832B SHA256
`40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5`.
All4077 indexed blocks acknowledged, positive finish and final drain,
state finish_verified, error null. 19 retry intervals,4078 response polls,
5296 rejected unrelated/stale/telemetry observations. Loader exit0.
Service38 and isolated45 release preserved; no firmware/source changes.

## K00576
Source `D0213`, original line17, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

Block2560, where45 timed out, was passed successfully. This disproves a
mandatory failure at that block under these conditions. Phone interference
remains a hypothesis: a single successful retry with phone OFF cannot establish
causality or exclude intermittent RF/queue scheduling failures.

## K00577
Source `D0213`, original line22, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

100% application block acknowledgement is not100% RF reception, cryptographic
flash readback, bootloader/recovery proof, or controller coexistence evidence.
Native finish CRC validation and host image identity do not prove every byte
of the running internal-flash image. No revision07 runtime query yet.

## K00578
Source `D0213`, original line27, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

## Cleanup and activation boundary

## K00579
Source `D0213`, original line29, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

All cleanup checks true: session closed, SRAM restored, MPU restored,
trace controls restored. Radio unit inactive, no loader/OpenOCD remains.
No Frame flash writes. No automatic second transfer. Do not replay this job.

## K00580
Source `D0213`, original line33, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

User was asked for normal button power-off/on to verify activation, following
the historically successful05 activation procedure. The new query must prove
revision07 functionality using a fresh nonce and exact chip, not merely FW189,
LED color, or a legacy fe01 response common to05/07. Activation is pending;
boot_verified=false and cadence_verified=false in transfer result.
Manual mode switching, automatic headset policy and RF coexistence are still
separate unverified milestones. A1 remains the recorded revision07 limitation.

## K00581
Source `D0213`, original line41, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

Preserved evidence: `outputs/ota46/attempt/` including job/image/manifest,
frozen invocation, raw event/publication logs, radio reports and cleanup.
Prior45 failure and46 paused preparation are unchanged. This record is linked
from AGENTS and execution status. Hardware approval flag remains false.

## K00582
Source `D0213`, original line46, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

## Post-button verification: responsive, no07-specific proof

## K00583
Source `D0213`, original line48, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

User confirmed full button off/on.45s no-OTA/no-setter schema2 query probe:
2285 TX-END,2243 CRC-valid RX (98.16%),2239 retained replies,3 ring drops,
2141 IMU samples, fresh exact serial/FW189. No matching fe02/nonce/chip/revision7
response was recorded. This is a negative capability-query result, not proof
that OTA activation failed or that stock05 is running. Actual running fork
revision and applied mode remain unconfirmed. All station cleanup true.

## K00584
Source `D0213`, original line55, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; English source prose.

Probe uses a process-local wrapper around measured legacy peer engine:
only query7e01 is changed to7e02, response matcher logs fresh exact26B fe02,
chip/revision7/nonce within8s. No remote7d setter or OTA job. Legacy runtime UI
cannot accept07 and thus remains unverified; no05 response was relabeled07.
Proof log absent means matcher obtained no required fresh response.
Evidence `outputs/ota46/verify07b/`, source `query07_loader.py`.
Initial probe `verify07` failed to import ownership BEFORE hardware entry;
fixed the wrapper's sys.path and used a new evidence directory, no replay.
Further diagnosis should examine reliable command/session delivery and native
activation before considering any additional flash. Successful transfer must
not be repeated solely because this query was inconclusive.

## K00585
Source `D0214`, original line1, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

# Iteration36: capture-ACK candidate, PAUSED by user before another OTA

## K00586
Source `D0214`, original line3, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User explicitly requests pause and status. **Stop all task work;
do not launch further RF, SWD, OTA, deployment or research until resumed.**
There is no active OTA. Attempts32/34/35 remain failed at block1,0/4056 ACKed.
No new tracker OTA start/data/finish was sent in iteration36.

## K00587
Source `D0214`, original line8, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

## Current work and evidence

## K00588
Source `D0214`, original line10, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

Read AGENTS and historical attempt34/35 before changes. The current user had
authorized fixing and flashing; this later explicit pause overrides that start.
Candidate `pc/radio/ram/roy_ram_station_service36.c` executes one-shot S1 ACK
only after a checksum-valid, hardware-CRC-valid full reply is committed to the
ten-record host ring. Every RX END clears the previous token; a corrupt frame,
oversize frame or full ring cannot acknowledge an unretained latest reply.
The low ACK bit overrides the host bit, including ACK-only polls. One global
token requires exactly one active slot0 peer; loader rejects other configurations
before opening debugger/RF. This can release repeated command22 notifications
ahead of OTA ACK. This is not an established complete fix for the live block1 failure.

## K00589
Source `D0214`, original line21, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

Final candidate ELF: `outputs/ota36/build-final/roy_station.elf`,2024 bytes,
ELF SHA256 `05c3c84f20ba0d07867fd09b70e5d850e4f9d3e13a105e216ffa85b772992604`,
code SHA256 `d68b808dff0ca1a172d76a32b7e52209914172f52d98a2523290e3d0802adb4a`.
Original service27 ELF and tracker revision05 image remain unchanged.
The loader/runtime/readiness Python supports narrowly constrained service36;
host originals were backed up on Frame under ota36/before-install.
The new module was selected by explicit path for temporary SRAM only;
normal service's module file was not replaced. No Frame flash was written.

## K00590
Source `D0214`, original line30, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

## Corrected diagnostic mistake, preserved failed intermediate

## K00591
Source `D0214`, original line32, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

The first candidate incorrectly treated RADIO+0x40c (RXCRC value) bit0 as CRC
validity and a draft host filter repeated that error. The ordinary non-OTA smoke
exposed roughly half the captured packets being rejected. Those artifacts remain
under outputs/ota36/build/,initial-service36.c,smoke-initial/ and intermediate XML.
Final code uses the actual CRCSTATUS gate at RADIO+0x400, restores the host's
original validity interpretation and invalidates any previous token on invalid END.
The official Nordic register map confirms these distinct meanings:
nRF52833 RADIO (original reference: `https://docs.nordicsemi.com/r/bundle/ps_nrf52833/page/radio.html`).
Tests now vary RXCRC parity independently of CRCSTATUS. Do not deploy the initial
0619a96f... ELF or use its intermediate passing emulator as hardware validation.
It is no longer accepted by the current service36 profile hash guard.

## K00592
Source `D0214`, original line44, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

## Offline validation

## K00593
Source `D0214`, original line46, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

Final suite **102 passed**, `outputs/ota36/final-tests.xml`. It executes compiled
Roy callback plus stock tracker RX/deferred scheduler/OTA worker, transfers the
full97320-byte revision05 fork with10% modeled loss each direction, two reliable
command22 records per block, IMU priority and deliberate host-ring overflow.
It asserts exact staging,4056 ACKed blocks,one erase/4056 writes, and nonzero ring
drops. CRC/checksum/full-ring/latest-invalid/one-shot/deadline and exact profile,
target and CLI authorization checks also pass. MMIO,time,SPI,IRQ,allocator remain
modeled/mocked; no physical OTA or boot/recovery claim follows.
Grok4.7 medium independently reviewed native last-reply ACK-list ordering and
identified the separate native scheduler-admission/same-sequence stall risk.
That second risk has not been repaired or established as the hardware cause.

## K00594
Source `D0214`, original line58, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

## Latest ordinary hardware smoke, no OTA job

## K00595
Source `D0214`, original line60, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

Final45s smoke confirmed the same serial[TRACKER_SERIAL]/chip[TRACKER_ID]/FW189,
controllers off. Last saved totals:2720 actual TX-END,2574 CRC-valid RX,
2361 retained unique replies,213 ring drops. CRC RX/TX94.6324%, retained/TX86.8015%.
These are whole-run ordinary radio metrics, not firmware application ACK delivery.
Ring loss is significant; do not call this ready for loss-free physical flashing.
New policy must not acknowledge those dropped packets; its compiled tests cover
that rule, but a complete physical OTA has not been attempted with this module.

## K00596
Source `D0214`, original line68, SHA256 `be88ccbdbc9af24394c016712a86df726615c9da6cb56043fb587f523e70f0b4`. Status: historical/unadjudicated narrative; English source prose.

Smoke final session/SRAM/MPU/trace cleanup all verified; unit inactive and normal
custom radio inactive. Pause-time process check finds no loader/OpenOCD.
Final smoke files currently reside on Frame at ota36/smoke-final; only the initial
smoke had been copied locally before pause. Final XML/build artifacts are local.
Detailed integration and new physical OTA are unfinished. Do not resume implicitly.

## K00597
Source `D0215`, original line1, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

# Hardware OTA attempt32: start ACK observed, block1 timed out

## K00598
Source `D0215`, original line3, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. The user reported rebooting the tracker and explicitly requested
start, lifting the previous pause. Earlier acceptance of unavailable SWD,
opening and recovery remained the accepted risk. A fresh nonreplayable job was
created; the paused iteration31 result was preserved and not reused.

## K00599
Source `D0215`, original line8, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

## Hardware outcome

## K00600
Source `D0215`, original line10, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

Live serial[TRACKER_SERIAL]/chip LE3eed1979 and firmware189 were checked by the
adapter before sender creation. HMD was valid and both controllers disconnected.
Exact97320-byte revision05 forkSHA256
`395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f`
and measured service27 ELF/profile passed the installed preflight.

## K00601
Source `D0215`, original line16, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

The sender passed its empty-reply preparation, transmitted start, and observed
valid `25 01 01` application ACKs. It moved through ACK-drain into data block1.
Block1 received no acceptable application ACK in10 seconds; after40 retry
intervals the sender failed with `OTA data block 1: application ACK deadline expired`.
**Zero of4056 blocks are acknowledged. Finish was not transmitted.**

## K00602
Source `D0215`, original line22, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

This was a real OTA start attempt, not a read-only probe. Start may have erased
external SPI staging. No staging dump or internal flash readback is available.
Start ACK is not independent proof of session identity, native idle or absence
of a previous worker. The known unmarked-ACK limitation remains unresolved.
Do not describe this as a successful installation or a verified unchanged flash.

## K00603
Source `D0215`, original line28, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

No automatic resume, reflash, tracker reset, finish or activation command followed.
The loader stopped and verified Roy session closure, SRAM, MPU and trace
restoration. The custom radio and OTA unit are inactive; no loader/OpenOCD remains.

## K00604
Source `D0215`, original line32, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

## Saved protocol evidence

## K00605
Source `D0215`, original line34, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

`outputs/ota32/capture/job/ota-result.json` records failed/block1,
acked_blocks0,retries40,rejected37,boot_verified=false,cadence_verified=false,
native_ota_idle_verified=false. The image hash is the intended source hash,
not evidence that the tracker stored that image.

## K00606
Source `D0215`, original line39, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

Event timestamps, as UTC Unix seconds:

## K00607
Source `D0215`, original line41, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

-1791481235.469239: prepare reply accepted, sender state start;
-1791481235.6437182: start ACK accepted, state drain_ack, next block1;
-1791481236.3034472: exact empty drain reply accepted, state data/block1;
-approximately10 seconds later: data deadline expired and loader cleaned up.

## K00608
Source `D0215`, original line46, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

Primary responses are retained in `radio-run.json`, with560 CRC-valid RX counted
by the last header and two dropped records. Because the runtime raised before
the normal loop-end summary, slots_granted/final TX counter diagnostics are absent;
do not invent a final TX-END denominator from scheduled slots.

## K00609
Source `D0215`, original line51, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

The working-record decoder found three start ACK observations at slots36,37,38:
two tagged16 and one tagged0. It found no data or finish application ACK.
During block1,496 retained records carry the attempted transmission tag20;
responses contain IMU/status telemetry, including command22 records of unknown
meaning. This demonstrates ongoing radio reception, not proof that the tracker
accepted or processed the long data command. One record not parsed by the
working decoder is not classified as corruption; bootstrap has a separate format.
Analysis is `outputs/ota32/response-analysis.json`.

## K00610
Source `D0215`, original line60, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

The live cause remains unknown: request reception/parser/dedup, worker execution,
session state, response-queue scheduling or record capture must be separated in
the next offline audit. Ordinary polling success and the earlier full ARM staging
simulation did not prove the live block-ACK path.

## K00611
Source `D0215`, original line65, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

## Post-failure health check

## K00612
Source `D0215`, original line67, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

A separate bounded20-second ordinary service-radio check, without an OTA job,
produced37 samples with live confirmed serial[TRACKER_SERIAL] and firmware189.
The last live observation reported878 decoded IMU samples,927 retained replies,
940 TX-END and fresh RF/IMU timestamps. The tracker application was responding.
Firmware189 alone cannot distinguish stock from the fork; no fork-specific mode
query or verified boot/activation was performed. This check does not establish
the contents of internal flash or external staging.

## K00613
Source `D0215`, original line75, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

The health check also verified Roy session/SRAM/MPU restoration and stopped
normal custom radio afterward. Captures: `health-live.json`,
`health-radio-live.json`, `health-radio-service.json` under the same capture folder.

## K00614
Source `D0215`, original line79, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

## Authorization and preserved state

## K00615
Source `D0215`, original line81, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

The invocation used explicit CLI `--authorize-unverified-fork`, exact-image/target
constraints, fresh result creation and no pause flag. Its wrapper has Restart=no
and forwards stop signals to the loader; no automatic second OTA is permitted.
The sender's failed job/result must not be replayed or overwritten.

## K00616
Source `D0215`, original line86, SHA256 `feefc1e6c7f2d36de6808d387e4e1698878670f7384e488846a8361a6630c61c`. Status: historical/unadjudicated narrative; English source prose.

Historical firmware/source/reports and the iteration31 pause capture remain intact.
New scripts, captures and analysis are indexed in
`outputs/ota32/artifact-manifest.json`. Next destructive attempt requires a new
reviewed plan and explicit user instruction; do not retry merely by increasing
timeouts or clearing the failed result.

## K00617
Source `D0216`, original line1, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

# Hardware OTA attempt34: corrected sender still failed block1

## K00618
Source `D0216`, original line3, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User reported attaching the charging cable and explicitly authorized
starting if the tracker was visible. This lifted iteration33's pause for one
fresh attempt. Charging current or USB enumeration was not independently measured.

## K00619
Source `D0216`, original line7, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

## Authorization and live target

## K00620
Source `D0216`, original line9, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

Read AGENTS, iteration33 response-budget audit, failed32 and its saved runner;
new attempt used the preserved reviewed package and exact installed source hashes.
Snapshot provenance remains in `outputs/ota33/provenance.json`; the binary and
service27 ELF were unchanged. Old paused/failed jobs were not replayed.

## K00621
Source `D0216`, original line14, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

Fresh `ota34/job/` targeted slot0,chip LE3eed1979,
serial[TRACKER_SERIAL],FW189. Runtime checked fresh decoded product identity
before sender creation. The HMD was connected/valid; both controllers were
disconnected. Loader controllers-off guards remained active. User's earlier
acceptance of unavailable SWD/opening/recovery remained the accepted risk;
those unknowns were not reclassified as verified facts.

## K00622
Source `D0216`, original line21, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

Exact97320-byte revision05 imageSHA256
`395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f`.
SenderSHA256 `b3fa958b741e05332720c5aaff78b5da27d376a981d18e81f0b1e56bc45560a2`;
runtimeSHA256 `807cb09542a4e52adfd8215708cf5c671c1dac3ee72411e39ab89a31522bf549`;
service27 ELF `65d217e5e47e8028a6d58fe46c17923fcd20877dff27ffed64bc4dbf71f64a44`.
These hashes were checked before hardware execution. Restart=no; one attempt.

## K00623
Source `D0216`, original line28, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

## Actual outcome

## K00624
Source `D0216`, original line30, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

The tracker replied with valid start ACK250101. Sender drained that ACK and
entered data block1. Block1 received no acceptable application ACK within10s.
Result: **failed,0/4056 acknowledged,41 total retry intervals,39 short response
poll publications, final generation172; no finish publication**.
Retries include earlier setup/drain intervals, not41 separately proven data writes.

## K00625
Source `D0216`, original line36, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

Unix timestamps from host publication/events:

## K00626
Source `D0216`, original line38, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

-1791483938.6973: start publication,sequence0,generation14;
-1791483938.8502: start application ACK accepted,drain state;
-1791483939.6492: data block1 entered,sequence1,generation18;
-1791483949.6465: data deadline expired and sender failed.

## K00627
Source `D0216`, original line43, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

The automatic loader stop verified Roy session closure, SRAM, MPU and trace
restoration. No second OTA, automatic resume, tracker reset, finish or activation
was attempted. Normal custom radio is inactive and no loader/OpenOCD remains.
The attempt unit is failed/inactive,ExecMainStatus1.

## K00628
Source `D0216`, original line48, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

Start may have erased external SPI staging. No staging/internal-flash readback
exists. Absence of data ACK does not prove absence of a write. No completed
installation or verified unchanged flash is claimed. The unmarked-start-ACK
pending-worker race, CRC8 identity limitation and boot/recovery unknowns remain.

## K00629
Source `D0216`, original line53, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

## What the improved capture establishes

## K00630
Source `D0216`, original line55, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

Host `.tx.jsonl` explicitly records39 long block1 publications and39 alternating
short data polls. All long payloads are exactly the reviewed block1 record:
`01011d250200011800600020a51300005d1300008119000061130000631300008b`.
Sequence remains1; S1 remains2 while waiting for data ACK. Publication is not
direct RF request readback or proof of receiver dispatch/application acceptance.

## K00631
Source `D0216`, original line61, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

Tagged retained RX:120 associated with long-data publications and342 associated
with short-data publications. The short replies contain larger IMU records36/42
bytes and208 command22 body23 observations. They contain no data application ACK.
Command22 sequence2 repeatedly remains visible; sequence3 also appears near the
end. No exactly empty working reply is retained during the data-wait interval.
Two start ACK observations are tagged14 at slots131/132.
Analysis: `outputs/ota34/response-analysis.json`; complete TX/RX evidence is in
`capture/job/ota-result.tx.jsonl` and `capture/radio-run.json`.

## K00632
Source `D0216`, original line70, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

This proves that enabling short polls alone **did not fix the live failure**.
Iteration33's native response-budget defect remains a valid offline reproducer,
but it is insufficient as an established explanation/remedy for the hardware.
Reliable notifications/IMU continue to compete for response space; receiver
dedup/queue admission, worker execution and staging state remain unread.
Do not infer that ACK was merely hidden, or that the first block was definitely
accepted/written. The native model's empty/controlled telemetry schedule is not
the live queue workload. Do not retry by clearing the result or raising10s.

## K00633
Source `D0216`, original line79, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

Last saved header:653 scheduled slots,546 CRC RX,1 ring drop. Exception occurs
before normal loop-end TX-END summary, so there is no verified final completed-TX
denominator for this attempt. Scheduled slots must not replace that counter.

## K00634
Source `D0216`, original line83, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

## Separate post-failure health check

## K00635
Source `D0216`, original line85, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

A bounded20-second ordinary radio check, **without any OTA job**, produced35
live identified samples for the same serial/FW189. Last identified sample:
787 IMU samples,847 retained replies,929 completed TX, fresh RF/IMU timestamps.
The tracker application was responding after the failed attempt.
This is not verified fork identity, staging contents, recovery or boot validation.

## K00636
Source `D0216`, original line91, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

The health check also verified all four Roy cleanup/restoration fields and stopped
normal custom radio. Captures: `health-live.json`,`health-radio-live.json`,
`health-radio-service.json` under `outputs/ota34/capture/`.
No statement is made about the tracker's present sleep/power state after radio stop.

## K00637
Source `D0216`, original line96, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

## Preserved state and next boundary

## K00638
Source `D0216`, original line98, SHA256 `a5b29413b1a9c449ce89b6ccd0521de7f743ccab7c0c80e7ac78076802956bdd`. Status: historical/unadjudicated narrative; English source prose.

New research records are English; historical reports/results/images remain intact.
`outputs/ota34/artifact-manifest.json` indexes the runner, capture, health and
analysis. AGENTS and execution status link this actual failure.
Fork binary still revision05 and no verified installation has been achieved.
Do not replay failed34 or reuse its result. Another destructive attempt requires
a new reviewed diagnosis/plan and separate human instruction; no automatic retry
or hidden resume remains running.

## K00639
Source `D0217`, original line1, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

# Hardware OTA attempt35: unplugging charger did not restore block ACK

## K00640
Source `D0217`, original line3, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User explicitly requested one additional send attempt to test whether
charging caused attempt34's failure. User then confirmed: cable removed, tracker
powered on. Electrical charging current/status was not independently measured.
Read AGENTS and the original attempt34 report/runner before preparing this test.
The earlier charging attempt and all historical evidence were preserved.
