# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K00801
Source `D0226`, original line5, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

User returned with both controllers. Phase108 stock-only 15-second baseline completed: 56 calls, 3584 rows, 3577 zero-span rows; both controllers had 16 metric windows with zero invalid samples and zero logged pose freeze. All RAM/MPU/hook restoration checks passed.

## K00802
Source `D0226`, original line7, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

Phase103 A (41874us/RX340) reached S2, then rejected phase admission at 40.488421s because 30-bin coverage was incomplete. Its S2 retained responses were 190/197 TX_END (96.4467%), IMU0. This is keepalive delivery, not streaming success. Thirty native calls produced 933 accepted rows; histogram: [54,62,55,27,24,16,33,29,35,35,43,54,50,54,50,32,50,37,30,26,23,16,12,0,3,4,7,8,30,34]. No S3/S4 or command19. Transitional controller impairment occurred. All cleanup checks passed.

## K00803
Source `D0226`, original line9, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

Phase103 B (42000us/RX340) never left S1. User confirmed controllers were ON but the second hand would not appear. It was interrupted at79.235698s:1790CRC/5603TX=31.94717%, IMU0. Last controller2 window:99/99invalid and79236.620584ms unchanged pose; controller1 healthy. Clean interruption exit0 is not successful coexistence. All cleanup checks passed. Six-second stock tracking after unload ended with both connected/valid/result200; this supports interference but does not prove its exact mechanism.

## K00804
Source `D0226`, original line11, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

A repeat 15-second stock-only observation in outputs/phase109/controller-stock-baseline completed:56 calls/3584 rows, all restoration checks and post-cleanup controller validity true. No tracker RF session or flash writes.

## K00805
Source `D0226`, original line13, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

## Isolated changes

## K00806
Source `D0226`, original line15, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

The old MCU observer samples32 consecutive RTC ticks, roughly1ms per burst. Phase109 accepts a new row after at least3 RTC ticks, spreading32 rows across at least93 ticks (2838.135us), close to one BLE period. Finite spin limit increases8192->32768. Interrupt mask is restored after each individual register bracket, not held for the whole burst. A stopped RTC remains bounded. Actual hardware duration, coverage and effect on an active session remain unmeasured. Better coverage is a hypothesis, not a guarantee; own-radio exclusions may still leave missing bins.

## K00807
Source `D0226`, original line17, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

An explicit connected_phase_start option admits S1->S2 after2s with a fresh valid HMD and two connected controllers, without requiring valid controller poses during this transition. This removes the circular wait observed in B. Validation requires the existing8s observation and hard12s grace, and rejects controller-OFF controls. Streaming still requires the unchanged strict two-controller/150ms guard before command19. Coverage>=5 per bin, busy<=5%, quiet width>=540us remain unchanged. No extended grace or relaxed phase gate.

## K00808
Source `D0226`, original line19, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

This deliberately permits temporary invalid controller poses during the bounded transition. Connected does not mean tracked. It must never be reported as healthy joint operation. A missing admission, expired grace or failed strict guard aborts the trial.

## K00809
Source `D0226`, original line21, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

## Build and checks

## K00810
Source `D0226`, original line23, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

Exact candidate2044bytes, entry0x2001151d, no reservation expansion.

## K00811
Source `D0226`, original line25, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

- ELF SHA256:13fdd7ae413574aafa4ecc68f4ca0abc9026002499a6a51541874aeda2fbccc7
- Code SHA256:ae9258ce928f37474fce6b0102b03b1ef15c94cbddce6a533310c9fc689edbec
- Source SHA256:3a860be47bf45625269e8b7d3c98dd716e66d62457a4a6eecdc3c95dc15da0a8

## K00812
Source `D0226`, original line29, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

47 offline tests passed, including exact machine-code boundedness/IRQ restoration/three-tick spacing across RTC wrap, current retry reservation length, TX_END denominator, host transition gating and actual script import. One configuration-validation regression was found and fixed before deployment; a test-fixture grace omission was corrected. The final direct script-import check also passed after sealing the loader hash.

## K00813
Source `D0226`, original line31, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

Candidate files: outputs/phase109/source, build, release, tests.xml. One-shot joint runner: /home/steamos/pico-ram-work/phase109/joint-stride3-connected41874/run.py. It is prepared, not executed at this checkpoint. Duration180s; fresh OFF bootstrap first, ON at proven S1, sparse41874/RX340, then admitted dense3000. No OTA or persistent firmware changes.

## K00814
Source `D0226`, original line33, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7-medium focused design review was requested; verdict pending at this checkpoint. No external approval is claimed. Preserve all previous evidence and sealed pre-controller archive. Goal98–100% with two continuously healthy controllers remains unmet.

## K00815
Source `D0226`, original line35, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

Review update: the focused Grok4.7-medium request timed out after210s without a verdict. This is not an approval and not a reported quota error. Primary review and47offline tests remain the available validation.

## K00816
Source `D0226`, original line38, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

## First hardware result

## K00817
Source `D0226`, original line40, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

The one-shot joint candidate was executed with fresh Frame-mode proof, controllers OFF for bootstrap and then ON at user confirmation. It reachedS1 at7.721988s andS2 at33.944761s; aborted at42.196809s because no quiet window passed admission.29 native calls yielded928 raw/908 accepted rows; all30bins covered, minimum13 rows/bin. Coverage improved over A's empty bin, but the longest<=5%busy run was300us, below the required540us. Therefore no phase lock,S3/S4,command19 orIMU. S2 retained190/197TX=96.4467%, total802CRC/4294TX=18.67722%,794retained, ringdrops0. Total ratio includes bootstrap and waiting and is not an IMU result. Both controllers were valid in the final recordedS2 metric window; preceding transition metrics were invalid and remain part of the evidence. All four cleanup flags true. Evidence outputs/phase109/joint-stride3-connected41874/attempt; offline fold result-analysis.json and analyze_result.py.

## K00818
Source `D0226`, original line42, SHA256 `f46eb168753c0b617f567934eb34bd5fe38b9e6bfed64c68f69905413c5970de`. Status: historical/unadjudicated narrative; English source prose.

The stock-only15s comparator has3576 zero-span rows. Its exploratory30-bin fold contains a900us low-busy run, but three other bins have fewer than5rows (2,3,4). It would fail the current whole-period coverage gate and does not establish a robust900us window. Do not use that width as admission or infer that the tracker short slot fits safely. Next investigate sampling/scheduler interaction and a fully covered stock comparison before another RF attempt; do not relax busy/width gates or repeat this candidate unchanged.

## K00819
Source `D0227`, original line1, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

# Phase110: stock controller schedule and observer comparator

## K00820
Source `D0227`, original line3, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

## Setup and execution

## K00821
Source `D0227`, original line5, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

User confirmed both controllers ON and was asked to move both hands in view. Custom tracker RF remained unloaded. A 60-second native-only baseline reused the exact Phase89 runner and244B Phase77 ELF. The historical runner was absent at its old remote path; it was copied unchanged into phase110 before execution. No hardware started on the initial missing-file attempt.

## K00822
Source `D0227`, original line7, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

A second15-second native-only comparator installed the exact2044B Phase109 image but invoked only operation4. It did not open an MPSL session or transmit tracker packets. The comparator adapts the established checkpoint/restoration runner to the packed32-word buffer. BASE is not retained; crossed RTC span is a boolean flag, not an exact tick duration. It is not equivalent to a live-session coexistence test.

## K00823
Source `D0227`, original line9, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

## Hardware results

## K00824
Source `D0227`, original line11, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

| Quantity | Stock60 / old observer | Stock15 / stride3 observer |
|---|---:|---:|
| Calls |214|48|
| Raw rows |13696|1536|
| Zero-span rows |13665|1516|
| Metric windows |61|17|
| Controller1 invalid samples |0|0|
| Controller2 invalid samples |0|0|
| Maximum logged pose freeze, both |0ms|0ms|

## K00825
Source `D0227`, original line21, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

Both runs exited0, all RAM/static-MPU/MPU-control/native-hook restore checks and post-cleanup controller validity true. No flash writes or tracker RF session. The adapted comparator omits the optional completed field in its report; success is established by exit0, retained rows and restoration checks, not a missing field. This logging omission should be corrected only in a future runner revision, preserving this executed script.

## K00826
Source `D0227`, original line25, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

## Conservative stock fold

## K00827
Source `D0227`, original line27, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

The stock60 rows were analyzed with the unchanged Phase89 method: six chronological segments,50us bins; every common quiet bin requires>=3rows and<=5%busy independently in every segment. Unknown bins are not quiet. Reference RTC tick397167.

## K00828
Source `D0227`, original line29, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

| Fold period | Shared quiet width | Conservative RX budget (subtract61.035us RTC uncertainty and200us padding) |
|---|---:|---:|
|3000us|650us|388.965us|
|6000us|600us|338.965us|
|9000us|650us|388.965us|
|12000us|600us|338.965us|
|18000us|600us|338.965us|
|24000us|500us|238.965us|

## K00829
Source `D0227`, original line38, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

For3000us the shared run begins800us relative to the reference. This reference belongs to this historical capture; it is not authorization to reuse that absolute anchor after controller reconnection. STATE observations do not prove an exclusive scheduler reservation. These results support the existence of a usable stock pause, not98–100% joint reception.

## K00830
Source `D0227`, original line40, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

Whole-period30-bin coverage is incomplete even in stock60: counts include0/1/4 in some bins. Its exploratory1100us run is not a valid admission. The stock15 stride3 fold similarly has undercovered bins, despite a700us exploratory run. Do not bypass the live quiet_window coverage gate using either value. The conservative six-segment650us result is a separate documented analysis.

## K00831
Source `D0227`, original line42, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

## Comparison and next hypothesis

## K00832
Source `D0227`, original line44, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

Phase109 joint stride3 covered every30-bin cell (minimum13rows) but only300us passed<=5%busy. Stock native-only stride3 caused no controller pose regression. Thus the observer change alone is not established as the reason for controller disruption. Live MPSL reservations/keepalive retries, changed BLE event timing after controller enable, and sampling selection remain unresolved causes. Captures differ in duration, session state and sample coverage; this is not a fully matched causal experiment.

## K00833
Source `D0227`, original line46, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

Do not repeat the unchanged joint candidate or relax540us/5%/150ms guards. Next design a bounded control that separates active-session scheduling from actual tracker RF and retains true TX_END counts, fresh mode proof, raw native rows and independent controller metrics. A live-session pause must respect prior observed0.6–1s tracker link-loss sensitivity; closing/swapping code for several seconds is not acceptable. The ability to preserve connection through such a controlled pause is untested. Any modified callback must retain desired-slot-length retries and exact cleanup verification.

## K00834
Source `D0227`, original line48, SHA256 `1aff60a92a4e7ddc95d8e0d59dd5dcb5e6dbfa6bb1e9fc910440bac969ef551d`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase110/controller-stock60, stride3-stock15, analysis.json, observer-comparison.json, analyze_stock60.py, compare_observers.py, executed stride3_native_only.py and evidence-sha256.json. Prior records and ZIP are preserved. Current hardware is restored;98–100% simultaneous tracker/controller goal remains unmet.

## K00835
Source `D0228`, original line1, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

# Phase111–112: tracker and two-controller radio diagnostic

## K00836
Source `D0228`, original line3, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

Hardware2026-10-09. Goal remains98–100% tracker sensor delivery simultaneously with both healthy Frame controllers. This goal is NOT achieved. Service52 results are not game-mode results.

## K00837
Source `D0228`, original line5, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

## Implemented change

## K00838
Source `D0228`, original line7, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

Phase111 cloned the Phase109 game coordinator and unchanged2044B image. During S2 it alternated ordinary native observations with host-controlled tracker RF pause through peer0 reserved bit. Two host tests verified restoration in finally and exclusion of paused samples from admission. Hardware stopped on the first paused invocation: host pause241.891ms exceeded150ms bound. All four cleanup checks passed. This implementation is rejected; do not repeat it.

## K00839
Source `D0228`, original line9, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

Phase112 moves pause inside MCU observer operation5; operation4 measures ordinary active polling. Peer0 transmission is paused only inside the finite32768-spin/32-row observer loop; cleared before native completion. Timeslot requests continue, other peers remain paused. No command19 or dense transition without admission from ordinary rows. The paired paused rows are diagnostic only and cannot admit a phase lock.

## K00840
Source `D0228`, original line11, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

Module2043B, ELF SHA25695510686c56300c2027cc0cdc65d94eeb8affe7a860cd1a4be25a5bf6224d432; code SHA256faa97d444c1058b1c37f47b2b9a55acb3798faca42bcb3c89df8937ddabf4b19. To fit the fixed2045B region, ABI tag text moved from loaded data to ELF entry symbol ROYSTATIONTHREE4 and linker entry changed accordingly. Loader tag validation still checks the ELF file; fixed entry address remains unchanged. First-row sentinel was replaced with initial RTC minus3; modulo difference remains24bit. No flash writes.

## K00841
Source `D0228`, original line13, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

20 Unicorn checks passed locally, including operations4/5, IRQ restoration, peer pause restoration, TX_END timeout accounting and current-length retry. Windows Unicorn emitted native access-violation diagnostics despite pytest reporting passes; Linux rerun was unavailable because Frame venv lacks pytest. This limitation is not hidden hardware verification. Exact build/source hashes preserved in deployment.json.

## K00842
Source `D0228`, original line15, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

## Attempts and restored stock tracking

## K00843
Source `D0228`, original line17, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

Phase112 initial and retry-hmd stopped because HMD pose was invalid for approximately0.5s. They never reached paired joint sampling, and all cleanup checks passed. Stock tracking_watch afterwards reported invalid identity poses for HMD and both connected controllers while custom radio was already unloaded. SteamVR service was restarted, after which stock HMD and one controller returned valid/result200. This supports a runtime-state problem separate from an active custom-radio failure; it does not prove its original cause. User left the headset on the monitor and controllers before its cameras; wearing the headset was not required for the final measurement.

## K00844
Source `D0228`, original line19, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

## Completed paired joint measurement: retry-monitor

## K00845
Source `D0228`, original line21, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

S0->S1 at2.90135s; both controllers connected continuously and S2 began29.71021s. S2 ran the configured8s sampling stage:

## K00846
Source `D0228`, original line23, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

-194completed transmissions;184retained replies =94.8454% service/keepalive replies.
-IMU0; command19 was not issued. Do NOT call this94.85% sensor delivery.
-Final S2 metrics for each controller:99samples,99pose changes,0invalid,0invalid episodes,0ms maxfreeze in that metrics window. Whole transition health is separately recorded, not assumed clean.
-28native calls/896raw rows:14ordinary calls448raw/442accepted;14paused calls448raw/444accepted.
-Ordinary observer spans4.608–7.233ms; paused spans4.761–6.989ms. These timestamp spans are not exact task-entry/exit pause bounds, but host round-trip delays no longer hold the RF pause.
-Exploratory3000us/30-bin fold,>=5rows/bin and<=5%busy: ordinary longest quiet300us, paused400us. Both arms cover all30bins at>=5rows. Each arm uses its own reference; no absolute-phase equivalence claim.
-Admission rejected because ordinary arm442samples is below the existing500sample minimum. No strict dense stream, no phase lock, no IMU claim. Even its exploratory300us quiet interval is below required540us; merely adding samples is not a justified fix.
-Final cumulative796CRC/3469TX=22.9461% includes bootstrap and waiting. It is not the joint S2 reply ratio.788retained,0ring drops. All session/RAM/MPU/trace cleanup checks true; no active experiment remains.

## K00847
Source `D0228`, original line32, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

## What changed in our understanding

## K00848
Source `D0228`, original line34, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

Short MCU-local suppression of tracker transmission did not restore the earlier stock650us quiet width. It produced400us in this paired sample, versus300us with RF. Live reservations/retries, sampling selection and changed BLE timing remain possible factors. This is not evidence that on-air tracker transmission alone causes the shrink. Prior10–11s beat explanation remains a hypothesis, not a proven cause.

## K00849
Source `D0228`, original line36, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

Do not repeat Phase109 unchanged, Phase111 host pause, or blindly reduce the540us admission requirement. Next candidate should measure/suppress new EARLIEST requests during a bounded silent observation while preserving NORMAL cadence and fresh last-reply bounds, thereby separating scheduling burden from RF. No such new scheduler candidate has been hardware validated here. Controllers can remain positioned before the headset; static poses alone cannot prove absence of frozen tracking.

## K00850
Source `D0228`, original line38, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

## Evidence and reviewer

## K00851
Source `D0228`, original line40, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

`outputs/phase111/attempt`; `outputs/phase112/attempt`, `retry-hmd/attempt`, `retry-monitor/attempt`; source/build/test scripts and manifests in `outputs/phase112`; analysis script `analyze_pair.py`, `paired-analysis.json`, `evidence-sha256.json`. The Oct9 artifacts postdate the Oct8 provenance catalog.

## K00852
Source `D0228`, original line42, SHA256 `6235aebb9656b148419e7b79b9e55402efa55cb147ff53f280153bb20389ac48`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7medium read-only review confirmed the compiled Service52/full-stack distinction and proposed separating timeslot request burden from RF during bounded silent sampling. Its suggestion was advisory; it did not approve Phase112 hardware or prove a game-mode solution. No OTA or tracker revision change.

## K00853
Source `D0229`, original line1, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

# Phase113 game scheduler and IMU gate

## K00854
Source `D0229`, original line3, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

Hardware and static results2026-10-09. Target [TRACKER_SERIAL], mode1/revision07 freshly proven. Tracker firmware unchanged; no OTA. Goal98–100% sensor delivery with two healthy controllers remains unmet.

## K00855
Source `D0229`, original line5, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

## Concrete implementation

## K00856
Source `D0229`, original line7, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

Compared with112, MCU observer operation5 also suppresses immediate EARLIEST retries while peer0 is paused. NORMAL requests remain unchanged; host recovery remains available after a blocked request. Paused samples occur every fourth call, leaving three quarters of samples as ordinary admission inputs. No relaxed quiet gate or early command19.

## K00857
Source `D0229`, original line9, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

Image2043B: ELFbc36b6eff5a6f59a34a5b245a652aeb7c3f842d6f97c086e20554574b3c7fad0, codeb99bdabaa6871cc48393a2e160142d635d96277ef1354a5a7db4130c3d6a8520. LTO enabled. To remain within the fixed SRAM region, DATA+36 now reports configured slot lengthD1876, rather than independently decoded last-granted request length. Observer key renamed configured_slot_us; do not claim it measures actual allocated length.24 exact-image Unicorn checks passed, including suppressed retry while paused, normal retry preservation, TX timeout exclusion and IRQ/pause restoration. Windows native diagnostics remain as documented in112; no Linux pytest available.

## K00858
Source `D0229`, original line11, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

## Joint hardware measurement

## K00859
Source `D0229`, original line13, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

Bootstrap->S1 at12.16615s; S2 at30.66468s after both controllers connected.32native calls over8s: ordinary24calls/768raw/743accepted; paused8calls/256raw/253accepted.

## K00860
Source `D0229`, original line15, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

S2:196completed TX,187retained keepalive replies=95.4082%,IMU0,ringdrops0. Final S2 metrics: controller1 had9invalid samples and one invalid episode in99samples,maxfreeze172.249ms; controller2 had0invalid and0freeze. Later metrics show further loss of controller1. This run does NOT establish two healthy controllers throughout.

## K00861
Source `D0229`, original line17, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

Ordinary3000us fold has minimum12rows/bin and longest quiet300us; ordinary6000us/100us-bin fold has minimum5rows/bin and longest quiet400us. Longer9000/12000/18000/24000folds have incomplete whole-period coverage. Paused3000us fold shows exploratory500us but minimum2rows/bin, so it is not an admitted window. Do not use paused rows to admit streaming.

## K00862
Source `D0229`, original line19, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

Admission failed with No quiet window before command16 dense preparation or command19. Unlike112 this is not a too-few-samples failure. Source quiet_window default minimum is600, not500: correct the older112 report's sample-floor statement when interpreting it;442was below either threshold.

## K00863
Source `D0229`, original line21, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

Final cumulative636CRC/3567TX=17.8301%,629retained,IMU0. This includes bootstrap/waiting and is not S2 delivery. Exit1 is the deliberate rejection; session closure/RAM/MPU/trace restoration all verified true. Current temporary radio stopped.

## K00864
Source `D0229`, original line23, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

## Native IMU limitation independently checked

## K00865
Source `D0229`, original line25, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7medium read-only review inspected the command audit and original firmware. Command19 at8efc/8f00 unconditionally sets20000fe1=1 and branchesaca4; body is ignored. This flag gates TIMER0 automatic transmissions and IMU enqueue; rev07 has no native command separating them. Relevant artifacts: outputs/fw189-command-audit15/native-handler-ranges.asm, outputs/swift189-full.asm and pc/radio/tracker_dual_mode_patch.py. Lead directly checked command19 instructions and mode cadence constants; the full review is advisory static analysis, not live validation.

## K00866
Source `D0229`, original line27, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

Frame mode uses3000us beacon forN0 and12000us for nonzeroN. Sparse41874us/N13 polling exceeds that12000us beacon. Prior Phase95/96/97 results already show why enabling19 on sparse N13 fails; do not repeat it unchanged. A proposed next cadence is6000us/N1, below the beacon deadline, but N1's scheduler aperture4810.5..5975us relative to its native start means exact6000is an edge case requiring measured phase proof. It is not ready for two-controller deployment, and current ordinary6000fold still admits only400us, below540us requirement.

## K00867
Source `D0229`, original line29, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

## Evidence and next constraint

## K00868
Source `D0229`, original line31, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

Preserved source/build/invocation/raw/cleanup: outputs/phase113. Reproduce folds with analyze.py; analysis.json and evidence-sha256.json preserve metrics/provenance. Snapshot Oct8 catalogs cannot establish provenance for these later Oct9 artifacts.

## K00869
Source `D0229`, original line33, SHA256 `0bf1e02d103b2be6a07bb94b1d3e263f7620b65cd8c6c34eeffee0075f9b5c3f`. Status: historical/unadjudicated narrative; English source prose.

Next work must resolve the shared-radio reservation/window deficit and validate N1's actual receive anchor before real IMU streaming. Simply changing19's payload, substituting6000, increasing sample count, or reporting keepalive percentage as sensor delivery is rejected. Full optical-inertial game pose remains unverified,pose_valid=false.

## K00870
Source `D0230`, original line1, SHA256 `71540c396a738f8e499d829d3b1a62522e03a12d8e75d6d99d22a516275389a8`. Status: historical/unadjudicated narrative; English source prose.

# Phase114: controller channel observation

## K00871
Source `D0230`, original line3, SHA256 `71540c396a738f8e499d829d3b1a62522e03a12d8e75d6d99d22a516275389a8`. Status: historical/unadjudicated narrative; English source prose.

Hardware2026-10-09, stock Frame radio only, no tracker RF session or flash write. New260B observer adds RADIO.MODE/FREQUENCY reads inside the existing short IRQ-masked state bracket. Output rows append state/mode/frequency to the six original fields; legacy fields metadata in samples.json is incomplete, use the nine-column decoder in analyze.py.

## K00872
Source `D0230`, original line5, SHA256 `71540c396a738f8e499d829d3b1a62522e03a12d8e75d6d99d22a516275389a8`. Status: historical/unadjudicated narrative; English source prose.

30s completed:119calls,7616raw,7583zero-span accepted rows. Both controllers: maximum invalid0 and maximum pose freeze0ms in recorded metrics. Both valid after cleanup. RAM/MPU/hook restoration verified. ELF e77f1cb1fbb00de5a0fa55b9186f12ab475b7f505551a328302885f19261a9d7; code a09213b134251eccf0b73536e4d479c37423f94214821f5318dc710506f528ad.

## K00873
Source `D0230`, original line7, SHA256 `71540c396a738f8e499d829d3b1a62522e03a12d8e75d6d99d22a516275389a8`. Status: historical/unadjudicated narrative; English source prose.

Busy accepted observations3907, all MODE4. BASE groupsPRIVATE_LINK_VALUE (original reference: `2151busy rows`) andbc36dc00(1756); not bound to left/right identity. Frequency varies across channels. This establishes live register observations, not BLE connection-period identification or joint tracker delivery.

## K00874
Source `D0230`, original line9, SHA256 `71540c396a738f8e499d829d3b1a62522e03a12d8e75d6d99d22a516275389a8`. Status: historical/unadjudicated narrative; English source prose.

Three-segment100us-bin folds find shared quiet700us at3000/6000/9000/12000us, but whole-period minimum coverage is0 in every fold. Only locally covered quiet runs are supported; there is no admitted whole-period scheduler window. Existing short64-row one-tick bursts can underobserve some phases. Next observer should span longer uninterrupted time while leaving IRQs available between atomic reads, retain strict two-controller health and forbid RF changes. No old RTC anchor reuse.

## K00875
Source `D0230`, original line11, SHA256 `71540c396a738f8e499d829d3b1a62522e03a12d8e75d6d99d22a516275389a8`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase114/controllers-channels30; source/build manifests; analyze.py,analysis.json,evidence-sha256.json. Tracker remains installed FW189/revision07, mode1 last proven inPhase113; no new live tracker-mode claim.

## K00876
Source `D0231`, original line1, SHA256 `71a3902727621ad2bd77f6684490b73302916c7f87a88add82d4251a42ab8270`. Status: historical/unadjudicated narrative; English source prose.

# Phase115–116: N1 cadence validation

## K00877
Source `D0231`, original line3, SHA256 `71a3902727621ad2bd77f6684490b73302916c7f87a88add82d4251a42ab8270`. Status: historical/unadjudicated narrative; English source prose.

Hardware2026-10-09. Installed tracker FW189/revision07 unchanged; no OTA. Target[TRACKER_SERIAL]/chip[TRACKER_ID].

## K00878
Source `D0231`, original line5, SHA256 `71a3902727621ad2bd77f6684490b73302916c7f87a88add82d4251a42ab8270`. Status: historical/unadjudicated narrative; English source prose.

Phase115272B stock-only observer:64rows separated by>=5RTCticks, finite262144spin loop, IRQs restored between atomic observations.30s,119calls,7616raw/7521accepted. Both controllers0invalid/0freeze, cleanup alltrue. Long bursts did not remove whole-period blind bins. Locally covered quiet runs700us remain distinct from whole-period admission. Evidence outputs/phase115/controllers-longburst30,source/build,analysis.json.

## K00879
Source `D0231`, original line7, SHA256 `71a3902727621ad2bd77f6684490b73302916c7f87a88add82d4251a42ab8270`. Status: historical/unadjudicated narrative; English source prose.

Phase116 new controller-OFF profile uses actualFrameN1 and nominal6000us, exact Service52 module, slot900/RX800. This is a new proposed game cadence validation, not joint success. Isolated loader admits only this fixed target/profile/schema2 and rejects stock before preparation; ordinary common-grid checks elsewhere unchanged.8offline checks passed, with previously documented Windows Unicorn native diagnostics.

## K00880
Source `D0231`, original line9, SHA256 `71a3902727621ad2bd77f6684490b73302916c7f87a88add82d4251a42ab8270`. Status: historical/unadjudicated narrative; English source prose.

45s completed:1984CRC/2127TX=93.2769%;1880retained=88.3874%;2036uniqueIMU;102ringdrops. Two raw schema2 bodies prove initialmode1/rev7/N0/pending0 and thenmode1/rev7/N1/pending0. Final mode proof expired after30s with2queryfailures; final stateconfirming, despite ongoing recent IMU. Thus this does NOT prove indefinitely refreshed capability/status or99% sensor delivery. Sample-gap6330 is not an independently validated completeness denominator.
