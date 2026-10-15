# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K01121
Source `D0254`, original line19, SHA256 `ffd1e2d6b4391d0614f146ccc445d6844e784b88110bf0a588c7e429067a545e`. Status: historical/unadjudicated narrative; English source prose.

## Final independent review disposition

## K01122
Source `D0254`, original line21, SHA256 `ffd1e2d6b4391d0614f146ccc445d6844e784b88110bf0a588c7e429067a545e`. Status: historical/unadjudicated narrative; English source prose.

Both repeat reviews returned. Grok4.7medium and Gemini found no code/ABI/stack/PRIMASK/length/legacy blocker in the supplied exact-source delta. Grok explicitly did not recompute the binary hash because the isolated package contained source only; the lead separately recomputed the binary SHA256. Verbatim reviews: PHASE158_GROK_FINAL_REVIEW_2026-10-09.md, PHASE158_GEMINI_FINAL_RESPONSE_2026-10-09.md and PHASE158_GEMINI_FINAL_REVIEW_2026-10-09.md.

## K01123
Source `D0254`, original line23, SHA256 `ffd1e2d6b4391d0614f146ccc445d6844e784b88110bf0a588c7e429067a545e`. Status: historical/unadjudicated narrative; English source prose.

Lead accepts narrower sufficiency: page4 exposes all three proven predicates before d53c and enables an actionable current gate classification alongside page3. A zero gate is a currently closed prerequisite, not proof it caused all historical missing samples or that the IRQ ran. Gemini's wording that it immediately isolates the cause is too strong and is not adopted. Its atomic claim applies to the short gate snapshot relative to ordinary masked interrupts, not to two pages together or to physical IRQ history. d53c posts a sensor message through d4b0 conditional on201c, not merely an allocator. Neither static review proves boot/recovery, sensor function or game coexistence. Known limits accepted as diagnostic interpretation constraints, not concealed.

## K01124
Source `D0254`, original line25, SHA256 `ffd1e2d6b4391d0614f146ccc445d6844e784b88110bf0a588c7e429067a545e`. Status: historical/unadjudicated narrative; English source prose.

Lead status: offline diagnostic preparation complete; user must be called at the pre-OTA boundary. No transfer started. Both controllers must remain off for the service38 transfer profile; enabling them is for subsequent gameplay validation. Newhash still excluded from production whitelist. Any future OTA admission is a narrowly reviewed operational step after the user's boundary, not a claim of physical safety.

## K01125
Source `D0255`, original line1, SHA256 `7558b007dd615777d991a4489f006b36af731289a412505da26ee221d25e6f74`. Status: historical/unadjudicated narrative; English source prose.

# Phase159/160 cadence and admission clarification

## K01126
Source `D0255`, original line3, SHA256 `7558b007dd615777d991a4489f006b36af731289a412505da26ee221d25e6f74`. Status: historical/unadjudicated narrative; English source prose.

User asks whether stock-cadence service OTA ran while tracker was mode1, and whether a speed check was omitted. Read raw saved evidence only; no hardware action.

## K01127
Source `D0255`, original line5, SHA256 `7558b007dd615777d991a4489f006b36af731289a412505da26ee221d25e6f74`. Status: historical/unadjudicated narrative; English source prose.

All seven retained FE02 mode replies in phase159/analysis.json contain byte22=0 (stock radio mode), byte23=7. First replies end00070000: mode0/revision7/N0/pending0. Later replies end00070100: mode0/revision7/N1/pending0. The prior report incorrectly called the value1 a pending-state byte: it is actualN at byte24, not pending at byte25. This correction supersedes that interpretation. Final host mode_verified=false/connected=false remains real host state, but cannot erase the raw mode0 replies or be described as a measured mode1 transition.

## K01128
Source `D0255`, original line7, SHA256 `7558b007dd615777d991a4489f006b36af731289a412505da26ee221d25e6f74`. Status: historical/unadjudicated narrative; English source prose.

Phase160 service38 config and job use4727us period,900us slot,declaredsourcegrid4727.5. They do not adapt to3000us mode1. The OTA adapter checks live product serial/FW189 and targetidentity, but its saved live output has radio_mode_required=false and radio_mode=null; no mode query was performed inside the OTA session. Therefore mode0 was observed in the preceding check, but mode was not freshly proved at destructive start. A mode1-at-start claim is unsupported, and an uninterrupted fresh mode0-at-start guarantee is also unsupported.

## K01129
Source `D0255`, original line9, SHA256 `7558b007dd615777d991a4489f006b36af731289a412505da26ee221d25e6f74`. Status: historical/unadjudicated narrative; English source prose.

No separate service38 throughput/cadence check was conducted immediately before Phase160. Phase159 was a30s common8800us identity/mode check and does not qualify as a service38 speed check. Sender reused the measured successful OTA46 service38 module/runtime. This omission is an admission-validation gap and should be acknowledged, not hidden by later success.

## K01130
Source `D0255`, original line11, SHA256 `7558b007dd615777d991a4489f006b36af731289a412505da26ee221d25e6f74`. Status: historical/unadjudicated narrative; English source prose.

Measured Phase160 outcome remains4085/4085 blocks plusfinish_verified,5retryintervals,373.1633701324463s progression span. Payload98032B/span isapproximately262.7B/s; not full RF throughput or exact total elapsedtransfer time. Activation stillpending in saved state. Do not repeat OTA merely to remedy a documentation/admission gap. For future transfers require fresh matching mode/cadence proof and a bounded same-profile check before destructive start, while preserving the user's pause boundary.

## K01131
Source `D0256`, original line1, SHA256 `684e352a6e87f5cedff182856e8da3fb96c9f999674da321c3751bbd85cbb8ed`. Status: historical/unadjudicated narrative; English source prose.

# Phase159 fresh pre-OTA check: IMU returned on installed07

## K01132
Source `D0256`, original line3, SHA256 `684e352a6e87f5cedff182856e8da3fb96c9f999674da321c3751bbd85cbb8ed`. Status: historical/unadjudicated narrative; English source prose.

User confirmed Frame and target enabled, both controllers off. Frame SSH reachable; stock tracking_watch reported headset connected/valid and two controllers disconnected. No active loader found before start. One bounded30s fresh identity/mode check reused frozen Phase148 no-host-command19 configuration, common8800us, slot900/RX700, unchanged MCU ELF SHA4a386160076ec4284c6f93086fd568360e837304c9d5411877a18e2b063b58ef. No OTA job or flash. This was a fresh-target admission check, not a repeated tuning experiment.

## K01133
Source `D0256`, original line5, SHA256 `684e352a6e87f5cedff182856e8da3fb96c9f999674da321c3751bbd85cbb8ed`. Status: historical/unadjudicated narrative; English source prose.

Actual product identifies [TRACKER_SERIAL], chip[TRACKER_ID], FW189. Actual FE02 replies report radio mode0 and wireversion7. First proof succeeded, but later queries failed admission and final connected=false/mode_verified=false with2query failures. Several later raw FE02 include pending-state byte1; do not call this a clean final mode transition or fresh final OTA clearance.

## K01134
Source `D0256`, original line7, SHA256 `684e352a6e87f5cedff182856e8da3fb96c9f999674da321c3751bbd85cbb8ed`. Status: historical/unadjudicated narrative; English source prose.

Important measured change: 1437unique accepted-counter IMU samples,4542counter gaps over29.527801229s host-read span,797opcode2 records. Lead independently replayed raw checksum-valid target records through the existing IMU decoder and reproduced both totals. Counter delivery fraction1437/(1437+4542)=24.0341194% within the accepted-counter span, not a denominator of all physical sensor acquisitions. CRC814/2598TX=31.3317937%; host ring dropped0. These metrics differ and neither proves full-body or joint controller operation. Both controllers were off. pose_valid=false.

## K01135
Source `D0256`, original line9, SHA256 `684e352a6e87f5cedff182856e8da3fb96c9f999674da321c3751bbd85cbb8ed`. Status: historical/unadjudicated narrative; English source prose.

Installed revision07 unchanged; diagnostic09 Phase158 remains unflashed. Earlier151/154/156IMU0 remains valid historical evidence. IMU return after user powered devices is measured; causal explanation is UNKNOWN. No claim that an offline firmware change fixed hardware. No deliberately issued host19 in this configuration; static radio-run working_commands metadata lists19 and cannot prove its actual native execution/absence. Do not overstate that acquisition was never enabled by any bootstrap/native path.

## K01136
Source `D0256`, original line11, SHA256 `684e352a6e87f5cedff182856e8da3fb96c9f999674da321c3751bbd85cbb8ed`. Status: historical/unadjudicated narrative; English source prose.

Run exit0; session_closed_verified,ram_restored_verified,mpu_restored,trace_controls_restored_verified alltrue. Raw files outputs/phase159/attempt, replay outputs/phase159/analysis.json, hash inventory outputs/phase159/evidence-manifest.json. Frozen runtime contains a hardcoded mailbox-publications path pointing at remote phase148/attempt, so its append log can be contaminated by this run. Per-run raw capture159 and analysis are independent; do not use historical148 mailbox log as a clean148-only artifact.

## K01137
Source `D0256`, original line13, SHA256 `684e352a6e87f5cedff182856e8da3fb96c9f999674da321c3751bbd85cbb8ed`. Status: historical/unadjudicated narrative; English source prose.

No automatic retry. Stop before OTA as requested. The new observation changes the blocker from presently proven IMU0 to intermittent acquisition/command-state/delivery behavior. Phase158 diagnostic pages remain useful for explaining state during good and bad operation, but are not themselves an IMU repair. Exact98032Bhash07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266 still not admitted to production whitelist. Any actual transfer must have fresh service38 live-identity/controller-off admission and explicit user start at the required boundary.

## K01138
Source `D0257`, original line1, SHA256 `d882192e286485a8f813ad69ecce452ed35a5b8621e5afa96e203d556627d006`. Status: historical/unadjudicated narrative; English source prose.

# Phase160 one authorized diagnostic09 OTA attempt

## K01139
Source `D0257`, original line3, SHA256 `d882192e286485a8f813ad69ecce452ed35a5b8621e5afa96e203d556627d006`. Status: historical/unadjudicated narrative; English source prose.

User explicitly said to perform OTA via service module after the required pre-OTA pause, exact candidate disclosure and confirmation Frame/target enabled/controllersOFF. Exact target[TRACKER_SERIAL]/chip[TRACKER_ID]; candidate98032B SHA07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266. This is diagnostic-only schema3/state32 plus schema4/upstream25, not a tracking repair. Bootloader/recovery remain unverified.

## K01140
Source `D0257`, original line5, SHA256 `d882192e286485a8f813ad69ecce452ed35a5b8621e5afa96e203d556627d006`. Status: historical/unadjudicated narrative; English source prose.

Read original OTA45 failure, OTA46 success and indexed capture39 before preparation. Isolated release copied from hardware-successful ota45/release and every frozen OTA46 invocation input hash checked. Sender08d4fc51a2319f73808af18c23156a1097e9fe37164e82ac79aaf72f0e7abca8 unchanged. Module service38 ELF44f4e52b07bbba4c459256c910228d440e25ff0d288238e76047fd21000c4c7c unchanged. Only isolated readiness whitelist adds this exact98032Bhash; main production whitelist unchanged. JSON approved_for_ota staysfalse to avoid implying physical approval; explicit CLI authorization controls the narrow risky exception.

## K01141
Source `D0257`, original line7, SHA256 `d882192e286485a8f813ad69ecce452ed35a5b8621e5afa96e203d556627d006`. Status: historical/unadjudicated narrative; English source prose.

Fresh offline load_job/reverse/size/hash/service38 preflight passed. Frame tracking_watch confirmed headsetconnected/valid and bothcontrollersdisconnected. Exact live product identity check remains inside sender runtime before OtaSession.begin. Effective service profile900us/4727us, slot0, otherpeerspaused, opticsOFF, controller-offguard, captureACK and indexed responsepolls. One fresh result-bearing job; no automatic repeat/resume. Remote invocation phase160/run.py --confirmed-start-diagnostic09. Result pending; no completion or activation claim yet.

## K01142
Source `D0257`, original line9, SHA256 `d882192e286485a8f813ad69ecce452ed35a5b8621e5afa96e203d556627d006`. Status: historical/unadjudicated narrative; English source prose.

Artifacts outputs/phase160/attempt/job, invocation and logs; frozen isolated release and original readiness-before-admission preserved. The prior159 check recovered1437IMUsamples on installed07 but modequery failed later; this is preserved as intermittent-state evidence, not reinterpreted as firmware repair. New runningimage proof requires fresh FE03/FE04 exact nonce/chip after verified reboot; FW189 alone is insufficient.

## K01143
Source `D0257`, original line11, SHA256 `d882192e286485a8f813ad69ecce452ed35a5b8621e5afa96e203d556627d006`. Status: historical/unadjudicated narrative; English source prose.

## Final measured OTA result

## K01144
Source `D0257`, original line13, SHA256 `d882192e286485a8f813ad69ecce452ed35a5b8621e5afa96e203d556627d006`. Status: historical/unadjudicated narrative; English source prose.

All4085/4085 indexed blocks acknowledged, positive finish and final drain: finish_verified, errornull, loaderexit0. Five retry intervals,4085responsepolls,8394rejected unrelated/stale/telemetry observations. Recorded progression event span373.1633701324463seconds (6min13.16s), not exact total RF/loader airtime. Sent98032Bhash07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266.

## K01145
Source `D0257`, original line15, SHA256 `d882192e286485a8f813ad69ecce452ed35a5b8621e5afa96e203d556627d006`. Status: historical/unadjudicated narrative; English source prose.

All cleanuptrue: sessionclosed, SRAMrestored, MPUrestored, tracecontrolsrestored. Remote radio unitinactive; no loader/OpenOCDprocess found afterward. No repeat/resume. Main whitelist stillunchanged. Hardware application block acknowledgement is100% for this attempt; it is not100% RF reception, internalflash SHA readback, boot/recovery proof or jointgame proof.

## K01146
Source `D0257`, original line17, SHA256 `d882192e286485a8f813ad69ecce452ed35a5b8621e5afa96e203d556627d006`. Status: historical/unadjudicated narrative; English source prose.

User asked for normal target-only button off/on, with FrameON/controllersOFF. Awaiting confirmation. boot_verified=false/cadence_verified=false remain; activation of09 not yet proved. Postboot evidence must include new schemas3/4 with strictfreshnonce/chip checks, separatepages not atomic. Running wireversionstill7, so FE02revision7/FW189 alone cannot prove09.

## K01147
Source `D0258`, original line1, SHA256 `eafa43082297c7af0feb27dfdfb981f5291da3af4a1ba47656c26fc4bffc2d2f`. Status: historical/unadjudicated narrative; English source prose.

# Phase161/162: physical diagnostic09 activation and sensor gate state

## K01148
Source `D0258`, original line3, SHA256 `eafa43082297c7af0feb27dfdfb981f5291da3af4a1ba47656c26fc4bffc2d2f`. Status: historical/unadjudicated narrative; English source prose.

No OTA. User trackerON, FrameSSH/trackingvalid, bothcontrollersdisconnected. Phase161 wrapper failure: loader executes as__main__ and imports sibling station_v4_runtime/peer_engine by short name; wrapper patched radio.ram.peer_engine, a separate class. Thus no diagnosticqueries were issued and no activation inference is valid from161. Allcleanuptrue. Preserve161raw separately. This import split may affect historical wrappers using the same pattern; do not retroactively treat their absent proofs as tracker rejection.

## K01149
Source `D0258`, original line5, SHA256 `eafa43082297c7af0feb27dfdfb981f5291da3af4a1ba47656c26fc4bffc2d2f`. Status: historical/unadjudicated narrative; English source prose.

Phase162 fixes short-name module identity before loader import, asserts runtime.PeerEngineis patchedclass BEFORE hardware. Seven meaningful host tests passed: matching application response, ACK-only cannot complete query, badnonce/chip/CRC rejected, boundeddeadline, actualfallbackclass identity. Same RFsettings/module as161: common8800us,slot900,RX700; no sweep/noOTA/no mode setter/no host sensorinit/LED/TXenable/decimationconfiguration. Five queries2,3,4,3,4 with unequal delays. Per-run instrumentation path fixed (oldsource retained).

## K01150
Source `D0258`, original line7, SHA256 `eafa43082297c7af0feb27dfdfb981f5291da3af4a1ba47656c26fc4bffc2d2f`. Status: historical/unadjudicated narrative; English source prose.

Actual target[TRACKER_SERIAL]/chip[TRACKER_ID] returned FE02 withfreshnonce: radio_mode0,wire_revision7,N0,pending0. Both FE03 andFE04 returned exactnew25/32B formats withfreshnonce/chip. This proves newdiagnostic functionality executes on hardware; candidate09activation supported. It is not cryptographic internalflash SHAreadback or bootloader/recovery proof. No claim of game/optical/fullbody validation.

## K01151
Source `D0258`, original line9, SHA256 `eafa43082297c7af0feb27dfdfb981f5291da3af4a1ba47656c26fc4bffc2d2f`. Status: historical/unadjudicated narrative; English source prose.

Two FE03 snapshots1791585112.9565103/1791585114.2865326:
sensor_mode128(0x80),cachedidentity68(0x44),enabledtrue,TXpermission0,samplecounter105,primaryringlatest105,UART0,sensorpostgate1. Mainqueueindices9/8 then13/12 (queuealive, not sensorRTOSoccupancy).

## K01152
Source `D0258`, original line12, SHA256 `eafa43082297c7af0feb27dfdfb981f5291da3af4a1ba47656c26fc4bffc2d2f`. Status: historical/unadjudicated narrative; English source prose.

Two FE04 snapshots1791585113.865576/1791585115.196540:
gate00441=1,gate00fa7=1,gate01fb0=0. The native a930 path skips d53c whenfb0=0; b118sets fb0=0 in sensor80/81 states. Current upstream sampling prerequisite is closed and counters wereunchanged. This is a measured inactive sensorstate, not proof explaining every previous failure or proving deadphysicalIRQ. Radio mode0 and sensor mode80 are DIFFERENT objects.

## K01153
Source `D0258`, original line15, SHA256 `eafa43082297c7af0feb27dfdfb981f5291da3af4a1ba47656c26fc4bffc2d2f`. Status: historical/unadjudicated narrative; English source prose.

Runexit0, allfourcleanuptrue. Raw outputs/phase162/attempt/diagnostic-proof.jsonl andradio-live-records.jsonl. Next justifiedfactor: single nativecommand17/body07 from measuredsensor80 (nativeb118fullinitbranch), thenfreshschema3/4 before19. Require actualmode07/enabled and gate/producerreadback, not transportACK. If producerrestores, one19 permission/start followedbyfieldsand sensorretentionmeasure using confirmedstock-compatible service38/4727 profile. No65flood/reinitloop/OTA.

## K01154
Source `D0259`, original line1, SHA256 `734b080eb3745fd85fbe5b76a2ddacc32663fe1fe633c04a3da46d5c1396cbeb`. Status: historical/unadjudicated narrative; English source prose.

# Game mode1 sensor recovery, Phase167 (2026-10-10)

## K01155
Source `D0259`, original line3, SHA256 `734b080eb3745fd85fbe5b76a2ddacc32663fe1fe633c04a3da46d5c1396cbeb`. Status: historical/unadjudicated narrative; English source prose.

Goal: 98�100% real sensor delivery with two healthy Frame controllers and measured full-body pose. NOT achieved.

## K01156
Source `D0259`, original line5, SHA256 `734b080eb3745fd85fbe5b76a2ddacc32663fe1fe633c04a3da46d5c1396cbeb`. Status: historical/unadjudicated narrative; English source prose.

Installed diagnostic09: 98032 bytes, SHA256 07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266. Activation was proven by fresh FE03/FE04 in Phase162. No new OTA is needed.

## K01157
Source `D0259`, original line7, SHA256 `734b080eb3745fd85fbe5b76a2ddacc32663fe1fe633c04a3da46d5c1396cbeb`. Status: historical/unadjudicated narrative; English source prose.

Phase166 restored radio mode1. Phase167 freshly proved mode1, wire revision7, N1, pending0. Game cadence6000us, reservation600us, RX500us; controllersOFF baseline only. Sensor initially mode0x80, enabled1, transmission0, upstream gates1/1/0. A single native command17/body07 produced sensor mode7, gates1/1/1, primary sample counters84 then110 before transmission. A single native19 then produced634 unique IMU samples,1199 counter gaps,1025 CRC replies/1989 TX=51.5334%,921 retained records,102 ring drops. Counter-span acceptance634/(634+1199)=34.5881%; this is not a complete physical sample-loss denominator.

## K01158
Source `D0259`, original line9, SHA256 `734b080eb3745fd85fbe5b76a2ddacc32663fe1fe633c04a3da46d5c1396cbeb`. Status: historical/unadjudicated narrative; English source prose.

Termination: the host imposed an8-second application deadline for a large FE03 diagnostic query AFTER starting IMU. The query did not return; the loader stopped and all four cleanup checks passed. This is NOT evidence of a physical tracker hang. Sustained game delivery, two-controller coexistence, optical pose and full-body tracking remain unproven.

## K01159
Source `D0259`, original line11, SHA256 `734b080eb3745fd85fbe5b76a2ddacc32663fe1fe633c04a3da46d5c1396cbeb`. Status: historical/unadjudicated narrative; English source prose.

Static evidence: native0x5eda invokes IMU builder0x9458 before queued-record builder0x95c4 at0x5ee8. The latter returns when the queue head exceeds remaining packet budget. Large diagnostic responses may compete with IMU; causal starvation is not established (Phase165 obtained such replies with a different profile). Phase168 changes one host factor: all identity/sensor/gate/producer proofs remain BEFORE19, and no diagnostic queries are queued AFTER19. Radio image, cadence and RX bounds stay unchanged. No sweep, no mode0 tuning, no OTA.

## K01160
Source `D0259`, original line13, SHA256 `734b080eb3745fd85fbe5b76a2ddacc32663fe1fe633c04a3da46d5c1396cbeb`. Status: historical/unadjudicated narrative; English source prose.

Raw evidence: outputs/phase167/attempt including diagnostic-proof.jsonl, actions.jsonl, radio-live-records.jsonl, radio-run.json and cleanup.json. Phase163/164 were pre-radio software/profile admission failures, not RF measurements. Phase165 established native17 sensor recovery, but its stock service profile is historical and not the target game mode.

## K01161
Source `D0259`, original line15, SHA256 `734b080eb3745fd85fbe5b76a2ddacc32663fe1fe633c04a3da46d5c1396cbeb`. Status: historical/unadjudicated narrative; English source prose.

## Phase168 completed: game mode1/N1/6000, no post-start diagnostic requests

## K01162
Source `D0259`, original line17, SHA256 `734b080eb3745fd85fbe5b76a2ddacc32663fe1fe633c04a3da46d5c1396cbeb`. Status: historical/unadjudicated narrative; English source prose.

The bounded60-second run exited0. Fresh nonce FE02 proved radio mode1,N1,pending0 before sensor actions. Native17/body07 opened all three gates and primary counter moved168 to189 before19. No diagnostic queries followed19. Final2956 unique IMU samples,7450 counter gaps (counter-span acceptance28.4067%),3097 CRC replies/7700 completedTX=40.2208%,3019 retained answered slots=39.2078%,78 host ring drops. ControllersOFF. No diagnostic timeout or runtime error; all four cleanup checks true, RAM restored. Final connected=false is the unloaded final snapshot, not evidence of a mid-run disconnect. pose_valid=false.

## K01163
Source `D0259`, original line19, SHA256 `734b080eb3745fd85fbe5b76a2ddacc32663fe1fe633c04a3da46d5c1396cbeb`. Status: historical/unadjudicated narrative; English source prose.

This establishes sustained real IMU production in the required radio mode1 and removes the previous host diagnostic deadline as a blocker. It does not establish99% delivery or two-controller/full-body success. No firmware or MCU image changed. Next work must identify remaining game delivery loss from retained timing, missed-slot and ring evidence; no unsupported parameter sweep or service-mode detour. Raw evidence outputs/phase168/attempt;11 host tests passed.

## K01164
Source `D0260`, original line1, SHA256 `7af7173baa96378fb2dfe9755091accad426c60f61abfdbb32dfba816f290e35`. Status: historical/unadjudicated narrative; English source prose.

# Phase169: mode1 receive-window control

## K01165
Source `D0260`, original line3, SHA256 `7af7173baa96378fb2dfe9755091accad426c60f61abfdbb32dfba816f290e35`. Status: historical/unadjudicated narrative; English source prose.

Goal:90-100% CRC with real IMU, ultimately healthy two-controller full-body. Historical Phase116 mode1/N1/6000/RX800 produced1984CRC/2127TX=93.2769%,2036IMU, but used a different MCU. Phase168 game mode1/N1/6000/RX500 produced3097CRC/7700TX=40.2208%,2956IMU.

## K01166
Source `D0260`, original line5, SHA256 `7af7173baa96378fb2dfe9755091accad426c60f61abfdbb32dfba816f290e35`. Status: historical/unadjudicated narrative; English source prose.

One changed factor versus168: reservation600 to900us, RX500 to800us. Same MCU4020c351..., installed diagnostic09 SHA07918813...,6000us,N1; pre19 fresh nonce mode/sensor/gate/producer proof; no post19 diagnostic requests. ControllersOFF. Hypothesis: receive-window truncation contributes materially; predicts increased CRC. If no material gain, reject window widening as sufficient explanation; do not sweep further widths.60-second bound, watchdog/fault stops and four cleanup proofs. No OTA or mode0 tuning. Raw outputs/phase169/attempt.11 host tests passed.

## K01167
Source `D0260`, original line9, SHA256 `7af7173baa96378fb2dfe9755091accad426c60f61abfdbb32dfba816f290e35`. Status: historical/unadjudicated narrative; English source prose.

Completed60-second bound, exit0, all four cleanup checks true. Fresh mode1/N1/pending0 and producer/gates application proof. 3247 unique IMU, 7866 counter gaps; 3445 CRC/6362 TX = 54.1496%; 121 ring drops. ControllersOFF, no optical pose. Wider RX alone insufficient for90-100%; do not sweep larger windows.

## K01168
Source `D0260`, original line11, SHA256 `7af7173baa96378fb2dfe9755091accad426c60f61abfdbb32dfba816f290e35`. Status: historical/unadjudicated narrative; English source prose.

Additional retained-evidence finding in168:1460 adjacent accepted-slot pairs have median5998.3125us spacing;1192 pairs separated by four slot numbers have median34010.75us spacing, rather than nominal24000us. Missing polls are not timestamped by these records, so BLE blocking, re-request timing and tracker phase remain hypotheses.169 comparison is in outputs/phase169/slot-gap-analysis.json. Native game MCU NORMAL requests use6000us distance; blocked re-request usesEARLIEST. Next: explain the measured extra10ms and repeated missing-slot pattern from MCU/native scheduling before proposing a new RF factor.

## K01169
Source `D0260`, original line13, SHA256 `7af7173baa96378fb2dfe9755091accad426c60f61abfdbb32dfba816f290e35`. Status: historical/unadjudicated narrative; English source prose.

## Direct MCU scheduling evidence

## K01170
Source `D0260`, original line15, SHA256 `7af7173baa96378fb2dfe9755091accad426c60f61abfdbb32dfba816f290e35`. Status: historical/unadjudicated narrative; English source prose.

State ABI word6 is blocked count, incremented on signal5..7. Phase168 final state slots7695/packets3097/dropped78/blocked5824. Phase169 slots6361/packets3445/dropped121/blocked4297. Blocks are not disjoint missed transmissions or proof that BLE controllers caused losses; controllers were OFF. Direct evidence establishes frequent scheduling refusals alongside the retained34ms gap pattern. Controller-OFF does not imply radio idle, as Phase78 previously measured. Requested controllersON for stock-only fresh scheduling measurement before joint profile admission; no RF running while waiting.

## K01171
Source `D0261`, original line1, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

# Phase170 baseline and Phase171 compact joint candidate

## K01172
Source `D0261`, original line3, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

Target full-body with healthy two controllers;98-100% real sensor delivery remains unproved. Installed diagnostic09 SHA07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266. NoOTA.

## K01173
Source `D0261`, original line5, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

Phase170 stock-only native272B sampler, noRF session or flash.30seconds,102 calls/6528 raw/6468 zero-span accepted rows. Both controllers connected/valid on initial live OpenVR sample;30-second metrics both0invalid/0freeze. Radio busy3303/6468 observations; two busy BASE groups0x2e7ca500(1578) and0x208bbd00(1725), MODE4. Three temporal segments on6000us grid share700us quiet using>=3 observations/bin and busy<=5%; whole-period blind bins remain, so this is local-window evidence, not whole-period coverage. All4sampler cleanup checks true.

## K01174
Source `D0261`, original line7, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

Phase171 proposed bounded100-second joint game test. Same2043B MCU4020c351..., radio mode1/N1/6000. Bootstrap uses900/RX800 for product and fresh nonce mode/sensor/gate/producer diagnostics. Single native17/body07 only if sensor inactive. Command19 held until fresh in-session16-second ordinary native samples admit>=700us quiet via covered_quiet_window (its own stronger admission), then compact500us reservation/RX400 with100us lead and phase locked6000. Short340us keepalive sampling occurs only after long diagnostics; no long status queries after19. Single native19, no65 resynchronization flood. Controller invalid/frozen poses are recorded, per user bench request; watchdog/MCU/link faults still stop. Wider900 bootstrap is transitional only and cannot count as controller acceptance. Retain separate stage metrics and raw controller records.

## K01175
Source `D0261`, original line9, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

Change justified by measured700us local quiet and retained shortIMU observations~333-423us:500reservation+200padding fits700. RX400 may lose larger/late IMU records; no guarantee90%. This test evaluates real compact phase-locked delivery, not a radio sweep.14 offline tests passed, including mode0 rejection, nonce/source/application gates, no19 before phase, and690us rejection/700us admission. Frozen staged_config validation passed before hardware. On hardware, failure to discover the required window must stop before19; do not relax threshold.

## K01176
Source `D0261`, original line11, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

Evidence outputs/phase170/baseline30, analysis.json; candidate/raw outputs/phase171.

## K01177
Source `D0261`, original line13, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

## Phase171 hardware result

## K01178
Source `D0261`, original line15, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

Fresh mode1/N1/pending0.1664 ordinary native rows,124 excluded own/crossed; stronger in-session temporal admission found700us quiet, center5950us. S3 compact500/RX400 phase admitted28.29s; continuous responses admitted S4 at31.39s. IMU not started: actual sensor-mode7/enabled1 but FA7=0, primary counter and ring remained251 in both post17 snapshots. Host application gate rejected19; no command19 publication. All4cleanup true. This is a concrete upstream sensor clock/TIMER2 blocker; it does not establish tracker RF failure or physical hang.

## K01179
Source `D0261`, original line17, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

Native provenance: command21 table callback8d40 validates selector1/2 and chip identity20000efe; then78f4 and7c44 initialize/restart TIMER2. TIMER2 callback7760 for event0x148 setsFA7=1 at785a. Disconnect64fe calls7bd4, stopping TIMER2 and clearingFA7. Command17 independently opensFB0 but does not itself restartTIMER2. This is a static causal dependency, not yet live repair proof. Phase172 adds ONE chip-guarded native21 [01,00,[TRACKER_ID]] before17, followed by freshFA7 and producer readbacks. Same mode1/N1/compact500/RX400/phase gate.19 offline tests passed (native entry guards and actual compare callback included; Unicorn prints known Windows mem_map diagnostics). No OTA, arbitrary RAM writes, or repeated init loop.

## K01180
Source `D0261`, original line19, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

## Phase172 final: CRC milestone, sensor delivery blocked

## K01181
Source `D0261`, original line21, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

Joint controllersON, fresh mode1/N1/pending0, noOTA. Elapsed29.924630s, fresh16-second ordinary sample1540 retained rows after124 own/crossed exclusions, three-segment admission found800us quiet (center5300us). Compact500/RX400 phase applied at26.862836s; S4 at29.859398s. Final hardware CRC4987/completedTX5016=99.421850%; NO IMU generated/received in this run. This is KEEPALIVE/CONTROL CRC success across the short whole run, NOT99% sensor delivery or sustained post-lock coexistence. Only~3seconds used final compact phase.

## K01182
Source `D0261`, original line23, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

Retention is a separate failure:1998 answered slots retained,2983 host-ring drops. S2 measurement alone2169drops; S3 final phase184drops. Do not present CRC as delivered application samples. Controller1 S3 unchanged pose2189.083487ms, valid flags remain200/true; stationary-pose versus real freeze not discriminated. Controller2 changed99/99 with0invalid. Joint controller health is NOT accepted. User record-only policy honored; no reset from pose metric.

## K01183
Source `D0261`, original line25, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

The one native21 publication was01003eed1979. Subsequent fresh FA7 remained0; native17 changed sensor80 to7 andFB0 to1, but primary counter/ring remained112/112. Command19 deliberately NOTsent; abort reason sensor producer/gate application proof failed. All four cleanup checks true. No physical tracker hang was established.

## K01184
Source `D0261`, original line27, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

Command21's live repair hypothesis FAILED: no FA7 restoration or producer progression. Its handler identity/selector guard and native compare2 FA7 setter are statically/emulator verified, but neither transport ACK nor code capability proves actual command callback execution. Additional native wire-dispatch tests passed (seq0 accepted/seq1 rejected in a fresh model);21 offline cases total. Do not repeat unchanged21 or sweep its sequence numbers. Actual running sequence state, callback delivery, TIMER2 configuration/interrupt event and intervening stop path remain unknown.

## K01185
Source `D0261`, original line29, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

### What is achieved

## K01186
Source `D0261`, original line31, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

- Diagnostic09 activation proven by FE03/FE04; mode1 remotely proved, N1/6000 game retained.
- Real sensor recovery and sustained2956/3247IMU measured in168/169 WITHOUT controllers.
- Healthy controller-only baseline170 and fresh joint quiet windows700/800us in171/172.
-99.42185% hardware CRC with BOTH controllers connected in172, but control/keepalive only.

## K01187
Source `D0261`, original line36, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

### Remaining work, ordered

## K01188
Source `D0261`, original line38, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

1. Resolve actualFA7=0 branch using native command21 execution/sequence/TIMER2 dependencies, not another RF scan. Reuse171/172 failed hardware evidence. No new OTA without exact-image review and required user pause.
2. Require fresh sensor counter progression before19; then sustain real IMU in the admitted joint phase. Recover host retention (2983drops) and measure CRC, counter completeness, rate and latency independently.
3. Discriminate unchanged pose from real controller freeze using controlled movement; require both controllers healthy throughout final streaming phase.
4. Measure identified optical LED poses, current-boot camera/world/time calibration, IMU alignment and fusion; publish real moving tracker pose in SteamVR. pose_valid=false until this passes.

## K01189
Source `D0261`, original line43, SHA256 `665c47eadce81d9891344307513a17147807449f19d40eb55482c7e4f51c5f9f`. Status: historical/unadjudicated narrative; English source prose.

Raw outputs/phase170/baseline30,phase171/attempt,phase172/attempt. Machine handoff outputs/phase172/state-for-ai.json. No RF active after cleanup; controllers can remainON.

## K01190
Source `D0262`, original line1, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

# Phase173 continuation validation, 2026-10-10

## K01191
Source `D0262`, original line3, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

## Result and scope

## K01192
Source `D0262`, original line5, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

Offline continuation only. No SSH, SWD, RF, OTA, controller toggle or tracker command was issued. Phase172 remains 4987/5016 control CRC (99.4218501%), IMU0. Full-body goal remains unmet.

## K01193
Source `D0262`, original line7, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

## Peripheral name correction

## K01194
Source `D0262`, original line9, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

**Additional correction from continuation174:**0x40001104 is RADIO.EVENTS_ADDRESS, not EVENTS_END (END=0x4000110c). Native767c therefore connects ADDRESS to TIMER1.CAPTURE2. Primary RADIO table:https://docs.nordicsemi.com/r/bundle/ps_nrf52832/page/radio.html . Earlier END labels in173 text/JSON are historical errors; raw endpoint values remain valid. Capture/compare concurrency still unmodeled. Native760c also configures COMPARE2->CLEAR and COMPARE2->PPI group-disable channels, initially disables that group and assigns its mask. A single767c endpoint test is insufficient to model the complete lifecycle.

## K01195
Source `D0262`, original line11, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

The prior Phase173 handoff calls 0x40009000 TIMER2. Nordic's nRF52832 register map names it **TIMER1**; TIMER2 is 0x4000a000. All earlier numerical dependency addresses remain unchanged. Use addresses when comparing older records.

## K01196
Source `D0262`, original line13, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

Primary specification: https://docs.nordicsemi.com/r/bundle/ps_nrf52832/page/timer.html . Capture copies the counter to CC[n]; compare is generated when the counter increments to equality. This supports shared-register interference as a hypothesis, but does not establish same-cycle capture/compare ordering or the live failure cause.

## K01197
Source `D0262`, original line15, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

## Native execution evidence

## K01198
Source `D0262`, original line17, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

`outputs/phase173/test_native_timer1.py` executes unmodified 0x7b50 and SDK 0x3868 with an explicitly mocked descriptor at 0x20000f90 (base0x40009000, instance1). It verifies MODE0, BITMODE3, PRESCALER0, CC2=500000, START1, INTENSET0x3f0000. Nominal first compare interval is 500000/16000000=31.25ms. It also executes 0x7bd4 and verifies STOP1, SHUTDOWN1 and FA7 reset0.

## K01199
Source `D0262`, original line19, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

Combined validation with Phase172 native dispatch/callback tests: **9 passed in 0.18s**. Log: `outputs/phase173/native-timer-validation.txt`. First invocation printed Windows/Unicorn access-violation diagnostics despite reporting nine passes; an immediate logged rerun completed without those diagnostics. Neither invocation simulates physical clocks, capture/compare concurrency, NVIC IRQ delivery or live command acceptance.

## K01200
Source `D0262`, original line21, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

## Pending audit recovery
