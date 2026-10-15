# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K03121
Source `D0398`, original line316, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read PHASE100_VARIANT_A_2026-10-09.md (original reference: `docs/PHASE100_VARIANT_A_2026-10-09.md`) before retrying Phase99. Run4 reached S2 but found no admitted quiet window; cleanup all true. S2 190/192 keepalive replies, zero IMU; controller 2 freeze 232.6 ms. Critical measurement limitation: RTC/state/BASE/RTC SWD brackets median 1525.9 us, exceeding the desired 540–600 us quiet window. Physical absence of a BLE pause is unproven. Next: precise MCU-side sampling, preserve raw records; do not relax guard/busy threshold based on the smeared histogram. 25 offline tests and Grok medium review passed for the bounded transition candidate; no OTA.

## K03122
Source `D0398`, original line318, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## State for AI after Phase91-99, 2026-10-09

## K03123
Source `D0398`, original line320, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read STATE_FOR_AI_2026-10-09_PHASE91_99.md (original reference: `docs/STATE_FOR_AI_2026-10-09_PHASE91_99.md`) first (includes run3 and NEXT STEP; new-chat prompt outputs/phase99/START_NEW_CHAT.txt): facts F1-F10, rejected hypotheses, staged single-session design (Phase99), run results, open problems. Diary with per-step numbers: docs/WORKLOG_CLAUDE_2026-10-09.md.

## K03124
Source `D0398`, original line322, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Claude work log 2026-10-09 (running diary, numbers)

## K03125
Source `D0398`, original line324, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read docs/WORKLOG_CLAUDE_2026-10-09.md (original reference: `docs/WORKLOG_CLAUDE_2026-10-09.md`). KEY CORRECTION E4: Phase91/92 "late +84.5us regime" is OUR longer poll (host 7e02 query 22B vs empty 1B, 2Mbit/s 4us/B), not tracker latency; Phase91 ceiling claim void for empty-poll 90d. Long late blocks = unanswered mode query until 8s timeout.

## K03126
Source `D0398`, original line326, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase92 cadence A/B (controllers OFF), 2026-10-09

## K03127
Source `D0398`, original line328, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read results (original reference: `docs/PHASE92_CADENCE_AB_LATE_REGIME_2026-10-09.md`). Tracker already mode1 (fe02 rev7 N0->N2, no setter).
Exact88b module RX800, 30s each: 3000 91.54%/95.68%, 2989 96.70%/84.55% RX/TX; IMU860-957.
Late(+85us) regime 25-30% in ALL four: cadence hypothesis rejected (Phase91 correlation confounded).
8800/N2 bootstrap 99% late. All cleanup verified; no OTA/flash.

## K03128
Source `D0398`, original line333, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase91 reply-latency regimes limit short-RX plan, 2026-10-09

## K03129
Source `D0398`, original line335, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read offline analysis (original reference: `docs/PHASE91_TRACKER_REPLY_LATENCY_REGIMES_2026-10-09.md`).
Retained TXEN/capture DWT pairs: tracker replies bimodal, late mode +84.5us for all
lengths, persistent seconds; late fraction Service52 0.3%, 84c 7.5%, 82 19.9%, 88b 26.5%,
mode53 60%. With RX330 the late IMU (30B 416us) cannot fit: ~74-80% ceiling, early 30B
margin ~4us. 90d coordinator as planned cannot reach98-100%. 641c soft-double cost
rejected as cause (144 instr). Mechanism unknown. No RF; Frame SSH unreachable.

## K03130
Source `D0398`, original line342, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Current resume handoff and target, 2026-10-09

## K03131
Source `D0398`, original line344, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read complete new-chat handoff (original reference: `docs/RESUME_COEXISTENCE_98_100_2026-10-09.md`) first for Phase90 work. User target is now98–100% tracker reception simultaneously with BOTH healthy Frame controllers. No joint success. Latest short-only RAM90d is1773B, ELF b77c10c7...9193; offline107passed. New peer finite-policy and closed phase helper are NOT wired into a coordinator/loader. No90 RF or OTA. Full handoff includes source hashes, raw evidence, prior failed controls, known limitations, missing tests and next integration steps. Preserve historical reports below.

## K03132
Source `D0398`, original line346, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase89 extended controller gaps, 2026-10-09

## K03133
Source `D0398`, original line348, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read measurement and short-profile constraints (original reference: `docs/PHASE89_EXTENDED_CONTROLLER_GAPS_2026-10-09.md`).60s native-only:13184raw/13144accepted,206calls; both0invalid/0freeze, full restoration. Six-segment common600us quiet at3000/6000/9000us gives338.965us conservative RX budget. Long handshake/mode refresh unsupported; no joint RF or99% proof.

## K03134
Source `D0398`, original line350, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase88 fine RX observations, 2026-10-09

## K03135
Source `D0398`, original line352, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read measurement and coexistence budget (original reference: `docs/PHASE88_FINE_RX_AND_COEXIST_BUDGET_2026-10-09.md`). Controllers OFF, fresh mode1, 30s:1546CRC/1601TX=96.5646%;1533retained=95.7527%;962IMU;11ring drops; all cleanup verified. Nominal10us callback checks are not hardware END timestamps. Conservative Phase85 RX budget259.373us does not establish a fit for observed IMU callbacks299/326us. No joint RF or99% claim.

## K03136
Source `D0398`, original line354, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Latest cadence controls and controller measurement, 2026-10-09

## K03137
Source `D0398`, original line356, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read docs/PHASE84_86_87_MODE_RESTORE_AND_CADENCE_2026-10-09.md and docs/PHASE85_CONTROLLER_SCHEDULE_HARDWARE_2026-10-09.md.84b fresh proof mode0 caused safe reject;86 remotely proved0/N1 ->1/N2, no OTA.84c mode1/N0 at2989us:89.3422%CRC/TX,84.5758%retained,2624IMU,249ringdrops.87 nominal2600us:14.9674%CRC,proof expired,294IMU; not accepted. All entered RF cleanups verified. Controller85 native-only2176raw/2170zero-bracket,34calls,12metrics windows:both0invalid/0freeze and full restore/postcheck pass. Fine second-half quiet fold520--550us, not robust1100us; no900us service admission or combined-RF claim.99% and two-controller>=60% still unachieved.

## K03138
Source `D0398`, original line358, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase83 regression; Phase84 prepared, 2026-10-09

## K03139
Source `D0398`, original line360, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read results and preparation (original reference: `docs/PHASE83_BATCH_REGRESSION_AND_84_PREPARATION_2026-10-09.md`). Host batching test:590CRC/4538TX=13.00%,575retained,349IMU,13ringdrops; target stopped replying halfway, mode proof expired. Cause unknown; batching disabled by default, historical evidence preserved. Exact07 RTC/TIMER2 conversion verified;2989us control with original publication prepared, not run pending target-wake confirmation. All83 cleanup verified.99% and controller coexistence goals remain unmet.

## K03140
Source `D0398`, original line362, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase81/82 dense Frame service hardware results, 2026-10-09

## K03141
Source `D0398`, original line364, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read measured results (original reference: `docs/PHASE81_82_DENSE_FRAME_SERVICE_2026-10-09.md`). Controllers OFF, actual mode1/rev7 proof, N0/M2, nominal3000us/cycle1/RX800. Exact Service52 module:4939CRC/5236TX_END=94.3277%;4445retained/5236=84.8930%;2710IMU;493host-ring drops; one mode-query timeout. Prior service79 module gave1689/8125 nominal20.7877%. Cleanup verified, no OTA/setter. Final connected=false is shutdown semantics, not a live RF failure. Narrow diagnostic policy only; common-grid/stock OTA guards preserved. Neither99% nor two-controller>=60% proven.

## K03142
Source `D0398`, original line366, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Service52-to-Frame3000 comparison, 2026-10-09

## K03143
Source `D0398`, original line368, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read verified settings and next diagnostic (original reference: `docs/SERVICE52_TO_FRAME3000_COMPARISON_2026-10-09.md`). Service52 uses cycle1/N0/M2, RX800, nominal4727us; Phase79/80 use cycle7/FrameN13/RX700. Dense Frame N0/RX800 is a hypothesis requiring fresh mode proof and phase-specific policy; do not bypass common-grid or stock OTA gates. Actual saved reply intervals are not uniformly4727us. No hardware run or firmware change in this comparison.

## K03144
Source `D0398`, original line370, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase79/80 controller-OFF RX-window controls, 2026-10-09

## K03145
Source `D0398`, original line372, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read hardware results (original reference: `docs/PHASE79_80_RX_WINDOW_CONTROL_2026-10-09.md`). Schema2 proves mode1/revision7 during both controls. Native service retries; RX700; periods42000/41874us yielded103/878 (11.73%)/3IMU and119/895 (13.30%)/6IMU. Neither stable connected at end; all cleanup verified. Nominal interior margin alone does not solve phase; native RTC truncation and MPSL inter-poll gaps require investigation. No setter/OTA; >=60% with two controllers remains unachieved.

## K03146
Source `D0398`, original line374, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase78 controller-OFF control runs, 2026-10-09

## K03147
Source `D0398`, original line376, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read results (original reference: `docs/PHASE78_CONTROL_RUNS_2026-10-09.md`).
Controllers OFF: no BLE quiet window (uniform~65% busy) -> phase lock off; MPSL
then grants only~7 slots/s. Phase78 END-check captures14B discovery at409us
(deadline400). Deadline400: product info0; deadline700 (slot900): product info3x,
serial [TRACKER_SERIAL] fw189. Still never connected/IMU0; mode UNKNOWN.
Option2 (controllers on, weakened gate) NOT run: 600/400 profile cannot pass product info.

## K03148
Source `D0398`, original line383, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase78 detect1 rejected BEFORE RF, 2026-10-09

## K03149
Source `D0398`, original line385, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read result (original reference: `docs/PHASE78_DETECT1_PRESTART_REJECTION_2026-10-09.md`).
User asked tracker detection with controllers on. Phase78 END-check ELF8c314daf,
phase67 loader/profile6000x7/slot600, read-only schema1, no setter/OTA. BLE gate
measured600us quiet (5% cap), needs800us -> rejected before module/session.
Controllers0invalid/0freeze; no tracker mode/presence/RX measured. First launch
import error (missing PYTHONPATH) before debugger. Gate not weakened, no retry.

## K03150
Source `D0398`, original line392, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Current English external-review handoff, 2026-10-09

## K03151
Source `D0398`, original line394, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read connection/research state with measured numbers (original reference: `docs/CONNECTION_AND_RESEARCH_STATE_FOR_EXTERNAL_REVIEW_2026-10-09.md`).
Claude review (original reference: `docs/CONNECTION_STATE_CLAUDE_REVIEW_2026-10-09.md`): coexist400us gate drops product-info/fe02 (observed>=441us in service52) -> handshake cannot complete; RX loop checks tick>=end before EVENTS_END (loses frames ending (last poll,end]). Missing control: coexist profile with controllers OFF.
Fresh native100/100 samples: HMD and both controllers valid; custom receiver
stopped, PICO mode/connection currently unknown. >=60% goal remains unachieved.
Phase76 native sampler33calls/2112raw/2090zero-tick accepted;12controller windows,
0invalid/0loggedfreeze; full RAM/static/control/hook restore and postcheck pass.
Conservative native folds:700us whole/600us eachhalf at30bins; finer-bin results
do not establish robust700us. Phase75 cleanup regression preserved, extra MPU
selector rewrite removed. Sampler recovery/liveness hardening remains pending.
Machine-readable state and live evidence SHA inventory:outputs/connection-state-20261009.

## K03152
Source `D0398`, original line405, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase74 entry failure diagnosed; baseline verified, 2026-10-09

## K03153
Source `D0398`, original line407, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read failure and checkpoint proof (original reference: `docs/PHASE74_PREENTRY_FAILURE_2026-10-09.md`).
Standalone runner omitted disable_mpu=True before RAM entry. No samples/mode/RF
result. Read-only postcheck CODE/DATA/counts/static match full checkpoint, hook0,
session inactive. Original failing script/report preserved. Corrected runner8
offline cases pass; no blind repeat/recovery claim from cleared hook alone.

## K03154
Source `D0398`, original line413, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase74 native sampler OFFLINE candidate, 2026-10-09

## K03155
Source `D0398`, original line415, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read compiled diagnostic and boundaries (original reference: `docs/PHASE74_NATIVE_BLE_SAMPLER_CANDIDATE_2026-10-09.md`).
224-byte native CPU sampler candidate eliminates SWD transfer from timestamp
bracket; no RF/MPSL calls, finite loop, existing RAM reservation.4 exact-binary
M-class tests pass. No live installation/invocation; restoration review pending.

## K03156
Source `D0398`, original line420, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase73 stock sampler uncertainty, 2026-10-09

## K03157
Source `D0398`, original line422, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read raw diagnostic and next action (original reference: `docs/PHASE73_BLE_SAMPLER_UNCERTAINTY_2026-10-09.md`).
20s register-read-only diagnostic: both controllers valid in all1978 samples.
RTC bracket median1648us/max4242us; four-read midpoint is asymmetric. Finer
histogram alone does not prove safe700us quiet window. Raw7848 observations saved.
Local raw-retention/offline-bin changes:36 tests pass, not deployed. No RF/OTA.

## K03158
Source `D0398`, original line428, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase71/72 admission results, >=60% goal active, 2026-10-09

## K03159
Source `D0398`, original line430, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read deployment correction and window results (original reference: `docs/PHASE71_72_WINDOW_RESULTS_2026-10-09.md`).
Both passed initial native tracking. Phase71 still used older loader (800us gate).
Corrected phase72 loader SHA92de4119...cc6d requires700us, measured only600us;
rejected before RF/session entry. MPU restored, no OTA. No reception/mode proof.
Do not claim prepared70 deployment matched tested local loader. Preserve evidence.

## K03160
Source `D0398`, original line436, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase70 pre-start rejection; phase71 prepared, 2026-10-09

## K03161
Source `D0398`, original line438, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read measured rejection (original reference: `docs/PHASE70_PRESTART_REJECTION_2026-10-09.md`).
Controller index2 was connected but native tracking invalid; guard rejected before
RF entry. No tracker mode or reception measurement. No loader/OpenOCD remains.
Fresh phase71 reuses the sealed RX500 release with new evidence paths; awaits
both native hands. No OTA. Offline combined tests: 101 passed.

## K03162
Source `D0398`, original line444, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase70 RX500 prepared, >=60% goal ACTIVE, 2026-10-09

## K03163
Source `D0398`, original line446, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read exact experiment (original reference: `docs/PHASE70_RX500_PREPARED_2026-10-09.md`).
Observed mode replies424--446us cannot pass old400us gate. Exactphase66 module,
500usRX/600slot,early-returnquiet700us with100us lead/post; restricted opt-in
loader gate,150ms controller guard unchanged. DesiredFrame/schema2 proof required.
Earlyproduct host race fixed;94 tests pass. User wake requested,new70 not yetrun.
No OTA/persistent deploy, priorfailed69TXcounteroverflow rejected.

## K03164
Source `D0398`, original line453, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase67 strict BLE quiet-window prepared, >=60% goal ACTIVE, 2026-10-09

## K03165
Source `D0398`, original line455, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read candidate/clarification (original reference: `docs/PHASE67_STRICT_QUIET_WINDOW_2026-10-09.md`).
Grok diagnosis completed: oldfinalslots0 is cleanup artifact; previous runs
did grant RF slots despite0hostpolls. Preserve distinction and historical logs.
Newopt-in maxbusy.05, exception slotcount fix, exactphase66 RAMgate;43 tests pass.
Isolated67 prepared, nativecontrollers invalid/disconnected, wake requested.
No67RF yet/noOTA; no60% or safetyclaim. Normal release unchanged,guard150ms.

## K03166
Source `D0398`, original line462, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase66 OFFLINE gate candidate, >=60% goal ACTIVE, 2026-10-09

## K03167
Source `D0398`, original line464, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read candidate/tests (original reference: `docs/PHASE66_OFFLINE_GATE_2026-10-09.md`).
SkipRF if BLE correction outside+-60us, retaining idle timer/nextslot/deadline.
ExactELFe8815477...63ee2040B,9 ARM tests pass. First live attempt guard-rejected
before hardware entry:2validcontrollers absent. Hardware validation pending,
normal release untouched, no OTA. Goal not achieved; Grok diagnosis running.

## K03168
Source `D0398`, original line470, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Coexist64/65 goal >=60% ACTIVE, 2026-10-09

## K03169
Source `D0398`, original line472, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read new results (original reference: `docs/COEXIST64_65_PROGRESS_2026-10-09.md`).
64 deadlineELF minute preserved healthycontrollers but1emptyreply/1434polls,0IMU.
65 reasonedfastbootstrap3000/cycle1/900slot abortedcontroller2 trackingloss;
phaseerror~1ms/startblocked. Both cleanup true; no OTA/setter. Investigate
initial EARLIEST/retry phase, do not repeatfast65 unchanged. Grok review running.

## K03170
Source `D0398`, original line478, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Coexist63 deadline module prepared, goal >=60% ACTIVE, 2026-10-09

## K03171
Source `D0398`, original line480, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read prepared profile/history (original reference: `docs/COEXIST63_DEADLINE_PROFILE_PREPARED_2026-10-09.md`).
Iteration28 deadline-bounded ELFc9b4972b...3ac10 prepared for fresh test; previous
runs used older8f784fc3. User says controller slept; nativecontroller2 invalid.
Wake/valid baseline needed. No63 RF yet, no OTA;150ms guard retained.
Grok source diagnosis running. Goal remains >=60% with2healthy controllers.

## K03172
Source `D0398`, original line486, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Coexist62 diagnosis: preflight fixed, native tracking unhealthy, 2026-10-09

## K03173
Source `D0398`, original line488, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read diagnosis (original reference: `docs/COEXIST62_TIMING_DIAGNOSIS_2026-10-09.md`).
MCU rephases distances on3000us BLE grid;5982 nominal window claim withdrawn.
Loader rejects nonmultiples before RF;68 regression tests pass. Fresh6000 run
still abortscontroller2 freeze162ms,0PICO polls;cleanup true. Native no-RAM
baseline controller2 invalid/no changes~10s. Root cause unresolved; guard retained.

## K03174
Source `D0398`, original line496, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read result (original reference: `docs/TIMING61_REPEAT_RESULT_2026-10-09.md`).
Controller2 freeze162ms;0target polls,cleanup all true,no RF/mode proof.

## K03175
Source `D0398`, original line499, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Timing60 repeat guard abort, 2026-10-09

## K03176
Source `D0398`, original line501, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read result (original reference: `docs/TIMING60_REPEAT_RESULT_2026-10-09.md`).
Controller2 freeze162ms;0target polls/IMU, no mode/RF metric. Fourth aligned
abort; cleanup all true, module unloaded. Preserve logs; diagnose startup.

## K03177
Source `D0398`, original line505, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Timing59 user-requested repeat: guard abort again, 2026-10-09

## K03178
Source `D0398`, original line507, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read repeat result (original reference: `docs/TIMING59_USER_REQUESTED_REPEAT_2026-10-09.md`).
Same aligned grid, controller1 freeze163ms,0target polls/IMU. Cleanup all true.
Three aborted aligned runs; RF quality/mode not measured. Diagnose startup,
do not infer user-motion fault or weaken guard without evidence.

## K03179
Source `D0398`, original line512, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Timing58 repeat guard abort, nominal alignment NOT validated, 2026-10-09

## K03180
Source `D0398`, original line514, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read repeat evidence (original reference: `docs/TIMING58_REPEAT_GUARD_ABORT_2026-10-09.md`).
User confirms hands moving/target ON. Same5982us grid; controller freeze162ms,
0target polls/IMU. All cleanup true. Two aborts: investigate baseline/startup/BLE
timeline, do not blame user movement, bypass150ms guard or blindly repeat.

## K03181
Source `D0398`, original line519, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Timing57 aligned common grid, guard-interrupted, 2026-10-09

## K03182
Source `D0398`, original line521, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read calculation and attempt (original reference: `docs/TIMING57_COMMON_GRID_2026-10-09.md`).
5982us*7=41874us stockN8/frameN13 fits BOTH conservative windows with~120us
minimum margin; old42000 edge and stockN9 mismatch corrected in isolated config.
Original coexistence RAM/ble lock/150ms guard retained. Test aborted controller1
freeze172ms before target polls; no RF performance/mode proof. Cleanup all true.
Fresh repeat needs hands moving/target awake, do not bypass guard or replay57.

## K03183
Source `D0398`, original line528, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Coexist56 controllers valid, PICO NOT connected, 2026-10-09

## K03184
Source `D0398`, original line530, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read bounded result (original reference: `docs/COEXIST56_CONTROLLER_TEST_2026-10-09.md`).
User/native baseline confirms2 tracked controllers;60s original coexistence
RAM profile ran.2 answers/1433 scheduled target polls,0IMU/noidentity/mode proof.
No watchdog abort; controller metrics preserved. Not working PICO coexistence.
All cleanup true, module unloaded; no OTA/setter. Target awake/state unknown.

## K03185
Source `D0398`, original line536, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Coexist55 blocked BEFORE RF: native controllers invalid, 2026-10-09

## K03186
Source `D0398`, original line538, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read pre-start result (original reference: `docs/COEXIST55_PRESTART_TRACKING_INVALID_2026-10-09.md`).
Both controllers connected but native OpenVR valid=false/result100, HMD valid.
Guard rejected before RAM/RF entry; no module loaded, no loader/OpenOCD remains.
Original coexistence ELF/profile staged; do not bypass tracking guard.

## K03187
Source `D0398`, original line543, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Mode53 HARDWARE remote0->1 / revision07 proven, 2026-10-08

## K03188
Source `D0398`, original line545, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read remote switch evidence (original reference: `docs/MODE53_REMOTE_SWITCH_HARDWARE_2026-10-08.md`).
Full07 host on isolated release, common8800us stockN1/frameN2, ordinary service27
RAM ELF (no service38 guard bypass/OTA). CRC fe02 nonce/chip actualmode1/rev7/N2
proves execution07 and applied remote switch; fd02 staging alone not proof.
60s1601TX/283CRC RX=17.68%,236retained=14.74%,0drops. Proof later expired30s,
final mode unknown; last verified1. All cleanup true. Grid performance poor,
do not equate mode1 with100% or infer it from reception ratio.

## K03189
Source `D0398`, original line555, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read measured reception (original reference: `docs/SERVICE52_RADIO_TEST_2026-10-08.md`).
User requested service38.45s,2189 TX/2160 CRC RX=98.6752%,2156 retained
answers=98.4925%,3 ring drops,2027 IMU. Exact target confirmed; no OTA/mode
command. Mode/07 revision unconfirmed. Cleanup all true, module unloaded.

## K03190
Source `D0398`, original line562, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read repeat result (original reference: `docs/STOCK51_NATIVE_FRAME_REPEAT_2026-10-08.md`).
3000 samples/30.38s, no connected generic tracker or target log matches.
No RAM radio/SWD/OTA; native visibility only, mode/RF loss unknown.

## K03191
Source `D0398`, original line568, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read longer observation (original reference: `docs/STOCK50_NATIVE_FRAME_LONGER_CHECK_2026-10-08.md`).
3000 native OpenVR samples: no connected generic tracker, no target log matches.
No RAM radio loaded. Repeated enumeration cannot measure raw RF loss or mode.

## K03192
Source `D0398`, original line574, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read awake repeat (original reference: `docs/STOCK49_AWAKE_NATIVE_FRAME_CHECK_2026-10-08.md`).
User confirms tracker ON.1000 OpenVR samples/10.11s: no connected generic
tracker, no exact target in latest300 XRService lines. No RAM radio/SWD/OTA.
Native device absence is not a packet-loss metric or mode proof.

## K03193
Source `D0398`, original line581, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read native-only observation (original reference: `docs/STOCK48_NATIVE_FRAME_CHECK_2026-10-08.md`).
No RAM radio/SWD/OTA. OpenVR200 samples: no connected generic tracker,
controllers disconnected; target not found in selected XRService log terms.
No native PICO packet denominator available, so do NOT call this0/10/100% RF
or infer tracker mode. Frame remains stock, activation07 still unconfirmed.

## K03194
Source `D0398`, original line587, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Nordic47 STOCK STATE RESTORED, 2026-10-08

## K03195
Source `D0398`, original line589, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read correction/cleanup (original reference: `docs/NORDIC47_STOCK_RESTORE_2026-10-08.md`).
User meant removing RAM module. Incorrect service27 probe interrupted; all
cleanup true, no loader/OpenOCD, radio service inactive. No RAM module running.
Its statistics are NOT native Frame measurements; mode/07 activation unknown.

## K03196
Source `D0398`, original line596, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read physical result (original reference: `docs/OTA46_REVISION07_TRANSFER_SUCCESS_2026-10-08.md`).
Post-button45s schema2 check: target responsive,2243/2285 CRC RX,2141 IMU;
no fresh revision7 nonce reply. Actual running revision/mode UNCONFIRMED.
Probe cleanup all true. Do not reflash merely to resolve absent query proof.
User confirmed start;4077/4077 blocks, finish_verified,19 retries, exit0.
All cleanup true; inactive/no loader. No replay. Phone OFF success is not
causality proof. User power-off/on requested; fresh schema2 revision07 nonce/chip
proof required before claiming new code execution. FW189/legacyfe01 insufficient.
Automatic mode/coexistence/recovery remain unverified, A1 remains known.

## K03197
Source `D0398`, original line608, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read repeat preparation (original reference: `docs/OTA46_PHONE_OFF_PRESTART_2026-10-08.md`).
User requests repeat, phone/controllers OFF and target ON confirmed. Same07
image/service38/isolated45 release, no code change. Fresh guarded pause identity
matches; ota_start_sent=false, cleanup all true. New job staged, unexecuted.
Stop before actual OTA per prior user instruction; final start confirmation pending.
Phone interference and native interrupted-OTA state remain unverified.

## K03198
Source `D0398`, original line617, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read hardware outcome (original reference: `docs/OTA45_REVISION07_FAILED_2026-10-08.md`).
2559/4077 ACKed, block2560 application ACK timeout10s, no finish/activation proof.
All station cleanup true; radio inactive, no loader/OpenOCD. Tracker OTA state
unknown. Preserve attempt logs; no replay/resume/automatic second attempt.
Sequence0 at failure is an observation, not established cause. New07 not
confirmed installed. Prior offline full-transfer success is not hardware proof.

## K03199
Source `D0398`, original line624, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## OTA45 revision07 USER-CONFIRMED ATTEMPT STARTED, 2026-10-08

## K03200
Source `D0398`, original line632, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read pre-start preparation (original reference: `docs/OTA45_REVISION07_PRESTART_2026-10-08.md`).
User selects risky07; controllers OFF/target ON confirmed. Exact07 size/hash
added to explicit CLI risk exception; hardware unknowns/approved_for_ota=false retained.
Isolated Frame ota45/release, normal release unchanged. Fresh exact serial/chip;
guarded pause result paused_before_ota_start,ota_start_sent=false. Both sessions
cleanup all true. Fresh job staged, NEVER executed. Stop before actual OTA until
final human confirmation; no replay of pause or successful39.
Grok admission/pause review (original reference: `docs/OTA45_REVISION07_GROK_REVIEW_2026-10-08.md`)
finds no concrete code blocker; hardware unknowns unchanged.
36 focused guard tests; exact07 service38 full4077-block offline campaign passes.
