# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K02801
Source `D0378`, original line3, SHA256 `c37843d5a5e1d1a8e0b4cc524f2a2916a0dd34888d97a85b7b27d9530864e576`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. User explicitly requests running the test despite prior guard
aborts. Read AGENTS/timing58 history. Fresh directory, same aligned5982us*7
grid41874us/stockN8/frameN13, original coexistence RAM, full07 host, BLE lock,
minimum2 controllers and150ms guard. No firmware/OTA or guard changes.

## K02802
Source `D0378`, original line8, SHA256 `c37843d5a5e1d1a8e0b4cc524f2a2916a0dd34888d97a85b7b27d9530864e576`. Status: historical/unadjudicated narrative; English source prose.

Controller1 pose freeze162.57ms triggered abort (controller2 freeze0).
Zero scheduled PICO polls/replies/IMU, no mode proof. Thus another admission/
startup failure, not a measured tracker RF reception percentage. Three aligned
attempts have now stopped at the controller guard before target polling.
Do not blame user motion or infer cause without startup/controller baseline.

## K02803
Source `D0378`, original line14, SHA256 `c37843d5a5e1d1a8e0b4cc524f2a2916a0dd34888d97a85b7b27d9530864e576`. Status: historical/unadjudicated narrative; English source prose.

Exit1; all session/SRAM/MPU/trace cleanup true. RAM unloaded, service inactive,
no loader/OpenOCD remains. Evidence outputs/timing59/attempt including logs,
metrics, invocation/config, recovery/live/report and cleanup. Prior evidence
preserved. Investigate startup/BLE and native tracking before another unchanged
repeat; do not silently relax guard or claim timing validation complete.

## K02804
Source `D0379`, original line1, SHA256 `3f41afd6b6f692addab3dce9a4731d05fca2bb9c3999738da4afbae668e32071`. Status: historical/unadjudicated narrative; English source prose.

# Timing60: requested repeat, controller guard abort

## K02805
Source `D0379`, original line3, SHA256 `3f41afd6b6f692addab3dce9a4731d05fca2bb9c3999738da4afbae668e32071`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. User explicitly requests another test after timing59. History read
in AGENTS; fresh output paths, same5982us*7 common grid, stockN8/frameN13,
original coexistence RAM and150ms controller guard. No firmware or OTA changes.

## K02806
Source `D0379`, original line7, SHA256 `3f41afd6b6f692addab3dce9a4731d05fca2bb9c3999738da4afbae668e32071`. Status: historical/unadjudicated narrative; English source prose.

Controller2 freeze161.66ms triggered guard; controller1 freeze0 at rejection.
Zero scheduled target polls, zero replies/IMU, no identity or mode proof.
This fourth aligned-grid abort did not measure PICO RF performance. Different
controller triggered this time; no evidence that user motion or PICO itself
caused the freeze. Do not silently relax guard or infer good/bad packet rate.

## K02807
Source `D0379`, original line13, SHA256 `3f41afd6b6f692addab3dce9a4731d05fca2bb9c3999738da4afbae668e32071`. Status: historical/unadjudicated narrative; English source prose.

Exit1, all session/SRAM/MPU/trace cleanup true. Module unloaded, radio unit
inactive and no loader/OpenOCD process remains. Evidence outputs/timing60/attempt
includes frozen config/invocation, controller metrics, stdout, recovery/report,
live state and cleanup. Earlier records preserved. Startup/BLE/native baseline
diagnosis is needed before an unchanged repeat can add useful RF evidence.

## K02808
Source `D0380`, original line1, SHA256 `94a60c528cbbb868cc2555c7491eefcdbf48b2a99c985c21e3ba19e23a3b3a90`. Status: historical/unadjudicated narrative; English source prose.

# Timing61: requested repeat rejected by controller guard

## K02809
Source `D0380`, original line3, SHA256 `94a60c528cbbb868cc2555c7491eefcdbf48b2a99c985c21e3ba19e23a3b3a90`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. User explicitly requested another run; timing60 history read and
preserved. New timing61 job, same5982us*7 grid,stockN8/frameN13,coexistence RAM,
BLE lock,full07 host and150ms guard. No OTA or firmware change.

## K02810
Source `D0380`, original line7, SHA256 `94a60c528cbbb868cc2555c7491eefcdbf48b2a99c985c21e3ba19e23a3b3a90`. Status: historical/unadjudicated narrative; English source prose.

Controller2 pose froze161.61ms,controller1 freeze0. Zero PICO polls/replies/IMU:
no RF percentage or mode proof. Cause not established; do not blame user motion.
All session/SRAM/MPU/trace cleanup true; module unloaded,service inactive,no
loader/OpenOCD. Evidence outputs/timing61/attempt. Further unchanged repeats
cannot replace investigation of native baseline and startup timing.

## K02811
Source `D0382`, original line1, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

# Claude work log, 2026-10-09 (English, machine-readable numbers)

## K02812
Source `D0382`, original line3, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

Purpose: running diary for humans and other models. Each entry has a time,
action, exact numbers, artifacts and status (OBSERVED / OFFLINE / HYPOTHESIS /
REJECTED / CONFIRMED). Times are approximate local CEST.

## K02813
Source `D0382`, original line7, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

Conditions for this session block: controllers OFF (user away about 1h), HMD on,
one tracker [TRACKER_SERIAL] (chip LE [TRACKER_ID]) on charger and powered.
Frame SSH steamos@[DEVICE_IP] reachable again (uptime ~2h at 12:34).

## K02814
Source `D0382`, original line13, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E1 Phase91: bimodal reply timing (OFFLINE, retained Phase88b records)
- Records: `sequence` = DWT at our TXEN; `roy_cycles` = DWT at capture (64 MHz).
- Late mode adds +84.5 us at every reply length (3/12/24/30 B). Our TXEN is +4.6 us later.
- Phase88b late fraction 26.4% (1499 records).
- Initial interpretation "tracker-side latency": SUPERSEDED by E4.
- Rejected sub-hypothesis: 641c soft-double cost. Unicorn: RTC branch 144 instructions.
  Script: `outputs/phase90/phase91_cost641c.py`.

## K02815
Source `D0382`, original line21, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E2 Phase92 hardware A/B, cadence 3000 vs 2989 (OBSERVED, controllers OFF)
- Stage a, 8800 us restore: fe02 actual mode1/rev7/N0 -> N2. Tracker was already
  mode1, no setter sent.
- 30 s each, Phase88b module ELF 8965f716…, RX800:

## K02816
Source `D0382`, original line26, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

| stage | interval_us | tx_end | crc_rx | rx/tx | imu | ring_drops | late_frac_30B |
|---|---|---|---|---|---|---|---|
| b | 3000 | 1489 | 1363 | 0.9154 | 887 | 0 | 0.297 |
| c | 2989 | 1574 | 1522 | 0.9670 | 957 | 11 | 0.269 |
| d | 3000 | 1528 | 1462 | 0.9568 | 944 | 5 | 0.289 |
| e | 2989 | 1612 | 1363 | 0.8455 | 860 | 0 | 0.263 |

## K02817
Source `D0382`, original line33, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

- REJECTED: cadence 3000 vs 2989 changes the late regime.
- Cleanup all true, five stages. Evidence: `outputs/phase92/`.

## K02818
Source `D0382`, original line36, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E3 Regime boundaries are exactly 10 s apart (OBSERVED)
- c-2989 boundaries at 0.11, 10.11, 20.11 s; e-2989 at 2.63, 12.63, 22.64 s;
  d-3000 at 43.43, 53.47 s (DWT time).
- 10 s = peer_engine `mode_refresh_s`. The host mode-refresh query triggers the regime.

## K02819
Source `D0382`, original line41, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E4 CONFIRMED (arithmetic): "late regime" = our own longer poll
- Host mailbox payload: empty poll 1 B; mode query `pack([record(seq,1,7e02+nonce16)])`
  22 B (+21 B). Setter 28 B, command16 6 B.
- ESB 2 Mbit/s = 4 us/byte -> +21 B = +84 us airtime. Observed +84.5 us.
  The extra +4.6 us before TXEN is the DMA copy loop.
- The tracker reply latency is constant. **Phase91 "tracker-side regime" and its
  74–80% ceiling claim are WRONG for empty-poll operation (90d).**
- What is true: any poll that carries a command record lengthens the exchange.
  A mode query (22 B poll + 32 B fe02 reply) cannot fit a 330 us RX.

## K02820
Source `D0382`, original line51, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E5 Loss rate is not affected by the pending query (OBSERVED)
- Replies per scheduled slot, early vs late blocks:
  b 0.6669/0.6684; c 0.6498/0.6639; d 0.6486/0.6488; e 0.5876/0.6591.

## K02821
Source `D0382`, original line55, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E6 Why queries stay pending for seconds (OBSERVED + HYPOTHESIS)
- After each 10 s query, fe02 usually arrives within about 0.05 s. The same nonce
  is often captured 2–3 times: the host learns of the reply late (SWD ring
  read latency) and keeps republishing, so the tracker answers duplicates.
- Long late blocks (c-2989 20.11–27.43 s) contain NO fe02. The query is never
  answered and the host waits for the 8 s timeout (`radio_mode_query_failures`=1
  in each run).
- HYPOTHESIS T3: the native deferred-command admission drops the query although
  the RF exchange completed (known FW189 queue/ACK defect), and the tracker then
  ignores the identical sequence. Host fix candidate: re-issue the query with a
  NEW sequence and nonce after a short no-answer interval instead of waiting 8 s.

## K02822
Source `D0382`, original line67, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E7 Reply composition for empty polls (OBSERVED, Phase92 b–e, 5717 CRC replies)
- 30 B = cmd2 body 24 (2156); 24 B = cmd2 body 18 (1436); 12 B = cmd2 body 6 (1029,
  not counted as an IMU sample by the host); 3 B = empty (952).
- About 1.6% carry extra records and grow to 38–56 B: cmd22 body 23 (status, ~1 per
  second, seq 1..28 in 30 s), cmd15 `0a01`, cmd1 product info / fe02.
- Empty-poll end time at slot timer (early mode, TXEN ~23 us): 3 B 215, 12 B 243,
  24 B 298, 30 B 326 us. Against a 330 us RX all IMU-only replies fit (30 B margin
  about 4 us, observation quantization 10 us). Every reply with an extra record does not.

## K02823
Source `D0382`, original line76, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E8 Tracker repeats an un-ACKed record in consecutive replies (CONFIRMED)
- The same (sequence, command, body) appears in 2–5 consecutive replies, e.g. b-3000
  slots 671/673/675 cmd22 seq7; fe02 nonce 4021… x4 at slots 881–896.
- Repeats stop after the ACK takes effect with a 1–2 reply pipeline delay.
- **BLOCKER for the 90d/RX330 plan (new):** a cmd22 status reply (50–56 B) cannot be
  captured, so it is never ACKed. IF the tracker repeats un-ACKed records without a
  retry limit (UNVERIFIED; observed bursts ended only because ACKs arrived), every
  later reply stays long and reception stalls within about 1 s.
  REQUIREMENT: in short-RX mode, ACK unconditionally (S1 bit0 = 1 every poll).
  90d takes S1 from the host mailbox meta, so the host must publish S1 = 3 during
  the short policy. Consequence: lost status/notification records (about 1/s), and
  the IMU in the same reply is lost too (about 1% at 111 polls/s).

## K02824
Source `D0382`, original line89, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E9 T5 blind-ACK hardware run with the Phase88b module (INVALID as an ACK test)
- `phase93/t5-blindack`: isolated copy of the 88b release, peer_engine S1 forced to 3
  (`peer_engine.blindack.py` saved), 30 s at 3000 us, controllers OFF.
- tx_end 1747, crc 1381 (79.05%), IMU 884, ring drops 3, mode frame verified,
  0 query failures. Cleanup all true.
- INVALID: the 88b module overrides S1 (`s->dma[1]=2|D(1900)`, ACK only after a valid
  capture), so the host S1 had no effect. Repeats are still present (12 of 26).
- The 79.05% sits below the 84.6–96.7% range but is single-run, not attributable.
- Next: validate E8 on 90d (host S1 honored) or an 88b variant with
  `s->dma[1]=3`, offline then hardware.

## K02825
Source `D0382`, original line100, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E10 T4 earlier TXEN (OFFLINE estimate, deprioritized)
- Extra 21 B copy costs only +4.6 us before TXEN. The ~23 us slot-start→TXEN is
  mostly MPSL dispatch, so moving register writes after TXEN gains ≤ ~5 us.

## K02826
Source `D0382`, original line104, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E11 HMD panel
- Backlight `ae94000.dsi.0` brightness 49/459, writable by the video group.
  Setting 0 was blocked by the auto-mode classifier (remote system setting).
  Left for the user.

## K02827
Source `D0382`, original line109, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E12 Coordinator design check: tracker link-loss timeout (HYPOTHESIS, experiment prepared)
- The handoff plan inserts a user action (controllers OFF->ON) between the
  bootstrap session and the short-RX session. FW189 has a link-loss threshold:
  6e1e compares 200/(f21 N+1) with counter 20000ef0, and when exceeded 6e68 calls
  64ac (teardown). The time unit of ef0 is not established.
- Every past multi-second gap between loader runs needed re-discovery: Phase92
  stage b starts with product info at slot 135. If the tracker drops the link
  within about 1 s of silence, NO human action can sit between the stages, and the
  coordinator must swap CODE within the timeout or bootstrap with controllers ON.
- Experiment Phase94 (controllers OFF, exact 88b module/profile): opt-in
  `link_pause_schedule` [[5,0.3],[10,0.6],[15,1.0],[21,2.0]] sets peer0
  `reserved` bit0 (MCU skips the slot) and logs transitions. Readout: after each
  pause, does the tracker answer working-channel polls immediately or only after
  re-discovery (ch 1/37/77/4/78 offers)?
  MCU peer timeout D(1612)=3 s (192e6 DWT), so pauses ≤2.5 s keep MCU stage 3.
- Patch written by Codex (gpt-6.1-sol, workspace-write, one file):
  `outputs/phase94/link-pause/release/radio/ram/roy_ram_loader.py`
  SHA 8a91b660…ca9e. Additions only (35 lines), py_compile OK, reviewed by Claude.
  Not executed yet: Frame SSH timed out (port 22) during deployment.

## K02828
Source `D0382`, original line129, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E13 Phase94 RESULT: tracker drops the link after 0.6–1.0 s of silence (OBSERVED)
- Controllers OFF (guard verified: HMD valid, controllers disconnected), exact 88b
  module/profile, mode frame verified, 30 s. Cleanup all true.
  tx_end 1654, crc 1096 (66.26%, lowered by forced re-discoveries).
- Host pause events (t_s, slot counter): 5.020–5.308 (slots 483–505),
  10.026–10.609 (910–957), 15.010–16.003 (1329–1420), 21.011–23.006 (1834–1981).
- After 0.29 s and 0.58 s: working-channel (ch 2) replies at +1 slot. LINK KEPT.
- After 0.99 s: no ch2 replies; ch78 discovery at +308 slots, then product info.
  LINK LOST. After 2.0 s: ch78 discovery at +111, product info at +140. LINK LOST.
- Matches the FW189 threshold 200/(N+1) missed windows x (N+1) x ~3 ms = ~0.6 s.
  The ~0.6 s budget is roughly independent of N.
- CONSEQUENCE for the coordinator: no human action (controllers OFF->ON) and no
  multi-second phase acquisition can sit between bootstrap and short RX. Silence
  must stay <~0.5 s. The handoff plan step 4 is INFEASIBLE as written.
- DESIGN DIRECTION (HYPOTHESIS): keep the link alive across the transition with
  sparse empty keepalive polls. Example: frame N=40 -> RX window 122.2–123.0 ms;
  a 123 ms poll period gives ~8 polls/s, about 0.5% airtime with 600 us slots and
  ~5 polls of slack inside 0.6 s. Requires command16 N change before controllers
  come on, and keepalive slots placed in BLE gaps once controllers reconnect.
  Evidence: `outputs/phase94/link-pause/`.

## K02829
Source `D0382`, original line150, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E14 Phase95: T6 (RTC-truncated window explains Phase79/80) REJECTED (OBSERVED)
- Controllers OFF. Exact Phase80 release/module/config (frame N13 via cmd16, cycle 7,
  slot 900, schema2 query, no setter), 30 s each, only the interval changed.
  Cleanup all true in every run.

## K02830
Source `D0382`, original line155, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

| run | interval x7 | replies/polls | IMU | ch2 / ch78 / ch4 records |
|---|---|---|---|---|
| a | 5970 (41790 us) | 76/440 = 17.3% | 1 | 78 / 5 / 3 |
| b | 5982 (41874 us, Phase80 repeat) | 88/439 = 20.0% | 1 | 90 / 4 / 3 |
| c | 5970 | 76/438 = 17.4% | 4 | 80 / 4 / 4 |

## K02831
Source `D0382`, original line161, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

- Moving the poll 84 us earlier (inside the 98-tick model window end of ~41870 us)
  did NOT help. REJECTED.
- Reply pattern: bursts with every poll answered (gap 7 slots: a 59x, b 68x), then
  long silences (172–344 slots, about 1–2 s) and ch78/ch4 re-discovery. The link
  drops repeatedly.
- Reply content at N13: mostly EMPTY (a 52, b 58), plus fe02 (cmd1/26), product info
  (cmd1/35) and cmd22/6. IMU cmd2 appears only 1–2 times. At dense N0 nearly every
  reply carries IMU (E7).
- HYPOTHESIS T7: with N>0 the tracker does not put IMU into poll replies; IMU may go
  through its own autonomous TX (TIMER0/fe1 path), which our slot RX does not hear.
  Untested. If true, sparse keepalive (E13) is fine for holding the link but not for
  data, and data needs the N0 dense cadence.
- Re-evaluate E13 keepalive: at N13 / 41.8 ms the link still dropped every few
  seconds, so the sparse keepalive is NOT validated by this data.
  Evidence: `outputs/phase95/`.

## K02832
Source `D0382`, original line177, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E15 T7 mechanism from existing knowledge + T8 prepared (OFFLINE)
- docs/roy-ram-station-technical-en.md §5.3: after command 19 the tracker transmits
  IMU on its own when no poll arrives within its TIMER0 CC0 period. FW189 61f4/62e4:
  CC0 = 3000 us (frame, N=0) or 12000 us (frame, N!=0). At N13 / 41.8 ms polls the
  tracker drains IMU autonomously, so poll replies are empty (matches E14).
  Those autonomous TX previously froze controllers.
- fe1 (fd0+0x11, autonomous permission) is set ONLY by command 19 (8efc/8f00) and is
  CLEARED by native teardown 64ac (651e `strb r3,[r5,#0x11]`). Each new loader
  session re-discovers the tracker, so fe1 starts at 0.
- T8 HYPOTHESIS: omit command 19 -> no autonomous TX; IMU only in poll replies.
  Good for coexistence (no random tracker TX) and possibly lets sparse polling carry
  IMU. Risk: the tracker may require 19 to stream IMU at all.
- Patch by Codex (gpt-6.1-sol): opt-in `omit_command19` in
  `outputs/phase96/release` (station_v4_runtime `_configure_engine` drops cmd19 in
  both branches; loader sets runtime.omit_command19 + bool validation + report key).
  Reviewed by Claude, py_compile OK. Deployed to Frame `phase96/`:
  a-dense-no19 (88b profile, N0, 3000 us, 30 s) and b-n13-no19 (Phase80 profile,
  N13, 5982x7, 30 s). NOT RUN YET: needs controllers OFF.

## K02833
Source `D0382`, original line196, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E16 Phase96 RESULT: command 19 is the IMU-stream switch; without it the link is rock-solid (OBSERVED)
- Controllers OFF, tracker mode frame verified in both runs, 30 s each, cleanup
  all true, `omit_command19` = True. Configuration sent: cmd16, cmd17 [07], cmd25 [02]
  (no 19).
- a-dense-no19 (88b module, N0, 3000 us, RX800): tx_end 2091, crc 2045 = 97.80%,
  replies 2022/2091 = 96.70%, ring drops 22. ch2 reply gaps: 1 slot x1901,
  2 x116, max 10. Composition: empty 3 B x2014, fe02 x6, product info x2.
  **IMU 0.**
- b-n13-no19 (Phase80 module/profile, N13, 5982x7 = 41874 us, RX700):
  replies 420/434 = 96.77%, gaps 7 slots x416, max 14 -> NO link drops.
  Composition: empty x415, fe02 x4, product info x1. **IMU 0.**
- Versus Phase95 (same N13 profile WITH cmd19): 17–20% replies, repeated drops.
  So E14's sparse-cadence failure is caused by cmd19-enabled behaviour (autonomous
  TX / IMU streaming), NOT by window timing. T6 stays rejected; T7 effectively confirmed.
- CONFIRMED: cmd19 enables IMU streaming (no IMU at all without it, in replies or
  otherwise).
- DESIGN CONSEQUENCES:
  1. Keepalive across a transition is VALIDATED in the no-cmd19 state: 1 B empty
     poll + 3 B empty reply (ends about 215 us), sparse N13 cadence 96.8%, no drops,
     no tracker autonomous TX.
  2. Proposed flow: bootstrap + mode proof + cmd16/17/25 with controllers OFF
     (no cmd19) -> empty-poll keepalive while controllers come ON (seconds OK) ->
     BLE phase lock -> short-RX dense N0 -> send cmd19 last, to start IMU.
     cmd19 itself is a short poll (6 B).
  3. Open: (T9) does cmd19 sent late (after the link is established) start IMU at
     N0 normally? (T10) once cmd19 is on, autonomous TX fires 3000 us after the
     last received poll, so missed polls make the tracker TX on its own; check
     whether that lands in our quiet phase.
  Evidence: `outputs/phase96/`.

## K02834
Source `D0382`, original line226, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E17 Phase97 RESULT: T9 CONFIRMED, late cmd19 starts IMU on a running link (OBSERVED)
- Controllers OFF, 88b module/profile (dense N0, 3000 us, RX800), omit cmd19 at
  configuration, then host queues (19, 00) at t=12.03 s (slot 917). Loader patch:
  opt-in `delayed_command19_s` in `outputs/phase97/release` (written by Claude,
  py_compile OK). Cleanup all true.
- tx_end 1760, crc 1708 = 97.05%, replies 1675, IMU 572, ring drops 32.
- Before cmd19: 808 ch2 replies, 796 empty, fe02 x10, product info x2.
  After cmd19: 867 replies: cmd2/24 x347, cmd2/18 x211, cmd2/6 x158, empty x129,
  plus cmd22 extras.
- CONSEQUENCE: a staged flow is possible: link up and keep it alive with empty polls
  (no cmd19, no tracker autonomous TX), move to the coexistence profile, THEN start
  IMU with one 6 B cmd19 poll.

## K02835
Source `D0382`, original line239, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## Status of theories after this block
- T1 cadence -> late regime: REJECTED (E2). T2 tracker-side latency: REJECTED, it is
  our longer poll (E4). T3 query loss: open. T4 earlier TXEN: low value (E10).
- T5 un-ACKed record repetition: CONFIRMED (E8), blind-ACK fix untested on 90d.
- T6 RTC truncation explains sparse failure: REJECTED (E14).
- T7 autonomous IMU at N>0: CONFIRMED indirectly (E15/E16).
- T8 no-cmd19 keeps the link solid: CONFIRMED (E16). T9 late cmd19 starts IMU:
  CONFIRMED (E17).
- Link-loss timeout 0.6–1.0 s: CONFIRMED (E13).

## K02836
Source `D0382`, original line249, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E18 Phase98: first controllers-ON keepalive attempt, guard abort at session start (OBSERVED)
- Controllers ON (HMD + both valid before start). Profile = Phase96 b-n13-no19
  (Phase80 SERVICE-build module, N13, 5982x7, slot 900, no phase lock, omit cmd19),
  guard min 2 / 150 ms. Cleanup all true (session closed, RAM/MPU restored).
- Abort after only 43 granted slots (~0.26 s), 6 polls, still in discovery:
  "Controller pose frozen 152ms (limit 150ms): {1: 151.5, 2: 0.0}". One metric window,
  0 invalid. No tracker reply, no mode proof.
- Native baseline right after (no custom RF, 1500 samples / 15.2 s): both
  controllers 0 invalid, max unchanged pose 11.4 ms. Evidence:
  `outputs/phase98/baseline-native-after.jsonl`.
- The freeze coincides with session start. HYPOTHESIS T10: the SERVICE build
  re-requests the EARLIEST slot immediately after every block (designed for
  controllers OFF), which can starve BLE events at startup. Counter-evidence to
  weigh: the non-service phase-locked module (coexist56/64, phase67) ran 60 s with
  controllers ON and 0 logged freeze. Timing57–61 also aborted at start (162–172 ms)
  with another module, so the start transient is not fully explained.
- Next: rerun the no-cmd19 sparse keepalive with the NON-service phase-locked
  coexistence module/profile (phase66/78 family; slot 600 within the measured
  600 us gap admission), or relax nothing and test the start transient separately.

## K02837
Source `D0382`, original line269, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E19 Phase99 run1: first staged single-session run (OBSERVED)
- Build: module phase99 = phase78 minus timed-optical metadata plus D1896-gated
  self re-request (same MPSL call as service mode). 1980 B, ELF a9b882d9…7bad1,
  entry 0x2001151d. Host: staged_coexist.py (Codex, reviewed and revised by Claude;
  20 offline tests pass), peer_engine blind_ack/continuity_lease, guard set_mode.
  Evidence `outputs/phase99/run1/`. Cleanup all true.
- Timeline: S0 bootstrap 0–3.31 s (52 polls, 39 replies, mode verified, no cmd19).
  S1 keepalive 3.31–41.74 s: slot 400 / RX 340 / interval 41874 / cycle 1, 834 replies,
  NO link drop while the user switched controllers on (about 36 s). S2 at 41.74 s
  (both ready 3 s). Abort about 0.8 s into S2: "HMD/controller tracking lost for 0.51s",
  controller index2 result=100.
- Controller metrics (1 s windows): at 36.1 s ctrl1 changes 18 / invalid 81; 37.1 ctrl1 ok,
  ctrl2 invalid 85; 38.1 ctrl1 invalid 63; 39.1 ctrl1 invalid 47; 40–42 both 0 invalid;
  43.1 ctrl2 invalid 87. Freshly powered controllers were already flaky inside S1.
- MPSL: 5387 slots granted, 4411 blocked events (82%). Every block triggers an
  immediate EARLIEST re-request (~125 slots/s, 400 us, random phase, ~5% airtime) and
  every granted slot transmits a poll.
- Interpretation (HYPOTHESES): (a) the 3 s readiness threshold is too short for
  just-powered controllers; (b) the self re-request churn plus unlocked polls may harm
  BLE (T10). Not separated by this run.
- Next run2: S1 exit only after 10 s of continuous readiness; S2 sampling 6 s instead
  of 10 to reduce the unlocked period. Keep guard strict from S2.

## K02838
Source `D0382`, original line292, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E20 Phase99 run2: S0 abort on legitimate bootstrap rediscovery (REVIEW BUG, fixed)
- Settings: controllers_stable_s 10, phase_sample_s 8. Started 15:33:18 Frame time with
  controllers OFF. Abort about 2 s in, still in S0: "MCU stage drop or rediscovery".
  325 slots, 242 blocked, 47 polls, 1 reply. Cleanup all true. Controllers were
  switched on after the abort, so there was no RF exposure with controllers ON.
- Cause: the stage-drop abort was armed already in S0, but a failed first handshake
  followed by rediscovery is normal during bootstrap (run1 happened to connect on the
  first try). Missed in Claude's review.
- Fix: the drop/rediscovery abort applies only from S1 on. New test
  `test_bootstrap_tolerates_rediscovery`; 21 tests pass. Deployed as phase99/run3
  (otherwise identical to run2). Evidence `outputs/phase99/run2/`.

## K02839
Source `D0382`, original line304, SHA256 `9137680cb3cc149fc39627f9763a9869cd3f860758f147001ed90f746f6f9a37`. Status: historical/unadjudicated narrative; English source prose.

## E21 Phase99 run3: unlocked keepalive causes PERIODIC alternating controller dropouts (OBSERVED)
- Same as run2 plus the S0 fix. Started 15:39:34; S0->S1 at 4.90 s (75 polls, 37 replies).
  S1 lasted the full 90 s timeout: 1987 replies, link never dropped. Abort "S1 timeout"
  because the 10 s continuous-readiness condition never held. 6858 slots, 4651 blocked
  (68%). Cleanup all true. Evidence `outputs/phase99/run3/`.
- Controllers on at ~40–43 s (metrics clock). After that, 1 s windows show invalid bursts
  ALTERNATING between controllers with a ~10–11 s period each:
  ctrl1 invalid at 48–49 (37,18), 59 (68), 68–69 (15,39), 79–81 (20,49,45), 90–91 (58,39);
  ctrl2 at 52–55 (14,85,78,52), 63–65 (53,65,30), 73–74 (91,38), 84–85 (32,33), 95 (38).
  Between bursts both are 0 invalid / 99 changes.
- Interpretation (HYPOTHESIS, strong): a beat between our unlocked deterministic slot
  schedule (41874 us polls, plus EARLIEST re-requests after blocks) and each
  controller's BLE connection-event schedule. Not a host mode refresh (suppressed in
  S1). Not simple warm-up: strictly periodic, alternating, and recurring for 50 s.
- Revises run1: its S2 abort (ctrl2 result 100) fits the same periodic pattern.
- CONSEQUENCE: unlocked keepalive with controllers ON is harmful in a deterministic way.
  Phase lock must happen as soon as the controllers are up, and readiness cannot
  demand 10 s clean while we are unlocked. Options for run4:
  (A) S1->S2 when both controllers are valid for 2 s; guard AWAIT during the 8 s phase
      sampling; STRICT only from S3 phase lock + 3 s. Transitional harm is accepted
      and documented, the final profile is judged strictly.
  (B) Reduce the beat: S1 interval 42000 (multiple of 3000, fixed BLE phase) instead of
      41874. Risk: the tracker N13 window (E14/F5 untested at 42000 without cmd19).
  (C) Suppress EARLIEST self re-request in S1 (module flag D1896=0) and rely on regular
      slots only. Risk: blocked slots are lost; link timeout is 0.6 s.

## K02840
Source `D0383`, original line1, SHA256 `5f25f48ec382589be108114b301a5100f2215dc4c540f49dcc5e416358bf9910`. Status: historical/unadjudicated narrative; English source prose.

# Additional live identity evidence220

## K02841
Source `D0383`, original line3, SHA256 `5f25f48ec382589be108114b301a5100f2215dc4c540f49dcc5e416358bf9910`. Status: historical/unadjudicated narrative; English source prose.

During read-only recovery check after user reported indicator went dark, remote fast-identity219.json differed from the locally preserved219 zero-record run. A later remote run under the same filename contains5hardwareCRC records. It was not launched by this investigation; preserve attribution separately. Copied raw snapshot to outputs/external-fast-identity219-observed220.json and decoded outputs/external-identity220-decoded.json. No new TX220.

## K02842
Source `D0383`, original line5, SHA256 `5f25f48ec382589be108114b301a5100f2215dc4c540f49dcc5e416358bf9910`. Status: historical/unadjudicated narrative; English source prose.

One12-byte bootstrap payload04001f0000000a01ad1da201 and four13-byte identity payloads02000a0000ad1da201... carry identical FICR bytes[TRACKER_ID], LEword01A21DAD. Low1DAD matches historical third slot/SN [TRACKER_SERIAL]. Both independent firmware producer layouts agree. Identity-response path68F0 length9 emits6844 only with paired marker!=AAAA at that instant. This is evidence of a responding unpaired tracker, not proof the user's first tracker powered off because of the request, not standalone6DoF. Pairwise correspondence with a physical LED remains unverified. Channels1/37/77, observations358us, slots7/27/29/39 for identity. Cleanup reports alltrue/flashfalse.

## K02843
Source `D0383`, original line7, SHA256 `5f25f48ec382589be108114b301a5100f2215dc4c540f49dcc5e416358bf9910`. Status: historical/unadjudicated narrative; English source prose.

A second full tracker ID is now available; first remains[TRACKER_ID], second logical slot stillunknown. No power-off command sent by known218/219 probes; full command dispatch side effects not yet exhaustively audited. User concern requires withholding new transmissions until state clarified.

## K02844
Source `D0384`, original line1, SHA256 `914eb17dcdc41b2821f9b004de8afe3bbf7d93bfb231aea69ab769b4ab317814`. Status: historical/unadjudicated narrative; English source prose.

# Bond-state observability215

## K02845
Source `D0384`, original line3, SHA256 `914eb17dcdc41b2821f9b004de8afe3bbf7d93bfb231aea69ab769b4ab317814`. Status: historical/unadjudicated narrative; English source prose.

Read-only trace. Swift63B4 builds the known 12-byte bootstrap payload from RAMef6. It also sets internal record+1 to 1 when paired marker equals AAAA, else0; this field is outside payload. Queue55D0 treats record+1 as a pipe selector (range0..7) and maintains PID per pipe.

## K02846
Source `D0384`, original line5, SHA256 `914eb17dcdc41b2821f9b004de8afe3bbf7d93bfb231aea69ab769b4ab317814`. Status: historical/unadjudicated narrative; English source prose.

This does NOT make received S1 a bond indicator. Reply5D68..5D84 copies only payload from record+5, then writes S1 from a separate register and clears bit0.5D8E..5D92 selects TXADDRESS from RADIO.RXMATCH. Another reply5F28..5F2C does likewise. Therefore a reply observed on bootstrap address or S1=2 can be sent using the incoming request address rather than disclosing the stored bond state. Packet207 alone does not distinguish paired from unpaired.

## K02847
Source `D0384`, original line7, SHA256 `914eb17dcdc41b2821f9b004de8afe3bbf7d93bfb231aea69ab769b4ab317814`. Status: historical/unadjudicated narrative; English source prose.

Do not label payload2=1F or S1 bit3 as bonded without further producer evidence. Current ID[TRACKER_ID] remains confirmed; bond-state inference remains unknown. No hardware mutation215. Evidence: swift-bond-wire215.txt,swift-txaddress215.txt. Other exploratory range dumps include data and are not executable-code evidence unless their boundaries are validated.

## K02848
Source `D0384`, original line9, SHA256 `914eb17dcdc41b2821f9b004de8afe3bbf7d93bfb231aea69ab769b4ab317814`. Status: historical/unadjudicated narrative; English source prose.

Next safe path: identify an externally distinguishable paired-only response/working-address behavior, or a protocol gate whose reply is rejected for every unpaired state before any negotiation persistence. A blind targeted21-byte negotiation still does not satisfy the no-flash constraint.

## K02849
Source `D0385`, original line1, SHA256 `d34a61cd875fde01ad088ac8f114a0ba16e15c7fc32f7cd67ba30106e3ad7555`. Status: historical/unadjudicated narrative; English source prose.

# User LED observation and bootstrap219

## K02850
Source `D0385`, original line3, SHA256 `d34a61cd875fde01ad088ac8f114a0ba16e15c7fc32f7cd67ba30106e3ad7555`. Status: historical/unadjudicated narrative; English source prose.

User reports first tracker blue light switches on/off rapidly. This confirms powered indicator activity, not a firmware bond-state enum. LED CD28 manipulates runtime mode fields but a reliable color/cadence mapping to saved AAAA marker has not been established. Web controller LED documentation describes different devices and was not used to classify Swift.

## K02851
Source `D0385`, original line5, SHA256 `d34a61cd875fde01ad088ac8f114a0ba16e15c7fc32f7cd67ba30106e3ad7555`. Status: historical/unadjudicated narrative; English source prose.

After this new power observation, bounded fast bootstrap identity query repeated on1/37/77:320slots,0records,return0,nohostretry. PosesonlyHMD0(73rows). Sessionclosed/RAMrestored/MPUrestored alltrue. No negotiation/IR/newpairing transmitted. Raw outputs/fast-identity219.json. No directconnectionclaim. Need distinguish bootstrap state/channel timing/upstreamS1 gate; do notrepeatidenticalnegativeprobe.

## K02852
Source `D0386`, original line1, SHA256 `526de217321fbc02291c12c4d7ca07d40145f6f9e4d435a0882ba6964ef9512c`. Status: historical/unadjudicated narrative; English source prose.

## Bootstrap active probe205 (2026-10-06)

## K02853
Source `D0386`, original line3, SHA256 `526de217321fbc02291c12c4d7ca07d40145f6f9e4d435a0882ba6964ef9512c`. Status: historical/unadjudicated narrative; English source prose.

After user-confirmedPICOoff, bounded no-op poll now tests bootstrapBASE552C6A1E/prefixC0 channels1/37/77 fromSwift75CCfallback. New --poll-bootstrap requires ROYSWIFTPOLLBOOT1, --swift-poll --poll-noop, excludesactive-mask mode and restricts exactbootstrapCRCprofile. Same safe postTX TIMER arming and1800us cleanup; module1773B. No reset/pair/config commands.

## K02854
Source `D0386`, original line5, SHA256 `526de217321fbc02291c12c4d7ca07d40145f6f9e4d435a0882ba6964ef9512c`. Status: historical/unadjudicated narrative; English source prose.

205:322slots in3s,0RXrecords;allcleanuptrue;nohostretry. ConcurrentposeonlyHMDdevice0(70rows). Transmission counts inrawreport retained;no directconnectionclaim. Bothworkingmask204 andbootstrap205 are negative withtrackerawake state notobserved independently afterPICOshutdown. Asked user toverify tracker LED and ordinarypoweron withoutlongpress/reset. Do notrepeat unchangedpolls without this state check or strongerframeconstruction evidence.

## K02855
Source `D0386`, original line7, SHA256 `526de217321fbc02291c12c4d7ca07d40145f6f9e4d435a0882ba6964ef9512c`. Status: historical/unadjudicated narrative; English source prose.

Next derive actual firststationpacket fromSwift6844/68F0 negotiation andPhoenixpair handler, ratherthan assumingopcode0 poll initiateshandshake. Current stationseed data remainavailable offline; fullFICR notauthenticated.20focused decoder/loader tests pass;they donotverifynewhardwareconnection.

## K02856
Source `D0386`, original line9, SHA256 `526de217321fbc02291c12c4d7ca07d40145f6f9e4d435a0882ba6964ef9512c`. Status: historical/unadjudicated narrative; English source prose.

Evidence outputs/bootstrap205.json,bootstrap205.py,bootstrap205.profile.json. Goalactive, standalone6DoF incomplete.

## K02857
Source `D0387`, original line1, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

## Current repair status252/253, 2026-10-06

## K02858
Source `D0387`, original line3, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

Continuous high-priority radio remains disabled after the right-controller
regression. User confirmed right tracking returned after radio stop. Low-priority
bounded251 received zero slots; this is not a working connection mode.

## K02859
Source `D0387`, original line7, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

Pairing-ready tracker test252 used known249 module for3s at20000us slot interval:
160slots,39RFrecords,36working replies with correct sum8 and54IMU samples.
FirstID bytes[TRACKER_ID] observed in two13-byte bootstrap replies. All close/RAM/MPU
cleanup checks passed. Only HMD pose rows were recorded, no both-controller proof.

## K02860
Source `D0387`, original line12, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

Read-only firmware audit253 used extracted Swift-image.bin, base0x1000. Stored
pair entry7574 derives personal address and selects pipe1; startup group is4/78/78
as documented by initializer6658 and branch7574. Existing module only negotiated
on bootstrap1/37/77 and never offered reconnect on that personal startup group.
New module alternates a personal-address21-byte offer on4/78 with the existing
bootstrap sequence, followed by channel2 configuration. Build2042bytes fits2045.
It has NOT been tested on hardware and must not be described as fixed reconnect.

## K02861
Source `D0387`, original line20, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

Native UI hover, queued clicks and readable results installed. API timeout/error
handling repaired. Current systemd radio entry remains a bounded low-priority
10-second diagnostic, inactive after cleanup. New persistent operation is gated
on live reconnect and controller coexistence evidence. OpenVR read-only
pc/frame_app/device_monitor.cpp installed to distinguish registered devices
from valid poses. Awaiting user to keep both controllers and Frame tracking.

## K02862
Source `D0387`, original line27, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

## Direct standalone connection240 — 2026-10-06

## K02863
Source `D0387`, original line29, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

PICO was reported powered off by the user. The user connected a physical tracker to USB power on the PC and entered pairing. Capture239 received 12 hardware-CRC bootstrap records identifying **[TRACKER_ID] / bytes[TRACKER_ID]**, not the initially assumed third tracker[TRACKER_ID]. Physical selection by historical slot/SN was wrong; radio identity must control targeting.

## K02864
Source `D0387`, original line31, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

Capture240 then targeted the observed first ID on bootstrap1/37/77 and switched to **BASE[PRIVATE_LINK_VALUE], prefix00, channel2**. 318 granted slots,94 CRC records:4bootstrap and90working replies. All90working replies have valid application sum8. Of these,86 carry gyro+accel (43body18,43body24),2carry aux-only body6,2are empty polls. These are actual tracker IMU replies with PICO absent from the runtime loop, not a pose bridge. Raw SHA2567c15482ce301f7e3cbb97f9fbc9231d0171486f34e83916e4f90aeef26443431. Exact evidence outputs/station-handover240.json and outputs/direct-connection240-decoded.json.

## K02865
Source `D0387`, original line33, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

This proves one bounded direct connection/IMU session, not ongoing connection, all-three tracking, factory serial verification, controller coexistence or6DoF. Each probe closes MPSL and restores RAM/MPU. Only HMD pose rows were observed during240. Subsequent saved-working-address reconnect238 (executed AFTER240 despite lower filename number) had322slots/0records; reconnect/keepalive remains unresolved. A request to wake the powered tracker is pending for a longer check.

## K02866
Source `D0387`, original line35, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

The station sequence uses discovery9B(C1), targeted offer21B(maxcmd7A,mask01,rawslot), then repeated sum8 records9/17/25/19/4/10 with seq0, S1=3, phase value0. It is an experimental retry sequence, not a completed per-command ACK state machine or camera synchronization implementation. Command25mode1 is included; optical light visibility remains unproven. Successfulfirst configuration payload is00090601003eed197900110107001901010013010000040116000a04000000003e (33bytes). Do not parse stale DMA bytes beyond the length field.

## K02867
Source `D0387`, original line37, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

Prior229–237 silent probes were not connection evidence.229321slots;23022;231321;232323;233323;234322;235320;23612;237321; all0frames.230/236 also had12host retries; other tests had no retries.239 on USB power got12bootstrap records,240gotIMU. This temporal change does not prove low battery was the cause. Windows detected aCH340COM6 interface, but it has not been attributed to the tracker or opened.

## K02868
Source `D0387`, original line39, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

All terminal tests verified session closure/RAM/MPU restore, no Frame flash writes. Ordinary tracker pairing storage was authorized, but tracker flash readback/persistence has not been independently checked. Newexperimental pc/radio/ram/roy_ram_station_link.c fits existing2045byteSRAM region with -Oz; explicit tagged --station-sequence loader guard is required.45focused tests pass. Private device IDs/evidence are not automatically published to GitHub.

## K02869
Source `D0387`, original line42, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

## Fast working-address test218 (2026-10-06)

## K02870
Source `D0387`, original line44, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

40TX/40fastRXhandovers,322slots,0responses. Cleanupverified,no flash. OnlyHMDposes. FirsttrackerLED/controllerstate confirmationrequested. See docs/fast-working218.md. Standalone6DoF incomplete.

## K02871
Source `D0387`, original line46, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

## Reconnect candidate213 (2026-10-06)

## K02872
Source `D0387`, original line48, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

Offline targeted21-byte builder added;9focusedtests pass. No hardware TX213. Paired AAAA branch skips seed/prefix replacement; complete no-flash path and present tracker state remain unverified. See docs/reconnect-candidate213.md and docs/negotiation-persistence212.md. Standalone6DoF incomplete.

## K02873
Source `D0387`, original line50, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

## First full tracker ID from radio mapped to FICR210 (2026-10-06)

## K02874
Source `D0387`, original line52, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

Firsttracker hardwareID nowobserved:bytes[TRACKER_ID], LEword7919ED3E. This is no longer solelyCRC/address reconstruction. RawhardwareCRCpacket207 was04001f0000000a013eed1979. Swift63B4:literalRAM20000ef6;writesfixed04,00,zero[3..5],0A,01,variablecontext+30at2;copies12bytes topayloadrecord+5. At63EA itloadsRAMef6+8=RAMefe, writespayload+8. Startup66BE/66C0 calls7D18 withdestinationRAMefe;7D18 readsNordicFICRDEVICEID[1]. Exactproducer mappinglinksreceivedsuffix toFICR. LowED3E matchespreviousfirstSNstationlogs. Existingseed74735415 yieldsBASE[PRIVATE_LINK_VALUE], independentlymatchinglivefullworkingaddress. RemainingtwofullIDs stillunknown.

## K02875
Source `D0387`, original line54, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

Addedstrictdecode_bootstrap_record for12-byte producerlayout, distinctfrom13-byte6844 identityresponse. RequiresindependentlycheckedRFCRC;doesnotauthenticatearbitraryunvalidatedbytes. VariablehighIDbytes tested, invalidlayout/truncationrejected.22focusedtests pass. outputs/first-tracker-id210.json recordsresult andsource.

## K02876
Source `D0387`, original line56, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

This confirmsfirstID/producer semantics, notcausality ofrequest207 orcompletedconnection. Needconstructverifiedstationnegotiationresponse and trace allpersistentwrites beforeTX undernoflashconstraint. No newhardwaretest thisturn. Standalone6DoF/otherIDs/coexistenceunfinished.

## K02877
Source `D0387`, original line58, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

Evidence outputs/firmware-standalone/swift-id-prefix210.txt (63B4builder),swift-ficr-callers207.txt (66C0destination),outputs/fast-identity207.json. No firmwareincludedinpublication.

## K02878
Source `D0387`, original line61, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

## First bootstrap frame with hardware turnaround207-209 (2026-10-06)

## K02879
Source `D0387`, original line63, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

New ROYSWIFTPOLLFAST1/--poll-fast-rx is restricted to taggedlength9 identityprobe. MODECNF0RU=1 fast ramp; TX SHORTS=11 READY_START|END_DISABLE|DISABLED_RXEN. Software waits until TXstates>=9 ended before removingDISABLED_RXEN, otherwise it wouldcancelhardwarehandover beforeDISABLED. RXstate1..3 increments1656handovercounter; failureendsownedslot. NoextraRXEN aftershortcut. Originalstop_radio clearsSHORTS beforeDISABLE; noautonomousradio outsidegrantedslot. Module1927B fits2045B. PrimaryNordicregistersource:https://raw.githubusercontent.com/nrfconnect/sdk-hal_nordic/main/nrfx/mdk/nrf52833_bitfields.h ; producttiming:https://docs.nordicsemi.com/r/bundle/ps_nrf52833/page/radio.html

## K02880
Source `D0387`, original line65, SHA256 `0345def3ddc153da81dc26774983607c9ce30c09ffdb9b1ae1ef3e751bb4eef9`. Status: historical/unadjudicated narrative; English source prose.

207 (PICOoff,userfirstpairing/secondsearch):14TXEND and14hardwareRXhandovers, oneCRC-validbootstrapframe onchannel1, slotordinal10, callback359us. Payload04001f0000000a013eed1979, S1=2. Slot10 pollPID=1 givesS1=2, matchingreceivedPID. Receivedpayloaddiffers fromninezeroTXbytes; notownTXbufferdecodedasresponse. Finalsample13slots vs14TX reflectsexistinghostsnapshotlag; donottreatasexactfinalslotcount.
