# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K02161
Source `D0343`, original line20, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

The differing denominators prevent treating the two percentages as a strictly identical loss metric. In Phase82, TX_END minus CRC RX is297; CRC RX minus retained answered slots is494, of which493 ring drops are recorded. Packet/discovery and duplicate accounting differ by one; sequential counter sampling does not establish an exact packet-by-packet loss ledger.

## K02162
Source `D0343`, original line22, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

During the controls, the tracker was live and connected with fresh nonce/chip-bound schema2 mode1/revision7 proof. Final connected=false is expected because publish(...,running=False) explicitly disables connected at shutdown; it is not evidence of a live link failure. Phase81 final IMU age0.696s, Phase82 0.666s. Phase82 additionally recorded one mode-query timeout and115 command retries; final proof remained fresh. This is not an error-free command-delivery result. Pose validity remains false.

## K02163
Source `D0343`, original line24, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

## Implementation and interpretation

## K02164
Source `D0343`, original line26, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

Phase81 uses exact ELF8ab01f97ffb07953d755b5a97f1ccad5df4b253cb643facdc39466a48833b5fb. Phase82 uses exact Service52 ELF44f4e52b07bbba4c459256c910228d440e25ff0d288238e76047fd21000c4c7c, verified before execution. Its blocked-slot handler attempts grid-preserving normal re-requests when the native MPSL state permits, otherwise earliest requests. Phase81 always takes earliest retry. The service modules also differ in counters/capture-ACK handling; this experiment does not isolate one instruction as the sole cause.

## K02165
Source `D0343`, original line28, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

The successful transfer consists of the dense cycle1/N0/M2 configuration, RX800, and the original measured service module. Merely copying nominal3000 into a different module did not reproduce the result. Service52 itself measured98.6752% CRC RX/TX_END at stock cadence, without proving mode; Phase82 now demonstrates high reception with actual mode1 proof.

## K02166
Source `D0343`, original line30, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

A narrowly scoped dense_frame_diagnostic preflight permits only the exact two module-code hashes, target[TRACKER_ID], nominal3000us, cycle1, N0 for both query configurations, schema2, no desired-mode setter, slot900/RX800, controllers OFF, optics OFF and no OTA job. Runtime separately validates this policy and requires a Frame reply before accepting configuration. The ordinary common Stock/Frame window validator and the stock service/OTA gate remain unchanged for all other profiles.

## K02167
Source `D0343`, original line32, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

Unlike the originally proposed two-stage handover, this finite diagnostic bootstraps directly at dense cadence. Discovery/offer traffic is native and does not rely on an assumed mode; if no valid proof is acquired, it does not claim mode or start configured streaming. This allows validating an already-installed Frame tracker without inventing a live proof from the previous run. Native discovery can reset N; the actual proof and applied N must be read from raw replies, not inferred from a saved final state.

## K02168
Source `D0343`, original line34, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

Nominal3000 is a hypothesis setting, not proof of the live timer branch or an exactly realized period. RTC0 conversion and actual requested/granted intervals still need characterization. Dense controller-free slots are not a proven safe two-controller coexistence schedule.

## K02169
Source `D0343`, original line36, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

## Validation, evidence and next work

## K02170
Source `D0343`, original line38, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

114 offline checks passed before Phase81 (outputs/phase81-offline-tests.xml), including exact-native schema2 reply acceptance, rejection of stock before command16, and preservation of the ordinary common-grid rejection. After admitting the exact Service52 module,76 relevant checks passed. Full isolated releases and frozen invocation/configuration, raw records, controller guard metrics, report/recovery/cleanup: outputs/phase81 and outputs/phase82. Sources and preparation scripts are preserved under outputs/phase79/prepare81.py and prepare82.py.

## K02171
Source `D0343`, original line40, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

Both wrappers exit0; session closure, reserved RAM restore, MPU restore and trace-control restore are verified true. Temporary receiver stopped. Historical Service52, Phase79/80 and the external-review archive remain preserved.

## K02172
Source `D0343`, original line42, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

Next: reduce host-ring drops without weakening publication atomicity or freshness checks, capture a comparable steady-state TX/RX loss ledger, then address the remaining RF misses. The99% objective is not achieved. The original>=60% objective with two working controllers also remains unachieved; these results concern controllers OFF only.

## K02173
Source `D0343`, original line44, SHA256 `7c46a5c9a8394c08e630e61c2c5918c0a7482ade91f38b55a03ed56c811c162c`. Status: historical/unadjudicated narrative; English source prose.

Raw Phase82 proof: six fe02 bodies, all(mode=1, revision=7, actualN=0, native_pending=0). These were verified by the nonce/chip matching engine; the hidden remote pending limitation remains separate. Evidence SHA inventory: outputs/phase82/evidence-manifest.json.

## K02174
Source `D0344`, original line1, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

# Phase83 host batching regression; Phase84 RTC-derived control prepared

## K02175
Source `D0344`, original line3, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. User requests99% reception, then controller schedule measurement and conflict-free coexistence. No OTA, tracker mode setter or persistent deployment.

## K02176
Source `D0344`, original line5, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

## Phase83 hardware result

## K02177
Source `D0344`, original line7, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

60-second controller-OFF control using the exact Service52 ELF, nominal3000us/cycle1, FrameN0/M2, RX800. Only host mailbox publication is coalesced across each copied record ring.94 offline checks passed, including old-tag rejection for an unpublished next command and no staged flush after an exception.

## K02178
Source `D0344`, original line9, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

4538 TX_END,590 CRC RX (13.0013%),575 retained answered slots (12.6708%),13 host-ring drops,349 IMU samples. Fresh mode1/rev7 proof was obtained during the run. At roughly the middle of the run the target ceased replying; final IMU age27.593s, mode unverified and connection confirming. This is a real liveness failure, unlike connected=false caused solely by final running=false in Phase82.

## K02179
Source `D0344`, original line11, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

The lower drop count is not evidence of a net improvement: traffic was much lower, and eventual RF silence dominates the result. Whether the tracker slept, batching altered protocol behavior, or timing shifted is unknown. The candidate is not accepted as the default. It is retained behind dense_mailbox_batch=false by default; Phase84 explicitly disables it.

## K02180
Source `D0344`, original line13, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

Wrapper exit0, session closed/RAM/MPU/trace restore all verified true. Full isolated tested release, raw records, invocation, controller metrics and cleanup are preserved in outputs/phase83. User was asked to wake and gently move the target during the next control, keeping controllers OFF. No automatic RF retry is performed while that confirmation is pending.

## K02181
Source `D0344`, original line15, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

## Phase84 static preparation

## K02182
Source `D0344`, original line17, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

The installed candidate07 binary was executed offline through its actual0x641c conversion paths. FrameN0 normal1810us ->59 RTC ticks or1810 TIMER2 ticks; alternate2207us ->72 RTC ticks or2207 TIMER2 ticks; miss2975us ->97 RTC ticks or2975 TIMER2 ticks.3000us ->98 RTC ticks, physically2990.72265625us at32768Hz. This verifies code arithmetic; the active branch on the real tracker remains unknown.

## K02183
Source `D0344`, original line19, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

A single derived candidate2989us (=98*30.5) is prepared, with exact Service52 module, original per-record host publication, N0/M2/cycle1/RX800, fresh schema2 proof before configuration, controllers-OFF guard and no setter/OTA. It tests the RTC timing hypothesis without changing tracker firmware. It is not proven correct for the live clock branch and is not a controller-ON schedule.

## K02184
Source `D0344`, original line21, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

Narrow dense preflight permits only3000 or2989us with equal initial/steady periods and the exact existing module/identity/profile restrictions. Ordinary common-grid and stock OTA validation stay unchanged.80 relevant checks passed, including exact candidate07 conversion and mode-proof guards; outputs/phase84-offline-tests.xml. Preparation script outputs/phase79/prepare84.py and frozen remote invocation are ready; Phase84 has not been launched as of this record.

## K02185
Source `D0344`, original line23, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

## Controller stage

## K02186
Source `D0344`, original line25, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

The controller schedule must be measured under native Frame operation before custom RF is admitted. Existing Phase73 SWD brackets were too wide; Phase76 native sampler data did not establish a robust repeated700us gap. Phase77 provides a hardened native sampler candidate but its exact runner/recovery policy still needs sealing and validation. Frequency separation alone cannot resolve a single-radio scheduling conflict. No new controller coexistence evidence or99% claim is made.

## K02187
Source `D0344`, original line27, SHA256 `d6d8a2e7aab2b71bb1bb46f79cba6edf9b1c5c4acb35ab84aa1e6acc54b3d923`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7-medium read-only review attempt timed out after330s (chat56dab74f-369f-42c7-94f0-3caba0bc7f47); no review verdict was received. Status preserved in outputs/phase83/grok-review-status.json. This is a timeout, not a quota failure or an approval.

## K02188
Source `D0345`, original line1, SHA256 `2689ce41d44100de3b1010925293ec1bb6c6a625e4139f83c9050d8f11e056ad`. Status: historical/unadjudicated narrative; English source prose.

# Mode restoration and two dense cadence controls

## K02189
Source `D0345`, original line3, SHA256 `2689ce41d44100de3b1010925293ec1bb6c6a625e4139f83c9050d8f11e056ad`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09. User confirms target awake/controllers OFF. No OTA or persistent flash. Target [TRACKER_SERIAL]/chip[TRACKER_ID].

## K02190
Source `D0345`, original line5, SHA256 `2689ce41d44100de3b1010925293ec1bb6c6a625e4139f83c9050d8f11e056ad`. Status: historical/unadjudicated narrative; English source prose.

## History and ordered attempts

## K02191
Source `D0345`, original line7, SHA256 `2689ce41d44100de3b1010925293ec1bb6c6a625e4139f83c9050d8f11e056ad`. Status: historical/unadjudicated narrative; English source prose.

1. Phase84: nominal2989us was rejected by initialize_v4's general3000us minimum before debugger/RF entry. Original stdout retained. A narrow opt-in initializer permits only derived2989us or interior2600us with dense diagnostic, equal steady cadence, cycle1, slot900/RX800, paused peers1/2 and normal priority. General lower bound remains unchanged.50 relevant checks passed, including default rejection and mismatched-profile rejection.
2. Phase84b: allowed2989us run returned a fresh schema2 mode0/revision7/N0/native_pending0 reply. Dense Frame guard intentionally rejected it and cleaned up.17 CRC RX/632TX_END,11 retained answers,0IMU. This is not a measured mode1 cadence failure; the tracker was actually stock at this point.
3. Phase86: audited common8800us, StockN1/FrameN2, cycle1, RX800, service79 module. Authorized remote mode policy used; no OTA. Actual raw replies prove(mode0,rev7,N0,pending0), then(mode0,rev7,N1,pending0), then(mode1,rev7,N2,pending0). This is applied-mode proof, not merely a staging ACK.30s run:449 nominal answers/2532slots,482IMU,2query timeouts,253 retries; fresh Frame proof at shutdown. It is bootstrap/control only, not a good data profile.
4. Phase84c: exact Service52 module, FrameN0/M2, cycle1, RX800, original per-record mailbox publication,2989us,60s. Six raw schema2 proofs all(mode1,rev7,N0,pending0). Final4686CRC/5245TX_END=89.3422%;4436retained/5245=84.5758%;249ring drops;2624IMU;101command retries;0mode-query failures. Live sample age0.671s at shutdown; final connected=false follows running=false. Intermediate96.96%CRC was cumulative at an earlier point, not the final result or proof of sustained improvement.
5. Phase87: same exact Service52 module and host publication, only nominal interval2600us,60s. This interval is inside both ideal FrameN0 window models, away from their closing edge. Result894CRC/5973TX_END=14.9674%;893retained=14.9506%;0ring drops;294IMU;250retries. Initial mode1 proof expired after30s;2query failures, final mode unknown. Last sample age0.699s indicates target still emitting data; do not explain this result as sleeping. The nominal interior-window hypothesis does not predict a successful live stream and is rejected as a preferred setup.

## K02192
Source `D0345`, original line13, SHA256 `2689ce41d44100de3b1010925293ec1bb6c6a625e4139f83c9050d8f11e056ad`. Status: historical/unadjudicated narrative; English source prose.

All entered RF attempts84b/86/84c/87 verify session close, complete reserved RAM restore, MPU restore and trace-control restore.84b exits1 by guard, others exit0. No persistent radio remains. Stock-at84b is a measured fact; that a power cycle caused it is an inference, since the user's exact button/power sequence was not recorded.

## K02193
Source `D0345`, original line15, SHA256 `2689ce41d44100de3b1010925293ec1bb6c6a625e4139f83c9050d8f11e056ad`. Status: historical/unadjudicated narrative; English source prose.

## Conclusion and next investigation

## K02194
Source `D0345`, original line17, SHA256 `2689ce41d44100de3b1010925293ec1bb6c6a625e4139f83c9050d8f11e056ad`. Status: historical/unadjudicated narrative; English source prose.

Neither derived2989us nor interior2600us reached99%; the best prior mode1 control remains Phase82:94.3277% CRC/TX_END. No new cadence is accepted as a robust solution. Avoid further blind period sweeps. Need actual MCU transmission timing, native tracker timing/reference behavior and mode-query/reliable-record interaction; copying ideal windows is insufficient. Ring drops and absent CRC answers remain distinct. SixN0 proofs do not reveal the live timer branch or hidden pending transition flag.

## K02195
Source `D0345`, original line19, SHA256 `2689ce41d44100de3b1010925293ec1bb6c6a625e4139f83c9050d8f11e056ad`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/phase84, phase84b, phase86, phase84c and phase87; all original attempts preserved. Preparation scripts outputs/phase79/prepare84b.py, prepare84c.py, prepare86.py and prepare87.py. Test record outputs/phase84b-offline-tests.xml and phase87-offline-tests.xml. Controller-only sampler85 is a separate no-MPSL/no-tracker-RF stage, launched only after the user confirms both hands tracking; its result must be reported separately.

## K02196
Source `D0346`, original line1, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

# Phase85: native controller schedule observations

## K02197
Source `D0346`, original line3, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

Hardware2026-10-09. User explicitly confirms both Frame hands tracking. Native diagnostic only: no tracker packet TX, no MPSL timeslot session, no flash. Exact sampler ELF d06d36e0bdc8bdcd68fc2916041ed1275ac0c533c9a863d6594051c7ab0cd8e1 and runner19c719e01f691ef370d7ebbccedcabbc0dc713899a00e4d853cf17b2e97e2017 are checked on Frame before launch.

## K02198
Source `D0346`, original line5, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

## Acquisition and safety observations

## K02199
Source `D0346`, original line7, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

10s sampling,34 native invocations,2176raw samples.2170have zero RTC bracket span;6cross a tick and are excluded from phase analysis. Native callback duration3.632..5.554ms, median4.532ms.12controller metrics windows, both controllers0invalid samples and0logged maximum freeze. Full reserved RAM, MPU static count, MPU control and native hook restoration verified; post-cleanup controller validity verified true. Wrapper exit0. These observations do not establish safety of arbitrary future radio traffic.

## K02200
Source `D0346`, original line9, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

BASE0 observations:2858104320 in1352accepted samples,3046980096 in818. BASE0 is read after the timestamp/STATE bracket, so address/state simultaneity and per-controller identity are not proven. Do not label these addresses left/right without additional identity evidence.

## K02201
Source `D0346`, original line11, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

## Conservative phase analysis

## K02202
Source `D0346`, original line13, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

Fold against3000us using reference RTC1 tick6430096. Each claimed quiet bin needs at least3observations and <=5%busy; unknown bins are excluded. All/first-half/second-half are evaluated separately using the same reference. Precision-padded widths subtract two32768Hz ticks (61.035us); this is a conservative analysis allowance, not a hardware admission guarantee.

## K02203
Source `D0346`, original line15, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

| Phase bins | Whole raw quiet width | First-half width | Second-half width | Second-half precision-padded width |
|---|---:|---:|---:|---:|
|30 (100us)|1100us|1100us|700us|638.965us|
|60 (50us)|600us|600us|550us|488.965us|
|98 (~30.61us)|581.633us|581.633us|520.408us|459.373us|

## K02204
Source `D0346`, original line21, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

Incomplete coverage remains: whole-run covered bins24/30,36/60,56/98. Finer second-half coverage48/98. These data do not prove a repeated1100us gap; coarse width changes with binning and time segmentation. Nor do they prove no longer usable interval exists: sampling coverage is incomplete. Do not select a900us service reservation solely from the coarse fold.

## K02205
Source `D0346`, original line23, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

The successful prior dense tracker profile requires900us reservations and800us RX, plus scheduling/padding constraints. These observations do not establish a compatible gap. Even a nominal600us coexistence slot with400us RX needs admission and native guard checks; hardware handshake/mode replies and IMU have different lengths. A shorter, precisely measured exchange or different scheduling policy must be established before combining devices.

## K02206
Source `D0346`, original line25, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

## Evidence and next steps

## K02207
Source `D0346`, original line27, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

Raw samples/calls, controller metrics, OpenOCD log, recovery and hardware report: outputs/phase85/attempt. Reproducible offline analysis: outputs/phase85/analyze85.py and analysis.json. This supersedes the preparation-only status in PHASE85_CONTROLLER_SCHEDULE_PREPARED_2026-10-09.md; historical preparation remains unchanged.

## K02208
Source `D0346`, original line29, SHA256 `a71811f721e270831b6ea5cb93995fb2da55125f8b8bc5722a6d8e3377ed7e97`. Status: historical/unadjudicated narrative; English source prose.

Next investigations: actual tracker poll/END timing and hardware timer branch, reliable-mode refresh interaction, denser controller phase coverage or native BLE schedule/ownership evidence. Do not relax controller guards or promise conflict-free operation from this histogram. No combined controller+tracker custom RF run was performed.99% tracker delivery and>=60% with two working controllers remain unachieved.

## K02209
Source `D0347`, original line1, SHA256 `8f0d548ca12aed37fc8e7be4c3e629387fc429c175c0cd9fadcf8c2283aee216`. Status: historical/unadjudicated narrative; English source prose.

# Controller-only schedule measurement prepared

## K02210
Source `D0347`, original line3, SHA256 `8f0d548ca12aed37fc8e7be4c3e629387fc429c175c0cd9fadcf8c2283aee216`. Status: historical/unadjudicated narrative; English source prose.

2026-10-09, static preparation only. User requests measuring both controllers and placing tracker exchanges without conflicting with them. No new controller RF experiment has run.

## K02211
Source `D0347`, original line5, SHA256 `8f0d548ca12aed37fc8e7be4c3e629387fc429c175c0cd9fadcf8c2283aee216`. Status: historical/unadjudicated narrative; English source prose.

The sealed runner outputs/phase85/native_probe85.py adapts the restored Phase76 CPU sampler workflow to the exact hardened Phase77 module: ELF d06d36e0bdc8bdcd68fc2916041ed1275ac0c533c9a863d6594051c7ab0cd8e1,244 bytes. Runner SHA25619c719e01f691ef370d7ebbccedcabbc0dc713899a00e4d853cf17b2e97e2017. Completion precedes native hook release; recovery uses the loader-compatible mpu_static_word key. Fault SCS registers are retained on error. Full reserved RAM/count/static restoration and post-cleanup controller validity checks remain mandatory. No MPSL session or RADIO task is launched by the sampler. The four exact-module ARM cases pass.

## K02212
Source `D0347`, original line7, SHA256 `8f0d548ca12aed37fc8e7be4c3e629387fc429c175c0cd9fadcf8c2283aee216`. Status: historical/unadjudicated narrative; English source prose.

Each observation brackets RADIO STATE with RTC1 reads under a brief saved/restored PRIMASK; BASE0 is read afterward and cannot be claimed simultaneous. Sampling is bounded64 observations/8192 spins per native invocation,10s total, with two valid native controllers and150ms freeze guard. The raw fields retain before/after/span uncertainty. A read-only native5-sample snapshot during preparation shows both controllers currently disconnected; no schedule is measured from absent controllers.

## K02213
Source `D0347`, original line9, SHA256 `8f0d548ca12aed37fc8e7be4c3e629387fc429c175c0cd9fadcf8c2283aee216`. Status: historical/unadjudicated narrative; English source prose.

Analyze full and split time segments, phase coverage and uncertainty before claiming a repeated free interval. Unknown/unsampled bins must not count as quiet. Historical Phase73 SWD sampling and Phase76 native folds do not establish a robust repeated700us gap. BASE0 observations alone do not establish which individual controller owns every event.

## K02214
Source `D0347`, original line11, SHA256 `8f0d548ca12aed37fc8e7be4c3e629387fc429c175c0cd9fadcf8c2283aee216`. Status: historical/unadjudicated narrative; English source prose.

The successful dense tracker control uses900us reservations and RX800. It cannot simply be placed into a measured600us quiet interval. A coexistence implementation may need shorter sensor exchanges, separate long-reply setup windows and a reduced polling rate. High delivered-data reliability may require retransmission/integrity semantics rather than99% success of every physical attempt; these are different metrics. No controller freeze guard is relaxed, and a diagnostic fold is not an admission guarantee.

## K02215
Source `D0347`, original line13, SHA256 `8f0d548ca12aed37fc8e7be4c3e629387fc429c175c0cd9fadcf8c2283aee216`. Status: historical/unadjudicated narrative; English source prose.

Run this stage after the pending controller-OFF tracker control and explicit controller wake/readiness. The prepared runner has not been installed or invoked, and the ordinary runtime remains unchanged.

## K02216
Source `D0348`, original line1, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

# Phase88: fine RX observation and coexistence budget

## K02217
Source `D0348`, original line3, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

Hardware measurement on 2026-10-09. Controllers OFF, target [TRACKER_SERIAL] ON. No OTA, flash or tracker firmware change. Initial Phase88 stopped safely after a fresh mode0 proof. A separate common-grid Phase86b restored mode1; Phase88b obtained its own mode proof before continuing. The user subsequently explained that the tracker had been power-cycled. This is consistent with the reset mode; it is not proof of the cause of every earlier reset.

## K02218
Source `D0348`, original line5, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

## Candidate and reproducible evidence

## K02219
Source `D0348`, original line7, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

The original Service38 source was used with only two nominal RX polling increments changed from100us to10us. The deadline remains800us; the original deadline-first END handling is unchanged. ELF SHA256:8965f716df5a02b66abb9c7c636f84543335d90774d445415b932f0b1cd6cb3c. CODE SHA256:f0771efd3caed44f2af55d5a176dcb577fde5a62f817187edd25e14434fabf79. Exact-binary tests and loader/peer regression checks:53 passed, outputs/phase88/offline-tests.xml.

## K02220
Source `D0348`, original line9, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

Full Phase88b evidence: outputs/phase88b/attempt. Reproduce statistics with outputs/phase88b/analyze.py; analysis.json and evidence-sha256.json preserve computed results and the local evidence inventory. Earlier attempts remain unchanged.

## K02221
Source `D0348`, original line11, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

## Controller-OFF result

## K02222
Source `D0348`, original line13, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

30s, nominal3000us, N0/M2, cycle1, slot900us/RX800us, original mailbox publication.

## K02223
Source `D0348`, original line15, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

| Metric | Measured |
|---|---:|
| Completed TX_END |1601|
| CRC-valid RX |1546 (96.5646% of TX_END)|
| Retained unique answered slots |1533 (95.7527% of TX_END)|
| Host ring drops |11|
| IMU samples |962|
| Mode query failures |1|
| Command retries |129|

## K02224
Source `D0348`, original line25, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

The2504 scheduler slots are not the1601 completed transmissions and must not replace the TX denominator. Final mode=frame, verified=true. Final connected=false accompanies running=false after shutdown; it does not prove an RF disconnect. Optical pose remains invalid. Reception percentage is not an IMU completeness or firmware delivery guarantee.99% remains unachieved.

## K02225
Source `D0348`, original line27, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

Session closure, reserved RAM restoration, MPU restoration and trace-control restoration are all verified true in cleanup.json.

## K02226
Source `D0348`, original line29, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

## Response duration observations and limitations

## K02227
Source `D0348`, original line31, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

Channel2, CRC-valid retained records, grouped by full DMA byte length:

## K02228
Source `D0348`, original line33, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

| Bytes | Count | Median callback observation, us | Maximum, us |
|---|---:|---:|---:|
|3|229|215|305|
|12|324|243|334|
|24|336|299|393|
|30|604|326|418|
|38|8|359.5|445|
|41|3|369|459|
|50|13|407|412|
|56|6|437|440|

## K02229
Source `D0348`, original line44, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

These are slot-relative callback observations, not hardware-captured END timestamps. Nominal10us polling does not guarantee10us accuracy: interrupt latency remains, including roughly90us differences. These values provide observed upper bounds, not proof that the packet physically ended at that instant. Do not claim a259us physical exchange is impossible solely from callback observations.

## K02230
Source `D0348`, original line46, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

## Admission based on controller evidence

## K02231
Source `D0348`, original line48, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

Phase85 finer second-half quiet width is520.408us with incomplete coverage. Subtract two RTC ticks61.035us and100us lead plus100us post allowance: conservative RX budget259.373us. Ordinary24/30-byte observed callbacks do not demonstrate a fit. Long handshake and mode replies also require separate treatment. The900us service reservation cannot be admitted from this histogram.

## K02232
Source `D0348`, original line50, SHA256 `12f22d8073744427cad82f1ce3b7796f9e893f61080afbac4186276ea657fa8f`. Status: historical/unadjudicated narrative; English source prose.

Next step: longer native-only controller acquisition with unchanged150ms guard, to improve coverage and test phase stability across several time segments and longer multiples of3000us. Recompute a fresh phase reference; the old absolute reference is not a live scheduler anchor. Unknown bins remain unknown. A combined RF run requires a supported gap and integrated deadline handling; no joint RF claim is made here.

## K02233
Source `D0349`, original line1, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

# Phase89: extended native controller gap measurement

## K02234
Source `D0349`, original line3, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

Hardware, 2026-10-09. Both controllers enabled and tracked, user confirmed readiness.60s CPU-only native sampler,206 invocations,13184 observations;13144 have zero RTC bracket span.40 crossing-tick observations excluded. No tracker RF packets, MPSL radio session, OTA or flash writes.

## K02235
Source `D0349`, original line5, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

Exact sampler ELF SHA256 d06d36e0bdc8bdcd68fc2916041ed1275ac0c533c9a863d6594051c7ab0cd8e1. Runner f42b6c9232cbeb38f909f68bb10908562cda6d7e58ce20e9d1a08cd8ec582e0f differs from the measured Phase85 runner only by acquisition duration10->60s.150ms controller guard unchanged. Both controllers: maximum invalid samples0, logged maximum freeze0ms. RAM, MPU static reservation, MPU control and native hook restoration verified; both controllers valid after cleanup.

## K02236
Source `D0349`, original line7, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

## Conservative offline admission

## K02237
Source `D0349`, original line9, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

Reference RTC tick9524117. Six chronological observation segments,50us phase bins. Every shared quiet bin requires at least3 observations and<=5%busy independently in each segment. Unknown bins are not admitted. Subtract two RTC ticks61.035us and100us lead plus100us post from the common width.

## K02238
Source `D0349`, original line11, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

| Fold period | Common quiet width | Conservative RX budget |
|---|---:|---:|
|3000us|600us|338.965us|
|6000us|600us|338.965us|
|9000us|600us|338.965us|
|12000us|550us|288.965us|
|18000us|500us|238.965us|
|24000us|450us|188.965us|

## K02239
Source `D0349`, original line20, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

For3000us the observed common interval starts at800us relative to the reference and ends at1400us. This historical phase is not a reusable absolute live scheduling anchor. Native STATE sampling does not establish a guaranteed exclusive reservation, and statistical<=5% busy is not collision-free proof.

## K02240
Source `D0349`, original line22, SHA256 `42468a13bd1d9fee2f9dc9a10ffe3365acfb35ecc6dd68a7ceea6ddefeb4eade`. Status: historical/unadjudicated narrative; English source prose.

Phase88b ordinary24/30-byte IMU median callback observations299/326us are within the339us budget; maxima393/418us are outside. IRQ observation lateness prevents equating these with exact packet END. Long mode/identity replies do not demonstrate a fit.900us service admission is unsupported.
