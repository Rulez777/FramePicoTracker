# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K04561
Source `D0164`, original line35, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This explains the main observation of runs with controllers without involving BLE and
phase. In all three runs, only empty 3-byte replies were caught, product info
is never present, IMU is 0, `connected=false`. Handshake deterministically drops
at the cutoff, so streaming is never reached. Short IMU replies
(around 335 µs) after connection would just fit into 400 µs.

## K04562
Source `D0164`, original line41, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The low reply percentage itself (around 0,1%) is not directly derived from this. But the cycle
"offer → tracker connected → host did not see product info → disconnect" plausibly
desynchronizes the tracker and station. This is a **hypothesis**, it has not been measured.

## K04563
Source `D0164`, original line45, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Defect in the module RX cycle (`outputs/phase66/source/roy_ram_station_v4.c`)

## K04564
Source `D0164`, original line54, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The last compare is set exactly to `end`. In this interrupt, `tick>=end` is always
true, so a frame with END in the interval (last 100-µs poll, `end`] is lost,
even though it finished BEFORE the deadline. With `SHORTS=19` (END→DISABLE), it already lies entirely in DMA,
with CRC. In current numbers, this is approximately the interval (335, 400] µs.

## K04565
Source `D0164`, original line59, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The fix does not extend RX and does not touch the ban on "+150 µs", after which
the controllers froze. It is sufficient on the `tick>=end` branch to first check
`EVENTS_END` and `CRCSTATUS`, capture the frame, and only then exit to `slot_end`.
This is a narrow version of Grok's hypothesis about the "late IRQ path", now backed by data.
This fix does not save replies with END after 400 µs.

## K04566
Source `D0164`, original line65, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## A control run is missing

## K04567
Source `D0164`, original line67, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Service52 (900 µs margin, controllers off) is compared to coexist
runs (slot 600 / deadline 400, controllers on). Two variables change between them
at once. A control is needed: exact coexist profile (6000×7, 600/400, same
ELF) **with controllers off**. It does not require OTA and does not risk controllers.

## K04568
Source `D0164`, original line72, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- If it is around 0% there as well and there is no product info, the cause lies in the profile and cutoff, not in BLE.
  Then work on phase and silence windows (phase73–76) does not answer the main question.
- If there is a high percentage, the cause is indeed contention for the air with BLE.

## K04569
Source `D0164`, original line76, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Recommended order

## K04570
Source `D0164`, original line78, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Offline: fix END check before exiting by deadline. Add ARM test: END
   at 380 µs with `end=400` must be captured, END at 420 — must not.
2. Control run of coexist profile with controllers off, first with
   the old ELF, then with the patched one. Count product info, `fe02`, IMU, and
   distribution of `callback_observation_us` by length.
3. Only after that return to the 500/700 µs window and BLE phase. Long replies
   (≥441 µs) will only fit with a deadline ≥ ~450–500 µs, while sporadic 531/588
   from mode53 — not even at 500.

## K04571
Source `D0164`, original line87, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Categories: timing table — **observation** (saved records). Frame loss in
(last poll, `end`] — **source deduction**, not measured on hardware.
Desynchronization due to repeated reconnections — **hypothesis**.

## K04572
Source `D0166`, original line539, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Execution plan and continuation point

## K04573
Source `D0166`, original line541, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Updated: 2026-10-07. Project: `K:\AI\pico`.

## K04574
Source `D0166`, original line543, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Full body tracking is not ready yet. The new station is installed on Frame (06.10 19:36), but has not been run. The new build has not shown a single real 6DoF tracker in SteamVR yet. Points 1–3 of the plan are completed, next point is 4 (hardware session).**

## K04575
Source `D0166`, original line545, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2026-10-07: iteration02, RC1 built for review; hardware PAUSE

## K04576
Source `D0166`, original line547, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- User: report every iteration, goal is firmware release today. Report docs/FW189_ITERATION_02_2026-10-07.md.
- RC1 snapshot outputs/fw189-grid3000-rc1-20261007/, ZIP alongside. 49 files with hashes; ZIP SHA256 7e504e1e1b5f565b9cbb1119f8bd1f5d04eefb6ec3e0d1a6d4382bd3f3557d2d. approved_for_ota=false.
- 145 tests passed: outputs/fw189-iteration02-20261007-tests.xml. Native queue/sequence filtering/dispatch, full96532 SPI transfer, and retry after lost ACK (single erase) executed offline.
- prepare_ack first clears ACK-only and requires exact empty reply. Pending old OTA work after an empty queue can still yield an old ACK; counterexample is now reproduced with native code. Fresh app startup clears necessary BSS; hardware boundary of fresh start/recovery is unknown.
- SRAM deadline source fixed: first compare clamp400, lateTX terminates slot, lateIRQ uses capture actualtime and does not record late packet. Before/after ARM callbacks verified with MMIO model. NewABI4 code2040/2045, SHA baebbf5621e01b245670776fd52ca0ea26352c52689afc770fca62288061cd08; ELF eb51829584d956252b37b977c65f664f9f6cac492978abb5ec67c3ad0e8ee210. Local compiler, ABI4 flag -fomit-frame-pointer; linker limit not increased.
- Firmware bytes are the same30/10; unconfirmed9922/10980/30.5 were not changed. Hardware/SSH/SWD/RF/OTA/services were not executed. The user's ongoing hardware prohibition is not overridden by today's release goal.

## K04577
Source `D0166`, original line554, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2026-10-07: offline refinement after review; hardware PAUSE

## K04578
Source `D0166`, original line556, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Report: docs/FW189_OFFLINE_REVISION_2026-10-07.md. 104 focused tests passed; JUnit outputs/fw189-offline-revision-20261007-tests.xml.
- Fixed OTA host/runtime: current/published generation checks across all operations, seed from peer, low16 wrap prohibition and check of minimum margin before start, repeated OTA ACK does not complete drain; prestart timeout/error/stop are preserved, identical status is not overwritten.
- ACK freshness is NOT fully closed: old application ACK issued by queue under fresh poll tag is still accepted. Reproducer and source hashes: outputs/audit_fw189_offline_revision.py, outputs/fw189-offline-revision-20261007-evidence.json.
- Actually executed TIMER2 SDK init (PRESCALER4) and both conversion branches for all N0..255. TIMER0 has tick1µs. OTA gate acts on0x62e4, but is absent in0x61f4 rearm; we draw no over-the-air conclusion from this.
- Application activation generates 24-byte metadata and system reset (NVMC mocked). Bootloader/recovery not proven.
- Additional replacements9922/10980/30.5 are not justified; new binary candidate not issued. Saved old image and exact reverse. Hardware, SSH, SWD, RF, OTA and services were not run.

## K04579
Source `D0166`, original line563, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2026-10-07: final Opus/Grok check; hardware PAUSE

## K04580
Source `D0166`, original line565, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Verdict: docs/FW189_GRID3000_FINAL_VERDICT_2026-10-07.md.
- Execution of init confirmed MODE=0, BITMODE=3 (+0x508), PRESCALER=4 (+0x510). TIMER0 has tick 1µs. Grok's latest statement about PRESCALER=3 is erroneous; 12000 corresponds to 12ms.
- RTC branch 0x641c executed offline: candidate N13 gives CC1338 after argument40810; physical time depends on RTC clock/prescaler. Evidence: outputs/tracker-grid3000-20261007/timer-units-countercheck.json; initial incorrect register signature explicitly corrected.
- Completeness of patch not confirmed. 9922->6467 not justified; 10980 requires branch analysis. OTA format confirmed only in model scope, waiting_target and ACK provenance remain open.
- Production sources, images and original reports were not changed; hardware/SSH/OTA were not executed.

## K04581
Source `D0166`, original line571, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2026-10-07: verification of independent Gemini report; hardware PAUSE

## K04582
Source `D0166`, original line573, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Per user request, `FW189_GRID3000_INDEPENDENT_REVIEW_REPORT.md` was reviewed.
  Analysis saved in `docs/FW189_GRID3000_REVIEW_COUNTERCHECK.md`;
  reproducer `outputs/check_gemini_fw189_review.py`, evidence JSON
  `outputs/tracker-grid3000-20261007/gemini-review-countercheck.json`.
- Byte-by-byte patch/reverse confirmed; 24 files match frozen hashes
  validation MD. Candidate and production sources were not changed.
- D1 confirmed offline: timeout prior to OTA leaves result waiting_target without
  cause even after publish(False); load_job rejects replay. FakeSWD writes0.
- Core OTA accepts start ACK with unrelated tx_generation and drain no-op with
  matching numeric tag4. No separate session epoch/freshness proof;
  live delayed-ring occurrence not reproduced. Gemini's verdict "sender is correct"
  is too strong; provenance must be resolved before hardware.
- Gemini B1: autonomousTX collision risk is valid, but "inevitability" and Valve
  interference are not proven by 12mod6 alone. Native ADDRESS origin, PPI TIMER0STOP,
  gates/queue/airtime are absent in simplified model. Emulation of N13 with f6a=1
  left CC0 unchanged. NOP CC0 store without full control-flow audit is not approved.
- B2: budget400 remains an issue; recorded 227..528µs are callback
  observation, not hardware END. Additionally, source-level gap: first RX compare
  after TX is set now+100 without clamp. With conditional TX end 350µs → compare 450µs.
  This is a source counterexample, not a new hardware result.
- B3: incorrect N on reconnect is a configuration risk, but "complete loss"
  from a single timer formula is not proven. Explicit per-ID map already exists, marker is not
  the only solution. D2 I/O stall risk is structurally real, specific latency
  and eMMC not tested. C1 activation/recovery unknown; lack of USB does not prove
  unrecoverability. OTA is not hardware approved, user pause remains.

## K04583
Source `D0166`, original line599, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2026-10-07: FW189 patch for 3000 µs grid and OTA prepared; PAUSE

## K04584
Source `D0166`, original line601, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Per user request, a standalone detailed artifact
`docs/FW189_GRID3000_INDEPENDENT_VALIDATION.md` was prepared for external independent review:
exact bytes/VA/SHA, timer origins and unproven phase assumptions, OTA layouts,
SPI/CRC/activation, counterexamples and assignments for five reviewer profiles;
sources and key stock ASM fragments embedded. Snapshot: 3761 lines,
184698 bytes. When packaging, repeated offline suite: 79 passed, 0 failures/errors/skips;
JUnit in `outputs/tracker-grid3000-20261007/independent-validation-tests.xml`.
Private registry IDs/seed/config/serial fixtures and firmware binaries are
not included in MD. Firmware/sources were not changed; hardware/RF/OTA pause remains.

## K04585
Source `D0166`, original line611, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- User clarified the task: small radio patch for FW189, without rewriting
  the firmware, preserving IMU/LED/camera/commands. This permission for tracker OTA
  replaces previous ban on modifying tracker flash; Roy flash remains stock.
  Then the user requested to stop after writing and wait for their signal.
- **Ready locally, not flashed to Frame and not run:**
  `outputs/tracker-grid3000-20261007/fw189-valve-grid-3000.bin`, stock alongside,
  `patch-manifest.json`, `ota-wire-example.json`. Image is 96532 bytes, modified
  **30 bytes in 10 places**; candidate SHA256
  `6a82a89206a609fdce92a3de764882ce53f79f0360b925bebe1ce7853cde6a0d`.
  Reverse application of manifest restores frozen stock byte-for-byte.
- TX in both branches: 4727→3000, 18910→12000 µs. RX double period:
  4727.5→3000; offsets 3538→1810.5, 3934.5→2207. Search integer
  offsets 3751→2024, 3355→1628 (round up by 0.5 µs).
  Window width and native RF delays preserved. **Clarification of old section 17:45:**
  skip branch calculates `(N+1)*period - 25`, not `+25`;
  this is confirmed by ARM code execution. Native compensation −25 µs preserved.
- `pc/radio/tracker_grid_patch.py`: hash/byte-gated patch/reverse without modifying
  the original and without forging signed vendor container.
  `tracker_ota.py`: command1/subcommand0x25, start/data/finish, CRC8/MAXIM,
  BE16 indices with 1,24-byte blocks. Application ACK is required; transport ACK
  does not advance transfer. ACK-only polls are separated from the next block so that
  repeated S1 bit0 does not delete the ACK of a block not yet read.
- `pc/radio/ram/tracker_ota_runtime.py` and opt-in `--tracker-ota-job` in loader:
  one pre-selected ID/slot from registry, other peers paused;
  optics off, HIGH600/6000 µs, phase lock, two controllers, freeze guard≤150 ms,
  RX ending400 µs. Limited timeframe, hash/manifest, log, no
  auto-resume after peer loss. MCU reply timeout 40 s for SPI erase/CRC;
  controller guard and host heartbeat continue working.
- `tracker_ota_prepare.py` creates separate one-off job/config, preserving
  original station-config. Source cadence is set explicitly: FW remains 189 and
  does not distinguish patch from stock. For original stock, N9 is used; for
  reverse OTA from 3000 µs, N13 with polling at 42 ms. Normal station supports
  `tracker_poll_decimation_by_id` so as not to change the rhythm of unpatched peers.
- **79 focused tests passed** (patch, OTA, runtime, existing peer/ABI4/guard).
  Unicorn executes real ARM TX/RX branches and subcommand dispatch→OTA handler;
  SPI/RF replaced with model. Verified full size 96532, boundaries 24/240,
  sentinel for multiple-of-24 size, full image CRC and rejection on corrupted
  staging byte. This is offline proof; not bootloader, over-the-air or hardware proof.
- Corrected significant Gemini conclusion: OTA application writes staging to
  **external SPI memory** (0xd130/d1f0, 10×24 bytes per 256-byte page;
  start erases 4×64 KB), then `0xf7d8` verifies CRC of entire image.
  Finish ACK confirms staging CRC. Flash metadata/activation occur
  separately via 0xfa98; bootloader and recovery after bad boot
  not yet verified. Presence of stock file does not guarantee OTA recovery
  after loss of executable application.
- **Further action only after user signal:** check current registry,
  Frame/Roy status, bootloader/activation/recovery; create job for single
  tracker, move agreed Python sources and image to separate directory,
  then OTA and measurement of real rhythm/phase/replies/controllers. Do not promise
  fixing RF issue based solely on nominal period match.
  Detailed commands and constraints: `TRACKER_OTA_RESUME_2026-10-07.md`.

## K04586
Source `D0166`, original line663, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2026-10-07 ~17:45: radio, tracker polling schedule (main blocker)

## K04587
Source `D0166`, original line665, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Test bench: headset hangs behind monitor, 3 trackers, 2 controllers. All 3 trackers connect and send IMU, controllers are clean. But poll responses are 3–30% for any N and periods (36/42/48/18/12 ms), frequent reconnects. Responses come in **deterministic bursts** (36 ms: 3 in a row and ~300 ms silence; 12 ms: bursts of 8, repetition 624 ms), meaning this is the tracker reception schedule, not over-the-air losses.
- Tested and does not work: 4727 µs grid with MPSL NORMAL (Roy yields not a single slot); 9 ms/N=1 with a slot every 3 ms (0 responses and **controller freeze for 484 ms**, do not do this).
- The model "reception window [N×4727.5+3538, (N+1)×4727.5] after our packet" is **incorrect**. According to FW189 code (Codex): after a packet, tracker closes reception in ~457.5−4L µs, sleeps for N×4727.5+3538 (S1 bit1=0) or +3934.5 (bit1=1), then opens reception and steps its scheduler (TIMER2/RTC1, step (N+1)×4727.5+25). Autonomous transmission via TIMER0 (4727 when N=0, **always 18910 when N≠0**) turns off reception. No link found to the frame timer of command 10. Numerically, no one has reproduced observations yet. Files: `outputs/cmd16-rx-window-gemini.md`, `outputs/tracker-rx-schedule-gemini.md` (speculative).
- PICO 2.96 station (from PICO 4 Enterprise, `outputs/pico4e/ctlbin/`, disassembler `outputs/pico4e/phoenixsta296-full.asm`): slot 945.5 µs, 5 slots per 4727.5 µs, tracker is polled every (N+1)-th rotation, command 16 = `[N, 8]`, N is set by host; disconnect after 100/(N+1) misses; command 10 on request once every ~50 ms capturing TIMER3.
- New in code: `controller_guard.min_controllers` (default 2); `live_source` deletes its camera recordings (SteamVR appends snapshots after stop, deletion repeats); loader takes `interval_us`/`steady_interval_us`/`mpsl_priority`/`tracker_poll_decimation_m` from `station-config.json`; `radio/ram/poll_window_sweep.py`. 295 tests. On Frame, initial config (42 ms, N=9), station is stopped, no camera recordings.
- Next step: exact tracker reception schedule (emulation of FW189 scheduler code or full disassembly of 0x6AB8..0x6F80), then polling tied to the last response of each tracker inside the controllers' quiet window.

## K04588
Source `D0166`, original line672, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2026-10-07 ~14:00: in-frame tracker recognition

## K04589
Source `D0166`, original line674, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Offline on 221 real frame (`outputs/assoc-eval`): several objects in the frame (group of 5 blobs ≈30 px — one side of tracker, 5 of 8 LEDs visible; separately another 2–3 blobs). Old search fitted the model to all blobs at once and required ≥6 → 3% success; relaxing thresholds did not help.
- Robust P3P search (Gemini, `ray_pose_solver.py`, support for 5 inliers; Codex attempt did not work and was shelved in `outputs/assoc-eval/`) by itself is also 3/80 and ~1 s per frame.
- **Solution:** blob groups (single-linkage 60 px, `blob_clusters`) and tracker search in each group ≥5, large ones first (`MeasuredPosePipeline._associate`, min_inliers 5, threshold 2.5 px). Offline: **63 out of 94 groups (67%)**, ~100 ms per group (3 models, desktop); distance ~0.32 m is stable. Models of the three trackers are nearly identical: which specific tracker — only by time-division of flashes.
- 292 tests. **Not uploaded** to Frame (turned off): `optical/measured_pipeline.py`, `optical/ray_pose_solver.py`.
- Next: upload, run with trackers on feet; inspect candidates with poses, temporal identity (dark frames), publishing poses to SteamVR.

## K04590
Source `D0166`, original line680, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2026-10-07 ~12:55: CAMERA SEES TRACKER LEDS

## K04591
Source `D0166`, original line682, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- With command 25 `[02]` and phase search with step 2000: at phase 264 000–274 000, cameras …039E and …05KV see 5–9 blobs per frame, at 276 000–280 000 almost 0. Matches firmware calculation (pulse at tick 268 727). Phase locked: 270 000 (`state/led-phase.json`, `locked`).
- 9 frames with tracker model (6 inliers, residual 1.1–1.3 px) from three different cameras yield one world point (−0.2, 1.4, −0.2) m: real object, first optical poses.
- Fixed along the way: false MCU session stop during prolonged transmission (10 min without stops, no loop freezes); candidate frames were discarded by age 150 ms (now 1 s for search, 250 ms for publication).
- Bottlenecks ahead: (1) association triggers on only ~9% of frames with ≥6 blobs (min_inliers 6, threshold 2 px) — verify offline on saved blob coordinates; (2) tracker identification (temporal identity from dark frames) with rare ACKs; (3) controller freeze for 464 ms at the end of the run.

## K04592
Source `D0166`, original line687, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Finding 2026-10-07 ~09:00: why LEDs were not visible

## K04593
Source `D0166`, original line689, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

FW189 analysis (Codex, `outputs/swift189-full.asm`):
- **Command 25:** LED gate `0xc5d4–0xc608`: mode 2 allows camera-synchronized pulses, **mode 1 suppresses them**, 0 — automatically when flags are zero. We were sending `[01]` → LEDs were not lighting up. Fixed to `[02]` (`peer_engine.configuration`).
- **Pulse:** GPIOTE ch3 on P0.16, TIMER1 CC0→PPI4→SET, CC1→PPI5→CLR; period 533333 ticks, pulse start **268727 ticks (16.795 ms)**, width 8–33 µs depending on class.
- **Command 10:** RADIO ADDRESS → TIMER1 CAPTURE[2]; the timer is aligned such that at the moment of ADDRESS it equals **V+467** (`0x79e0`); dead band ±8.
- **Command 14 `[1,N,r]`:** tracker frame is chosen based on command arrival time (counter is reset on the next pulse); reason does not set the index.
- Expected phase (tracker timer at exposure time): about 268 727 − 467 ≈ 268 260; the only frame with the tracker model was at 272 000. Search restarted from 256 000 with step 2000 (`led-phase.json`, `optics.phase_step_ticks=2000`).
- SteamVR driver `pico_tracker` loaded (vrserver 06:02: "no suitable devices" until poses appear).

## K04594
Source `D0166`, original line697, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## CONTINUATION POINT 2026-10-07 ~08:30: LED phase search

## K04595
Source `D0166`, original line699, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Configuration (`station-config.json`): 600/6000 µs, HIGH, phase alignment, `slot_cycle=7`, `tracker_poll_decimation_n=9`, `staggered_enroll=true`, `optics.timed_windows=false`, phase search step 16000 ticks (1 ms), window 12 frames, settle 6; MCU heartbeat headroom 1000 slots.
- **Coexistence is stable:** runs of 4–20 min with three active trackers without a single controller freeze.
- **Phase search:** progressed 0 → 400 000 out of 533 333 ticks (progress is saved in `state/led-phase.json`, currently 400 000, `searching`). No clear "lit" range found: number of blobs across cameras 0–5 barely depends on phase (background: controllers/reflections; camera …05KV stably 5 blobs). The only frame with tracker model: phase 272 000, camera …0488, 6 inliers, residual 1.26 px, `[TRACKER_ID]`, world (0.49, 1.54, 0.03), likely a false positive.
- **Issues:** (1) twice "MCU service heartbeat expired": host loop stalled > 1.2 s, then also > 6 s; need to find what blocks the loop (likely optics in the same process). (2) Trackers frequently reconnect (absent/confirming), search step ~30–40 s. (3) Not verified that tracker IR LEDs turn on at all: command 25 `[01]` (IR mode) and the "dark frame" rule; verify via firmware and on the test bench (camera + one tracker close, remove all other IR sources).
- Next step: finish scanning 400 000 → 533 333, then with fine step around 272 000; in parallel verify IR LED enablement and move optics to a separate process.

## K04596
Source `D0166`, original line705, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Morning 2026-10-07 (07:30–08:30): camera synchronization started

## K04597
Source `D0166`, original line707, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- **ACK attribution bug fixed:** synchronization plan was linked to the tag before `engine.mailbox()`, which yields a new tag when S1 changes; acknowledgments of commands 14/10 were being lost, trackers were reconnecting every ~25 s. Now the plan is linked to the published tag.
- **Command 16:** with N=8 (per the window formula), the tracker went silent after it took effect (cycle "setup → notification 22 → silence 3 s → reconnect" every ~4.5 s). Without command 16: acknowledgments 10/14 are coming in, two trackers reached `streaming`, but after ~30 s controller freeze occurred again (spontaneous tracker transmissions). Empirically N=7 and N=9: controllers are clean for 50 s; N=9 gives more sync acknowledgments (`tracker_poll_decimation_n=9`, FW189 window formula from analysis is not confirmed).
- **LED phase search started for the first time** (`searching`, phase 25600) at N=9.
- Remaining: freezes at startup (mass reconnect of trackers), frequent reconnects (every few seconds), phase search doesn't have time to complete a cycle. Plan: (1) sequential enrollment of trackers (`staggered_enroll`), (2) investigate reconnects, (3) bring phase search to first frames with inliers ≥ 6.

## K04598
Source `D0166`, original line712, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Morning 2026-10-07 (06:30–07:30): breakthrough in coexistence

## K04599
Source `D0166`, original line714, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- **Cause of controller freezes with active trackers found:** after command 19, the tracker, having received no poll for 4.7275 ms, transmits IMU on its own at random moments (outside our quiet window). When polled once every 42 ms, each tracker transmitted over the air ~9 times on top of controller BLE events. Verification: with trackers turned off (gone to standby), 90 s clean.
- **Fix:** command 16 `[N,2]` first in tracker configuration (`poll_decimation`, `set_poll_period`, `tracker_poll_decimation` in config). N=8 with polling once every 42 000 µs: tracker reception window opens after N×4727.5+3538 = 41 358 … 42 547.5 µs. Result: **2–2.5 min, all three trackers active, 0 controller freezes, 23.7% responses** (was ~10%).
- Station timeouts 3 s; command 10 without transmission window (`optics.timed_windows=false`; MCU recalculates phase tick on TX itself); phase search starts after acknowledgment from any tracker (`phase_require_all_peers=false`).
- **Roy clock was rejected** during SWD readings of 6–8 ms (point spread 1.5 ms > 1 ms): criterion replaced with offset estimation error (RMS/√n + best half-width). After this, synchronization plans are built and command 14 is sent.
- **Camera intervals:** all frames from the queue are now taken into account in order, not just the last one (previously there were no plans for the first ~70 s).
- The last run stopped because the headset was taken off (HMD result 101). Next step: a run of 3–5 min with the headset on the user and three trackers in the cameras' field of view; wait for acknowledgments of command 14/10 and LED phase search steps (`sync.phase_acks`, `phase_search.window`), then the first frames with inliers ≥ 6.

## K04600
Source `D0166`, original line721, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Morning 2026-10-07 (05:30–06:30), headset on user

## K04601
Source `D0166`, original line723, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Accelerated optics on Frame: `associate` with clutter 1862 → 93 ms (×20).
- Clean Roy after reboot: two sessions 600/100/200 of 90 s without freezes, responses 8.5–10.8%. Dense live picture of the air (12 000 measurements): our slot preempts events of both controllers in its period (in the neighboring period at the same phases events exist); phase sampler before start is offset by roughly +300 µs relative to MCU RTC. Added `live_radio_log_samples`, `ble_phase_offset_us`.
- Slot of 700 µs on clean Roy: controller loss in ~1 s. Limit of 600 µs confirmed.
- Station timeouts 3 s (`REPLY_TIMEOUT_S`, MCU `D(1612)=192e6`): all three trackers reached `waiting_camera_sync` simultaneously for the first time. But as soon as trackers are active, controllers freeze again (164–400 ms in ~20 s; with threshold 1 s: loss of both controllers 0.63 s and escalating freezes). LED phase search did not start, candidate frames 0.
- **Conclusion:** reliable coexistence of trackers and two controllers on the built-in Roy could not be achieved. Clean runs occurred with inactive or nearly inactive trackers; increased tracker activity (their responses and commands) breaks controllers. Hypotheses: long tracker transmissions extend beyond our slot, or interference in adjacent channels (2402 MHz near BLE 2404). Protocol changes do not help: the problem is in airtime of a single radio.
- Status: station stopped, freeze threshold 150 ms, optics enabled in config, timeouts 3 s.

## K04602
Source `D0166`, original line730, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Night 2026-10-07 (~02:30–03:30)

## K04603
Source `D0166`, original line732, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- **Tracker firmware FW189** (Codex + Gemini from disassembly listing `outputs/swift189-full.asm`, image `outputs/swift189-image.bin`, LOAD_BASE 0x1000, SHA `477000fc…`): work record dispatcher `0x94fc–0x958a`, handler table `0x20001868+cmd*16`; command 16 `[N,C]` = poll decimation (N periods of 4727.5 µs, C sample), `[00,00]` without decimation; 17 `[mode]` sensor mode (07 confirmed; 80 off, 81 low-power; 08 possibly slower, unverified); 11 recalculates frame selection from the ring (8×18 B, `0x20001afc`). Response puts 0–2 samples based on accumulation, aux 6 B alternates; **there is no "1 sample per response" command**. Response budget `min(54, 65 − poll length)` (`0x5a50`): long poll shortens the response, but occupies airtime itself. Response is prepared in software and triggered immediately after reception (ACK-like). Tracker reception window: shutdown after 457.5−4L µs, turn-on after N×4727.5+3538 or +3934.5 µs (according to S1 bit1).
- **Continued listening for response** (+150 µs once address is already captured): response rate 7% → 14.7%, but controller 2 froze (331 ms). Rolled back.
- **Safe mode 600/100/200 is unstable:** in one run 90 s clean, in another a freeze after 4 s. Live air log during session (`live_radio_log`) works, but host loop yields only 2–3 measurements per second: insufficient data. Need to speed up host loop or make a separate diagnostic mode with frequent measurements.
- **Optics accelerated** (Codex, verified locally): `ray_pose_solver.associate` with clutter 773 → 39 ms (×19.8), `solve_known` ×7, `refine` ×2, blob detector ×1.25; 275 tests (of which 44 are new for equivalence with reference). **Not uploaded to Frame:** Frame was turned off before upload. Upon next boot: upload `optical/ray_pose_solver.py`, `blob_detector.py`, `bench_optics.py`, `tests/test_optics_speed.py` and run `python -m optical.bench_optics` in `release-20261006b/pc`.
- ABI4 on Frame: 2028/2045 B (without continued listening). Station stopped; `station-config.json`: optics disabled (`optics_enabled=false`) after diagnostics, revert to `true` for full operation; `live_radio_log` enabled.

## K04604
Source `D0166`, original line738, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## CONTINUATION POINT (2026-10-07 ~02:15)

## K04605
Source `D0166`, original line740, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

User: built-in Roy only, no dongle; "do whatever it takes". Station stopped, safe configuration (600/6000 µs, offset 100, margin 200, HIGH, phase lock, `slot_cycle=7`, optics on). ABI4 2028/2045 B (slot end margin = `D(1892)`). 231 offline test.
Next tasks in order:
1. **Earlier transmission in slot** (`roy_ram_station_v4.c`, signal 0): first RADIO configuration and TXEN, then phase math with division and other things while ramp-up/transmission is in progress; copy mailbox to DMA in advance (at the end of the previous slot). Measure shift via `callback_observation_us` (tick 100 µs: for accuracy, temporarily poll END more frequently).
2. **Shorter tracker responses:** analyze command handlers in `pc/radio/Swift2.0_sv1.89_20241213_b189_app.bin` for commands 16 (poll interval/decimation), 17 (IMU mode: 07, 0x80/0x81), and 11 (frame reselection) using the tool `pc/radio/swift_fw_disasm.py` (capstone 5.0.7 is in .venv). Goal: 1 IMU sample per response or lower IMU frequency, so that the response ends before ~400 µs from the start of the slot.
3. Test on the testbench (headset on the table or on the user, controllers in field of view): response rate at 600/100/200 should increase from 7% to >50%, freezes 0.
4. If points 1–2 do not yield >50%: second quiet window, otherwise record that the built-in Roy cannot accommodate trackers together with two controllers.
5. Then: optics into a separate process (GIL), LED phase search, poses, SteamVR driver.

## K04606
Source `D0166`, original line748, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Slot mode sweep (~01:40–02:10)

## K04607
Source `D0166`, original line750, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Slot end margin is configurable (`slot_end_margin_us`, MCU `D(1892)` = RX boundary from slot start). Three trackers, cycle 7, optics disabled:
| slot/offset/margin, µs | responses to poll | controller freeze |
|---|---|---|
| 600/100/200 | 7% | 0 (40 s) |
| 650/0/150 | 15% | 205–297 ms |
| 700/0/150 | 17% | 386 ms |
| 700/0/120, 750/0/150, 650/50/120 | ~17% | freezes immediately |
Conclusion: only configuration 600/100/200 is safe, i.e. reception up to 400 µs from slot start, while tracker responses end at 227–528 µs (long responses with IMU later than 400). On the built-in Roy with two controller BLE connections with an interval of 3 ms, there is physically very little usable airtime for trackers. Remaining paths: (1) reduce the delay before the start of our transmission in the slot (currently CPU preparation of registers + ramp-up); (2) shorten tracker responses (commands 16/11/17: fewer IMU samples in response) via tracker firmware; (3) use the second quiet window (~100–500 µs of phase) for separate short polls; (4) find out if the Frame driver can increase the controller connection interval (not our firmware, probably not).

## K04608
Source `D0166`, original line759, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Stage 1–2, continuation (~01:00–01:40): tracker responses vs safe slot

## K04609
Source `D0166`, original line761, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Slot cycle length is configurable (`slot_cycle`, `DATA+1888`), individual peer pause (`diagnostic_pause_peers`), optics can be disabled (`optics_enabled`).
- Sweep of single tracker polling period k×6 ms (k=1..8, slot 600 µs): response rate is 3–10% for any k, no obvious tracker reception window is visible; in 3 out of 8 runs the controller froze for 153–183 ms (on the verge of threshold): the 600-µs slot in the found window is not always safe.
- **Key contradiction:** tracker responses arrive at 227–528 µs from slot start; reception ends 200 µs before the end of the slot, so a 600 µs slot cuts off late responses. A 800 µs slot froze the controller after 4 s; a 650 µs slot with a 80 µs margin at the end froze the controller after 0.25 s (radio must be turned off in advance before the next BLE event). Margins returned to 200/300 µs, slot 600 µs.
- Returned to verified configuration: 600/6000 µs, HIGH, phase lock, `slot_cycle=7`, optics enabled, `ble_phase_lead_us=100`. Station is not running.
- Next steps: (1) during the session in the host loop, read the RADIO state with a single TCL command to directly see the overlaps of our slots with BLE events and choose the window based on facts rather than pre-start estimation; (2) shorten our exchange: TX immediately at slot start without extra preparation, smaller polling payload so tracker response ends earlier than ~400 µs; (3) move optics to a separate process; (4) then LED phase search, poses, SteamVR driver.

## K04610
Source `D0166`, original line767, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Stage 1 RESOLVED: coexistence with controllers (2026-10-06, ~23:10–01:00, testbench)

## K04611
Source `D0166`, original line769, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**There were two causes, both resolved:**
1. **`MODECNF0`.** Normally `0x201` (fast ramp-up + DTX=Center), the Roy BLE stack writes it once at startup. Our module wrote `1` (DTX=B1), and the value remained after the slot until Roy reboot: BLE transmissions to controllers gradually dropped packets (freezes after 10–20 s). Now we write `0x201`. Important: part of the early "pauses" (without TX) was also distorted by this residual value.
2. **Slot phase.** Roy maintains two BLE-2M controller connections on a grid of exactly 3000.0 µs (RTC/LFCLK): A is approximately 500–1200 µs, B is 2200–3100 µs, quiet approximately 1300–2200 µs. `ble_phase.py`: before start, 10 s of measurements (RTC1 + RADIO state **in a single OpenOCD command**: separate queries smeared the phase), search for quiet window; MCU reads RTC1 at the start of each slot and itself sets the interval to the next slot so that it starts near the beginning of the window + 100 µs (`D(1884)=bit31|target tick`, `D(1880)=correction`; 3000 µs = 12288 units). Accuracy ±10–30 µs.
- MPSL rejects the next slot when the gap is < ~3 ms (3000/600 and 4700/2000 always BLOCKED); 6000/600 and 9000/600 without rejections.
- **Result:** 6000 µs, slot 600 µs, HIGH, with real transmission to trackers: 90 s, 13 000 slots, **0 controller freezes, 0 tracking losses**. ABI4 2020/2045 B.

## K04612
Source `D0166`, original line775, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Trackers (stage 2, open):** schedule of 7 slots (each tracker once every 42 ms, 42000 mod 4727.5 = 4180 µs in the estimated reception window; after offer the next slot is for this tracker). All three trackers connect, product-info is read, two reach `waiting_camera_sync`, but respond to only ~8% of polls (`packets` 299 over 60 s). Hypothesis: the tracker's reception window after command19 is tied to its own 4727.5-µs autonomous transmission cycle, and our 42-ms step drifts in phase. Needed: empirical sweep of single tracker polling period (k×6000 µs) recording response rate; analysis of command 16 (polling interval) in tracker firmware; accounting for autonomous transmissions.
**Optics:** with optics enabled, the host loop stalls due to GIL (candidate ~900 ms pure Python), the ring drops packets (222 vs 11 without optics). Need to move optics to a separate process (stage 3). Flag `optics_enabled` in station-config.

## K04613
Source `D0166`, original line778, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Stage 1: isolating the cause of controller loss (2026-10-06, 22:30–23:10, testbench: headset on user, controllers and trackers on table)

## K04614
Source `D0166`, original line780, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Baselines without RF (in hands and on table): pose in 100% of samples, freezes 0. Threshold `freeze_limit_ms=150` catches "lost hand".
| Run | Result |
|---|---|
| 1000/4727.5 µs, HIGH, trackers | controller 1 frozen 195/431 ms after ~10 s |
| 800/9455 µs (8.5% airtime), HIGH | controller 1 frozen 164/389 ms: airtime fraction is not the cause |
| Only XRService camera recording, without Roy | 0 freezes |
| Only SWD reads at station rate (3900 reads/25 s, `swd_read_probe.py`) | 0 freezes |
| Module loaded, HIGH slots, **radio does not turn on** (`diagnostic_pause_all`) | both controllers 373/425 ms: slot occupancy itself interferes |
| Same, NORMAL priority | **0 slots** (170 BLOCKED), controllers perfect |
| HIGH, pause, 9455 µs, without threshold | freezes in bursts 6–7 s with period ~19 s (beats) |
| HIGH, pause, 9405 µs, without threshold | freezes all 50 s continuously |

## K04615
Source `D0166`, original line792, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Conclusion: stock Roy firmware occupies radio almost continuously (NORMAL gets no slots) with a periodic structure of the order of 100 ms; a HIGH slot falling into the sensitive third of this period breaks controller connection for hundreds of ms. This does not depend on our transmission or airtime fraction, only on slot phase relative to the stock schedule.
Next steps of stage 1: (1) measure the exact period of stock activity (fine interval sweep, long runs without threshold on the testbench; or reading Roy RADIO/TIMER registers via SWD between our slots: frequency/address of stock reception show what it is: BLE scan, controller connections, or timesync); (2) hard-synchronize our period with this period (multiple) and keep phase in safe zone: MCU takes reference time of stock event (for example, from RADIO/TIMER registers upon slot entry) and adjusts distance of the next request; (3) alternative: find MPSL mechanism giving slots on NORMAL (shorter slot, different request type).

## K04616
Source `D0166`, original line795, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Plan execution progress (2026-10-06, 21:40–22:30, offline + read-only)

## K04617
Source `D0166`, original line797, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Stage 1, metric: `controller_guard.py` counts pose changes, maximum freeze, and loss episodes for each controller; log `state/controller-metrics.jsonl` once per second; `controller_baseline.py` captures baseline without RF. **Baseline (station off, 60 s, controllers in hands): pose changes in 100% of samples, freeze 0 ms, losses 0.** Threshold `coexistence.freeze_limit_ms=150` enabled in `station-config.json`: "lost hand" when valid=true now stops RF.
- Stage 2 (Codex, reviewed and refined): MCU writes ACK tag to `Peer.reply_s1` (bits 16–31) and valid response counter `DATA+1888+12*i`; empty working responses do not go into the ring; interval 4727 + 1 µs on odd slots (`DATA+1880`, average 4727.5 = stock station grid); host receives ACK from peer table, reads table and counters in a single SWD read (416 B), header 120 B and ring in a single read. Separate fast mode removed from MCU (size; plus it suppressed controllers). ABI4 1992/2045 B, code `4ec1cdb4…`, ELF `2c0c109e…`, installed in release.
- Stage 3: Gemini (run read-only) rewrote blob detector to cv2/scipy on its own, which are not present on Frame, skipped cameras by stream number, and cut RANSAC to 50 hypotheses without measurements. Reverted; timers and frame counters (`worker_timing` in status) kept for measurement on hardware.
- 228 offline tests. Station was not started.
- Next step: testbench (stage 0) and radio modes sweep with new metric (stage 1).

## K04618
Source `D0166`, original line803, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Further development plan (from 2026-10-06 21:40)

## K04619
Source `D0166`, original line805, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Based on the facts of sessions 19:39–20:55. Strict order: next stage begins when the previous one has met its readiness criterion. Short RF trials with user in headset are not done: hardware checks take place on the testbench (stage 0), user is needed only for final checks.

## K04620
Source `D0166`, original line807, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage 0. Testbench for checks without user in headset
- Frame lies on table, both controllers on stands in field of view of headset cameras, three trackers nearby. User permission is needed for autonomous RF runs on the testbench (each run is time-limited, watchdog is enabled, Roy recovers).
- Done when headset and controllers are stably `valid` for 10 minutes without a person (`tracking_watch` check).

## K04621
Source `D0166`, original line811, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage 1. Coexistence with controllers (main blocker)
Fact: at 21% airtime (1000/4728 µs, priority HIGH), right hand disappeared while watchdog stayed silent.
1. New watchdog metric: not just `valid`, but rate and freshness of controller pose updates and `eTrackingResult`, pose jumps/freezing; baseline is taken with station turned off.
2. Testbench sweep without trackers: slot 600/800/1000 µs × interval 4728/9455 µs × priority; for each, 3 minutes, controller metric against baseline.
3. Compress slot to actual need (responses arrive in 264–474 µs): target ≤700 µs.
- Done: a mode is found where controller metric does not differ from baseline for 10 minutes.

## K04622
Source `D0166`, original line818, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage 2. Reliable tracker response
Fact: the same tracker either streams ~50 responses/s or responds to 1–10% of polls.
1. MCU: fractional interval 4727.5 µs (alternation of 4727/4728) matching the stock station grid.
2. MCU writes tag of last response to peer table; only non-empty responses into ring; host reads header and ring in a single read (SWD 6–7 ms per operation, host loop currently 100–300 ms).
3. Counters in MCU: polls, responses, responses with S1=0 for each peer; clarify meaning of S1=0.
4. Offline replay of recorded sessions via PeerEngine (regression tests on real packets).
- Done: on testbench, three trackers for 10 minutes in `streaming`, ≥90% of polls with response, OFF/ON of tracker brings it back without station restart.

## K04623
Source `D0166`, original line826, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage 3. Real-time optics
Fact: in 30 s, 1 tracker frame was processed out of 24 needed for phase search.
1. Profile live_source/decode/blob/solver on Frame; decode only needed IR streams, only own-frames of active tracker, resolution reduction for blob search.
2. Hardware decoder or restriction to 2 cameras during phase search.
- Done: ≥25 processed frames/s; LED phase search locks phase on testbench; temporal identity confirms all three trackers.

## K04624
Source `D0166`, original line832, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage 4. Poses and IMU
1. Measured world poses of three trackers on testbench (stationary: stability ≤5 mm; ruler movement: meters correct).
2. Motion IMU calibration (axes/bias/offset), enabling inertial prediction.
- Done: poses stable during head movement, IMU prediction bounded and does not drift.

## K04625
Source `D0166`, original line837, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage 5. SteamVR and game (with user)
1. Restart SteamVR with new driver, three GenericTrackers with real poses.
2. Session of 10 minutes in headset: controllers, trackers, consumer game (VRChat FBT).
3. Operation: autostart, Frame reboot, tracker OFF/ON, UI closure.

## K04626
Source `D0166`, original line842, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Orchestra
- Codex: protocol/MCU logic, packet replay, review. Gemini: profiling and optics optimization. Cursor (gpt/grok, without -fast): small isolated modules and tests. Claude: integration, verification of each change, testbench runs.

## K04627
Source `D0166`, original line845, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Update 2026-10-06, 20:00–20:55: second and third hardware sessions

## K04628
Source `D0166`, original line847, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Achieved for the first time on onboard Roy: station runs stably for minutes (up to 17 000 slots), Valve controllers remain tracking; product-info (serial numbers) of all three trackers; tracker `[TRACKER_ID]` passed 17/25/19, command14/10, and **streamed IMU ~50 responses/s for two minutes** (interval 4728 µs, slot 1000 µs, MPSL priority 0).
Fixed along the way: fixed Roy clock skew 15.625 ns/cycle (fitting skew from SWD reads of 6–7 ms yielded >0.02% and crashed the station); clock exceptions in scheduling/confirmation skip the cycle instead of crashing; controller watchdog waits up to 5 s at start, loss threshold 0.5 s, writes device name in error; loss of XRService recording restarts camera in background, radio does not crash; auto-measurement of camera clock after reboot (Frame rebooted, boot `3b366fc9…`) discarding first second; MCU ACK tag, reset on `search`.
**Open (main):** trackers respond unstably: after connection/configuration they go silent or respond to 1–10% of polls; the same tracker streams in one run, but is silent in another. Hypotheses: tracker reception is tied to 4727.5 µs grid (ours is 4728, drift ~106 µs/s), after frequent station restarts tracker remains in old connection state, or S1=0 in part of responses means loss of sync. Optics: LED phase search makes virtually no progress (tracker frames in window 0–1 over 30 s), camera processing on Frame is too slow. 6DoF in SteamVR not achieved.

## K04629
Source `D0166`, original line851, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Update 2026-10-06, late evening: first hardware session and fixes from it

## K04630
Source `D0166`, original line853, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Session with the user in the headset (19:39–19:48). Found and fixed:
1. **Unit:** `--mpsl-priority 1` = MPSL NORMAL → 0 slots (known since 191/192). Now 0 (HIGH).
2. **Config:** the path to camera clock evidence pointed to the wrong place. Now at startup the station measures the camera clock domain itself if there is no file for the current boot (RAW must be the only causal domain, otherwise rejection); the "after reboot" blocker from item 6 has been removed in code.
3. **Dataset:** `device_config` appears in a new recording with a delay; waiting up to 10 s.
4. **Slots:** at 2000/4700 µs every NORMAL request for the next slot was blocked (slots only by repeated host call, ~3/s). At 2000/7000 slots started flowing, two trackers responded, but after ~8 s the controllers lost tracking and the watchdog stopped the station (Roy restored, controllers returned). Tracker responses arrive within 264–474 µs from the start of the slot, so the slot was shortened to 1000 µs, handshake 6000 µs, steady 6000 µs (was 700/1576 = 44% airtime, now ~17%). ABI4 interval range expanded to 7600 µs (3×7.6 ms < 23 ms tracker timeout).
5. **Main protocol bug:** all tracker responses arrive with `S1=3` regardless of our S1, and the engine counted ACK only when S1 matched → ACK never, product-info was repeated without confirmation, host duplicated command9 (seq0 already sent MCU), restart cycle 0.8 s. Now: ACK = response in slot with our tag; S1 bit0 is calculated on each publication; product-info in CONFIRMING = command9 completed (seq 9→1); confirmation deadline 0.8 s; any change to mailbox content gets a new tag (ABA); MCU reconnection is determined by `search` counter, not by tag 0; MCU transitions to stage 3 only upon response on channel 2.
ABI4 after fixes: 1964 B, code `1dd26a35…`, ELF `422d53ff…`, installed in release. 223 offline tests. **Next step:** repeated session with the user to verify specifically these fixes (coexistence with controllers at 17% airtime, confirmation and configuration of trackers).

## K04631
Source `D0166`, original line861, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Update 2026-10-06, evening (Claude, orchestra: Codex + Gemini)

## K04632
Source `D0166`, original line863, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Items 1 and 2 of the plan completed offline, item 3 built but not installed. RF/SWD were not started, SteamVR was not restarted.

## K04633
Source `D0166`, original line865, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Integration audit (item 1).** Codex verified the radio part, Gemini the optics. Confirmed and fixed defects:
- `roy_ram_station_v4.c`: mailbox timing words (window, phase reference) were read after checking seqlock; now a snapshot is inside seqlock. Response timeouts were counted in slots and upon transitioning to fast interval 1576 µs were reduced from 0.94 s to ~0.3 s; now in DWT cycles (host writes 60160000 to DATA+1612, stage2 1472000 = 23 ms).
- `peer_engine.py`: retiming published the same generation, ACK of old transmission was attributed to new plan. Now `retime()` gives a new tag, ACK of any attempt completes the command, `acked_generation` indicates the accepted attempt; generations with zero lower 16 bits are excluded.
- `station_optics.py`: plans by `(slot, gen16)`; schedule caches by physical slot (previously by compact position, broke with active slots [0,2]); `plan['chip']` instead of `active_chips[remainder]`; when changing device count, queue 10/14 and lights are reset; worker catches any exception, poll sees dead thread; close() closes resources independently.
- `exposure_schedule.py`: frame index was counted from session start by dividing by averaged period (drift); now incrementally with anchors, with locking between threads.
- `temporal_identity.py`: static box 2.5 cm rejected any body movement; now constant-velocity fitting (residual ≤2 cm, velocity ≤1.5 m/s), jumps of another constellation are rejected.
- `station_v4_runtime.py`: unified `_reset_peer` for all reconnection paths (previously product-info path did not reset IMU counter/clock); upon enrollment empty mailbox and registry preservation before unpausing.
- `roy_ram_loader.py`: cleanup of each resource in its own finally; OpenOCD terminate→kill.
- False audit assertion: "firmware hangs for 70 s on expired window" (C sends empty poll).

## K04634
Source `D0166`, original line875, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Inertial path (item 2).** `pc/optical/imu_calibration.py` (Codex): command2 decoder, `GyroAxisCalibrator` (body_from_sensor axes, gyro bias, time offset ±25 ms based on optical rotations, accelerometer gravity/scale check; verified only upon excitation along ≥2 axes, residual ≤0.01 rad, ≥2 stationary windows), `SyncTimeResolver` (time by tracker sync-timer, only as evidence). `station_optics.py` feeds real decoded samples: SampleClock (lower envelope, method from document) → queue → optical thread → calibrator (solution in background every 15 s) → `MeasuredPosePipeline.imu` only after applied verified calibration. `measured_pipeline.py`: bias/offset, atomic propagation with rollback (previously covariance rejection left rotated orientation), IMU buffer replay after late optical anchor.

## K04635
Source `D0166`, original line877, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**New: LED phase search (`pc/optical/phase_search.py`).** `phase_at_exposure_ticks` was not measured anywhere. Now `"search"`: step of 6400 ticks (0.4 ms), step is counted after ACK of command10 of all active peers and 12 settle frames; window of 24 own frames, "lit" = LED model fit ≥6 inliers; range capture and locking at center, writing to `state/led-phase.json`; loss of glow or 900 frames without temporal identity resume search.

## K04636
Source `D0166`, original line879, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Tests.** 220 offline tests pass (was 177): +12 integration ABI4 (`test_integration_abi4.py`, Codex), +5 phase search, +IMU calibration, updated temporal identity test.

## K04637
Source `D0166`, original line881, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Build (item 3).** On Frame in `/home/steamos/pico-ram-work/release-20261006b`: ABI4 1956/2045 bytes, code SHA256 `686794f8880e09893cd40b248e0f3c3b7b261db5876eba137bc9a4238fb25e43`, ELF `30eedf64ac2a4fb55ac33b02d723a62c1b035e91d54a2445d631890b332a8a32`; UI rebuilt from new main.cpp; driver and tracking_watch without source changes (taken from integration-20261006). compileall on Frame ok. **Installation not performed**: writing to Frame requires user permission. Script: `pc/frame_app/install_release.sh` (backup, radio unit is not enabled and not started).

## K04638
Source `D0166`, original line883, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Installed on 2026-10-06 19:36 upon user permission** by `install_release.sh` script: `release` → `release-20261006b/pc`, ELF `30eedf64…`, UI `4f4d826d…`, driver `507baa58…`, API restarted and active, radio unit ABI4 disabled/inactive, OpenOCD none. Backup `backup-before-20261006b`. Running UI process and loaded SteamVR driver remain old until restarted.

## K04639
Source `D0166`, original line885, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Remains.** Item 4: one 10-minute session with the headset on and two controllers. Constraints: camera clock evidence is tied to current boot (`a7b75755…`), after reboot the service does not start until a new clock audit; IMU calibration must be conducted by moving trackers along all axes and two pauses; command sequence 10/14 and phase search are not hardware-verified.

## K04640
Source `D0166`, original line887, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Where the work stopped (before the update above)
