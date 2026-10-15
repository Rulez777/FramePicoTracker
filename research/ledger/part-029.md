# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K02241
Source `D0349`, original line24, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

## Implementation implications and unresolved issues

## K02242
Source `D0349`, original line26, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

A candidate needs separate controller-OFF long-response bootstrap and controller-ON short telemetry. Existing loader requires at least500us steady slot; this alone does not forbid an earlier RX cutoff. For example a600us reservation with270us end margin gives330us RX, within the runtime numeric bounds. However the exact Phase66 admission policy permits only600us slots/RX500 and requires700us quiet. A330us RX candidate therefore needs a separate exact-binary admission policy and validated early return, not a generic guard bypass. Transition must preserve tracker protocol state and obtain a fresh controller phase before any short RF exchange.

## K02243
Source `D0349`, original line28, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

Mode-status refresh also returns a long reply; changing only initial handshake is insufficient. Its ongoing proof lease and failure behavior need explicit design. Tracker decimation/poll phase must remain compatible with mode1 timing. Short operation, long-reply handling, recovery and controller coexistence have not been measured. No>=60% joint reception or99% claim.

## K02244
Source `D0349`, original line30, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase89/attempt (samples, calls, controller metrics, OpenOCD, recovery and report). Reproduction: outputs/phase89/analyze.py; analysis.json includes all six segment histograms. Local inventory: evidence-sha256.json. Prior Phase85 and Phase88 artifacts preserved.

## K02245
Source `D0350`, original line1, SHA256 `5da9bd9efe48b0c13aa181a9aad45b1245024722416a57eb4e123993bfc58ff2`. Status: historical/unadjudicated narrative; English source prose.

# Phase89 Grok design review disposition

## K02246
Source `D0350`, original line3, SHA256 `5da9bd9efe48b0c13aa181a9aad45b1245024722416a57eb4e123993bfc58ff2`. Status: historical/unadjudicated narrative; English source prose.

Read-only Cursor bridge, model grok-4.7-medium, completed2026-10-09. Raw response preserved in outputs/phase89/grok-review.md. No helper hardware access or edits authorized. This is an external static opinion, not hardware evidence.

## K02247
Source `D0350`, original line5, SHA256 `5da9bd9efe48b0c13aa181a9aad45b1245024722416a57eb4e123993bfc58ff2`. Status: historical/unadjudicated narrative; English source prose.

Accepted findings:330us short RX cannot carry long handshake and mode replies; ordinary OFF bootstrap followed by full cleanup and a60s ON sample loses MCU peer state and expires mode/liveness proof. A finite short test must preserve protocol state and proof age, and stop before expiry. A fresh native phase reference is required. Command17 low-power selection is not measured proof of shorter replies.

## K02248
Source `D0350`, original line7, SHA256 `5da9bd9efe48b0c13aa181a9aad45b1245024722416a57eb4e123993bfc58ff2`. Status: historical/unadjudicated narrative; English source prose.

Rejected proposal details:

## K02249
Source `D0350`, original line9, SHA256 `5da9bd9efe48b0c13aa181a9aad45b1245024722416a57eb4e123993bfc58ff2`. Status: historical/unadjudicated narrative; English source prose.

1. The proposed430us slot contradicts both the retained500us minimum and the review's instruction to leave that floor untouched.600us reservation with270us margin can represent330us RX within numeric bounds; a separate exact-binary policy is still needed. This is a corrected design possibility, not an installed candidate.
2. END set when a late callback runs does not prove the frame ended before the deadline. With no hardware END capture, a frame may have completed after the deadline but before observation. Deadline-capture recovery must retain this uncertainty; it cannot be described as exclusively recovering pre-deadline frames.
3. The smallest staged proposal describes restoring RAM between stages but does not implement validated MCU/host protocol-state continuity. It therefore is not ready to run.
4. Moving to9000us requires Frame N2 (not the current dense N0), fresh proof/configuration and measured response timing at that decimation. Existing observations cannot simply be transferred.

## K02250
Source `D0350`, original line14, SHA256 `5da9bd9efe48b0c13aa181a9aad45b1245024722416a57eb4e123993bfc58ff2`. Status: historical/unadjudicated narrative; English source prose.

Remaining implementation work: explicitly staged state continuity; fresh phase sampling without erasing the live peer; bounded short mode-proof lease; handling of long reliable queue entries; exact deadline/ISR lateness and early-return behavior; offline callback tests, sealed deployment, then guarded hardware test. No new OTA, joint RF measurement or>=60% claim from this review.

## K02251
Source `D0351`, original line1, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

# Phase91: tracker reply-latency regimes limit the short-RX plan

## K02252
Source `D0351`, original line3, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09, Claude Opus 5.5. OFFLINE analysis of retained hardware records only;
no RF, SSH (Frame unreachable: port22 timeout at start), OTA or code deployment.

## K02253
Source `D0351`, original line6, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

## Finding 1: reply timing is bimodal, with a fixed +84.5us late regime

## K02254
Source `D0351`, original line8, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

Phase88b channel2 CRC-valid records. The record `number` (JSON `sequence`) is the DWT
cycle count at our TXEN, and `cycles` (`roy_cycles`) is the count at capture. For
every reply length, the observation splits into a tight early mode (spread about 1us)
and a late mode about 85us later (delta histogram: 0/-1/+1 -> 1035 records,
88/89/90 -> 358). The late mode is 26.4%.

## K02255
Source `D0351`, original line14, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

| DMA bytes | TXEN->capture early / late (median us) | Our TXEN at slot timer (early / late) |
|---|---|---|
| 3 | 191.8 / 276.2 | 23.2 / 27.8 |
| 12 | 219.8 / 304.2 | 23.2 / 27.8 |
| 24 | 274.6 / 359.9 | 23.6 / 28.1 |
| 30 | 302.7 / 386.3 | 23.2 / 27.8 |

## K02256
Source `D0351`, original line21, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

Our TXEN moves by only about 4.6us between regimes, so about 84.5us of the shift lies
between our TX and the captured reply. Frame MPSL timeslot signals run at the highest
priority, so a capture delay lasting several seconds on the Frame side is unlikely.
The **tracker reply is later**. This is an inference: there is no tracker-side timestamp.

## K02257
Source `D0351`, original line26, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

The regime persists. Consecutive replies share a state 99.2% of the time, and late
runs last 2.6–8.8s, interleaved with short flicker. It is not a simple 1s beat.
RSSI median is 29 (late) vs 31 (early).

## K02258
Source `D0351`, original line30, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

## Finding 2: the late fraction depends on cadence

## K02259
Source `D0351`, original line32, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

30-byte replies, fraction more than 50us above the run minimum:

## K02260
Source `D0351`, original line34, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

| Run | Profile | Late fraction |
|---|---|---|
| Service52 | stock cadence 4727us, N0 | 0.3% |
| Phase84c | Frame 2989us | 7.5% |
| Phase82 | Frame 3000us | 19.9% |
| Phase88b | Frame 3000us | 26.5% |
| Mode53 | common 8800us | 60% |

## K02261
Source `D0351`, original line42, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

The Frame mode1 native period is 98 RTC ticks = 2990.72us. Polling at exactly 3000us
(the BLE grid) is the mismatched case, and it shows the most late replies among the
dense runs. This is a correlation, not a mechanism.

## K02262
Source `D0351`, original line46, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

## Rejected mechanism

## K02263
Source `D0351`, original line48, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

In the reply path 5a08 -> 65f0 -> 641c the RX delay is rescheduled before the TX, and
f02 selects the TIMER2 or RTC branch (RTC uses a soft-double divide by 30.5).
Unicorn emulation of the RTC branch: 144 instructions, a few microseconds, not 85us.
**Rejected** as the cause on cost grounds. The TIMER2 branch asserted in the bounded
emulator (app timer not initialized), so it was not measured. Script:
`outputs/phase90/phase91_cost641c.py`, run against stock image 477000fc…a690.

## K02264
Source `D0351`, original line55, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

## Consequence for the Phase90 short-RX plan (98–100% target)

## K02265
Source `D0351`, original line57, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

The plan's RX budget is about 330–339us within a 600us common quiet window.
Slot-relative observations for 30B IMU are 326 (early) and 416 (late), and for
24B 299 / 389.

## K02266
Source `D0351`, original line61, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

- Every late-regime IMU reply is outside a 330us RX, so at Frame 3000us cadence the
  ceiling is about 74–80% before any RF loss.
- Even the early 30B mode leaves only about 4us margin at 330us.

## K02267
Source `D0351`, original line65, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

Therefore 90d plus a coordinator, as specified, **cannot reach 98–100%** unless the
late regime is removed or replies become shorter. Building the coordinator first
would test a design whose arithmetic ceiling is already below the target.

## K02268
Source `D0351`, original line69, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

## Next grounded options (decisions for the user)

## K02269
Source `D0351`, original line71, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

1. Identify the tracker-side cause of the +84.5us regime from FW189 disassembly
   (reply construction 970c/95c4/52c4, preemption sources, f02/f04 state machine).
   Offline only.
2. A controllers-OFF diagnostic at known-good Phase88b settings, varying only the
   period (e.g. 2990/2991 vs 3000), to test the cadence-mismatch correlation.
   The BLE 3000 grid still constrains the final coexistence profile.
3. Shorter useful replies, e.g. an IMU mode with an 18-byte command2 body, to gain
   RX margin. Command17 mode semantics must be verified first.
4. A tracker-firmware timing change (needs OTA, not authorized). Listed only.

## K02270
Source `D0351`, original line81, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

## Update after Phase92

## K02271
Source `D0351`, original line83, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

The controlled A/B in Phase92 (original reference: `PHASE92_CADENCE_AB_LATE_REGIME_2026-10-09.md`) found about 25-30% late replies at both 3000 and 2989us with the same module. Finding 2's cadence correlation was confounded by module differences. Option 2 is done and negative.

## K02272
Source `D0351`, original line85, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

## CORRECTION (E4 in WORKLOG_CLAUDE_2026-10-09)

## K02273
Source `D0351`, original line87, SHA256 `4f86f30f62ec1ed1312db8b1586c88e981ae8963c2a936fc5ac8843f736d959d`. Status: historical/unadjudicated narrative; English source prose.

The +84.5us "late regime" is our own poll carrying the host mode-query record (22B vs 1B empty, +21B x 4us/B at 2Mbit/s). It is NOT tracker-side latency. The 74-80% ceiling statement does not apply to empty-poll operation.

## K02274
Source `D0352`, original line1, SHA256 `f6f8d9171c4441793739815e819bb5bd6a7f8fa145467c8cc1333a71c5e480d9`. Status: historical/unadjudicated narrative; English source prose.

# Phase92: 3000/2989us A/B does not change the late reply regime

## K02275
Source `D0352`, original line3, SHA256 `f6f8d9171c4441793739815e819bb5bd6a7f8fa145467c8cc1333a71c5e480d9`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09, Claude Opus 5.5. Hardware run; the user turned both controllers OFF.
Tracker [TRACKER_SERIAL] / [TRACKER_ID] awake. No OTA, no Frame flash, no code change.
Exact Phase86b and Phase88b releases, modules and configs were copied into
isolated `~/pico-ram-work/phase92/<stage>`. Only the poll interval and evidence
paths differ. All five stages exited 0, and session/RAM/MPU/trace cleanup was
verified true for each. Evidence: `outputs/phase92/<stage>/`.

## K02276
Source `D0352`, original line10, SHA256 `f6f8d9171c4441793739815e819bb5bd6a7f8fa145467c8cc1333a71c5e480d9`. Status: historical/unadjudicated narrative; English source prose.

## Mode

## K02277
Source `D0352`, original line12, SHA256 `f6f8d9171c4441793739815e819bb5bd6a7f8fa145467c8cc1333a71c5e480d9`. Status: historical/unadjudicated narrative; English source prose.

Stage a (Phase86b common 8800us bootstrap, desired frame, schema2): the fresh
fe02 sequence reported actual mode1/rev7/N0, then mode1/rev7/N2, native pending 0.
**The tracker was already in mode1. No setter (fd02) was needed or sent.**
Each stage b–e ended with a fresh `radio_mode=frame` and `verified=true`.

## K02278
Source `D0352`, original line17, SHA256 `f6f8d9171c4441793739815e819bb5bd6a7f8fa145467c8cc1333a71c5e480d9`. Status: historical/unadjudicated narrative; English source prose.

## Results (controllers OFF, Phase88b module ELF 8965f716…, RX800, 30s each)

## K02279
Source `D0352`, original line19, SHA256 `f6f8d9171c4441793739815e819bb5bd6a7f8fa145467c8cc1333a71c5e480d9`. Status: historical/unadjudicated narrative; English source prose.

| Stage | Interval | TX_END | CRC RX | RX/TX | IMU | Ring drops | Late fraction 3/12/24/30B |
|---|---|---|---|---|---|---|---|
| b | 3000 | 1489 | 1363 | 91.54% | 887 | 0 | .275/.299/.286/.297 |
| c | 2989 | 1574 | 1522 | 96.70% | 957 | 11 | .255/.254/.284/.269 |
| d | 3000 | 1528 | 1462 | 95.68% | 944 | 5 | .252/.273/.273/.289 |
| e | 2989 | 1612 | 1363 | 84.55% | 860 | 0 | .261/.230/.269/.263 |
| a (8800, N2) | 8800 | n/a | 301 | n/a | 319 | n/a | 24B .991, 30B .990 |

## K02280
Source `D0352`, original line27, SHA256 `f6f8d9171c4441793739815e819bb5bd6a7f8fa145467c8cc1333a71c5e480d9`. Status: historical/unadjudicated narrative; English source prose.

"Late" means the TXEN->capture DWT interval is more than 50us above the minimum for
that length. The late mode is again about +85us.

## K02281
Source `D0352`, original line30, SHA256 `f6f8d9171c4441793739815e819bb5bd6a7f8fa145467c8cc1333a71c5e480d9`. Status: historical/unadjudicated narrative; English source prose.

## Conclusions

## K02282
Source `D0352`, original line32, SHA256 `f6f8d9171c4441793739815e819bb5bd6a7f8fa145467c8cc1333a71c5e480d9`. Status: historical/unadjudicated narrative; English source prose.

- **Hypothesis rejected:** with the same module, 2989us does not reduce the late
  regime compared with 3000us (about 25–30% in all four runs). The earlier 84c 7.5%
  came from a different module/publication path, so the cadence correlation in
  Phase91 was confounded.
- RX/TX varies from 84.6% to 96.7% between runs with the same settings. Cadence did
  not explain it.
- At the 8800us N2 bootstrap almost every reply is late (99%). At dense N0, about 27%.
  Service52 (Service38 module, 4727us) had 0.3%. The regime depends on the
  module/tracker state relationship, possibly the poll position inside the tracker
  RX window or branch (N, normal/alternate). **Mechanism still unknown.**
- The Phase91 consequence stands: a 330us RX cannot capture late-regime IMU replies,
  so the Phase90 short plan is capped well below 98%.

## K02283
Source `D0352`, original line45, SHA256 `f6f8d9171c4441793739815e819bb5bd6a7f8fa145467c8cc1333a71c5e480d9`. Status: historical/unadjudicated narrative; English source prose.

## CORRECTION

## K02284
Source `D0352`, original line47, SHA256 `f6f8d9171c4441793739815e819bb5bd6a7f8fa145467c8cc1333a71c5e480d9`. Status: historical/unadjudicated narrative; English source prose.

See WORKLOG_CLAUDE_2026-10-09 E3/E4: regime boundaries are 10s host mode refreshes; late = longer host poll (pending 7e02 query).

## K02285
Source `D0353`, original line1, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

# Phase99: staged single-session coexistence (specification)

## K02286
Source `D0353`, original line3, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09, Claude Opus 5.5. Implementation delegated to Codex (gpt-6.1-sol) in
the isolated copy `outputs/phase99/release`. Nothing in `pc/` changes. No OTA, no
Frame flash.

## K02287
Source `D0353`, original line7, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

## Why (evidence: WORKLOG_CLAUDE_2026-10-09 E13–E18)
- The tracker drops the link after 0.6–1.0 s without received polls (E13), so the
  whole flow must be ONE MPSL session with no gap.
- Without cmd19 the tracker never transmits on its own and answers 1 B polls with
  3 B replies; the link holds at sparse N13 cadence (E16). A late cmd19 starts IMU
  on a running link (E17).
- With controllers ON the measured common BLE quiet is about 600 us (Phase89/E2);
  bootstrap replies (370–420 us) do not fit, so bootstrap needs controllers OFF.
- The module (phase78 ELF 8c314daf…, non-service, END-before-deadline check)
  re-reads these DATA words every slot: +36 slot_us (first request only), +40
  interval_us, +1876 slot length, +1884 phase target (bit31|RTC1 tick), +1888 slot
  cycle, +1892 RX end (us from slot start). S1 comes from the host mailbox meta.

## K02288
Source `D0353`, original line20, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

## Stages (host state machine, one call per loader loop iteration)

## K02289
Source `D0353`, original line22, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

S0 BOOTSTRAP. Controllers must be OFF (guard forbid mode). Profile:
interval 5982, cycle 7, slot 900, RX end 700, no phase lock, frame N13 via cmd16,
schema2 mode query, cmd19 omitted. Exit when engine.state is WAIT_SYNC,
mode_verified() is true, and that has held for 2.0 s. Timeout 25 s.

## K02290
Source `D0353`, original line27, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

S1 KEEPALIVE. Write live: +1876=400, +1892=340. Freeze the mode proof:
`continuity_lease` (opt-in, documented) means mode stays verified while MCU stage
stays 3, peer engine state is WAIT_SYNC/STREAMING and replies keep arriving
(engine liveness 3 s). The 10 s refresh query is suppressed, because its 22 B poll
and 32 B reply cannot fit RX 340. Any rediscovery or reset ends the run.
Blind ACK: publish S1=3 on every poll (E8). Guard in AWAIT mode: HMD must stay
valid; controllers may connect; no freeze limit yet. Exit when both controllers
are connected+valid+result200 continuously for 3.0 s. Timeout 90 s
(user switches controllers on).

## K02291
Source `D0353`, original line37, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

S2 PHASE. Guard STRICT (min 2 controllers, 150 ms freeze). For 10 s, take
ble_phase.sample() in bursts (at most about 30 ms of sampling per loop iteration)
while the loop keeps heartbeat, ring drain and runtime.poll running. Exclude samples
whose base0 equals the tracker working BASE0 (our own slots). Then
quiet_window(max_busy_fraction=0.05). Require width_us >= 100 + 340 + 100 = 540.
Build PhaseLock(reference, center, slot 400 us, width, lead 100 us) and write
+1884 = 0x80000000|target_tick. Timeout: sampling must finish within 15 s.

## K02292
Source `D0353`, original line45, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

S3 DENSE. Queue cmd16 [0x00,0x02] (N0) to the engine. When its ACK arrives, write
+40=3000 and +1888=1 (one peer, every slot). Peers 1/2 stay paused. Exit after
3.0 s of continuous replies (the 1 s rolling reply rate never at 0).

## K02293
Source `D0353`, original line49, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

S4 STREAM. Queue cmd19 [0x00] once. Run until duration end. Report IMU samples,
replies, tx_end and controller metrics separately per stage.

## K02294
Source `D0353`, original line52, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

Abort at any stage: guard exception, MCU stage drop or rediscovery, engine
RECONNECTING, stage timeout. Then the normal loader cleanup (session close, RAM/MPU
restore) runs unchanged.

## K02295
Source `D0353`, original line56, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

## Required report fields
`staged['transitions']`: [stage, t_s, slots, reason]; per-stage counters (polls,
replies, IMU, ring drops, guard metrics); `staged['phase']`: reference, center_us,
width_us, samples total and excluded own-slot, profile; final stage reached.

## K02296
Source `D0353`, original line61, SHA256 `42cd81c3b607a72d732db6bb48f094c2a490688bd9daea4e3c836f85448d0ac3`. Status: historical/unadjudicated narrative; English source prose.

## Acceptance for the first hardware run
Pass = S4 reached, IMU flowing, guard never tripped, cleanup verified.
Any abort is recorded as a result, not retried blindly.

## K02297
Source `D0354`, original line1, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

# Resume handoff:98–100% tracker reception with two working Frame controllers

## K02298
Source `D0354`, original line3, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Authoritative live-project handoff,2026-10-09. Read this after root AGENTS.md and before starting any hardware operation. This supersedes preparation-only summaries, not original evidence. Latest user instruction: prepare artifacts for a new chat because this conversation is repeatedly compacting. Hardware work was stopped at artifact preparation.

## K02299
Source `D0354`, original line5, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## Objective and acceptance

## K02300
Source `D0354`, original line7, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

The user now explicitly requires **98–100% reception from the PICO tracker while both Frame controllers work simultaneously**. The previous>=60% milestone is insufficient as final success. No such joint result exists. Controllers-OFF96.56% is a diagnostic result, not success. Need actual completed TX denominator, CRC-valid RX, retained unique replies, IMU sample completeness/gaps, mode proof/age, and controller validity/freeze metrics over the same interval. Do not equate RF packet percentage with lossless firmware transfer or real6DoF pose. A finite8s experiment is only an intermediate diagnostic, not sustained98–100% operation.

## K02301
Source `D0354`, original line9, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Do not silently change the metric to nominal scheduler slots, empty replies alone or a controller-OFF result. Define meaningful useful-data delivery and sustained duration before claiming success. RF retry/ACK integrity is distinct from IMU continuity.

## K02302
Source `D0354`, original line11, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## First reading and provenance

## K02303
Source `D0354`, original line13, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

1. K:/AI/pico/AGENTS.md (live knowledge base).
2. K:/AI/PICO-Frame-CURRENT-2026-10-08/AI_READ_FIRST.md.
3. Same snapshot CURRENT.md,SUPERSEDED.md,RESEARCH_MAP.md,DEVELOPMENT_HISTORY.md.
4. Snapshot provenance/artifact-catalog.csv locates original evidence; provenance/manifest.csv verifies snapshot hashes. These were consulted earlier; the snapshot is older than the live Phase78–90 work.
5. Original task-specific reports and their captures/disassembly/source, particularly the list below. Do not repeat a failed experiment without its original history.

## K02304
Source `D0354`, original line19, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

New local resume inventory is separate from the snapshot manifest; it does not certify historical provenance or hardware validity. New durable records must be English. Preserve originals and rejected hypotheses. Keep durable links in AGENTS.md and docs/EXECUTION_STATUS_2026-10-06.md, not Codex memory.

## K02305
Source `D0354`, original line21, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## Hardware and authorization at handoff

## K02306
Source `D0354`, original line23, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

- Frame SSH:steamos@[DEVICE_IP]; work directory:/home/steamos/pico-ram-work; Python:venv/bin/python.
- Main release:/home/steamos/pico-ram-work/release points to release-20261006b/pc. Experiments use isolated release directories; do not overwrite the normal release.
- Target:[TRACKER_SERIAL]; chip LE bytes[TRACKER_ID], integer[TRACKER_ID]. Other trackers are not authorized targets.
- The user enabled both controllers for Phase89. Last actual measured readiness was both healthy. They may now be asleep or off; don't assume present readiness from this handoff. Trackers may also sleep. Fresh guards are required before RF.
- Latest process check during handoff:SSH pgrep found no roy_ram_loader --run or OpenOCD. This was a process check, not fresh RF mode/identity proof. No new hardware RF session was started in Phase90; remote SSH only built/copied offline ELF artifacts.
- Later attempt to copy remote Phase86b timed out on SSH port22; it was NOT copied by that attempt. Frame connectivity is therefore unknown at final packaging. Don't infer current power or radio state from the earlier successful process check. Failure preserved in the resume package.
- Phase89 full RAM/MPU/hook restoration and post-cleanup controller validity verified. Phase88b session/RAM/MPU/trace cleanup all verified. No new tracker OTA, Frame flash or persistent radio changes.
- RAM experiments and remote mode requests were previously authorized. Do not perform new OTA from this handoff. No SWD access to the tracker, opening tracker, dongle or Frame firmware flash.
- Keep min2 controllers and150ms freeze guard for joint RF; don't attribute past failures to insufficient user movement without evidence. Pose validity remains false until optical6DoF is proven.
- Native tracking binary:release/tracking_watch <count>,100Hz. Never pass --help:atoi0 can run indefinitely.

## K02307
Source `D0354`, original line34, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## Tracker firmware and mode

## K02308
Source `D0354`, original line36, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Installed tracker candidate07:outputs/remote44/candidate07-initial-connection/fw189-dual-mode.bin,97832 bytes,SHA25640941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5. Prior OTA46 completed4077 blocks. No newer tracker image installed in this task.

## K02309
Source `D0354`, original line38, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Mode0 uses stock base4727.5us. Mode1 selects a3000us base; channelsESB2/18/32/42/46 are unchanged. Native scheduler conversions include truncation by30.5 whereas hardware RTC is32768Hz. Nominal3000us converts to98 ticks,2990.72265625us. Mode1 alternate receive window is approximatelyN*3000+2207 to(N+1)*3000us; active timer/RTC branch and real phase adaptation require measurement. Selecting a different frequency does not solve shared-radio airtime.

## K02310
Source `D0354`, original line40, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Schema2 request7e02+16-byte nonce; fe02 body26 binds nonce,chip,mode,revision7,actualN and native pending. Setter7d02 ACK fd02 is staging only, never applied-mode proof. Mode proof refresh10s, lease30s, query timeout8s; normal RF liveness3s. A1 known fork limitation: hidden remote pending switch can survive disconnect and is not reported by the native-pending byte. Schema2 native_pending0 does not prove absence of that hidden transition.

## K02311
Source `D0354`, original line42, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

The user explains mode0 observations by power-cycling the tracker. Fresh mode0 really was observed in84b and initial88;86/86b remotely restoredFrame. Don't assume mode1 persists through reboot or between sessions. Last hardware Frame proof was88b; current mode unknown without a new reply. FW version189 alone does not prove the fork.

## K02312
Source `D0354`, original line44, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## Measured facts and rejected directions

## K02313
Source `D0354`, original line46, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

| Run | Conditions and result | Meaning |
|---|---|---|
|Service52|Controllers OFF; original Service38 module,4727us,N0/M2,cycle1,slot900/RX800.2160CRC/2189TX=98.6752%;2156retained=98.4925%;2027IMU;3ring drops|Near99% OFF benchmark; no mode query, don't label it provenmode0|
|Phase78 controls|OFF, RX400:no product info; RX700:product info3x,0IMU,no complete connection. Scheduler~7slots/s while stock hunts controllers|400us handshake cutoff fails even without controllers; OFF does not imply idle radio|
|Phase79|OFF,FrameN13,42000us/cycle7,RX700:103/878 nominal11.73%,3IMU|Fresh Frame proof but poor cadence, rejected|
|Phase80|OFF,FrameN13,41874us:119/895 nominal13.30%,6IMU|Interior window formula alone not sufficient|
|Phase81|OFF,denseFrameN0,3000us,service79 module:1689/8125 nominal20.7877%,975IMU|Module behavior matters|
|Phase82|Same dense setup, exact Service38:4939CRC/5236TX=94.3277%;4445retained=84.8930%;2710IMU;493ring drops|Both RF and host losses; not99%|
|Phase83|Mailbox batch/coalesce:590CRC/4538TX=13.00%;575retained;349IMU;13drops;stopped replying halfway,proof expired|Cause unknown; batching disabled by default, do not repeat unchanged|
|Phase84b|2989us but freshmode0|Guard stopped; not a validFrame cadence test|
|Phase86|Common8800us bootstrap,StockN1->FrameN2 proved;482IMU,2query failures,253 retries|Mode restoration works; not a high-delivery profile|
|Phase84c|OFF,FrameN0,2989us:4686CRC/5245TX=89.3422%;4436retained=84.5758%;2624IMU;249drops|Worse than3000 control|
|Phase87|OFF,2600us:894CRC/5973TX=14.9674%;893retained;294IMU;0drops;proof expired|Rejected preferred cadence; target still recently replied, do not simply blame sleep|
|Phase85|Controllers ON,native observer only10s:2176raw/2170zero-bracket,34calls,both0invalid/0freeze|Fine stable quiet width520–550us, incomplete coverage; coarse1100us not robust|
|Initial88|OFF,fine observer but freshmode0|Safe reject preserved|
|Phase86b|Fresh common-grid mode restore before88b,30s;460/2512nominal18.31%,490IMU,2query failures,250 retries|Final cachedFrame proof may be expired;88b obtained its own proof|
|Phase88b|OFF,FrameN0/M2,3000us,cycle1,slot900/RX800,fine10us nominal polling.1546CRC/1601TX=96.5646%;1533retained=95.7527%;962IMU;11drops;1query failure;129retries|Best current measuredFrame diagnostic, not99% or joint result|
|Phase89|Both controllers ON,native-only60s,13184raw/13144zero-bracket,206calls,both0invalid/0freeze|Six-segment common600us quiet at3000/6000/9000 folds; no custom tracker RF|

## K02314
Source `D0354`, original line65, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Nominal denominators above are explicitly labeled; do not compare them as actual TX completion ratios. Final connected=false when running=false is loader shutdown semantics, not proof of an RF disconnection. Phase88b2504 scheduler slots !=1601 completed TX.

## K02315
Source `D0354`, original line67, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Earlier joint attempts Timing57–61,Coexist62/64/65,Phase67–78 failed guards/admission or received no useful IMU. Phase62 had a controller invalid even without custom RF. Several identical attempts incorrectly assumed user motion was the cause. See originals, don't weaken guards. Phase73 asynchronous SWD timestamps were inaccurate;74 first entry omitted disable_mpu and produced no valid sample;75 extra MPU rewrite was removed;77 native observer hardened completion-before-unhook.

## K02316
Source `D0354`, original line69, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## Tracker response observations

## K02317
Source `D0354`, original line71, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Phase88b channel2 retained CRC records, full DMA length:

## K02318
Source `D0354`, original line73, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

| Bytes | Count | Median callback us | Maximum us |
|---|---:|---:|---:|
|3|229|215|305|
|12|324|243|334|
|24|336|299|393|
|30|604|326|418|
|38|8|359.5|445|
|41|3|369|459|
|50|13|407|412|
|56|6|437|440|

## K02319
Source `D0354`, original line84, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

These are callback observation upper bounds, NOT hardware END timestamps. Nominal10us interrupt polling still had~90us observation spread. END set at a late deadline callback does NOT prove pre-deadline physical completion. Long mode/identity replies do not demonstrate a fit in a330us deadline.12-byte replies are not automatically full IMU: host only decodes command2 bodies18/24/36/42 as full samples.

## K02320
Source `D0354`, original line86, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Phase89 six chronological segments,50us bins,each bin>=3 observations and<=5%busy in every segment. Common quiet widths:600us at3000/6000/9000,550 at12000,500 at18000,450 at24000. Subtract two RTC ticks61.035us and100us lead plus100us post:338.965us budget for the600us case. Reference tick9524117,raw interval800..1400us relative to that reference. This historical anchor must NOT be replayed as live. Sampling<=5%busy and incomplete coverage are not a collision-free reservation guarantee.
