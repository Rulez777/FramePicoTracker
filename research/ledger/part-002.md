# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K00081
Source `D0162`, original line45, SHA256 `6a25d69ea984f5ef6a2a3e04c9aa479f5fae8468c6b80276a18d45c8861d748d`. Status: historical/unadjudicated narrative; English source prose.

- `slot_end_margin_us` 100, so `D(1892)=500`. Early return stays there.
- Quiet gate becomes lead + that deadline + 100 µs post = 700, so a measured 700 µs run is allowed.

## K00082
Source `D0162`, original line48, SHA256 `6a25d69ea984f5ef6a2a3e04c9aa479f5fae8468c6b80276a18d45c8861d748d`. Status: historical/unadjudicated narrative; English source prose.

Do not also switch to phase68’s 3000 µs / cycle 1. That attempt never transmitted. The earlier fast 3000/cycle-1 run aborted on controller tracking. Do not replace the first request with NORMAL: type 1’s distance is from the previous slot start, and every image here opens with type 0 earliest (`a=length`, `b=timeout`). Nothing in this tree shows the stock request API accepts NORMAL as the first call. The ±60 µs skip remains the tested control for an off-phase earliest slot.

## K00083
Source `D0162`, original line50, SHA256 `6a25d69ea984f5ef6a2a3e04c9aa479f5fae8468c6b80276a18d45c8861d748d`. Status: historical/unadjudicated narrative; English source prose.

The result that matters is whether fe02/product frames appear with `callback_observation_us` < 500 while the guard stays quiet. Two more empty `010300` frames would mean the long reply is still past the new gate; the new histogram then decides a later step. It is not a 60% claim, and mode53’s ratio does not transfer to this grid.

## K00084
Source `D0162`, original line52, SHA256 `6a25d69ea984f5ef6a2a3e04c9aa479f5fae8468c6b80276a18d45c8861d748d`. Status: historical/unadjudicated narrative; English source prose.

Finer 60 µs bins (50 across 3000 µs) would only stop one 6% edge from deleting a whole 100 µs bin under the 0.05 cap. About 4000 samples still leave well above 5 hits per bin. Phase68 already has a 700 µs run at 100 µs bins, so the deadline test does not need a bin change. Mixing them would make a pass or a guard abort ambiguous.

## K00085
Source `D0163`, original line1, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

# PICO / Steam Frame: connection and research state

## K00086
Source `D0163`, original line3, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

**External review handoff — English, 2026-10-09 (Europe/Warsaw).**

## K00087
Source `D0163`, original line5, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Repository: `K:\AI\pico`. Latest native-device snapshot was collected around **2026-10-08 23:27:51 UTC / 2026-10-09 01:27:51 CEST**. This is a bounded state report, not a promise of current connectivity after that timestamp. Quantitative values below were checked against retained reports and raw JSON artifacts. Machine-readable data and SHA256 evidence manifest: `outputs/connection-state-20261009/`.

## K00088
Source `D0163`, original line7, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

## 1. Decision summary

## K00089
Source `D0163`, original line9, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

**The requested result — at least 60% real PICO reception while two Valve controllers keep working — has NOT been achieved.**

## K00090
Source `D0163`, original line11, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- High controller-free reception is measured: **2160 CRC-valid receptions / 2189 completed TX-END = 98.6752%** in Service52.
- Firmware revision07 execution and an actual remote **mode0 → mode1** transition were proved in Mode53, with controllers off. That proof subsequently expired. **Current tracker mode is unknown.**
- Three completed coexistence runs preserved healthy controllers, but obtained only **1–2 retained replies per approximately 1433 scheduled tracker opportunities**, **zero IMU samples**, and no completed PICO connection or fresh mode proof.
- A new CPU-side BLE diagnostic now works. Phase76 retained **2112 observations**, with **2090 accepted zero-tick brackets**, and no controller invalid samples in **12 logged metric windows**. It did not transmit to the tracker.
- The custom receiver is currently stopped. Normal Frame firmware remains installed; no Frame flash was written. The most recent temporary sampler's RAM/checkpoint restoration passed readback.

## K00091
Source `D0163`, original line17, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

## 2. Devices and current observation scope

## K00092
Source `D0163`, original line19, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Target PICO Motion Tracker:

## K00093
Source `D0163`, original line21, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- Serial: **[TRACKER_SERIAL]**.
- Chip ID, little-endian wire bytes: **[TRACKER_ID]**; numeric value `[TRACKER_ID]`.
- Stock application version: **189**. This number alone cannot distinguish stock from the fork.
- Last specifically proved fork protocol revision: **07**.
- Current mode, current decimation N, current awake/asleep state and current PICO RF connection: **not freshly measured**. The last active radio tests did not establish them.

## K00094
Source `D0163`, original line27, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Steam Frame station:

## K00095
Source `D0163`, original line29, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- Onboard Roy: nRF52833; exact stock build **1788551713**.
- Stock image SHA256: `0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50`.
- No external dongle, no Frame firmware flash, no tracker SWD/opening.
- Snapshot: **100 OpenVR observations over 1.003142343 seconds**. HMD valid in **100/100**; both controllers connected, valid and tracking result200 in **100/100**.
- Process check found no running custom RAM loader, native sampler or OpenOCD. `systemctl --user is-active frame-pico-radio.service` returned `inactive`.
- This native snapshot proves the two controllers/HMD were tracked during that second. It does **not** prove the PICO tracker is connected, that mode1 is active, or that controller RF loss is zero.

## K00096
Source `D0163`, original line36, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Optics remain disabled in these radio experiments. `pose_valid=false` is intentional. No optical 6DoF success is claimed.

## K00097
Source `D0163`, original line38, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

## 3. Firmware identity and OTA history

## K00098
Source `D0163`, original line40, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Last uploaded tracker candidate: `outputs/remote44/candidate07-initial-connection/fw189-dual-mode.bin`.

## K00099
Source `D0163`, original line42, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- Size: **97,832 bytes**.
- SHA256: `40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5`.
- OTA45 failed after **2559 acknowledged blocks**, waiting for block2560; no verified finish.
- Fresh OTA46 succeeded: **4077/4077 indexed blocks acknowledged**, positive final response and `finish_verified`; **19 retry intervals**. Both controllers were off.
- A successful application transfer is not 100% physical RF reception, bootloader recovery proof or cryptographic readback of every running flash byte.
- Mode53 later produced revision07-specific, nonce/chip-validated replies. Thus the running tracker executed revision07-specific code; this is stronger than observing version189, and weaker than full internal-flash SHA256 readback.

## K00100
Source `D0163`, original line49, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Bootloader extraction/recovery remains unproved. No new OTA is part of the current coexistence investigation. Revision07's known A1 limitation remains: a staged remote transition is not exposed in the query reply and can survive disconnect. `native_N_pending=0` does not rule out that separate hidden staging flag.

## K00101
Source `D0163`, original line51, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Primary reports: OTA46 (original reference: `OTA46_REVISION07_TRANSFER_SUCCESS_2026-10-08.md`), Mode53 (original reference: `MODE53_REMOTE_SWITCH_HARDWARE_2026-10-08.md`), revision07 external audit (original reference: `FW189_REMOTE_MODE44_CLAUDE_AUDIT_2026-10-08.md`).

## K00102
Source `D0163`, original line53, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

## 4. Reception measurements — keep denominators separate

## K00103
Source `D0163`, original line55, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

### Service52: exclusive service profile, controllers OFF

## K00104
Source `D0163`, original line57, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

45-second run, service38 RAM, stock service cadence4727 us, reservation900 us, N0/M2, other tracker peers paused.

## K00105
Source `D0163`, original line59, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- Completed TX-END: **2189**.
- CRC-valid RX: **2160** → **98.6752% RX/TX**.
- Retained unique answered slots: **2156** → **98.4925% retained answered/TX**.
- Retained IMU samples: **2027**.
- Host-ring dropped records: **3**.
- Exact target serial/chip confirmed; all four session/RAM/MPU/trace cleanup checks true.
- No fresh validated mode query was selected in this run. **98.68% does not identify mode0 or mode1.**

## K00106
Source `D0163`, original line67, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

These cumulative counters were read sequentially. They are not an exact simultaneous loss ledger. CRC receptions, unique answers, IMU samples and host-ring losses are different quantities.

## K00107
Source `D0163`, original line69, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

### Mode53: remote switching proof, controllers OFF

## K00108
Source `D0163`, original line71, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

60-second run, common8800 us cadence, reservation900 us, stockN1/frameN2.

## K00109
Source `D0163`, original line73, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- Completed TX-END: **1601**.
- CRC-valid RX: **283** → **17.6765% RX/TX**.
- Retained unique answered slots: **236** → **14.7408% retained answered/TX**.
- Retained IMU samples: **251**; host-ring drops: **0**.
- Whole-run counts include enrollment and switching. They are not an isolated steady mode1 measurement.
- Fresh schema2 reply sequence proved mode0/rev7/N0; mode0/rev7/N1; setter ACK stating actual0/requested1/staged; then **actual mode1/rev7/N2/native pending0**.
- Mode proof expired after30 seconds when refresh replies were not obtained reliably. Final state correctly became unverified.

## K00110
Source `D0163`, original line81, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

### Coexist56: two controllers ON, no PICO connection

## K00111
Source `D0163`, original line83, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

60-second RF run; **10037 granted MCU slots**; **2 / 1433 scheduled target opportunities = 0.139567% retained-answer ratio**. **0 IMU samples**. PICO `connected=false`, mode unverified. Controller log: **71 metric windows**, **0 invalid samples**, **0 ms maximum logged unchanged-pose freeze**. All four cleanup checks true.

## K00112
Source `D0163`, original line85, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

### Coexist64: two controllers ON, bounded deadline module

## K00113
Source `D0163`, original line87, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

60-second RF run; **10042 granted slots**; **1 / 1434 = 0.069735% retained-answer ratio**. **0 IMU samples**. PICO disconnected/mode unverified. Controller log: **71 windows**, **0 invalid samples**, **0 ms maximum logged unchanged-pose freeze**. All four cleanup checks true.

## K00114
Source `D0163`, original line89, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

### Phase67: two controllers ON, strict quiet-window selection and phase skip

## K00115
Source `D0163`, original line91, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

60-second RF run; **10032 granted slots**; **2 / 1433 = 0.139567% retained-answer ratio**. Both retained payloads were short empty `010300` replies, observed at **266 us**. **0 IMU samples**, no product identity and no schema2 mode proof. Controller log: **71 windows**, **0 invalid samples**, **0 ms maximum logged unchanged-pose freeze**. All four cleanup checks true.

## K00116
Source `D0163`, original line93, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

The coexistence module does not instrument the same completed-TX-END counter as the service module. Its denominator is **nominal scheduled target opportunities**, not exact completed transmissions. Do not compare these percentages as though all runs measured identical physical RF packet loss.

## K00117
Source `D0163`, original line95, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Zero logged controller freezes is a pose-metrics observation, not proof of zero lost controller BLE packets. Healthy controllers with no PICO data are not the requested successful coexistence.

## K00118
Source `D0163`, original line97, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

## 5. Timing model and actual constraints

## K00119
Source `D0163`, original line99, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Measured/project-derived radio constraints:

## K00120
Source `D0163`, original line101, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- PICO ESB channels in this profile: **2/18/32/42/46**. Mode1 changes timing, not this channel list.
- PICO stock timing period: **4727.5 us**; mode1 timing period: **3000 us**.
- Frame's BLE schedule is folded on a **3000 us** grid. The current phase-locked loader rejects requested intervals not divisible by3000.
- One onboard radio serves BLE controllers and custom PICO traffic. Choosing a different ESB channel does not eliminate radio-time contention.
- Coexistence profile: base interval **6000 us**, seven-slot cycle → target opportunity every **42000 us**, about **23.81 Hz**. StockN8/frameN13; one target, other peers paused.
- Phase66 RF skip rejects a slot with phase correction outside **±60 us**. Minimum two tracked controllers and **150 ms** unchanged-pose guard remain enabled.

## K00121
Source `D0163`, original line108, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Static host-window model:

## K00122
Source `D0163`, original line110, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

`Stock(N) = [N*4727.5 + 3934.5, (N+1)*4727.5] us`

## K00123
Source `D0163`, original line112, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

`Frame(N) = [N*3000 + 2207, (N+1)*3000] us`

## K00124
Source `D0163`, original line114, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

For stockN8/frameN13, intersection is **[41754.5, 42000] us**. A42000 us poll lies at the Frame model's closing endpoint: **zero closing margin**. This arithmetic is proved by the host check. Its actual contribution to RF misses is **not measured**; native phase synchronization may matter.

## K00125
Source `D0163`, original line116, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

9000 us (stockN1/frameN2) and33000 us (stockN6/frameN10) also fit their respective common model windows at the Frame closing endpoint. They are calculations, not validated alternatives. A prior 3000 us/cycle1 fast experiment regressed controller tracking and must not be repeated unchanged.

## K00126
Source `D0163`, original line118, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

The earlier5982*7=41874 us “interior margin” claim was withdrawn: the MCU rephases actual slot distances to the3000 us BLE grid. Nominal arithmetic did not describe the actual schedule.

## K00127
Source `D0163`, original line120, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

## 6. RX deadline and admission findings

## K00128
Source `D0163`, original line122, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Long revision07 schema2 responses retained in Mode53 were observed by the callback at **424–446 us**. Six mode replies were in this range. The timestamps are **timer-IRQ observation times**, not precise hardware END timestamps.

## K00129
Source `D0163`, original line124, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- Original coexistence reservation600 us with margin200 ends RX at **400 us**.
- The handler checks `tick >= end` before capturing. Therefore an END observed at424–446 us is discarded by a400 us capture gate.
- A500 us RX deadline with100 us lead and100 us post-return padding requires **700 us** of quiet time, assuming early MPSL release behaves as inferred.
- Phase71 used an accidentally stale loader: measured700 us, but old admission required800 us. This deployment error was identified and preserved.
- Phase72 used the corrected loader. It measured only **600 us** at the strict **5% busy-bin cap**, required700 us, and rejected before RF/session entry.
- That rejection is not a tracker packet-loss measurement.

## K00130
Source `D0163`, original line131, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Grok's grounded hypothesis: `RADIO.SHORTS=19` includes END_DISABLE; an already completed CRC-valid frame may be retained on the existing400 us deadline's late IRQ path without deliberately extending reception. This needs an exact code-budget, timing and hardware review. It is **not implemented or hardware-validated** here. Faster IRQ sampling is another untested possibility; it cannot rescue a packet whose actual END is after the deadline.

## K00131
Source `D0163`, original line133, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

## 7. Latest research: measure phase inside Nordic

## K00132
Source `D0163`, original line135, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

### Phase73: host-side sampling uncertainty

## K00133
Source `D0163`, original line137, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Read-only diagnostic, no RAM module or radio session. **7848 observations**; both controllers valid in all **1978 native tracking observations**.

## K00134
Source `D0163`, original line139, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Existing sampler reads RTC / STATE / BASE0 / RTC through OpenOCD and assigns the midpoint. Measured before/after aperture:

## K00135
Source `D0163`, original line141, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- Minimum **1312.26 us**.
- Median **1647.95 us**.
- p90 **2319.34 us**.
- p99 **2990.72 us**.
- Maximum **4241.94 us**.

## K00136
Source `D0163`, original line147, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

This aperture is much larger than a100 us histogram bin, and asymmetric because BASE0 is read after STATE. Finer bins alone cannot make the assigned phase accurate. A historical approximately300 us sampler-placement bias exists in older research; a current calibrated correction is not established by Phase73.

## K00137
Source `D0163`, original line149, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

### Phase74: failed first native entry, diagnosed and baseline verified

## K00138
Source `D0163`, original line151, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

224-byte CPU-side sampler candidate, finite8192-iteration/64-observation cap, no radio/MPSL API calls. Initial runner omitted `disable_mpu=True` before entering execute-never manufacturing RAM. It timed out without samples. This was a runner defect, not evidence that the native sampling concept failed.

## K00139
Source `D0163`, original line153, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Read-only post-failure comparison verified all **2044 CODE bytes, 2048 DATA bytes, RTT counts and static MPU word** against the original checkpoint. Hook zero; session inactive. Initial error report remains unchanged. Exact executing script and verification are retained.

## K00140
Source `D0163`, original line155, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

### Phase75: acquisition works, cleanup tracking regression

## K00141
Source `D0163`, original line157, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Corrected pre-entry flags produced **2112 observations in33 callbacks**. RAM/static MPU/control/hook readback checks passed. Controller log nevertheless recorded **30 invalid samples per controller** in the final window and **292.97 ms** maximum unchanged pose. Acquisition completion is not a clean controller-coexistence result.

## K00142
Source `D0163`, original line159, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

The runner had an extra asynchronous region1 MPU rewrite during cleanup, unlike the established reserved-MPU loader. It was removed. That is a plausible cause of the cleanup regression; one subsequent successful run does not establish sole causality.

## K00143
Source `D0163`, original line161, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

### Phase76: successful guarded CPU-side diagnostic

## K00144
Source `D0163`, original line163, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Latest completed diagnostic:

## K00145
Source `D0163`, original line165, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- **33 callbacks; 2112 raw observations**.
- **2090 zero-LF-tick brackets accepted** for conservative offline folding; **22 other brackets excluded**.
- Maximum raw bracket **1007.08 us**, showing that IRQ preemption still occurs; accepted zero-tick brackets remain within one31 us RTC quantization interval.
- Callback wall duration: minimum **3631.59 us**, median **4547.12 us**, maximum **5432.13 us**. Do not substitute the approximately2 ms theoretical sampling span for these measured durations.
- **12 controller metric windows; 0 invalid samples; 0 ms maximum logged unchanged-pose freeze**.
- Both controllers remained valid during the additional post-cleanup check.
- RAM, static MPU word, MPU control and cleared hook verified by readback. **No RF session or flash write.**

## K00146
Source `D0163`, original line173, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Conservative offline fold excludes ambiguous brackets and treats bins with fewer than5 observations as unknown/non-quiet. A full-circle coverage requirement in the original estimator rejects these burst samples because some busy phases are undersampled. No missing bin was silently classified as quiet.

## K00147
Source `D0163`, original line175, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Whole capture:700 us quiet run at30 bins,660 us at50 bins,700 us at60 bins,581.63 us at98 bins. First and last halves separately: **600 us at30 bins**,660 us at50 bins,650 us at60 bins, and551.02/520.41 us at98 bins.

## K00148
Source `D0163`, original line177, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Therefore this is **not repeated-resolution proof of a robust700 us window**. It provides substantially better timestamps and repeated coarse600 us runs, with residual sampling/edge-count uncertainty. Admission of a500 us deadline is still unproved.

## K00149
Source `D0163`, original line179, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Exact sampler ELF SHA256: `99ca7351b511cc083efbce85a9f62c66fd65c95a223ea66ec194b8d15fec4e4f`; code224 bytes, code SHA256 `e5df028a8a2de3c5be0bd21091ece0f0964d9c61a413a7179d198657b6c7b3d6`.

## K00150
Source `D0163`, original line181, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

## 8. Code validation, limitations and next experiment

## K00151
Source `D0163`, original line183, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Latest completed combined offline suite: **106 passed** (quiet-window, loader, peer engine, remote mode44, exact phase66 ARM callback and native sampler tests). Added native runner lifecycle cases plus sampler cases: **8 passed**. These suites overlap; do not add them into a claimed114 independent tests.

## K00152
Source `D0163`, original line185, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7medium additionally identified diagnostic hardening work:

## K00153
Source `D0163`, original line187, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

1. Clear the native shell hook only after sampling/completion, so generic recovery cannot confuse an active no-MPSL callback with an idle station.
2. Use the generic checkpoint key `mpu_static_word`, not only the standalone runner's `static` key.
3. Optionally mask interrupts only around the three instantaneous RTC/STATE/RTC reads; never across the full callback. Existing Phase76 analysis instead rejects non-zero-span records.
4. Capture fault status on a timeout and ensure MPU control recovery is explicit.

## K00154
Source `D0163`, original line192, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

These points are not all implemented. **Do not apply the generic recovery tool to a retained sampler checkpoint without reviewing its key/liveness compatibility.** Phase76 itself completed/restored, so there is no active sampler awaiting such recovery.

## K00155
Source `D0163`, original line194, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Next grounded steps:

## K00156
Source `D0163`, original line196, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

- Harden native acquisition/recovery and collect a fresh phase profile with controller health verified throughout and after cleanup.
- Check stable quiet-window width and phase from bounded-aperture records; retain the5% cap,100 us lead/post padding and150 ms controller guard.
- Review a narrowly scoped late-END capture or finer-IRQ experiment at the existing deadline; preserve TX wait bounds and the2045-byte code reservation. No new OTA is needed for that host/RAM investigation.
- Obtain fresh nonce/chip/revision-matched `fe02` mode proof with both controllers active. A setter ACK is not an applied-mode proof.
- Only then measure sustained real PICO IMU/data reception and controller health together, keeping the exact denominator visible. The >=60% objective remains active.

## K00157
Source `D0163`, original line202, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

## 9. Instructions for an external reviewer

## K00158
Source `D0163`, original line204, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Read root `AGENTS.md` and the original reports/captures before proposing an experiment. The immutable research snapshot is `K:\AI\PICO-Frame-CURRENT-2026-10-08`; locate historical artifacts through its `provenance/artifact-catalog.csv` and verify their provenance through `provenance/manifest.csv`. This handoff's evidence manifest is a separate hash inventory of current live artifacts, not a replacement for snapshot provenance.

## K00159
Source `D0163`, original line206, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Review useful source paths: `pc/radio/ram/roy_ram_loader.py`, `ble_phase.py`, `peer_engine.py`, `station_v4_runtime.py`; `outputs/phase66/source/roy_ram_station_v4.c`; `outputs/phase74/source/ble_native_sampler.c`; the exact executed runner in `outputs/phase76/attempt/executed-native-probe.py`; the revision07 candidate source/patch manifest and original FW189 disassembly referenced by the project reports.

## K00160
Source `D0163`, original line208, SHA256 `91966dedf8680f6bb198445c8a9c13f7f3f66cd3e8caf6665c0d988acc62e20c`. Status: historical/unadjudicated narrative; English source prose.

Primary evidence roots:
