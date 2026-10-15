# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K00881
Source `D0231`, original line11, SHA256 `71a3902727621ad2bd77f6684490b73302916c7f87a88add82d4251a42ab8270`. Status: historical/unadjudicated narrative; English source prose.

Actual N1 proof body fe021223922268d8d863e8ecd031d45559823eed197901070100; nonce is captured in original protocol state. Capture host timestamp1791567688549794685ns is host read time, not RF arrival.

## K00882
Source `D0231`, original line13, SHA256 `71a3902727621ad2bd77f6684490b73302916c7f87a88add82d4251a42ab8270`. Status: historical/unadjudicated narrative; English source prose.

All session/RAM/MPU/trace restoration checks true. Evidence outputs/phase116/attempt, frozen invocation/release/tests. The shared-radio game117 candidate is separate: sameN1/6000 from bootstrap to stream, no reconfiguration toN0, strict controller/window gate before19. At this record's writing it is running and awaiting/measuring two controllers; no joint IMU result yet.

## K00883
Source `D0232`, original line1, SHA256 `96911a508e4ef03486d7c136f22c1c1b5eff821af1a4125031e0f8f7472f16f2`. Status: historical/unadjudicated narrative; English source prose.

# Phase117: N1 joint keepalive milestone

## K00884
Source `D0232`, original line3, SHA256 `96911a508e4ef03486d7c136f22c1c1b5eff821af1a4125031e0f8f7472f16f2`. Status: historical/unadjudicated narrative; English source prose.

Hardware2026-10-09, tracker FW189/revision07 unchanged; Frame game module2043B, codeb99bdabaa6871cc48393a2e160142d635d96277ef1354a5a7db4130c3d6a8520. No OTA.

## K00885
Source `D0232`, original line5, SHA256 `96911a508e4ef03486d7c136f22c1c1b5eff821af1a4125031e0f8f7472f16f2`. Status: historical/unadjudicated narrative; English source prose.

New mechanism: fresh FrameN1/6000 proof from bootstrap, same6000us cadence through S1 and prospective S4; no N0 reconfiguration. Existing540us/600sample admission and strict guard before19 retained.10offline checks passed. Initial preflight attempt had0slots and rejected stale Native101 code whitelist inherited from local112 loader; corrected to actual113 image hash, original rejected release preserved remotely. It is not an RF result.

## K00886
Source `D0232`, original line7, SHA256 `96911a508e4ef03486d7c136f22c1c1b5eff821af1a4125031e0f8f7472f16f2`. Status: historical/unadjudicated narrative; English source prose.

Retry: S1 at6.36790s, S2 at43.95298s. S2 approx8s,1351completed TX,264retained replies,IMU0,1077host ring drops. Controller metrics at final S2: both99pose changes/99samples,0invalid/0freeze. No claim of healthy entire bootstrap/transition.

## K00887
Source `D0232`, original line9, SHA256 `96911a508e4ef03486d7c136f22c1c1b5eff821af1a4125031e0f8f7472f16f2`. Status: historical/unadjudicated narrative; English source prose.

Header CRC counter at S2 transition slot6591 was2558; final slot7943 CRC3903. Delta1345CRC compared with staged1351TX_END givesapproximately99.5559%. Boundary attribution/counters are sampled sequentially, so this is not an exact packet ledger. It demonstrates a short joint keepalive reception improvement; no IMU19 was sent. Huge ring loss makes retained delivery far lower than RF capture.

## K00888
Source `D0232`, original line11, SHA256 `96911a508e4ef03486d7c136f22c1c1b5eff821af1a4125031e0f8f7472f16f2`. Status: historical/unadjudicated narrative; English source prose.

26observer calls: ordinary20calls/640raw/588accepted; paused6calls/192raw/190accepted. Ordinary3000fold locally covered600us quiet;6000fold900us; whole-period bin coverage incomplete. The legacy minimum600ordinary samples rejected before any phase/19. Do not claim admitted window, sensor delivery or finished game tracker.

## K00889
Source `D0232`, original line13, SHA256 `96911a508e4ef03486d7c136f22c1c1b5eff821af1a4125031e0f8f7472f16f2`. Status: historical/unadjudicated narrative; English source prose.

A possible next implementation removes paired diagnostic calls so all normal rows contribute, and evaluates only a candidate quiet run with UNKNOWN BINS BUSY. Candidate bins must each have enough observations independently in chronological segments; sample minimum600, busy<=5%, width>=540us and strict controller guard remain. The old whole-period check should remain unchanged for other profiles. This design must be tested against missing-window bins and transient busy segments before hardware; not approved by this report.

## K00890
Source `D0232`, original line15, SHA256 `96911a508e4ef03486d7c136f22c1c1b5eff821af1a4125031e0f8f7472f16f2`. Status: historical/unadjudicated narrative; English source prose.

Evidence outputs/phase117/retry/attempt,analyze.py,analysis.json,evidence-sha256.json; exact game source/releases and10test results. All four cleanup checks true. Target98–100% real sensor delivery remains unmet.

## K00891
Source `D0233`, original line1, SHA256 `2603884e77662c95419ee31bed6af978844eb59ba35c53715f9634d0d86f9a6a`. Status: historical/unadjudicated narrative; English source prose.

# Full6000us phase discovery — Phase118/119

## K00892
Source `D0233`, original line3, SHA256 `2603884e77662c95419ee31bed6af978844eb59ba35c53715f9634d0d86f9a6a`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. Material offline discovery backed by a new joint hardware capture. Tracker firmware remainsFW189/revision07; no OTA.

## K00893
Source `D0233`, original line5, SHA256 `2603884e77662c95419ee31bed6af978844eb59ba35c53715f9634d0d86f9a6a`. Status: historical/unadjudicated narrative; English source prose.

Phase118 introduced candidate-only admission: unknown/underobserved bins are busy; each admitted bin requires>=5observations and<=5%busy independently in3chronological segments;600ordinary sample floor,540us width and strict controller guard retained.15offline tests passed. Joint run rejected at S2 with No covered540us window in all segments;IMU0. All cleanup true. Raw data retained at outputs/phase118/attempt.

## K00894
Source `D0233`, original line7, SHA256 `2603884e77662c95419ee31bed6af978844eb59ba35c53715f9634d0d86f9a6a`. Status: historical/unadjudicated narrative; English source prose.

Source inspection identified that N1 polls already repeat every6000us while the MCU phase correction still used3000us modulus12288RTC fractional units. The3000fold aliases the two halves of a6000cycle. This is a static design mismatch, not proof by itself of past hardware failures.

## K00895
Source `D0233`, original line9, SHA256 `2603884e77662c95419ee31bed6af978844eb59ba35c53715f9634d0d86f9a6a`. Status: historical/unadjudicated narrative; English source prose.

Reprocessing EXACT Phase118 ordinary capture with the same3segment/5observations/5%busy/600sample policy but6000period/60bins yields765accepted ordinary rows and a700us covered window, referenceRTC6648443, centerrelative3150us. The3000policy rejected that same recording. This is OFFLINE evidence; the old RTC anchor must not be reused after reconnecting controllers.

## K00896
Source `D0233`, original line11, SHA256 `2603884e77662c95419ee31bed6af978844eb59ba35c53715f9634d0d86f9a6a`. Status: historical/unadjudicated narrative; English source prose.

Phase119 changes MCU modulus to24576fractional RTC units (6000us), centered threshold12288; host PhaseLock and covered-window period also6000us/60bins. Same6000poll distance and FrameN1, unchanged radio deadline and guards.16checks passed:15host/firmware proof tests and one exact compiled-module emulation proving a sample in the second3000us half is distinguished. Loaded code still2043B. Windows Unicorn native-diagnostic limitation remains as previously documented.

## K00897
Source `D0233`, original line13, SHA256 `2603884e77662c95419ee31bed6af978844eb59ba35c53715f9634d0d86f9a6a`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase119/source/build,prepare_phase_test.py,test_mcu_phase.py,tests.xml and phase-test-output.txt; Phase118 raw dataset. Live119 joint stream is NOT yet measured at this record's writing; no99% sensor or game-pose claim. A fresh119 capture must admit its own window before19.

## K00898
Source `D0233`, original line15, SHA256 `2603884e77662c95419ee31bed6af978844eb59ba35c53715f9634d0d86f9a6a`. Status: historical/unadjudicated narrative; English source prose.

## Subsequent hardware result: first joint sensor stream

## K00899
Source `D0233`, original line17, SHA256 `2603884e77662c95419ee31bed6af978844eb59ba35c53715f9634d0d86f9a6a`. Status: historical/unadjudicated narrative; English source prose.

Phase119 subsequently ran on the target with both controllers enabled. Fresh ordinary capture admitted a 700 us window on the 6000 us period: 832 raw rows, 768 accepted, 64 excluded; reference RTC 1904245, center 1050 us, target start 800 us. Initial tracker response verified mode=1, revision=7, applied N=1. Installed tracker image remains 97,832 bytes, SHA256 40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5; no OTA.

## K00900
Source `D0233`, original line19, SHA256 `2603884e77662c95419ee31bed6af978844eb59ba35c53715f9634d0d86f9a6a`. Status: historical/unadjudicated narrative; English source prose.

S4 started at elapsed 61.819143 s and the run ended at 64.463204 s: approximately 2.644061 s of joint IMU streaming. Recorded 306 completed transmissions, 263 retained replies (85.9477%), 239 unique IMU samples (approximately 90.39 samples/s), and 33 ring drops. Hardware CRC counter increased from 4500 to 4787: approximately 287/306=93.7908% CRC/TX; sequential stage-boundary reads make this an approximate ratio, not an exact synchronized denominator. Scheduled-slot delta was 550. Sample gaps=258 are not an independently established loss denominator.

## K00901
Source `D0233`, original line23, SHA256 `2603884e77662c95419ee31bed6af978844eb59ba35c53715f9634d0d86f9a6a`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase119/attempt/radio-run.json and the accompanying raw capture/cleanup. Further hypothesis: after phase anchoring, EARLIEST retries can reserve HIGH-priority off-phase slots even when the MCU suppresses transmission. Investigate phase-preserving NORMAL retries using measured elapsed time relative to the preceding START; do not repeat Phase107's unqualified retry disable or relax the strict guard.

## K00902
Source `D0234`, original line1, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

# Phase119 motion control and Phase120 preparation

## K00903
Source `D0234`, original line3, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. Hardware evidence and offline preparation; no OTA.

## K00904
Source `D0234`, original line5, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

The user confirmed both controllers OFF for bootstrap, then both ON and physically moving. The exact Phase119 MCU image was reused intentionally to isolate the stationary-pose confound. ELF SHA256 4a386160076ec4284c6f93086fd568360e837304c9d5411877a18e2b063b58ef, loaded code2043bytes/SHA2564020c351db7e62fc68ef77663695f3f53c7e583bc51fd3e78cd3113bc215b299. Target firmware189/custom07 remains unchanged. Initial live response verified mode1.

## K00905
Source `D0234`, original line7, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

The motion control reached S1 at14.523541504s, S2 at29.983341666s, then stopped at38.229944806s because the fresh three-segment6000us fold had no covered540us window.755ordinary rows were accepted. Longest covered run was five100us bins=500us, starting at bin40. S2 had1374completedTX,264retained replies,1108ringdrops,IMU0; this is not a sensor-delivery percentage. Last99controller samples for each controller were valid and changing, maxfreeze0. This does not resolve Phase119's earlier151.748ms S4 guard trip because the motion control never reached S4. Session/RAM/MPU/trace restoration alltrue.

## K00906
Source `D0234`, original line9, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase119/attempt-motion including radio-run.json, cleanup.json, controller-metrics.jsonl; analyze_motion.py reproduces window counts/rates and writes analysis-motion.json for both attempts. Previous239IMU milestone remains valid but does not establish sustained coexistence.

## K00907
Source `D0234`, original line11, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

Historical review before Phase120: SERVICE_FAST_POLL_ITERATION26 and27 document actual stock request eligibility. NORMAL is accepted only in native state2 (byte0x20010592); state1 NORMAL returns-35. Therefore Phase120 attempts phase-preserving NORMAL retries only while anchored and state2; state1 retains EARLIEST recovery. It retains API-error stop and low-priority actionNONE. This does not fully eliminate off-phase state1 re-anchors.

## K00908
Source `D0234`, original line13, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

First Phase120 compilation overflowed reservedMODULE by58bytes. No Phase120 ELF was admitted or deployed, and no Phase120 RF ran. Source/candidate retained in outputs/phase120. Host sampler rejects paused operation5; candidate supports ordinary operation4 only to reclaim unused diagnostic writes. Do not bypass memory bounds or load the overflowing source.

## K00909
Source `D0234`, original line15, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7-medium review of Phase118 was received. Admission itself was fail-closed, but its exported profile could show low busy rates for bins with1-4samples. Verified Phase119 already includes enough-mask and worst[~enough]=1, correcting that reporting issue. Remaining review limits: chronological thirds are count-based;5%busy is an aggregate allowance; strict guard arms before19 rather than during all transition samples. No external verdict approves Phase120.

## K00910
Source `D0234`, original line17, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

## Phase121: user-authorized bench observation policy

## K00911
Source `D0234`, original line19, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

The user explicitly requested removing the unchanged-pose automatic stop because it interrupts bench research. Phase121 retains EXACT119MCU/ELF and firmware07; only its isolated host release/config changes. Unchanged pose is recorded through existing per-second metrics but freeze_limit_ms=None prevents that condition alone from ending the session. Actual controller/HMD invalidity or disconnection retains the existing0.5s stop, and watchdog/MCU fault/slot limits remain. This is diagnostic authorization, not game-quality acceptance.

## K00912
Source `D0234`, original line21, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

Phase sampling is16s instead of8s, still6000us and600ordinary floor/three chronological segments/5samples per bin/5%busy/540us width. Transition grace is0; strict validity is armed immediately upon enteringS2, so observation time does not extend the unguarded transition. S2 timeout25s, entire session110s. Fresh window admission still precedes19. Three targeted tests pass: unchanged pose does not stop bench but does stop original150ms policy; missing controller/dead watchdog rejected; altered grace rejected. Artifacts outputs/phase121/test_bench_policy.py and isolated config/release. No modified tracker firmware or expanded RAM.

## K00913
Source `D0234`, original line23, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

### Follow-up policy and rejected first121 attempt

## K00914
Source `D0234`, original line25, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

First121attempt stopped during transition because two controllers were connected but one had not yet acquired valid tracking. Both became valid shortly after cleanup; startup delay versus interference is unresolved. No19/IMU. Initial release preserved as outputs/phase121/release-initial and raw attempt retained. This exposed the separate invalid-pose stop, not the removed unchanged-pose stop.

## K00915
Source `D0234`, original line27, SHA256 `a2a365ad79746a25b775bf6419ec4e556417ef19ebefc2f69b34484fd42fffdf`. Status: historical/unadjudicated narrative; English source prose.

At the user's explicit request to keep bench research continuous, the isolated121release now provides ControllerGuard mode'observe': fresh monitor rows are required, but invalid/disconnected/unchanged controller states are logged rather than terminating the session. Stale monitor (>0.5s), monitor process failure and Nordic/slot faults remain stops. Observe is armed atS2, and the110s session remains bounded. This supersedes the strict-validity bench policy described above; original non-bench strict policy remains intact. Four targeted tests pass, including actual invalid controller metrics increment without abort and stale-monitor rejection. New unique attempt-observe does not overwrite initial attempt. Any controller impairment still FAILS game-quality acceptance even though the diagnostic run continues. phase window/firmware proof/CRC gates are unchanged.

## K00916
Source `D0235`, original line1, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

# Phase122 joint bootstrap bench experiment

## K00917
Source `D0235`, original line3, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. User explicitly requested continuous bench research without controller health auto-stop/RAM reset. Controllers were left ON after delayed confirmation caused121S1 timeout. Initial121 observe attempt is preserved, cleanup alltrue; it never entered jointS2.

## K00918
Source `D0235`, original line5, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

Phase122 removes the physical OFF/ON bootstrap cycle for this bounded diagnostic only. Exact119MCU remains2043bytes, code SHA2564020c351db7e62fc68ef77663695f3f53c7e583bc51fd3e78cd3113bc215b299; ELF4a386160076ec4284c6f93086fd568360e837304c9d5411877a18e2b063b58ef. Tracker remains FW189/custom07, installed97832bytes/SHA25640941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5. NoOTA.

## K00919
Source `D0235`, original line7, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

Isolated host opt-in requires both top-level and staged bench_joint_bootstrap=true plus bench_pose_observation=true. Exact module whitelist,6000/N1, nooptics,noOTA, slotcycle1, initial900us/RX700, subsequent400us/RX340 and phase coverage gates remain. Controller health is observed from startup; watchdog/MCU/slot errors remain stop conditions. BootstrapS0 remains bounded25s, phase sample16s/timeout25s, entire run110s. This deliberately permits temporary controller impairment and is not production game acceptance.

## K00920
Source `D0235`, original line9, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

HARDWARE milestone: initial stock monitor had BOTH controllers connected/valid with HMDvalid. Target connected and freshmode1 verified without controller toggling. Mid-run live cumulative CRC2355/TX2435=96.7146% was KEEPALIVE/IMU0, not sensor delivery.1534ringdrops and810retained replies show substantial observer-stage host backlog. Controller1 unchanged pose14248.6373ms while valid; controller2 changing, both last99rows invalid0. Observation continued, establishing the requested no-reset behavior. Final result pending; never reuse the measured phase anchor in another connection.

## K00921
Source `D0235`, original line11, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase122 isolated source/build/release/config/invocation/test_bench_policy.py;4targeted tests pass. Raw attempt will be pulled after completion.

## K00922
Source `D0235`, original line13, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

## Final hardware result and pause

## K00923
Source `D0235`, original line15, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

Run completed at110.036335900s. S0->S1 at2.781823088s, S2 at4.897677482s, S3 at20.967621757s, S4 at24.040148585s. Fresh window600us, referenceRTC11735697, center1800us, targetstart1600us;1632ordinary raw rows,126excluded,1506accepted. Final recent phase-error maximum16us, last-8us. No controller toggling was needed during this run.

## K00924
Source `D0235`, original line17, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

Entire-run sequential counters: **18251 CRC-valid replies /18371 completed TX =99.3467966%**;7643host ring drops;10608retained unique answered slots /18371TX=57.7432%. **IMU=0**. This is successful high-reliability KEEPALIVE transport, not99% sensor delivery or full-body tracking.

## K00925
Source `D0235`, original line19, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

S4 lasted85.996187315s:14330completedTX,9260retained replies=64.619679%,5033ringdrops,IMU0. An exact S4 CRC/TX ratio cannot be reconstructed from the report's last1000header samples: the S4 start atslot4036 is no longer in that retained header subset. Never divide tail CRCdelta by the full S4 TX count. Raw live records remain available for further analysis. Final99controller metric rows for both devices were valid and changing, maxfreeze0; controller1 had prolonged unchanged-but-valid pose earlier. Whole-run health must be assessed from all controller-metrics.jsonl, not only that final window.

## K00926
Source `D0235`, original line21, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

Command19 was queued at24.040145304s/slot4036; transport ACK total increased to5. This is not proof that the tracker executed the application handler. Native command19 normally sets both IMU enqueue and autonomous TX gates, and119previously produced239IMU. Why122did not remains UNKNOWN. Candidate investigations: original native record sequence dedup across reconnects, application dispatch versus RF ACK, setup commands17/25 and emitted mailbox publication. No dedup cause has been proven and no further RF trial was run after the pause request.

## K00927
Source `D0235`, original line23, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

Cleanup session/RAM/MPU/trace alltrue. Post-cleanup stock monitor showed HMD and both controllers connected/valid. User requested pause; work is paused. Tracker firmware07unchanged,08notflashed,pose_valid=false. The98-100% joint sensor goal remains unmet.

## K00928
Source `D0235`, original line25, SHA256 `745ac61ef0b59168fa1efd71ff2df8a7e5ecb444e7aad2695868af5742ec5506`. Status: historical/unadjudicated narrative; English source prose.

Evidence pulled locally: outputs/phase122/attempt/radio-run.json, radio-live.json, radio-live-records.jsonl, controller-metrics.jsonl, cleanup.json, manifest/recovery/invocation/config. analyze_result.py reproduces final stage/transport numbers and explicitly marks unavailable S4 CRC ratio. Phase120 remained unrun:58B initially,50B after arithmetic optimization; LTO outliner still50B, -Os324B overflow. Grok4.7-medium size review timed out after630s without a verdict; this was not a reported quota limit.

## K00929
Source `D0236`, original line1, SHA256 `db5888869c6014342f9b2a2aabb667f71001fe811f0691c6a1f3256cc5ab1ce9`. Status: historical/unadjudicated narrative; English source prose.

# Phase123: native command19 sequence investigation

## K00930
Source `D0236`, original line3, SHA256 `db5888869c6014342f9b2a2aabb667f71001fe811f0691c6a1f3256cc5ab1ce9`. Status: historical/unadjudicated narrative; English source prose.

Work resumed at explicit user request after Phase122 pause. No OTA or tracker firmware change.

## K00931
Source `D0236`, original line5, SHA256 `db5888869c6014342f9b2a2aabb667f71001fe811f0691c6a1f3256cc5ab1ce9`. Status: historical/unadjudicated narrative; English source prose.

OFFLINE evidence: original FW189 outer record dispatcher0x94fc and command19 handler were executed through existing QueueFirmware. Submit record sequence0/command19/body00 sets byte20000fe1=1. Clear that byte in emulator only, resubmit SAME sequence0: byte remains0. Submit sequence1: byte becomes1. This proves native duplicate-record filtering can suppress a repeated command19, not that this caused Phase122 IMU0. Hardware dedup state across reconnection has not been measured. Emulator intercepts IRQ/fixed allocation/scheduling as documented; no physical sensor/radio model.

## K00932
Source `D0236`, original line7, SHA256 `db5888869c6014342f9b2a2aabb667f71001fe811f0691c6a1f3256cc5ab1ce9`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase122/check_native19.py and native19-result.json; outputs/phase123/test_sequence19.py, one native focused test passes. Stock image hash is checked by the existing emulator. Installed tracker07 uses the original native handler, as established in original fork reports; installed firmware unchanged.

## K00933
Source `D0236`, original line9, SHA256 `db5888869c6014342f9b2a2aabb667f71001fe811f0691c6a1f3256cc5ab1ce9`. Status: historical/unadjudicated narrative; English source prose.

Prepared Phase123 is an isolated copy of122, exact2043byte119MCU/ELF. It sets command19 record sequence1 immediately before queueing, instead of fresh host engine default0. It logs every changed mailbox publication AFTER the host writer succeeds: monotonic time,slot,generation,S1,payload hex. RF transport ACK still does not prove application execution; real IMU is the outcome. No frozen-pose/invalid-controller auto-reset in bench mode; watchdog/MCU/slot faults remain, freshphase admission unchanged. No Phase123 RF yet at this record's writing. Current stock check: HMD+controller1valid,controller2disconnected. Await physical readiness before joint test.

## K00934
Source `D0236`, original line11, SHA256 `db5888869c6014342f9b2a2aabb667f71001fe811f0691c6a1f3256cc5ab1ce9`. Status: historical/unadjudicated narrative; English source prose.

## Subsequent live result and corrected sequence interpretation

## K00935
Source `D0236`, original line13, SHA256 `db5888869c6014342f9b2a2aabb667f71001fe811f0691c6a1f3256cc5ab1ce9`. Status: historical/unadjudicated narrative; English source prose.

User confirmed all devicesON.123ran and completed with cleanup verified. Final cumulative17367CRC/18370TX=94.5400%,7456ringdrops,9911retained,IMU0. Mailbox log confirms payload0113010015 (sequence1/command19/body00/checksum15), generation38, S1=3; then empty publication with samegeneration after transport retirement. Changing one sequence number did not produce sensors.

## K00936
Source `D0236`, original line15, SHA256 `db5888869c6014342f9b2a2aabb667f71001fe811f0691c6a1f3256cc5ab1ce9`. Status: historical/unadjudicated narrative; English source prose.

Further native tests establish EXPECTED-IN-ORDER sequence, separately per command: fresh command19 sequence1 is rejected; command25sequence0 followed by command19sequence0 accepts19, while command25sequence0 followed by command19sequence1 rejects19. After accepted19seq0, skipping to2 is rejected; expected1 accepts. Thus 'any fresh number is sufficient' is false. Earlier limited seq0/repeat0/seq1 test must not be generalized to arbitrary counters.

## K00937
Source `D0236`, original line17, SHA256 `db5888869c6014342f9b2a2aabb667f71001fe811f0691c6a1f3256cc5ab1ce9`. Status: historical/unadjudicated narrative; English source prose.

Phase124 prepares a bounded idempotent resynchronization:65command19 records with host sequences0..63,0. For each of64possible native expected counters, exact stock dispatcher tests prove one full0..63cycle sets gate and ends expecting0; a final0works.64tests pass in0.70s. This is protocol counter recovery, not a sweep of radio timing/channels. The native setter writes the same enabled value; noOTA/mode/firmware update. Gate state in realtracker remains unobserved until actualIMU. SameELF,phase admission,deadlines and benchpolicy retained; full mailbox log. If this still hasIMU0, do not treat mere transportACKs as execution or keep repeating sequence variants.

## K00938
Source `D0237`, original line1, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

# Phase124/125 sensor enable recovery

## K00939
Source `D0237`, original line3, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. TrackerFW189/custom07unchanged; noOTA. Same2043BMCU and6000/N1 scheduling as119/122. User bench controller health observation retained.

## K00940
Source `D0237`, original line5, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

124hardware completed:17787CRC/18368TX=96.8368902%,10243retained,7544ringdrops,IMU0;69transportACKs. Allsession/RAM/MPU/tracecleanuptrue. Fullcommand19sequencecycle did not produceIMU: dedup recovery alone is insufficient, and native19execution is not directlyobservable from ACK. Rawoutputs/phase124/attempt and mailbox-publications.jsonl preserved. Do not repeat unchangedsequence-only experiment.

## K00941
Source `D0237`, original line7, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

Originalcommand17handler8ee0 callsd500, enqueueing sensor mode request[01,07]; workerd430 dispatches tob118. b118returns immediately when requestedmode equalscurrentmode; otherwise usesnative mode configuration.125firstresynchronizes17 with65records0..63,0/body07, then19 with65records/body00. Same RFwindowsandfreshphaseadmission; no radio parameter sweep.65offline tests pass:64possible17expectedcounters and same-mode no-reconfiguration path. RTOSqueue/sensorhardware are intercepted in emulator, so physicalsensorstartup remainsunverified. Scriptoutputs/phase124/test_sensor_mode_resync.py.

## K00942
Source `D0237`, original line9, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

125started afterfreshstockHMDandBOTHcontrollersconnected/valid check. Full changedmailboxpublications logged. Completed; exact final results below. No sensor success. Keep application acceptance separate from transport ACK and CRC ratio.

## K00943
Source `D0237`, original line11, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

125 FINAL HARDWARE: 18339 CRC-valid RX / 18377 completed TX = 99.7932198%; 10609 retained replies, 7730 host-ring drops, 134 transport ACKs, zero IMU. All four restoration checks true. Target movement was requested near the end, so a full-duration movement control was not established. The 17/19 counter recovery did not restore sensor output. CRC success is keepalive transport only, not sensor success.

## K00944
Source `D0237`, original line13, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

126 single-factor control prepared: ordinary ACK instead of blind ACK, identical MCU/6000us/N1/window and recovery commands. 11 peer-engine tests passed. Stock headset and both controllers connected/valid immediately before launch. No OTA, installed tracker revision07 unchanged. Results pending.

## K00945
Source `D0237`, original line15, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

126 FINAL HARDWARE: stopped at S0 handshake timeout before the planned ACK factor was applied. 4156 CRC / 4212 TX = 98.6704653%; 1398 host-ring drops; IMU0; transport ACK0; mode unverified. All four cleanup checks true. Critical limitation: blind_ack changes only on S0->S1, which was never reached. Therefore126 is NOT an ordinary-versus-blind ACK comparison and cannot implicate ACK policy. Previous progress commentary was corrected after source inspection. Raw outputs/phase126/attempt preserved. Next work must establish bootstrap/application response state, without repeating radio timing sweeps or claiming sensor delivery from CRC.

## K00946
Source `D0237`, original line18, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

Offline candidate finding, not hardware cause: native sensor-mode worker b118 compares current byte20000442 and returns atb150 when equal. A07->09->07 transition instead traverses a750/b104/aadc and sets20001fb0=1. b104 selects native timer argument10 for07 and40 for09. One bounded emulator test outputs/phase126/test_sensor_roundtrip.py passed with hardware calls intercepted; Windows Unicorn emitted an access-violation diagnostic during initialization despite pytest pass, so this is not a clean runtime/hardware validation. No sensor roundtrip sent to hardware. Grok4.7-medium read-only review requested for119vs125 command/state differences; pending.

## K00947
Source `D0237`, original line20, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

127 prepared and launched after fresh stock headset and BOTH controllers connected/valid. Single new factor relative125: native sensor09 counter cycle then07 cycle, then19 cycle, with blindACK restored as125. Exact MCU/radio unchanged; noOTA. Nativeb104 explicitly supports09/07; b118/aadc path inspected. Grok review timed out210s, no verdict. Runtime must freshly prove targetmode/N before new commands; all source changes sealed in127 invocation hashes. Results pending; do not claim sensor recovery.

## K00948
Source `D0237`, original line22, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

127 FINAL HARDWARE:18379completedTX,17609CRC-validRX=95.8104358%;10562retained=57.4677621%;7047host-ringdrops;199transportACKs;IMU0. BothcontrollersON bootstrap; freshmode1/N1 proof; phase4700us, recentmaximumerror22us. Native17 body09 counter-cycle then07 counter-cycle then19 completed at transport level only; actual application mode changes unobservable. No sensor recovery established. Allsession/RAM/MPU/tracecleanuptrue. Grok medium review timedout210s, no verdict. Hypothesis sensor-reconfiguration alone restoresIMU rejected for this observed run, but applicationacceptance notproven. User asked to powercycle ONLY targettracker for a clean-start control; Frame/controllers remainON. Awaitphysicalreply; do not repeat127unchanged. NoOTA; tracker07 unchanged; pose_valid=false.

## K00949
Source `D0237`, original line24, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

Invocation metadata limitation: old inherited descriptive fields grid_us42000/controllers_off_required=true/hardware_started=false are stale annotations, not actual executed settings. Executed command, station-config, MCU and staged sources determine the measured6000us joint bench run. Preserve original invocation as evidence; do not silently rewrite it.

## K00950
Source `D0237`, original line26, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

## Clean tracker reboot,128-130,2026-10-09
User powercycled ONLY target; controllers remainedON.128 remote-set candidate rejected beforeRF by runtime dense-profile policy:0TX, allfourcleanuptrue.19focused current peer/hostpolicy tests passed; old bundled86/122tests had2stale assertions and were not claimed green. No need to weaken diagnostic mode guard blindly.
129 original query-only6000 diagnostic obtained fresh NON-FRAME (stock) proof and deliberately aborted in peer_engine._accept_mode; targetidentity/FW189 present.599completedTX,19CRC,9retained,2ringdrops,IMU0; allfourcleanuptrue. Shortbootstrap capture ratio is not gameplay delivery. This confirms expected post-button reset to stock; sensor start was not reached.
130 PREPARED/UNRUN: exact previously hardware-verified86 common8800us remote restore, StockN1/FrameN2,900us slot, schema2, desiredframe. Existing controllersOFF prerequisite retained. NoOTA. After fresh appliedFrame proof, return to6000/N1 joint profile; no sensor success claim yet. User asked to turn controllersOFF only for bounded restore; awaitingreply. Currentgame goal98-100%sensors withbothhealthycontrollers remainsunmet. Installedtracker07 unchanged.

## K00951
Source `D0237`, original line31, SHA256 `845f042bca47f7e0156b6f5bbd87e114b90d7160f77401502a0cdb39c63b3417`. Status: historical/unadjudicated narrative; English source prose.

130 LIVE MATERIAL MILESTONE after user target-only powercycle and controllersOFF: native common8800 remote restore freshly proves Frame mode;460uniqueIMU already received at1119scheduledslots,270retainedreplies,1485countergaps. Sensors now demonstrably produce data, unlike125/127. This is controller-OFF control, not game success, and does not isolate whether reboot, restored configuration or wide profile was decisive. Installed07 unchanged; noOTA. Preserve final counts and fresh rawproof. Follow with bounded joint6000/N1 test.

## K00952
Source `D0238`, original line1, SHA256 `505631926daf8485125cbe34ddeea18f93ae4148de83593572e56bfdd63df766`. Status: historical/unadjudicated narrative; English source prose.

# Phase130-132: clean boot, sensor recovery control and joint validation
2026-10-09. Installed target [TRACKER_SERIAL] /[TRACKER_ID]/FW189 custom revision07 unchanged. No OTA. Goal98-100% sensor delivery with TWO healthy Frame controllers remains unachieved.

## K00953
Source `D0238`, original line4, SHA256 `505631926daf8485125cbe34ddeea18f93ae4148de83593572e56bfdd63df766`. Status: historical/unadjudicated narrative; English source prose.

130 HARDWARE, controllersOFF: exact previously measured86 common8800us, StockN1/FrameN2, RX800/slot900, schema2 remote setter. Fresh appliedFrame proof,1250uniqueIMU,4047countergaps,702retained replies/2477scheduledslots=28.3407%; one modequeryfailure,226commandretries. Scheduledslots are NOT completedTX; no CRC/TX99% claim. Allsession/RAM/MPU/trace restoration true. Sensor output returned after target-only powercycle and wide-profile mode restoration; causality is confounded by reboot/configuration/profile. No controller coexistence proof.

## K00954
Source `D0238`, original line6, SHA256 `505631926daf8485125cbe34ddeea18f93ae4148de83593572e56bfdd63df766`. Status: historical/unadjudicated narrative; English source prose.

131 HARDWARE:6000/N1 freshly verified, but S1 inherited await guard required HMDvalid and stopped at0.51s while headset stood on monitor and controllersOFF.327completedTX,147CRC,144retained,ringdrops0,IMU0. This was a pre-stream guard stop, not evidence of sensor failure. Preserve outputs/phase131/attempt (pull after completion).

## K00955
Source `D0238`, original line8, SHA256 `505631926daf8485125cbe34ddeea18f93ae4148de83593572e56bfdd63df766`. Status: historical/unadjudicated narrative; English source prose.

132 PREPARED/ACTIVE: exact124 MCU (2043B/codeSHA4020c351db7e62fc68ef77663695f3f53c7e583bc51fd3e78cd3113bc215b299),6000us/N1. Bench observation now remains enabled during S1, and fresh two-controller-connected readiness does not require HMDpose when guardmode observe. Strict mode still requires HMDvalid; stale monitor/process/Nordic faults still stop. One behavioral test passed (fresh invalidHMD +twoON admitted onlybench; oneOFF/stale rejected). Native19 boundedcountercycle as124, no sensor09/07 changes. User confirmed bothcontrollersON after freshFrame proof. Final result pending; monitor health separately from receive ratios. pose_valid=false.

## K00956
Source `D0238`, original line10, SHA256 `505631926daf8485125cbe34ddeea18f93ae4148de83593572e56bfdd63df766`. Status: historical/unadjudicated narrative; English source prose.

Artifacts: outputs/phase130/attempt;outputs/phase131/attempt;outputs/phase132/attempt andtest_bench_ready.py. Exact invocation/file hashes retained. Generated131/132 inherited inaccurate descriptive experiment strings from preparation helper; actualsource/config/command are authoritative, original metadata preserved.

## K00957
Source `D0238`, original line12, SHA256 `505631926daf8485125cbe34ddeea18f93ae4148de83593572e56bfdd63df766`. Status: historical/unadjudicated narrative; English source prose.

## 132 FINAL and native Standby discovery
110.027582s total; S4 started40.213463s (~69.814119s streaming stage).5461uniqueIMU (~78.22/s S4),7818countergaps; observed counter-span retention5461/(5461+7818)=41.1251%. This estimate is NOT a pure RF loss percentage: native sample generation/selection and counter wrap semantics may contribute. Whole13627TX_END,10812CRC=79.34248%;8130retained=59.66097%;2681ringdrops. S4 alone7173TX_END,5915retained,199ringdrops,5461IMU. Wholecounter denominator must not be mixed withtail-only CRC delta. Tail1000records:508length24,491length30,one35; callback334-337us for999IMU records,one354us report. Freshquietwindow600us;mode1/N1 verified. Allfourcleanuptrue.
Controller telemetry111windows: each10990samples; controller1 invalid10521,changes471;controller2 invalid10453,changes539. S4 controllersinvalid, so132 FAILS healthy gameplay coexistence. No health-triggered reset, peruserbench policy.
Correction: reported phase_lock last_error is negative MCU next-distance correction readfromDATA+1880, not independently measured residual RF phase error. Values~2.4ms alone do not prove failed phase lock; source slot admission rejects correction outside+-60us. Further physicalslot timing needed.

## K00958
Source `D0238`, original line17, SHA256 `505631926daf8485125cbe34ddeea18f93ae4148de83593572e56bfdd63df766`. Status: historical/unadjudicated narrative; English source prose.

POST-STOCK HARDWARE: custommodule unloaded/allrestorationsverified, HMD101/controllers100 stillinvalid. Restarted ONLY steamvr.service (OSuptime10h51m, noOSreboot command). Controllers reconnected butinvalid. Native xrservice log explicitly shows onEnterStandby->UserPresenceDetected0 and firstIMU samples fromBOTHcontrollers. Saved outputs/phase132/attempt/post-stock-standby.txt. After user putONheadset, stock tracking_watch HMD+bothcontrollers connected/valid/result200. This establishes current presence/standby confound; does NOT retroactively prove all earlier controller failures were standby ratherthanRF.

## K00959
Source `D0238`, original line19, SHA256 `505631926daf8485125cbe34ddeea18f93ae4148de83593572e56bfdd63df766`. Status: historical/unadjudicated narrative; English source prose.

133 ACTIVE/NOT YET RESULT: exact132 radio/MCU/19sequencepolicy, new awake-headset control withall3stockvalid immediatelybeforelaunch, BOTHcontrollers alreadyON. No toggling. Userwearing/keepinghandsvisible. NoOTA; installed07unchanged. PreservefinalsensorANDcontroller numbers.

## K00960
Source `D0239`, original line1, SHA256 `e1e359bcd015bb3beaf4d50597934d16f51fed1416b3e99d3eea72bc71d6e2f3`. Status: historical/unadjudicated narrative; English source prose.

# Phase133-136: awake-headset control and command19 dwell
2026-10-09. Installed target[TRACKER_SERIAL]/[TRACKER_ID]/FW189 custom07unchanged; noOTA. Goal98-100%sensor delivery AND twohealthycontrollers notachieved.
