# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K01601
Source `D0300`, original line27, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

Evidence: `outputs/phase234/optical-pass/attempt/driver-analysis.json`, `radio-live.json`, `diagnostic-proof.jsonl`, `phase.json`, `cleanup.json`, `power-restored.json`; complete host evidence archive `outputs/phase234-evidence.tar.gz`. Original native dataset remains in the remote attempt/dev-recording archive. Session, RAM, MPU and trace controls restoration all true; temporary screen timeout restored300→5. Native camera force60Hz remains active. RF no longer running.

## K01602
Source `D0300`, original line29, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

## Investigation and next work

## K01603
Source `D0300`, original line31, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

Current IMU samples=0; axes and time alignment unverified. Current pipeline cannot give sensor-derived position during visual loss. Native command19 and an actual per-session optical/gyro axis/time calibration remain necessary before using inertial fusion. Controller coexistence and multiple body trackers remain unfulfilled.

## K01604
Source `D0300`, original line33, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

Raw pixel journaling is deliberately sampled: all frames with >=3 pixels above180, otherwise only every30th processed frame. Never compute lit fractions by dividing bright rows by all logged rows. `gap-analysis.json` reports processed index spans and sampled bright rows separately, relative to the RAW camera start; no guessed RAW/MONOTONIC alignment. After lock, clock-fit update jumps ranged−1.904498 to+1.757430us, RMS0.628978us. This does not exclude accumulated bias, tracker clock drift, native synchronization effects or visibility changes.

## K01605
Source `D0300`, original line35, SHA256 `d3fff35a7052d93ca460bbb5b8cc725da534a2918daf32d9e3aa4af5b0f1fd41`. Status: historical/unadjudicated narrative; English source prose.

Next discriminating work: integrate one guarded native19 run on current diagnostic10, keeping exact protected module and controllers-off admission, fresh same-mode producer advancement and schema4 gates. Measure decoded IMU, time and axes before fusion. Do not remove guards, repeat consumed runs, blind-sweep, or claim full-body readiness from persistent model display.

## K01606
Source `D0301`, original line1, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

# Phase235–236: actual IMU delivery and measured counter-clock repair

## K01607
Source `D0301`, original line3, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

## Phase235 completed

## K01608
Source `D0301`, original line5, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Fresh user readiness: both Frame controllers OFF, HMD/PICO ready, user explicitly ready to rotate PICO. Actual native mode1/N1/pending0 was reproved, exact protected Phase210 Roy module and6000us/900us timing unchanged; no OTA or flash edits. Fixed optical phase267620 was reverified after900 camera indices:48 lit intervals/59 observed in61 expected. No phase search/restart. Existing temporal identity admitted two command10 dark pulses with six lit brackets.

## K01609
Source `D0301`, original line7, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Bench admission for native19 is separate from joint-game `stream_admitted`: strict current controllers-OFF/HMD watchdog, fresh same-link Frame proof,3s continuous <=250ms-old replies, actual temporal optical identity, verified fixedphase and fresh optical pose. After initialization and native10/IR31, two nonce-bound schema3 snapshots in sensor mode7/enabled/postgate1 advanced132→176; intervening schema4 was exactly allthree gates1. Producer proof age at logical19/body00 was0.404771s, below3s. No joint coexistence claim. Admission cases and no-restart fixedphase were checked offline before this exclusive hardware attempt.

## K01610
Source `D0301`, original line9, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Hardware result: **7,171 accepted actual IMU samples**,18,943 counter gaps; full source counter span26,113 over128.99458075s. This is sparse delivery, not a continuous200Hz stream. Optical publications3,562 (station measurements3,567). After fixedphase lock130.680944s:9,547/12,568=75.96276257% fresh result200; visual availability12,407/12,568=98.71896881%, stale visual201=2,860. Maximum fresh sample gap3.783474s. These availability metrics are not controlled A/B causal results and are not smooth full-body readiness.

## K01611
Source `D0301`, original line11, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Full native DEV video20fps: `F:/Shlik/Desktop/PICO-SteamVR-optical-and-IMU-2026-10-10.mp4`. This video is an optical pilot with actual IMU received, **not verified inertial fusion**. All four cleanup fields true; RF stopped and temporary screen timeout restored300→5.

## K01612
Source `D0301`, original line13, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

## Clock blocker proved from actual packets

## K01613
Source `D0301`, original line15, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Existing SampleClock assumed4,976,480ns and rejected a fitted drift >0.2%. Actual counter/recorded Roy DWT lower envelope across130 one-second bins gave **4,939,977.608340125ns**:−0.73349821% relative to the old seed, so rejected. Independent first/second halves:4,939,967.223497 and4,939,993.002801ns (25.779ns difference). DWT rate15.625ns/cycle is approximate here; actual measured fit was15.625051ns/cycle, a few ppm difference and far smaller than0.7335%. This proves counter cadence mismatch; it does not alone prove physical sample offset or gyro axes.

## K01614
Source `D0301`, original line17, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Exact source: `outputs/phase235/optical-pass/attempt/radio-live-records.jsonl`; fit and source binding in`outputs/phase235/counter-fit.json`, `outputs/phase236/optical-pass/sample-period-evidence.json`. Offline replay through the actual candidate SampleClock: old seed accepted0, rejected7,087 for drift; measured seed accepted7,084, rejected3. Same0.2% drift gate and counter validation. No expanded tolerance.

## K01615
Source `D0301`, original line19, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

## Phase236 completed; fusion not admitted

## K01616
Source `D0301`, original line21, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Candidate makes the seed explicit and target-bound (chip[TRACKER_ID], serial[TRACKER_SERIAL], sensor07), pins the evidence in the frozen invocation, and preserves all clock/axis/time/source-age/RF guards. Existing factory/default seed remains for other targets. Actual physical alignment still requires gyro/optical calibration. Added measured optical rotations, aligned-IMU candidate timestamps/counters/raw vectors, and full calibration axes/bias/singular values to evidence logging for offline diagnosis. Candidate timestamps are labelled unverified calibration candidates.

## K01617
Source `D0301`, original line23, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

After offline replay passed, user confirmed readiness for ONE new<=3min rotating/stationary calibration session. Phase236 completed180s, allfourcleanupTRUE, screen timeout restored300→5, RF stopped. No firmware/OTA changes or phase scan. Actual6,853IMU,19,261counter gaps;6,368 aligned candidates and2,343 published optical rotations. SyncTimeResolver verified400samples, final spread673,894ns/MAD19,566.5ns. This verifies its sync consistency criterion, not measured gyro axes or the final optical residual time offset. Those remain unverified; no fusion admitted.

## K01618
Source `D0301`, original line25, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Fresh native200availability6,469/12,574=51.4474%; display12,281/12,574=97.6698%; maxfreshgap8.817s. Full native video onDesktop: `F:/Shlik/Desktop/PICO-SteamVR-IMU-time-verified-2026-10-10.mp4`. This is evidence of the optical/IMU pilot, not a smooth fullbody demonstration.

## K01619
Source `D0301`, original line27, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

## Phase237 offline delivery diagnosis

## K01620
Source `D0301`, original line29, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Recorded current-policy calibration has ZERO admissible pairs at every allowed−25..+25ms offset. At zerooffset:365opticalpairs outsideIMUextent,87outside20..200ms opticalinterval, andALL1,890remainingpairs intersect both missing counters and>8ms gap. Typical opticalspacing33.3ms; IMU deltas1 and7 dominate. Longest consecutivecounter run39.521ms. Preserve current no-interpolation/across-gap gate.

## K01621
Source `D0301`, original line31, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Native six-byte suffix is magnetometer (documented stockB3D8..B432/9778); it is not an omitted gyro sample. After19+1s native records:command2body18=2,957;24=2,909;36=207;42=259. No partial records included. Decoder correctly reads full18B blocks and leaves6B auxiliary field.

## K01622
Source `D0301`, original line33, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Exact installedimage `outputs/phase189/ota-proposal/image.bin`, SHA040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d, matches investigated stock code byte-for-byte for9778..98e8 andad08..add8. AD08..ADD4 increments20001fb1 and queues18B samples.97A8 maskscounter&7 into eight-slot ring20001afc.9858..98DE selects at most TWO18B records: selectedindex20001b95 thenlatestindex20001afc.97F4..984C adjusts selectedindex from producer delta and may choose an intermediate older sample. Therefore sparse observed counters cannot be attributed solely to RF loss; native selection can skip history. Actual producer/transport/call timing remains to be distinguished. Disassemblies and recorded diagnosis: `outputs/phase237/queue-enqueue-stock-disasm.txt`, `queue-select-stock-disasm.txt`, `delivery-diagnosis.json`. The initial disassembly of encrypted vendor container was invalid; `sensor-queue-disasm.txt` is NOT usable code evidence. Use decrypted stock disassembly plus exactinstalled-image equality.

## K01623
Source `D0301`, original line35, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Next: trace callers of9858 and97F4 in exactinstalledimage, explain selection/cadence using recorded records and native mode1 scheduling; derive one stock-compatible sequential-delivery change only after source evidence. No new RF/OTA in237. Do not relax calibration gates or claim missing samples as measurements.

## K01624
Source `D0301`, original line37, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Caller trace completed statically in the exactinstalled image: packet builder9458 calls9858 at948C. It parses received working records at94D0..94EE; record command11 (`cmp #0xb` at94D8) passes its first body byte to97F4 at94E0, which updates the native sample selection. Native magnetometer retrieval9778 is appended on alternating builder passes94A0..94AE. Current executed host PeerEngine and StationV4Runtime never publish a command11 sample-counter record; they track `last_counter` locally only. This is a concrete protocol omission, **not yet hardware-proven cause of all gaps**. Packet body-counter feedback to native selection is source-proven; exact latest-counter semantics and scheduling with pending timed10 still need review before sending. Preserve independent command11 sequence and existing ACK/working checksum/reconnect gates; never combine it into the exact8B timed10 packet. `outputs/phase237/queue-caller-installed-disasm.txt` records this path; call candidate scan alone is not code-boundary proof.

## K01625
Source `D0301`, original line39, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

## Remaining risks and continuation

## K01626
Source `D0301`, original line41, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

Sparse IMU reception commonly has delta1 anddelta7; the current calibrator requires consecutive counters and gaps<=8ms for continuous optical/IMU integration. Even a repaired timebase may leave insufficient continuous calibration segments. Do not weaken these requirements blindly; analyze the recorded rotation/IMU candidates and actual delivery pattern first. Current physical fullbody, multi-tracker roles, controller coexistence and verified fusion remain incomplete.

## K01627
Source `D0301`, original line43, SHA256 `0e6059fde4b0482c3a34d08c4a5b5f58a758bbf6d21b97722d2d389467aee892`. Status: historical/unadjudicated narrative; English source prose.

The Phase235 `phase.json` persisted the same numeric phase only once and can still say`verifying`; the actual in-session phase verification result is in`radio-live.json`/`optical.jsonl`. This is stale metadata, not a failed hardware phase lock.

## K01628
Source `D0302`, original line1, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

# Phase238: native IMU sample-counter feedback

## K01629
Source `D0302`, original line3, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

User explicitly requested continuing until body tracking. Fresh user readiness confirmed HMD on, PICO visible, both Frame controllers OFF. No OTA/firmware changes.

## K01630
Source `D0302`, original line5, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

## Source-proven protocol change

## K01631
Source `D0302`, original line7, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

Exact installed diagnostic10 SHA040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d native packet builder9458 handles incoming working command11 at94D8, passing its first body byte at94E0 to97F4. That function updates selected IMU ring index20001b95 relative to latest20001afc;9858 can emit selected+latest18B samples. Prior host never published this counter feedback. This is an omitted protocol input, not hardware proof that it explains every lost sample.

## K01632
Source `D0302`, original line9, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

Candidate `outputs/phase238/optical-pass` sends command11 with a counter only after an actual CRC/checksum/identity-validated and forward-counter accepted IMU sample. Feedback is enabled only when the guarded native19 step is reached. Existing independent per-command sequence and transport-tag ACK retire it. Only newest unsent feedback is retained; existing queued timed10/14 work takes priority. Exact8B timed10 remains a separate command packet. Disconnect clears feedback and disables it. No changes to protected Roy code, RF period/slot, optical identity, clocks,250ms age, calibration continuity or controller guards.

## K01633
Source `D0302`, original line11, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

Focused actual host checks passed: sample feedback disabled before19; latest counter coalescing while pending; timed10 priority and exact packet; command11 independent sequence/checksum; reconnect revokes feedback. Evidence `outputs/phase238/feedback-check.json`. These validate host behavior, not sensor throughput or application acceptance.

## K01634
Source `D0302`, original line13, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

## Phase238 stopped; Phase239 scheduler repair active

## K01635
Source `D0302`, original line15, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

One exclusive <=180s attempt started using freshmode1/N1 proof, fixedphase267620 (reverification, no scan), measured sampleperiod4.939977608ms and existing guarded19 admission. Native DEV recording retained. Hardware acceptance: actual decoded samples/counter gaps, continuous optical/IMU pairs, calibration verification and fresh native200poses; transport ACKs alone do not count.

## K01636
Source `D0302`, original line17, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

238 stopped early after75.8s by SIGINT to its verified runner PID. Actual1,634IMU/2,893countergaps/460feedbackpublications,942opticalmeasurements. Optical age21.841s with RF alive; native10phaseACK count froze at39 and busy11 count514. Source-proven integration error: StationOptics.poll skipped ALL pendingcommands, so continuous11 occupied every opportunity to enqueue due10. Existing queued10 priority alone was insufficient. AllfourcleanupTRUE, power300→5. Evidence archived `outputs/phase238-evidence.tar.gz`; no replay. Recorded alignedcounter pattern changed:delta1=479,delta2=508,delta5=443,delta7=57; this demonstrates native selection changed, not continuous fusion/calibration success.

## K01637
Source `D0302`, original line19, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

239 changes ONLY the scheduler admission for due opticalsync: allow planning/queuing one sync while pendingcommand11 waits forACK; any alreadyqueued work or other pendingcommand still blocks duplicate planning. ACK11 retires normally, existingqueue dispatches10next. No overwriting pending11 or mixedpacket; exact8Btimed10 preserved. Focused host queue/sequence checks passed.239 new exclusive180s started with same current userreadiness and freshHMD/controllersOFF runtime preflight; noOTA/noRFparameterchange/no scan. Do not repeat238.

## K01638
Source `D0302`, original line21, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

239 COMPLETE/allfourcleanupTRUE, screen timeout restored300→5.9,027IMU/16,949countergaps (counter-span fraction34.75%, NOT physicalpacketloss percentage),1,853feedbackpublications; native10ACK175 confirms starvation corrected.3,447actualopticalpubs,8,515alignedIMUcandidates; sync400verifiedspread0.571ms. Hold130.187s fresh2008,062/12,512=64.4341%,visual12,252/12,512=97.9220%; maxfreshgap4.572s. Noaxes/finalphysicaltime/fusionverification. FullnativeDEVvideo20fps: `F:/Shlik/Desktop/PICO-SteamVR-counter-feedback-2026-10-10.mp4`; notsmoothfullbodydemonstration. Evidencearchive`outputs/phase239-evidence.tar.gz`.

## K01639
Source `D0302`, original line23, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

## Phase240 offline calibration identifiability

## K01640
Source `D0302`, original line25, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

Native feedback changes deliveredsamplepattern but maximum contiguouscounterrun19.761ms, ZERO oldcalibrator pairs for all51timeoffsets. Existing calibration requires continuouscounter/<=8ms across20..200ms opticalpairs and >=300ms continuous quietgravitywindows; recording cannotmeetthesecriteria. No gap interpolation/guard weakening allowed.

## K01641
Source `D0302`, original line27, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

Alternative identifiability DIAGNOSTIC only: angular rate from local five ACTUAL opticalorientations (80..160ms span, adjacent<=40ms, local angularfitresidual<=0.01rad), compared to NEAREST actual gyro sample<=2.5ms, no gyrointerpolation.2,703derivatives; bestoffset−11ms/656pairs, heldoutmedian0.0891rad/s/p90.2111. Unconstrainedfit singularvalues1.901/.310/.100 and negative determinant−.0588: **REJECTED**, not a proper measuredrotation/scale. A forcedproperrotation from this unreliable fit mustneverauthorizefusion.

## K01642
Source `D0302`, original line29, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

Gravity diagnostic:874actualquietpairs, properrotationfitmedianangle0.584deg/p90.1.373; documentedprior2.623/3.325deg. Opticalgravity centeredexcitation singularvalues.302/.206/.00316, nearlyconstant orientation; this cannot independently determine allaxes. These calculations do not certify missing-sample periods as stationary.

## K01643
Source `D0302`, original line31, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

Results: `outputs/phase240/sparse-axes-diagnosis.json`, `gravity-diagnosis.json`; scripts240. Next implement explicit sparse-observation calibration candidate with actualsample pairing, sufficient three-axis excitation, properrotation/scale, heldoutresidual and independent measuredgravity checks; retain original inertial propagation/sourceage/gap/uncertainty guards. Verify candidate on existingrecordings (must reject weak239) before one purposeful strongthree-axis gesturecapture. Do not rerun unchanged238/239 or switch to documentedaxes bydefault. Currentbodytrackinggoalremainsunfulfilled; user asked notto stop.

## K01644
Source `D0302`, original line33, SHA256 `550e3ce4cb4978b251faa01d0e6b7171de50ca69ec180b71dcf5eddc714903d5`. Status: historical/unadjudicated narrative; English source prose.

Do not repeat consumed attempts or relax gates. Check allfourcleanup and power restoration before further RF. Final body tracking still requires verified axes/physicaltime/fusion, multiple tracker roles and controller coexistence; persistent visual201 is never fresh measured tracking.

## K01645
Source `D0303`, original line1, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

# Phase242 completion (supersedes active state below)

## K01646
Source `D0303`, original line3, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

Hardware run completed180s. All four cleanup flags TRUE; RF closed; temporary power300 restored5. Actual9078 IMU,17040 counter gaps,1731 native11 feedback publications,3048 station optical measurements /3047 publication-log records. Native phase ACK173 confirms optical planning priority remained functional.

## K01647
Source `D0303`, original line5, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

Calibration refused:254 matched pairs; held-out proper angular-rate RMS0.418707rad/s and p950.712201rad/s; unconstrained singular scales0.949470/0.782756/0.528791. Zero admissible stationary gravity windows. Axes and physical time not verified; no IMU fusion. Driver actual fresh20058.5092%, visual200+20198.3037%, maximum freshgap5.805s. Last-known visual model is not body tracking.

## K01648
Source `D0303`, original line7, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

Evidence outputs/phase242/optical-pass/attempt/{radio-live.json,cleanup.json,power-restored.json,driver-analysis.json,optical.jsonl}. Follow-on offline analyses and next justified measurement in PHASE243_245_STATIC_AXIS_MEASUREMENT_2026-10-10.md.

## K01649
Source `D0303`, original line9, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

# Phase241–242: explicit sparse-observation IMU calibration

## K01650
Source `D0303`, original line11, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

## Why the calibration method changed

## K01651
Source `D0303`, original line13, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

Phase239 exact native11feedback and opticalsync priority correction produced9,027actualIMU and3,447opticalpubs. But maximum consecutivecounterrun19.761ms and ZERO oldcontinuous-integral calibrationpairs at all51offsets. Further identicalmovement captures with that method are unjustified. Native8slotring/getter chooses at mosttwo records, so missing historicalsamples do not alone prove RF packetloss.

## K01652
Source `D0303`, original line15, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

New explicit method `sparse_observations_v1` estimates axes from sparse **actual** gyro observations against measured optical angular derivatives. It does not reconstruct missing counters or interpolate gyros. It is selected only in242opticalconfig; the oldcontinuous method remains the default. Inertialpropagation/sourceage/clock/counter/covariance gates are unchanged.

## K01653
Source `D0303`, original line17, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

## Admission requirements

## K01654
Source `D0303`, original line19, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

- Optical localderivative uses5actualorientations spanning80..160ms, adjacentgap<=40ms, local angularfit residual<=0.01rad.
- Gyro matching is nearest ACTUAL sample within2.5ms; n>=240. Physicaltimeoffset search±25ms; boundarywinner rejected.
- Independent temporal70/30train/heldout blocks; three-axis excitation in BOTH (second singularvalue>=3, third>=1.5).
- Unconstrainedfit positive determinant and allthree scale singularvalues within3% of1. Proper rotation used for validation, not simply projected badmatrix accepted.
- Heldoutproper angularrate RMS<=0.05rad/s, p95<=0.1rad/s; measuredgyro bias<0.05rad/s.
- At leasttwo disjoint observedgravitywindows, directions separated>=15degrees. One-secondwindows need>=20actualIMU and>=15actualoptical observations spanning>=800ms; IMUgap<=50ms/opticalgap<=70ms. All observedgyro<0.05rad/s; all observed optical orientations within0.02rad. These are observedstationaritychecks, not missing-sample integration.
- Everygravitywindow scale ratio0.97..1.03 and directionerror<=5deg. Missing/insufficient evidence refusesfusion.

## K01655
Source `D0303`, original line27, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

No fabricated angular residual is logged as measured: integral`residual_rms_rad` is null for thismethod; actualheldout angularrate metrics are explicit.

## K01656
Source `D0303`, original line29, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

## Phase241 offline validation

## K01657
Source `D0303`, original line31, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

Actualweak239recording MUSTreject and did: badunit-scale fit, insufficient3axisexcitation, heldoutresidual, insufficientdistinctgravityorientations and gravitymismatch. No default/documentedaxes accepted.

## K01658
Source `D0303`, original line33, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

Necessary mathematical recovery case with explicitknownproperrotation, bias[2,-3,1]counts, timeoffset−2ms, syntheticthree-axisopticalmotion and missingcounterpattern1/2/5: axeserror0.000113715rad(0.006516deg), recoveredoffset−2ms, bias[1.99199,-2.99059,.99472], scale singularvalues.999544/.999424/.999214,6quietwindows separated57.3deg. This validates estimatorrecovery and rejection; **not hardware proof**. Evidence `outputs/phase241/sparse-replay.json`, `sparse-contract.json`. Source `pc/optical/sparse_imu_calibration.py`.

## K01659
Source `D0303`, original line35, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

## Phase242 active; result and cleanup pending

## K01660
Source `D0303`, original line37, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

User explicitlyready for one<=3minute calibration with20–30deg three-axis rotations/repeatedcycles and2second opposingtilt holds.242exclusiveattempt started afterverified239closure. Freshmode1/N1, fixed267620/no scan, native19gates, native11feedback/schedulerpriority and protectedRoymodule/RF preserved. NoOTA. Signal user when actualIMU starts; do not keepuserwearingHMDduringofflinework. Native DEV recording preserved.

## K01661
Source `D0303`, original line39, SHA256 `25b735a8e5d0645321c3fafe36c4cfeba2bd7174c163c5cefeae6c0f3a93cc50`. Status: historical/unadjudicated narrative; English source prose.

Success requires actualhardwarecalibrationverified and applied, correctly moving fresh SteamVR200poses with IMU evidence, then controllercoexistence/bodyroles. Weakfitrejection is not success. Everyrun freshmode; no unstructured sweeps or consumedattempt replay.

## K01662
Source `D0304`, original line1, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

# Phase245 completion supersedes active heading below

## K01663
Source `D0304`, original line3, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

All four cleanup true, power restored5, RFclosed.6668IMU/2587opticalpubs.6quietwindows onlyonegravityorientation; noaxes/time/fusion. Next independentstereo path and detailedactualcoverage in PHASE246_249_CALIBRATED_STEREO_2026-10-10.md.

## K01664
Source `D0304`, original line5, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

# Phase243–245: diagnose derivative fit and measure stationary sensor axes

## K01665
Source `D0304`, original line7, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

## Phase243: offline diagnosis, no hardware operation

## K01666
Source `D0304`, original line9, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

Full242 replay of linear five-frame derivative rejected:252 pairs, offset−15ms, proper held-out rate RMS0.3857rad/s, unconstrained scales0.871/0.828/0.521. Cubic derivative reduced smoothing attenuation but amplified optical noise:507 pairs, offset+1ms, RMS0.8218rad/s, p951.8163rad/s, scales0.960/0.849/0.824, excessive bias. Both have zero stationary windows. Cubic estimator was not deployed; source/calibration admission unchanged.

## K01667
Source `D0304`, original line11, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

Closest actual one-second quiet window still has gyro maximum0.2108rad/s versus required0.05, and optical orientation swing0.06162rad versus0.02. Constant-rate diagnosis found1395 optical windows, only3 with estimated acceleration<0.5rad/s²,33 with actual gyro variation<0.1rad/s, ZERO satisfying both. Do not repeat handheld oscillations or force/project these fits into accepted axes.

## K01668
Source `D0304`, original line13, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

Evidence outputs/phase243/{derivative-replay.json,quiet-diagnosis.json,constant-rate-diagnosis.json}. Scripts outputs/phase243_*py. These are data diagnoses, not physical tracking proof.

## K01669
Source `D0304`, original line15, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

## Phase244: exact installed sensor provenance and separate static analyzer

## K01670
Source `D0304`, original line17, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

Installed98868-byte image SHA040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d verified before disassembly. A7F8 sends SPI read0xF5 (WHO_AM_I register0x75), caches byte atA862. AB46 accepts0x47; AC88 accepts0x44 and sets sensor class0. AADC writes register0x4F/0x50 throughAA30; class0 uses requested sensor-mode byte unchanged. AD08 reads14 bytes starting0x1D and AD7C..ADD4 endian-swaps native accelXYZ/gyroXYZ into radio sample without axis permutation or scaling.

## K01671
Source `D0304`, original line19, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

Official TDK ICM42686-P datasheet (original reference: `https://d17t6iyxenbwp1.cloudfront.net/s3fs-public/2026-01/DS-000639-ICM-42686-P-v1.0.pdf?VersionId=Vini1jOm6111UWYHkdtrQQzpdIb9HxvK`) ID0x44; GYRO_CONFIG0 0x4F and ACCEL_CONFIG0 0x50 select±4000dps and±32g for upper bits000, ODR200Hz for low nibble7. Static inference conditional on observed mode07 and no later register override; this does not substitute for measured axes/time. Disassembly outputs/phase243/sensor-installed-disasm.txt contains explicitly bounded code regions, not encrypted vendor container.

## K01672
Source `D0304`, original line21, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

Offline analyzer outputs/phase244_static_axes.py uses disjoint1s actual quiet windows (20IMU/15optical, spans800ms, gaps50/70ms, all correctedgyro<0.05rad/s, observed optical excursion<0.02rad, individual acceleration normwithin3%). Estimates independent gravity means in first/second halves, proper rotation from first halves, validates second halves≤5deg/scale3%. Requires≥3 observed gravity directions separated15deg and second direction singularvalue≥0.2. Reports static axes/bias only; physical time and fusion authorization always FALSE. Existing242 mustreject and does: zero windows.

## K01673
Source `D0304`, original line23, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

## Phase245 active: result and cleanup pending

## K01674
Source `D0304`, original line25, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

User confirms stable surface possible and HMD/PICO ready/controllers off. One exclusive150-second candidate, physical capture protocol changed to10-second untouched holds at baseline and approximately±30degrees. Signal user only after actual IMU starts; record real stability rather than assume instructions were followed. Same installed firmware, protected2043-byte RAM module,6000/900 RF, fixedphase267620/no scan, native11 feedback and optical queue priority; fresh mode1 required. Existing calibration/source/propagation gates remain. No OTA. New static fit will be reviewed offline; no auto-fusion from gravity-only fit.

## K01675
Source `D0304`, original line27, SHA256 `7b974c83c5f0608128e01713a8a520f92d09c109f074863466f1de79c033f413`. Status: historical/unadjudicated narrative; English source prose.

Fullbody remains unfinished: physical axes/time and useful dynamic fusion, healthy controller coexistence, additional trackers and role assignment. Do not claim smoothness from result201 visual retention. Every attempt must close/restore all four components and power5 before another hardware run.

## K01676
Source `D0305`, original line1, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

CURRENT252 OFFLINE:249 CLOSED early at50.1s; fixed-phase verification11lit/52decoded FAILED, IMU19neverstarted/samples0. All4cleanupTRUE, RFclosed, power restored5; noOTA.249freshmode1/N1 and two commanded-dark temporal identity confirmations were actual,264publication logrecords; these do not establish smoothtracking/fullbody.250/251 OFFLINE actual249native15Hz619stereopairs:298accepted (20three-LED fresh-prior continuations), compute p50/p95/max3.915/10.283/71.210ms; newsolver NOTdeployed.252originaldecoded pixels prove both-camera near-black runs (max3/255 in several frames), followed by gradual brightness recovery around repeated native10ACKs. Fixed control interval has13/30pairs withzeroLED blobs. Camera frame-index fit33.332383ms/index,maxresidual2.802us; RAW-MONO replaybridge364actualmatches,maxresidual6.256us. Clock/phase causality stillUNDERINVESTIGATION, notproven; no unchangedRF/phase sweep/gateweakening. Need source-backed timing repair before newpurposefullivecapture. Static axes stillonlyTWOgravitydirections, physicaltime/dynamicfusion/controllercoexistence/multitrackerroles unverified. UserDONOTSTOPuntilbodytracking; HMDcanberemovedduringofflinework. Read docs/PHASE250_252_STEREO_TIMING_DIAGNOSIS_2026-10-10.md. OlderCURRENTentrieshistorical.

## K01677
Source `D0305`, original line3, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

# Phase249 ACTIVE supersedes prepared headings below

## K01678
Source `D0305`, original line5, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

Userready; one150s exclusivehardwareattemptstarted, freshmode1/N1verified. IMUstart/result/cleanup pending. Wait for actualsamples before movements, then baseline10s/twodistincttilts10s. NoOTA/no scan.

## K01679
Source `D0305`, original line7, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

# Phase246–249: recover optical poses from partial stereo constellations

## K01680
Source `D0305`, original line9, SHA256 `70d2ab08dead0205d49a8e46ed3536a4ed54a6698b4898431a92fd0773e7014a`. Status: historical/unadjudicated narrative; English source prose.

## Hardware245 completed
