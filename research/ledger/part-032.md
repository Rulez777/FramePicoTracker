# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K02481
Source `D0367`, original line21, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

| # | Fact | Numbers | Evidence |
|---|---|---|---|
| F1 | The "+85 us late reply regime" is OUR longer poll, not tracker latency | host 7e02 query mailbox 22 B vs empty 1 B; +21 B x 4 us/B = 84 us, observed +84.5 us; regime boundaries every 10.0 s = host mode refresh | E3/E4, phase88b/92 |
| F2 | Reply end time for empty polls, slot-relative | 3 B 215 us, 12 B 243, 24 B 298, 30 B 326 (IMU); extra-record replies 38–56 B reach ~410–440 | E7 |
| F3 | Tracker repeats an un-ACKed record in consecutive replies until the ACK takes effect | 2–5 repeats, 1–2 reply pipeline delay; cmd22 status ~1/s | E8 |
| F4 | Tracker drops the link after radio silence | 0.29 s and 0.58 s pauses survive, 0.99 s and 2.0 s cause re-discovery; FW threshold 200/(N+1) windows ≈ 0.6 s | E13, phase94 |
| F5 | cmd19 is the IMU-stream switch and also enables tracker autonomous TX | without cmd19: 0 IMU; dense N0 97.80% CRC/TX, sparse N13 96.77% replies, zero link drops; with cmd19 at sparse N13: 17–20%, repeated drops | E15/E16, phase95/96 |
| F6 | cmd19 sent late on a running link starts IMU immediately | before: 808 replies, all empty; after: 867 replies, 716 with IMU; 572 IMU in ~18 s; 97.05% CRC/TX | E17, phase97 |
| F7 | fe1 (autonomous permission) is cleared by native teardown 64ac; each new session starts with fe1=0 | FW189 651e | E15 |
| F8 | Controllers-OFF radio is not idle: Frame BLE ~65% busy (scanning) | phase78 control1 profile | phase78 |
| F9 | With controllers ON, the common BLE quiet window is ~600 us per 3000 us | Phase89, phase78 detect1 | earlier |
| F10 | Single staged session keeps the link through the controllers OFF->ON transition | Phase99 run1: S1 38.4 s, 834 replies, 0 drops | E19 |

## K02482
Source `D0367`, original line34, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

## Rejected hypotheses
- Cadence 3000 vs 2989 changes the late regime: REJECTED (Phase92 A/B, 25–30% both).
- Tracker-side latency regime / 641c soft-double cost: REJECTED (F1; 144 instructions).
- RTC-truncated window (98 ticks = 2990.72 us) explains sparse-cadence failure: REJECTED
  (Phase95: 41790 us 17.3% vs 41874 us 20.0%; the real cause is F5).
- Moving TXEN earlier: low value, ≤5 us gain (MPSL dispatch dominates ~23 us).
- The earlier Phase91 "74–80% ceiling" claim: VOID (based on the F1 misread).

## K02483
Source `D0367`, original line42, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

## Current approach: Phase99 staged single-session coexistence
Spec: PHASE99_STAGED_COEXISTENCE_SPEC_2026-10-09.md (original reference: `PHASE99_STAGED_COEXISTENCE_SPEC_2026-10-09.md`).
Isolated code: `outputs/phase99/release` (host) and `outputs/phase99/source,build`
(module). Nothing in `pc/` changed.

## K02484
Source `D0367`, original line47, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

- S0 bootstrap with controllers OFF: slot 900 / RX 700, interval 5982 x cycle 7
  (41874 us per poll), frame N13, schema2 mode proof, **no cmd19**.
- S1 keepalive: live DATA writes slot 400 / RX 340 / interval 41874 / cycle 1; blind
  ACK (S1=3); continuity lease (no 22 B mode refresh during short RX); guard AWAIT.
  The user switches controllers on.
- S2: guard STRICT, SWD BLE phase sampling excluding our busy own-base samples;
  require quiet ≥540 us.
- S3: cmd16 N0, wait 3 replies for the native countdown, then write phase target and
  interval 3000 (dense, phase-locked).
- S4: cmd19 -> IMU.
- Module phase99: phase78 (END-before-deadline capture) minus timed-optical metadata,
  plus D1896-gated self re-request after BLOCKED. 1980 B, ELF a9b882d9…7bad1.
- Host code: written by Codex (gpt-6.1-sol) and reviewed by Claude. Claude fixed three
  hardware-level design errors in review (phase lock at a non-3000 interval, N change
  before the countdown, own-base exclusion of idle samples). 20 offline tests pass.

## K02485
Source `D0367`, original line63, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

### Run results so far
- run1: S0 3.31 s -> S1 38.4 s (link held) -> S2 abort after ~0.8 s: controller index2
  result=100 for 0.51 s. Freshly powered controllers were already flaky inside S1
  (ctrl1 invalid 81/63/47 of 99 in three 1 s windows). MPSL 5387 slots, 4411 blocked
  (82%): every block triggers an immediate EARLIEST re-request (~125 slots/s, 5% airtime,
  random phase).
- run2 (controllers_stable_s 10, phase_sample_s 8): S0 abort after ~2 s, 1 reply. A
  review bug armed the drop/rediscovery abort during bootstrap; normal handshake
  retries tripped it. Fixed (abort only from S1 on, +1 test, 21 pass). No RF ran with
  controllers ON.
- run3 (run2 settings + fix): S0->S1 at 4.90 s; S1 held the tracker link for the full
  90 s (1987 replies, 0 drops, 6858 slots, 4651 blocked = 68%). Abort "S1 timeout":
  after the controllers came on (~43 s) they lost tracking ALTERNATELY with a ~10–11 s
  period each (ctrl1 at 48/59/68/79/90 s, ctrl2 at 52/63/73/84/95 s, 1–3 s per burst,
  0 invalid between bursts), so 10 s of continuous readiness never held. Strong
  hypothesis: a beat between our UNLOCKED schedule (41874 us polls + EARLIEST
  re-requests) and each controller's BLE connection events. Details: WORKLOG E21.

## K02486
Source `D0367`, original line81, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

## NEXT STEP for the next AI (user still to choose; Claude recommended A)
Run4 = run3 with:
(A) S1->S2 as soon as both controllers are valid for 2 s (`controllers_stable_s: 2`);
    guard stays AWAIT during the 8 s phase sampling; switch to STRICT (2 controllers,
    150 ms) only 3 s after the phase lock/dense switch in S3. This needs a small code
    change in staged_coexist.py (today S1->S2 sets STRICT immediately). Transitional
    harm is accepted and recorded; only the final locked profile is judged strictly.
    The user must approve relaxing the guard during the transition.
(B) alternative: S1 interval 42000 (fixed BLE phase); risk: the tracker N13 window at
    42000 without cmd19 is untested.
(C) alternative: disable EARLIEST self re-request in S1 (D1896=0); risk: lost slots
    and the 0.6 s link timeout.
Operator procedure that worked (runs 1–3): user turns controllers OFF -> launch
`setsid nohup venv/bin/python phase99/runN/run.py > phase99/runN/runner.out 2>&1 &`
on Frame from /home/steamos/pico-ram-work -> tell the user the start time -> user
turns controllers ON ~20–30 s later (after S1 starts; S0 takes 3–5 s, S0 timeout 25 s,
S1 waits up to 90 s). Create runN by copying phase99/run3 (attempt/invocation.json,
station-config.json, registry.json, run.py with the path replaced; symlinks release ->
phase99/release, build -> phase99/build; recompute inv["files"] hashes if the release
changed). The runner's pgrep guard matches its own SSH command line if that line
contains "openocd" or "roy_ram_loader": launch it in a separate SSH command.

## K02487
Source `D0367`, original line103, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

## Artifacts
- Local: outputs/phase99/{release,source,build,test_staged_coexist.py,run1,run2,run3}.
  Tests: `.venv\Scripts\python.exe -m pytest -q outputs/phase99/test_staged_coexist.py`
  (21 pass). Module ELF a9b882d9…7bad1 (1980 B).
- Frame: /home/steamos/pico-ram-work/phase99/{release,build,source,run1,run2,run3}.
  Nothing is running; every run cleaned up and was verified.
- Earlier evidence: outputs/phase92 (cadence A/B), phase93 (invalid blind-ACK test),
  phase94 (link-loss timeout), phase95 (N13 window), phase96 (no cmd19), phase97
  (late cmd19), phase98 (controllers-ON service-module abort + native baseline).
- Codex (gpt-6.1-sol via agent-intern codex_ask, workspace-write) wrote most host
  code; its sandbox cannot start the venv Python, so the reviewer must run the tests.

## K02488
Source `D0367`, original line115, SHA256 `700c0c45d82b82fbb1be8e8a8edb2266c9344c8fbc6482a6945a2a7126359f0e`. Status: historical/unadjudicated narrative; English source prose.

## Open problems (ordered)
1. Separate controller warm-up flakiness from our S1/S2 churn (run2 tests the warm-up
   side). If S2 still aborts: rate-limit the self re-request or use normal-distance
   re-requests instead of EARLIEST.
2. Untested downstream: S2 quiet-window measurement under our own slots, S3 N change
   from N13 to N0 under phase lock, S4 IMU with RX 340 and a 30 B reply at 326 us
   (14 us margin), blind-ACK loss rate of cmd22 status records.
3. Mode proof during long streaming: the continuity lease is an explicit experimental
   policy, not the project's 30 s lease rule. A tracker reboot would show up as a link
   drop / re-discovery and abort the run.
4. Measure the true 98–100% metric (completed TX denominator) for the phase99 module.
   It has no TX_END counter; polls_scope = scheduled slots.

## K02489
Source `D0368`, original line1, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

CURRENT190 HANDOFF: User reports tracker TURNED ON after requested post-OTA powercycle. Activation NOT yet queried; do not ask another powercycle without evidence. Continue at rewritten complete English handoff (original reference: `K:/AI/pico/docs/STATE_FOR_AI_PHASE173_HANDOFF_2026-10-10.md`) and rewritten start prompt (original reference: `K:/AI/pico/START_NEW_CHAT_PHASE173.txt`). Diagnostic10 OTA1894120/4120+finish_verified,2retries,335.968s,allcleanuptrue; image98868B/SHA040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d. FIRST next task: fresh observer activation proof, NO repeatOTA. Fullbody/90-100% BOTH-controller IMU still unmet. New handoff supersedes older unapproved/untransferred/pending-audit headings below.
CURRENT HANDOFF173: Read Phase173 continuation handoff (original reference: `docs/STATE_FOR_AI_PHASE173_HANDOFF_2026-10-10.md`) and START_NEW_CHAT_PHASE173.txt first. Phase172 99.42185% CONTROL CRC /IMU0; full-body unmet. Phase173 offline TIMER2/PPI dependency is a hypothesis, not a hardware cause proof. No new RF/OTA. Grok4.7medium review pending at handoff creation.

## K02490
Source `D0368`, original line4, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

CURRENT172: BOTH controllersON + mode1/N1/6000,4987CRC/5016TX=99.42185% CONTROL/KEEPALIVE ONLY,IMU0. Fresh800us quiet admittedcompact500/RX400;FA7 remains0/counter112stable after21/17,19NOTsent.2983hostringdrops,controller1unchangedpose2189ms => no sensor/fullbody/healthy-coexistence acceptance. Allcleanuptrue/noRFactive. Read docs/PHASE170_171_JOINT_MODE1_COMPACT_SLOT_2026-10-10.md and outputs/phase172/state-for-ai.json. Older entries historical.

## K02491
Source `D0368`, original line6, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

CURRENT169: mode1/N1/6000 RX800 control complete, 3445/6362CRC=54.1496%,3247IMU,allcleanuptrue. WiderRX insufficient; analyze repeated slot gaps before another RF change. ControllersOFF/fullbody unmet. See docs/PHASE169_MODE1_RX800_CONTROL_2026-10-10.md.

## K02492
Source `D0368`, original line8, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

CURRENT168: GAME mode1/N1/6000 freshly proved;60s complete,2956IMU/7450countergaps,3097CRC/7700TX40.2208%,78ringdrops,allcleanuptrue. No post19 diagnostic timeout. ControllersOFF only; fullbody goal unmet. Read docs/PHASE167_GAME_MODE1_SENSOR_RECOVERY_2026-10-10.md.

## K02493
Source `D0368`, original line10, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

CURRENT167: installed diagnostic09 activated; mode1/N1 verified,634IMU after native17/19, post19 diagnostic timeout, cleanupalltrue. See PHASE167_GAME_MODE1_SENSOR_RECOVERY_2026-10-10.md. Fullbody unproved.

## K02494
Source `D0368`, original line12, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

# Current AI handoff — Phase113 — 2026-10-09

## K02495
Source `D0368`, original line14, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## CURRENT160FINAL — supersedes historical status sections below

## K02496
Source `D0368`, original line16, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

User authorized one exact diagnostic09 OTA through service38 after required pause. Transfer COMPLETE4085/4085+positivefinish/finaldrain, finish_verified,5retries,8394rejected,373.163s recordedprogression. Exact98032B SHA07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266. Isolated successfulOTA46sender/release plus narrowexacthashreadinessadmission; mainwhitelistunchanged. All4cleanuptrue; radioinactive/noLoader/OpenOCDafterward. Read English result (original reference: `PHASE160_SERVICE38_DIAGNOSTIC09_OTA_2026-10-09.md`) and outputs/phase160/summary.json/evidence-manifest.json. No automatic retry or replay.

## K02497
Source `D0368`, original line18, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

ACTIVATIONPENDING: user asked target-only buttonoff/on, FrameON/controllersOFF. No confirmation yet. NeedfreshFE03andFE04exactnonce/chipto prove newdiagnostic09execution; FW189/FE02wire7 insufficient. boot_verified/cadence_verified=false; pose_valid=false and fullbodygoalunmet. Phase161query_loader.py is an offline prepared, unexecuted bounded probe and needs host validation before use. Prior1591437IMUsamples24.0341% accepted-counterspan shows current sensorreturnon07; causeUNKNOWN, notnewfirmwarefix.

## K02498
Source `D0368`, original line20, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Material phase discovery:118/119

## K02499
Source `D0368`, original line22, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Read full6000phase discovery (original reference: `PHASE118_119_FULL_6000_PHASE_DISCOVERY_2026-10-09.md`). Exact118joint765ordinary rows reject3k temporal-covered policy but PASS same strict policy on6k with700us window (OFFLINE,not reusableanchor). MCU correction previouslystillmod3k despiteN1/6kpolls.119alignsMCU24576fractionalRTCmodulus,hostPhaseLock6000,and60bin covered-window admission;16checks passed.2043Bcode,tracker07unchanged. Fresh joint stream notyetmeasured. Never confuse this offline admission with live119success.

## K02500
Source `D0368`, original line24, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Delivery-quality milestone: Phase117 N1 joint

## K02501
Source `D0368`, original line26, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Read Phase117 milestone (original reference: `PHASE117_N1_JOINT_MILESTONE_2026-10-09.md`). N1/6000 joint S2 approx8s:1345CRC header delta /1351stagedTX approximately99.5559%; sequential-boundary limitation. Both final99metrics0invalid/0freeze;IMU0.264retained and1077ringdrops reveal severe host drainage loss. Ordinary588accepted rows show locallycovered600usquiet but no whole-period admission; stopped before19. Allcleanuptrue. New covered-window design being prepared must treat unknownbinsbusy and keep600samples/540us/5%/strictguards. This is a KEEPALIVE milestone, not game sensor success.

## K02502
Source `D0368`, original line28, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Material update: Phase115–116 and new game117

## K02503
Source `D0368`, original line30, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Read N1 cadence result (original reference: `PHASE115_116_N1_CADENCE_2026-10-09.md`). New N1/6000 controller-OFF control45s:1984CRC/2127TX=93.2769%,2036IMU,102ringdrops. Raw schema2 provesmode1/rev7/N1/pending0; finalproof expired after30s,2queryfailures, so long-term confirmation unresolved. Allcleanuptrue. Tracker firmware remains07.115long-burst observer keptbothcontrollershealthy but blindbinsremain.117 now tests N1/6000 on game module with unchanged540us/600sample gate; no N0 switch and no19 beforestrictadmission. Do not preclaim joint success.

## K02504
Source `D0368`, original line32, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Live progress update: Phase114

## K02505
Source `D0368`, original line34, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Read controller channel observation (original reference: `PHASE114_CONTROLLER_CHANNEL_OBSERVATION_2026-10-09.md`).30s stock-only observer:119calls/7616raw/7583accepted, both controllers0invalid/0freeze, cleanup verified.260B MCU observer captures MODE/FREQUENCY atomically with STATE. Two busy BASE groups[PRIVATE_LINK_VALUE] andbc36dc00; all3907accepted busy rows MODE4, varying frequencies. Three-segment folds show locally supported700us quiet runs but whole-period coverage has empty bins; no new game admission. Next improve uninterrupted observer coverage; no tracker/Frame firmware changes. Record every material discovery or delivery improvement here immediately.

## K02506
Source `D0368`, original line36, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

This is the current continuation entrypoint. Read project AGENTS.md first, then this document, the linked original reports and their raw evidence. Preserve older snapshots, attempts and manifests. Labels below distinguish hardware measurement, static analysis and unvalidated hypotheses.

## K02507
Source `D0368`, original line38, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Objective and honest readiness

## K02508
Source `D0368`, original line40, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Build a usable game tracker:98–100% sensor delivery simultaneously with TWO healthy Frame controllers, followed by validated optical-inertial tracking. NOT achieved. Recent94–95% joint figures are KEEPALIVE replies, with IMU0. They are not sensor completeness, sustained healthy coexistence, firmware transfer guarantees or full-body tracking. pose_valid=false.

## K02509
Source `D0368`, original line42, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Tracker firmware and identity

## K02510
Source `D0368`, original line44, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Target serial[TRACKER_SERIAL]; chip ID little-endian[TRACKER_ID]. Native firmware version189; installed custom dual-mode revision07. Historically installed image: `outputs/remote44/candidate07-initial-connection/fw189-dual-mode.bin`,97832bytes, SHA25640941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5. Do not substitute the different97848byte superseded audit-final candidate.

## K02511
Source `D0368`, original line46, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Latest Phase113 live reply verified Frame mode1/revision07. Reboot can reset mode; get fresh nonce/chip-bound proof before another run. Mode or N from an older attempt is not current live proof. Phase113's configured bootstrap was stockN8/frameN13, followed by sparse41874us keepalive. No dense command16 transition or command19 occurred. Do not claim actual finalN was independently queried in Phase113.

## K02512
Source `D0368`, original line48, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Revision08 UART-default candidate is OFFLINE ONLY,97864bytes,SHA2561e111618677983dfaf9be1a4c875463d63bebf2b21094dd3fcfd4aaf7f557078,not flashed,approved_for_ota=false. Native UART enable was sent to installed07: command1 body500100, response5000. No new USB/COM enumeration was observed. UART disable was not sent; persistence after reboot and physical routing remain unknown. See UART/08 report (original reference: `FW189_UART_ENABLE_AND_REV08_CANDIDATE_2026-10-09.md`).

## K02513
Source `D0368`, original line50, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Frame radio and service/game distinction

## K02514
Source `D0368`, original line52, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Frame stock Roy build1788551713, stock SHA2560375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50. Experiments use temporary reserved SRAM only; no Frame flash change. Latest game candidate Phase113:2043loaded bytes, entry2001151d, ELF SHA256bc36b6eff5a6f59a34a5b245a652aeb7c3f842d6f97c086e20554574b3c7fad0, code SHA256b99bdabaa6871cc48393a2e160142d635d96277ef1354a5a7db4130c3d6a8520.

## K02515
Source `D0368`, original line54, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Game coordinator began atPhase99 and continued through113; these numbers are station experiments, NOT tracker firmware revisions. Phase113 is experimental, not a released working game profile. Service52 is a separate controller-OFF diagnostic/OTA profile; its high reception cannot establish coexistence.

## K02516
Source `D0368`, original line56, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Phase113 suppresses tracker RF and immediate EARLIEST retries only inside MCU observer operation5; NORMAL requests continue. Operation4 measures ordinary polling. Three ordinary calls per one paused call; only ordinary rows admit phase. DATA+36 now means configured slot length, not measured actual allocated length.24 exact-image tests passed; Windows Unicorn emitted native diagnostics despite pytest passes, Linux pytest unavailable. Source/build manifests are authoritative; neighboring copied C may not match compiled ELF.

## K02517
Source `D0368`, original line58, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Latest hardware results

## K02518
Source `D0368`, original line60, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

| Experiment | Measured result | Limit |
|---|---|---|
| Phase112 retry-monitor S2, two controllers |184/194=94.8454% retained keepalive replies; IMU0; both final99-sample metrics0invalid/0freeze |Short metrics window, not sustained sensor stream; ordinary442samples below600minimum; quiet300us |
| Phase113 S2, two controllers |187/196=95.4082% retained keepalive; IMU0; controller1 nine invalid/99,maxfreeze172.249ms; controller2 zero invalid/freeze |Ordinary743accepted rows; no admitted quiet window; no19; one controller regressed |
| Phase113 entire attempt |636CRC/3567TX=17.8301%;629retained;0ringdrops |Includes bootstrap/waiting; not S2 ratio |
| Fresh service full-stack recheck, controllersOFF,60s |3447CRC/4355TX=79.1504%;2015IMU;95ringdrops |Service only |
| Service follow-up requested120s, interrupted by controller connection |2116/2212=95.6600%;1222IMU;71ringdrops |Not completed120s, not joint success |
| Current staged OFF IMU control after UART |S4 1052/4330=24.2956%;582IMU/33.9603s=17.1377samples/s |Different module/host path from Service52 |

## K02519
Source `D0368`, original line69, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

All these entered RF attempts verified session/RAM/MPU/trace restoration. Last custom session is stopped. Historical shutdown connected=false means deliberate cleanup, not independently proven live disconnection. User last confirmed both controllers ON before cameras; current sleep/connection status must be read live.

## K02520
Source `D0368`, original line71, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Radio-window evidence and rejected routes

## K02521
Source `D0368`, original line73, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Phase113 ordinary fold:3000us period,30bins,min12rows/bin,longest<=5%busy run300us;6000us period,60bins,min5rows/bin,longest400us. Larger folds lack complete coverage. Paused3000fold shows exploratory500us but has bins with only2rows; cannot admit. Existing streaming gate requires540us and600ordinary samples. Older documents saying500sample minimum are inaccurate; source quiet_window default is600.

## K02522
Source `D0368`, original line75, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Stock Phase11060s previously found650us conservative quiet across six segments. Comparisons do not prove why an active session shrinks it. Short suppression of RF/EARLIEST alone did not produce an admitted game window. Do not reuse old absolute RTC anchors, weaken guards or count paused/unknown bins as quiet.

## K02523
Source `D0368`, original line77, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Phase111 host-controlled pause lasted241.891ms and aborted; rejected. Phase112 moved pause inside the MCU's finite observer, avoiding host round-trip duration. Two112 attempts stopped due invalid HMD tracking before joint sampling; preserved separately. Stock invalid poses persisted after our radio was unloaded. SteamVR service restart restored valid poses, but cause of original runtime state is unproven. User does not want repeated headset-wearing requests: headset on monitor CAN work if cameras are unobstructed, scene is lit and runtime supplies valid poses. Static controllers alone do not establish freeze-free motion.

## K02524
Source `D0368`, original line79, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Native IMU gate — static facts and hardware corroboration

## K02525
Source `D0368`, original line81, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Command19 at8efc/8f00 unconditionally writes1 to20000fe1; ignores body. That flag gates both IMU enqueue and autonomous TX. No verified native command in installed07 splits these permissions. Original command disassembly was read; Grok4.7medium supplied a read-only firmware review. No hardware claim follows from the review.

## K02526
Source `D0368`, original line83, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Mode1 TIMER0 beacon is3000us forN0 and12000us for any nonzeroN. Sparse41874us/N13 exceeds that beacon. Phase95 with19 gave17–20% and link drops; Phase96 no19 held approximately96–98% with IMU0; Phase97 dense delayed19 demonstrated real IMU. Do NOT repeat unchanged sparseN13+19 or vary19's body expecting different behavior.

## K02527
Source `D0368`, original line85, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Candidate6000us/N1 keeps poll cadence below12000us beacon, but native nominal receive aperture4810.5..5975us means6000is an edge case relative to scheduler start. Unknown physical phase must be measured. Current ordinary6000fold is only400us, below540us, so this is an UNVALIDATED hypothesis, not ready joint deployment. Separate firmware permissions or scheduler/controller changes require concrete design, review and hardware validation; no implicit OTA approval.

## K02528
Source `D0368`, original line87, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Sensor and pose readiness

## K02529
Source `D0368`, original line89, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

RF decoder recognizes gyroXYZ,accelXYZ,temperature,24-bit timestamp,sample counter and optionalmagnetometerXYZ. Latest OFF capture contains477magnetometer-bearing records, including standalone6B blocks; duplicates are not unique samples. Active IMU calibration path does not integrate magnetometer. RF reply percent is not generated-sample delivery; modulo sample-gap counts are not an independently validated denominator. Full optical observations, camera/LED calibration, time synchronization and coordinate transforms remain necessary. Processed SLPI quaternion fields are not proof that raw RF supplies a full valid pose.

## K02530
Source `D0368`, original line91, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Evidence, navigation and next work

## K02531
Source `D0368`, original line93, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Read Phase113 report (original reference: `PHASE113_GAME_SCHEDULER_AND_IMU_GATE_2026-10-09.md`), Phase111–112 (original reference: `PHASE111_112_JOINT_RADIO_DIAGNOSTIC_2026-10-09.md`), service comparison (original reference: `SERVICE_REVALIDATION_AND_GAME_MODE_STATUS_2026-10-09.md`), sensor inventory (original reference: `FW189_MODE1_SENSOR_STREAM_AFTER_UART_2026-10-09.md`). Then originalPhase95–110 reports, assembly and exact source as relevant. Earlier handoff101–110 (original reference: `STATE_FOR_AI_PRE_CONTROLLER_PHASE101_2026-10-09.md`) and its archive remain historical evidence, not current instructions.

## K02532
Source `D0368`, original line95, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Raw/results/source: outputs/phase111,phase112,phase113; phase113/analyze.py,analysis.json,evidence-sha256.json; outputs/imu-after-uart; outputs/service-recheck-after-uart; outputs/service-steady-after-uart. Current sources and test results are frozen locally. These Oct9 records postdate Oct8 snapshot provenance: use their local SHA manifests, and locate original historical artifacts through snapshot provenance/artifact-catalog.csv and manifest.csv. Preserve all failures.

## K02533
Source `D0368`, original line97, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Next work: resolve actual shared-radio admission/scheduling deficit and validate N1 receive anchoring before enabling real sensor stream with two controllers. Success must include IMU/sample rate, RF CRC/TX denominator, retained/ring losses, continuous controller health and explicit duration. No keepalive-only success claim. Do not repeat failed variants without a documented changed mechanism. Write each new result in English and link it from AGENTS.md and this current handoff.
# Immediate hardware milestone: Phase119 joint IMU, 2026-10-09

## K02534
Source `D0368`, original line100, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Latest125inprogress: exactmodule/radio unchanged;17sensor-mode65recordcounterrecovery then19enable65records.65offlinecasespassed.124completed17787CRC/18368TX96.8369%,10243retained7544dropsIMU0,cleanupalltrue;19dedupaloneinsufficient. ReadPHASE124_125_SENSOR_ENABLE_RECOVERY_2026-10-09.md. NoOTA,tracker07unchanged,pose_valid=false. BothcontrollersON/valid at125start.

## K02535
Source `D0368`, original line102, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Latest123hardware: sequence1command19verifiedpublication0113010015,generation38,S1=3; retiredbutIMU0.17367CRC/18370TX94.54%,7456ringdrops,cleanupalltrue. OFFLINE correction: native expects exactnextnumber percommand; fresh1rejected,0->skip2rejected,0->1accepted.124prepared65idempotent19records0..63,0 resynchronizesall64expectedstates (64native tests). Radio/module/deadlines unchanged; freshquietwindow stillrequired. UserconfirmedbothcontrollersON and lateststockbothvalid; noOTA. Read123report/outputs124. SequencecauseofIMU0 remainsunproven.

## K02536
Source `D0368`, original line104, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

RESUMED by user. Latest123offline native19duplicatefilter verified: initialseq0setsIMUgate, repeatseq0doesnot, seq1setsit. Emulator-only gateclear; noRFcauseclaim. Prepared123isolatedexact122MCU pluscommand19seq1/mailboxpublicationlogging,1testpassed. Read PHASE123_NATIVE_COMMAND19_SEQUENCE_2026-10-09.md. Physicalreadiness pending: latestFrameHMD+controller1valid,controller2disconnected. HistoricalPAUSEsection below no longer controls authorization after explicitresume. Tracker07/noOTA/pose_valid=false unchanged.

## K02537
Source `D0368`, original line106, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## PAUSED at user request — final Phase122 state

## K02538
Source `D0368`, original line108, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Latest completed joint run122: BOTHcontrollersON from bootstrap; connected targetmode1 in2.781823s, fresh600us/6000phase admitted, final phaseerrorrecentmax16us. Entire110.036336s:18251CRC/18371completedTX=99.3467966%KEEPALIVE,7643ringdrops,10608retained=57.7432%,IMU0. S4after19queue lasted85.996187s:14330TX,9260retained=64.6197%,5033drops,IMU0. S4CRCratio unavailable because onlylast1000headersretained. Final99controllerrows bothvalid/changing,maxfreeze0; fullrunhealth requires complete metrics. Do not claim99%sensor/game/opticalsuccess.119earlier239IMUover2.644s remains the only new shortjointIMU milestone, stopped byoldposeguard.

## K02539
Source `D0368`, original line110, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Current bench user preference: controller pose/invalidity/disconnection recorded without autoRAMreset. Watchdog/Nordic/slot faults stillstop.122supports controllersONbootstrap with isolated explicitflags and exact119module; do not reinstate repeatedoff/on cycles for this bench path. Firmware189custom07unchanged97832B/SHA40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5;08notflashed; noOTA. All122cleanuptrue; stock HMD+bothcontrollersvalid aftercleanup. No active custom run remains.

## K02540
Source `D0368`, original line112, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Next work after explicit resume: read PHASE122_JOINT_BOOTSTRAP_BENCH_2026-10-09.md and rawoutputs/phase122/attempt. Compare emitted command19 record/tag/sequence and configuration17/25 against119whichgeneratedIMU; execute native dispatcher/dedup offline before newRF. TransportACK5doesnotproveapplicationacceptance. Dedup is a HYPOTHESIS only.120state2NORMALretry source notready/unrun:overflows50B; neverload/expandRAM. Groksizeaudit630stimeout,noverdict,noquotaerror. Preserve all121rejected/waittimeouts and122raw records. PAUSE means nofurtherhardwareoperations untiluserresumes.

## K02541
Source `D0368`, original line114, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

LATEST122: exact119MCU connected target with BOTHcontrollers alreadyON; freshmode1 verified. User-requested controller health OBSERVE from startup, so no off/on cycles required for this bounded bench diagnostic. Midrun2355CRC/2435TX=96.7146%KEEPALIVE,IMU0,1534ringdrops; finalpending. Controller1 stationary14.2486s/valid, noRAMreset; both last99valid. All impairments must be separatelyreported. Read PHASE122_JOINT_BOOTSTRAP_BENCH_2026-10-09.md.121observe timedoutS1 because confirmationcameafterwaitingbound; cleanupalltrue, nojointstage.122stillrequires fresh540usphasewindow before19,110sbound,noOTA.

## K02542
Source `D0368`, original line116, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

LATEST policy update121: at direct user request, bench controller health is now OBSERVE (invalid/disconnected/unchanged poses logged without RAM reset). This supersedes strict-validity bench note below. Fresh monitor/watchdog and Nordic faults stillstop,110s bound,16s phase sampling and unchangedwindow admission. Four tests pass. Initial121attempt stopped because one connected controller was not yetvalid; raw retained and release-initial preserved. Current attempt-observe uses exact119ELF/noOTA. Never call diagnostic continuation healthy controller coexistence unless entire measured stage passes separate metrics.

## K02543
Source `D0368`, original line118, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Current experiment121: user-authorized bench unchanged-pose RECORD-ONLY, exact119MCU/ELF, noOTA. Strict connectivity/validity remains; no transition grace,16s phase sample,110s entire session,3tests passed. Do not silently restore the150ms pose-stop policy for this bench session or describe record-only as proof of healthy tracking. See PHASE119_MOTION_CONTROL_AND_120_PREPARATION_2026-10-09.md.120candidate remains unrun (second optimization stilloverflow50B).

## K02544
Source `D0368`, original line120, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Subsequent motion control: both controllers physically moving, exact119module,755ordinary rows; fresh covered window500us, so S2 rejects below540us and no19/IMU. Both final99metrics healthy; all cleanuptrue. This did not reachS4 and cannot resolve the previousfreeze cause. Phase120state2-only NORMAL recovery preparation firstoverflowed58bytes; no deployment/RF. Read PHASE119_MOTION_CONTROL_AND_120_PREPARATION_2026-10-09.md and outputs/phase120; latest hardware idle after cleanup. User can leave Frame on monitor; require motion only during actual joint test.

## K02545
Source `D0368`, original line122, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Fresh 6000 us phase admitted a 700 us window (768 accepted/832 raw ordinary rows). Target FW189 custom revision07, SHA256 40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5, mode1/applied N1 verified at connection; no OTA. With BOTH controllers enabled, S4 received 239 unique IMU samples over approximately 2.644061 s (90.39/s), 306 completed TX, 263 retained replies, 33 ring drops. Hardware CRC delta 287 gives approximate 93.7908% CRC/TX; stage-boundary reads are sequential. This supersedes earlier '119 not yet live' status below.

## K02546
Source `D0368`, original line146, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

130 LIVE MATERIAL MILESTONE after user target-only powercycle and controllersOFF: native common8800 remote restore freshly proves Frame mode;460uniqueIMU already received at1119scheduledslots,270retainedreplies,1485countergaps. Sensors now demonstrably produce data, unlike125/127. This is controller-OFF control, not game success, and does not isolate whether reboot, restored configuration or wide profile was decisive. Installed07 unchanged; noOTA. Preserve final counts and fresh rawproof. Follow with bounded joint6000/N1 test.
# Phase130-132: clean boot, sensor recovery control and joint validation
2026-10-09. Installed target [TRACKER_SERIAL] /[TRACKER_ID]/FW189 custom revision07 unchanged. No OTA. Goal98-100% sensor delivery with TWO healthy Frame controllers remains unachieved.

## K02547
Source `D0368`, original line165, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

133 ACTIVE/NOT YET RESULT: exact132 radio/MCU/19sequencepolicy, new awake-headset control withall3stockvalid immediatelybeforelaunch, BOTHcontrollers alreadyON. No toggling. Userwearing/keepinghandsvisible. NoOTA; installed07unchanged. PreservefinalsensorANDcontroller numbers.
# Phase133-136: awake-headset control and command19 dwell
2026-10-09. Installed target[TRACKER_SERIAL]/[TRACKER_ID]/FW189 custom07unchanged; noOTA. Goal98-100%sensor delivery AND twohealthycontrollers notachieved.

## K02548
Source `D0368`, original line285, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Measured hardware
|Phase|TX|CRC RX|Retained replies|Ring drops|Unique IMU|Transport ACKs|Observed native20 reply sequences|Final recorded mode|
|---|---:|---:|---:|---:|---:|---:|---|---|
|149|3437|1767|1756|10|0|5|[0]|frame|
|150|8065|7556|4041|3508|0|8|[]|None|
|151|18325|18170|10498|7671|0|11|[0, 1]|frame|
|152|5139|2529|2527|1|0|7|[0, 1]|stock|
|153|5142|2585|2570|14|0|11|[0, 1, 2, 3]|stock|

## K02549
Source `D0368`, original line294, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

149 native20 body01 was received from exact target, including retained raw DMA/sourceID/CRC. Native93c8 offline execution intercepted92a8 and confirmed fixed queue args (20000fd0,length1,opcode20,body01); no sensor handler. Repeated same-sequence replies are retransmissions, not independent worker events.
150 assumed command20 counter persisted across reconnect (start1): no20 reply, no19 published; aborted3s afterS4. This is NOT IMU causing a fault. Pending application-proof timeout can trigger host disconnect; report MCU rediscovery alone must not be promoted to physical tracker hang. Its test initially failed collection (wrong import); corrected behavioral test passed after RF launch, before interpreting results. Production RF source was not changed midrun.
151 starts20at0: response0 before single19seq0 and response1 after.110s game6000/N1/500reservation/RX440,99.15416%CRC but IMU0. Native command processing/reply path remains alive after the start attempt; full sensor-queue execution/fe1 readback still not proven by transportACK.
152 fresh STOCK mode0/StockN1 common8800/RX700 likewise20responses0/1 aroundsingle19,IMU0. Mode1-only explanation is insufficient. This diagnostic profile is not game acceptance.
153 stock single17mode09 then07 held1s each,20recordsbetween,thenone19: IMU0. Finalworker sequences recorded in table; no65 loops.
All149-153 cleanup checks true; installed tracker07 unchanged,08unflashed,noOTA. pose_valid=false.

## K02550
Source `D0368`, original line307, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Phase154 candidate
Same153 stock common8800, single sensor80->07 held1s with native20 replies between, thenone19 and final20. No65, no batch, no firmware patch/OTA. Tests ran real nativeb118 with hardware calls intercepted; actual sensor initialization not emulated/proven. Await final hardware result. Exact invocation/ELF/source hashes and scoped mailbox/worker-proof logs under outputs/phase154.

## K02551
Source `D0368`, original line314, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Phase154 final / Phase155 offline diagnostics
154:5139TX/2574CRC(50.0875657%)/2559retained/15drops/IMU0,20responses0..3,stockmode0proved,allcleanuptrue. NoRFactive. Sensor80->07 request did not restoreIMU.155unflashed09 fixedreadonlysensorquery candidate97912B SHA298d6ebb7caa19b66e9952895efa22c09c531b88b07d0c4439d351dacbf45c65;65offlinechecks passed;Grokmedium audit pending. Read PHASE155_READONLY_SENSOR_DIAGNOSTIC_CANDIDATE_2026-10-09.md. NoOTAapproval/07installedunchanged/fullbodyunmet.

## K02552
Source `D0368`, original line318, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Latest156final and155reviewcomplete
156nativeUARToff500000/application5000 gate passed,45s5140TX/2330CRC/2326retained/3drops,IMU0,20seq0..4,allcleanuptrue. UARToffaloneinsufficient.155candidate09Grokreviewcomplete ABIconsistent;2actualnativequeue testsadded,77offlinecasesincludingone4080blocktransfer. Proposaljobunapproved,exacthash298d6ebb7caa19b66e9952895efa22c09c531b88b07d0c4439d351dacbf45c65,97912B. Runtimewhiteliststill05/07only. AwaitnewimageauthorizationatuserrequiredOTA boundary;nofirmwarechanged/noRFactive/pose_validfalse.

## K02553
Source `D0368`, original line322, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

User currentphysicalstate:controllersOFF,targetON,confirmedafter156. Userrequested explanation andGrokadditionaldiagnostic-sufficiencyaudit;dispatchedgrok4.7medium/read-only. NoOTAauthorizationinthisrequest. Preserveuserrequiredpre-OTA pause.

## K02554
Source `D0368`, original line325, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## User prohibition of circular probing
Mandatory FULL_BODY_EXECUTION_CONTRACT_2026-10-09.md: no unchangedfailed RF reruns/65floods/toggleloops. Fourorderedgates: actualIMUfaultlocalization,targetedrepair,jointsensorquality,realSteamVRopticalpose. Grokdiagnostic-sufficiencyreviewpending. Framephysicalstateuncertain/SSHtimeoutdoesnotblockofflinework. NoOTAauthorization.

## K02555
Source `D0368`, original line329, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

## Phase157 finalstate32/userrequesteddualreview
97976B SHA23beacdfe9185dbe107fa4e6a810e6646a4eeaf528df05077e0908490eab2451;79offlinecases including4083blockexacttransfer. Addsproducer/ringcounters,UARTram,sensorpostgate,mainqueueindices to4basicfields;schema3body32fitsoriginal32Bstack. Old15526Bunflashedsuperseded. UserasksGrokandGeminiopinion afterselfcheck;bothread-onlyauditsactive. NoOTAauthorization,newhashnotwhitelisted/07installedunchanged. FollowPHASE157_DIAGNOSTIC_STATE32_AUDIT_INPUT_2026-10-09.md and strictfacts-onlyexecutioncontract.
# Phase158 latest offline preparation

## K02556
Source `D0368`, original line333, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Latest159 freshpreOTAcheck: IMU RETURNED on unchanged07:1437unique/4542gaps,24.0341% accepted-counter span,814CRC/2598TX31.3318%,hostdrop0. Raw replay independentlymatches. Product targetverified, initialmode0proof, finalmodequeryfailed/connectedfalse; pendingbyte1 inlaterFE02. Allcleanuptrue; noOTA. See PHASE159_PRE_OTA_IDENTITY_AND_IMU_RETURN_2026-10-09.md, avoidoverclaimingIMU0currentstate. Acquisition now intermittent/command/deliveryunknown; diagnostic158stillunflashed. Stopatuserboundary, noautoretry. BothcontrollersOFF, nojointgameproof.

## K02557
Source `D0368`, original line335, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Final dual reviews returned without code blockers; exact-source-only external review, lead verified binaryhash. See Phase158 audit final disposition and preserved reviewer documents. Diagnostic preparation complete, stopped at user-required pre-OTA call; no transfer. Require Frame and target enabled, controllersOFF for service38. Real boot/recovery/FE03/FE04/sensor/game pose remain unverified. Do not equate currently closedgate with historical rootcause or combine pages as atomic.

## K02558
Source `D0368`, original line337, SHA256 `1b6864ee316780eb9e888e4cc8149ef730e49cb7ea303ab75348329b1cc906fc`. Status: historical/unadjudicated narrative; English source prose.

Read docs/PHASE158_UPSTREAM_GATES_AUDIT_2026-10-09.md first. Diagnostic candidate09 now adds schema4 fixed upstream IRQ gates00441/fa7/fb0 (25B reply) alongside unchanged state32 schema3. Exact candidate98032B SHA07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266, 96offline cases pass including4085/4085 exact staging transfer. No live sensor/boot proof. Both repeat audits dispatched. Installed07 unchanged; candidateUNFLASHED; pose_valid=false and full-body goal unmet. User devices off, will enable after call before OTA; do not launchRF/OTA now. Productionwhitelistonly05/07. Phase157 inconsistent approval flags preserved/restored; Gemini prior hook was not human authorization. Use fresh Phase158 proposal, not historical157 runner.

## K02559
Source `D0369`, original line1, SHA256 `892740ea1d33c39d8baed20d161049d501dfcac49ca238de5633ce1e81af26e6`. Status: historical/unadjudicated narrative; English source prose.

CURRENT365 ENGLISH HANDOFF: read docs/STATE_FOR_AI_PHASE365_HANDOFF_2026-10-11.md FIRST after root rules. User authorized ONE-tracker pilot, then requested handoff. Pilot NOT started/fullbody UNACHIEVED.363 selected frozen273 identity/ACK/lifecycle review overlay exists but hard-refuses launch;364 changed triangulated helper accepted21/30 selected stable recorded pairs,21/59 total, conditional geometry ONLY. Helper/detector/wholepipeline/package/cleanup integration still open. NoRF/OTA/SRAM/newcapture after273; installed diagnostic10/210 unchanged. FirstONE verified then rolloutother2 then3body/controllers. Goal ACTIVE. Current sources303/307/355/357/358 edited; older manifests historical. New hashes outputs/phase365/handoff-manifest.json.

## K02560
Source `D0369`, original line3, SHA256 `892740ea1d33c39d8baed20d161049d501dfcac49ca238de5633ce1e81af26e6`. Status: historical/unadjudicated narrative; English source prose.

CURRENT362 SINGLE-TRACKER PILOT AUTHORIZED:360 real retained measurements20/30 selected stereo pairs, 0.592mm/1.132deg max accepted steps; recorded754-pair median compute6.934x faster.361 canonical native25 ACK/writer hooks prepared, conservative Roy/camera uncertainty and independent evidence snapshots added; NOT installed/NOT RF-ready. Need selected runtime/lifecycle/exclusive scheduling/stereo adapter/old native10 identity replacement and sealed bounded live candidate. NoRF/OTA/SRAM/newcapture since273; diagnostic10/210 unchanged. FirstONE verified then other2 rollout/fullbody. Read docs/PHASE360_361_PILOT_MEASUREMENTS_AND_ACK_BRIDGE_2026-10-11.md.
