# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K01201
Source `D0262`, original line23, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

Cursor status confirms bridge0.32.1, CLI available/authenticated. Recovered read-only SQLite assistant text from chat `c89068c2-6581-46b8-aa3a-0b44c0b0cd72`, matching the Phase172 command21 audit prompt. `outputs/phase173/grok173-recovered-text.json` preserves two verbatim intermediate text blocks with blob IDs and source path. **No final verdict was found**. Do not call the audit complete or dispatch a duplicate merely because its old functions cell cannot be resumed. Recovery script uses SQLite mode=ro and extracts only assistant text.

## K01202
Source `D0262`, original line25, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

## Discriminating next step

## K01203
Source `D0262`, original line27, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

Current revision09 lacks command21 application/expected-sequence and TIMER1/PPI state. FE03/FE04 alone cannot distinguish rejected command21, accepted-but-stopped TIMER1, and active capture overwriting CC2. A quiet-start intervention would alter radio traffic without proving which case existed.

## K01204
Source `D0262`, original line29, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

Before hardware intervention, finish audit recovery or establish its terminal status; then design observability for command21 accepted-handler count, its expected sequence/main-worker enqueue outcome, TIMER1 MODE/PRESCALER/CC2/COMPARE2/INTEN and relevant PPI channel enable/endpoints, plus 0x7bd4 stop count. Reads must not trigger capture, clear events or bypass producer gates. If this requires a firmware candidate, prepare/review offline and retain the user's explicit pre-OTA gate. No new OTA is authorized. Do not sweep sequence numbers, repeat unchanged21, or treat a longer pause as a fix.

## K01205
Source `D0262`, original line31, SHA256 `d8b8582ebf9ec73eb9e44a7392dfd423bdd3965ac7fb801d6e2db0162230a9dc`. Status: historical/unadjudicated narrative; English source prose.

Inherited host retention and contaminated historical mailbox logger remain unresolved. Native19 remains gated on fresh changing producer/all gates and fresh phase admission; no post19 diagnostic queries.

## K01206
Source `D0263`, original line1, SHA256 `029c796391eb930641ff39fa910326c98c1ed76a2d5dbf8fcfb580a56b069d80`. Status: historical/unadjudicated narrative; English source prose.

# Phase173: TIMER2/PPI blocker analysis (offline)

## K01207
Source `D0263`, original line3, SHA256 `029c796391eb930641ff39fa910326c98c1ed76a2d5dbf8fcfb580a56b069d80`. Status: historical/unadjudicated narrative; English source prose.

Current hardware172 mode1/N1 and joint99.42185%control CRC,IMU0,FA7=0,producer112stable. Native21 did not restore gate; do not repeat unchanged. NoRF active/noOTA.

## K01208
Source `D0263`, original line5, SHA256 `029c796391eb930641ff39fa910326c98c1ed76a2d5dbf8fcfb580a56b069d80`. Status: historical/unadjudicated narrative; English source prose.

New static/native-emulated dependency: native767c enables PPI RADIO.EVENTS_END(40001104) to TIMER2.TASKS_CAPTURE2 (original reference: `40009048`). Capture writes the same TIMER2 CC2(40009548) that7b50 initializes500000 and whose COMPARE2 handler7760/7842 setsFA7=1. This is not an independent diagnostic timer. Firmware21 path7c44->7bf0 calls767c and7b50, so subsequent radioEND events can overwriteCC2 before its planned compare. Nordic Product Specification explicitly says capture copies timer counter intoCC[n]; compare uses sameCC[n]. Primary reference https://docs.nordicsemi.com/r/bundle/ps_nrf52832/page/timer.html . Exact capture/compare event race is NOT modeled or proven causal on hardware. Frequent6ms polling before initial31.25ms compare (assuming16MHz native configuration) is a candidate mechanism; clock interpretation and event behavior must be verified, not assumed.

## K01209
Source `D0263`, original line7, SHA256 `029c796391eb930641ff39fa910326c98c1ed76a2d5dbf8fcfb580a56b069d80`. Status: historical/unadjudicated narrative; English source prose.

Executed unmodified native767c with mockedPPIchannel2; EEP/TEP addresses and enabled channel verified. Evidence outputs/phase173/ppi-path-proof.json. This verifies register programming only, not FA7 starvation. IndependentGrok4.7medium review requested, read-only,600sbudget. No external auditor authorized to executehardware. Next: verify fulldriverfrequency/mode, true TIMER2 path and whether a bounded radio-idle bootstrap is justified; no RF before a specific distinguishing plan.

## K01210
Source `D0264`, original line1, SHA256 `f544855c8bc07a620759fa31eda1f926aeac7c4f514c6ec07354f59e71827c7c`. Status: historical/unadjudicated narrative; English source prose.

# Phase174 host retention preparation

## K01211
Source `D0264`, original line3, SHA256 `f544855c8bc07a620759fa31eda1f926aeac7c4f514c6ec07354f59e71827c7c`. Status: historical/unadjudicated narrative; English source prose.

Objective remains sustained measured90–100% realIMU with both controllers on, per latest user instruction. No new RF, SSH, SWD or OTA in this preparation. Producer FA7 remains unresolved; this host change cannot repair it.

## K01212
Source `D0264`, original line5, SHA256 `f544855c8bc07a620759fa31eda1f926aeac7c4f514c6ec07354f59e71827c7c`. Status: historical/unadjudicated narrative; English source prose.

## Measured reason

## K01213
Source `D0264`, original line7, SHA256 `f544855c8bc07a620759fa31eda1f926aeac7c4f514c6ec07354f59e71827c7c`. Status: historical/unadjudicated narrative; English source prose.

Phase172 retained1998 replies and dropped2983. Its ten-record MCU ring holds about60ms at167Hz. Loader already copies published records and releases the captured write count before processing. Runtime then publishes a mailbox through SWD after every retained record. Multiple publications per copied ring consume time before the next drain.

## K01214
Source `D0264`, original line9, SHA256 `f544855c8bc07a620759fa31eda1f926aeac7c4f514c6ec07354f59e71827c7c`. Status: historical/unadjudicated narrative; English source prose.

## Prepared change

## K01215
Source `D0264`, original line11, SHA256 `f544855c8bc07a620759fa31eda1f926aeac7c4f514c6ec07354f59e71827c7c`. Status: historical/unadjudicated narrative; English source prose.

Isolated files: `outputs/phase174/host-retention/roy_ram_loader.py` and `station_v4_runtime.py`, based on exact Phase172 release hashes in `base-provenance.json`. No main source or installed deployment changed.

## K01216
Source `D0264`, original line13, SHA256 `f544855c8bc07a620759fa31eda1f926aeac7c4f514c6ec07354f59e71827c7c`. Status: historical/unadjudicated narrative; English source prose.

Explicit `compact_mailbox_batch=true` enables existing coalescing algorithm with compact game mode; it does not enable `dense_frame`. Loader admits the option only for station-v4 service, no optics, exactPhase119 code SHA4020c351db7e62fc68ef77663695f3f53c7e583bc51fd3e78cd3113bc215b299,6000us intervals and bench_joint_bootstrap. Each copied ring is decoded in order, then one final mailbox per peer is published. Exceptions discard staged publications. Raw records and every valid IMU are retained; old transmitted tags cannot ACK unpublished commands.

## K01217
Source `D0264`, original line15, SHA256 `f544855c8bc07a620759fa31eda1f926aeac7c4f514c6ec07354f59e71827c7c`. Status: historical/unadjudicated narrative; English source prose.

Mailbox publication log now follows runtime live-state output directory instead of historicalPhase162 path. Earlier contaminated aggregate is preserved.

## K01218
Source `D0264`, original line17, SHA256 `f544855c8bc07a620759fa31eda1f926aeac7c4f514c6ec07354f59e71827c7c`. Status: historical/unadjudicated narrative; English source prose.

## Validation and limits

## K01219
Source `D0264`, original line19, SHA256 `f544855c8bc07a620759fa31eda1f926aeac7c4f514c6ec07354f59e71827c7c`. Status: historical/unadjudicated narrative; English source prose.

Eight offline tests passed,0.26s; both changed files compile. Tests cover equivalence with per-record decoding, unique counters/wrap/duplicate/gaps, unchanged raw log, invalidCRC/partial/wrong-chip rejection, no ACK of unpublished successor, exceptions and optical/no-opt-in rejection. Evidence `outputs/phase174/host-retention-validation.txt`. No measured hardware retention improvement yet.

## K01220
Source `D0264`, original line21, SHA256 `f544855c8bc07a620759fa31eda1f926aeac7c4f514c6ec07354f59e71827c7c`. Status: historical/unadjudicated narrative; English source prose.

Do not launch this as an unchanged FA7 probe. Integrate only with the justified next producer intervention, preserving fresh admission/19 gate/no-post19-queries and all cleanup controls. Pending native21 audit is continuing in original Grok chat, no duplicate dispatch.

## K01221
Source `D0265`, original line1, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

# Phase175 native10 TIMER1 reanchor: prepared discriminating control

## K01222
Source `D0265`, original line3, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

**DISPOSITION: NOT ADMITTED, NOT RUN.** Narrow Grok follow-up identifies unobserved59cc/E08guard, PPI capture endpoint and running TIMER1 prerequisites. Native10 does not START a stopped timer. Fixedphase0 fixture is not live-state proof. Preserve175prepared files, but do not deploy/run without missing-state evidence. Proceed to offline observability candidate; noRF/OTA authorized by this review.

## K01223
Source `D0265`, original line5, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

Goal active:90–100% measured realIMU with both controllers. No new firmware/OTA. This document records the required justification before any run.

## K01224
Source `D0265`, original line7, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

## Verified dependency and failed history

## K01225
Source `D0265`, original line9, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

171/172FA7=0 and stable primary producer blocked19. Native21 in172 failed to restoreFA7; no sequence sweep or repeated21 is planned. Prior173 names are corrected:0x40001104 is RADIO.ADDRESS;0x40009000 is TIMER1.

## K01226
Source `D0265`, original line11, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

Native command10 table handler8df0 calls79b8 when59cc permits.79c8 tests fixedPPI channel3TEP0x4001f52c for TIMER1CAPTURE2.79ea sets the capture endpoint to0, preventing ADDRESS from overwritingCC2.79e0 adds467ticks to the requested signed phase.79a/7a2 paths updateCC2; negative error also enables PPI group0 at7a3c after760c installs COMPARE2->CLEAR and group-disable endpoints.7760(event148) setsFA7 at785a and reinstalls capture via767c.

## K01227
Source `D0265`, original line13, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

Five native tests `outputs/phase174/test_native_phase10.py` execute94fc->8df0->79b8 using explicit capturedCC2=100000/period533333 fixture. Phase0 writesCC2=99533 and TEP0. Phase131072 exercises negative error/group arm. Explicitly supplied COMPARE2 callback setsFA7 and restores capture. Native command10 sequence-filter flag is0;0/1/63 are accepted in offline assertions. These tests do not model real IRQ timing, timer progression or physical optical alignment.

## K01228
Source `D0265`, original line15, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

172 never published native10: opticsfalse, wrapperPLAN contains21/17/19 but no10, runtime/staged code has no standalone10 publication. Its phase_applied is Roy BLE slot alignment, not tracker TIMER1 reanchor. A post-BLE-phase read alone is therefore not the proposed repair.

## K01229
Source `D0265`, original line17, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

## ONE changed sensor factor

## K01230
Source `D0265`, original line19, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

Use a single logical native10/body00000000 as **sensor TIMER1 control**, with ordinary same-record retries only. It replaces failed explicit21 from172; no additional21 or9. Phase0 is a source-supported valid signed argument and a fixed diagnostic reference, NOT a claim of calibrated optical exposure timing. Keep pose_valid=false.

## K01231
Source `D0265`, original line21, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

Only publish10 after fresh FE02mode1/N1 and sensor17 state, fresh covered quiet window, compact500/RX400 and3s continuous admitted replies. Clear cached sensor/gate snapshots when10 is published. Require three fresh FE03 samples, freshFE04all gates and changing primary counter before one19. No queries after19. FA7=1 plus advancing producer is the distinguishing success; unchangedFA7 stops before19 and prohibits unchanged rerun.

## K01232
Source `D0265`, original line23, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

## Bound and cleanup

## K01233
Source `D0265`, original line25, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

Exact Phase1192043B MCU/codeSHA4020c351db7e62fc68ef77663695f3f53c7e583bc51fd3e78cd3113bc215b299, installed diagnostic09 unchanged.100s total bound;8s diagnostic application deadlines; watchdog, fault and rediscovery aborts retained. Controller health recorded per existing bench policy. Both controllers/headset freshly observed connected/valid before preparation; no toggle request. Close session and restore RAM/MPU/trace controls; incomplete cleanup prohibits furtherRF.

## K01234
Source `D0265`, original line27, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

Isolated `outputs/phase175`: same172release except future-only mailbox log path corrected to175. No host batching enabled in this first factor control;174 candidate is held for a separate retention comparison after producer recovery. Twenty-one focused offline cases passed0.44s. Outputs per-run diagnostic-proof/actions/radio-live-records/radio-run/controller-metrics/cleanup; wrapper clears pre10cached proof.

## K01235
Source `D0265`, original line29, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

## Grok audit disposition

## K01236
Source `D0265`, original line31, SHA256 `3cf9a97b4facd3d1c9036f0c24e885524889c90ed5469a5de59209306fc0014d`. Status: historical/unadjudicated narrative; English source prose.

Original173audit final saved verbatim in `outputs/phase174/grok173-final-verbatim.md`. Accept no9/no blindsequence1 and incomplete21observability. Reject its END label; ADDRESS is specified. Its268s-wrap explanation omits nativeCOMPARE0 CLEAR branches, so do not treat it as the only eventual compare mechanism. Diagnostic state classifications are suggestive, not definitive where bootstrap already initialized registers or ISR clears event. Newly discovered native10 path was absent from that review; narrow read-only follow-up is pending. No auditor authorizes OTA.

## K01237
Source `D0266`, original line1, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

# Phase176 ordered TIMER1 observer candidate

## K01238
Source `D0266`, original line3, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

## Current state

## K01239
Source `D0266`, original line5, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

Goal ACTIVE: sustained90–100% real unique IMU delivery in game mode1 with both controllers. Last hardware172 remains99.42185% CONTROL CRC,IMU0. Installed tracker stays diagnostic09/SHA07918813e82b8253be2af375c053754f9810c5a96be9710412b8546f00cf7266. No RF or OTA was run in173–176. Read-only SSH confirmed no active loader/OpenOCD/service and freshly observed HMD plus both controllers connected/valid; this is not sustained coexistence proof.

## K01240
Source `D0266`, original line7, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

175native10control is NOT ADMITTED/NOT RUN. Missing live running-timer/PPI/E08/F98 conditions make it potentially a no-op. Do not run prepared175 or repeat21/9/sequence sweeps without the missing state.

## K01241
Source `D0266`, original line9, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

## Exact current candidate

## K01242
Source `D0266`, original line11, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

`outputs/phase176/candidate10-timer-observer/fw189-dual-mode.bin`:98868bytes, SHA256`040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d`. Revision10 observer, mode protocol stays7. **UNINSTALLED, approved_for_ota=false.** Current source `outputs/phase176/build_observer.py`; exactassembly and reverse manifest beside image.

## K01243
Source `D0266`, original line13, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

Earlier versions `candidate10-initial-unaligned-history` and `candidate10-u8-counter-history` are superseded/offline, never installed. The initialu8design was rejected for event-order/overflow ambiguity before any OTA. Verbatim Grok audit is retained in176/grok-u8-review-verbatim.md. **Final exact-image review COMPLETE:**Grok found no source blocker for the before/after stamp classifier, confirmed hooks/reservation/reset/read-only fields, and requires9->10->11->9 stable event-sequence bracket. Verbatim176/grok-final-review-verbatim.md. Lead accepts this narrow verdict; not hardware safety/boot proof or OTA authorization.

## K01244
Source `D0266`, original line15, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

## What is observed

## K01245
Source `D0266`, original line17, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

Seven fixed32-byte query replies, schemas5–11, each with exact16-byte nonce and4-byte chip identity. Pages only read fixed RAM/MMIO; no CAPTURE task, event clearing, CHEN/task writes, SPI/calibration access or arbitrary address input. PRIMASK protects CPU snapshot reads; peripheral clocks/PPI are not frozen. Across-page observations are not atomic.

## K01246
Source `D0266`, original line19, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

Page5:summaryu8counts, live expectedcommand21sequence bits6..11 at2000102e, command10E08guard,FA7,F98,mainqueueproducer/consumer. Summary counters are never proof of absence due8-bit wrap.

## K01247
Source `D0266`, original line21, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

Page6:TIMER1MODE/BITMODE/PRESCALER,SHORTSlow8,CC2,INTENcompare bits16..23,capturechannelbyte. Page7:CC0,nominalperiod,E10connected/pendingpairflagE48. Page8:fixedchannel3EEP/TEP,CHENlow8,COMPARE2eventlow8.0x40001104 is ADDRESS, not END;0x40009000 is TIMER1.

## K01248
Source `D0266`, original line23, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

Pages9–11:authoritative32-bit event sequence and last accepted21/completedSTART/nativeSTOP-clear/actualFA7SET/otherclear event stamps. Also last-eventkind,1f99calibrationstate,FCC and1ff6flags. Kind1=21guardaccepted,2=STARTstorecompleted,3=STOP/SHUTDOWN+FA7clearcompleted,4=6588otherclearcompleted,5=FA7SETcompleted. An accepted21 stamp alone does not imply downstreamSTART.

## K01249
Source `D0266`, original line25, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

## Hooks and memory cost

## K01250
Source `D0266`, original line27, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

8d58BL:stamp after actual21 identity/selector guard,tail78f4; no guard bypass.7b8e:perform nativeTASKS_STARTstore thenstamp,replayLDRr3=e000e100,resume7b92.7be2:perform nativeFA7zero after STOP/SHUTDOWN thenstamp,BXLR.65c0:perform otherFA7zero thenstamp,replayLDRr3=2000045c,resume65c4.785a:perform actualFA7one thenstamp,replayLDRBr3[r1+1],resume785e.

## K01251
Source `D0266`, original line29, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

Action+stamp updates execute in a short IRQ-masked window; restore previousPRIMASK,APSR and volatile registers exactly. PPI still runs. Added instrumentation latency is not hardware-measured. It must not be described as zero-overhead or proof of unmodified live timing.

## K01252
Source `D0266`, original line31, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

Observer/profile reservation grows8->32bytes at20005150..2000516f, consuming24additionalbytes from heap:512->480bytes. Both native boundary literals1348/134c changed; heap end unchanged. Boot/poweroff explicitly clear all32bytes. Native tests cover reset and heap-boundary reversal; real peakheap/headroom and bootloader/recovery remain unknown. No casually borrowed RAM or buffer overlap.

## K01253
Source `D0266`, original line33, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

## Validation

## K01254
Source `D0266`, original line35, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

70focused tests passed0.97s:fixednewpages and old1–4compatibility; nativequeue/allocationfailure; nativehardware/RAM/register/APSR/PRIMASK equivalence at masks0/1; identity guard; completed-action ordering1/2/3/4;u8summarywrap;manualbootreset;remote mode roundtrip/OTA-busy/falseproof checks. Four conservative classifier tests passed0.06s. Exact-image frozenservice38 native staging test passed48.25s:4120/4120blocks,finish_verified,exactstaginghash,48syntheticretries; MMIO/time/SPI/IRQ/allocator modeled. **75offline cases total; no physical OTA/boot proof.** Windows/Unicorn occasional mem_map diagnostic text and aJUnitproperty warning are retained; reported test assertions passed. Early fixture errors (append exceeded old emulated map, non-MCLASS PRIMASK fixture, oldcompatibilitysymbolmanifest) were corrected; production emulator sources unchanged.

## K01255
Source `D0266`, original line37, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

Classifier `outputs/phase176/classify_observation.py` requires verified identity/nonce/CRC and stable event-sequence bracket. Mixedpages/reset/wrap/stale/incompletepatterns return unresolved. Actualclear ordered afterSTART/SET selects stop vs otherclear; enabledADDRESScapture with measuredCC2progression andSTARTnewerthanclear identifies activecapture/gateclosed, NOT proof of capture starvation. Gateopen still requires fresh advancingproducer before19.

## K01256
Source `D0266`, original line39, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

## Concrete execution boundary

## K01257
Source `D0266`, original line41, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

`outputs/phase176/ota-proposal` is a preparation-only isolated service38 package with exactimage/job/manifest/registry/config/frozenrelease hashes. Main whitelist unchanged. Job preparation_only=true,approved_for_ota=false. Runner additionally refuses missing directhumanstartargument, unapprovedjob, replay, activeRF/OpenOCD, staleprestartproof, wrongserial/chip, missingfreshnonce/activation, missingstockmode0/N0/pending0 proof, controllersnotOFF or absent freshsame-profile service38admission. No prestart-proof is fabricated or supplied. Package is local/unexecuted.

## K01258
Source `D0266`, original line43, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

User's prior hard boundary remains: **pause/call the user before actual OTA**. This is required by `docs/FULL_BODY_EXECUTION_CONTRACT_2026-10-09.md` and current handoff; today's insistence on90–100% does not silently authorize a newimage. Final exact-image review must complete before asking. Stock/service use only for this OTA, then return to mode1game; no mode0sensor detour.

## K01259
Source `D0266`, original line45, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

After approved successful transfer/activation: freshnoncepage5/9 proves new observer capability; before/after stable-bracket pages establish actual21 acceptance, completedSTART/clear ordering, captureendpoints/CC2progression. Choose ONE supportednative intervention. Require changingprimarycounter/allgates, freshjointphase thenone19; noqueriesafter19.174hostbatching stays a separate measuredretention factor. Measure actualcounter/gaps/rate,retention andbothcontroller motion/health until90–100%;pose_valid=false until optical/full-body proof.

## K01260
Source `D0266`, original line47, SHA256 `a93f7072a9090bca94aa8cc56f4a9f841e3d367f93b42b3383701e60a6ae14cd`. Status: historical/unadjudicated narrative; English source prose.

Preparation reproducibility correction: the proposal generator now copies the guarded runner template verbatim, preserving fresh identity/mode/N0/controller-off/profile/age gates. Template and prepared runner are byte-identical and parse successfully. No job approval changed, no upload/execution/hardware action occurred.

## K01261
Source `D0267`, original line1, SHA256 `eb62b8aef77ca3a1032f0d53903ee0cb4716d38cf2ffd03f2e40e26b43d3ec79`. Status: historical/unadjudicated narrative; English source prose.

Superseded host repair status: read PHASE178_HOST_AUDIT_REPAIRS_2026-10-10.md. Original audit below preserved.

## K01262
Source `D0267`, original line3, SHA256 `eb62b8aef77ca3a1032f0d53903ee0cb4716d38cf2ffd03f2e40e26b43d3ec79`. Status: historical/unadjudicated narrative; English source prose.

# Phase177 independent audit and tomorrow handoff - 2026-10-10

## K01263
Source `D0267`, original line5, SHA256 `eb62b8aef77ca3a1032f0d53903ee0cb4716d38cf2ffd03f2e40e26b43d3ec79`. Status: historical/unadjudicated narrative; English source prose.

Both reviews completed: Gemini 3.8 Flash High (plan/read-only) and Grok4.7medium (Cursor read-only). Verbatim responses: outputs/phase177/gemini-verbatim.md and grok-verbatim.md. No hardware/RF/SSH/SWD/OTA this round. User requests PC shutdown after saving, resume tomorrow with OTA; this is not the concrete pre-start approval.

## K01264
Source `D0267`, original line7, SHA256 `eb62b8aef77ca3a1032f0d53903ee0cb4716d38cf2ffd03f2e40e26b43d3ec79`. Status: historical/unadjudicated narrative; English source prose.

Image unchanged:98868 bytes SHA256040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d; uninstalled/unapproved. Installed09 unchanged. Last hardware172 CONTROL4987/5016=99.4218501%,IMU0. Goal90-100% real unique IMU mode1/N1/6000 BOTH controllers remains unmet; pose_valid=false.

## K01265
Source `D0267`, original line9, SHA256 `eb62b8aef77ca3a1032f0d53903ee0cb4716d38cf2ffd03f2e40e26b43d3ec79`. Status: historical/unadjudicated narrative; English source prose.

Accepted findings: proposal runtime load_job overwrites disk preparation_only; approved_for_ota is not enforced internally. Runner lacks exact image/argv binding and true approval check. Prestart proof booleans do not validate raw nonce/chip/mode/N/pending records or same-session service38 admission. Attempt directory missing. Classifier accepts caller-trusted identity/CRC/bracket and has equal-clear and contradictory-FA7 cases. MMIO pages and both CC2 samples must join the observation bracket; no integrated live collector exists yet.

## K01266
Source `D0267`, original line11, SHA256 `eb62b8aef77ca3a1032f0d53903ee0cb4716d38cf2ffd03f2e40e26b43d3ec79`. Status: historical/unadjudicated narrative; English source prose.

Containment completed: both runner/template raise unconditionally before any launch; isolated require_ota_clearance overridden to fail closed; stored authorize flag removed. Do not run old proposal or regenerate it as an admitted package. Generator copies blocked template. No permission/job flags changed to approved. Firmware is not rejected by this host-launch finding; source hook equivalence remains the narrow reviewed result.

## K01267
Source `D0267`, original line13, SHA256 `eb62b8aef77ca3a1032f0d53903ee0cb4716d38cf2ffd03f2e40e26b43d3ec79`. Status: historical/unadjudicated narrative; English source prose.

Classifier corrections: use bracketed page10 FA7 rather than stale page5. Reject equal nonzero clear stamps and SET/FA7 contradictions. Stale-summary regression plus existing tests passed5/5 before final contradiction guards; focused followup verification recorded separately.

## K01268
Source `D0267`, original line15, SHA256 `eb62b8aef77ca3a1032f0d53903ee0cb4716d38cf2ffd03f2e40e26b43d3ec79`. Status: historical/unadjudicated narrative; English source prose.

Gemini conclusions rejected as overclaims: native capture/compare pattern does not prove live capture starvation; no native NOP or40ms quiet experiment admitted. Missing accepted21 does not isolate RF drop vs dispatcher/queue solely from current fields. Heap480 bytes is correct; additional cost relative to09 is24 bytes, not32. Existing hooks are benign with movr1,r2; hypothetical future-hook risk is not current image ABI failure.

## K01269
Source `D0267`, original line17, SHA256 `eb62b8aef77ca3a1032f0d53903ee0cb4716d38cf2ffd03f2e40e26b43d3ec79`. Status: historical/unadjudicated narrative; English source prose.

Tomorrow order: readAGENTS and this report; repair exact isolated internal approval preservation and image/argv pinning; implement raw evidence validation and actual same-session prestart admission; exclusive attempt directory/start marker; implement verified observation collector9->5->6first->8->10->11->6second->9 with raw replies/nonce/chip/CRC evidence and conservative ordering limitations; test adversarial input. Obtain final review of changed host package. Only then ask for exact-image human pre-start approval and perform fresh target/environment gates. No blind sweeps; no unchanged RF retry; after activation choose ONE intervention from measured actions/gates/producer then measure IMU counters,gaps,rate,retention and both controller health.
Final focused classifier verification after containment:5 passed in0.05s. External review verbatim files saved successfully. Windows shutdown requested by user after this handoff.

## K01270
Source `D0268`, original line1, SHA256 `6d94159ec2b5f7572eef2e897a7e1ba3404ad63148d1e3d04454b81d06bbd0e0`. Status: historical/unadjudicated narrative; English source prose.

Current status superseded by PHASE179_FINAL_AUDIT_VERIFICATION_2026-10-10.md: final reviews completed, additional repairs and61offlinecases; no OTA authorization. Earlier178 evidence below preserved.

## K01271
Source `D0268`, original line3, SHA256 `6d94159ec2b5f7572eef2e897a7e1ba3404ad63148d1e3d04454b81d06bbd0e0`. Status: historical/unadjudicated narrative; English source prose.

# Phase178 host audit repairs - 2026-10-10

## K01272
Source `D0268`, original line5, SHA256 `6d94159ec2b5f7572eef2e897a7e1ba3404ad63148d1e3d04454b81d06bbd0e0`. Status: historical/unadjudicated narrative; English source prose.

User requested correction of confirmed Phase177 defects. This is local host preparation only: no SSH/RF/SWD/OTA or new firmware. Image98868bytes/SHA040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d remains uninstalled; job preparation_only=true,approved_for_ota=false. Last hardware172 CONTROL99.42185%,IMU0; sustained90-100% realIMU with BOTH controllers and optical/fullbody still unmet.

## K01273
Source `D0268`, original line7, SHA256 `6d94159ec2b5f7572eef2e897a7e1ba3404ad63148d1e3d04454b81d06bbd0e0`. Status: historical/unadjudicated narrative; English source prose.

## Repairs

## K01274
Source `D0268`, original line9, SHA256 `6d94159ec2b5f7572eef2e897a7e1ba3404ad63148d1e3d04454b81d06bbd0e0`. Status: historical/unadjudicated narrative; English source prose.

Isolated load_job now rejects unapproved/preparation-only disk jobs in hardware mode instead of overwriting the prohibition; offline preparation remains explicit and cannot pass actual require_hardware_ota_clearance. The actual hardware function now admits only exact diagnostic10/image/identity/slot0/service38/sourcecadence and requires both explicit approval fields plus CLI authorization. Historical image exceptions removed from this isolated copy; main pc source unchanged. Phase177 containment mistakenly redefined require_ota_clearance instead of the called require_hardware_ota_clearance; this is corrected, not claimed as having protected the internal path yesterday.

## K01275
Source `D0268`, original line11, SHA256 `6d94159ec2b5f7572eef2e897a7e1ba3404ad63148d1e3d04454b81d06bbd0e0`. Status: historical/unadjudicated narrative; English source prose.

Runner uses explicit exceptions (not assertions), exact frozen argv, image/manifest/config/registry digests and every release-file hash. Job paths are fixed, authorization flag absent from stored argv and added only after runner approval validation. Attempt directory is created exclusively before launch and blocks all replay, including failure before result creation. No supplied prestart-proof boolean file grants admission.

## K01276
Source `D0268`, original line13, SHA256 `6d94159ec2b5f7572eef2e897a7e1ba3404ad63148d1e3d04454b81d06bbd0e0`. Status: historical/unadjudicated narrative; English source prose.

LiveAdmission inside TrackerOtaRuntime waits for fresh exclusive service38 observation:>=32 completedTX and>=90% unique retained CRC-valid target reply slots from this runtime's baseline (a conservative admission guard, not measured IMU proof). Then queries schema3 diagnostic capability and finally schema2 mode/rev/N/pending. Raw reply length/nonce/chip/hardwareCRC/source/slot and timing validated. Requires mode0/rev7/N0/pending0 from the actual last application reply. MCU ACK retirement is disabled while proof query is active; only matching application response completes it. Native reply acknowledgement is preserved. No target rediscovery allowed. Service38's completedTX counter is DATA+1912, verified from pc/radio/ram/roy_ram_station_service38.c159; isolated runtime now enables its ABI counter read explicitly.

## K01277
Source `D0268`, original line15, SHA256 `6d94159ec2b5f7572eef2e897a7e1ba3404ad63148d1e3d04454b81d06bbd0e0`. Status: historical/unadjudicated narrative; English source prose.

Immediately before OtaSession.begin, actual ControllerGuard requires a fresh controllers-off OpenVR sample, and raw sample plus reply records and admission counters are saved. Existing same-loader watchdog remains active. Capability schema3 is not a live flash SHA read: no firmware hash interrogation exists here, so provenance of installed09 plus live capability/identity are kept distinct.

## K01278
Source `D0268`, original line17, SHA256 `6d94159ec2b5f7572eef2e897a7e1ba3404ad63148d1e3d04454b81d06bbd0e0`. Status: historical/unadjudicated narrative; English source prose.

Observer collector implements9->5->6first->8->10->11->6second->9 with fresh distinct nonce per request, target chip/hardwareCRC/partial rejection and raw retained replies. It passes actual twoCC2 samples and both event-sequence snapshots to conservative classifier. Stale nonce cannot satisfy repeated schema6/9. Missing acceptance stays unresolved; schema5 sequence/queue report is retained without falsely identifying radio-vs-worker loss. Peripherals are not frozen; capture starvation remains a hypothesis, not a cause claim. No automatic native intervention or command19 emitted by collector.

## K01279
Source `D0268`, original line19, SHA256 `6d94159ec2b5f7572eef2e897a7e1ba3404ad63148d1e3d04454b81d06bbd0e0`. Status: historical/unadjudicated narrative; English source prose.

Classifier uses bracketed page10FA7, rejects equal nonzero clear stamps and newerSET/clear contradictingFA7. Generator now exports the repaired frozen unapproved package, with full hash/argv/template verification, instead of rebuilding the weaker Phase160 runtime; export requires new destination and cannot copy a used/approved attempt.

## K01280
Source `D0268`, original line21, SHA256 `6d94159ec2b5f7572eef2e897a7e1ba3404ad63148d1e3d04454b81d06bbd0e0`. Status: historical/unadjudicated narrative; English source prose.

## Validation and scope
