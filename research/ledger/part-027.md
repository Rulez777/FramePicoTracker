# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K02081
Source `D0333`, original line7, SHA256 `3af5bc2ba794130a61c64d8f4b0093f39d7fda15272ae827d854fdfef19cf1b8`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase70/attempt/stdout.log. Preserve this attempt and use a fresh output directory for the next run. Keep the two-controller tracking prerequisite and 150 ms guard.

## K02082
Source `D0334`, original line1, SHA256 `37f6feca48d1c12649b066a51ce1f7543814103b9d0aedd3638def66cf9446a2`. Status: historical/unadjudicated narrative; English source prose.

# Phase70: RX500 and explicit early-return quiet-width experiment

## K02083
Source `D0334`, original line3, SHA256 `37f6feca48d1c12649b066a51ce1f7543814103b9d0aedd3638def66cf9446a2`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. >=60% mode1 reception with healthy controllers remains active.
Read original captures and Grok deadline review (original reference: `COEXIST_RX_DEADLINE_GROK_2026-10-09.md`).
Mode53 schema2 END-observation times424--446us exceed the old400us capture gate.
These are compare-IRQ observation times, not precise RF arrival.169/236 mode53
working observations >=400us;214/236 <500us.2156 service52 observations all<500us.
Analysis outputs/phase68/deadline-analysis.json uses decoded command1/schema2,
not a substring search (initial substring counted accidental IMU bytes; corrected).

## K02084
Source `D0334`, original line11, SHA256 `37f6feca48d1c12649b066a51ce1f7543814103b9d0aedd3638def66cf9446a2`. Status: historical/unadjudicated narrative; English source prose.

Phase67 minute preserved healthy controllers but2empty replies/no product.
Phase68 fast-grid attempt rejected quiet-width700us before RF. No stablefast
reception evidence. No arbitrary interval sweep. Phase69 optional TX-counter
build exceeded2045-byte reservation; rejected, no SRAM expansion or deployment.

## K02085
Source `D0334`, original line16, SHA256 `37f6feca48d1c12649b066a51ce1f7543814103b9d0aedd3638def66cf9446a2`. Status: historical/unadjudicated narrative; English source prose.

Prepared isolated70 full host,6000us/cycle7,600us reservation,RXdeadline500us
(margin100),strictbusy.05,100us lead+100us post-return quiet padding. New optional
quiet-width calculation gives700us, restricted by loader to exactphase66 code
SHA87756a16...1ce0,6000/600/7,margin/lead/post100,two-controller150ms guard,no OTA.
Legacywidthcheck remains slot+200 unless explicit option selected. Early return
timing and actual MPSL effects remain hardware unknowns; watchdog retained.

## K02086
Source `D0334`, original line23, SHA256 `37f6feca48d1c12649b066a51ce1f7543814103b9d0aedd3638def66cf9446a2`. Status: historical/unadjudicated narrative; English source prose.

Full07 automatic desiredFrame policy,stockN8/frameN13/common42000us (boundary,
not an interior-margin claim) with fresh schema2 proof required. This additionally
tests the requested mode, not an assumption from old53. Source hashes frozen in
new job. Early product-info race in PeerEngine.start fixed: fresh own-session
product received whileABSENT is used, not cleared before ACK; stale cached product
does not confirm.94 combined tests pass6.42s,including early/stale product cases.

## K02087
Source `D0334`, original line30, SHA256 `37f6feca48d1c12649b066a51ce1f7543814103b9d0aedd3638def66cf9446a2`. Status: historical/unadjudicated narrative; English source prose.

Exact phase66 RAMELFe8815477...63ee retained. No tracker OTA/firmware changes,
persistent release unchanged. No blind MPSL first NORMAL request or sparse
confirm modification. Request user wake/nativehealthy controllers after preparation.
Job outputs/phase70 initially unexecuted. Do not replay previous results.

## K02088
Source `D0334`, original line35, SHA256 `37f6feca48d1c12649b066a51ce1f7543814103b9d0aedd3638def66cf9446a2`. Status: historical/unadjudicated narrative; English source prose.

Final combined suite after explicit RX400-vs500 ARM retention cases:101 passed
in6.50s,outputs/phase70/offline-tests.xml. All runtime hashes including ble_phase
sealed before test, global N override made8 to match explicit stock profile.
Latest native baseline still has controller2 disconnected; user wake question
pending. No70 radio test has occurred; >=60% goal is active, not complete.

## K02089
Source `D0334`, original line41, SHA256 `37f6feca48d1c12649b066a51ce1f7543814103b9d0aedd3638def66cf9446a2`. Status: historical/unadjudicated narrative; English source prose.

Automatic goal continuation revalidated the hardware blocker: fresh100-sample
baseline again shows controller1 connected/valid,controller2 disconnected/invalid.
Radio service inactive,no loader/OpenOCD,attempt/stdout.log absent on Frame.
Thus70 remains unexecuted; no live job to wait on. New baseline preserved in
outputs/phase70/baseline-next.jsonl. Previous goal turn made source/test progress;
this continuation rechecks the same required hardware condition, not60% proof.

## K02090
Source `D0335`, original line1, SHA256 `8b852dbb48f6366b02a69e2161a236b5e80a4085bfbb4ddd1ca265aeb5bd615d`. Status: historical/unadjudicated narrative; English source prose.

# Phase71 and phase72: quiet-window admission

## K02091
Source `D0335`, original line3, SHA256 `8b852dbb48f6366b02a69e2161a236b5e80a4085bfbb4ddd1ca265aeb5bd615d`. Status: historical/unadjudicated narrative; English source prose.

Both runs passed the initial two-controller native tracking prerequisite. Neither reached RF slot entry; reception and tracker mode remain unmeasured.

## K02092
Source `D0335`, original line5, SHA256 `8b852dbb48f6366b02a69e2161a236b5e80a4085bfbb4ddd1ca265aeb5bd615d`. Status: historical/unadjudicated narrative; English source prose.

Phase71 exposed a deployment mistake: the isolated phase70 loader was the older strict-window loader and required slot_us+200 = 800 us. The measured window was 700 us. The tested local explicit early-return admission code had not been copied into that release. Preserve phase70/71 as evidence of this mismatch; previous preparation assertions did not prove correct deployment.

## K02093
Source `D0335`, original line7, SHA256 `8b852dbb48f6366b02a69e2161a236b5e80a4085bfbb4ddd1ca265aeb5bd615d`. Status: historical/unadjudicated narrative; English source prose.

Phase72 uses a separate release with the correct loader, SHA256 92de41194469c601d23faf65dedd33ecc244731f184bfc7dfff8749e6418cc6d. Its code requires 100 us lead + 500 us RX deadline + 100 us post-return guard = 700 us. The fresh strict BLE sample instead yielded only 600 us, so admission correctly rejected it. The 150 ms controller guard and 5 percent per-bin maximum busy threshold remain unchanged. No OTA or persistent deployment occurred.

## K02094
Source `D0335`, original line9, SHA256 `8b852dbb48f6366b02a69e2161a236b5e80a4085bfbb4ddd1ca265aeb5bd615d`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase71/attempt and outputs/phase72/attempt, including frozen invocation, stdout, BLE profile and radio-run.json. MPU restoration is verified true. Session/RAM/trace restoration fields are absent because the rejection precedes session/code installation; they must not be reported as verified true. Wrapper exceptions caused by missing radio-live.json are secondary reporting errors.

## K02095
Source `D0335`, original line11, SHA256 `8b852dbb48f6366b02a69e2161a236b5e80a4085bfbb4ddd1ca265aeb5bd615d`. Status: historical/unadjudicated narrative; English source prose.

The next design step must account for measured 600 us strict quiet windows or establish a reproducible wider window. Repeating the same profile or relaxing the busy threshold is not evidence of safe coexistence. No >=60 percent result has been achieved.

## K02096
Source `D0336`, original line1, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

# Grok phase72 quiet-window review

## K02097
Source `D0336`, original line3, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

Cursor Grok4.7medium, read-only review. Recommendations and arithmetic below are not hardware approval. Later phase73 raw timing observations additionally expose a large asymmetric timestamp bracket; see PHASE73_BLE_SAMPLER_UNCERTAINTY_2026-10-09.md.

## K02098
Source `D0336`, original line5, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

+A finer-bin pass is a defensible read-only measurement. It is not a reason to admit the 500 µs deadline, and the N13/42 ms grid is a real schedule-edge problem that this measurement does not fix.

## K02099
Source `D0336`, original line7, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

## What phase72 actually showed

## K02100
Source `D0336`, original line9, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

The deployed loader matches the prepared gate. Code SHA `87756a16…1ce0`, ELF `e8815477…63ee`, required width 100+500+100 = 700 µs. The strict profile (`ble_max_busy_fraction` 0.05, 4043 samples, 30 bins of 100 µs) produced a 600 µs run, so admission rejected it before any slot. `outputs/phase72/attempt/radio-run.json` and `stdout.log` agree. Mode and reception were not measured.

## K02101
Source `D0336`, original line11, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

The 600 µs run is bins 26–29 and 0–1, all ≤ 0.020. It stops on the two neighbors:

## K02102
Source `D0336`, original line13, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

| Bin | Busy fraction | Role |
|---|---|---|
| 25 | 0.051 | one step over the cap |
| 2 | 0.060 | one step over the cap |

## K02103
Source `D0336`, original line18, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

Same rule on earlier strict captures: phase67 800 µs, phase68 700 µs, phase71 700 µs, phase72 600 µs. The sub-5% plateau already moves by 200 µs from run to run. The shoulders just outside it sit at about 0.05–0.09. That is a ramp cut by 100 µs bins, not a hidden quiet region.

## K02104
Source `D0336`, original line20, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

## Finer bins

## K02105
Source `D0336`, original line22, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

Yes, with the cap and the 150 ms guard left untouched. The only way a 700 µs region can exist inside this profile is if the inner part of bin 25 and the inner part of bin 2 are each ≤ 0.05. That would add about 50 µs per side and land exactly on 700 µs. It is a hypothesis. The saved profile cannot test it: `quiet_window` stores three-decimal bin rates and drops the ticks.

## K02106
Source `D0336`, original line24, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

Do not treat one fine histogram as an admission result. Run-to-run width already varies by more than one bin. A pass mixed into the same RF attempt would still be ambiguous, as the earlier deadline review said.

## K02107
Source `D0336`, original line26, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

At ~135 hits per 100 µs bin, 0.05 is about 7 hits. Bin 25 at 0.051 is roughly one hit over the cap. Split to one RTC tick (~30.5 µs) and that edge is a handful of counts. Finer bins can show a sharp step. They cannot certify 4.9% versus 5.1%.

## K02108
Source `D0336`, original line28, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

## Uncertainties that bound the measurement

## K02109
Source `D0336`, original line30, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

**Proof from the code and these captures**

## K02110
Source `D0336`, original line32, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

- The sample clock is RTC1 at 32768 Hz: one tick is 30.5 µs. `sample()` in `pc/radio/ram/ble_phase.py` keeps the integer midpoint and throws away the before/after counter pair, so the read aperture is not in the phase72 artifact.
- Busy means `RADIO STATE` at `0x550` is nonzero. That includes ramp-up and ramp-down, not only on-air time.
- The fold assumes exactly 3000.000 µs. The two humps stay sharp, so smear over one capture is smaller than the hump width. How much of the shoulder is smear versus real BLE activity is not separated.
- `callback_observation_us` is the 100 µs TIMER0 poll that sees `EVENTS_END`, and the handler drops the frame when `tick >= D(1892)` before `capture()`. The mode53 fe02 notices at 424–446 µs are late bounds. True END can be up to one poll earlier. The 400 µs gate still cannot record them, because the late IRQ takes the `slot_end` path.
- Phase72 sets `ble_phase_offset_us` to 0. A historical sampler bias of about +300 µs is recorded in `docs/EXECUTION_STATUS_2026-10-06.md` and is not applied. Width does not depend on that offset. Placement of the slot inside the window does.
- Early return is the callback writing action 3 at `D(1892)`. There is no MPSL header in this tree. The 2026-10-07 note in `roy_ram_station_v4.c` is the evidence that a later return was visible to the controllers. It does not prove the stack releases at 500 µs rather than holding the 600 µs request.
- HFCLK versus LFCLK over 500 µs is a fraction of a microsecond. It is not the margin that matters. The 100 µs IRQ grid, the 30.5 µs RTC tick, and the 60 µs phase deadband are.

## K02111
Source `D0336`, original line40, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

## Smallest change that is warranted

## K02112
Source `D0336`, original line42, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

Host only, in the sample loop that already runs before rejection. No C change, no ELF change, no change to `BINS`, `0.05`, `quiet_width_us`, or the 150 ms guard.

## K02113
Source `D0336`, original line44, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

1. Have `sample()` also return the RTC before/after span.
2. Write the raw `(tick, busy, span)` list beside the existing 30-bin profile, then keep the current 700 µs rejection.

## K02114
Source `D0336`, original line47, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

Offline, rebin at one RTC tick and plot phase against sample order so drift is visible. Take several captures. Change admission binning only if more than one of those captures shows a contiguous ≤ 0.05 run of at least 700 µs, with edge counts that are not a one-hit flip, and with an aperture much smaller than the recovered margin.

## K02115
Source `D0336`, original line49, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

If the inner halves stay over 0.05, or the recovered run is ~650 µs, stop. The 500 µs deadline does not fit a 600 µs strict window. Do not lower 0.05, the 100 µs lead, or the 100 µs post pad to force it in.

## K02116
Source `D0336`, original line51, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

## Mode1, N13, and 60%

## K02117
Source `D0336`, original line53, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

**Proof, from the host window check.** `validate_remote_mode_grid` uses Frame `[N×3000+2207, (N+1)×3000]` and Stock `[N×4727.5+3934.5, (N+1)×4727.5]`. For N13 and N8 the intersection is `[41754.5, 42000]` µs. The phase-lock rule requires the interval to be a multiple of 3000 µs. The only such value in that intersection is 42000 µs, which is the inclusive close of the Frame window. Timing57’s interior 5982×7 = 41874 µs (about 120 µs of margin) cannot be used while that lock rule stays. The 60 µs deadband of lateness leaves the model window; that part is arithmetic, not a measured miss.

## K02118
Source `D0336`, original line55, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

**Proof, from mode53.** On a different, interior grid (8800 µs, Frame N2), controllers off, deadline 800 µs: 283/1601 CRC = 17.68%, and the fe02 cluster was 424–446 µs. Six of those schema2 notices were inside 500 µs; 22 of 236 channel-2 notices were at or after 500 µs. A 500 µs gate does not turn that result into 60%, and the ratio does not transfer onto the 42000 µs grid.

## K02119
Source `D0336`, original line57, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

**Not proof that this grid cannot reach 60%.** Phase67, on 6000×7 with both controllers healthy and the 400 µs gate, kept two CRC-valid `010300` frames, both noticed at 266 µs, and no product record, across 10032 slots. The tracker answered something. Whether the missing long frames were past 400 µs, outside the Frame window, or lost to BLE is unseparated. Service52’s ~98% was the stock grid with controllers off.

## K02120
Source `D0336`, original line59, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

**Hypothesis.** After sleep, mode is unknown. Mode53’s mode1 proof expired at 30 s and was not re-queried. Revision 07 can still be present while the mode byte is 0. Nothing in the short `010300` frames carries mode.

## K02121
Source `D0336`, original line61, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

## Establishing mode1 without OTA or controller shutdown

## K02122
Source `D0336`, original line63, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

The proof path is already the phase72 policy: schema2 `fe02` for chip `[TRACKER_ID]`, revision 7, mode byte, pending 0. A setter ACK (`fd02`) is not applied mode. Mode53 is the template, and it was done with controllers off, so it does not carry over.

## K02123
Source `D0336`, original line65, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

That query cannot be run until a strict window admits the deadline that can see a 424–446 µs notice. With a true 600 µs window the largest deadline the current pads allow is 400 µs, which is the gate that drops those notices. Turning the controllers off would repeat mode53 and would violate the constraint you set. Leaving them on and relaxing 0.05 would no longer be the same safety case.

## K02124
Source `D0336`, original line67, SHA256 `7b0c00d57370b3a025806c113c38ebbb74b6c96d714d6aecb42367d49aaa29cc`. Status: historical/unadjudicated narrative; English source prose.

So the order is: save raw BLE samples and measure the shoulder; admit 500 µs only if a repeated tick-level run is actually ≥ 700 µs at ≤ 0.05; then let the existing schema2 exchange run under the 150 ms guard. The result that matters is a fresh `fe02` with `callback_observation_us` under the deadline while both controllers stay valid. If that reply says mode 0, the existing setter may stage mode 1, and only a later `fe02` with mode 1 and pending 0 establishes it. Neither outcome is a 60% measurement.

## K02125
Source `D0337`, original line1, SHA256 `0221d96d97018baa2681db2dafb7f66946c8727614891e68950e35bfdd8c087b`. Status: historical/unadjudicated narrative; English source prose.

# Phase73: stock BLE sampler timing uncertainty

## K02126
Source `D0337`, original line3, SHA256 `0221d96d97018baa2681db2dafb7f66946c8727614891e68950e35bfdd8c087b`. Status: historical/unadjudicated narrative; English source prose.

This was a bounded 20-second diagnostic using only OpenOCD register reads, without a RAM module, MPSL session or register writes. The native tracking watcher recorded 1,978 samples; both controllers were connected and valid in every sample.

## K02127
Source `D0337`, original line5, SHA256 `0221d96d97018baa2681db2dafb7f66946c8727614891e68950e35bfdd8c087b`. Status: historical/unadjudicated narrative; English source prose.

The existing four-read sampler brackets RADIO.STATE with RTC1.COUNTER reads, but also reads BASE0 between STATE and the final timestamp. The measured bracket span was: minimum 1,312 us, median 1,648 us, p90 2,319 us, p99 2,991 us, maximum 4,242 us. No sample had a bracket span <=3 LF ticks. Calling these reads back-to-back does not establish an instantaneous radio-state timestamp. Midpoint assignment is asymmetric and its bias/jitter is not bounded to the 100 us histogram bin size by these observations.

## K02128
Source `D0337`, original line7, SHA256 `0221d96d97018baa2681db2dafb7f66946c8727614891e68950e35bfdd8c087b`. Status: historical/unadjudicated narrative; English source prose.

The raw fold gives 700 us with 30 bins and 720 us with 50 bins, but the last half gives 600/660/700 us at 30/50/60 bins. These are offline histogram results, not proof of a physically safe quiet interval. Finer bins alone do not resolve timestamp uncertainty.

## K02129
Source `D0337`, original line9, SHA256 `0221d96d97018baa2681db2dafb7f66946c8727614891e68950e35bfdd8c087b`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase73/samples.json (7,848 observations with original before/after ticks), native-tracking.jsonl, openocd.log and analysis.json. Acquisition source: outputs/phase70/ble_diagnostic73.py; analysis: outputs/phase70/analyze73.py.

## K02130
Source `D0337`, original line11, SHA256 `0221d96d97018baa2681db2dafb7f66946c8727614891e68950e35bfdd8c087b`. Status: historical/unadjudicated narrative; English source prose.

Local changes retain future loader raw observations before admission and permit explicit offline histogram resolution while preserving the default 30 bins, 400 us physical minimum, sample coverage checks and 5 percent busy cap. 36 focused tests pass. These changes are not deployed into the normal Frame release and no new RF result is claimed.

## K02131
Source `D0337`, original line13, SHA256 `0221d96d97018baa2681db2dafb7f66946c8727614891e68950e35bfdd8c087b`. Status: historical/unadjudicated narrative; English source prose.

Next action: establish a bounded or calibrated timestamp for RADIO.STATE, rather than infer that a refined histogram can safely widen the RF reservation. The >=60 percent two-controller goal remains active and unachieved.

## K02132
Source `D0338`, original line1, SHA256 `108cc1a583a4141b12bf1efcbf77975ebacdefbf8e48df94217f29e226b53d65`. Status: historical/unadjudicated narrative; English source prose.

# Phase74 native BLE measurement candidate (offline only)

## K02133
Source `D0338`, original line3, SHA256 `108cc1a583a4141b12bf1efcbf77975ebacdefbf8e48df94217f29e226b53d65`. Status: historical/unadjudicated narrative; English source prose.

Problem: phase73 measured host-side RTC bracket spans of 1.3--4.2 ms. This prevents interpreting a 100 us folded histogram as a bounded RADIO.STATE timestamp. The candidate samples RTC-before, RADIO.STATE and RTC-after directly in a native CPU callback; BASE0 is read afterwards. Host transfer latency therefore lies outside the stored bracket.

## K02134
Source `D0338`, original line5, SHA256 `108cc1a583a4141b12bf1efcbf77975ebacdefbf8e48df94217f29e226b53d65`. Status: historical/unadjudicated narrative; English source prose.

Source: outputs/phase74/source/ble_native_sampler.c. Build: outputs/phase74/build/ble_native_sampler.elf. Entry 0x2001151d; code size 224 bytes. ELF SHA256 99ca7351b511cc083efbce85a9f62c66fd65c95a223ea66ec194b8d15fec4e4f; code SHA256 e5df028a8a2de3c5be0bd21091ece0f0964d9c61a413a7179d198657b6c7b3d6. The manifest retains source SHA256. Build uses the phase66 linker reservation and clang Cortex-M4 Thumb freestanding -Oz options.

## K02135
Source `D0338`, original line7, SHA256 `108cc1a583a4141b12bf1efcbf77975ebacdefbf8e48df94217f29e226b53d65`. Status: historical/unadjudicated narrative; English source prose.

The entry copies the validated phase66 native-shell MPU reservation and stock bypass reset call. Sampling itself restores the incoming interrupt mask before running, reads only RADIO.STATE/BASE0 and RTC1.COUNTER, writes only the existing manufacturing data reservation, and calls no MPSL or radio API. At most 64 observations are stored (within the first 1,280 bytes of the 2,048-byte reservation). An 8,192-iteration bound terminates even if RTC stops. This is an instruction bound, not yet a measured wall-clock duration under interrupt load.

## K02136
Source `D0338`, original line9, SHA256 `108cc1a583a4141b12bf1efcbf77975ebacdefbf8e48df94217f29e226b53d65`. Status: historical/unadjudicated narrative; English source prose.

Four exact-binary Unicorn Cortex-M execution cases pass: running/stopped RTC and incoming PRIMASK 0/1. They verify return, prior interrupt mask restoration, the stock bypass-reset call arguments, sample cap, clock wrap and allowed write addresses. The first emulator invocation used a non-M-class CPU configuration and rejected PRIMASK instructions; corrected test configuration uses UC_MODE_MCLASS. No firmware defect was inferred from that harness failure.

## K02137
Source `D0338`, original line11, SHA256 `108cc1a583a4141b12bf1efcbf77975ebacdefbf8e48df94217f29e226b53d65`. Status: historical/unadjudicated narrative; English source prose.

No native sampler was installed or called on the live Nordic. Before execution, review the callback ABI, MPU/checkpoint restoration, finite-duration behavior and monitor lifecycle. Preserve the two-controller 150 ms guard. A future sampler must retain per-record bracket span and reject ambiguous observations rather than hide interrupt delays. This diagnostic candidate does not establish radio reception, mode1 or >=60 percent coexistence.

## K02138
Source `D0339`, original line1, SHA256 `2b94904e3f950e26fc80626c03c39164aa896ebd0de2b5354dfd1279ee1b199f`. Status: historical/unadjudicated narrative; English source prose.

# Phase74 native diagnostic: pre-entry failure and verified baseline

## K02139
Source `D0339`, original line3, SHA256 `2b94904e3f950e26fc80626c03c39164aa896ebd0de2b5354dfd1279ee1b199f`. Status: historical/unadjudicated narrative; English source prose.

The first native sampler invocation timed out without completion magic or observations. Its runner correctly retained the checkpoint instead of asserting successful cleanup. This was not a tracker reception test and no OTA or MPSL session was requested.

## K02140
Source `D0339`, original line5, SHA256 `2b94904e3f950e26fc80626c03c39164aa896ebd0de2b5354dfd1279ee1b199f`. Status: historical/unadjudicated narrative; English source prose.

Root-cause evidence from source: the runner called invoke_native(..., reserved_mpu=True) but omitted disable_mpu=True. The established loader passes args.disable_mpu or args.reserve_mpu for the disable argument. Merely setting reserved_mpu prevents the alternate MPU mapping; it does not remove the execute-never protection before the first branch into manufacturing RAM. The candidate entry can only reserve executable CODE and reenable MPU after that branch. The first runner therefore omitted a required pre-entry step. The exact failing runner is preserved in outputs/phase74/attempt/executed-native-probe.py; corrected candidate uses both flags.

## K02141
Source `D0339`, original line7, SHA256 `2b94904e3f950e26fc80626c03c39164aa896ebd0de2b5354dfd1279ee1b199f`. Status: historical/unadjudicated narrative; English source prose.

Post-failure observations: shell hook zero, MPSL session inactive, manufacturing code/data zero, RTT counts3/3, static count2. A second read-only inspection compared all CODE2044 bytes, DATA2048 bytes, RTT counts and static word with recovery.json: all comparisons true. Thus the baseline is verified by readback; this does not establish the mechanism that returned it. Fault status at inspection was zero and both native controllers remained valid. MCU reboot/fault recovery is plausible but no captured reset reason establishes it.

## K02142
Source `D0339`, original line9, SHA256 `2b94904e3f950e26fc80626c03c39164aa896ebd0de2b5354dfd1279ee1b199f`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase74/attempt/report.json, recovery.json, executed-native-probe.py, post-failure-verification.json and original openocd.log. Original failure report remains unchanged. Corrected runner and four lifecycle fault cases are tested together with four exact-binary sampler cases:8 passed. The sampler ELF itself is unchanged. Any repeat must use a fresh output directory and verify restoration independently.

## K02143
Source `D0342`, original line1, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

# Phase79/80: schema2 and interior receive-window controls

## K02144
Source `D0342`, original line3, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

Hardware, 2026-10-09. Target [TRACKER_SERIAL], chip [TRACKER_ID]. Controllers OFF; no tracker flash or mode setter. User explicitly requested receive-window alignment with the tracker ON. Historical evidence consulted: Phase78 controls, Timing57, Mode53, remote-mode44 audit, and FW189 GRID3000 review.

## K02145
Source `D0342`, original line5, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

## Changes and results

## K02146
Source `D0342`, original line7, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

Phase79 combines native service blocked-slot retries with revision07-compatible schema2 queries. Its host files and ELF are sealed in invocation.json. The exact compiled service ELF is 8ab01f97ffb07953d755b5a97f1ccad5df4b253cb643facdc39466a48833b5fb (2028 code bytes). Offline validation: 106 passed, outputs/phase79/offline-tests.xml. The older Phase78 release used schema1 queries, which cannot provide revision07 schema2 proof. This is an additional handshake issue, separate from the measured short RX cutoff.

## K02147
Source `D0342`, original line9, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

Phase80 retains the exact Phase79 ELF and host hashes, changes only initial/steady slot interval from 6000 to 5982 us, and retains cycle7, slot900/RX deadline700 us, native retries and schema2. BLE phase locking stays OFF. No BLE admission gate is weakened: both experiments enforce controllers OFF.

## K02148
Source `D0342`, original line11, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

| Hardware control, 60 s | Nominal target period | Granted slots | Answered target slots / nominal opportunities | IMU samples |
|---|---:|---:|---:|---:|
| Phase79 | 42000 us | 6148 | 103 / 878 = 11.7312% | 3 |
| Phase80 | 41874 us | 6271 | 119 / 895 = 13.2961% | 6 |

## K02149
Source `D0342`, original line16, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

Both received fresh nonce/chip-bound mode replies proving mode1, revision7 during the run. Phase79 raw replies show actualN0 after reconnection and actualN13 after command16 preparation; native pending is0. This does not rule out the separately documented hidden remote pending flag. Both final snapshots explicitly have running=false; publish() therefore sets connected=false. This shutdown flag alone does not prove a live disconnect. Sparse retained IMU and long sample gaps still do not establish a sustained stream. Pose validity remains false. Mode proof during these experiments is not perpetual proof after the receiver stops.

## K02150
Source `D0342`, original line18, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

Denominator is scheduled opportunities computed from granted slots/cycle, not an independently counted TX_END denominator. These percentages are not directly comparable to Service52's TX_END-based 98.68%. A small difference between two single runs does not establish reproducible improvement.

## K02151
Source `D0342`, original line20, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

## Window arithmetic and limits

## K02152
Source `D0342`, original line22, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

The nominal conservative FrameN13 window is [41207,42000] us; StockN8 is [41754.5,42547.5] us. 41874 us has nominal opening/closing margins119.5/126 us in their intersection. Timing57 previously proposed this value but its controller guard aborted before target polling. Using it here is meaningful because the service build omits BLE rephasing, which invalidated the old coexistence interpretation.

## K02153
Source `D0342`, original line24, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

This experiment does not establish correct RF phase. MPSL grants can be delayed or missed. The original GRID3000 review also documents the native RTC conversion through division by30.5 and integer truncation; ideal microsecond windows are therefore insufficient evidence of actual programmed RTC boundaries. Enlarging the host RX cutoff does not make an asleep tracker hear the outbound poll. The next investigation must reconstruct actual native RTC boundaries and actual inter-poll timing, rather than treat 3000 us as an exact hardware tick period or sweep nominal periods blindly.

## K02154
Source `D0342`, original line26, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

## Evidence and cleanup

## K02155
Source `D0342`, original line28, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

Full records, frozen config/invocation, radio report, controller metrics, recovery and cleanup: outputs/phase79/attempt and outputs/phase80/attempt. Both wrappers exited0; session close, complete reserved RAM restore, MPU restore and trace-control restore are verified true. The temporary receiver is stopped. Historical attempts and the external-review handoff are preserved.

## K02156
Source `D0342`, original line30, SHA256 `7c7b0356affb14f632832de4536edb7e3a8608842479d878faff2d4d95cd3982`. Status: historical/unadjudicated narrative; English source prose.

The >=60% two-controller goal remains unachieved. No coexistence, 100% delivery, optical tracking, bootloader recovery or OTA readiness claim follows from these controls.

## K02157
Source `D0343`, original line1, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

# Dense Frame service: Phase81/82 hardware controls

## K02158
Source `D0343`, original line3, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. User authorizes adapting the successful Service52 receive profile to mode1. Target [TRACKER_SERIAL]/chip[TRACKER_ID], controllers OFF. No tracker OTA, mode setter, Frame flash or persistent receiver deployment.

## K02159
Source `D0343`, original line7, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

Both controls run60s with nominal3000us interval, cycle1, command16 N0/M2, slot900us and RX cutoff800us. BLE phase lock OFF; peers1/2 paused; optics OFF. Schema2 capability proof is required before configuration. The temporary diagnostic rejects a stock-mode reply by raising an exception and entering normal cleanup; it never switches the tracker mode.

## K02160
Source `D0343`, original line9, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

| Metric | Phase81, service79 module | Phase82, exact Service52 module |
|---|---:|---:|
| Granted slots |8125|5234|
| Completed TX_END |not independently counted|5236|
| CRC-valid RX |not separately exposed|4939|
| CRC RX / TX_END |unknown|94.3277%|
| Retained unique answered slots |1689|4445|
| Answered ratio |20.7877% per granted slot|84.8930% per TX_END|
| Retained IMU samples |975|2710|
| Host-ring dropped records |not reported here|493|
