# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K00961
Source `D0239`, original line4, SHA256 `e1e359bcd015bb3beaf4d50597934d16f51fed1416b3e99d3eea72bc71d6e2f3`. Status: historical/unadjudicated narrative; English source prose.

133 hardware:all3stockdevicesvalid atstart, userheadsetON.110.032279s;18372completedTX,18146CRC=98.7698672%;10995retained=59.8465056%;7150hostringdrops;69transportACKs;IMU0. Freshcoveredquiet800us,6000/N1/mode1. Controllertelemetry10990samples each: invalid9392(controller1),2671(controller2);changes1599/6131. Stockafterunload HMD+controller2valid,controller1initiallyinvalid; subsequentfreshsnapshotall3valid. ThusStandby alone cannotexplain133. Neither ACKs norCRC prove application19execution. Allfourrestorationchecks true.

## K00962
Source `D0239`, original line6, SHA256 `e1e359bcd015bb3beaf4d50597934d16f51fed1416b3e99d3eea72bc71d6e2f3`. Status: historical/unadjudicated narrative; English source prose.

134 hardware:onefactor>=250ms identicalcommand19 dwell before ACK retirement (65countercycle). Behavioraltestpassed:earlyACKkeepspending/identicalretry,after250ms retiresonce;othercommandsunaffected. Freshstockall3valid atstart.6427TX,5686CRC=88.4705150%;2655retained,3030ringdrops;44transportACKs;IMU0. Aborted MCUstage drop/rediscovery; allcleanuptrue. Dwell alone didnotprove sensor recovery. MissinglateIMU under340usRX is stillonlyhypothesis, notmeasuredcause.

## K00963
Source `D0239`, original line8, SHA256 `e1e359bcd015bb3beaf4d50597934d16f51fed1416b3e99d3eea72bc71d6e2f3`. Status: historical/unadjudicated narrative; English source prose.

135 proposedslot600us/RX540us, onlyafterfreshcoveredquiet>=800us (observed in133). MCUunchanged. Neverreachedadmission/widen: S0timeout;4206TX,CRC0,IMU0,productidentity absent, allcleanuptrue. No longRXhardware result.
136 proposedsamewindow with safer widening: pausepeerRF,write600usreservation, requireTWO actualidle grants beforeRX540/unpause; waitbounded500ms, abortifnotgranted. This replaces135fixed50ms assumption. UserconfirmedtargetLEDlit. S0timeout again:4207TX,CRC0,IMU0,allcleanuptrue. WiderRX/held19 notexecuted. Do notrepeatblindly; waittargetphysicalstatechange.

## K00964
Source `D0239`, original line11, SHA256 `e1e359bcd015bb3beaf4d50597934d16f51fed1416b3e99d3eea72bc71d6e2f3`. Status: historical/unadjudicated narrative; English source prose.

Userrequestedmanual Frame boot now: fullbuttonOFF, thenbootbuttonheldapproximately1.2sandreleased. Installed07source profile_boot_release selectsFrame onlyresetreason0/count100..149;>=150 selectsstock. Nativec4a2 delays10units via10dd0 eachiteration; docs describe600counts as6s, so~1-1.5s is nominal, actualgesture mustbe confirmed withfreshschema2proof. ControllersremainON; no requestedOFFtoggle. Awaituserphysicalreply.

## K00965
Source `D0239`, original line13, SHA256 `e1e359bcd015bb3beaf4d50597934d16f51fed1416b3e99d3eea72bc71d6e2f3`. Status: historical/unadjudicated narrative; English source prose.

Artifacts outputs/phase133..136/attempt (rawheaders,tailrecords,mailboxpublications,metrics,cleanup,invocationSHA). outputs/phase134/test_held19.py; outputs/phase135/prepare_safe136.py. Prior132 retained5461IMU butcontrollerhealthFAIL; sensor-span41.1251% is NOT pureRF loss. Preserve targetboot/service/gamecontrols separately; pose_valid=false.

## K00966
Source `D0239`, original line15, SHA256 `e1e359bcd015bb3beaf4d50597934d16f51fed1416b3e99d3eea72bc71d6e2f3`. Status: historical/unadjudicated narrative; English source prose.

## 137 remote Frame restoration with controllers already ON
User declinedmanualgesture and requestedradioselection afterturningtargetON. Built standaloneCOMMON-GRIDbootstrap from currentgameMCU,NOTservice79binary:8800us/cycle1,StockN1/FrameN2,RX700/slot900,schema2/desiredframe,benchobserve;staged/denseprofile removed, existingcommon-grid validator retained. omit_command19=true prevents intentionalautonomousIMUstart duringrestoration. No MCUcodechange; codeSHA4020c351db7e62fc68ef77663695f3f53c7e583bc51fd3e78cd3113bc215b299. No permission/guard bypass for invalidgrid, noOTA.
137HARDWARE30s:targetserial/FW189 andfreshFrameproof;3435TX_END,1740CRC=50.6550218%;1737retained=50.5676856%;2ringdrops;IMU0EXPECTEDbecause19omitted. FourtransportACKs. Last99metrics BOTHcontrollers99changes/0invalid/0freeze. Allfourcleanuptrue. This demonstratesboundedremote Frame policy/bootstrap withcontrollersON; notsensor/game99%proof. Futuremode-restoration doesNOTneed manualgesture orcontrollersOFF ifthisexactjointMCU/common-gridprocedure isreused.
138preparedandlaunchedafter137cleanup:exact136widerRXcandidate fresh800usquietgate,slot600/RX540,twoactualidlegrantsbeforeunpause,hold19>=250ms,6000/N1/freshschema2. Actualwidth/gates mustbe checked inthisrun; no preclaim. UserFrame/head/controllersON. Goal98-100%sensorsandhealthycontrollersstillunmet.

## K00967
Source `D0239`, original line20, SHA256 `e1e359bcd015bb3beaf4d50597934d16f51fed1416b3e99d3eea72bc71d6e2f3`. Status: historical/unadjudicated narrative; English source prose.

## 138 FINAL actual long-RX control
Freshcoveredquiet800us admitted slot600/RX540; twoactualidlegrantsrequired beforeunpause. Same2043B MCU/6000/N1,held19>=250ms.18371TX_END,18130CRC=98.6881498%;10517retained=57.2478363%;7613ringdrops;69transportACKs;IMU0. S4 alone14297TX,9155retained,5066ringdrops,IMU0. Last99BOTHcontrollers99changes/0invalid/0freeze. Allfourcleanuptrue. Narrow340usRX alone is NOT sufficient explanation for zeroIMU inthese runs; do not repeat138unchanged. Mode1proof doesnotprove sensorworker/fe1.
139ACTIVE: same138radio/admission, new native17 mode09countercycle65then07cycle65,then19cycle65; >=250msdwell for BOTH17and19 beforetransportretirement. Offlinebehavioraltestpassed, nativeb118/aadc/b10409->07 paths previouslyinspected/emulated. Sensor modes07/09 are separate fromradiomode1. NoOTA, firmware07unchanged. InitialstockHMD+controller1valid/controller2disconnected; userwokecontrollers; awaitfreshconnectedgate. Recordallfinalnumbers; no preclaim.

## K00968
Source `D0239`, original line24, SHA256 `e1e359bcd015bb3beaf4d50597934d16f51fed1416b3e99d3eea72bc71d6e2f3`. Status: historical/unadjudicated narrative; English source prose.

139 FINAL gate rejection: freshcoveredquiet700us below required800us forreserved600us.6134TX,5263CRC=85.8004565%;2722retained,2530ringdrops;IMU0;only4bootstrapACKs. Sensor09/07 andheld19 NEVERexecuted; cannotuse139 torejectsensor-reconfigure hypothesis. Allfourcleanuptrue.
140 ACTIVE: same139held17/19sensor roundtrip; physicallysmaller500usreservation/RX440, freshquiet>=700us. Sameedge allowances:100uslead, MCUadmittedstartcorrection<=60us, at least40us trailingmargin atworst offset. Twoactualidlegrantsrequired beforeunpausing widenedRF. This scalesreservation andquietgate together, notrelaxing margins. RX440 nominallyfits historical416us late30BIMU +16us extraairtimefor5Bcommandversus1Bpoll with8usheadroom; excludes some longerreports anddoesnotpreprove99%sensors. Module/fwunchanged,noOTA. Awaitfinalhardware result.

## K00969
Source `D0240`, original line1, SHA256 `2313e704f6ef0065ee0a5cfa619f207875d183687d3a0faef06b3acaf435f881`. Status: historical/unadjudicated narrative; English source prose.

# Phase141: fresh remote mode restoration after user powercycle (2026-10-09)

## K00970
Source `D0240`, original line3, SHA256 `2313e704f6ef0065ee0a5cfa619f207875d183687d3a0faef06b3acaf435f881`. Status: historical/unadjudicated narrative; English source prose.

## Hardware evidence
The user reported another target powercycle. Previous mode proofs were invalidated. The exact Phase137 compatible bootstrap was repeated for this new boot, not as an unchanged sensor experiment: common 8800 us, Stock N1 / Frame N2, reservation 900 us, RX700, current 2043-byte game MCU, desired Frame, schema2, command19 omitted, controller health record-only. No OTA, tracker revision07 unchanged.

## K00971
Source `D0240`, original line6, SHA256 `2313e704f6ef0065ee0a5cfa619f207875d183687d3a0faef06b3acaf435f881`. Status: historical/unadjudicated narrative; English source prose.

Fresh live response confirmed serial [TRACKER_SERIAL], native firmware189, ID[TRACKER_ID], applied Frame mode (mode1), no pending switch. Connection was observed live; final connected=false follows deliberate session closure. This is mode evidence, not sensor-stream success.

## K00972
Source `D0240`, original line8, SHA256 `2313e704f6ef0065ee0a5cfa619f207875d183687d3a0faef06b3acaf435f881`. Status: historical/unadjudicated narrative; English source prose.

Final counters: 3432 completed TX, 1450 CRC-valid RX (42.2494%), 1446 retained answered slots (42.1329%), 3 host-ring drops, 4 transport ACKs, 9 retries, 0 IMU. IMU0 is expected because command19 was deliberately omitted. These ratios are not full-body sensor delivery percentages. All four cleanup checks are true: session, RAM, MPU, trace controls.

## K00973
Source `D0240`, original line10, SHA256 `2313e704f6ef0065ee0a5cfa619f207875d183687d3a0faef06b3acaf435f881`. Status: historical/unadjudicated narrative; English source prose.

## Phase140 finalized and confound
Pulled final raw records: 5281 TX, 4595 CRC-valid RX (87.0100%), 1814 retained replies, 2780 ring drops, 20 transport ACKs, 110 retries, 0 IMU; MCU rediscovery/mode proof loss stopped the experiment. All four cleanup checks true. The newly reported target powercycle may overlap this failure; its exact time is unknown. Do not attribute rediscovery to native09 reconfiguration without resolving this confound.

## K00974
Source `D0240`, original line13, SHA256 `2313e704f6ef0065ee0a5cfa619f207875d183687d3a0faef06b3acaf435f881`. Status: historical/unadjudicated narrative; English source prose.

## Next hypothesis, not demonstrated
Preserve a single connection from compatible8800 mode restoration through verified Frame N1 preparation, then game6000 phase admission and sensor enable. Reconnection between restoration and sensor start may interact with native sensor state; this remains a hypothesis. Require fresh applied N1/pending0 evidence and actual controller phase coverage. Goal98-100% sensors with both healthy controllers is still unmet. pose_valid=false.

## K00975
Source `D0240`, original line16, SHA256 `2313e704f6ef0065ee0a5cfa619f207875d183687d3a0faef06b3acaf435f881`. Status: historical/unadjudicated narrative; English source prose.

## Provenance
Raw records: outputs/phase141/attempt; exact invocation hashes and ELF hash in invocation.json; new evidence-manifest.json seals downloaded files. Phase140 final records are preserved separately. Current game MCU code SHA256: 4020c351db7e62fc68ef77663695f3f53c7e583bc51fd3e78cd3113bc215b299.
Installed tracker07 SHA256: 40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5. Revision08 remains unflashed.

## K00976
Source `D0241`, original line1, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

# Phase142-143: single-session game bootstrap, 2026-10-09

## K00977
Source `D0241`, original line3, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

## Phase142 measured result
Initial compatible8800 StockN1/FrameN2 remote restoration, same game MCU/engine/connection. At9.322496s explicitly invalidated prior mode lease, prepared FrameN1 and requested a new nonce proof. Fresh verified N1 reached S1 at11.947506s; S2 at14.051573s, S3 at30.199570s, S4 at33.288002s. Fresh native observation found1100us covered quiet window; admitted reservation600/RX540, same two-idle-grant barrier and full6000us phase grid. Held65 command19 records,250ms each. No native09 roundtrip.

## K00978
Source `D0241`, original line6, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

110.063786s:17866TX,16803CRC (94.050151%),9609 retained replies (53.783723%),7194 ring drops,73 transportACKs,257 retries,0IMU. All four cleanup checks true. Final last99 controller samples: controller1 99invalid/0changes, maxfreeze1338.399ms; controller2 0invalid/99changes/0freeze. Controller health FAIL. No game/fullbody success. Single-session reconnection hypothesis is insufficient by itself: link survived, sensors absent. Native sensor command execution remains unproven by transportACKs.

## K00979
Source `D0241`, original line8, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

## Phase143 bounded one-factor control
Prepared/running same142 architecture, hardware module, phase gates and65 held19 records; only change: engine.blind_ack=False instead ofTrue afterS0. This controls S1 bit0 reply-record acknowledgment, NOT reliable application-command execution. Earlier126 failed before this factor applied; this is the first admitted comparison of this factor with compatible single-session bootstrap. Results pending; do not claim success.

## K00980
Source `D0241`, original line11, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

Firmware tracker07 unchanged (97832bytes, SHA40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5),08unflashed,noOTA. Game MCU2043bytes SHA4020c351db7e62fc68ef77663695f3f53c7e583bc51fd3e78cd3113bc215b299. pose_valid=false.

## K00981
Source `D0241`, original line13, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

Exact invocations/raw records: outputs/phase142/attempt and outputs/phase143/attempt;142 evidence-manifest seals local downloaded records. Four admission cases passed for compatible142 policy; initial test import failed due missing PYTHONPATH, corrected before RF. Grok4.7-medium narrow transition audit pending; no verdict claimed.

## K00982
Source `D0241`, original line16, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

## Phase143 finalized
Stopped S2 before sensor command queue: covered700us window, requirement800us. 21.521787s;3578TX/3506CRC=97.987703%,1059retained,2436ringdrops,8transportACKs,8retries,IMU0. All cleanup true. Last99 bothcontrollers99changes/0invalid/0freeze. NOT a negative IMU/ACK-policy result because S4 never executed.

## K00983
Source `D0241`, original line19, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

## Phase144 active / Phase145 prepared
144 derives reservation500/RX440 from measured700us quiet window; preserves100uslead,+/-60usphase tolerance and remaining margin, freshgate700required. Same single-session, ordinary S1-recordACK policy, no09. Live near13388TX:13269CRC=99.1111%,IMU0; interim only.
145 adds65 held native17-mode07 records (250ms) before existing65 held19 records. This differs from125(no17dwell) and140(09roundtrip,powercycleconfound). One behavioral hold/retry test passed. No RF145 until144 cleanup. NoOTA.
Grok4.7-medium narrow review timed out210s; no verdict.

## K00984
Source `D0241`, original line25, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

## Phase144 finalized
Elapsed 110.044158s; final stage S4; abort none. Radio: {"tx_end": 18336, "crc_valid_rx": 18103, "rx_per_tx_end": 0.987292757417103, "host_ring_dropped": 7702, "scope": "single active peer; cumulative counters read sequentially"}. Tracker IMU0, retained10400, transportACKs73, retries251. Controller aggregate (sample windows, not independent RF-loss counts): {"1": {"samples": 10990, "invalid": 85, "changes": 10862, "max_freeze_ms": 426.964906}, "2": {"samples": 10990, "invalid": 0, "changes": 10990, "max_freeze_ms": 0.0}}. Cleanup: {"session_closed_verified": true, "ram_restored_verified": true, "mpu_restored": true, "trace_controls_restored_verified": true}.

## K00985
Source `D0241`, original line28, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

## Phase145 finalized
Elapsed 110.038391s; final stage S4; abort none. Radio: {"tx_end": 18328, "crc_valid_rx": 18132, "rx_per_tx_end": 0.9893059799214317, "host_ring_dropped": 7792, "scope": "single active peer; cumulative counters read sequentially"}. Tracker IMU0, retained10340, transportACKs138, retries510. Controller aggregate (sample windows, not independent RF-loss counts): {"1": {"samples": 10990, "invalid": 40, "changes": 10925, "max_freeze_ms": 395.598391}, "2": {"samples": 10990, "invalid": 0, "changes": 10990, "max_freeze_ms": 0.0}}. Cleanup: {"session_closed_verified": true, "ram_restored_verified": true, "mpu_restored": true, "trace_controls_restored_verified": true}.

## K00986
Source `D0241`, original line31, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

## Phase146 finalized
Elapsed 27.421140s; final stage S4; abort MCU stage drop or rediscovery. Radio: {"tx_end": 4091, "crc_valid_rx": 3791, "rx_per_tx_end": 0.9266682962600831, "host_ring_dropped": 2436, "scope": "single active peer; cumulative counters read sequentially"}. Tracker IMU0, retained1353, transportACKs8, retries50. Controller aggregate (sample windows, not independent RF-loss counts): {"1": {"samples": 2773, "invalid": 34, "changes": 2678, "max_freeze_ms": 629.149947}, "2": {"samples": 2773, "invalid": 535, "changes": 91, "max_freeze_ms": 21791.448804}}. Cleanup: {"session_closed_verified": true, "ram_restored_verified": true, "mpu_restored": true, "trace_controls_restored_verified": true}.

## K00987
Source `D0241`, original line34, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

Interpretation:144-145 sustained RF keepalive near99% but sensor delivery remains0. Held07 resync before19 was insufficient.146 batch dropped MCU stage3.053s afterS4, so host-drain optimization is NOT admitted; do not claim batching caused drop solely from this comparison (fresh phase/controller schedules differ). Historical83 batching failure remains relevant. Record-only health policy retained.

## K00988
Source `D0241`, original line36, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

Provenance correction: copied runtime mailbox logger in142-145 retained a hardcoded remote phase138 path. Its mailbox-publications.jsonl is now a CROSS-RUN append-only stream; never treat all its rows as Phase138. Do not delete/rewrite it. Exact per-run radio-run/raw records, invocation hashes and controller logs remain separately scoped.146/147 logger path fixed in isolated runtime. Historical138 original sealed records remain evidence; mailbox stream needs timestamp/invocation attribution. No live module/source mutation was performed midrun.

## K00989
Source `D0241`, original line38, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

147 prepared/running: same145, no batch, held65 native09 then65 native07 then65 native19. Unlike139 it may reachS4; unlike140 do not powercycle during test. Native09->07 forces real sensor reconfiguration path if commands execute; radio ACK remains transport evidence only. NoOTA/revision07 unchanged/pose_valid=false. Grok audit timedout210s, no verdict.

## K00990
Source `D0241`, original line41, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

## Phase147 finalized before sensor factor applied
40.01917569199577s, S0 timeout;4574 completedTX/0CRC, no product identity/fresh mode proof/IMU0,0transportACKs. S4 never reached, no09/07/19 roundtrip sent. This is NOT a negative sensor-roundtrip result. MCU discovery/confirming state alone is not verified target application identity. All cleanup checks true. User asked to wake target; sensor comparison remains pending.

## K00991
Source `D0241`, original line44, SHA256 `b3c684c9c5992913763e08e84788e62bc5033786eee32ea47c88d2b25df61600`. Status: historical/unadjudicated narrative; English source prose.

Post147 stock-only tracking_watch1: HMD and bothcontrollers connected=true,valid=true,result200. This is a single fresh post-cleanup sample, not sustained native-baseline certification. No custom radio remains active. Waiting for user to wake target; no further RF/OTA while target presence is unconfirmed.

## K00992
Source `D0242`, original line1, SHA256 `ed77cc6fd87a2208a706b20485ed349e5f9e964cce00a39f68822aae749c8a92`. Status: historical/unadjudicated narrative; English source prose.

# Phase148: user-observed tracker hang and post-reset no19 control (2026-10-09)

## K00993
Source `D0242`, original line3, SHA256 `ed77cc6fd87a2208a706b20485ed349e5f9e964cce00a39f68822aae749c8a92`. Status: historical/unadjudicated narrative; English source prose.

## Observation and limits
The user reported that the physical tracker hung, required a restart, and that hangs appeared after IMU work began. This is USER-OBSERVED evidence, not a recorded fault PC, watchdog cause or established command19 causality. The exact hang onset is unknown. Previous radio mode proofs were invalidated by the reboot.

## K00994
Source `D0242`, original line6, SHA256 `ed77cc6fd87a2208a706b20485ed349e5f9e964cce00a39f68822aae749c8a92`. Status: historical/unadjudicated narrative; English source prose.

146 lost MCU connection3.053163s after S4 admission; only8 bootstrap transportACKs,50 retries; its queue started with native17-mode07. Native19 was queued behind65 held17 records and was not retired/sent as that next step before this drop.145 had previously retired138 transportACKs across bootstrap+65native07+65native19, IMU0; its tracker-side state after host RAM cleanup was not measured.147 failed in S0 before the09/07/19 queue, so it cannot show a new IMU command causing the physical hang.

## K00995
Source `D0242`, original line8, SHA256 `ed77cc6fd87a2208a706b20485ed349e5f9e964cce00a39f68822aae749c8a92`. Status: historical/unadjudicated narrative; English source prose.

## Static distinction
Command19 handler0x8efc writes permission byte20000fe1=1, then0xaca4(1) writes20001f98=1 and returns. The short handler itself contains no sensor bus wait. Downstream IMU enqueue and TIMER0 autonomous transmission use permission; either downstream path or an unrelated cause remains unknown. Native dispatcher0x94fc may queue handlers through0x15d4; transportACK does not prove worker execution. Command18 is enter_idle_mode (8f74->6030), not a validated independent IMU-stop control; do not send it as a guessed fix.

## K00996
Source `D0242`, original line11, SHA256 `ed77cc6fd87a2208a706b20485ed349e5f9e964cce00a39f68822aae749c8a92`. Status: historical/unadjudicated narrative; English source prose.

Source references: outputs/swift189-full.asm at8efc/aca4/94fc/8f74; outputs/fw189-command-audit15/table.json and native-handler-ranges.asm; docs/FW189_ITERATION_15_COMMAND_READBACK_AUDIT_2026-10-08.md; docs/PHASE113_GAME_SCHEDULER_AND_IMU_GATE_2026-10-09.md. Preserve119/130/132 successful sensor evidence as counterexamples to an unconditional 'IMU always hangs' claim.

## K00997
Source `D0242`, original line13, SHA256 `ed77cc6fd87a2208a706b20485ed349e5f9e964cce00a39f68822aae749c8a92`. Status: historical/unadjudicated narrative; English source prose.

## New control
148 uses exact137 compatible8800 StockN1/FrameN2,900usreservation/RX700, current2043byte MCU, fresh schema2 desiredFrame, controllersON record-only. Duration90s instead of30s follows newly reported physical hang. omit_command19=true; no65resync, no09roundtrip, no batch, noOTA. Initial ordinary17/25 configuration remains; this is a no19 control, NOT a no-sensor-configuration control. Mailbox logger corrected to148-scoped path.
Fresh live target identity [TRACKER_SERIAL]/FW189/[TRACKER_ID] and appliedFrame mode verified; early3415TX/1745CRC/1731retained/11drops/IMU0(expected), connected=true. Interim only; final results pending. Firmware07 unchanged, revision08 unflashed, pose_valid=false; game goal remains unmet.

## K00998
Source `D0242`, original line18, SHA256 `ed77cc6fd87a2208a706b20485ed349e5f9e964cce00a39f68822aae749c8a92`. Status: historical/unadjudicated narrative; English source prose.

## Final148 hardware result
90s bounded no19 control completed, serial/FW189/appliedFrame proof maintained, no session-loss abort.10253TX,5236CRC (51.067980%),5224retained (50.950941%),11ringdrops,4transportACKs,12retries,0IMU(expected omit19). Final connected=false is deliberate session closure; connection was observed live. All four cleanup checks true. Controller aggregates: {"1": {"samples": 9010, "invalid": 0, "changes": 9010, "max_freeze_ms": 0.0}, "2": {"samples": 9010, "invalid": 0, "changes": 9010, "max_freeze_ms": 0.0}}. This diagnostic compatibility profile is NOT the game scheduler and its response ratio is NOT sensor delivery. No physical fault PC or post-run physical-button responsiveness was measured; 'no observed radio hang during90s' is the proper scope.

## K00999
Source `D0242`, original line21, SHA256 `ed77cc6fd87a2208a706b20485ed349e5f9e964cce00a39f68822aae749c8a92`. Status: historical/unadjudicated narrative; English source prose.

146 scoped mailbox publication opcode counts: {"1": 17, "16": 5, "17": 3, "25": 2}. Command19 publication count 0; held native17 was the first newS4 publication. This supports narrowing the immediate146drop to before a NEW19 publication, while prior145trackerstate remainsunknown. No65start/reconfiguration loops should be repeated until this native-worker/firmware path is understood. User-reported physicalhang chronology recorded, not promoted to a confirmed command cause.

## K01000
Source `D0243`, original line1, SHA256 `e383e538ab269ca099b68f97d8bf3e18cd4441e8a514167ebddb646c79eedc20`. Status: historical/unadjudicated narrative; English source prose.

# Phase149-154: application responses and real sensor initialization (2026-10-09)

## K01001
Source `D0243`, original line3, SHA256 `e383e538ab269ca099b68f97d8bf3e18cd4441e8a514167ebddb646c79eedc20`. Status: historical/unadjudicated narrative; English source prose.

Goal remains real full-body tracker in SteamVR with two healthy Frame controllers, requiring real IMU plus identified metric optical pose and valid driver publication. Not achieved; no synthetic poses/default calibration accepted.

## K01002
Source `D0243`, original line5, SHA256 `e383e538ab269ca099b68f97d8bf3e18cd4441e8a514167ebddb646c79eedc20`. Status: historical/unadjudicated narrative; English source prose.

## Measured hardware
|Phase|TX|CRC RX|Retained replies|Ring drops|Unique IMU|Transport ACKs|Observed native20 reply sequences|Final recorded mode|
|---|---:|---:|---:|---:|---:|---:|---|---|
|149|3437|1767|1756|10|0|5|[0]|frame|
|150|8065|7556|4041|3508|0|8|[]|None|
|151|18325|18170|10498|7671|0|11|[0, 1]|frame|
|152|5139|2529|2527|1|0|7|[0, 1]|stock|
|153|5142|2585|2570|14|0|11|[0, 1, 2, 3]|stock|
|154|5139|2574|2559|15|0|11|[0, 1, 2, 3]|stock|

## K01003
Source `D0243`, original line15, SHA256 `e383e538ab269ca099b68f97d8bf3e18cd4441e8a514167ebddb646c79eedc20`. Status: historical/unadjudicated narrative; English source prose.

149 native20 body01 was received from exact target, including retained raw DMA/sourceID/CRC. Native93c8 offline execution intercepted92a8 and confirmed fixed queue args (20000fd0,length1,opcode20,body01); no sensor handler. Repeated same-sequence replies are retransmissions, not independent worker events.
150 assumed command20 counter persisted across reconnect (start1): no20 reply, no19 published; aborted3s afterS4. This is NOT IMU causing a fault. Pending application-proof timeout can trigger host disconnect; report MCU rediscovery alone must not be promoted to physical tracker hang. Its test initially failed collection (wrong import); corrected behavioral test passed after RF launch, before interpreting results. Production RF source was not changed midrun.
151 starts20at0: response0 before single19seq0 and response1 after.110s game6000/N1/500reservation/RX440,99.15416%CRC but IMU0. Native command processing/reply path remains alive after the start attempt; full sensor-queue execution/fe1 readback still not proven by transportACK.
152 fresh STOCK mode0/StockN1 common8800/RX700 likewise20responses0/1 aroundsingle19,IMU0. Mode1-only explanation is insufficient. This diagnostic profile is not game acceptance.
153 stock single17mode09 then07 held1s each,20recordsbetween,thenone19: IMU0. Finalworker sequences recorded in table; no65 loops.
All149-154 cleanup checks true; installed tracker07 unchanged,08unflashed,noOTA. pose_valid=false.

## K01004
Source `D0243`, original line22, SHA256 `e383e538ab269ca099b68f97d8bf3e18cd4441e8a514167ebddb646c79eedc20`. Status: historical/unadjudicated narrative; English source prose.

## New offline findings
Native94fc dispatcher ignores15d4 enqueue failure and advances expected opcode counter at9566..957c. Bounded emulation with deliberately full scheduler ring returned queue error4 and advanced expectedseq20 to1 without executing handler. This is a STOCK behavior, not proof the live queue overflowed. It explains why reliable radio/sequence retirement cannot prove application execution. Do not resume65-command floods as a cure.
Nativeb118: same-mode07 returnsb150.07->09->07 calls a750/b104/aadc twice and does NOT call full initializerab20.07->80->07 calls a75c/b020, thena750/b104/ab20. Bounded nativeb118 execution confirmed those branch traces, hardware functions intercepted.
Fullinitializerab20 clears20001fb4, checks native sensor identity20001fa0 against47 (separate44 branch), programs sensor registers, and only its successful path sets20001fb4nonzero at ac82. IMUcallbackad08 returns immediately when20001fb4is0. A failed initial init plus currentmode07 could therefore survive reissuing07 or09->07; live flag/WHOAMI is UNKNOWN. This is a concrete hypothesis, not a measured root cause.
Sensor-off80 pathb020 only observed native sensor-register writes4e/14/65 and delays; do not confuse sensor mode80 with radio mode0/1.

## K01005
Source `D0243`, original line28, SHA256 `e383e538ab269ca099b68f97d8bf3e18cd4441e8a514167ebddb646c79eedc20`. Status: historical/unadjudicated narrative; English source prose.

## Phase154 candidate
Same153 stock common8800, single sensor80->07 held1s with native20 replies between, thenone19 and final20. No65, no batch, no firmware patch/OTA. Tests ran real nativeb118 with hardware calls intercepted; actual sensor initialization not emulated/proven. FINAL:45s,50.0875657%CRC/TX,allfourcleanuptrue,actual20sequences0/1/2/3,IMU0. SensorRTOSexecution and enabledflag remain unobserved; requestingfullinit did not produce IMU. Do not repeat unchanged. Exact invocation/ELF/source hashes and scoped mailbox/worker-proof logs under outputs/phase154.

## K01006
Source `D0243`, original line31, SHA256 `e383e538ab269ca099b68f97d8bf3e18cd4441e8a514167ebddb646c79eedc20`. Status: historical/unadjudicated narrative; English source prose.

## Full-body route check
Registered SteamVR external driver is /home/steamos/pico-ram-work/steamvr-driver (fresh openvrpaths read); registration does not prove valid pose. pc/optical/measured_pipeline.py requires measured axes/scale/time alignment and identified factory LED model. pc/run_optical.py defaults to synthetic benchmark and must NOT be used as hardware proof. Historical metric camera/serial/clock evidence exists but current-boot alignment and real moving tracker pose remain unverified; no pose publication gate relaxed.

## K01007
Source `D0244`, original line1, SHA256 `fcaeae718c9c7e52e88a209d4c6ccd9d1df01852f82c97da7a111bfd57ac91c6`. Status: historical/unadjudicated narrative; English source prose.

The fixed candidate is a read-only query hook, not an IMU fix, and it is not approved for OTA. The image under review is `outputs/phase155/candidate09-diagnostics/fw189-dual-mode.bin`, 97912 bytes, SHA256 `298d6ebb7caa19b66e9952895efa22c09c531b88b07d0c4439d351dacbf45c65`. The fallthrough image `e4752f78e0d50c333eedcb19780b689a011d64c7c984194533e84bd1d673e3ba` is a different binary.

## K01008
Source `D0244`, original line3, SHA256 `fcaeae718c9c7e52e88a209d4c6ccd9d1df01852f82c97da7a111bfd57ac91c6`. Status: historical/unadjudicated narrative; English source prose.

## Blockers

## K01009
Source `D0244`, original line5, SHA256 `fcaeae718c9c7e52e88a209d4c6ccd9d1df01852f82c97da7a111bfd57ac91c6`. Status: historical/unadjudicated narrative; English source prose.

1. **These four bytes are software flags. They are not IMU samples, and their live values are still unknown.** Phase 151’s 18170/18325 CRC replies (99.15416%) with zero IMU is unchanged by this offline patch. Installed firmware remains revision 07, `40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5`.

## K01010
Source `D0244`, original line7, SHA256 `fcaeae718c9c7e52e88a209d4c6ccd9d1df01852f82c97da7a111bfd57ac91c6`. Status: historical/unadjudicated narrative; English source prose.

| Reply offset | Address | What the Swift image actually does |
   |---|---|---|
   | 22 | `0x20000442` | Mode byte. `0xb118` stores a new value; `0xb0f8` returns it; `0xb1a4` forces `0x80`. `0xb104` maps mode 7 to `0x0A` and mode 9 to `0x28`. The snapshot only copies the byte. |
   | 23 | `0x20001fa0` | Cached identity. `0xa862` stores the byte read at `sp+5` after `0xa7f8`. `0xab46` accepts `0x47`; `0xac88` accepts `0x44`. Any other value leaves the enabled word at 0. |
   | 24 | `0x20001fb4` | Enabled word. `0xab32` stores 0; `0xac82` stores 1 only after that identity check. `0xad14` returns from the producer at `0xad08` when the word is 0. Any nonzero value is reported as 1. |
   | 25 | `0x20000fe1` (`0x20000fd0+0x11`) | TX permission. `0x8efc`/`0x8f00` sets it to 1 and ignores the command-19 body, then `0xaca4` sets `0x20001f98=1`. `0x651e` clears it on the `0x64ac` path. `0x6206` and `0x62f6` skip TIMER0 start while it is 0. A 0 does not uniquely mean command 19 never ran. |

## K01011
Source `D0244`, original line14, SHA256 `fcaeae718c9c7e52e88a209d4c6ccd9d1df01852f82c97da7a111bfd57ac91c6`. Status: historical/unadjudicated narrative; English source prose.

2. **A healthy-looking snapshot can still sit in front of zero IMU.** The producer’s 23-byte sample enqueue is a different slot from this reply: `0xae40` calls `0x92a8` with `r2=0x16` and `r3=0x17`. The one-shot 6-byte enqueue at `0xad60` is skipped when `0x20001f98` is 0. `0x92a8` returns `0x0F` when `0x20000e10` is 0, `0x13` when the node allocator `0x8c3c` fails, and `0x06` on its index check. None of those bytes are in the 26-byte reply.

## K01012
Source `D0244`, original line16, SHA256 `fcaeae718c9c7e52e88a209d4c6ccd9d1df01852f82c97da7a111bfd57ac91c6`. Status: historical/unadjudicated narrative; English source prose.

3. **The 26 Thumb tests never execute `0x92a8`.** `Firmware._hook` intercepts `0x9368`, copies `r0`/`r1`, and returns. `.Lreply` discards the queue result. A missing schema-3 reply on hardware is therefore not evidence that the four fields were read. Length 26 itself matches the existing schema-2 reply: `0x9368` passes the caller length through as `r3`, and `0x9326` stores that length in one byte at `node+0x0F` before the copy to `node+0x10`.

## K01013
Source `D0244`, original line18, SHA256 `fcaeae718c9c7e52e88a209d4c6ccd9d1df01852f82c97da7a111bfd57ac91c6`. Status: historical/unadjudicated narrative; English source prose.

4. **Schema 3 is not a setter and does not start the sensor task.** The old setter still requires length 24, opcode `0x7d`, schema byte 2, mode ≤ 1, N ≤ 40, and the chip at `0x20000efe`. Opcode `0x7e` with schema 3 and length 18 only fills the stack buffer and calls `0x9369`.

## K01014
Source `D0244`, original line22, SHA256 `fcaeae718c9c7e52e88a209d4c6ccd9d1df01852f82c97da7a111bfd57ac91c6`. Status: historical/unadjudicated narrative; English source prose.

Register, stack, and ABI in the fixed image are consistent. `profile_command` keeps the `push {r4,r5,r6,r7,r8,lr}` / `sub sp,#32` frame. Offsets 22–25 fit in those 32 bytes. `r8` holds the saved PRIMASK and is restored before `blx 0x9369`. The `ite` around the enabled word is a legal two-instruction block. Entry `r0` is the 26-byte buffer and `r1` is 26, which is what `0x9368` forwards into `0x92a8`.

## K01015
Source `D0244`, original line24, SHA256 `fcaeae718c9c7e52e88a209d4c6ccd9d1df01852f82c97da7a111bfd57ac91c6`. Status: historical/unadjudicated narrative; English source prose.

Schema 1 still ends at length 24 with revision byte 1. Schema 2 still writes mode, revision 7, N, and pending, then branches to `.Lreply` at line 486. The rejected source falls from that `msr PRIMASK,r8` straight into `.Lsensor_snapshot` and would replace that revision-7 tail. Do not use that image.

## K01016
Source `D0244`, original line26, SHA256 `fcaeae718c9c7e52e88a209d4c6ccd9d1df01852f82c97da7a111bfd57ac91c6`. Status: historical/unadjudicated narrative; English source prose.

The new `cpsid i` covers only the four loads, then PRIMASK is restored. The longer mask already present in revision 07 remains in `profile_rx_boundary`: when a mode transition is pending it masks IRQs across the native halt at `0x57ed` and the TIMER0 shutdown. Schema 3 does not widen that window.

## K01017
Source `D0244`, original line28, SHA256 `fcaeae718c9c7e52e88a209d4c6ccd9d1df01852f82c97da7a111bfd57ac91c6`. Status: historical/unadjudicated narrative; English source prose.

No OTA approval. This prepares a diagnosis of why keepalive can be near 99% while `0xad08` never emits IMU.

## K01018
Source `D0245`, original line1, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

# Phase155: fixed read-only sensor diagnostics, 2026-10-09

## K01019
Source `D0245`, original line3, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

Goal: real full-body tracking in SteamVR with two healthy Frame controllers and 98-100% actual sensor delivery. NOT achieved; pose_valid=false. Installed tracker07 unchanged, SHA25640941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5. Candidate08 UART remains unflashed. No OTA in155/156; no RF active after156 cleanup.

## K01020
Source `D0245`, original line5, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

## Hardware blocker

## K01021
Source `D0245`, original line7, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

151 game6000/N1:18170CRC/18325TX=99.15416%,IMU0.152 stock control:IMU0.154 sensor80->07 (full native initializer requested):5139TX/2574CRC=50.0875657%,2559retained,15drops,IMU0,20seq0..3,allcleanuptrue.156 adds nativeUARToff500000 with actual5000 response:5140TX/2330CRC=45.3307393%,2326retained,3drops,13ACK58retry,20seq0..4,IMU0,allcleanuptrue. Native main command worker alive; sensor RTOS execution and flags UNKNOWN. Do not repeat unchanged65/19 floods or report empty keepalive as sensor delivery.

## K01022
Source `D0245`, original line9, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

## Candidate09, NOT installed or authorized

## K01023
Source `D0245`, original line11, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

Image outputs/phase155/candidate09-diagnostics/fw189-dual-mode.bin:97912bytes,SHA256298d6ebb7caa19b66e9952895efa22c09c531b88b07d0c4439d351dacbf45c65. Adds80bytes to installed07. Builder outputs/phase155/build_diagnostic.py checks exact generated07assembly against installed artifact before modification; reverse patch asserts frozenstock SHA256477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89. Source inputs catalogued and archive-manifest matches verified in input-provenance.json; post-snapshot inputs separately sealed. No UART default-on change.

## K01024
Source `D0245`, original line13, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

Command1 request body7e03 +16byte nonce (18bytes). Reply fe03 +same nonce +nativechip4 +fourbytes (26bytes): sensor mode20000442, cachedidentity20001fa0, Boolean enabledword20001fb4!=0, TXpermission20000fe1. Fixed addresses, no arbitraryRAM request, no SPI/init/write. Four loads under saved PRIMASK, restored before9369; existing32byte stack frame and oldschema1/2 retained. Schema2 wire revision remains7; diagnostic candidate revision9 is the image label.

## K01025
Source `D0245`, original line15, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

Native b118 updates sensor mode; ab20 clears enabledword and accepts identity47 or44 before success setsword1 at ac82; ad08 returns when enabledword0. Permission can be cleared by651e, so zero does not uniquely mean19 never executed. Snapshot is software state, not samples or physical sensor-health proof. A missing reply is inconclusive.

## K01026
Source `D0245`, original line17, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

## Offline verification

## K01027
Source `D0245`, original line19, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

77 unique cases passed:26 actualThumb handler/legacyquery tests,39 oldremote-mode regressions againstcandidate,2 native stock branch/counter tests,7 strictcodec nonce/chip/schema tests,2 actualnative92a8 responsequeue/allocationfailure tests,1 exact-image4080blocktransfer. Linux independently confirmed28 handler/native-branch cases. WindowsUnicorn emitted mem_map access-violation diagnostics despite successful test status; Linuxconfirmation and stderr preserved. Nativequeue allocator/IRQ mocked; native reply insertion and wirelength execute. No tests prove realIRQ/RF/bootloader/recovery.

## K01028
Source `D0245`, original line21, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

Exactservice38 transfer emulator with10% injectedloss and hoststalls:4080/4080blocks ACKed,4080writes,oneerase,exactstaging,49retries,finish_verified. Nativeactivation/cadence/boot NOTverified. XML and text retained.

## K01029
Source `D0245`, original line23, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

First candidate fell through fromschema2into diagnostics;4 compatibility tests failed. Rejected image preserved at candidate09-rejected-fallthrough SHA256e4752f78e0d50c333eedcb19780b689a011d64c7c984194533e84bd1d673e3ba. Fixedexplicitbranch; finaltests pass. Rejectedimage never flashed.

## K01030
Source `D0245`, original line25, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

## External audit

## K01031
Source `D0245`, original line27, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7-medium read-only audit completed. ABI/stack/register/PRIMASK/legacycommands consistent; no code blocker identified. Verbatim report PHASE155_GROK47_DIAGNOSTIC_AUDIT_2026-10-09.md. Reviewer correctly states softwareflags cannot prove IMU/fullbody. Follow-up actualnativequeue tests address intercepted9368 limitation; missinghardware reply remains inconclusive.

## K01032
Source `D0245`, original line29, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

Lead clarification:23byte record at ae40 is opcode22, downstream of primary18byte IMU ring insertion add4->97a8. It is not the sole IMU producer. Further sampling/ring/report diagnostics needed if enabled and permission are both nonzero.

## K01033
Source `D0245`, original line31, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

## Actual OTA boundary / continuation

## K01034
Source `D0245`, original line33, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

Offline proposaljob outputs/phase155/ota-proposal/job.json,image.bin,patch-manifest.json is NOTauthorized. approved_for_ota=false. Production runtime exact-image whitelist still only historically accepted05/07; no bypass added. User required pause immediately before actual OTA. New exactimage needs authorization and fresh target/controller-off service preflight before any start; recovery remains unproven as previously accepted by user for old images.

## K01035
Source `D0245`, original line35, SHA256 `eb61da3345618b13dd2d57efd01bc910968596e8de4b9a8de4dfe05bcf90762a`. Status: historical/unadjudicated narrative; English source prose.

If actualdiagnostics enabled0: distinguish unexpectedcachedidentity vs initfailure/currentmode. If permission0: inspectnative19/clearpath. If bothnonzero withIMU0: inspectactualsampling task/ring/report construction. After realIMU recovery, addresshostringloss, measuredaxes/time alignment, identified calibrated opticalpose and validSteamVRpublication with bothcontrollershealthy. These remain acceptancegates, not future guarantees.

## K01036
Source `D0246`, original line1, SHA256 `995f8ab51bfea94d1beb1540a6f1b440e1223c40e0126a8a9b6494b19506cc85`. Status: historical/unadjudicated narrative; English source prose.

# Phase156 — native UART-off factor, 2026-10-09

## K01037
Source `D0246`, original line3, SHA256 `995f8ab51bfea94d1beb1540a6f1b440e1223c40e0126a8a9b6494b19506cc85`. Status: historical/unadjudicated narrative; English source prose.

Goal unchanged: real full-body tracker with healthy Frame controllers, not keepalive. Candidate09 remains unflashed; no OTA.

## K01038
Source `D0246`, original line5, SHA256 `995f8ab51bfea94d1beb1540a6f1b440e1223c40e0126a8a9b6494b19506cc85`. Status: historical/unadjudicated narrative; English source prose.

Prior UART report and original disassembly were read. Native command1 body500000 invokes8814, writes UART flag20001d45=0, calls1918, returns application body5000. This native flag changes a400/b8e0/be4a/c788 branches; transparent normal-operation behavior was never established. d408 can also enable it when native d350 returnsbb. Actual live flag/persistence unknown. Previous582IMU after UART enable prevents claiming UART universally blocks IMU.

## K01039
Source `D0246`, original line7, SHA256 `995f8ab51bfea94d1beb1540a6f1b440e1223c40e0126a8a9b6494b19506cc85`. Status: historical/unadjudicated narrative; English source prose.

New bounded control: same Phase154 MCU/stock common8800/900 reservation/RX700, controllers alreadyON,45seconds. Add ONE logical UART disable after first native20 proof; require application5000 in a slot carrying its published generation before retirement. Then native20 proof, sensor80→07 held1s with20 proofs, single19 held1s and final20. No65 floods, batching, tracker flash or changed MCU timings. Replies to5000 carry no nonce, so this is bounded current-session application response evidence, not arbitrary register readback.

## K01040
Source `D0246`, original line9, SHA256 `995f8ab51bfea94d1beb1540a6f1b440e1223c40e0126a8a9b6494b19506cc85`. Status: historical/unadjudicated narrative; English source prose.

Two offline tests passed: actual installed07 native setter clears flag/calls1918/returns5000 without staging flash calls (driver/MMIO intercepted); host refuses empty reply and5000 tagged to an unrelated TX generation. Per-run source hashes and invocation sealed under outputs/phase156/attempt/invocation.json.
