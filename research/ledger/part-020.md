# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K01521
Source `D0295`, original line8, SHA256 `7a5eb331cf3a1b45998683c1dc3a9e2ed34dc586472326820c8224c3160f9259`. Status: historical/unadjudicated narrative; English source prose.

Runner/clock/reference/model feed integration remains unfinished; no hardware session has started. The originalPhaseSearch16000tick(1ms) steps are coarser than measured37us exposure and8..33us LED pulses, and cannot be presented as exhaustive narrow-pulse coverage. Choose trial spacing from measured overlap width and clock error (roughly16..32us steps may be needed); derive the bounded span around native268260 estimate from clock/model uncertainty, not an arbitrary full-period sweep. Whole-session deadline can expire before every phase if ACK/camera/health gates stall; do not guarantee success or fullcoverage in one run. Fix the range/limits once before hardware; no automatic subsequent launches. CorrectedPhase2102043B module and Phase211 actual-SRAM capability gate remain required. pose_valid=false.

## K01522
Source `D0296`, original line1, SHA256 `7c002b33163662ef413aba3bb590cee67fd53e3e3e145d23cddcf130c4a0cce1`. Status: historical/unadjudicated narrative; English source prose.

# Phase213: controllers-off optical preparation and live calibration (2026-10-10)

## K01523
Source `D0296`, original line3, SHA256 `7c002b33163662ef413aba3bb590cee67fd53e3e3e145d23cddcf130c4a0cce1`. Status: historical/unadjudicated narrative; English source prose.

User explicitly instructed that controllers are not required for tracker optical training and are intentionally OFF. Do NOT ask to turn them on for this bench stage. Current prerequisite is valid HMD, exact single target identity, bounded radio return/deadlines and recoverable RAM. Controller coexistence/full-body acceptance is separate. Pending previous wake question is superseded. Fresh tracking_watch showed HMDconnected/valid, both controllersdisconnected; no radio was started.

## K01524
Source `D0296`, original line5, SHA256 `7c002b33163662ef413aba3bb590cee67fd53e3e3e145d23cddcf130c4a0cce1`. Status: historical/unadjudicated narrative; English source prose.

A single PURPOSEFUL live camera calibration acquisition completed. `outputs/phase213/result.json` records160fresh decoded frames across4IRstreams and a successfully preserved active calibration snapshot. Owned recording stopped and datasetcleanup completed. No RF/SRAMloading/OTA; pose_valid=false. Capture was for missing active calibration and controllers-free baseline, not a phase-sweep attempt.

## K01525
Source `D0296`, original line7, SHA256 `7c002b33163662ef413aba3bb590cee67fd53e3e3e145d23cddcf130c4a0cce1`. Status: historical/unadjudicated narrative; English source prose.

Preserved `outputs/phase213/calibration/sensorRigCalibration.json`:22314B,SHA7e2d78302252e76743e295f83e3e20813ff3895ec14cd112fbf12cfce4ccfe54; unique active deviceconfig7067B,SHA47571d1d4c330f885911752182508b72b6178263da0e66bff30e7df7661aba58. Manifest records source Deckard dataset; snapshots survive its cleanup. Frame rows and baseline-summary.json saved. All160frames had0blobs at the existing detector threshold. This is an LEDs-off/no-RF baseline, NOT evidence that PICO LEDs are faulty, that tracker is invisible after synchronization or that cameras cannot track it.

## K01526
Source `D0296`, original line9, SHA256 `7c002b33163662ef413aba3bb590cee67fd53e3e3e145d23cddcf130c4a0cce1`. Status: historical/unadjudicated narrative; English source prose.

Still pending: isolated runtime+camera/clock/model integration for ONE finite optical phase pass, Phase210actual-code capability binding and source-derived finite range/resolution. Use controllers_off=true,min_controllers=0, validHMDguard rather than weakening/removing joint-controller logic. Keep diagnostic10/noOTA. No scan has run; do not report all phases searched. PICO visibility confirmation is physical positioning supplied by user, not verified optical identity. pose_valid=false.

## K01527
Source `D0297`, original line1, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

# Phase214–218: actual optical execution and unresolved native LED gate

## K01528
Source `D0297`, original line5, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

No tracker OTA was performed. Installed diagnostic10 remains unchanged. The reviewed Phase210 temporary Roy module DID execute during Phase214–218; runtime SRAM code admission verified SHA256 `63d319d4b424bc5a291558e41d9a740962b20eeb33dba77ba288d0c1d0480575`, 2043 bytes in the existing 2045-byte reservation. All five hardware attempts verified session closure, RAM restoration, MPU restoration and trace-control restoration. No RF attempt remains active. Controllers are intentionally disconnected for this optical bench; HMD validity remains required. `pose_valid=false`; full-body tracking is unproved.

## K01529
Source `D0297`, original line7, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## Measured attempts and corrections

## K01530
Source `D0297`, original line9, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

- Phase214 stopped before native10: the inherited admission incorrectly required a quiescent observer event sequence and FA7=0. Static installed native10 at 0x8df0/0x79b8 reads E08/F98 and the PPI capture path, not FA7 or SHORTS. Admission now retains these real prerequisites and bounded CC2 progression with START after STOP; no command21 repair was sent.
- Phase215 corrected admission but waited for unrelated IMU bootstrap before polling optics. Interrupted with all restoration flags true; no completed optical scan.
- Phase216 acquired the Roy clock (163 probes, 158 admitted; best modeled uncertainty approximately 286184 ns), observed 33 phases with zero detected blobs, then stopped at the unrelated staged S0 timeout. The uncertainty is a modeled host bracket, not physical exposure alignment proof.
- Phase217 separated the controllers-off optical bench from joint IMU stages, retained the initial native application proof, native10 and commands25/14, omitted command19. ONE bounded integrated pass completed 128 phases, 235492–300516 ticks in 512-tick/32-us steps (4.064-ms span). All 128 transport ACKs occurred; approximately 7149 processed camera frames had zero blob/model evidence. ACK is not direct native10 application proof. Finite exhaustion stopped the attempt; no automatic restart.
- Phase218 was a DIFFERENT measurement: fixed phase, no repeated scan, original grayscale PNGs and sampled brightness counts across four streams, while the user moved the tracker. Native LED25/14 and the same RF/module prerequisites retained; no19/noOTA. Raw frames show nearly black sensor images and isolated pixels, no tracker constellation. Lowering the detector threshold is not justified by these pixels. The launcher inherited `--duration 120` because the preparation script looked for `--seconds`; the operator issued SIGINT after sufficient pixel evidence. All restoration flags were subsequently verified true. Preserve the exact actual duration in radio-run.json rather than describing it as a completed 60-second run.

## K01531
Source `D0297`, original line15, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## New static discovery; do not overinterpret

## K01532
Source `D0297`, original line17, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Exact installed image base is 0x1000, SHA256 `040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d`.

## K01533
Source `D0297`, original line19, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Native LED gate 0xc5d4 returns false if byte 0x20000f6a is nonzero, or BOTH bytes 0x20001ff6 and 0x20001ff8 are zero. Only after those conditions does command25 mode2 allow the pulse. Existing diagnostic11 exposes 1ff6=0; it DOES NOT expose 1ff8 or F6A. Consequently LED gate closure on the actual tracker is not yet measured.

## K01534
Source `D0297`, original line21, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

The direct 1ff8/1ff9 writer at 0xc844 debounces its inputs for ten calls. Its caller at 0xbed2 obtains an ADC average (five reads, converted by 3600/4096) or a GPIO condition depending on board state. It assigns state values through 0x20001fe1. This proves electrical-state involvement, NOT that moving the tracker necessarily opens the gate. Earlier motion-only explanation is an unproved hypothesis. Do not force these stock RAM bytes or call unknown routines.

## K01535
Source `D0297`, original line23, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

The user now confirms the tracker was USB charging WITHOUT its strap/socket during Phase218. Next physical configuration requested: unplug USB and install the tracker in its stock socket/strap, in camera view. This is a source-backed changed factor, not another unchanged scan. User readiness is pending. Actual accessory role and native gate opening still require evidence. Official PICO specification distinguishes tracker and socket, but does not prove this firmware gate behavior.

## K01536
Source `D0297`, original line25, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## Continue

## K01537
Source `D0297`, original line27, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

1. Await stock socket/USB-disconnected readiness, without asking for controllers.
2. Seal a new isolated, one-use fixed-phase raw-pixel attempt with an ACTUAL `--duration 60` and unchanged module/RF/native admission. Do not repeat Phase217's scan unchanged.
3. Check original pixels for multiple LED clusters and model-fit evidence. If none, finish native electrical-state/command10 application audit on installed diagnostic10; no OTA and no arbitrary RAM writes.
4. Only after genuine target constellation exists, use measured timing and bounded temporal identity; then verify real moving 6DoF through the existing driver, including loss/staleness. No valid pose before identity and motion proof.

## K01538
Source `D0297`, original line32, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase214–218 isolated attempts, Phase217 native-led-gate.asm and native-led-state-writers.asm; Phase218 original raw-camera PNGs and pixels.jsonl (both downloaded partial snapshot and final attempt copy). Keep every attempt and its hashes.

## K01539
Source `D0297`, original line34, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## Phase219–220 continuation

## K01540
Source `D0297`, original line36, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

User installed the tracker in its stock strap/socket after the unplug-USB request (USB removal was requested; user explicitly confirmed socket installation). Phase219 completed the actual bounded `--duration 60` fixed-phase camera measurement with all four restoration flags true. Each stream retained 62 brightness samples: stream1 maximum7, stream2 maximum9, stream3 maximum255 but at most ONE pixel above180, stream4 maximum25. No constellation. Fixed-phase negative evidence does not prove LEDs are disabled across all phases.

## K01541
Source `D0297`, original line38, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

New exact installed-code discovery: stock radio command1 forwards its body through 0x8c10, which compares the first byte against 13 pairs of opcode/callback. Pair at 0x16568/0x1656c contains opcode0x31 and Thumb callback0x8681. Callback0x8680 takes its next argument byte, writes it to 0x20001ff6 at 0x8698, invokes the native LED-state routines, and emits a two-byte native reply. Thus body `31 01` is an existing stock LED-state command, not an arbitrary RAM write or new firmware. Profile-command hook falls back to the original native command1 prologue for this non-7e two-byte payload. Its electrical-state variable 0x20001fe1 is DIFFERENT from command19 autonomous-TX byte 0x20000fe1; do not confuse these addresses.

## K01542
Source `D0297`, original line40, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Phase220 sealed and STARTED: initial native prerequisites/10/25/14, one logical native command1 body3101, then fresh diagnostic11 MUST prove1FF6=1 before optics is allowed. Follow-up diagnostic6/8 captures the native timer/PPI state. ONE bounded128-phase pass is then allowed as a changed factor: native LED-state activation, with original raw-camera pixels retained. If the flag does not change, abort BEFORE scan. Same protected2043B Roymodule, same RF6000/900, no19, noOTA. Phase220 result and cleanup pending; do not launch another attempt until cleanup verified.

## K01543
Source `D0297`, original line42, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## Phase220 final / Phase221 prepared

## K01544
Source `D0297`, original line44, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Phase220 STOPPED before the diagnostic PLAN: product information identified the exact target, working RF replies persisted (~50% retained reply/TX), but the initial native7e mode application reply was absent. No native3101, no phase scan, no valid pose. All four cleanup flags TRUE, full attempt downloaded. Do not call this proof of the native IR command failing. Native dispatcher94fc/9544–9556 compares incoming per-command six-bit sequence against stored expected sequence; mismatch can suppress callbacks. Missing mode reply cause remains INFERRED, not proven. User requested to power-cycle only PICO to begin at a known sequence; fresh readiness pending.

## K01545
Source `D0297`, original line46, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Phase221 prepared/uploaded, NOT RUN: same220 stock3101+freshdiag11+diag6/8+boundedscan, with immediate abort on first mode_query_failures (avoid the prior reconfirmation loop). Hardware execution requires the user's fresh power-cycle readiness. No counter-value sweep, noOTA, controllers stayOFF. A new logical mode query failure is a stop, not an automatic retry. Preserve installed image and exact target identity.

## K01546
Source `D0297`, original line48, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Further exact string provenance: callback8680 loads literal at86D0=164A6; original NUL-terminated string is `set independent %d`. Therefore opcode31 is the stock independent-mode setter, not a newly invented LED command. Its volatile1FF6 flag participates directly in the LED admission gate. The nearby `recv body info` string belongs to a different callback; do not conflate them. No flash writing is shown in8680 itself. Hardware command execution remains unproved pending221.

## K01547
Source `D0297`, original line50, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## Phase221 result and Phase222 full-period complement

## K01548
Source `D0297`, original line52, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

User power-cycled PICO in its stock socket and reminded that power cycling changes radio mode. Fresh221 schema2 explicitly confirmed mode1/wire7/N1/pending0 after the existing bootstrap mode transition; do not inherit mode across reboot. Fresh independent flag diagnostic11 changed0→1 following stockcommand1 body3101. Retained native command1 reply3101 corroborates it. RF records include244 command23 body01 reports: exact native ISR77F6–7828 emits these every fifth allowed LED period, after c5d4 gate andFA4+2 permission. This is hardware proof of executing the enabled pulse-report branch, NOT optical photons or pose proof. The128phase4.064ms range nevertheless had no constellation; terminal finite exhaustion, allcleanuptrue. Fullattemptdownloaded. NoOTA/no19.

## K01549
Source `D0297`, original line54, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Phase222 STARTED: source-backed remaining timing uncertainty is the unmeasured physical exposure position inside a33.333ms period. Explicit915-phase complementary list saved/hashbound inattempt/remaining-phases.json,512ticks(32us)step, excludesalreadytested235492–300516range. Maximum420s, single integrated session; first modequeryfailure stops immediately, diag11 mustagainproveindependent1. Retainsprotectedmodule/RF/HMD/controllersOFF/nativeguards. Stops aftera3-rawblob candidate and2seconds additionalcapture for analysis; raw search lock DOES NOT bypass model-fit/temporalidentity or admitpose. No autorestart. This changes the earlier self-imposed128phase bound to cover the remainingperiod under the user's authorization foroneintegrated opticalpass; not915separate launches. Actual222result/cleanupPENDING.

## K01550
Source `D0297`, original line56, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## CRITICAL CORRECTION: Phase221 HAS optical evidence; Phase223 active

## K01551
Source `D0297`, original line58, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Full optical.jsonl analysis, rather than partial raw-pixel sampling, found33 exposures with>=3 detected blobs. Most are genuine calibrated model fits:5inliers oncamera[CAMERA_SERIAL] and6inliers on[CAMERA_SERIAL], residual0.1796–0.2152px in initial frames; world-position candidates near(-0.228,1.505,-0.224)m. Phases267236/267748/268260; strongest267748. The saved original1.png visibly showsfivebrightLEDs. Previous statements that221 had zero constellation are FALSE and superseded. No temporalidentity/validpose yet. The old scan required>=6inliers in>=half of fourframeindices, despite the legitimate5inliercamera and interleaved cameras; it continuedpastrealopticalevidence. Save the fulljournal and brightPNGs, not merely partial samples.

## K01552
Source `D0297`, original line60, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Phase222 was interrupted once this evidence was found, ratherthan completingunnecessaryremainingphases. Allfourcleanupflags verifiedTRUE. Preserve222resultbutdo notclaimfullperiodcoverage.

## K01553
Source `D0297`, original line62, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Phase223 STARTED: fixedmeasuredphase267748, NOSCAN;5inliersearchvisibility consistent with existingMeasuredPosePipeline minimum5. TemporalIdentity two commanded darkframes/six stablelit bracketingframes, quality/age/driverpublication gates UNCHANGED. Originalbrightframes are savedimmediately(>=3pixels>180), ratherthan samplingonlyevery30thframe. Freshnative3101/diag11 andmodeproof retained. User instructedslowmotion. Sameprotectedmodule/no19/noOTA.223result/cleanupPENDING.

## K01554
Source `D0297`, original line64, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## Phase223 FIRST REAL OPTICAL + STEAMVR PILOT, complete

## K01555
Source `D0297`, original line66, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Fixed267748/no scan completed120seconds with ALLfourcleanupflagsTRUE, noOTA/no19. Final station snapshot:2261actualopticalmeasurements,7633rejections(overallfourstream count, not a valid-fraction denominator), temporal_identity_verified=true, pose_valid=true atfinalsnapshot/age130.243ms/quality0.5704. Phase file locked,119phaseACKs. Raw originalLEDimages saved. Finalpresence is not proof of uninterruptedtracking.

## K01556
Source `D0297`, original line68, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Independent actualSteamVR tracking_watch captured296samples over~3s. Exactchip[TRACKER_ID]/index3/kind3 (generictracker):231validsamples,281connected. Valid-only data contains63distinctposematrices, XYZ ranges0.070289/0.043038/0.056179m. Maximum gap betweenvalidmonitor samples375.978ms. NEVER includeinvalidzero matrices in movement ranges (doing so incorrectly makesYspan3.265m). This proves live measured optically moving pose reached SteamVR; limitedshort-windowvalidity is78.0405%, so robust fullbody remainsunfinished. Controllers intentionallyOFF, noIMUfusion, noocclusionrecovery.

## K01557
Source `D0297`, original line70, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Phase224 STARTED host-only latency factor: TimestampMux formerlyusedPES_packet_length=0 for everyvideoAU; now finite8+AUlength when<=65535 (largerkeepszero), with identicalpayload/PTS. Potentialoneframe demuxdelayreduction is a hypothesis untilmeasured. Samefixedphase/Roymodule/RF/stocknative3101/identity/150msageguards. Addsmeasurement-onlypublishedposeage,rejectionreasonlogging andindependentSteamVRmonitor forwhole90s attempt. NoOTA/no19/noageextension.224result/cleanupPENDING.

## K01558
Source `D0297`, original line73, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## Phase224 completion and Phase225 clock correction

## K01559
Source `D0297`, original line75, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Hardware: Phase224 completed; all four cleanup flags true. No OTA. 1347 published-pose journal entries versus 1346 station counter snapshot (snapshot/log boundary); age minimum71.189ms, maximum119.953ms, mean80.486ms. Actual SteamVR valid4886/6149=79.46% between first and last publication. Maximum publication gap3376.977ms. Camera sync starts after radio/bootstrap, so comparing last10seconds of entire driver monitor includes post-RF invalidation and does NOT establish late-session loss. Last clock probe71.784seconds after firstprobe, last pose72.482seconds, firstpose10.154seconds. User confirms tracker remains in view with IR strip toward HMD and not hand-covered. Real intermittent gaps remain; physical occlusion must not be assumed.

## K01560
Source `D0297`, original line77, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Offline measured-data analysis: original40point exact64MHz fixed-rate clock offset changes63.728us within one five-second bin; camera exposure~37us. This is a timing-instability candidate, not a proven cause. A causal400point weighted rate+offset replay, after100points and5seconds, reduces maximum successive offset change8977.711ns to3885.418ns; p95new1558.740ns. Learned median slope15.625053672ns/cycle, last15.625055222. Replay uses existing1030actualclock probes; it is not a hardware success claim. Evidence: outputs/phase224_analyze.py and outputs/phase224/optical-pass/attempt/timing-analysis.json.

## K01561
Source `D0297`, original line79, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Phase225 started one host clock factor only: original40point startup preserved; after100realprobes spanning5seconds, fit weighted slope+offset over up to400points. Reject rate outside100ppm; preserve1msuncertainty ceiling,250ms projection horizon,150ms pose age, real temporal identity, stock mode/native3101 proof, protected2043B module and all restoration gates. Log actual fit parameters. Fixed267748; no scan/no19/noOTA/controllersOFF. Result and cleanup pending; do not claim continuous/fullbody tracking from replay.

## K01562
Source `D0297`, original line82, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## Phase225 completed: clock candidate NOT validated

## K01563
Source `D0297`, original line84, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

All four cleanup flags TRUE; no active RF and no tracker OTA. Fresh Frame mode and stock independent flag proved. 82phase ACKs, learned slope15.625046891ns/cycle at final snapshot. Temporal identity never verified, zero published poses. Do not promote this clock change or interpret smooth offline fit as improved optical tracking.

## K01564
Source `D0297`, original line86, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Full journal: camera[CAMERA_SERIAL] has533 genuine>=5inlierfits out of2044decoded candidates; camera[CAMERA_SERIAL]/2015. Both cameras contain only ONE distinct decoded dark frame (index48) with all six lit bracketing frames. Other two cameras have no model fits. Two DISTINCT commanded dark events are mandatory, so zero publication is the correct fail-closed outcome; no need to weaken the gate. Candidate-analysis reports lack of model fit as dark, which does NOT necessarily mean no photons: distinguish zero blobs, fewer than5blobs, solver rejection and missing decodedframes next. These ratios are visibility/modelfit measures, not SteamVR validity. Physical visibility was confirmed by user; cause of intermittent lit fits remains open. Source and evidence: outputs/phase225_candidates.py and outputs/phase225/optical-pass/attempt/candidate-analysis.json. Baseline224 remains the last real-pose implementation.

## K01565
Source `D0297`, original line88, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Next work must use retained candidate/rawpixel/nativeRF journals to distinguish phase instability from detection/model failure, reconstruct/log exact native10 ACK timestamp and predicted skipped-frame index, and test a justified correction only after naming its distinguishing criterion. Do not repeat unchangedRF/phase scans, conflate transportACK with nativeexecution, extend pose age or claim fullbody completion. User powercycle changes mode; fresh proof mandatory on any future attempt.

## K01566
Source `D0297`, original line91, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## Phase226 one bounded local recalibration

## K01567
Source `D0297`, original line93, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Phase225 camera1:1337zero-blob,165one-to-fourblob,9five-plusblob failedfit,533validmodel frames; camera2:1302zero-blob,96one-to-four,4failedfit,613validmodel. The dominant loss precedes pose solver. Photons below detector threshold are still possible; zero detected blobs does not prove electrically off LEDs.

## K01568
Source `D0297`, original line95, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Phase226 prepared and started one finite LOCAL calibration, not an unchanged fullperiodscan: learnedclock changesoffset relative to earliercalibratedphase, so choose best within prior measuredlitregion expanded to +/-64us. Exactnine phases267748+[-1024,-768,-512,-256,0,256,512,768,1024]ticks. Wait900camera frames for400point fit maturity; each candidate needs freshspecificphase ACK,6settleframes,120subsequentframe slots. Missingdecodedframes do not improve score. Select highestlit-frame score, require>=60decoded and>=12litframe observations, then HOLD, never restart. Maximum180s. Identityand150msposeage unchanged; noOTA/no19. Add exactACK/nativecycle/fit/exposure/predicteddark journal for causal diagnosis. Boundarycheck verifiednine-phasebound, measured-best, no-restartafterhold anddark-exhaustion. Result/cleanup pending.

## K01569
Source `D0297`, original line98, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## Phase226 interrupted by confirmed tracker sleep; Phase227 fresh continuation

## K01570
Source `D0297`, original line100, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Hardware226: RF stops after lastpacket1676/slot4001 at host10648224687764ns; stopped runner with SIGINT, allfourcleanupTRUE. Localcalibration had notbegun: statewaiting_clock/local_results[],0/9candidate phases. User confirms tracker slept, then switched itON. Mode proof is invalidated onwake; freshnativequery stillmandatory. Thus do notattributeRFfailureto clocks/opticalcode or claim localcalibration tested.

## K01571
Source `D0297`, original line102, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

226camera1/2 each652candidateframes, zero>=5inlierfits;180zero-blob each, remainingframes1-4blobs. Original1.png showsfourpoints onsmallconstellation. Exact22command10 ACKs/expected-dark/exposure/fit retained, detailedack-analysis.json. That recording differsfrom225visibility and cannot support clock-only causalcomparison. Noemptyautorestart; userwake is the changed external condition.

## K01572
Source `D0297`, original line104, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

227 newexclusive attempt copies exact226code to execute the previously UNPERFORMED nine-phase calibration and subsequenthold under freshmode/nativeindependentproof. NoOTA/no19/noidentity/ageguardchanges. User instructedslowmovementwithIRstrip towardHMDtoavoidsleep. Max180seconds; no secondscan/restartinsideattempt.227result/cleanupPENDING.

## K01573
Source `D0297`, original line107, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

## Phase227 complete:98.4168percent driver validity after one localcalibration

## K01574
Source `D0297`, original line109, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

All4cleanupTRUE/noOTA. Full180s attempt,3523actualpublishedposes/realtemporalidentity(two darkframes98and151). Nine-phasecalibration COMPLETE; best267492 has115/120litintervals, prior26774875/120,26723698/120; fourhighphases andlowestphase zero,266980two. Holdstarts10875777941108ns fromnextclockprobeafterlocked-result journal. Excludeinitial1secondsettling:94.143s measuredhold, actualSteamVR9138/9285valid=98.4168percent, maxvalidsamplegap304.723ms, maxpublicationgap466.311ms. This is measured singletracker opticalbench progress, not robustfullbody/IMU/multipletrackers/controllerscoexistence. Driver-analysis.json andphase227_driver_analysis.py preserve exactmethod.

## K01575
Source `D0297`, original line111, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

CRITICAL AGE REPORT CORRECTION: actual executed measured_pipeline.POSE_AGE_NS anddriver.pose_frame.MAX_AGE_NS are250000000ns; nativeSteamVRdriver also invalidatesat250ms. Earlier150ms-ceiling claims inthesehandoffs areFALSE. 150000000 in_send isquality-decay denominator only. No ageceilingswerechanged in224-228;98.4168percent usesoriginal250msrules and is NOT frame-by-frame opticalvisibility fraction. No inventedposeadded, but SteamVRcanreturnlastvalidpose withinagebudget. Sourceverificationperformednow; recordedpublicationages~80ms remain actual.

## K01576
Source `D0297`, original line113, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

Origin bug:12240validdriver/publicationmatches medianXYZoffset[-0.00000918,1.74208476,-0.00001389]m. Read-only actualOpenVR GetRawZeroPoseToStandingAbsoluteTrackingPose returnsidentityrotation/Ytranslation1.74207985m, preservedin228origin-proof.json. HMDhistory alreadyusesTrackingUniverseStanding, whileDriverPose withidentityworldFromDriver expectsrawframe; SteamVRaddsroomtransformagain. Earlier no-doubletransform conclusion basedsource only was insufficient; hardware establishesduplicateheight.

## K01577
Source `D0297`, original line115, SHA256 `c3c969a8c6fd2431c9c069f89f855b452294a7dc1e5b68b3004fd5a930e67b04`. Status: historical/unadjudicated narrative; English source prose.

228 host-only correction: tracking_watch logs actualraw_to_standing everysample;ControllerGuard.driver_pose obtainsvalidsame-exposureHMDbrackets<=30ms, requiresfiniteproperrigidmatrixandunchangedoriginacrossbrackets. Inverseactualorigin convertsoutputworldpose,quaternion,velocity,angularvelocity torawdriverframe. PipelineinternalStandingmodel/calibration unchanged. Missing/nonrigid/recenterorigin rejects ratherthanusingguessedheight. Outputjournaladds wire_position forindependentcomparison. Compiledmonitor exacthash sealed; actualvector/rotation/poseroundtrip +missing/recenterreject checks passed. Compileg++-O2/openvrrootheader/arm64libopenvr_api/-pthread. NoOTA/no19, same227clock/ninelocalphasecalibration/250msrules.228RUNNING result/cleanupPENDING.

## K01578
Source `D0298`, original line1, SHA256 `c96a7e3bfc9678b44cddf0a1a008e2d451317196e17ac9a7adfd1b6dcc6727ce`. Status: historical/unadjudicated narrative; English source prose.

# Phase229: native DEV screen recording and optical synchronization blocker

## K01579
Source `D0298`, original line3, SHA256 `c96a7e3bfc9678b44cddf0a1a008e2d451317196e17ac9a7adfd1b6dcc6727ce`. Status: historical/unadjudicated narrative; English source prose.

Hardware: one fresh exclusive attempt on current diagnostic10; no OTA, no command19. HMD valid, controllers disconnected. Fresh native mode and independent LED flag proved; radio connected. Native `start_tracking_recording 1 1 0` produced actual 1920x1080 headset video in owned dataset `tmp_h264.h264` (61,341,696 bytes). First decoded frame visually verified: actual headset passthrough with recording indicator. No valid tracker poses were published. This recording is NOT a successful tracker demonstration.

## K01580
Source `D0298`, original line5, SHA256 `c96a7e3bfc9678b44cddf0a1a008e2d451317196e17ac9a7adfd1b6dcc6727ce`. Status: historical/unadjudicated narrative; English source prose.

The exact observed IR timestamp interval for all four kind38 streams is 0.030000000000654836 seconds (315 samples each in a bounded completed-container inspection). Existing ExposureSchedule only admits intervals strictly between .032 and .035 seconds, so intervals remained zero and no synchronization plan was emitted. Screen recording causing the cadence change is inferred, not independently proven. Do not claim causation or rerun unchanged RF. Earlier mixed kind32/kind38 metadata inspection incorrectly yielded 13.296ms for upper cameras; corrected by independent stream keys.

## K01581
Source `D0298`, original line7, SHA256 `c96a7e3bfc9678b44cddf0a1a008e2d451317196e17ac9a7adfd1b6dcc6727ce`. Status: historical/unadjudicated narrative; English source prose.

Stopped with SIGINT. All four independent restoration fields true: session closed, RAM restored, MPU restored, trace controls restored. No active RF remains. Owned native dataset exported to remote attempt/dev-recording before original dataset cleanup. Preview video local outputs/phase229/optical-pass/attempt/dev-recording.mp4 is remuxed with an ASSUMED 60fps; its playback duration/speed is not certified. Preserve original H264 and derive actual cadence before distributing video.

## K01582
Source `D0298`, original line9, SHA256 `c96a7e3bfc9678b44cddf0a1a008e2d451317196e17ac9a7adfd1b6dcc6727ce`. Status: historical/unadjudicated narrative; English source prose.

Host changes: actual prior measured world pose is reprojected using current exposure HMD/camera transform to seed association; all observed LED/identity/age requirements retained. This change is NOT hardware validated because synchronization never started. Shutdown rechecks worker after camera source closure.

## K01583
Source `D0298`, original line11, SHA256 `c96a7e3bfc9678b44cddf0a1a008e2d451317196e17ac9a7adfd1b6dcc6727ce`. Status: historical/unadjudicated narrative; English source prose.

Next: determine actual active tracking-camera cadence and its control from native evidence. Timer LED period is 533333 ticks (~33.333ms), so merely widening interval admission to 30ms is insufficient to guarantee stable optical synchronization. Resolve camera/LED cadence compatibility before another RF session. Reuse native DEV capture; no GL mirror experiments. User must not wait wearing headset during offline preparation.

## K01584
Source `D0299`, original line1, SHA256 `304d24f5e56847071f63b6cc1257b49370e955cadf4bc3115cda458a9bc47ebc`. Status: historical/unadjudicated narrative; English source prose.

# Native video, half-size model, and continuing optical work

## K01585
Source `D0299`, original line3, SHA256 `304d24f5e56847071f63b6cc1257b49370e955cadf4bc3115cda458a9bc47ebc`. Status: historical/unadjudicated narrative; English source prose.

Phase230 completed: mode1 and native LED flag freshly proved; real temporal identity, 1742 optical publications. After finite local calibration, 94.122s hold had4869/9005 valid SteamVR samples (54.07%); maximum valid gap5.202s, publication gap5.269s. All four radio restoration fields true. Actual headset video visibly contains the native 3D model above the tracker held in user's hand; this is not a smooth/fullbody demonstration. Full recording delivered to `F:/Shlik/Desktop/PICO-SteamVR-DEV-2026-10-10.mp4`,1920x1080,3861frames,20fps remux,193.05s. Original H264 retained remotely in phase230/optical-pass/attempt/dev-recording. Native SPS num_units_in_tick1,time_scale40; earlier229 remux60fps was incorrect preview and must not be distributed.

## K01586
Source `D0299`, original line5, SHA256 `304d24f5e56847071f63b6cc1257b49370e955cadf4bc3115cda458a9bc47ebc`. Status: historical/unadjudicated narrative; English source prose.

Root of229 synchronization failure resolved from native logs: Auto lighting frequency selected50Hz at21:02:16, BEFORE DEV recording; camera rate then33.3Hz/30ms. Native setting `camera/lightSourceFrequency=1` maps to internalForce60Hz and restored30Hz/33.333ms, confirmed in230. `=2` mapsForce50Hz; original0Auto. Stock reader cf3828..cf38e0 with lookup at7baee8 [0,2,1]. Current setting1 persists; no firmware changes.

## K01587
Source `D0299`, original line7, SHA256 `304d24f5e56847071f63b6cc1257b49370e955cadf4bc3115cda458a9bc47ebc`. Status: historical/unadjudicated narrative; English source prose.

Phase231: half-size PRIVATE render model built in pico_tracker/resources/rendermodels/pico_half; original generic_tracker untouched. Driver source changed ONLY RenderModelName to `{pico_tracker}pico_half`; actual installed original source matched repository SHA849a3ec5..., original ELF507baa58..., backup remotephase231/driver_before.so. New ELF91478b622943e1f812a8f53bc4c3e487509856f02d5709b9798c7f3d98707ac7. Native steamvr.service restarted after RF cleanup, loaded new driver. Native RenderModels API validates2802vertices for both; dimensions original[.100064002,.100064002,.0534920022] versus half[.050032001,.050032001,.0267460011]. Pose scale unchanged. In-headset moving validation of smaller model pending.

## K01588
Source `D0299`, original line9, SHA256 `304d24f5e56847071f63b6cc1257b49370e955cadf4bc3115cda458a9bc47ebc`. Status: historical/unadjudicated narrative; English source prose.

Phase232 prepared: decode only two upper IR cameras with proven PICO constellation, keep all metadata; remove meaningless empty-blob axis error; measured velocity50ms lowpass, preserve velocity across near-simultaneous stereo pairs instead of reset. Actual positions, quaternions, identity, age invalidation unchanged. Offline replay1742real230publications reduces velocity jump RMS.121459->.049051m/s (59.62%); visual benefit unproven. Initial and second readiness checks stopped BEFORE RAM/RF because HMD invalid/user absent; user subsequently confirms wearing HMD. Native wake exits standby but default five-second screen timeout returns to standby. User authorizes trying; temporary power/turnOffScreensTimeout5->300 applied, original saved in232attempt/power-before.json. MUST restore to5 when this attempt ends, including failure.

## K01589
Source `D0299`, original line11, SHA256 `304d24f5e56847071f63b6cc1257b49370e955cadf4bc3115cda458a9bc47ebc`. Status: historical/unadjudicated narrative; English source prose.

USER RULE: Fresh mode1 must be proved EVERY run. Tracker sleep/button wake may change mode; never inherit old evidence. Query-first strict bootstrap retained. User observes native model disappearance coincident with IR blink; current pilots have NO IMU stream (no command19), so do not claim sensor-derived absolute position. Display continuity and actual valid SteamVR pose are distinct; persistent last-known visual model feature is discussed but NOT implemented. IMU integration remains to do, including measured gyro axes/timing and optical anchor. Never mark old/frozen pose as newly measured fullbody tracking.

## K01590
Source `D0299`, original line13, SHA256 `304d24f5e56847071f63b6cc1257b49370e955cadf4bc3115cda458a9bc47ebc`. Status: historical/unadjudicated narrative; English source prose.

Next: finish232 actual fresh readiness/mode and bounded pilot, save video, compare actual availability/latency/jitter against230; restore power setting. Then implement explicit stale visual presentation without falsifying tracking and integrate proven current-firmware IMU. Fullbody, controller coexistence, and3trackers not achieved.

## K01591
Source `D0300`, original line1, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

# Phase233–234: connected visual continuity, measured phase plateau

## K01592
Source `D0300`, original line5, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

Phase233 was stopped by the existing controllers-off guard after controller 1 connected, before optical validation. All four cleanup fields were true. After the user confirmed both controllers OFF and PICO ready, a new exclusive Phase234 attempt ran to completion; Phase233 was not replayed. Native tracker mode1/N1/pending0 was proved freshly. No OTA, firmware edits, command19 or IMU stream.

## K01593
Source `D0300`, original line7, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

Phase234 made 2,628 publication-log records (station optical count 2,630). After phase lock, the measured interval was 94.285652401 seconds, with 9,064 native SteamVR tracker samples:

## K01594
Source `D0300`, original line9, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

- Fresh tracking (`valid && result==200`): 6,785/9,064 = 74.85657546%.
- Last-known visual hold (`valid && result==201`): 2,263 samples.
- Display availability (`valid`, either result): 9,048/9,064 = 99.82347749%.
- Maximum gap between fresh valid samples: 11,760.490102 ms; between publications: 8,623.915982 ms.

## K01595
Source `D0300`, original line14, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

**Do not promote display availability to tracking availability.** Visual hold retains the last measured transform with zero velocities and OutOfRange201 while current RF status says connected. It supplies no new position or orientation. Before the first measured pose, or after disconnect, pose validity is false. Fresh result200 still requires actual optical identity, source time and the unchanged 250ms age limit.

## K01596
Source `D0300`, original line16, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

The finite nine-candidate in-session calibration measured 116/120 lit intervals at267492 and115/120 at267748. The new selection chose their midpoint267620, avoiding a one-frame noisy winner at a plateau edge. The improvement relative to232 (13.02% fresh availability) is observed, not an isolated causal proof: lighting and motion were not controlled between sessions.

## K01597
Source `D0300`, original line18, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

Matched fresh driver/publication positions (8,479 matches): median offsets X−0.157665mm, Y+0.005705mm, Z+0.200137mm. P05/P95 offsets are within about4.1mm. These are coordinate-path consistency, **not absolute tracker accuracy**. Native half-size model dimensions remain exactly one half of the original model.

## K01598
Source `D0300`, original line20, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

## Native DEV video and cleanup

## K01599
Source `D0300`, original line22, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

Full native headset DEV recording, uncut, remuxed at the H264 SPS-derived20fps:
`F:/Shlik/Desktop/PICO-SteamVR-connected-model-2026-10-10.mp4`.

## K01600
Source `D0300`, original line25, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

Reviewed actual native frames at110s and130s: the white half-size 3D model is rendered over the physical PICO tracker. Still frames prove rendering; they do not prove smooth dynamic tracking. The scene/tracker is largely stationary in those frames.
