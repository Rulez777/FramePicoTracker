# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K00161
Source `D0163`, original line210, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- `outputs/service52/service52/` — exclusive service counters and raw records.
- `outputs/mode53/attempt/` — actual schema2 mode0→1 proof and weak RF run.
- `outputs/coexist56/attempt/`, `outputs/coexist64/attempt/`, `outputs/phase67/attempt/` — completed controller-on runs.
- `outputs/phase71/attempt/`, `outputs/phase72/attempt/` — admission rejection and exact deployments.
- `outputs/phase73/` — raw host-side brackets.
- `outputs/phase74/attempt/` — initial failure, checkpoint, executed runner and subsequent readback comparison.
- `outputs/phase75/attempt/`, `outputs/phase76/attempt/` — executed native sampler, raw records, controller metrics and restoration checks.
- `outputs/phase76/analysis.json` — numerical conservative folding results.

## K00162
Source `D0163`, original line219, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Keep four categories explicit: **hardware observation, offline result, hypothesis, unknown**. Do not infer tracker mode from a10%/100% reception classifier, infer no RF from a final slots0 publication, equate transport ACK with applied mode, equate OTA block completion with100% RF, or treat compilation/CRC/audits as controller-coexistence proof. Earlier guard-aborted runs could have granted RF slots even when final host counters were zero; original reports are preserved with that interpretation superseded.

## K00163
Source `D0165`, original line1, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

# Active working state — 2026-10-11

## K00164
Source `D0165`, original line3, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

Goal: FramePicoTracker discovers PICO trackers, maintains game mode1, supplies real optical/IMU6DoF and body roles to SteamVR, and separately uses service mode0 for authorized custom-firmware updates. Full body is NOT achieved.

## K00165
Source `D0165`, original line5, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

## Verified now

## K00166
Source `D0165`, original line7, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

- Installed native UI has Game/Service choices and no Stop Radio button. API routing fix390 reads the current systemd owner's unique live stream, not the old root file.
- Actual app/API attempt391 detected first tracker [TRACKER_SERIAL] / chip[TRACKER_ID] / product firmware189, retained2998 replies over10149 scheduled slots. This is RF discovery, not reliable tracking or mode proof.
- App still showed Checking mode. IMU0/optical0/pose_valid=false. Native schema2 records report Frame/N0; their host nonce correlation was not persisted, so do not call them completed applied-mode proof.
- Attempt391 was stopped early through the internal API. Matching invocation e060a8280ded484297617760719a339d has all4 MCU restoration barriers=true, child exit0, flash_written=false. Source/image unchanged. The consumed Game catalog has been disabled; no radio is running.
- diagnostic10 and original210 remain the protected baseline. No new OTA, camera/native profile changes, or firmware flashing in390–392.

## K00167
Source `D0165`, original line13, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

## Current result and next blocker

## K00168
Source `D0165`, original line15, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

Actual393-v2 completed one150s RF-only app check after fresh direct human readiness. Invocation39bb0f5335bc4d7684bc164a7bc4e006:16 request-matched fresh nonce replies,15 distinct nonces confirmed mode1/N2/pending0 over140.104938491 seconds. App showed actual serial and Game mode; no mode-query errors in observed status. IMU0/optical0/pose false by this test's explicit scope. Child exit0 and all4 actual MCU restoration barriers=true. Radio inactive; consumed catalog disabled. Evidence outputs/phase393/actual-v2-summary.json, actual-v2-cleanup.json, actual-v2-attempt/.

## K00169
Source `D0165`, original line17, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

The ACK deadlock392 is now hardware-verified fixed in this bounded run. Do not rerun it to obtain the same result. The earlier393 launch failed before devices because watcher execution permission was missing; ProfileOwner and candidate-v2 now reject this at preflight, and actual read-only watcher invocation passed. No flash/OTA or protected image change.

## K00170
Source `D0165`, original line19, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

Next blocker is optics, not discovery/mode. Actual385 had25 stereo candidates but15 candidate-age and6 camera-age identity rejects; fixed verification window900..960 had no accepted lit result. Stereo acquisition accepts candidates aged up to1s while causal identity requires250ms. Do not relax identity,48-of61,pixel,clock,covariance or IMU guards. Investigate processing latency from retained evidence and reproduce a specific correction offline before preparing a changed optical candidate. Lower-threshold approximation is not native contour proof or optical identity.

## K00171
Source `D0165`, original line21, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

Offline follow-up in outputs/optical-latency-review: source simulation reproduces unnecessary geometry work on500ms pairs rejected by250ms identity; isolated opt-in strict candidate age gate avoids that work without widening guards, NOT deployed. Frame saved-image detector benchmark median2.6/3.6ms.20 exact simultaneous recorded centroid pairs: direct stereo bootstrap accepted0 (hypothesis capacity18, ambiguity2). Full geometry/fallback with correct carried measured seed in common HEAD reference: median18.5ms/max206.7ms,17 continuation/2 triangulated/1 rejected. CPU benchmark clock/head basis is explicitly simulated; no live freshness/world/identity claim. These measurements do not prove the whole live latency cause.

## K00172
Source `D0165`, original line23, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

Actual camera-only probe completed after a fresh direct answer:1618 exact packet-timestamp-matched decoded frames; no error; camera_close_returned=true and watcher_exited=true; noRoy/RF/OTA/IMU/LED. Upper-camera RAW-age median66.84ms, maximum85.13ms; decoder-to-consumer median1.45ms, maximum9.06ms. MONOTONIC comparison ages negative, domains are reported separately. Evidence outputs/optical-latency-review/camera-latency-result.json. This consumed probe must not repeat unchanged.

## K00173
Source `D0165`, original line25, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

Offline geometry optimization: ray memoization did not improve median and is OFF by default. Batched refinement keeps exact prior reprojection/covariance/conditioning/step/ambiguity/capacity checks. On100 exact recorded simultaneous centroid pairs using explicitly simulated benchmark clock/common HEAD basis, source baseline median19.417ms/max304.011ms vs vectorized8.214ms/max177.084ms; all labels,inliers,acceptances/rejections match; max numeric difference3.922e-12. No live pose/identity claim. Isolated source remains under outputs/optical-latency-review; not installed into active app. Evidence geometry-vectorized-parity.json, saved-full-path-baseline.json, saved-full-path-vectorized.json. Frame cProfile overhead changed timings; profiled timings are not baseline production latency.

## K00174
Source `D0165`, original line27, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

Prepared passive_stereo_probe.py: one30s passive actual stereo model-fit check, strict250ms age, live bracketing HMD poses, exact fresh active calibration, current-boot uniquely causal RAW-vs-MONO/BOOTTIME measurement, no radio/MCU/OTA/IMU/LED commands and no pose publication.118 sealed dependencies; remote owner/hash/executable preflight passed before devices. Independent human-readiness question is pending; do not run --run without its new direct answer. First target must be visible/stationary and others OFF. This cannot establish causal optical identity by itself; report only actual unassigned fits/freshness.

## K00175
Source `D0165`, original line29, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

Full body, real sensor delivery, causal optical identity and SteamVR pose publication remain unachieved. No automatic restart of393-v2/385 or consumed camera probe.
## Do not repeat / do not claim

## K00176
Source `D0165`, original line32, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

| Failure or misleading assumption | Established result | Boundary |
|---|---|---|
| Installed UI means tracking is ready | False; radio was disabled | Report UI, RF, mode, optical, IMU and SteamVR separately |
| API root radio-live.json is the new owner's stream | False; fixed390 | Bind active systemd invocation and per-owner output |
| Live replies permit infinite mode proof | False; finite389 lease retained392 | Refresh after10s, expire after30s; no continuity bypass |
| Final-N verification can gate every native16 ACK | Deadlocks grid preparation | Exact fresh-identity/native16/tag ACK only starts a new query |
| Timeout or product firmware189 proves stock firmware | False | Need explicit fresh capability/identity evidence for update offer |
| Only negative native10 correction calls760c | False; both signs outside +/-8 converge7a32 | ACK is not applied correction or physical pulse proof |
| Pixels below180 mean no photons | Unsupported | Actual385 native-contrast approximation140/1771 both-four, longest20; no identity/48-of61 proof |
| Repeat385 will produce 6DoF | No changed optical explanation | Consumed; no unchanged optical run or phase sweep |

## K00177
Source `D0165`, original line43, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

## Exact evidence pointers

## K00178
Source `D0165`, original line45, SHA256 `2ebe00a478bc306ab256f822593de97655d10065a45a2fdcbe2c1b728ef6db47`. Status: historical/unadjudicated narrative; English source prose.

- Actual391: `outputs/phase391/actual-attempt/`, `actual-cleanup.json`, `api-start-result.json`, `remote-verification.json`.
- Installed API390: `outputs/phase390/installed-status.json`; Frame backup `/home/steamos/pico-ram-work/backup-api-phase390`.
- Optical failure385 and timing correction: `docs/PHASE388_389_APP_STATUS_AND_MODE_LEASE_2026-10-11.md`.
- Original210 source: `outputs/phase210/roy_ram_station_v4.c`; actual ELF: `outputs/phase273/optical-pass/build/roy_station.elf`. Historical copied release C is not that executed module.
- Full historical AGENTS preserved in `docs/history/context-reset/AGENTS.before-compact-2026-10-11.md`. Search a specific question there; do not load it wholesale as current state.

## K00179
Source `D0166`, original line1, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Phase67 candidate ready, >=60% goal active, 2026-10-09

## K00180
Source `D0166`, original line3, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read strict quiet-window and corrected counter interpretation (original reference: `PHASE67_STRICT_QUIET_WINDOW_2026-10-09.md`).
43 offline checks pass, no hardware67 yet; nativecontrollers wake requested.
Final oldslots0 did not prove noRF; Grok diagnosis archived. Protections retained.

## K00181
Source `D0166`, original line7, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Timing61 repeat guard abort, 2026-10-09

## K00182
Source `D0166`, original line9, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read result (original reference: `TIMING61_REPEAT_RESULT_2026-10-09.md`).
Controller2 freeze162ms;0target polls,cleanup all true,no RF/mode proof.

## K00183
Source `D0166`, original line12, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Timing60 repeat aborted, 2026-10-09

## K00184
Source `D0166`, original line14, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read result (original reference: `TIMING60_REPEAT_RESULT_2026-10-09.md`).
Controller2 freeze162ms;0target polls,cleanup all true. RF result unavailable.

## K00185
Source `D0166`, original line17, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Timing59 requested repeat aborted, 2026-10-09

## K00186
Source `D0166`, original line19, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read result (original reference: `TIMING59_USER_REQUESTED_REPEAT_2026-10-09.md`).
Controller freeze163ms,0target polls; all cleanup true. No RF quality proof.

## K00187
Source `D0166`, original line22, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Timing58 aligned-grid repeat abort, 2026-10-09

## K00188
Source `D0166`, original line24, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read repeat evidence (original reference: `TIMING58_REPEAT_GUARD_ABORT_2026-10-09.md`).
Controller freeze162ms despite user motion;0target polls,cleanup all true.
Nominal timing alignment prepared, RF performance unverified; diagnose before repeat.

## K00189
Source `D0166`, original line28, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Timing57 grid aligned, hardware result pending, 2026-10-09

## K00190
Source `D0166`, original line30, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read timing decision (original reference: `TIMING57_COMMON_GRID_2026-10-09.md`).
5982*7=41874us stockN8/frameN13 validated; guard aborted controller freeze172ms
before target polls. All cleanup true, no OTA; no RF success claim.

## K00191
Source `D0166`, original line34, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Coexist56 native controllers tracked, PICO connection absent, 2026-10-09

## K00192
Source `D0166`, original line36, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read test result (original reference: `COEXIST56_CONTROLLER_TEST_2026-10-09.md`).
60s original coexistence module,2/1433 scheduled target answers,0IMU.
No fresh mode/identity; controller watchdog did not abort. Cleanup all true.

## K00193
Source `D0166`, original line40, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Mode53 remote switch/revision07 execution proven, 2026-10-08

## K00194
Source `D0166`, original line42, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read hardware proof (original reference: `MODE53_REMOTE_SWITCH_HARDWARE_2026-10-08.md`).
Freshfe02 actualmode1/rev7/N2 after stagedsetter; fullhost, no OTA.
Common8800us grid weak:17.68% CRC RX; proof later expired, final mode unknown.
All cleanup true, module unloaded. Last verified1, no100% claim.

## K00195
Source `D0166`, original line47, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Service52 RAM radio test, 2026-10-08

## K00196
Source `D0166`, original line49, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read measured reception (original reference: `SERVICE52_RADIO_TEST_2026-10-08.md`).
45s service38:98.6752% CRC RX/TX,98.4925% retained answers.3 ring drops.
No mode proof/OTA; module unloaded, cleanup all true.

## K00197
Source `D0166`, original line53, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Stock51 native Frame repeat, 2026-10-08

## K00198
Source `D0166`, original line55, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read repeat result (original reference: `STOCK51_NATIVE_FRAME_REPEAT_2026-10-08.md`).
3000 samples/30.38s, no connected tracker; no RAM module or mode/RF proof.

## K00199
Source `D0166`, original line58, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Stock50 longer native Frame repeat, 2026-10-08

## K00200
Source `D0166`, original line60, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read longer observation (original reference: `STOCK50_NATIVE_FRAME_LONGER_CHECK_2026-10-08.md`).
3000 OpenVR samples, no connected tracker; no RAM radio, raw RF loss unknown.

## K00201
Source `D0166`, original line63, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Stock49 user-woken native Frame repeat, 2026-10-08

## K00202
Source `D0166`, original line65, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read awake repeat (original reference: `STOCK49_AWAKE_NATIVE_FRAME_CHECK_2026-10-08.md`).
1000 native OpenVR samples/10.11s: no connected tracker; no RAM module loaded.
No RF denominator; mode/07 activation still unconfirmed.

## K00203
Source `D0166`, original line69, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Stock48 native Frame check, 2026-10-08

## K00204
Source `D0166`, original line71, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read native observation (original reference: `STOCK48_NATIVE_FRAME_CHECK_2026-10-08.md`).
No RAM module. No connected generic tracker in200 OpenVR samples; no PICO
RX/TX denominator exposed. Mode/07 activation remain unconfirmed.

## K00205
Source `D0166`, original line75, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## OTA46 revision07 TRANSFER SUCCESS, ACTIVATION PENDING, 2026-10-08

## K00206
Source `D0166`, original line77, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read physical result (original reference: `OTA46_REVISION07_TRANSFER_SUCCESS_2026-10-08.md`).
Post-button schema2 check responsive (98.16% CRC RX), no fresh revision7 proof;
activation/mode remain unconfirmed. Cleanup all true; no repeat OTA.
4077/4077 blocks, finish_verified, exit0; cleanup all true. No replay.
User restart requested; revision07-specific query proof pending.

## K00207
Source `D0166`, original line83, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## OTA46 phone OFF, PRESTART PAUSED, 2026-10-08

## K00208
Source `D0166`, original line85, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read repeat preparation (original reference: `OTA46_PHONE_OFF_PRESTART_2026-10-08.md`).
Fresh target matches, ota_start_sent=false, cleanup all true; new job staged.
Same exact07/service38, no code change or replay45. Final start confirmation pending.

## K00209
Source `D0166`, original line89, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## OTA45 revision07 FAILED at block2560, 2026-10-08

## K00210
Source `D0166`, original line91, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read hardware outcome (original reference: `OTA45_REVISION07_FAILED_2026-10-08.md`).
2559/4077 ACKed, block2560 ACK deadline expired; no finish/activation proof.
Cleanup all true, inactive/no loader. Tracker OTA state unknown.
No replay or automatic retry; preserve outputs/ota45/attempt.

## K00211
Source `D0166`, original line96, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## OTA45 revision07 PRESTART PAUSED, NO OTA, 2026-10-08

## K00212
Source `D0166`, original line98, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read pre-start preparation (original reference: `OTA45_REVISION07_PRESTART_2026-10-08.md`).
Fresh serial/chip verified; paused_before_ota_start,ota_start_sent=false;
SRAM/MPU/trace/session restored. Exact07 admission is CLI-bound risk exception,
not hardware approval. New unexecuted one-shot job staged, normal release unchanged.
Final human confirmation required before OTA;
Grok admission review (original reference: `OTA45_REVISION07_GROK_REVIEW_2026-10-08.md`) complete,
no concrete code blocker; not hardware approval.

## K00213
Source `D0166`, original line106, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Remote mode44: revision07 offline candidate, NO OTA, 2026-10-08

## K00214
Source `D0166`, original line108, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read independent audit entry (original reference: `FW189_REMOTE_MODE44_AUDIT_2026-10-08.md`).
Manual startup retained; schema2 addressed mode+N staging, fresh applied-mode/N
proof, opt-in automatic headset policy and common-grid preflight implemented.
Installed05 unchanged. Candidate97832B SHA40941b34...56d5; outputs/remote44/.
Grok4.7medium first review found TX/countdown defects; fixes implemented, delta
review confirms TX/countdown fixes. Lead also fixed false command19 flag gate;
final Grok opinion (original reference: `FW189_REMOTE_MODE44_GROK_FINAL_2026-10-08.md`) finds no further
demonstrable code blocker.245 final tests pass. No RF/new OTA/runtime deployment.
Hardware transition ordering, recovery and controller coexistence unverified.
Claude audit disposition (original reference: `FW189_REMOTE_MODE44_AUDIT_DISPOSITION_2026-10-08.md`):
A1 retained as known limitation (hidden staged transition survives disconnect);
A2 post-switch16 mandatory; A3/A4 hardware unknown. Audited07 image unchanged,
approved_for_ota=false. Query proves native pending only, not remote pending.

## K00215
Source `D0166`, original line122, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Mode enable43: revision06 counter fix prepared, NO OTA, 2026-10-08

## K00216
Source `D0166`, original line124, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read counter repair and limitations (original reference: `MODE_ENABLE43_COUNTER_REPAIR_2026-10-08.md`).
Installed05 counter not cleared on repeated c3ec entry; offline prior150 causes
next hold to exit151/mode0. Live counter/reason unknown. Candidate06 clears
counter at each entry; resetreason0-only gate preserved, not universal wake fix.
97328B SHA6d1424d7...f2a6;57 candidate tests incl full transfer/query/cadence pass.
Installed05 unchanged; no new OTA whitelist/package. User replacement selection
pending, exact-image hardware guard still05 only. Artifacts outputs/boot43/.

## K00217
Source `D0166`, original line132, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Mode41 stock confirmed; startup gate limitation, 2026-10-08

## K00218
Source `D0166`, original line134, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read fresh check and exact button condition (original reference: `MODE_CHECK41_BOOT_GATE_2026-10-08.md`).
mode0, protocol revision1 are distinct. Final05 selects Frame only resetreason0
and release count100..149; ordinary button wake is not guaranteed to meet it.
Live resetreason/timing unknown. Prior universal1.2s off/on advice incomplete.
45s no-OTA check cleaned all state; no new firmware changes.

## K00219
Source `D0166`, original line140, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration39 HARDWARE SUCCESS: OTA + fork response, 2026-10-08

## K00220
Source `D0166`, original line142, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read complete result and limits (original reference: `OTA_INDEXED_CAPTURE39_2026-10-08.md`).
[TRACKER_SERIAL]/chip[TRACKER_ID]:4056/4056 application block ACKs, positive
finish + final drain,382.96s (~6m23),9 retry intervals, unchanged fork05 SHA395ce84b...14f.
After user button action, two CRC-valid fe01 nonce/chip/mode0/revision1 replies
prove fork query execution; stock mode verified,2406 IMU samples. No full flash
SHA readback, recovery, Frame gesture/cadence/6DoF/coexistence proof.
Reliable OTA success is not100% packet RF; ordinary ring drops remain402/45s.
All three39 sessions cleaned SRAM/MPU/trace; no loader/OpenOCD or active units.
Do not replay successful job. Normal Frame module retained; no Frame flash.
Candidate38 code2036B tag0 no-retirement; host39 skips only indexed data drain.
Reports/artifacts37/38 preserved. Prior historical states below superseded.

## K00221
Source `D0166`, original line154, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration39 indexed capture ACK, 2026-10-08

## K00222
Source `D0166`, original line156, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read indexed progression (original reference: `OTA_INDEXED_CAPTURE39_2026-10-08.md`).
Physical38 confirmed549/4056 then empty-data-drain timeout, no finish; cleanup true.
Service38 sender now skips empty barrier ONLY after positive indexed data ACK;
prestart/start/final boundaries retained. Full4056 offline campaign passes.
New authorized physical39 prepared; exact target/fork/ELF unchanged.
Do not claim success until physical finish and fork handshake; preserve old reports.

## K00223
Source `D0166`, original line163, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration38 tag-zero fix, 2026-10-08

## K00224
Source `D0166`, original line165, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read hardware37 failure and candidate38 (original reference: `OTA_TAG_ZERO38_2026-10-08.md`).
OTA37 ACKed11/4056 then block12 timeout; actual success ACK carried tag0 and
was retired by MCU despite host rejecting it. Cleanup all true. No finish/boot proof.
Candidate38 refuses ACK retirement on tag0; full compiled/native4056 test passes.
Physical38 ACTIVE,136 blocks confirmed at latest saved check; outcome pending.
Do not interrupt/restart or infer success; monitor exact running session.
User explicitly authorizes fixing and proceeding. Exact target/image unchanged.
Grok37 review timeout630s, no findings. Preserve historical entries below.

## K00225
Source `D0166`, original line174, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration37 RESUMED, 2026-10-08

## K00226
Source `D0166`, original line176, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read host publication and absent target (original reference: `OTA_HOST_BATCH37_2026-10-08.md`).
User explicitly resumes and authorizes flash once corrected; target exclusive Frame.
This supersedes pause36 below, preserved as historical evidence.
38 host tests pass; no OTA37. No fresh identity in bounded ordinary smoke; cleanup all true.
Wake/LED clarification pending. Final36 smoke copied locally.

## K00227
Source `D0166`, original line182, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Hardware OTA attempt35: charger removed, still FAIL, 2026-10-08

## K00228
Source `D0166`, original line184, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read authorized unplugged comparison (original reference: `OTA_HARDWARE_ATTEMPT35_UNPLUGGED_2026-10-08.md`).
User confirmed cable removed/tracker on and authorized one new attempt. Same
reviewed code/profile/image as34; live exact identity/controllers-off verified.
Start ACK accepted, first block timed out10s:0/4056 acknowledged,53 total retries,
39 short polls, no finish. Charging removal alone did not restore OTA.
Receiver/worker/dedup/queue/staging unknown; no claimed installation/unchanged flash.
Cleanup ALL true; no-OTA20s health confirms same serial/FW189,807 IMU samples,
871 retained/890 TX. Custom radio inactive, no loader/OpenOCD. Preserve failed35;
no automatic further start/resume. Evidence:outputs/ota35/.

## K00229
Source `D0166`, original line194, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Hardware OTA attempt34: actual FAIL, 2026-10-08

## K00230
Source `D0166`, original line196, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read corrected-sender hardware outcome and health (original reference: `OTA_HARDWARE_ATTEMPT34_2026-10-08.md`).
User authorized start if visible after connecting charger; fresh exact identity and
controllers-off verified. Start ACK accepted, block1 still timed out10s,0/4056 ACKed,
41 total retry intervals,39 short polls, no finish. Short polls alone did not fix live
OTA. Exact outgoing block logged,342 short-tag RX include208 command22 records,
no data ACK. Receiver/worker/queue/staging contents unknown; no verified installation.
Roy cleanup ALL true, custom radio inactive. Separate no-OTA20s health check confirms
same serial/FW189 and IMU responsiveness, not fork/boot/recovery. Preserve failed34;
no automatic replay/retry. New diagnosis/plan and human instruction required.
Evidence:outputs/ota34/. Older33 pause was lifted only for this completed attempt.

## K00231
Source `D0166`, original line207, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## OTA response-budget iteration33: current PAUSE, 2026-10-08

## K00232
Source `D0166`, original line209, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read corrected sender, native reproducer and live stop (original reference: `OTA_RESPONSE_BUDGET33_2026-10-08.md`).
User requests all preparation/checks, stop immediately before actual OTA.
No new tracker OTA start/data/finish. Native33B request leaves32 reply bytes;
reliable command22 takes26 and hides8B block ACK. Reproduced offline, hardware
cause remains inferred. Service27 uses short S1=2 response polls, current-tag
phase change, same-sequence retries and no wrap. 70 tests pass; full97320B
native RX/scheduler/worker transfer4056 blocks exact. No hardware block transfer.
Live exact target paused_before_ota_start/start_sent=false; whole retained/TX
88.5008%, tail1000/1007 scheduled99.3049% (no tail TX-END snapshot).
Cleanup ALL true, custom radio inactive; host code installed/hash checked,
service27 ELF/fork05 unchanged. Data/drain10s; bounded overall7200s because
rare empty barriers can exceed30min. No claimed recovery/boot/CRC8 identity.
Evidence:outputs/ota33/. Preserve paused job; new start requires fresh job and
separate human instruction. Do not replay failed32 or paused33.

## K00233
Source `D0166`, original line224, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Hardware OTA attempt32 FAILED at block1, 2026-10-08

## K00234
Source `D0166`, original line226, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read actual OTA failure and live post-failure health (original reference: `OTA_HARDWARE_ATTEMPT32_2026-10-08.md`) before any retry.
User reported reboot and explicitly lifted pause/requested start; noSWD risk accepted.
Fresh exact iter05 fork job; live target[TRACKER_SERIAL]/chip[TRACKER_ID]/FW189 verified,
controllers off. Start ACK250101 observed; drain completed; data block1 timed out10s.
0/4056 acknowledged,40 retry intervals, no finish. SPI staging may be erased;
internal/staging contents unknown. Do not call firmware installed or flash unchanged.
496 records tagged20 while block1 pending contain telemetry, no data application ACK.
Cause unestablished; no automatic resume/reflash/reset/activation or timeout increase.
Roy session/SRAM/MPU/trace cleanup verified, custom radio inactive, no loader/OpenOCD.
Post-failure ordinary20s check: same live serial/FW189 and IMU responds;37 live samples.
Health does not prove fork identity, boot/recovery or staging contents.
Preserve failed result. Next destructive attempt needs a new reviewed plan and
explicit human instruction. Evidence:outputs/ota32/capture/,response-analysis.json.

## K00235
Source `D0166`, original line240, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Live OTA preflight31: PAUSED before real start, 2026-10-08

## K00236
Source `D0166`, original line242, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read live identity, authorization exception and actual pause (original reference: `OTA_LIVE_PREFLIGHT31_2026-10-08.md`).
User accepts noSWD/recovery risk, then explicitly requests pause after checks
and before real OTA. Latest pause supersedes earlier start. DO NOT auto-resume.
Live target confirmed:[TRACKER_SERIAL]/chip[TRACKER_ID]/FW189.
Actual result paused_before_ota_start,ota_start_sent=false; no sender constructed.
No OTA start/data/finish, tracker flash/reset/activation.65s validation cleaned up
Roy session/SRAM/MPU/trace; custom radio inactive, no loader/OpenOCD.
Earlier stable window99.791%; later paused-adapter whole runCRC/TX88.721%,
retained/actualTX88.281%; cause unknown. Do not call last run99% or ready block OTA.
Exact CLI-only risky-fork exception exists, but actual invocation uses explicit
pause-before-ota-start.63 pause/risk/runtime/loader tests pass; prior118 passed.
First import launch failed before hardware, dependencies repaired and checked
on Frame. Existing job/result cannot replay. Fresh human instruction required.
Evidence:outputs/ota31/final-capture/; firmware/recovery/clean-start remain unproven.

## K00237
Source `D0166`, original line257, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## OTA preparation30, 2026-10-08

## K00238
Source `D0166`, original line259, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read exact-fork service27 preparation and hardware stop (original reference: `OTA_PREPARATION30_2026-10-08.md`).
User requested preparation, stop before flashing a real tracker.
Offline package: outputs/ota-prepare30/package/, exact97320-byte iter05 fork,
service27 ELF, cycle1/N0/4727us/900us/margin100, measured slot0 identity.
load_job accepts the expanded image ONLY with preparation_only=True API;
normal CLI/hardware whitelist still rejects it. Hardware clearance unchanged.
plan_preflight validates exact service27 profile/module and controllers-off guards.
Result cannot overwrite image/job/registry/manifest.89 focused tests pass,
including full TrackerOtaRuntime record/status/mailbox -> native stock ARM worker
transfer:4056 blocks, one erase, exact staging, wrong-chip/lost-reply checks.
Saved runtime finish_verified is OFFLINE; native idle/boot/cadence remain false.
NoSSH/SWD/RF/OTA/device operations or deployment. Stop before first real OTA start.
Bootloader/recovery/clean-start/live identity evidence still required.

## K00239
Source `D0166`, original line273, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## OTA transfer audit29, 2026-10-08

## K00240
Source `D0166`, original line275, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read full-fork fault campaign and remaining installation blockers (original reference: `OTA_TRANSFER_AUDIT29_2026-10-08.md`).
Latest SWD plan is revision11; latest fork binary remains iteration05.
Offline native stock receiver transfers97320-byte fork exactly under0.018%,1%,10%
TX/RX omissions, burst loss, bad CRC/checksum, delayed/duplicate/old-tag replies.
One erase/4056 writes; stock whole-image CRC8 collision independently reproduced:
finish success is not SHA256 identity proof. Pending-worker fresh-tag race remains.
Sender rejects nonfinite deadlines; same-operation retries keep their tag;
ACK drain requires an exact empty reply.79 focused tests pass. Offline preflight lists
hardware adapter/image/cadence integration plus boot/recovery/clean-start blockers.
Nohardware/noOTA/no deployment, whitelist and hardware gate unchanged.
Evidence: outputs/ota-transfer29/. Read iteration07-11 before using older summaries.
