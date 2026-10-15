# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K02321
Source `D0354`, original line88, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Important new negative result: pooling the same Phase89 evidence into only THREE segments produced a900us claimed common window. SIX segments yielded600us. short_phase90.common_plan was corrected to six segments; do not weaken it back to three to pass admission.

## K02322
Source `D0354`, original line90, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## Current live code changes, offline only

## K02323
Source `D0354`, original line92, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

pc/radio/ram/peer_engine.py now has opt-in short-exchange policy:

## K02324
Source `D0354`, original line94, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

- mode_actual_n and mode_native_pending retained from accepted schema2 replies and cleared with mode proof.
- enter_short_exchange(duration<=8,expected_n) requires settledFrame/schema2 proof,actualN matching,native_pending0,WAIT_SYNC/STREAMING,no pending/queued commands or switch,normal fresh3s liveness.
- enter_short_resume_probe allows an explicitly documented closed-session handoff with stale liveness; it does NOT rewrite last_reply. A valid working reply must arrive within1s or the policy raises/stops and clears mode proof.
- _begin rejects all commands while short policy armed; mailbox and advance enforce the finite deadline,proof lease,liveness and clean protocol state.
- In this explicit finite short policy only,10s refresh is deferred inside the existing30s lease. Lease and confirmation timestamp are NOT extended. This supersedes the initial idea of stopping before the10s refresh. Ordinary callers remain unchanged.
- On stop, short_exchange_until remains armed, preventing accidental rebootstrap on the same engine. Driver must stop/restore, not silently clear it to resume RF.
- The resume API assumes an owning driver has verified actual closed MPSL ownership and preserved state. That driver is NOT implemented yet.

## K02325
Source `D0354`, original line102, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

pc/radio/ram/short_phase90.py is a new helper, NOT invoked by existing loaders:

## K02326
Source `D0354`, original line104, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

- require_closed verifies stock MPSL inactive,station session255,native hook0 and drained write_count==read_count.
- collect_closed requires exact90d CODE SHA,does operation3 bounded native observation,checks hook release/completion and preserves96B peer fields and192B mailboxes. Caller owns checkpoint/restoration. It never opens/closes RF or installs CODE.
- Scratch is DATA+256..1279, so drained historical RX records must already be saved by the owner. This avoids replacing the live peer/mailbox state with standalone sampler DATA.
- common_plan requires>=1000zero-bracket rows,six segments,50us bins,>=3 observations/bin,<=5%busy; requiresbudget>=330us. Target includes half the RTC allowance plus100us lead. It labels statistical admission, not guaranteed safety.
- Relative import and standalone-loader fallback are included. No hardware deployment of these new host files occurred.

## K02327
Source `D0354`, original line110, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Existing earlier changes remain: strict denseFrame diagnostic hash/config gates,query schema2 support,early product-info cache race fix,correct ring read-before-ack ownership,optional mailbox batching disabled,common-grid/OTA gates. Do not remove these or broadly accept new binaries in the standard loader.

## K02328
Source `D0354`, original line112, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## Sealed offline RAM candidates

## K02329
Source `D0354`, original line114, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

All remote builds usedclang19.1.7,aarch64 host,ARM Cortex-M4,-Oz,original linker d0ab128f2462988b32ece34c54043d51910bee307520b1239422d44394f5c8e6. Build scripts are outputs/phase90/build90*.py; exact source,ELF,deployment.json are outputs/phase90,phase90b,phase90c,phase90d. Earlier versions preserved.

## K02330
Source `D0354`, original line116, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

| Candidate | Bytes | Change | CODE SHA256 |
|---|---:|---|---|
|90|2008|Phase66 base,removed timed-optical metadata support,fine10us polls,closed-session16-sample observer,unhook last|b3d2601671af3638ef6379bafb1ef1689eb4ce1c7cd87a336dba59c7540a41c2|
|90b|1745|Short-only alreadyconfigured target/stage3,empty polls only,no native rediscovery;finite handoff deadline and completed-TX counter|43f7aa0d46a32544605c631bd5401234e06d2647ec764346af749481f0f5e7c5|
|90c|1753|64-sample closed/drained observer;retain END+CRC at deadline without rearming|ed76988c5432da909a61e686eb23f1ff58ac8e99fbe4c4688d040ca70876e8aa|
|90d latest|1773|Custom RX ownership flag suppresses stale END capture in off-phase/no-TX slots|a498034510958d7544aefa35887ccb526991c33fc6b89a0e32adc2441807932b|

## K02331
Source `D0354`, original line123, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

90d ELF SHA:b77c10c764aeea38c3c1b6e0a7109e226c62b65923577e28619a2bc5a9ad9193. Source SHA:677ce710eb20b74dc54e166ab5131605ff2b489cbb7108963cb88a7ad2d94b71. Entry0x2001151d. Existing Phase66 phase gate+-60us retained. All versions fit2045B. They are build/test candidates, NOT installed/approved RF profiles.

## K02332
Source `D0354`, original line125, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

New90d fields:DATA+1896 explicit DWT handoff deadline (nonzero required);+1900observer sample count;+1904custom RX-active flag reset on every slot and enabled only when custom TX setup runs;+1908completed TX_END counter. Old three-peer ABI preserved. All peers except0 must remain paused in the future sealed profile. Wrong ID/stage or command mailbox stops the candidate. Native3s stale age may be bypassed only before explicit handoff deadline; host must limit this to1s. Hardware date bounds/normal controller guard are still mandatory.

## K02333
Source `D0354`, original line127, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

TX SHORTS11 already is READY_START|END_DISABLE|DISABLED_RXEN. It is the proven fast TX->RX path; don't try the rejected guess that11 retransmits TX. RX SHORTS19 is READY_START|END_DISABLE|ADDRESS_RSSISTART. Verified against original turnaround207–209 reports and Nordic bitfields:https://raw.githubusercontent.com/nrfconnect/sdk-hal_nordic/main/nrfx/mdk/nrf52833_bitfields.h.

## K02334
Source `D0354`, original line129, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## Tests and limitations

## K02335
Source `D0354`, original line131, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Latest run:107 passed in0.64s; outputs/phase90/offline-tests.xml.

## K02336
Source `D0354`, original line133, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Command from K:/AI/pico:

## K02337
Source `D0354`, original line135, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

.venv/Scripts/python.exe -m pytest -q -p no:faulthandler pc/tests/test_short_phase90.py pc/tests/test_coexist_observer90.py pc/tests/test_short_callback90c.py pc/tests/test_short_exchange90.py pc/tests/test_peer_engine.py pc/tests/test_roy_ram_loader.py --junitxml=outputs/phase90/offline-tests.xml

## K02338
Source `D0354`, original line137, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

New tests execute exact ELFs for bounded closed observer,IRQ mask preservation,late unhook,peer/mailbox preservation,no RADIO/TIMER writes in observer,short target/state/command/probe rejection and TX counter. Host tests cover lease expiry,no fake liveness,no command publication,probe success/failure. Phase helper reproduces actual Phase89 six-segment600us evidence and rejects insufficient,busy,cross-tick data and ownership conflicts.

## K02339
Source `D0354`, original line139, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Still missing:explicit off-phase stale-END regression test for90d;330us deadline END/CRC/rearm cases on90d;drained-ring observer rejection execution on exact90d;CODE-switch/close/reopen lifecycle/restore tests;sealed integrated deployment;hardware validation. The existing short callback fixture tests0/3stage and identity rejection but not all concurrent ownership races. Unit tests are not coexistence proof.

## K02340
Source `D0354`, original line141, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## External Grok review

## K02341
Source `D0354`, original line143, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Completed read-only Grok4.7-medium through Cursor, raw outputs/phase89/grok-review.md, disposition docs/PHASE89_GROK_REVIEW_DISPOSITION_2026-10-09.md. Accepted long-reply/state/lease concerns. Rejected its430us-slot proposal that contradicted the500us minimum and its claim that late END necessarily completed before the deadline.600us reservation/RX330/margin270 is numerically representable, but needs its own sealed candidate policy, not loosening the old Phase66 SHA gate. Grok is an opinion, not hardware proof. No pending helper/tool/process remains at this handoff.

## K02342
Source `D0354`, original line145, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Earlier83 review timed out, no verdict; don't confuse it with completed89. Use cursor_ask/cursor_continue modelgrok-4.7-medium,never-fast,workspaceK:/AI/pico,read-only for review,timeout sufficient (900s allowed). Prompts start:Read AGENTS.md in the project root first and follow it. User already authorized Grok assistance. Standalonegrok_ask absent. Don't touch corporate~/.codex profile or memory files.

## K02343
Source `D0354`, original line147, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## Proposed integration, NOT implemented

## K02344
Source `D0354`, original line149, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

1. Controllers-OFF known common8800us long bootstrap using service79 module (StockN1/FrameN2),remote mode1 proof and fully ACKed configuration. Require fresh schema2 actualN2/native_pending0,no hidden staged switch initiated by host,no pending/queued commands.
2. Preserve all peer,mailbox,sequence,ACK and mode timestamps in the SAME owning runtime; save bootstrap raw evidence. Close MPSL and verify session255,stock MPSL inactive,hook0,drained ring. Do NOT do normal full RAM cleanup/reinitialize peer state between stages.
3. Only while ownership is closed,replace CODE with sealed90d,readback exact hash. Keep checkpoint of original complete RAM for final restore. CODE swap must not race native return or outstanding callback. Resume starts at the same native entry address; review trace-control save/restore across close/reopen.
4. With custom RF stopped,let user enable both controllers; monitor fresh native tracking. Acquire fresh phase using integrated observer without overwriting peer/mailbox data. Budget the user wait and sampling against the original30s mode lease. Expiry means no short RF and cleanup, not proof extension.
5. Validate fresh600us+ common gap and330us RX budget; configure NORMAL priority,interval9000us,cycle1,N2,slot600us,end margin270us,RX330,100us lead/post,peers1/2paused,no optics/no OTA. Set actual live phase reference,short probe deadline1s and TX counter0. Existing normal loader gates must remain untouched; a dedicated sealed coordinator is required.
6. Resume only empty polls; do NOT invent last_reply or fresh mode. Require a real working reply within1s. Run<=8s within existing mode lease,stop on guard/liveness/proof expiry/unexpected state. Keep bootstrap and short counters separate. Poll percentages use90d completedTX_END,not granted slots.
7. Stop before the finite host deadline through the owning loop,close and verify session,restore original full RAM/MPU/trace controls,and postcheck controllers. Do not report expected finite stop as an unexplained RF failure or ignore real cleanup errors.

## K02345
Source `D0354`, original line157, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

This procedure is a proposal. **No coordinator,loader hook,CLI admission,guard transition or integrated hardware runner was written.** No call to enter_short_* or collect_closed is wired into production. Do not run90d with the generic loader: it cannot bootstrap and has no appropriate admission/profile. Do not import serialized peer state into a fresh runtime and call it continuous without validating actual target continuity.

## K02346
Source `D0354`, original line159, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

A successful8s result would only justify the next stage. Sustainable98–100% still needs long reliable queue/mode proof refresh handling,possible compact tracker status protocol or better native scheduling,ISR jitter/airtime bounds,host-ring loss reduction,and optical independence/controller checks. No new firmware OTA is authorized just to solve this automatically.

## K02347
Source `D0354`, original line161, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## Exact next work

## K02348
Source `D0354`, original line163, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

First review the current new source/diffs and this manifest; add the missing exact90d tests above. Then implement and review the sealed owner/coordinator with a fully recoverable CODE switch and finite proof budget. Ask Grok to audit that concrete code. Only when one integrated test is ready,ask user to turn controllers OFF for bootstrap,then ON during the closed-radio stage. Do not request hardware toggles now merely for exploratory reads. If controller readiness takes longer than remaining proof budget,stop transparently and redesign/prepare a new bootstrap; don't extend the lease or remove guards.

## K02349
Source `D0354`, original line165, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

Check live docs/research after this handoff before executing anything. Repo has many unrelated preexisting modified/untracked files; no git reset/checkout/restore/commit performed. Preserve those changes.

## K02350
Source `D0354`, original line167, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

## Evidence navigation

## K02351
Source `D0354`, original line169, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

- docs/PHASE89_EXTENDED_CONTROLLER_GAPS_2026-10-09.md; outputs/phase89/attempt,analysis.json,evidence-sha256.json,analyze.py.
- docs/PHASE88_FINE_RX_AND_COEXIST_BUDGET_2026-10-09.md; outputs/phase88b/attempt,analysis.json,analyze.py,evidence-sha256.json.
- docs/PHASE84_86_87_MODE_RESTORE_AND_CADENCE_2026-10-09.md; outputs/phase84c,phase86,phase87.86b remains remote:/home/steamos/pico-ram-work/phase86b; copy before relying on raw facts if absent locally.
- docs/PHASE85_CONTROLLER_SCHEDULE_HARDWARE_2026-10-09.md; outputs/phase85/attempt,analyze85.py,analysis.json.
- docs/PHASE83_BATCH_REGRESSION_AND_84_PREPARATION_2026-10-09.md; docs/PHASE81_82_DENSE_FRAME_SERVICE_2026-10-09.md; docs/SERVICE52_TO_FRAME3000_COMPARISON_2026-10-09.md.
- docs/PHASE79_80_RX_WINDOW_CONTROL_2026-10-09.md; docs/PHASE78_CONTROL_RUNS_2026-10-09.md; docs/PHASE78_DETECT1_PRESTART_REJECTION_2026-10-09.md.
- docs/CONNECTION_AND_RESEARCH_STATE_FOR_EXTERNAL_REVIEW_2026-10-09.md and CONNECTION_STATE_CLAUDE_REVIEW_2026-10-09.md are older frozen review records; do not silently rebuild/replace their60-SHA archive.
- docs/PHASE66_OFFLINE_GATE_2026-10-09.md and outputs/phase66/source,build2 are90 ancestry,verified exact source hash1288f05009f8e20118cb54a48e105a4f00f8cb7e1c347b822ba0a18591bfccb6.
- outputs/phase88/source/build and outputs/phase85/build88.py preserve fine-service ancestry. Original Service38 source is ota38/build-source/roy_ram_station_service27.c,SHA73360563d7acb7083221b281f4bf9de8497aa1b49125c9198da5e24a8513209d; the ordinary current pc source is NOT identical.

## K02352
Source `D0354`, original line179, SHA256 `b9b8074d174d3fa79746514a61f34c5c9080cee0e392120ef745c1d92a84b404`. Status: historical/unadjudicated narrative; English source prose.

No active Codex goal object was found by get_goal at handoff. Do not report a nonexistent goal as paused/completed. Carry the objective from this document and the user's prompt. User-level instructions import RTK.md, but K:/AI/RTK.md was absent when checked; do not invent its contents. Project rules remain available in AGENTS.md.

## K02353
Source `D0355`, original line1, SHA256 `298368f21473f3a37673e21737baea0004dbd40f65a2e7b31c68cdcc479a7e96`. Status: historical/unadjudicated narrative; English source prose.

# RX check42 and mode clarification

## K02354
Source `D0355`, original line3, SHA256 `298368f21473f3a37673e21737baea0004dbd40f65a2e7b31c68cdcc479a7e96`. Status: historical/unadjudicated narrative; English source prose.

User requests fresh Nordic reception confirmation. One20s ordinary no-OTA
service38 run:994 TX-END,0 CRC RX,0 working replies, no fresh identity/IMU
or mode response. Current reception NOT confirmed. Previous mode41 actually
confirmed mode0(stock), protocol revision1; never infer mode1 from revision1.
OTA39 delivered4056/4056 indexed blocks with controllers OFF, not loss-free
RF/coexistence with two controllers. Service38 remains controller-free only.
Evidence outputs/rx42/rx42. Cleanup session/SRAM/MPU/trace all true, inactive.
No new firmware or OTA. Cause of silence unknown.

## K02355
Source `D0356`, original line1, SHA256 `86f2263cc28158db96ba0f5e273d804ae7dea1b135e64a587bb738c19266a0b8`. Status: historical/unadjudicated narrative; English source prose.

# Service52: measured reception using service38 RAM module

## K02356
Source `D0356`, original line3, SHA256 `86f2263cc28158db96ba0f5e273d804ae7dea1b135e64a587bb738c19266a0b8`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User explicitly requested the custom service RAM module and a test.
History checked in AGENTS; measured exclusive controller-free service38 reused.
45-second no-OTA run. Single target[TRACKER_SERIAL]/chip[TRACKER_ID],FW189.
Module SHA25644f4e52b07bbba4c459256c910228d440e25ff0d288238e76047fd21000c4c7c.
Stock service cadence4727us,900us slot,N0/M2,peers1/2 paused, optics off.
Controller-off guard retained. No OTA job, mode setter or mode query selected.

## K02357
Source `D0356`, original line10, SHA256 `86f2263cc28158db96ba0f5e273d804ae7dea1b135e64a587bb738c19266a0b8`. Status: historical/unadjudicated narrative; English source prose.

## Hardware measurements

## K02358
Source `D0356`, original line12, SHA256 `86f2263cc28158db96ba0f5e273d804ae7dea1b135e64a587bb738c19266a0b8`. Status: historical/unadjudicated narrative; English source prose.

- 2189 completed TX-END,2160 CRC-valid RX:98.6752% RX/TX ratio.
- 2156 retained unique answered slots:98.4925% answered/TX ratio.
- 3 host-ring dropped records;2027 retained IMU samples.
- Exact serial and chip confirmed; no runtime error or command retry.

## K02359
Source `D0356`, original line17, SHA256 `86f2263cc28158db96ba0f5e273d804ae7dea1b135e64a587bb738c19266a0b8`. Status: historical/unadjudicated narrative; English source prose.

These are cumulative counters read sequentially, not an exact simultaneous
packet-loss ledger. RF nonresponses and host drops are distinct. IMU sample
gaps are not interchangeable with lost RF packets. No6DoF or valid optical pose.

## K02360
Source `D0356`, original line21, SHA256 `86f2263cc28158db96ba0f5e273d804ae7dea1b135e64a587bb738c19266a0b8`. Status: historical/unadjudicated narrative; English source prose.

This confirms high reception through the service38 path, not100% physical RF
or the tracker being in mode1. The high result is obtained under the stock
service cadence; mode0 vs1 and running revision07 remain unconfirmed because
no validated mode reply was captured. Do not use a10%/100% classifier.

## K02361
Source `D0356`, original line26, SHA256 `86f2263cc28158db96ba0f5e273d804ae7dea1b135e64a587bb738c19266a0b8`. Status: historical/unadjudicated narrative; English source prose.

## Cleanup and evidence

## K02362
Source `D0356`, original line28, SHA256 `86f2263cc28158db96ba0f5e273d804ae7dea1b135e64a587bb738c19266a0b8`. Status: historical/unadjudicated narrative; English source prose.

All cleanup checks true: session closed, SRAM restored, MPU restored,
trace controls restored. Radio service inactive; no loader/OpenOCD remains.
Temporary module unloaded after test, no Frame flash or tracker OTA.
Evidence `outputs/service52/service52/`: invocation/config/registry, radio
report/manifests/recovery/raw records, live snapshot and cleanup.json.
Normal service left unchanged and inactive.

## K02363
Source `D0357`, original line1, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

# Service52 versus Frame-mode receive-window controls

## K02364
Source `D0357`, original line3, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

Static analysis of saved hardware evidence, 2026-10-09. No hardware/RF/OTA launched for this comparison.

## K02365
Source `D0357`, original line5, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

## Verified reference

## K02366
Source `D0357`, original line7, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

Service52: 2160 CRC-valid receptions / 2189 TX_END =98.6752%; retained answered slots2156/2189=98.4925%, IMU2027, host-ring drops3. Its exact ELF SHA25644f4e52b07bbba4c459256c910228d440e25ff0d288238e76047fd21000c4c7c was recalculated and agrees with both deployment.json and the run manifest. Configuration overrides the stale CLI slot/interval arguments; effective values below come from station-config.json. Mode was not queried in Service52: high reception does not itself prove mode0.

## K02367
Source `D0357`, original line9, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

| Parameter | Service52 | Phase79 | Phase80 | Proposed dense Frame control |
|---|---:|---:|---:|---:|
| Requested slot interval, us |4727|6000|5982|3000 nominal, pending RTC reconstruction|
| Slot cycle |1|7|7|1|
| Nominal target period, us |4727|42000|41874|3000|
| Command16 N / M |0 / 2|Frame13 / 2|Frame13 / 2|Frame0 / 2|
| Reserved slot, us |900|900|900|900|
| Slot-relative RX cutoff, us |800|700|700|800|
| BLE phase lock |OFF|OFF|OFF|OFF|
| Controllers |OFF|OFF|OFF|OFF required|
| Mode proof |none|schema2 mode1/rev7|schema2 mode1/rev7|required before cadence change|

## K02368
Source `D0357`, original line21, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

Native blocked-request retries, single active target and paused peers1/2 are part of the service concept. However Service52 uses a different compiled module with TX_END/capture diagnostics and capture-ACK handling; copying numbers into Phase79 does not reproduce the entire measured path.

## K02369
Source `D0357`, original line23, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

## What transfers, and what does not

## K02370
Source `D0357`, original line25, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

The promising transfer is dense polling with N0/M2 and RX800, not merely multiplying a sparse42000us period by3000/4727.5. N13 delays receiver reopening by thirteen base steps. N0 removes that long delay and changes how a delayed MPSL poll can find an awake receiver. This explanation is a hypothesis, not a measured cause of the low Phase79/80 rate.

## K02371
Source `D0357`, original line27, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

Retained Service52 records include both short and long intervals: modulo32-bit DWT differences at64MHz have median32980.1875us, p90 37817us, maximum42654.6875us for consecutive channel2 records. Summed DWT time44.727s agrees with host-read span44.752s. These are inter-received-record intervals, not a complete TX schedule; missing replies and device transmissions cannot be separated from them alone. The requested4727us cadence was not a uniformly realized arrival cadence. A successful profile must tolerate MPSL scheduling gaps rather than assume exact periodic transmission.

## K02372
Source `D0357`, original line29, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

The host RX cutoff and the tracker's receive-open interval are different settings. RX800 gives Frame enough time to retain long replies; it does not cause the tracker to hear a poll outside its own window.

## K02373
Source `D0357`, original line31, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

The firmware selectors contain Frame period3000, normal offset1810.5 and alternate offset2207. The earlier GRID3000 review identifies native0x641c's nonzero-clock branch: divide by30.5, truncate, program RTC0. A nominal3000 maps to98 RTC ticks, or2990.72265625us at32768Hz. Which native timer branch is active, and the RADIO ADDRESS-to-window reference, remain to be established for the live tracker. Do not call3000 an exact programmed hardware period on that branch.

## K02374
Source `D0357`, original line33, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

## Concrete next implementation

## K02375
Source `D0357`, original line35, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

Preserve Service52 and its stock-cadence gate. Prepare a separate no-OTA, controllers-OFF dense Frame diagnostic with the measured service's counters and RX800. Bootstrap and obtain fresh nonce/chip-bound schema2 proof on an audited common cadence; command16 prepares N0/M2 and its real applied state must be verified before handing over to the dense Frame cadence. Keep mode query refresh and liveness checks; do not fabricate mode proof from saved metadata. This requires an explicit phase-specific host policy: the existing validator intentionally requires a common Stock/Frame window, and3000 with StockN0 fails it. Do not globally bypass that validator or reuse the stock OTA profile as a3000 profile.

## K02376
Source `D0357`, original line37, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

Use TX_END, CRC RX and retained answered counts for a comparable result, and record actual transmission intervals. Start with nominal3000 as a bounded hypothesis only after reconstructing the RTC path; do not claim99% until a hardware run demonstrates it. Controllers-ON remains a separate test with the original guards, because dense900us service reservations are not a proven coexistence profile.

## K02377
Source `D0357`, original line39, SHA256 `9ef2c4f23efc7d07b220ce8517d55389a524492b2e9ec43fb3dd4ae67cbe2b89`. Status: historical/unadjudicated narrative; English source prose.

References: docs/SERVICE52_RADIO_TEST_2026-10-08.md; docs/PHASE79_80_RX_WINDOW_CONTROL_2026-10-09.md; docs/FW189_GRID3000_REVIEW_2026-10-07.md; pc/radio/ram/tracker_ota_runtime.py::validate_service27_plan; pc/radio/ram/station_v4_runtime.py::validate_remote_mode_grid; outputs/service52/service52; outputs/ota38/build/deployment.json.

## K02378
Source `D0359`, original line58, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; English source prose.

`646590a28013411de51d54eba8e58a4b936a83887edb5eb86722fe2492db449f`.

## K02379
Source `D0359`, original line64, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; English source prose.

- PC `0x13e54`, LR `0x10507`;
- r0=106 (`0x6a`), r1=472 (`0x1d8`);
- CFSR/HFSR=0;
- module state: 2 slots, 1 blocked, phase3, reply.action3.

## K02380
Source `D0359`, original line91, SHA256 `af71c81f3bf78c5577cb71dffd60c6f8d37ecc36481773d39fcd0fdfa4f61504`. Status: historical/unadjudicated narrative; English source prose.

`a911666fb384972639026d4c9fb828ee7302066d0a7b9076811d4f73a4a325ef`.

## K02381
Source `D0360`, original line21, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; English source prose.

1. command16 body0802: N=8,M=2,sequence1;
2. command16 body0002: N=0,M=2,sequence2.

## K02382
Source `D0360`, original line54, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; English source prose.

`3c9014d1894041aa4323e47cb6cd030de17aa71fdb75c2a37ad6fe0d9aafa8f6`.

## K02383
Source `D0360`, original line56, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; English source prose.

Code SHA256:

## K02384
Source `D0360`, original line58, SHA256 `b3b5526a74091860d769da1f5986e0df98ad39e0d65812dbc146bb8aabbfbca2`. Status: historical/unadjudicated narrative; English source prose.

`cd1c5545795fc1fb329ff2881eca8d35a62953993dbb315feed75d6531485b56`.

## K02385
Source `D0361`, original line42, SHA256 `453bb8a83912cae840a2c326b1a1600be035792c8d19c4640a37d3a99bf38ce1`. Status: historical/unadjudicated narrative; English source prose.

## 4. S1 / ACK

## K02386
Source `D0363`, original line1, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

# Iteration26: native request eligibility countercheck

## K02387
Source `D0363`, original line3, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. Offline development only. No SSH, ADB, SWD, RF, device reset,
deployment, flash or OTA operation was performed. Device state was not queried.

## K02388
Source `D0363`, original line6, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

## Result

## K02389
Source `D0363`, original line8, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

The saved iteration25 callback has a reproducible request eligibility defect.
With an existing NORMAL request and native session state1, low-priority signals
5,6 or7 issue another NORMAL request. The actual stock request API returns -35;
the callback stores the error and sets stop=1. In state2 the same path succeeds
with scheduler submission mocked. Iteration25 is therefore not ready to promote
as a general lifecycle fix. This does not establish that the failure occurred
in a saved hardware run, nor explain assert106/501 or the missing tracker replies.

## K02390
Source `D0363`, original line16, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

The old outputs/service-fast-poll25/test_grid.py intercepts API0x18948 and
unconditionally returns success. It verifies retry arithmetic, but cannot
verify the native API's state eligibility. Historical files remain unchanged.

## K02391
Source `D0363`, original line20, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

## History and provenance

## K02392
Source `D0363`, original line22, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

Read the library entry documents, chronology and relevant original service
reports20,21,22,23, earlier timer-poll198-201, timing-mask191-194, technical
station notes and the iteration24 reviewer records/source/disassembly. Relevant
limits remain: earlier low-priority REQUEST asserts; phase command10 produced
no material RX uplift; cameras are not necessary for assert106/501; native
command16 exists; changing requested N or sequence did not resolve losses.
No failed RF experiment was repeated.

## K02393
Source `D0363`, original line30, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

outputs/service-fast-poll26/audit.py locates selected artifacts through both
provenance/artifact-catalog.csv and provenance/manifest.csv. It rehashes snapshot
and live copies. All38 selected files match their recorded snapshot hashes;
there are no later changes in this selected artifact set. This is not an audit
of every file in the repository or a live device inspection. Live docs were
also checked by modification time; no later service report was found.

## K02394
Source `D0363`, original line37, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

The original Roy image is outputs/service-fast-poll20/roy-stock.bin, SHA256
0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50.
The tested ELF is outputs/service-fast-poll25/build/roy_station.elf, SHA256
a7de084a9e9697e1702b7829d89cbe67577945f14e7490ff0ec376cd9fec8305.
Its source is outputs/service-fast-poll24/roy_ram_station_v4_grid.c, SHA256
e508c9016297fd02c12677c5a6d3837237d31656232f70b3df60156a26b8a1f3.
Both binary hashes are enforced by the native tests.

## K02395
Source `D0363`, original line45, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

Iteration24's actual ELF matches its capture manifest: ee5cc5d3…4084,
1936 code bytes. Its adjacent deployment metadata describes another binary.
The new audit preserves and reports this mismatch; it does not rewrite evidence.
Independent recomputation of7543 adjacent saved software TXEN gaps gives
min1249us, median1256.4375us, max32348.625us. These are not RF-edge timestamps.
Saved run-summary reports cleanup and one fault-free bounded run; it does not
prove continuous stability or currently restored device state.

## K02396
Source `D0363`, original line53, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

## Native control flow

## K02397
Source `D0363`, original line55, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

Stock0x18956..0x18964 admits requestable states1 and2. After request validation,
0x1897e..0x18988 accepts a nonzero request type (NORMAL) only when state==2;
otherwise0x1898a returns r6=-35. EARLIEST takes0x18992 and is eligible in both
states. Request copying runs through actual native0x18874 or0x188a6.

## K02398
Source `D0363`, original line60, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

The dispatch listing at0x1038e..0x10394 can place a low-priority callback into
state1 when the prior state is not4; it preserves the available r0 path when
state4 takes the branch. SESSION_IDLE has a separate state write at0x104c2.
Thus signal number alone is not adequate proof that a NORMAL request is eligible.
The test matrix intentionally supplies state1 andstate2; it does not emulate
all asynchronous dispatch interleavings or actual reservations.

## K02399
Source `D0363`, original line67, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

## Executable verification

## K02400
Source `D0363`, original line69, SHA256 `036f9475e6b81bd73e7de205235439cc6ac96dc061ce0cfeff6484ac8411681f`. Status: historical/unadjudicated narrative; English source prose.

New pc/tests/test_service_grid_native_request.py executes the saved ARM callback,
stock request validation, eligibility and request-copy functions in Unicorn.
Only native session lookup0x10628 and scheduler submission0x18860 are mocked.
There is no simulated RF or hardware claim.
