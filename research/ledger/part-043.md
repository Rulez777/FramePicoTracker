# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K03361
Source `D0398`, original line2673, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Controller schedule sampler85 prepared, 2026-10-09
Read docs/PHASE85_CONTROLLER_SCHEDULE_PREPARED_2026-10-09.md. Hardened77 exact244B module, runner sealed19c719e0...e2017, proper recovery key/fault retention,4exactbinary checks pass. Prepared only; controllers currently disconnected. No MPSL/RADIO session; do not infer robust quiet gap or individual ownership from BASE0 alone. Dense900us service does not fit a600us quiet gap.

## K03362
Source `D0398`, original line2676, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Phase89 external review: Grok disposition (original reference: `docs/PHASE89_GROK_REVIEW_DISPOSITION_2026-10-09.md`). Raw review preserved;430us-slot contradiction and pre-deadline END claim rejected. State continuity/long mode replies remain implementation blockers; no joint RF claim.

## K03363
Source `D0398`, original line2678, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase109 controller-return transition candidate
Read PHASE109_TRANSITION_AND_OBSERVER_CANDIDATE_2026-10-09.md (original reference: `docs/PHASE109_TRANSITION_AND_OBSERVER_CANDIDATE_2026-10-09.md`). A rejected incomplete BLE coverage (933 accepted rows, one empty bin); B42000 stuckS1 with secondcontroller invalid and was cleanly interrupted. Both restored. New isolated2044B candidate uses >=3RTCtick observer spacing and opt-in connected-only phase-start under unchanged12s grace/8s acquisition; strict two-controller150ms guard before19 unchanged.47offline tests passed. Candidate is prepared, not hardware-validated. Stock-only repeat56calls/3584rows restored cleanly. NoOTA; near100% coexistence unachieved.

## K03364
Source `D0398`, original line2682, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Phase109 hardware update: joint candidate reachedS2 and coveredall30bins (928raw/908accepted, min13/bin), but quiet<=5%busy only300us; rejected beforephase lock/19/IMU. S2keepalive190/197TX=96.45% doesnotprove streaming. All4cleanuptrue. Stock15s exploratory900usrun hasundercoveredbins and isnotadmissible. See109report/result-analysis; nextfullycoveredstockcomparison andschedulerinteraction investigation, no unchangedretry.

## K03365
Source `D0398`, original line2685, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phase110 native-only comparison
Read PHASE110_STOCK_AND_STRIDE_COMPARISON_2026-10-09.md (original reference: `docs/PHASE110_STOCK_AND_STRIDE_COMPARISON_2026-10-09.md`). Stock60:214calls/13696raw/13665zero-span;61metricwindows both0invalid/0freeze;6segment common650us at3000 (RXbudget388.965). Stock15 withexact109observer:48calls/1536raw/1516accepted;17windows both0invalid/0freeze. Allrestored. Wholeperiod30bincoverage is incomplete in these stockcaptures: exploratory1100/700us isnotadmissible. Joint109300us vsstock650us supports nextactive-session scheduling control, causalmechanismunknown. NoRF/OTA in110, jointgoalunmet.

## K03366
Source `D0398`, original line2689, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Native UART activation / revision08 offline candidate
Read FW189_UART_ENABLE_AND_REV08_CANDIDATE_2026-10-09.md (original reference: `docs/FW189_UART_ENABLE_AND_REV08_CANDIDATE_2026-10-09.md`). User-authorized0x50enable onexisting07: one logical500100 request, native5000 response(raw43/slot361), target[TRACKER_ID], continuedresponses, all4cleanuptrue. USB/HID/Ports29before/during/after,noaddedIDs. UARTelectrical/TypeCroutingunverified; noUARTdisable sent; rebootpersistenceunknown. Nativeflag20001d45 affectsotherpaths. Offline08candidate97864B SHA1e111618...7078 defaultsUARTon atsubsysteminit;7mockedtests/reversepass; NOTinstalled/NOTOTAapproved. Exactinstalled07is40941b34...56d5, notsuperseded6c702e51. NoOTA inUARTactivation.

## K03367
Source `D0398`, original line2704, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Latest 130-132 clean boot and sensor control (original reference: `docs/PHASE130_132_CLEAN_BOOT_SENSOR_CONTROL_2026-10-09.md`):130OFF1250IMU/freshFrameproof,cleanuptrue;131S1HMDguardstop;132S1benchpolicy fixed/tested, joint active, no result claim. Tracker07/noOTA.

## K03368
Source `D0398`, original line2706, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Material132:5461IMU in~69.814sS4 butcontrollerhealthFAIL; allcleanuptrue. NativeStandby/UserPresence0 verified AFTERunload; headsetONrestoredall3stockvalid.133 awake-headset control active. Source1880 isphasecorrection, notresidualerror. See130-132report.

## K03369
Source `D0398`, original line2710, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Latest138:actualfresh800quietgate600/RX540,98.6881%CRC/IMU0,allcleanuptrue,last99BOTHhealthy.139held17sensor09->07thenheld19 active; firmware07,noOTA. Mode1verified butsensorstateunproven. Do not repeat138unchanged.

## K03370
Source `D0398`, original line2712, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Latest139gate:700quiet<800, no17roundtrip/19executed,cleanuptrue.140smaller500/RX440 with700freshgate preservesmargins; held17sensor09->07/19 active; noOTA/07unchanged.137 remoteFrame restore withcontrollersON nowproven.
# Phase157 final dual review

## K03371
Source `D0398`, original line2715, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Phase160ACTIVE: user explicitly authorized OTA via service module after prestart pause. One exact diagnostic09/98032B/SHA07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266 attempt through successfulOTA46 frozen release/service38 (900/4727), live target guard, controllersOFF. Isolated exact whitelist admission only; main whitelist unchanged. No autoretry/resume. Read docs/PHASE160_SERVICE38_DIAGNOSTIC09_OTA_2026-10-09.md. Completion/activation pending; fullbody unmet.

## K03372
Source `D0398`, original line2717, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Latest159: fresh30s preOTA identitycheck, target[TRACKER_SERIAL]/FW189/chip[TRACKER_ID], installed07 produced1437uniqueIMU/4542countergaps (24.0341% accepted-counter span),814CRC/2598TX31.3318%,ringdrop0, allcleanuptrue. Initialmode0proof but finalmodequeryfailure/connectedfalse; no finalclearance. NewIMUreturn causalUNKNOWN, nofirmwarechange/noOTA. English measured report (original reference: `docs/PHASE159_PRE_OTA_IDENTITY_AND_IMU_RETURN_2026-10-09.md`). Both controllersOFF; fullbody unmet. PausebeforeactualOTA remains.

## K03373
Source `D0398`, original line2719, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Phase158FINAL: both repeat reviewers returned, no code blocker; lead accepted current gate classification only, rejected physical-cause/atomic-two-page overclaims. 96offline cases,98032B/SHA07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266; UNFLASHED. Read docs/PHASE158_UPSTREAM_GATES_AUDIT_2026-10-09.md final disposition. Pause/call user beforeOTA; enableFrame+target, controllersOFF for service38. Whitelist unchanged/noRF started/fullbody unmet.

## K03374
Source `D0398`, original line2721, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Current158: three proven upstream sampling gates added as fixed schema4 reply25B; schema3 remains32B. Candidate98032B SHA07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266, 96offline cases including4085/4085 exact-image transfer, UNFLASHED. Both repeat audits dispatched on isolated source-only package. English audit/state (original reference: `docs/PHASE158_UPSTREAM_GATES_AUDIT_2026-10-09.md`). User requires call/pause BEFORE OTA and will enable devices then. NoRF launched. Old157 job flags found inconsistent with authorization, evidence preserved and reset; whitelist05/07 unchanged.

## K03375
Source `D0398`, original line2723, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Both Grok 4.7 medium and Gemini returned reviews of the 97976-byte diagnostic candidate. Read dual review disposition (original reference: `docs/PHASE157_DUAL_REVIEW_DISPOSITION_2026-10-09.md`) before any OTA decision. No code/ABI blocker reported; first-branch diagnosis only, three proven upstream sampling gates omitted. Candidate unflashed, approved_for_ota=false. Gemini reported unauthorized failed execution attempts; do not treat its hook or SAFE wording as authorization. User confirms headset and tracker were off. Full-body goal remains unmet.

## K03376
Source `D0399`, original line1, SHA256 `c09295179f1d57f6149354c39d1e9f34bc68624134b9ef1087f521d1bdd76c9a`. Status: historical/unadjudicated narrative; English source prose.

CURRENT201: User authorized current diagnostic10 host work, NO OTA.199attempt2 fullgame N1/6000/700quiet/500RX440 yielded6756uniqueIMU/5745gaps,16269CRC/16346TX=99.52894%,6810hostringdrops;exit0/all4cleanuptrue. Controller1full101windows maxinvalid42/100,freeze417.389ms, includingpost19;controller2healthy. New schema12 was unnecessary for this path.200port174compactbatch(runtime5integritycases passed,actualguard3cases): firstlaunch prehardware unboundservice fixed in separateattempt2; secondhardware stopped BEFORE10/19 becauseSHORTS1/1,allcleanuptrue.201exactinstallednativeaudit: native10 does notreadSHORTS; currentCC2capture436810->439442 withstableSTART/PPI provides alternative runningtimer inference,notdirectcounter/IRQ/starvationproof. Actualsavedpages admit,flatCC2/closedguard/changedorderreject.201host-current bounded100s attempt ACTIVE;samefirmware/module/RF,compactbatch;noOTA. Read docs/PHASE199_CURRENT_FIRMWARE_DIAGNOSTIC_AUDIT_2026-10-10.md,PHASE200_HOST_RETENTION_FACTOR_2026-10-10.md,PHASE201_CAPTURE_ADMISSION_AND_RETENTION_2026-10-10.md. Goalstillunmet/pose_valid=false. No unchangedautomaticreruns.
CURRENT199: User REJECTS new diagnostic11 OTA; exhaust installed diagnostic10, no unsupported/empty probes. Phase198 OTA proposal withdrawn. Saved hardware-only audit57retained records: schema3/32B528us; schema4/25B423-424us(two queries); schema11/32B440 or528us. These are slot-relative END callback observations, NOT pure airtime; new schema12 has NO measured timing/delivery improvement. Do not assume universal size-to-latency. CurrentFW alternative: full native10 prerequisite+producer3/4/3 in wide bootstrap, explicit S0 completion gate, same-link N1/6000 transition without tracker reinit/reset, existing short schema4 after phase admission, one19; producer proof age must be recorded and NOT called fresh after transition. Preserve700quiet/500RX440 and reconnect abort. Host-only ordering prepared in outputs/phase199/host-proposal: bootstrap3/4/3 completion gate, existing4afteradmission, explicitcounterproofage. Launcher preparation-only; invocation/mode-refresh/controllerpolicyreview pending. Changedfiles syntax parsed; no synthetic tests/newRF/OTA. Read docs/PHASE199_CURRENT_FIRMWARE_DIAGNOSTIC_AUDIT_2026-10-10.md. Fullbody unmet/pose_valid=false.
CURRENT198: Diagnostic10 ACTIVE191. Native10 in195 openedFA7/allgates and producer60->104;19660sreal1959IMU/9506gaps onmode1N2/8800, controller2healthFAIL(maxinvalid41/99,freeze416.5ms),allcleanuptrue.197N1/6000quiet700<800 for600/RX540 => refusedbefore10/19; no unchangedretry. Long32Bqueries528us don'tfit400RX. LocalUNFLASHEDshortschema12candidate11:99028B/SHA7d921ddfca7941f2d5835e18ffb6300fc7793b430b964de6a56105373547cd41,25Bcounter/ring/gatebits;7shortnative+14oldpage+2modeABIchecks passed; GrokassemblyauditCOMPLETE/nosourceblocker,narrowonly. Unapprovedisolatedoutputs/phase198/ota-proposal/default95% restored, noattempt/result,noOTA. Needexplicitnewimageprestartpause; neverinherit189qualityoverride. Read docs/PHASE195_NATIVE10_PRODUCER_RECOVERY_2026-10-10.md,PHASE196_197_REAL_IMU_AND_GAME_ADMISSION_2026-10-10.md,PHASE198_SHORT_PRODUCER_CANDIDATE_2026-10-10.md. pose_valid=false/fullbodyunmet/noRFactive.

## K03377
Source `D0400`, original line1, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

CURRENT201: User authorized current diagnostic10 host work, NO OTA.199attempt2 fullgame N1/6000/700quiet/500RX440 yielded6756uniqueIMU/5745gaps,16269CRC/16346TX=99.52894%,6810hostringdrops;exit0/all4cleanuptrue. Controller1full101windows maxinvalid42/100,freeze417.389ms, includingpost19;controller2healthy. New schema12 was unnecessary for this path.200port174compactbatch(runtime5integritycases passed,actualguard3cases): firstlaunch prehardware unboundservice fixed in separateattempt2; secondhardware stopped BEFORE10/19 becauseSHORTS1/1,allcleanuptrue.201exactinstallednativeaudit: native10 does notreadSHORTS; currentCC2capture436810->439442 withstableSTART/PPI provides alternative runningtimer inference,notdirectcounter/IRQ/starvationproof. Actualsavedpages admit,flatCC2/closedguard/changedorderreject.201host-current bounded100s attempt ACTIVE;samefirmware/module/RF,compactbatch;noOTA. Read docs/PHASE199_CURRENT_FIRMWARE_DIAGNOSTIC_AUDIT_2026-10-10.md,PHASE200_HOST_RETENTION_FACTOR_2026-10-10.md,PHASE201_CAPTURE_ADMISSION_AND_RETENTION_2026-10-10.md. Goalstillunmet/pose_valid=false. No unchangedautomaticreruns.
# PICO / Steam Frame — complete continuation handoff after Phase189 OTA

## K03378
Source `D0400`, original line4, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

CURRENT198: See AGENTS newest entry and docs/PHASE195_NATIVE10_PRODUCER_RECOVERY_2026-10-10.md,PHASE196_197_REAL_IMU_AND_GAME_ADMISSION_2026-10-10.md,PHASE198_SHORT_PRODUCER_CANDIDATE_2026-10-10.md. Native10 recoveredproducer,1961959realIMUbut9506gaps/controller2healthfail,197quiet700refused800needed. Newshort25Bschema12candidate1199028B/SHA7d921ddfca7941f2d5835e18ffb6300fc7793b430b964de6a56105373547cd41UNFLASHED/unapproved/Grokassemblyauditcomplete,narrowonly.23nativeABIcasespassed; nohardwaretimingclaim. Isolatedproposaldefault95%,noattempt/result. ExplicitnewimageOTAstartpause required;189overrideNOTreused. NoRFactive/fullbodyunmet/pose_validfalse.

## K03379
Source `D0400`, original line6, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

CURRENT191 2026-10-10 supersedes activation-pending below: fresh stock2 and all ordered observer replies prove diagnostic10 activated, exact target serial/chip/nonce/CRC/checksum. NoOTA/no19, exit0/all4cleanuptrue. Individual FA7=1/FB0=1/START98/SET109 vsSTOP1; event101->112 and SHORTS1->0 mean unstable bracket/classifierUNRESOLVED, not capture-starvation proof. Read docs/PHASE191_DIAGNOSTIC10_ACTIVATION_2026-10-10.md and outputs/phase191/attempt2. ControllersOFF; next game mode1 fresh producer/gates and BOTH-controller acceptance; fullbody remains unmet. Do not repeat stock observer unchanged or OTA.

## K03380
Source `D0400`, original line8, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Updated 2026-10-10, Phase190 handoff. This is the REWRITTEN original Phase173 entry point. Read K:/AI/pico/AGENTS.md first, then THIS document. Earlier CURRENT headings in phase reports are historical snapshots. This document supersedes their claims that diagnostic10 is unapproved/untransferred or that original Grok/Gemini reviews remain pending.

## K03381
Source `D0400`, original line10, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

## 1. Immediate state and next action

## K03382
Source `D0400`, original line12, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

- Exact diagnostic10 OTA completed: 4120/4120 blocks, finish_verified, 2 retries, no transfer error, progression335.96822476387024 seconds (~5m36s). Runner exit0; all four cleanup flags true.
- Image:98868 bytes, SHA256040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d.
- Target [TRACKER_SERIAL], little-endian chip[TRACKER_ID]. Do not substitute another target.
- Transfer was in freshly proved STOCK radio mode0/revision7/N0/pending0, service38, interval4727us, reservation900us, M2, slot0, peers1/2 paused, opticsOFF, no BLE phase, live controllersOFF watchdog.
- USER NOW REPORTS TRACKER HAS BEEN TURNED ON after being asked to powercycle ONLY the tracker. This is a physical user report, NOT measured activation. Do not ask to powercycle again without a concrete reason.
- boot_verified=false and cadence_verified=false in saved OTA result. No post-powercycle diagnostic10 query has been performed. Do not claim image10 booted merely from FW189 or revision7.
- NEXT: fresh live identity and observer-page activation verification, then ordered sensor/timer diagnosis. NO repeat OTA. No new firmware image is authorized.
- Last checked after OTA: no Roy RF loader/OpenOCD process active; frame-pico-radio.service inactive. This is a snapshot, not a guarantee after time passes. Current controller state after user's tracker-on report has not been rechecked.
- Full-body goal NOT achieved. Last BOTH-controller measurement remains Phase172:4987/5016=99.4218501% CONTROL CRC, IMU0. This must not be replaced by OTA/control percentages or controllers-OFF sensor observations. Keep pose_valid=false.

## K03383
Source `D0400`, original line22, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

## 2. User operating contract and authorization chronology

## K03384
Source `D0400`, original line24, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

User requires sustained90–100% REAL UNIQUE IMU with TWO healthy Steam Frame controllers and ultimately real optical SteamVR full-body tracking. Initial handoff's98–100% wording is historical; latest explicit minimum goal is90–100%. No blind sweeps, empty repeated probing, arbitrary sequence increments, unsupported commands, or meaningless toggles. Use a factor derived from native/static/measured evidence with a discriminating expected result. Keep English quantitative records for AI; concise Russian progress for user.

## K03385
Source `D0400`, original line26, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

ALL OTA transmissions and connection quality tests MUST use STOCK mode0. Game sensor work returns to mode1/N1/6000 after activation. Do not conflate radio mode1 with sensor mode7. Service/stock is for OTA and transition, not a substitute for both-controller game acceptance.

## K03386
Source `D0400`, original line28, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Original explicit OTA permission this session: check quality first; start if>=95%; report loading status. Phase180/181 incorrectly counted quality BEFORE stock proof. Lead admitted and corrected this. Their percentages are NOT valid confirmed-stock tests.

## K03387
Source `D0400`, original line30, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Later user explicitly said start with statuses AFTER being told confirmed-stock quality80.91% was below95% and transfer had not begun. Phase189 recorded this newer instruction as superseding the blocking95% condition for ONE exact diagnostic10 attempt. No hidden claim of95% admission: actual prestart unique-reply ratio202/274=73.72262773722628%. Only this percentage gate was waived; freshstock0/N0,5seconds/256TX, capability3, final different-nonce stock proof, exact image/target, controllerOFF and cleanup gates stayed mandatory.

## K03388
Source `D0400`, original line32, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Earlier instruction to shut down PC was tied to finishing and resuming tomorrow; PC was not shut down, and user's later active OTA/handoff requests superseded that workflow. Do not shut down during continuation. Preserve current work; very dirty repository contains authored/preexisting changes. No destructive git restore/reset/checkout. Durable project knowledge belongs in AGENTS.md, not Codex-only memory.

## K03389
Source `D0400`, original line34, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

## 3. Exact artifacts, profiles and access

## K03390
Source `D0400`, original line36, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Project K:/AI/pico; PowerShell; project Python K:/AI/pico/.venv/Scripts/python.exe. Frame SSH steamos@[DEVICE_IP], root /home/steamos/pico-ram-work, venv /home/steamos/pico-ram-work/venv/bin/python. Verify live accessibility before hardware action.

## K03391
Source `D0400`, original line38, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

| Artifact | Exact identity |
|---|---|
| Transferred diagnostic10 | outputs/phase176/candidate10-timer-observer/fw189-dual-mode.bin;98868B; SHA040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d |
| Executed OTA package | outputs/phase189/ota-proposal; remote /home/steamos/pico-ram-work/phase189/ota-proposal |
| OTA package archive | outputs/phase189/package.tar.gz; SHA1af82c20c383c7d9ef617f3fc966bd72c62b9826c1dc37b98372ae57a2cbf8ba |
| Pre-OTA diagnostic09, activation previously proved | outputs/phase158/candidate09-state32-gates/fw189-dual-mode.bin;98032B; SHA07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266 |
| Original tracker stock | outputs/swift189-image.bin;96532B; SHA477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89 |
| Tracker disassembly | outputs/swift189-full.asm; SHA4ed45fa85967e4b004181b4927ee0493fe0a92c0d6c3116e3ca276c0d103c553 |
| Executed service38 Roy ELF | proposal/roy_station.elf; SHA44f4e52b07bbba4c459256c910228d440e25ff0d288238e76047fd21000c4c7c |
| Service38 code |2036B; codeSHAb14a214100bbf00c5bd25592c753eedaa4326375343699d6748933c35be2b11a; codebudget2045B |
| Frozen bootstrap profile | proposal/bootstrap205.profile.json; SHAc9153f3e947e5a647117ef8d1d6cdf9418ba8f37381f0168c1125fa9a4d39169 |
| Roy stock used by loader | stock.bin; SHA0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50 |
| Historical game Roy ELF | SHA4a386160076ec4284c6f93086fd568360e837304c9d5411877a18e2b063b58ef;2043B,only2B spare |

## K03392
Source `D0400`, original line52, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Game code base2001151c, Thumb entry2001151d; game codeSHA4020c351db7e62fc68ef77663695f3f53c7e583bc51fd3e78cd3113bc215b299. Phase119 source outputs/phase119/source/roy_ram_station_v4.c SHA796a13e5a033a9ee31ab3991f8d16dab6421d8ecdffd9cf489f7f464fe96621d. Do not conflate game and OTA ELF/code provenance.

## K03393
Source `D0400`, original line54, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

## 4. Session chronology, measured outcomes and mistakes

## K03394
Source `D0400`, original line56, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

| Phase | What happened | Result/limit |
|---|---|---|
|173 continuation|Native dependency validation, Grok audit recovery|TIMER naming corrected;9 native/dispatch tests passed; original pending review later recovered. No new RF in173–176.|
|174|Isolated compact host retention preparation, no dense_frame|8 tests; historical logger path corrected for future copy; not deployed and no hardware gain claim.|
|175|Proposed native10 control audited|NOT admitted/NOT run: missing live running-timer/PPI/E08/F98 conditions could make it no-op.|
|176|Ordered observer10 built; initial unaligned/u8-history variants rejected before OTA|Final98868B hash above;75 offline cases including native staging4120blocks/finish/hash,48 synthetic retries. Grok exact observer review complete.|
|177|Second Grok/Gemini audit|Host launch blocked pending approval/raw-evidence/hash fixes. No hardware in177–179.|
|178|Confirmed host repairs|44 offline cases reported; host binding/raw admission/controller and observer collector repaired.|
|179|Final independent audits, theory checks and narrow closure|61 focused cases passed2.96s; final Grok/Gemini verdicts completed. Not general hardware safety proof.|
|180|User's conditional>=95% permission; thresholds strengthened|43 repair cases passed2.42s after resealing stale hash pins; initial failure log retained. One hardware attempt1190/1471=80.89734874235214%,30.701s; no mode proof, no OTA. Wrong quality-before-stock order.|
|181|Direct user repeat request, separate exclusive package|1211/1482=81.71390013495277%,30.651s; no mode proof,0OTA; allcleanuptrue. Also invalid as confirmed-stock test.|
|182|Stock-before-quality baselines implemented, capability3 still first|9 tests0.13s; capability query timeout8.658s; no quality/noOTA; allcleanuptrue.|
|183|Previously measured common8800/900 stock transition fromPhase159 reused, no OTA|Exit0; rawFE02 stock0/rev7 initiallyN0 thenN1; verifiedstock true;767/2515 cumulative replies are transition counts, not service38 quality; allcleanuptrue.|
|184|Corrected gate after actual stock transition|Capability3 timeout8.660s; no quality/noOTA; allcleanuptrue.|
|185|Short schema2 first, capability deferred until after quality|9 tests0.15s; mode proof timeout8.666s; no quality/noOTA; allcleanuptrue.|
|186|User explicitly requested repeat|No source/image change; mode proof timeout8.680s. Hardware423CRC/432TX=97.9166667%,1 ringdrop; qualityNOTmeasured; noOTA; allcleanuptrue.|
|187|Saved-data analysis, no RF|Established physical RX present. Initial analysis read ordinary live-record log only and was INCOMPLETE; full raw_records later show hundreds of IMU packets, not merely ACK/product events. Sequence/ACK hypothesis NOT confirmed live cause.|
|188|Confirmed inherited automatic19 in OTA host setup; targeted removal|10 tests0.18s. FIRST freshstock0/N0 proof received~0.118s; measured1170/1446=80.91286307053942%,30.687s; below95%, noOTA, allcleanuptrue.|
|189|New direct START overrides95% condition; exact transfer with status updates|11 tests0.16s; rawstock2/capability3/finalstock2 proof, actualadmission202/274=73.72263%;4120/4120,2retries,finish_verified;335.968s; allcleanuptrue/exit0. Activation still unmeasured.|
|190|User reports tracker turned on; asks English handoff rewrite|Documentation only; NO post-powercycle hardware query.|

## K03395
Source `D0400`, original line77, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Do not repeat unchanged attempts. Every failed attempt/result/start-ticket remains preserved. Separate phase paths were used; no old consumed attempt was reset or reused. No tracker firmware was written until Phase189. Many intermediate edits were HOST admission/preparation changes only.

## K03396
Source `D0400`, original line79, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

## 5. Actual host bug and current isolated source

## K03397
Source `D0400`, original line81, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

PeerEngine default configuration was17/07,25/02,19/00. OTA StationV4Runtime inserted16/N0/M2 but retained19, starting IMU BEFORE proof. Full185/186 radio-run.json raw_records contains hundreds of command2 checksum-valid sensor payloads. User's 'strong signal' was correct; diagnostic timeout was not radio silence.

## K03398
Source `D0400`, original line83, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Phase188/189 TrackerOtaRuntime._configure_engine overrides base setup and removes19 from configuration and mode_configurations, preserving16/00/02,17/07,25/02. Tests verify actual PeerEngine configuration twice. Removing19 allowed immediate stock proof; exact native starvation/timing mechanism is still inferred, not established. No arbitrary sequence bump or radio parameter sweep was used.

## K03399
Source `D0400`, original line85, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Current executed source is outputs/phase189/ota-proposal/release/radio/ram/tracker_ota_runtime.py, with LiveAdmission(min_quality=0.) for the explicit one-attempt user override. Do NOT blindly promote that permissive threshold into ordinary future packages. Generic LiveAdmission default remains0.95. Main pc source was not edited by these isolated OTA repairs.

## K03400
Source `D0400`, original line87, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

LiveAdmission order: FIRST schema2 fresh nonce/chip tail00070000; then new TX/slot baselines, clear counted slots and start measurement timer; require>=5s and>=256completedTX; then schema3; then different-nonce finalschema2; livecontrollersOFF sample immediately before OtaSession.begin. Schemas accept exact lengths:FE02len26,FE03len32 with enabled-byte boolean. Profile/channel/source/CRC/partial/S1/envelope and working sum8 are checked. Unique slot window excludes baseline, old/future/duplicates and rejects >=16384 window. Quality is unique working replied slots/completedTX, NOT RSSI, IMU or full-body proof.

## K03401
Source `D0400`, original line89, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Runner requires exact approved job and direct startargument, frozen image/manifest/registry/config/release/ELF/profile hashes and argv, no existing attempt/result, no active RF/OpenOCD/service. Exclusive attempt mkdir, random token, start.json and exclusive start.consumed.json bind process env/argv/all input hashes; actual load_job consumes ticket before hardware. Transport ACK cannot retire a diagnostic query; exact application reply advances cmd1 sequence. Reply ACK bit retirement remains live. Proof timeout8s; no automatic rediscovery/resume. Controller watchdog uses fresh checked raw sample (<250ms), lock and live process. MCU search rediscovery rejected. Loader stock protected by native SHA validation. Cleanup flags required even on failed admission.

## K03402
Source `D0400`, original line91, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

## 6. Exact Phase189 evidence and progress

## K03403
Source `D0400`, original line93, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Read docs/PHASE189_DIAGNOSTIC10_OTA_COMPLETE_2026-10-10.md and outputs/phase189/:
- ota-result.json:finish_verified,block4120,total_blocks4120,acked_blocks4120,retries2,rejected5010,errornull,response_polls4120,capture_acktrue,response_polltrue,boot_verifiedfalse,cadence_verifiedfalse.
- rejected5010 means unmatched protocol observations, NOT failed image blocks. native_ota_idle_verified=false; empty_prestart_reply_observed=true. Do not claim stronger idle proof.
- ota-result.admission.json:tx_baseline340,tx_latest614,unique_reply_slots202,slot_baseline339,slot_latest612,rawcontroller sample and rawquery records.
- Schemas2,3,2; bodies:
  fe02d1d7d0e50150687d20daaa75f72cec9e3eed197900070000
  fe0349316f8f721faf3acf11af7c0f4b31e93eed197907440100f0f000010403
  fe02224059ceae7a36124f98125f7efc1c3f3eed197900070000
- ota-result.tx.jsonl:8192 host mailbox publications,not RF-TX proof; first-to-last335.96822476387024s.
- ota-result.events.jsonl; remote-attempt/radio-run.json,radio-live-records.jsonl,controller-metrics.jsonl,cleanup.json,radio-run.recovery.json,start.consumed.json,stdout.log,openocd.log.
- All four cleanup true:session_closed_verified,ram_restored_verified,mpu_restored,trace_controls_restored_verified. Subsequent remote process/service check inactive.
- Progress reported to user:13/4120(0.3%),661(16.0%),1302(31.6%),1937(47.0%),2606(63.3%),3251(78.9%),3883(94.2%),4120(100%). Two retries accrued near end. No hidden transfer interruption.

## K03404
Source `D0400`, original line106, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

## 7. Diagnostic10 purpose, compatibility and costs

## K03405
Source `D0400`, original line108, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Observer-only extension; does not itself repair IMU. Mode protocol revision remains7; oldschemas1–4 retained. Stock mode0 and frame mode1 source compatibility checked offline; actual new image boot/recovery/heap/timing still unmeasured. Stock mode0 is an application fallback, NOT an independent rescue firmware. Bootloader rescue and flash readback unknown.

## K03406
Source `D0400`, original line110, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Seven fixed32-byte replies schemas5–11:FE+schema,nonce16,chip4,data10. Fixed RAM/MMIO only; no arbitrary address input, calibration/SPI access, CAPTURE task, event clearing, CHEN or timer task writes from queries. IRQ mask protects CPU reads, not PPI/peripheral clocks; pages are not atomic.

## K03407
Source `D0400`, original line112, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

- Page5:summaryu8 action counts, expectedcommand21 sequence bits6..11 at2000102e,E08guard,FA7,F98,mainqueueproducer/consumer. u8wrap means summary is not absence proof.
- Page6:TIMER1MODE/BITMODE/PRESCALER,SHORTSlow8,CC2,INTENcomparebits16..23,capturechannel. Page7:CC0/nominalperiod/E10/E48. Page8:fixedchannel3EEP/TEP,CHENlow8,COMPARE2eventlow8.
- Pages9–11:authoritative globalu32 event sequence and5 last action stamps:accepted21,completedSTART,completednativeSTOP-clear,otherclear,actualFA7SET; kind1=accepted21,2=START,3=STOP/SHUTDOWN+FA7clear,4=otherclear6588,5=FA7SET. Include1f99,FCC,1ff6 fields. Acceptance does not imply START.
- Hooks8d58 guardaccepted/tail78f4;7b8e actualTASKS_START THEN stamp/resume7b92;7be2 nativeSTOP/SHUTDOWN+FA7zero THEN stamp/BXLR;65c0 otherclear then replay/resume65c4;785a actualFA7one thenstamp/replay/resume785e. Preserve volatile registers,APSR,previousPRIMASK and native effects. Instrumentation latency not measured.
- Mode/profile reservation at20005150..2000516f grows8->32B:24B extra versus09. Original stock heap512B->480B (09heap504B). Both boundary literals1348/134c updated; boot/poweroff zero32B. Real heap peak/headroom unknown.
- Final builder outputs/phase176/build_observer.py; manifest/assembly beside candidate. Earlier unaligned andu8history variants superseded, never flashed.

## K03408
Source `D0400`, original line119, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

## 8. Audits and theory decisions — completed, not pending

## K03409
Source `D0400`, original line121, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Read docs/PHASE179_FINAL_AUDIT_VERIFICATION_2026-10-10.md, with177/178 repairs for provenance. Grok4.7medium via Cursor read-only; Gemini3.8FlashHigh plan/read-only. User called it Gemini3.8high; record actual returned backend designation. Initial Gemini600s run timed out; partial saved. Targeted continuation yielded complete final reply; narrow closure for both completed. Do not relabel partial as complete.

## K03410
Source `D0400`, original line123, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

outputs/phase179/grok-verbatim.md,gemini-partial-verbatim.md,gemini-final-verbatim.md,grok-closure-verbatim.md,gemini-closure-verbatim.md. Original173 Grokfinal also recovered;176/grok-u8-review-verbatim.md andgrok-final-review-verbatim.md preserve observer reviews. Grok closed5 concrete host findings; Gemini closed narrow observer findings. Lead accepted narrow source verdicts, NOT comprehensive hardware certification. Later automatic19 omission was still missing despite closure and was repaired188; audit does not prove all paths safe.

## K03411
Source `D0400`, original line125, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Confirmed corrections:
- 0x40009000=TIMER1,0x4000a000=TIMER2. 0x40001104=RADIO.ADDRESS;END=0x4000110c. Older timer/END wording is WRONG.
- TIMER+0x200=SHORTS, notCLEAR;actualTASKS_CLEAR+0x00c. Native77c6 conditionally enablesCOMPARE0_CLEAR.
- Nativecommand9 dispatch8d89 haschipmismatch8dd8->6588;command10dispatch8df1 hasE08/79b8, not samechipmismatch branch.
- CAPTURE overwritesCC2 withcounter. CounterexampleCC2=100000,counter100001,32bit16MHz,40msadvances640000ticks:cannotre-match100000withoutotherCLEAR/STOP/write beforewrap. Thus40msquiet does not universally restore original500000compare. NOT proof of live capture starvation.
- Host6000us is not proof ofnativeCC0 frameclock period. Unverified40mslinklossthreshold not adopted.
- ((after-before)&0xffffffff)>0 merelytestsinequality, not stronger direction proof.
- No NOP7bf6, sampler swap, operation4 in timeslot, gate bypass, artificialFA7 or syntheticIMU admitted.
- Native duplicatecmd1 sequence0 thennextsequence1 behavior andoldreplyACKretirement emulated; not established cause ofservice38 timeout. No blindsequenceincrement allowed.

## K03412
Source `D0400`, original line135, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

61 tests2.96s in179 cover runner/replay/rawproof/controllers/checksum/realPeerEngine deadlines/slotwindow/observer/nativequeue. Known WindowsUnicorn mem_map fatalaccessviolation diagnostic text sometimesprinted even whenassertions/exit0 passed; preservelogs, do not claim hardware.17675cases include70focused0.97s,4classifier0.06s,1nativeOTA48.25s/4120blocks/48syntheticretries. These counts are different scoped suites, not one additive success metric.

## K03413
Source `D0400`, original line137, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

## 9. Historical IMU blocker and accepted both-controller evidence

## K03414
Source `D0400`, original line139, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

FA7=20000fa7 must be open with20000441 and20001fb0 fornativea930 primaryproducer a952->d53c->ad08. FE03len32:byte22sensor_mode20000442,23cachedID20001fa0,24enabled20001fb4,25TXpermission20000fe1,26producer20001fb1,27ring20001afc,28UART20001d45,29postgate2000201c,30/31mainqueueproducer/consumer2000066c/2000067c. Main queue is notsensorRTOSqueue. FE04len25bytes22/23/24 are441/FA7/FB0. Querybodycommand1=7e+schema+nonce16. Fresh nonce/chip/CRC/workingchecksum required. Primaryu8 counter may wrap/reset; pages non-atomic.

## K03415
Source `D0400`, original line141, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

| Phase | Evidence | Limit |
|---|---|---|
|167|mode1/N1/6000/controllersOFF,RX500;634IMU/1199gaps;1025/1989CRC51.533%|Post19querytimeout; not physicalhang proof|
|168|same,no post19queries,60s;2956IMU/7450gaps;3097/7700CRC40.2208%,78ringdrops|ControllersOFF only|
|169|reservation900/RX800;3247IMU/7866gaps;3445/6362CRC54.1496%,121drops|WiderRX insufficient; no width sweep|
|170|bothcontrollers stockobservation30s;6528raw/6468accepted,0invalid/0freeze|Covered700us localquiet,notglobalradioquiet|
|171|bothcontrollers,compact500/RX400,6ms;covered700us;sensor80->7,FB0->1,FA7=0,producer251unchanged|No19;correctlyrefusedproducer|
|172|bothcontrollers,oneidentityguarded21;4987CRC/5016TX99.4218501%,IMU0,29.92463s|Controlonly;finalphase~3s;notsustainedsensor|

## K03416
Source `D0400`, original line150, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

172freshmode1/N1/pending0;freshsampling1664rows/124excluded;800usquietcenter5300us;compact500/RX400/lead100/6000. Retained1998records,dropped2983. Controller1unchangedpose2189.083487mswhilevalid;controller2changed99/99,0invalid. Stationaryvsfreezeunresolved; healthycoexistence NOT proven. After21FE04gates1/0/0;after17sensor7/enabled1/TXpermission0/gates1/0/1. Producer112/ring112unchanged in2freshFE03;19NOTsent. Last both-controller baseline remains this, even though185/186controllers-OFF rawIMU later appeared.

## K03417
Source `D0400`, original line152, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

## 10. Concrete next-session plan, no empty repeats

## K03418
Source `D0400`, original line154, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

1. Read AGENTS, this handoff,189report,176observerdoc,179audit,verified_observation.py/observer_protocol.py/classify_observation.py and exact current hashes. Do not reload all bulklogs or dispatch duplicate completed audits.
2. Respect user's TRACKER ALREADY ON report. Cheap live identity/environment/controller/process checks. No new OTA, no activation assumption.
3. Prepare a bounded observer activation/read-only application query session using stock initially, target/chip exact, without inherited automatic19. Use a NEW one-shot observation path; do not run consumed189OTA runner. Existing pure Observation collector does not itself implement RF; integrate with guarded proven host path, never invent CLI commands or manufacture raw proof.
4. Fresh schemas5/9 or other10-onlypages must answer exact nonce/chip to distinguish10 from09. FW189 andFE02rev7 alone are insufficient. Require whole activation evidence, retainrawrecords andcleanup. If powercycle failed to activate, diagnose once fromactualstate; do not automaticallyreflash.
5. Ordered observation PLAN=(9,5,6,8,10,11,6,9), freshnonce EACH request includingsecond6/9. Requirestableeventsequence/accepted21/kind andMODE/BITMODE/PRESCALER/SHORTS/INTEN/capturechannel bracket. All5nonzeroactionstampsmustpairwiseunique; canonicalFA7/SETprovenance. Changed/reset/wrap/mixed/stale/incomplete observations unresolved. cc2_changed_with_capture_configured_gate_closed is neutral, NOT runningclock/starvation proof.
6. SelectONE native intervention only afternewobserverdata distinguishesaccepted21/START/nativeSTOP/otherclear/SET/config. Prepared175native10 remainsnotadmitted untilmissingrunningtimer/PPI/E08/F98 facts established. No21/9/seqsweep,40msquietassertion,NOP7bf6 orforcedFA7.
7. Returngame mode1/N1/6000 withBOTHcontrollers, freshmode/config proof, realprimarycounteradvancement/allgates beforeONE19; noquerytrafficafter19. Kee[TRACKER_SERIAL]/freeze safety. Host174retention is separate factor; measure beforeadopting.
8. ReportactualuniqueIMU,countergaps/rate,retention/ringdrops andbothcontrollerhealth/motion. Counterspanacceptance isnotcompletephysicalsampleloss denominator. Achieve90–100%sustainedrealIMU beforeclaiminggoal; opticalcalibration/fusion/SteamVRmovement proofstillrequiredforfullbody/posevalid.

## K03419
Source `D0400`, original line163, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

## 11. Document/evidence index and preservation

## K03420
Source `D0400`, original line165, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Latest concise reports: docs/PHASE180_OTA_ADMISSION_DENIED_2026-10-10.md;PHASE181_REPEAT_ADMISSION_2026-10-10.md;PHASE185_STOCK_BEFORE_QUALITY_2026-10-10.md;PHASE187_SIGNAL_VS_APPLICATION_PROOF_2026-10-10.md (corrected by188/fullrawrecords);PHASE188_OTA_HOST_AUTOSTART_FIX_2026-10-10.md;PHASE189_DIAGNOSTIC10_OTA_COMPLETE_2026-10-10.md.

## K03421
Source `D0400`, original line167, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Original full-body contract:docs/FULL_BODY_EXECUTION_CONTRACT_2026-10-09.md. Native continuation:PHASE173_CONTINUATION_VALIDATION_2026-10-10.md,PHASE174_HOST_RETENTION_PREPARED_2026-10-10.md. Important correction:PHASE159_160_CADENCE_ADMISSION_CORRECTION_2026-10-10.md —historical160hadprecedingmode0proofbutnotfreshsame-sessionproof;do notrewritehistory.

## K03422
Source `D0400`, original line169, SHA256 `958d299dce769ea10d399ccee2beab8a0d2663b791f50aade0b4f3d630f2abc3`. Status: historical/unadjudicated narrative; English source prose.

Original state/start before THIS rewrite preserved at outputs/phase190-handoff/original-state-before-rewrite.md andoriginal-start-before-rewrite.txt. Phase-specific docs retain dated historical flags; use this rewritten current handoff and rootAGENTS first. START_NEW_CHAT_PHASE173.txt has also been replaced with current189/190 instructions.

## K03423
Source `D0416`, original line1, SHA256 `fba7b286ef936af75266c1b391ca7a611201daa4acbf16a3a088285270d07f74`. Status: historical/unadjudicated narrative; English source prose.

CURRENT274 OFFLINE / 273 CLOSED (2026-10-11): One native camera profile3 attempt273 consumed; actual upper-camera exposures782.266676us each (21.01x old37.233us), verified BEFORE RF. Profile also changes native emitter control, NOT exposure-only A/B. Fresh target mode1 verified. Fixed267972 verification FAILED; IMU19 not started, samples0, optical publications0. All4 MCU cleanup TRUE; owned camera closed, original XRServiceSettings restored, timeout5 restored; managed XRService PID38729 confirms restart after restoration. Wrapper SIGINT interrupted only final restart wait AFTER child guard failure/MCU cleanup, not the RF run. NO OTA/new firmware. Recorded native PNGs visibly contain tracker LEDs; PNG saves BEST brightness frame per camera, not first or necessarily simultaneous. Background KB-invalid observation can reject entire stereo batch; per-observation rejection under offline study, no guard weakening/deployment. Sparse1Hz replay58pairs original0/filtered0 is NOT full recording; dense15Hz replay pending. Keep HMD off during offline work. Need measured live optical identity/coverage, verified axes/time/fusion, multipeer/controller coexistence/body roles before fullbody. Read docs/PHASE269_274_NATIVE_PROFILE_AND_RECORDED_GEOMETRY_2026-10-11.md. OlderCURRENT entries historical.

## K03424
Source `D0418`, original line1, SHA256 `69a2de44654e9cfa310ea3dbe163d002201ff63c39f7c70588258d676dcce901`. Status: historical/unadjudicated narrative; English source prose.

CURRENT280 OFFLINE / 273 CLOSED (2026-10-11): Native profile3 attempt273 consumed and FAILED finite fixed267972 verification; fresh mode1 verified, no19/IMU0/opticalpub0, all4 MCU cleanupTRUE, original native settings restored, managed XRService PID38729 confirmed, owned camera closed/timeout5 restored. NO OTA/new firmware/RF active. Actual profile3 exposure782.266676us both upper cameras (21.01x old37.233us); changes emitter behavior too. Recorded273 dense15Hz:857 pairs; stereo bootstrap original0/KB-domain-filtered0. Full existing mono-seeded geometry+individual KB rejection280:754 eligible/340 accepted/303 compute<250ms; Frame compute p50/p95/max95.304/631.964/1591.750ms over ALL eligible, accepted p50/p95/max7.510/347.533/828.977ms. OFFLINE geometry only, no identity/real-world/live age/fusion proof. BatchP3P279 on16 actual ray batches preserves10752 ordered roots, max matrixdiff3.2302e-13, desktop kernelmedian25.140->2.825ms; isolated/UNDEPLOYED. Baseline275 COMPLETE: all754 decisions/methods and340 final transforms EXACTLY match280. Baseline all-compute p50/p95/max660.838/3757.459/7567.392ms vs batch95.304/631.964/1591.750ms; compute<250ms accepted261->303, processing only. No live improvement claimed. Evidence outputs/phase282/equivalence.json. Area>=10px diagnostic240/266 geometry but too slow and excludes distant small LEDs; NOTdeploy. Complete deduplicated stereo prototype277 still ambiguous12/12, rejected. Native276 profile3 coerces requested emitter mode0->1; pulse-width0 accepted by setter but actual physical off NOTproven. Keep HMD off during offline work. No unchangedRF/phase sweep/gateweakening. Next: finish baseline comparison, reject late/ambiguous results, close photon loss with existing native23/timed10/video evidence; only then purposefully sealed host repair/fresh readiness. Fullbody still needs axes/time/fusion, multipeer RAM/runtime, controller coexistence and body roles. Read docs/PHASE269_280_NATIVE_PROFILE_AND_RECORDED_GEOMETRY_2026-10-11.md. OlderCURRENT entries historical.

## K03425
Source `D0473`, original line1, SHA256 `0e8b03dd23b9ea1346b0384053b849291dd99b2cbe4ef9ed9848549ab33dc06e`. Status: historical/unadjudicated narrative; English source prose.

CURRENT385 CONCRETE PILOT WAITING FRESH READINESS:173sealedfiles/stagedFramephase385/candidate; invocationSHA f675ec8c9b36bb8072875c9c70cd324880d28c8c283c1a9c1f0ec618456d5939. Query+combined nowboundadmission/freshhumanfile(candidatehash/currentboot/<=300s), fileABSENT. ActualARM64 verifierbothentryrefuseFreshphysicalreadinessabsentbeforedevices/Popen. Standalonerunner/profilemainrefusalretained/allliveguardsunchanged. AsyncuserreadinessquestionSENT/replypending; NEVERcreateconfirmationfilewithoutfreshhumananswer. Nextactualready->exclusivefile->ONEsupervised150scombinedpilot/noOTA/no retry; actualIMUsignalbeforemotion. SSH/SCP/chmod/verificationonly; NOcamera/power/native restart/RF/SRAM/OTA/newcapture;diagnostic10/210unchanged;goalACTIVE/fullbodyUNACHIEVED. Read outputs/phase385/NOTES.md then384/382/365. Lowerentrieshistorical.

## K03426
Source `D0477`, original line1, SHA256 `1b3cce7c4887441051efc2b3d4f95b640f6707a1c474502322e0ca951e9a2301`. Status: historical/unadjudicated narrative; English source prose.

# Integration plan262

## K03427
Source `D0477`, original line3, SHA256 `1b3cce7c4887441051efc2b3d4f95b640f6707a1c474502322e0ca951e9a2301`. Status: historical/unadjudicated narrative; English source prose.

User prohibits repeated short RF probes. No new RF session launched in262.

## K03428
Source `D0477`, original line5, SHA256 `1b3cce7c4887441051efc2b3d4f95b640f6707a1c474502322e0ca951e9a2301`. Status: historical/unadjudicated narrative; English source prose.

1. Three-peer fairness: removed first-ready-peer ctz preference. With any peer
   ready, rotate all three profiles. Module262 compiled, hardware pending.
2. SteamVR driver: lazy registration on valid PTP1 pose, no fixed serial devices.
   Installed and registered /home/steamos/pico-ram-work/steamvr-driver using
   vrpathreg. Registration verified. Current SteamVR runtime has not reloaded it.
3. Camera transport: GrowingOCC reads complete records from a growing dataset,
   retaining partial EOF. Camera H264 demultiplexer added. Partial-write test
   passes. Continuous decoder, timestamps, exposure sync, tracker association
   and calibrated world-space pose producer remain to implement.
4. Final validation: one integrated three-tracker session, independent of PICO,
   all three real poses in SteamVR, retained Valve controller tracking, power-cycle
   reconnect. Do not claim this result until observed.

## K03429
Source `D0477`, original line18, SHA256 `1b3cce7c4887441051efc2b3d4f95b640f6707a1c474502322e0ca951e9a2301`. Status: historical/unadjudicated narrative; English source prose.

Exact blocker: IMU-only runtime never produces position; existing optical module
has no live Frame camera ingestion wired to driver IPC. Driver installation alone
cannot satisfy6DoF. No fake position or synthetic feed allowed.

## K03430
Source `D0478`, original line1, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

# Integration work, 2026-10-06

## K03431
Source `D0478`, original line3, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

## Superseding execution update

## K03432
Source `D0478`, original line5, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

Read EXECUTION_STATUS_2026-10-06.md (original reference: `EXECUTION_STATUS_2026-10-06.md`) for the
current done/remaining list and exact continuation point. It supersedes the
initial snapshot below. Current checks: 177 offline tests, ABI4 1928B, native
watchdog compile, one owned camera-only15s validation with1372liveIRframes and
RAW clock-domain evidence. New RF/SRAM/driver deployment and real6DoF remain
unverified. Latest Python integration code still needs audit and consistent
release upload; IMU body-axis/bias/time calibration and wiring are unfinished.

## K03433
Source `D0478`, original line13, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

## Archived initial snapshot

## K03434
Source `D0478`, original line15, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

The rest of this document describes the earlier157-test/ABI3 state. In
particular, its no-camera-recording statement is historical and does not apply
to the subsequent camera-only validation. No new RF session has been run.

## K03435
Source `D0478`, original line19, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

The standalone three-tracker 6DoF objective is **not complete**. No new RF probe,
SRAM invocation, camera recording, SteamVR restart or firmware write was performed.
This change establishes safer integration components, not a production station.

## K03436
Source `D0478`, original line23, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

## Current verification

## K03437
Source `D0478`, original line25, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

- Frame SSH reachable; API active, radio unit inactive, no OpenOCD process.
- The deployed unit still uses `roy_station_service250.elf`, 10-second duration,
  10 ms slot interval. Its ELF SHA256 is
  `43be1afcadcff67f3afdda3d2b242cb5aaaa23b8124d642af716372b9b2cbe1a`.
  `roy_station_service262.elf` was absent remotely. This installed unit was not changed.
- 157 Python tests pass. These are offline tests, not tracking evidence.
- Updated native driver compiles on Frame using the installed OpenVR headers.
- Native standalone PTP2 codec test accepts Python's 88-byte frame and rejects
  stale and malformed packets. It does not initialize OpenVR or publish test poses.
- Native UI syntax check passes; updated UI source was not installed or visually tested.
- Persistent ffmpeg decoder decoded four **recorded** 1056x1024 grayscale images
  from `live-camera264/38-10-0.h264`; this does not prove live latency or timestamps.
- Linux ownership exclusion and release/reacquisition verified without opening SWD.
- ABI3 ELF builds reproducibly with clang 19.1.7: 2039 code bytes, 2040 aligned,
  within the existing 2045-byte reservation. Code SHA256:
  `7797a4e020840824fb6851fd11f84fe9278476b1a65ad2dd20534a0f81aeab26`.
  ELF SHA256: `7c4caf80ce93d94d8da31cfde2f7cbe0fbd32da201e1118d45ec8e951c2882ef`.
  Candidate files are isolated at `/home/steamos/pico-ram-work/integration-20261006`.
  They were not injected into Roy or copied over the installed driver.

## K03438
Source `D0478`, original line45, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

## Source changes

## K03439
Source `D0478`, original line47, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

`roy_ram_loader.py`, `ownership.py`, `roy_ram_recover.py`, `build_station.py`:
shared Linux flock outside the deployment checkout; reject an existing OpenOCD
listener or owned shell hook; early SIGTERM handling; close-before-restore;
MPU control readback; reject invalid ring counters; deployment manifest;
aligned ELF boundary check; explicit seed and registry before touching hardware.
Recovery no longer relies on removable Python assertions for readback checks.
ABI3 now rejects missing identities, duplicate identities and zero/free slots.
It deliberately cannot claim generic enrollment. The systemd **source template**
uses an unnumbered ELF and explicit config, and disables forced SIGKILL.

## K03440
Source `D0478`, original line57, SHA256 `6f0e1d1dbf25c4cad88fcacc9e5b8b75c7b4f6bbca7dbe473306ea6c5cd0566f`. Status: historical/unadjudicated narrative; English source prose.

`roy_ram_station_three.c`: fair round robin also during cold discovery; omit
unused diagnostic tag/bitmap in the service build to fit the existing reserve.
The experimental repeated configuration packet is still present.
