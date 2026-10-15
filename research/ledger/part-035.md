# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K02721
Source `D0371`, original line85, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

The new TX counter exposes a denominator absent from Phase99/100: thousands of actual completions while sparse tracker replies occur around20Hz. Nominal41874/42000us does not imply only24 actual polls/s. EARLIEST retries and host retries are separate scheduling paths. A42000 control did not produce99%:12.52% versus14.18% across two individual runs is not a statistically established cadence improvement.

## K02722
Source `D0371`, original line87, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Native precision was successful: Phase102 zero-span fractions99.5942% and99.6324%; Phase10399.0558%. Only zero-span rows are admitted, so the RTC bracket did not cross a30.518us tick. This is a quantization bound, not exact microsecond RF timing. BeforeIRQ timestamps crossing a tick are flagged and excluded; flags do not retain exact nonzero span. Host transfer stays outside the stored bracket.

## K02723
Source `D0371`, original line89, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

After crossed and busy-own exclusions: Phase102 kept4581/4705 rows, busy59.72495%/62.23167%; Phase103 kept3933 rows, busy50.39410%. These scanner-active OFF observations cannot identify a live controller phase. Host native-call median224.102/221.222/224.168ms; maxima253.844/254.462/253.428ms. The callback itself collects32 distinct native ticks; this host transfer delay does not mean its IRQ mask lasts224ms. Phase103 all139 calls returned32 rows. At the measured OFF rate an8s acquisition would provide on the order of750 usable rows (inferred); actual controller-ON coverage and gate admission remain unknown.

## K02724
Source `D0371`, original line91, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

### Phase103: fixed retry reservation length

## K02725
Source `D0371`, original line93, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Source inspection found that EARLIEST used `s->slot_us`, the last granted reservation length. Immediately after the host switches900->400us, that field can still be900 until a new NORMAL400us grant occurs. This is a static stale-length path, not proof that every earlier freeze was caused by it. Phase103 request builders use the current desired `D(1876)` instead. A regression test starts with last slot900/desired400 and verifies the retry API receives400. The D1896 enable gate is restored. The unused native trap-only return markerD1600 is removed to fit; this candidate explicitly rejects trap-only mode. Existing omitted native-call countD1604 remains omitted.

## K02726
Source `D0371`, original line95, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Phase103 hardware: all139 sampler status observations reported last granted slot400us and configured interval42000us. This verifies the updated steady configuration, not a per-event controller overlap bound. Its13.8563% OFF CRC/TX does not establish a rate improvement over Phase10214.1806%; there was also one extra status read per sampler call.

## K02727
Source `D0371`, original line97, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Current source `outputs/phase103/source/roy_ram_station_v4.c`,2032 code bytes, entry`0x2001151d`:

## K02728
Source `D0371`, original line99, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

- ELF SHA256 `5972db0fb2748efee6c03b59dab0c96538ea20b2ad14be1b10cb85855136d267`.
- Code SHA256 `d8a667567564aa682588ce41c79986ef5e7647e7a1a65279eeb7c421daacb036`.
- Source SHA256 `a9e5a5849dc46e20d2462e15a119214ed7ead7d02ab2916b5761cc7e7d5e588b`.

## K02729
Source `D0371`, original line103, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Exact code hash is gated in the isolated loader.44 tests pass:12 exact-binary cases,29 staged host cases,2 transport cases,1 real script-import case. Recovery/session/state checks remain active; all four cleanup checks true in every hardware attempt. No lingering custom loader or debugger should remain; verify actual argv before next session.

## K02730
Source `D0371`, original line105, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

### Final prepared controller matrix

## K02731
Source `D0371`, original line107, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Use Phase103 runners ONLY; Phase101/102 joint preparations are superseded, preserved and unexecuted.

## K02732
Source `D0371`, original line109, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

| Phase103 directory | Sparse interval | RX end | Immediate MCU retry after dense switch |
|---|---:|---:|---|
|joint-a-native41874-rx340|41874us|340us|enabled|
|joint-b-grid42000-rx340|42000us|340us|enabled|
|joint-c-grid42000-rx330|42000us|330us|enabled|
|joint-d-grid42000-rx340-slow-retry|42000us|340us|disabled|

## K02733
Source `D0371`, original line116, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

D disables D1896 only after command16 ACK plus three later native RX advances commit the switch; before that it behaves as B. `--repeat-host` remains enabled, so D retains slower host recovery and does not promise absence of all EARLIEST requests. Its purpose is to isolate rapid MCU retries. Test shows no19 is queued at gate change. Missing replies or failure of strict admission still aborts. No variant may silently extend the12s grace or accept a narrowed/flat histogram.

## K02734
Source `D0371`, original line118, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Order for one return visit: A first to isolate precise measurement from Phase100; B tests nominal grid; C tests earlier RX; D tests immediate retries. Choose the next trial based on the observed failure, not automatic blind repetition. Controllers must be briefly OFF before each fresh S0 bootstrap, then ON at verified S1. Never power-cycle the tracker between these controls unless needed; prove its mode again if reset. Normal stream scoring starts only in S4 after strict admission and acknowledged useful traffic. Record time in S4, CRC/TX, unique IMU/gaps, each controller's invalid samples, freeze maxima, reconnects and cleanup.

## K02735
Source `D0371`, original line120, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

### Review status and evidence

## K02736
Source `D0371`, original line122, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7 medium's preliminary design review identified the exact SWD timing issue and required in-session sampling, separate TX completion denominator, atomic STATE/BASE and bounded loops. Final broad audit timed out at630s; focused audit timed out at270s. Neither returned a final candidate verdict; this is not a quota-limit claim. Do not label Phase103 independently approved. Primary-agent code review, exact-image tests and OFF controls are the present validation.

## K02737
Source `D0371`, original line124, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Final handoff evidence: Phase101 failed attempt; Phase102 two complete attempts and control-analysis.json; Phase103 complete attempt, control-analysis.json, exact source/build manifest, isolated release,44-test XML and four sealed unstarted joint invocation files. Native observer records contain actual packed words, counts, host call durations and Phase103 reservation status. Historical input logs and reports are preserved. A new inventory and ZIP accompany this document; do not modify the older Phase90 resume archive.

## K02738
Source `D0371`, original line126, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Target98–100% with two healthy controllers remains **unachieved**. The completed work is precise in-session measurement and a reproducible, guarded multi-hypothesis test kit ready for the user's return.

## K02739
Source `D0371`, original line128, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

## Additional IMU controls: Phase104–107

## K02740
Source `D0371`, original line130, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

These were performed before handing the project back, because sparse empty replies alone could hide a short-window streaming failure. Module remains the exact2032B Phase103 image; isolated host coordinators first bootstrap atN13, then ACK command16N0, observe three distinct RX advances, switch nominal poll to3000us, wait3s continuous replies, and queue19. No controller phase was claimed. Controller guard remained FORBID throughout. Thirty/31/32 host tests passed for the successive OFF-only coordinator additions; the long900us/800us window is permitted only in explicitly controller-OFF dense controls, never in the Phase103 joint kit.

## K02741
Source `D0371`, original line132, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

| Attempt | Reservation / RX | Total CRC / TX_END | Total CRC/TX | S4 seconds | S4 TX_END | S4 retained replies | S4 IMU | Total ring drops |
|---|---|---:|---:|---:|---:|---:|---:|---:|
|104 dense-rx340|400 /340us|996 /6694|14.8790%|30.892682|5040|314|199|204|
|104 dense-rx330|400 /330us|841 /6418|13.1038%|33.924909|5379|311|172|68|
|105 dense-rx800|900 /800us|1684 /6413|26.2592%|34.165470|5408|990|555|266|
|106 dense-rx800 +state query|900 /800us|1632 /6315|25.8432%|33.600837|5199|1007|556|218|

## K02742
Source `D0371`, original line139, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Each completed approximately45s with full cleanup. Totals include bootstrap and transition, so do not label total CRC/TX as a pure S4 metric. S4 retained replies exclude lost ring records and are not all CRC frames. S4 ring drops10/0/130/95 respectively; much of the remaining ring loss occurred in S3. Counter-derived host sample_gaps5763/5962/6298/6199 respectively are not a calibrated exact RF-loss rate: the modulo8-bit decoder ignores repeats and ambiguous deltas>=128. Do not turn these counters into a precise loss percentage without checking raw sequence history.

## K02743
Source `D0371`, original line141, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

The wide control improved these individual totals but did not reproduce historical Service52/Phase96/97 near99%. Thus neither shortening RX alone nor a nominal3000us clock is established as a correct setup. Cause of low completed-TX delivery remains unresolved; immediate retries/actual poll timing, tracker listening phase, scanner competition and differences from the older exact service module remain hypotheses. Historical successful service evidence is preserved and not invalidated by these different-profile controls. Reusing an older near99% number for this candidate would be incorrect.

## K02744
Source `D0371`, original line143, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

### Fresh applied N0 proof

## K02745
Source `D0371`, original line145, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Phase106 queued one raw read-only schema2 request in S4 at16.463521s, outside host mode-policy handling; no setter command was issued. Nonce `31727fc0f0ec5e2f87772c924ca3a0f9`. CRC/checksum-valid record748, slot2150:

## K02746
Source `D0371`, original line147, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

`fe0231727fc0f0ec5e2f87772c924ca3a0f93eed197901070000`

## K02747
Source `D0371`, original line149, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Parsed: chip[TRACKER_ID], mode1, revision7, actual N0, native countdown0. This confirms applied Frame/N0 in **that** run, not just transport ACK and not the current state after cleanup. It rules out an unapplied N0 switch as the sole explanation of that run's25.84% total. Raw probe, parser and result: outputs/phase106/analyze_probe.py and probe-analysis.json.

## K02748
Source `D0371`, original line151, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

### Reject the initial slow-retry variant before controller testing

## K02749
Source `D0371`, original line153, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Phase107,900/800us, disabled immediate MCU retry on the dense switch while leaving host retry enabled. It aborted in S3 at12.166077s, approximately3.071550s after dense switch at9.094527s: `MCU stage drop or rediscovery`. No19 or IMU, total157 CRC /807 TX=19.4548%,156 retained replies, ring drops0. All cleanup checks true. This is a short aborted attempt, not a comparable45s rate.

## K02750
Source `D0371`, original line155, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

**Do not run prepared Phase103 joint-d as a recommended controller experiment in its present form.** The analogous retry change failed without controllers; it may require deferral until useful streaming or a different recovery schedule. Such a redesign is untested. Preserve the prepared directory and failed107 evidence, mark D rejected/deferred. Recommended joint candidates are A/B/C; their OFF short-window baseline is weak, so they are diagnostic trials rather than99%-ready profiles.

## K02751
Source `D0371`, original line157, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

## Controller visit: one baseline plus three guarded hypotheses

## K02752
Source `D0371`, original line159, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Prepared **Phase108 native controller-only baseline** is a15-second copy of the historically measured60-second Phase89 observer, with the same244B Phase77 ELF and150ms/two-controller guard. It opens no tracker/MPSL session and transmits no custom tracker packets. Runner SHA256`e6d3d705a25c5315769e64f6232c31b7e6bbbeb6411384b3e82acd0f5b80c5ac`; only acquisition duration changed60->15s. It has not been executed. Use it FIRST while custom tracker radio is unloaded; never swap this standalone observer into a live tracker session.

## K02753
Source `D0371`, original line161, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Remote command after both controllers are valid:

## K02754
Source `D0371`, original line163, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

`venv/bin/python phase108/native_probe15.py --run --out /home/steamos/pico-ram-work/phase108/native-controller-baseline`

## K02755
Source `D0371`, original line165, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Then brief controllers-OFF bootstrap for A, user enables controllers at verified S1, holds both hands moving for the bounded180s test. B/C can be tried during the same visit, with controllers briefly OFF before each new bootstrap. Keep target ON, charging and unreset; obtain fresh mode proof each session. Do not run D unchanged. Never infer controller health from OFF metrics: invalid99/99 and growing freeze counters in OFF controls are the expected absence of controllers, not controller degradation evidence.

## K02756
Source `D0371`, original line167, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Controller trials can establish whether accurate phase observation/locked timing improves the scanner-heavy OFF results. They cannot be promised to reach98%. Record exact native coverage and admitted quiet width, actual completed TX and useful IMU continuity, independent controller health, transition impairment, dropouts and verified cleanup. If phase gate rejects, analyze saved rows rather than relax5%/540us or extend grace.

## K02757
Source `D0371`, original line169, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Final endpoint: every experiment unloaded/restored, no custom loader/debugger active; Frame/target left physically as supplied. Controller presence and precise latest tracker mode should be verified fresh when resuming. No controller-ON test, OTA or persistent firmware change occurred in this work session.

## K02758
Source `D0371`, original line171, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

## Phase108 controller-return baseline executed

## K02759
Source `D0371`, original line173, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

The user returned and confirmed both hands visible. The prepared 15-second native controller-only observer completed: 56 calls, 3584 raw rows, 3577 zero-span rows. No custom tracker RF session and no flash writes. All four restoration checks and post-cleanup controller validity were true. Evidence: outputs/phase108/native-controller-baseline. Joint A/B/C remain unexecuted at this checkpoint. The sealed pre-controller ZIP remains unchanged; this new evidence postdates it.

## K02760
Source `D0371`, original line176, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

## First joint A executed after user return

## K02761
Source `D0371`, original line178, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Phase103 joint-a-native41874-rx340 completed with a guarded S2 rejection: Radio samples do not cover the BLE period. Elapsed40.488421s; S1 at3.333419s, S2 at32.238198s. S2 retained190 replies /197 completedTX (96.4467%), IMU0; no phase admission, S3/S4 or command19. Total866CRC/4074TX=21.25675%;859retained; ring drops0. Frame mode freshly verified. A controller2 transitional window showed98/99 invalid and1002.814412ms freeze; final S2 metric window both healthy does not erase this transient impairment. All four cleanup flags true. Raw evidence outputs/phase103/joint-a-native41874-rx340/attempt. Next distinct prepared hypothesis B changes sparse cadence41874->42000us while retaining the same admission rules; no unchanged repeat and no relaxed coverage gate.

## K02762
Source `D0371`, original line181, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

## Joint B stopped on persistent controller loss

## K02763
Source `D0371`, original line183, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

B42000/RX340 reachedS1 at2.600331s but neverS2. User reported the second hand would not appear. Primary agent sentSIGINT at79.235698s; loader exit0 denotes clean interruption, not successful trial. Total1790CRC/5603TX=31.94717%;1752S1 replies;IMU0. LastS1 controller2 window99/99invalid, max unchanged pose79236.620584ms; controller1 healthy. All four restoration flags true. A six-second stock tracking capture after unloading showed both connected/valid/result200 in its final sample. This recovery supports RAM-radio interference but does not by itself prove causality or continuous recovery throughout the capture. Evidence outputs/phase103/joint-b-grid42000-rx340/attempt and outputs/phase108/post-b-stock-tracking.jsonl. Do not call B a successful joint test or run C blindly on the same failing pre-lock schedule.

## K02764
Source `D0371`, original line186, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

## Phase109 continuation
Read PHASE109_TRANSITION_AND_OBSERVER_CANDIDATE_2026-10-09.md for exact A/B failures, recovered stock baseline, isolated2044B stride3/connected-transition candidate,47offline tests and pending hardware validation. Old sealed ZIP is historical and is not overwritten.

## K02765
Source `D0371`, original line190, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

Phase109 hardware now executed: nativecoveragefixed (908accepted,min13/bin), quietrunonly300us -> gate rejection; no19/IMU; allcleanuptrue. Full numerical result in PHASE109_TRANSITION_AND_OBSERVER_CANDIDATE_2026-10-09.md. Targetstillunmet.

## K02766
Source `D0371`, original line193, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

## Phase110 stock comparison
See PHASE110_STOCK_AND_STRIDE_COMPARISON_2026-10-09.md. Fresh stock60 commonquiet650us in6segments;13665accepted, bothcontrollers0invalid/0freeze. Exact109observer stockonly15s also0regression,1516accepted. Allrestored. Wholeperiodcoverageunderfilled, nooldanchor reuse orrelaxedadmission. NextisolateactiveMPSLreservation effects fromtrackerRF withboundedcontinuous-linkdesign; no110trackerRF.

## K02767
Source `D0371`, original line197, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

## UART continuation
See FW189_UART_ENABLE_AND_REV08_CANDIDATE_2026-10-09.md. Native0x50enable replied5000 oninstalled07, noUSBenumerationchange, allRAMrestored, UARTdisable notsent. Offline08candidateexistsbutnotflashed/notOTAapproved. UARTphysicalroutingandnormaltrackingimpactremainunknown.

## K02768
Source `D0371`, original line200, SHA256 `00c29c46a90ac07ef57354374ba1a5f346080be4c3a821d7735d0addeef0c54c`. Status: historical/unadjudicated narrative; English source prose.

> Current continuation entrypoint: STATE_FOR_AI_CURRENT_PHASE113_2026-10-09.md (original reference: `STATE_FOR_AI_CURRENT_PHASE113_2026-10-09.md`). This document and its archive preserve earlier history; do not run its old next-visit instructions without checking the current handoff.

## K02769
Source `D0372`, original line1, SHA256 `d9ccd4ed9dcfd5e1ba6545b594247c8d2785193dc3c7d7fd107c78ce18bb861a`. Status: historical/unadjudicated narrative; English source prose.

# Stock48: native Frame check without RAM radio module

## K02770
Source `D0372`, original line3, SHA256 `d9ccd4ed9dcfd5e1ba6545b594247c8d2785193dc3c7d7fd107c78ce18bb861a`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. Read-only check after user explicitly requested stock Frame,
following RAM restoration (original reference: `NORDIC47_STOCK_RESTORE_2026-10-08.md`).
No RAM module, SWD/OpenOCD, pairing, firmware write or OTA was started.
Radio service inactive; no radio loader/OpenOCD process observed.

## K02771
Source `D0372`, original line8, SHA256 `d9ccd4ed9dcfd5e1ba6545b594247c8d2785193dc3c7d7fd107c78ce18bb861a`. Status: historical/unadjudicated narrative; English source prose.

Native OpenVR tracking_watch200 samples: only HMD and controller device classes;
no generic tracker connected, both controllers disconnected. The watcher reads
OpenVR device state, not RF counters. Raw evidence
`outputs/stock48/stock48-watch.jsonl`. Current XRService log search for exact
target serial/chip/PicoSwift or packet-loss/RX-counter terms yielded no matching
lines. Absence in these surfaces does not prove no RF energy or raw packets.

## K02772
Source `D0372`, original line15, SHA256 `d9ccd4ed9dcfd5e1ba6545b594247c8d2785193dc3c7d7fd107c78ce18bb861a`. Status: historical/unadjudicated narrative; English source prose.

There is no measured PICO RX/TX denominator on these native stock surfaces.
Therefore neither10%,100%, nor0% packet reception can be honestly reported.
The target is not exposed as a connected native OpenVR tracker in this sample.
Do not infer mode0/mode1 or revision07 activation from this absence.

## K02773
Source `D0372`, original line20, SHA256 `d9ccd4ed9dcfd5e1ba6545b594247c8d2785193dc3c7d7fd107c78ce18bb861a`. Status: historical/unadjudicated narrative; English source prose.

An initial attempt to get tracking_watch help invoked its stream because argv
is parsed as an integer count; stopped only that invocation with SIGINT.
The bounded200-sample run followed after reading its source. No radio was loaded.

## K02774
Source `D0372`, original line24, SHA256 `d9ccd4ed9dcfd5e1ba6545b594247c8d2785193dc3c7d7fd107c78ce18bb861a`. Status: historical/unadjudicated narrative; English source prose.

Automatic mode query/set remain commands of the custom PICO host protocol.
Removing that host means its mode query and RX counters are not available
through its inactive runtime. A real proof requires a compatible command path
or a validated native stock diagnostic interface; neither is shown by this
OpenVR observation. Successful OTA46 transfer remains distinct from activation.

## K02775
Source `D0373`, original line1, SHA256 `6268d2d0f5a03355e31b2334a148522557ee62346ecd96af46276fa574812382`. Status: historical/unadjudicated narrative; English source prose.

# Stock49: repeat native Frame observation after tracker wake

## K02776
Source `D0373`, original line3, SHA256 `6268d2d0f5a03355e31b2334a148522557ee62346ecd96af46276fa574812382`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User reported the tracker had slept during stock48, then confirmed
it is ON and requested a repeat. No RAM radio module, SWD, pairing or OTA.
Radio service inactive; no loader/OpenOCD process observed.

## K02777
Source `D0373`, original line7, SHA256 `6268d2d0f5a03355e31b2334a148522557ee62346ecd96af46276fa574812382`. Status: historical/unadjudicated narrative; English source prose.

Read-only OpenVR capture:1000 samples spanning10.11249s, zero connected generic
tracker samples, zero connected controller samples. The latest300 XRService
log lines contain no exact target serial[TRACKER_SERIAL]/chip[TRACKER_ID]/PicoSwift
matches. Raw files in `outputs/stock49/`.

## K02778
Source `D0373`, original line12, SHA256 `6268d2d0f5a03355e31b2334a148522557ee62346ecd96af46276fa574812382`. Status: historical/unadjudicated narrative; English source prose.

Target remains absent from observed native device enumeration despite the
user-reported wake. This is NOT a raw RF reception measurement. No native PICO
TX/RX denominator was obtained; do not report0%,10% or100% or infer mode/07
activation. Stock Frame remains running without the custom RAM radio module.
Previous stock48 observation remains preserved and was sleep-confounded.

## K02779
Source `D0374`, original line1, SHA256 `005c654a8b3afecdb7a13bb10ad38143f47c304399ef676ef1bad083328bad91`. Status: historical/unadjudicated narrative; English source prose.

# Stock50: longer native Frame observation

## K02780
Source `D0374`, original line3, SHA256 `005c654a8b3afecdb7a13bb10ad38143f47c304399ef676ef1bad083328bad91`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User explicitly requested another test after stock49. A longer
3000-sample native OpenVR window was selected; no RAM radio/SWD/pairing/OTA.
Radio service inactive and no radio loader/OpenOCD process observed.

## K02781
Source `D0374`, original line7, SHA256 `005c654a8b3afecdb7a13bb10ad38143f47c304399ef676ef1bad083328bad91`. Status: historical/unadjudicated narrative; English source prose.

No connected generic tracker appeared in the3000 samples. Latest300 XRService
lines contained no target serial[TRACKER_SERIAL]/chip[TRACKER_ID]/PicoSwift match.
Raw watch/log and exact duration/count summary: `outputs/stock50/`.

## K02782
Source `D0374`, original line11, SHA256 `005c654a8b3afecdb7a13bb10ad38143f47c304399ef676ef1bad083328bad91`. Status: historical/unadjudicated narrative; English source prose.

This repeats device-enumeration observation over a longer window, not a raw
Nordic RF test. It cannot establish a packet-loss percentage, active mode,
or revision07 execution. More unchanged enumeration tests will not supply
the missing packet counters or mode command path. Native stock state preserved.

## K02783
Source `D0375`, original line1, SHA256 `696dfcc2551a80361529174d614cf239f2b58369809a3aac188a4c468c0564e2`. Status: historical/unadjudicated narrative; English source prose.

# Stock51: user-requested native Frame repeat

## K02784
Source `D0375`, original line3, SHA256 `696dfcc2551a80361529174d614cf239f2b58369809a3aac188a4c468c0564e2`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User explicitly requested another unchanged test after stock50.
History checked in root AGENTS; no RAM radio, SWD, pairing or OTA started.
Radio service inactive, no radio loader/OpenOCD process observed.

## K02785
Source `D0375`, original line7, SHA256 `696dfcc2551a80361529174d614cf239f2b58369809a3aac188a4c468c0564e2`. Status: historical/unadjudicated narrative; English source prose.

3000 OpenVR samples over30.38402s: no connected generic tracker. Latest300
XRService log lines contain no target serial[TRACKER_SERIAL]/chip[TRACKER_ID]/
PicoSwift match. Raw watch/log and summary preserved in `outputs/stock51/`.

## K02786
Source `D0375`, original line11, SHA256 `696dfcc2551a80361529174d614cf239f2b58369809a3aac188a4c468c0564e2`. Status: historical/unadjudicated narrative; English source prose.

This is device visibility only, not raw RF packet reception. No denominator
for packet loss, no mode response and no revision07 activation proof obtained.
Native Frame remains without the custom RAM radio module. Earlier observations
are preserved; unchanged repeats cannot replace a protocol-level mode proof.

## K02787
Source `D0376`, original line1, SHA256 `63a3849c5ff7962b463d5249f7d6b77dfb280b3ffa46ec4444d1351e194288a5`. Status: historical/unadjudicated narrative; English source prose.

# Timing57: common Stock/Frame coexistence grid, hardware test interrupted

## K02788
Source `D0376`, original line3, SHA256 `63a3849c5ff7962b463d5249f7d6b77dfb280b3ffa46ec4444d1351e194288a5`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. User authorizes timing alignment. Read root AGENTS, native profile
selectors and mode44 audit window constraints, and coexist55/56 history.
No tracker firmware changes or OTA. Original coexistence RAM ELF retained;
full07 host isolated53 release, BLE lock and two-controller150ms guard retained.

## K02789
Source `D0376`, original line8, SHA256 `63a3849c5ff7962b463d5249f7d6b77dfb280b3ffa46ec4444d1351e194288a5`. Status: historical/unadjudicated narrative; English source prose.

## Calculation and configuration

## K02790
Source `D0376`, original line10, SHA256 `63a3849c5ff7962b463d5249f7d6b77dfb280b3ffa46ec4444d1351e194288a5`. Status: historical/unadjudicated narrative; English source prose.

Old6000us * cycle7 =42000us sits at the end of FrameN13 receive window
[41207,42000]us. StockN9 conservative window [46482,47275]us does not include
42000us; the old alternative-profile configuration was inconsistent.

## K02791
Source `D0376`, original line14, SHA256 `63a3849c5ff7962b463d5249f7d6b77dfb280b3ffa46ec4444d1351e194288a5`. Status: historical/unadjudicated narrative; English source prose.

New5982us * cycle7 =41874us. FrameN13 window [41207,42000]us; StockN8 window
[41754.5,42547.5]us. Intersection [41754.5,42000]us; new period gives119.5us
from opening and126us before closing. The audited validate_remote_mode_grid
accepts exact identities/stock8/frame13/period41874 before hardware entry.
This is a nominal conservative calculation, not actual arrival/clock proof.
MPSL grant delays and BLE phase correction can still violate the small margin.

## K02792
Source `D0376`, original line21, SHA256 `63a3849c5ff7962b463d5249f7d6b77dfb280b3ffa46ec4444d1351e194288a5`. Status: historical/unadjudicated narrative; English source prose.

Prepared fresh timing57 config: interval/steady5982us,cycle7,slot600us,
BLEphase lock, optics off, peers1/2 paused, desired Frame via schema2 fresh
proof, stock8/frame13. No service38 guard bypass; original coexistence ELF
8f784fc334c1e24a455513ff5565385b63a309427994b10d89ed20b7ceca44e4.
Normal release and persistent config unchanged. Artifact prepare57's copied
print message says no automatic setter; it is stale wording: actual config
tracker_desired_mode=frame enables audited automatic policy. No setter was sent
in this run because target polling never began.

## K02793
Source `D0376`, original line30, SHA256 `63a3849c5ff7962b463d5249f7d6b77dfb280b3ffa46ec4444d1351e194288a5`. Status: historical/unadjudicated narrative; English source prose.

## Hardware attempt and limits

## K02794
Source `D0376`, original line32, SHA256 `63a3849c5ff7962b463d5249f7d6b77dfb280b3ffa46ec4444d1351e194288a5`. Status: historical/unadjudicated narrative; English source prose.

Guard aborted on controller1 frozen172.32ms (limit150ms), controller2 freeze0.
Zero scheduled target polls and zero replies/IMU; no identity/mode proof.
Do not report this as new-grid RF failure or success, or blame the custom
radio for the freeze without native baseline comparison. Loader exit1.
All session/SRAM/MPU/trace cleanup true; module unloaded. No automatic retry.
Evidence outputs/timing57/attempt includes config/invocation/stdout/controller
metrics/radio report/recovery/cleanup. Previous experiments preserved.

## K02795
Source `D0376`, original line40, SHA256 `63a3849c5ff7962b463d5249f7d6b77dfb280b3ffa46ec4444d1351e194288a5`. Status: historical/unadjudicated narrative; English source prose.

Next bounded test needs user moving both tracked controllers and keeping the
target tracker awake. Guard remains enabled; no change to its150ms threshold.
No claim that mode1 gives100% RF or hardware coexistence until measured.

## K02796
Source `D0377`, original line1, SHA256 `4f0f3b553f6ab10da9d431b63d421b444de195938a0db07a237debdbc82afe17`. Status: historical/unadjudicated narrative; English source prose.

# Timing58: aligned grid repeat aborted by controller guard

## K02797
Source `D0377`, original line3, SHA256 `4f0f3b553f6ab10da9d431b63d421b444de195938a0db07a237debdbc82afe17`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. User confirms target ON and hands moving. Fresh repeat of
timing57 configuration (original reference: `TIMING57_COMMON_GRID_2026-10-09.md`), no code/OTA change.
5982us*7=41874us,stockN8/frameN13,original coexistence module,BLE lock,
two-controller150ms guard. Exact common-window calculation retained.

## K02798
Source `D0377`, original line8, SHA256 `4f0f3b553f6ab10da9d431b63d421b444de195938a0db07a237debdbc82afe17`. Status: historical/unadjudicated narrative; English source prose.

Guard aborted controller1 freeze162.36ms/controller2 81.48ms. Target polls,
replies and IMU all0; no identity or mode proof obtained. Final controller
metrics window includes invalid samples and freezes; preserve it rather than
blaming lack of user motion. Cause may be tracking/startup or test interaction;
not established by these observations. Two aligned-grid tests aborted before
target polling, whereas previous6000us coexist56 ran for60s. This comparison
does not isolate causality but argues against blindly repeating this setup.

## K02799
Source `D0377`, original line16, SHA256 `4f0f3b553f6ab10da9d431b63d421b444de195938a0db07a237debdbc82afe17`. Status: historical/unadjudicated narrative; English source prose.

All cleanup checks true,exit1,module unloaded. Evidence outputs/timing58/attempt.
No guard bypass or automatic retry. Timings aligned nominally but hardware
quality remains untested. Next work: compare native/controller baseline and
initialization timeline/BLE grant behavior, retaining150ms guard, before new RF.

## K02800
Source `D0378`, original line1, SHA256 `c37843d5a5e1d1a8e0b4cc524f2a2916a0dd34888d97a85b7b27d9530864e576`. Status: historical/unadjudicated narrative; English source prose.

# Timing59: explicitly requested unchanged repeat
