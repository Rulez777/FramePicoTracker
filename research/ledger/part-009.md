# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K00641
Source `D0217`, original line9, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

## Controlled change and authorization

## K00642
Source `D0217`, original line11, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

One fresh nonreplayable job in `ota35/job/`; same exact revision05 image, sender,
runtime, service27 ELF, radio profile, S1/sequence policy and deadlines as attempt34.
No protocol/software change or timeout increase was introduced for this retry.
Source/ELF hashes were checked by the runner before hardware execution.
Source and firmware provenance remains in iteration33's manifest verification;
attempt34 documents exact deployment hashes. This is newer live research.

## K00643
Source `D0217`, original line18, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

Runtime required fresh live serial[TRACKER_SERIAL],chip LE3eed1979,FW189 before
sender creation. Both controllers were disconnected; HMD connected/valid.
The user authorized this new attempt after the previous failure and explicitly
confirmed unplugging. Earlier noSWD/recovery risk acceptance remained in scope;
boot/recovery/clean startup were not newly verified. Restart=no, one attempt.

## K00644
Source `D0217`, original line26, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

Start ACK250101 was observed and accepted. ACK drain completed, then data block1
again timed out after10s without application ACK. **0/4056 blocks acknowledged**,
53 total retry intervals across setup/drain/data,39 short response polls,
final generation176, command sequence1. No finish publication or activation.

## K00645
Source `D0217`, original line31, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

Captured RX associated with host publication tags:129 long-data and325 short-data
responses. There was no exactly empty working reply during data wait, and no
data application ACK. Three start ACK observations: two tagged18,one tagged0.
Host TX log records39 identical long block1 publications. Publication/response
association is not tracker request readback, receiver dispatch or staging proof.
Analysis: `outputs/ota35/response-analysis.json`; raw evidence under `capture/`.

## K00646
Source `D0217`, original line38, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

The result repeats attempt34's first-block failure after user-confirmed cable
removal. **Removing charging alone did not restore OTA.** This does not prove
charging has no effect on every RF/timing behavior or establish the unique cause.
The prior native queue-budget defect is offline-valid, but the live block worker,
queue admission/dedup and staging state are still unknown. No successful firmware
installation or verified unchanged internal flash is claimed. Start may erase SPI
staging; missing data ACK does not prove no write occurred.

## K00647
Source `D0217`, original line46, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

## Stop, cleanup and separate application health

## K00648
Source `D0217`, original line48, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

The sender failed terminally; no automatic retry/resume, tracker reset, second
start, finish or activation followed. Roy session/SRAM/MPU/trace restoration all
verified. Attempt unit failed/inactive,ExecMainStatus1. No loader/OpenOCD remains.

## K00649
Source `D0217`, original line52, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

A separate bounded20s ordinary radio check, without an OTA job, produced37 fresh
identified samples for the same serial/FW189. Last live observation:807 IMU
samples,871 retained replies,890 completed TX and fresh RF/IMU timestamps.
The application responded after the failed attempt. This does not prove fork
identity, staging contents, bootloader/recovery or absence of flash changes.
Health check cleanup verified all four restoration fields and stopped normal
custom radio. Current radio is inactive; tracker sleep/power after stop is unknown.

## K00650
Source `D0217`, original line60, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

## Research records and next boundary

## K00651
Source `D0217`, original line62, SHA256 `654501e9cb3b06f55847543d58c6a75280af9896491f490a4e7811b74d6c4fef`. Status: historical/unadjudicated narrative; English source prose.

Image remains97320-byte dual-mode revision05,SHA256
`395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f`.
No implementation change or new test campaign: this iteration is the specifically
authorized hardware comparison. New English report is linked from AGENTS and
execution status; `outputs/ota35/artifact-manifest.json` indexes preserved evidence.
Do not replay failed35 or reuse its result. No further OTA is left running.
Another destructive attempt requires a new reviewed change/diagnosis and fresh
human instruction; do not repeat the same experiment merely by clearing results.

## K00652
Source `D0218`, original line1, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

# Iteration37: resumed OTA work, ordered host publication, target absent

## K00653
Source `D0218`, original line3, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User explicitly resumed after pause36, confirmed the target is
exclusive to Frame, and authorized flashing when the corrected path is ready.
Exact target remains [TRACKER_SERIAL]/chip[TRACKER_ID]; revision05 image unchanged.
No OTA start/data/finish has been transmitted in this iteration at this point.

## K00654
Source `D0218`, original line8, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

## Prior evidence and narrow change

## K00655
Source `D0218`, original line10, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

Read root AGENTS, iteration36 pause/capture-ACK report, previous failed35 and
response-budget33 evidence. Preserve all earlier experiments. Final36 ordinary
capture was copied locally to outputs/ota36/smoke-final after resume.
Its 213 ring drops motivate reducing host publication latency; this is a
performance hypothesis, not a measured cause of every drop or OTA failure.

## K00656
Source `D0218`, original line16, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

Debugger.write_many now validates every region using the same RAM-only bounds
before any IO, then submits the ordered writes as one TCL script. MailboxWriter
uses it for odd generation, complete body, even commit. A TCL error aborts
before subsequent statements; failed publication is not cached. The old
write-only interface remains supported. No firmware, timing, OTA sequence,
retry timeout or image change. 38 focused loader/peer tests pass, including
validation-before-IO, order and failed-commit retry. Grok review pending.

## K00657
Source `D0218`, original line24, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

Frame originals backed up in ota37/before-install before installing the two
Python files. Candidate ELF remains final36 05c3c84f...604/code2024B, selected
explicitly; original normal module is unchanged.

## K00658
Source `D0218`, original line28, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

## Hardware: one ordinary 45-second run, no OTA job

## K00659
Source `D0218`, original line30, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

Evidence outputs/ota37/smoke-batched. Actual TX-END3087, CRC-valid RX2,
retained working replies0, ring drops0, no fresh serial/product/IMU.
Both raw records are channel1 DMA0902c10000000000001b1b. Their record's
source_chip_id is the configured slot identity, NOT an independently decoded
tracker identity. They do not establish the target is present or connected.
Do not call this a successful ring-loss comparison: the working link was absent.
User was asked to wake the target and report LED state; replied that the button
was pressed. LED state was not supplied.
After unpair, changed bond state or sleeping is only a hypothesis.

## K00660
Source `D0218`, original line40, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

Session/SRAM/MPU/trace cleanup all true. Unit inactive. No OTA runtime was
constructed by this no-job smoke. Do not start destructive OTA without fresh
live exact identity.

## K00661
Source `D0218`, original line44, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

## Wake confirmation, ordinary second run

## K00662
Source `D0218`, original line46, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

After user button press, one new ordinary45s run recovered fresh exact serial,
FW189 and3278 IMU samples. TX3025/CRC2983=98.6116%, retained2597=85.8512%,
ring dropped385. All cleanup flags true. Host batch alone did NOT eliminate
ring loss; this is not loss-free transport proof. Candidate36's retained-only
ACK and indexed application retries were tested offline with loss/overflow.
One fresh user-authorized physical OTA was launched next, preserving earlier
failed attempts; outcome pending. Guards still require live exact identity.

## K00663
Source `D0218`, original line54, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

## Native scheduler diagnosis, offline only

## K00664
Source `D0218`, original line56, SHA256 `23a9bb0da544541fcce4df8f9e3f5d0c0c13bf1d296beef4e9ad17904988e7d1`. Status: historical/unadjudicated narrative; English source prose.

New test pc/tests/test_scheduler_admission37.py runs actual native15d4/168c
and RX5a08. Deliberately fills deferred scheduler until return4, submits block1
sequence1 without scheduler execution: incoming expected sequence advances2,
block never writes. After draining scheduler, identical sequence1 retries are
ignored; a diagnostic sequence2 writes once. This proves the static admission
defect in the model, not that it caused hardware32/34/35. No recovery sequence
sweep is deployed: in RF old-ACK/admission races make blind sequence changes
unsafe. One new diagnostic test passed;68 additional sender/runtime guard tests
passed (full unchanged slow campaign excluded). Grok second opinion pending.

## K00665
Source `D0219`, original line1, SHA256 `8961660c6a4887fadc66c80dea5dbf11e332c7a7b978a18b43184be9dd7cd253`. Status: historical/unadjudicated narrative; English source prose.

# Iteration39: indexed progression under MCU capture ACK ownership

## K00666
Source `D0219`, original line3, SHA256 `8961660c6a4887fadc66c80dea5dbf11e332c7a7b978a18b43184be9dd7cd253`. Status: historical/unadjudicated narrative; English source prose.

Read OTA_TAG_ZERO38_2026-10-08.md and preserved actual38 first. User requests
fixing and proceeding. Physical38 wrote/ACKed549 blocks then the empty-reply
barrier timed out despite continued telemetry. No finish or installed fork claim.

## K00667
Source `D0219`, original line7, SHA256 `8961660c6a4887fadc66c80dea5dbf11e332c7a7b978a18b43184be9dd7cd253`. Status: historical/unadjudicated narrative; English source prose.

Service38 MCU owns retained-only reliable ACK retirement, excluding fallback
tag0. Sender now sets capture_ack=True ONLY for exact guarded service38.
On a positive indexed data ACK, advance immediately to next indexed operation
with fresh generation/eligible tags; no empty telemetry barrier between blocks.
Keep prestart, start drain and final drain unchanged. Actual application success
ACK still required for each block, exact index and current-operation published
tag checked. Old indexed ACK cannot acknowledge next block. No blind sequence
sweep, reconnect/resume, larger write payload or new image. Default/service27/36
sender policy unchanged. Native dispatcher enqueues cmd1 by sequence; current
application ACK follows worker completion. MCU can retire previous ACK list
independently while next data is received. Queue head blocking still uses short
response polls and retained-only ACK. Scheduler admission risk remains unknown
on hardware; no recovery proven.

## K00668
Source `D0219`, original line21, SHA256 `8961660c6a4887fadc66c80dea5dbf11e332c7a7b978a18b43184be9dd7cd253`. Status: historical/unadjudicated narrative; English source prose.

50 offline tests passed including full exact97320 image/4056 blocks through
compiled38+native RX with modeled10% directional loss/backlog/ring overflow.
Separate new test verifies retained start barrier and rejects old ACK both with
old and current tags after immediate indexed advancement. Peripherals/time/SPI
remain modeled; no physical completion follows from these results.

## K00669
Source `D0219`, original line27, SHA256 `8961660c6a4887fadc66c80dea5dbf11e332c7a7b978a18b43184be9dd7cd253`. Status: historical/unadjudicated narrative; English source prose.

Host originals backed up on Frame ota39/before-install. ELF38 and fork05 unchanged.
Fresh authorized physical39 wrapper checks all installed source hashes, ELF,
controller guard and live exact serial; one attempt, no automatic restart.
## Hardware result: complete transfer and fork response

## K00670
Source `D0219`, original line32, SHA256 `8961660c6a4887fadc66c80dea5dbf11e332c7a7b978a18b43184be9dd7cd253`. Status: historical/unadjudicated narrative; English source prose.

Exact live serial/chip/FW189 validated by adapter before destructive start.
All4056 indexed blocks ACKed, including empty sentinel, positive finish ACK,
then final empty drain. state finish_verified, no error,9 retry intervals,
4055 short response polls,9241 rejected unrelated/stale/telemetry observations.
Start-state to finish_verified382.9567s (6m22.96s),97320 payload bytes,
254.13 payloadB/s. This is100% application block acknowledgement for this run,
NOT100% per-packet RF reception or cryptographic device readback.
Frozen sent image SHA395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f.
Native finish validates CRC8 only; no full flash SHA256 readback was obtained.

## K00671
Source `D0219`, original line42, SHA256 `8961660c6a4887fadc66c80dea5dbf11e332c7a7b978a18b43184be9dd7cd253`. Status: historical/unadjudicated narrative; English source prose.

First no-OTA45s fork query immediately after transfer had no mode reply,
despite fresh serial and responsive RF. Missing reply alone did not prove
activation failed: cmd1 sequence/session and application startup were unknown.
User was asked to power off/on; answered blue LED on. No physical resetreason
or exact full-off was independently measured. Native activation distinction:
finish updates RAM verified metadata; fa98 writes persistent metadata/reset,
8548/855e uses argument1, power-off a638/a648 uses argument0. Their bootloader
behavior remains unproven; blue LED is not a firmware identity measurement.

## K00672
Source `D0219`, original line51, SHA256 `8961660c6a4887fadc66c80dea5dbf11e332c7a7b978a18b43184be9dd7cd253`. Status: historical/unadjudicated narrative; English source prose.

Second no-OTA45s check after user's button action obtained two actual native
reliable command1 fork responses, CRC-valid, schemafe01, different nonces,
chip[TRACKER_ID],mode0(stock),revision1. Runtime checked freshly generated nonce
and chip before reporting radio_mode_verified=true. This establishes execution
of the fork's added query path, which stock exact FW189 lacks; it does not
read back all97320 flashed bytes. Raw evidence fork-handshake-proof.json:
fe01d89c30763795c726e046adde77360cd03eed19790001,
fe0157791af20c2731185340c3255c374d7c3eed19790001.
Same serial/FW189,2406 IMU samples. Mode stock is correct for ordinary startup;
Frame button gesture, Frame cadence, optical6DoF, two-controller coexistence
and interruption recovery not validated by this run. pose_valid=false.

## K00673
Source `D0219`, original line63, SHA256 `8961660c6a4887fadc66c80dea5dbf11e332c7a7b978a18b43184be9dd7cd253`. Status: historical/unadjudicated narrative; English source prose.

Second check whole-run RF:3039 TX-END,2948 CRC RX=97.0056%,2546 retained
replies=83.7776%,402 ring drops. Host ring loss remains unresolved for ordinary
telemetry; reliable indexed OTA now survives it. MCU ACK custody alone is not
a generic guarantee that arbitrary unindexed data cannot be lost/corrupted.

## K00674
Source `D0219`, original line68, SHA256 `8961660c6a4887fadc66c80dea5dbf11e332c7a7b978a18b43184be9dd7cd253`. Status: historical/unadjudicated narrative; English source prose.

Transfer and both verification checks session/SRAM/MPU/trace cleanup ALL true,
units inactive, no loader/OpenOCD. Normal Frame module unchanged; no Frame
flash write. Evidence outputs/ota39/attempt/,verify-fork/,verify-after-button/,
transfer-summary.json,fork-handshake-proof.json,artifact-manifest.json.
Previous37/38 failed attempts remain preserved. Successful job must not replay.

## K00675
Source `D0220`, original line1, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

# Live OTA preflight31: user-requested pause before start, no tracker OTA

## K00676
Source `D0220`, original line3, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. The user stated that SWD and opening trackers are unavailable,
accepted proceeding with that risk and then requested start. During preparation,
the user explicitly superseded start with: pause immediately before actual OTA,
after the checks. The latest instruction is a pause, not permission to continue.

## K00677
Source `D0220`, original line8, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

## Exact outcome

## K00678
Source `D0220`, original line10, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

The live adapter reached `paused_before_ota_start` for stockFW189 tracker
serial[TRACKER_SERIAL], chip LE3eed1979. `ota_start_sent=false`.
No OtaSession was constructed, and no tracker OTA start/data/finish was sent.
The bounded65-second validation exited0 and verified Roy session closure,
SRAM, MPU and trace restoration. Both custom radio and the prestart unit are
inactive; no loader/OpenOCD remains. The result stays paused after cleanup.

## K00679
Source `D0220`, original line17, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

Primary capture: `outputs/ota31/final-capture/job/ota-result.json`,
`radio-prestart-run.json`, `prestart-cleanup.json`, `prestart-stdout.log`,
`prestart-live.json` and `radio-live-records.jsonl`.

## K00680
Source `D0220`, original line21, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

## Radio observations, do not merge the runs

## K00681
Source `D0220`, original line23, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

Initial normal-service preflight confirmed live identity and controller absence.
The selected earlier41.53-second window yielded2022 retained replies /2045
TX-END=98.875306%; its provisional99% threshold did not pass.
The saved final39.531388-second stable window after initial configuration yielded
1908/1912=99.790795% retained,1912 CRC-valid replies and2 ring drops.
This window was analyzed from the saved capture, not a new RF sweep.

## K00682
Source `D0220`, original line30, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

The later65-second paused-adapter run had3183 actual TX-END,2824 CRC-valid RX,
and2810 retained checksum-valid replies. Whole-run CRC/TX=88.721332%; retained
replies /actual TX=88.281495%. Its live UI uses scheduled slots as denominator
in this subclass and shows88.448222%; that is not the exact TX-END metric.
There was no captured stock fault and no host retry. Ring dropped13 records.
Most of this new gap is upstream of ring retention; its cause is not established.
Do not advertise this last run as99% or assume the earlier rate persisted.
No block/application-ACK throughput or bootloader behavior was measured.

## K00683
Source `D0220`, original line39, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

## First launch failed before hardware access

## K00684
Source `D0220`, original line41, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

The first attempted loader invocation failed while importing
radio.tracker_image_audit, before debugger creation, MCU setup or OTA result
creation. The wrapper also failed when it tried to read a nonexistent report.
The full traceback is retained in `final-capture/stdout.log`.

## K00685
Source `D0220`, original line46, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

The missing image-audit/grid-patch dependencies were installed with host backups.
An actual Frame-side import, exact image validation and effective-profile
preflight then passed without hardware access, recorded in
`final-capture/installed-preflight.json`. The failure was not a failed tracker
flash and does not justify an OTA resume. No automatic second OTA was performed.

## K00686
Source `D0220`, original line52, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

## Authorization handling and pause mechanism

## K00687
Source `D0220`, original line54, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

A narrow `--authorize-unverified-fork` CLI exception was added for the exact
iteration05 imageSHA395ce84b…14f,97320 bytes, stock-cadence slot0/chip[TRACKER_ID],
serial[TRACKER_SERIAL] and measured service27 profile. It reflects the user's
explicit accepted risk; it does not clear the underlying bootloader, recovery
or old pending-worker uncertainty. JSON claims are overwritten by CLI values.
Default hardware loading and all other images/targets remain blocked.
TrackerOtaRuntime additionally checks the fresh decoded serial before beginning.

## K00688
Source `D0220`, original line62, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

The later `--pause-before-ota-start` CLI option takes precedence in this actual
invocation. After normal target configuration, fresh product decode and hardware
preflight, the runtime records the paused identity and returns before creating
OtaSession. There is no automatic release token or timer that starts OTA.
Cleanup does not relabel this intentional pause as a failure.

## K00689
Source `D0220`, original line68, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

118 prewrite regression tests passed. After the pause implementation,63 focused
pause/authorization/runtime/readiness/loader tests passed. A mocked constructor
fails if OtaSession is ever created in the ready paused path; repeated polls and
cleanup keep `paused_before_ota_start` and `ota_start_sent=false`.
Tests and sources are saved/hash-indexed under `outputs/ota31/`.

## K00690
Source `D0220`, original line74, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

## Current stop boundary

## K00691
Source `D0220`, original line76, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

The user's latest instruction remains in force. Do not run an unpaused OTA job,
delete/replay the existing result, create an automatic resume, or send reset,
activation or destructive staging probes. A future explicit instruction is
needed to leave this pause. The saved job/result is nonreplayable; any future
authorized attempt requires a fresh reviewed invocation and live checks.

## K00692
Source `D0220`, original line82, SHA256 `44e756a70efdf090173bbf20f53eb9e625fbcb97c5a03ba84f8cb807b5a3c5d9`. Status: historical/unadjudicated narrative; English source prose.

The channel-rate change and the existing bootloader/recovery/clean-start risks
remain visible limitations. The fork binary has not been modified or installed.

## K00693
Source `D0221`, original line1, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

# OTA preparation30: exact fork and service27 adapter, hardware stop retained

## K00694
Source `D0221`, original line3, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User authorized preparing the transfer and explicitly stopping before
flashing a real tracker. No SSH, SWD, RF, real OTA start/data/finish, reset or device
operations occurred. The exact iteration05 firmware is unchanged.

## K00695
Source `D0221`, original line7, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

## Concrete package

## K00696
Source `D0221`, original line9, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

`outputs/ota-prepare30/package/` contains:

## K00697
Source `D0221`, original line11, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

- image.bin:97320-byte dual-mode revision05, SHA256
  `395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f`;
- original patch-manifest.json and stock-reference.bin, verified by exact reverse;
- service27.elf:SHA256
  `65d217e5e47e8028a6d58fe46c17923fcd20877dff27ffed64bc4dbf71f64a44`;
- isolated station-config.json:4727us interval,900us slot, cycle1,N0/C2,
  margin100us, optics/BLE phase-lock off, controllers-off guard, peers1/2 paused;
- job.json, preparation registry, negative launch proofs in PREPARATION.json,
  generated file hashes in manifest.json and a stop-boundary README.

## K00698
Source `D0221`, original line21, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

Target identity is bound to the saved iteration28 measurement: slot0,
chip LE3eed1979, serial[TRACKER_SERIAL], stockFW189. The registry is explicitly
historical evidence, not a fresh physical identity or controller-state check.
Source configuration, ELF, measurements and firmware bundle are preserved.

## K00699
Source `D0221`, original line26, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

## Adapter preparation and isolation

## K00700
Source `D0221`, original line28, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

`pc/radio/tracker_ota_fork_prepare.py` validates all inputs offline, creates a new
directory without replay/overwrite, copies the exact files, and demonstrates two
independent rejections before declaring the package prepared.

## K00701
Source `D0221`, original line32, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

`load_job(..., preparation_only=True)` now validates the exact expanded fork
against its fixed SHA256, known size and reverse manifest. Ordinary hardware
load_job rejects that action. The CLI has no preparation-only launch flag, and
the hardware image whitelist is unchanged. This is an offline parsing path,
not an approval JSON bypass or a new hardware authorization.

## K00702
Source `D0221`, original line38, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

`plan_preflight` can now validate an explicitly selected service27 radio_profile.
It requires the exact measured settings and ELF, stock cadence, slot0,
controllers off and the paused peer set. It rejects dual-mode handshake config,
other tracker slots, combat ELF, other interval/N/margin and optical operation.
The independent hardware preflight still checks clearance first and rejects the
prepared job on the existing bootloader/recovery/clean-start requirements.

## K00703
Source `D0221`, original line45, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

Job result paths now also exclude the patch manifest, preventing the status file
from overwriting the evidence used to validate the image. Image, job and registry
path exclusions and existing-result replay protection remain active.

## K00704
Source `D0221`, original line49, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

## Integrated offline transfer

## K00705
Source `D0221`, original line51, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

The new complete-image regression exercises TrackerOtaRuntime.record, target
identity filtering, application ACK parsing, sender transitions, status/event
files and seqlocked MailboxWriter publication. Replies come from actual stock
FW189 ARM dispatcher/queue/dedup/OTA worker instructions. Every127th reply is
omitted; periodic wrong-chip records are presented before the proper record.
SPI, allocation, IRQ and SWD are modeled, not real hardware.

## K00706
Source `D0221`, original line58, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

The test explicitly injects an OtaSession into a simulated runtime. It does not
claim that runtime pre-start clearance passed, does not bypass production
hardware gates and does not demonstrate a live startup or bootloader.

## K00707
Source `D0221`, original line62, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

Expected completion is4056 acknowledged blocks, one erase,4056 native staging
writes and byte-for-byte equality with the full fork. All simulated SWD writes
remain in target-slot mailbox0. Successful finish still has boot_verified=false;
the CRC8 and old pending-worker limits from audit29 remain unchanged.

## K00708
Source `D0221`, original line67, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

**89 tests pass,348.30 seconds**, including the full runtime integration and
previous sender/native/readiness regressions. The initial focused preparation
run separately passed10 tests. The full runtime case exercises real status/event
filesystem writes; it is an offline software test, not measured radio duration.
Its retained output is `outputs/ota-prepare30/runtime-final.json` and
`runtime-events.jsonl`:finish_verified,4056 acknowledged blocks, image identity
as expected, native_ota_idle_verified=false,boot_verified=false,
cadence_verified=false. Wrong-chip records leave transfer progression unchanged.

## K00709
Source `D0221`, original line76, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

Validation results are recorded in `outputs/ota-prepare30/tests.xml` and the
outer artifact manifest. The retained earlier audit29 fault campaign also covers
bursts, corrupt replies, tag exhaustion boundary, hidden ACKs and CRC8 collision.

## K00710
Source `D0221`, original line80, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

## Exact stop and outstanding hardware evidence

## K00711
Source `D0221`, original line82, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

Preparation stops before the first real OTA start command. Start already erases
external staging; no real "stage-only" start/finish probe is authorized here.
No code was deployed to Frame or enabled on a tracker in this iteration.

## K00712
Source `D0221`, original line86, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

Software package preparation is not complete hardware readiness. Still needed:
verified bootloader acceptance of97320 bytes, recovery independent of a healthy
application, a proven clean startup with no pending old OTA worker, fresh live
target/controller checks and measured block-ACK throughput. The latest procedure
for obtaining bootloader evidence remains
SWD plan revision11 (original reference: `FW189_SWD_READONLY_PLAN_2026-10-08.md`).
Target/probe/tool/pinout for tracker SWD have not been assigned. The plan itself
does not authorize attachment or destructive recovery.

## K00713
Source `D0221`, original line95, SHA256 `ac1a4ffbd6e0dc03b8fd5e75a258fa40591ce21737c010fe4d6dc2616cb77f49`. Status: historical/unadjudicated narrative; English source prose.

Historical report audit29 (original reference: `OTA_TRANSFER_AUDIT29_2026-10-08.md`) described the
adapter gaps before this preparation. Expanded-image parsing and measured-profile
validation now exist in the explicitly offline path; actual hardware activation
remains blocked and no prior measurement is relabeled as firmware-transfer proof.

## K00714
Source `D0222`, original line1, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

# OTA iteration33: response-budget defect, corrected sender, live pause

## K00715
Source `D0222`, original line3, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. Current instruction: prepare and check everything, stop immediately
before the actual OTA procedure. Earlier authorization of attempt32 does not
authorize another start. **No new tracker OTA start/data/finish was sent.**

## K00716
Source `D0222`, original line7, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

## Historical evidence and provenance

## K00717
Source `D0222`, original line9, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

Read attempt32, preparation30, transfer29, pause31, revision05 mode handshake,
revision06 boot/recovery, the independent GRID3000 countercheck and SWD plan11.
The initial snapshot remains historical. Reports29–32 are newer live research.
`outputs/ota33/provenance.json` records hashes and snapshot classifications;
original artifacts were located through the snapshot artifact catalog and
checked against its manifest. Stock image and original disassembly still match
SHA256 `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89`
and `4ed45fa85967e4b004181b4927ee0493fe0a92c0d6c3116e3ca276c0d103c553`.
Historical captures/results and firmware were preserved.

## K00718
Source `D0222`, original line19, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

## Established offline failure mechanism

## K00719
Source `D0222`, original line21, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

Native FW189 RX callback0x5a08 builds a response **before** dispatching the
received command at0x5b40. At0x5a50–0x5a60 it sets the reply budget to
`min(54,65-request_length)`. The33-byte OTA data packet therefore leaves32 bytes.
Native0x9458 serializes IMU first;0x95c4 then traverses queued records and stops
when the next record cannot fit. A reliable command22 with23-byte body takes26
record bytes. The data ACK with5-byte body takes8: **26+8 exceeds32**.
Repeated long requests with S1=2 leave the first record queued and hide the ACK.

## K00720
Source `D0222`, original line29, SHA256 `8b01f6d40fe949ea8b1bd4bdf7245a1d50da855bb79549045c798f5c1efaf5ff`. Status: historical/unadjudicated narrative; English source prose.

`pc/tests/fw189_rx_emulator33.py` now runs the actual0x5a08 callback, actual
0x15d4 scheduler admission/copy and0x168c deferred execution, plus the existing
native dispatcher, dedup, reliable queues and OTA worker. Initialized .data is
copied from the stock startup source range, including last-block sentinelffff.
The deferred queue permits75-byte entries; a29-byte OTA command body does not
exceed that limit. Earlier queue models bypassed the radio callback and mocked
the scheduler; their success did not cover this response-budget seam.
