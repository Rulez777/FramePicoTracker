# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K03761
Source `D0497`, original line132, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

`generation (u32, odd while writing), meta (u32), payload[56]`

## K03762
Source `D0497`, original line134, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

| meta bits | Meaning |
|---|---|
| 0..7 | payload length (1..56) |
| 8..9 | S1 value to send |
| 16 | patch command 10 phase value at send time (below) |
| 17 | send only if DWT is inside `[t0, t1]` |

## K03763
Source `D0497`, original line141, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Payload words at +40: `t0, t1, dwt_ref, tracker_ticks_ref`. With bit 16 the module writes `(ticks_ref + (now - dwt_ref)/4) mod 533333` LE32 into payload bytes 3..6 and recomputes sum8 at byte 7. 64 MHz / 4 = 16 MHz = tracker timer rate.

## K03764
Source `D0497`, original line143, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

The module copies payload, then rechecks `generation`; a changed generation sends an empty poll instead.

## K03765
Source `D0497`, original line147, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

## 4. Radio settings inside a slot (Static, Hardware)

## K03766
Source `D0497`, original line149, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

| Register (RADIO 0x40001000 +) | Value |
|---|---|
| FREQUENCY 0x508 | startup 1 / 37 / 77 / 4 / 78 / 78 (cycled), working 2 |
| MODE 0x510 | 4 (BLE 2M) |
| PCNF0 0x514 | 0x00040008 (LEN 8 bit, S1 4 bit) |
| PCNF1 0x518 | 0x01040096 (MAXLEN 150, BALEN 4, big endian) |
| BASE0 0x51C / PREFIX0 0x524 | pairing 0x552C6A1E / 0xC0, working per peer |
| TXADDRESS 0x52C / RXADDRESSES 0x530 | 0 / 1 |
| CRCCNF 0x534 / POLY 0x538 / INIT 0x53C | 2 / 0x11021 / 0xFFFF |
| DATAWHITEIV 0x554 | 0 (whitening off) |
| **MODECNF0 0x650** | **0x201, always** |
| SHORTS 0x200 | 11 for TX (READY_START, END_DISABLE), 19 for RX |

## K03767
Source `D0497`, original line162, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

**MODECNF0:** stock Roy sets 0x201 (fast ramp up, DTX center) once at boot and the BLE stack never rewrites it. Writing 0x200 or 0x000 left both controllers unstable until a Roy reboot (Hardware).

## K03768
Source `D0497`, original line164, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Startup slot sends a 9 B discovery (`C1 ...`) on the pairing address for search steps 0..2 and a 21 B connect request on the pairing or saved address for the rest. Connect request body as in the other doc: `04 00 7A 00 00 00 55 01 00 00 00 [station id 4] [slot] [tracker id 4]` (frequency mask `01 00 00 00 00` = 2402 MHz only).

## K03769
Source `D0497`, original line166, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

The module sends the confirm (command 9) itself on the peer's very next own slot after the offer, without a host round trip, because the tracker drops an unconfirmed link after ~23 ms.

## K03770
Source `D0497`, original line168, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

RX inside a slot: TIMER0 CC0 ticks every 100 us; each tick checks END, captures, restarts RX until `D(1892)`. The slot end is fixed. Extending RX while a reply was in flight (+150 us) froze controllers (Hardware).

## K03771
Source `D0497`, original line172, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

## 5. Coexistence with the Valve controllers (Hardware)

## K03772
Source `D0497`, original line174, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

### 5.1 BLE schedule

## K03773
Source `D0497`, original line176, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

- Two BLE 2M connections, one per controller, on an exact **3000.0 us** grid in the RTC/LFCLK domain.
- Measured by sampling `RTC1.COUNTER (0x40011504)` and `RADIO.STATE (0x40001550)` in one TCL command, folded modulo 3000 us, 30 bins, at least 600 samples.
- Controller A on air ~500..1200 us, B ~2200..3100 us; quiet ~1300..2200 us of the period. Exact phases differ per boot; measure every start.
- 3000 us = exactly 12288 units of 1/(32768 x 125) s, so the RTC1 tick phase can be computed in integer math.

## K03774
Source `D0497`, original line181, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

### 5.2 Our slots

## K03775
Source `D0497`, original line183, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

| Item | Value |
|---|---|
| Priority | HIGH (0) |
| Slot length | 600 us; RX ends 200 us before slot end |
| Distance | 6000 us (two BLE periods), type 1 (EARLIEST is only used for the first) |
| Cycle | 7 slots; slot k serves peer `(slots-1) mod 7`, 0..2 polls, 3..6 idle. A stage 2 peer takes the next slot |
| Per tracker poll period | 42 000 us (42000 mod 4727.5 = 4180 us, inside the tracker RX window) |

## K03776
Source `D0497`, original line191, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Phase lock, every slot, in the MCU:

## K03777
Source `D0497`, original line200, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Plus a host side correction from the measured radio fold. A 100 us lead is applied to the target.

## K03778
Source `D0497`, original line202, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Things that froze controller poses for 200..400 ms (Hardware): slot overlapping a BLE event, any RX extension, MODECNF0 change, and tracker autonomous IMU transmissions (5.3).

## K03779
Source `D0497`, original line204, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

### 5.3 Tracker autonomous transmissions and command 16 (Hardware, Static FW 189)

## K03780
Source `D0497`, original line206, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

After command 19 the tracker transmits IMU on its own if no poll arrives within ~4727.5 us. With our 42 ms poll period this happened at random times and hit controller events.

## K03781
Source `D0497`, original line208, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Command 16 body `[N, 2]` sets poll decimation. With decimation N the tracker reopens RX in `[N x 4727.5 + 3538, (N+1) x 4727.5]` us after a poll and does not transmit by itself in between (FW 189 code at 0x6100 / 0x6CAC).

## K03782
Source `D0497`, original line214, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Formula gives 8, but **N = 8 made trackers go silent and reconnect in a loop; N = 9 works** (Hardware). Send command 16 before command 19.

## K03783
Source `D0497`, original line216, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

### 5.4 Guard

## K03784
Source `D0497`, original line218, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

The host watches OpenVR poses of HMD and both controllers. RF stops and Roy is restored if a controller is invalid for 0.5 s or its pose stays bit-identical for more than 150 ms. Result of the above: long runs with three trackers connected and and zero controller freezes.

## K03785
Source `D0497`, original line222, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

## 6. Tracker behaviour learned on FW 189 (Hardware unless noted)

## K03786
Source `D0497`, original line224, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

| Topic | Finding |
|---|---|
| Reply S1 | Replies **always** carry S1 = 3, whatever we sent. S1 echo cannot be used for ACK matching. We match ACKs by our own mailbox generation tag (`D(1648)`) |
| Command 9 | Complete when command 1 / 0x82 product info arrives; resend only if it does not |
| Configuration order | 9 confirm, then 16 `[9,2]`, 17 `[07]`, 25 `[02]`, 19 `[00]`, then 10 |
| Command 25 | LED gate at FW 189 0xC5D4. `[02]` lets the tracker fire IR pulses on camera sync. `[01]` suppresses them. `[00]` depends on state. With `[01]` the Frame cameras saw no tracker LEDs at all |
| Command 10 | Tracker timer equals sent value + 467 ticks at RADIO ADDRESS (16 MHz ticks, wraps 533 333) |
| LED pulse | At tracker tick ~268 727 of 533 333 (33.33 ms frame). Our camera aligned value is 270 000 sent through command 10 |
| Command 14 | `[01, N, reason]` as in the other doc; N = 3 for three trackers (each tracker lights every third frame) |
| Reply timeout | 3 s without valid reply returns a peer to search (shorter values caused reconnect storms during camera load) |
| Reconnect | All configuration is lost on disconnect; the module re-offers, host redoes 9 to 10 |

## K03787
Source `D0497`, original line238, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

## 7. Steam Frame cameras (Hardware)

## K03788
Source `D0497`, original line240, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

| Item | Value |
|---|---|
| IR streams | 4 x 1056 x 1024, plus 2 color |
| Clock domain | CLOCK_MONOTONIC_RAW, needs a per boot measured offset to MONOTONIC |
| Frame | 33.33 ms; tracker LED exposure is short (< 1 ms) |
| Roy clock to host | DWT 64 MHz fitted with fixed slope 15.625 ns per cycle, offset from 40 SWD samples, uncertainty RMS / sqrt(n) |

## K03789
Source `D0497`, original line247, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Pose from LEDs: blobs clustered by single linkage at 60 px, P3P on 5 or more inliers with 2.5 px max residual, factory LED model per tracker. 67 % of lit frames associate offline at the moment.

## K03790
Source `D0497`, original line251, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

## 8. Stop and restore

## K03791
Source `D0497`, original line253, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

1. Set `stop`, wait for MPSL signal 9 (session closed) or 8/10.
2. Call entry with `'2'` (session close), check `D(1600) == 0x52455431`.
3. Clear the shell hook, restore RTT counts `3,3`, CODE, DATA, MPU static word, DWT_CTRL, DEMCR from the recovery file. Verify by readback.
4. A Roy reboot (headset restart) clears everything anyway. SteamVR restart does not affect Roy.

## K03792
Source `D0497`, original line260, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

## 9. Still unknown

## K03793
Source `D0497`, original line262, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

- Why decimation N = 8 fails while the formula predicts it is correct
- Exact meaning of command 16 byte 1 (we always send 2)
- Whether more than 3 trackers fit in the 7 slot cycle without controller impact (PICO stations hold 5 devices, so likely yes with tighter slots)
- LED drift rate against the Frame cameras between command 10 resends

## K03794
Source `D0498`, original line372, SHA256 `3cb846f9d77424aad9e8273ba01281b264317522fbafcec7fbe84aa414c9b450`. Status: historical/unadjudicated narrative; English source prose.

## PHY/preamble review and unavailable Frame connection, 2026-10-06

## K03795
Source `D0498`, original line374, SHA256 `3cb846f9d77424aad9e8273ba01281b264317522fbafcec7fbe84aa414c9b450`. Status: historical/unadjudicated narrative; English source prose.

Swift VA5218 loads config byte+8 and521A writes it directly toRADIO.MODE+510; no enum translation. VA50EC writes PCNF0=40008 (8bit preamble). VA6188 separately writesMODE4. These instructions are verified but their complete runtime ordering/live applicability not newly measured. Nordic nRF52833 primary spec requires PLEN16 for Ble_2Mbit; Nrf_2Mbit uses8bits: https://docs.nordicsemi.com/r/bundle/ps_nrf52833/page/radio.html. Found static combination is nonstandard per specification; do not resolve by silently changing researched firmware claims.

## K03796
Source `D0498`, original line376, SHA256 `3cb846f9d77424aad9e8273ba01281b264317522fbafcec7fbe84aa414c9b450`. Status: historical/unadjudicated narrative; English source prose.

Prepared narrowly whitelisted raw comparisons: existingmode4/preamble8, alternativemode4/preamble16 andmode1/preamble8. Same three conditional tails, noCRC/noWH, evenchannels2..80. Harness --swift-phy4p8/4p16/1p8;61tests pass including rejection of unreviewed mode/preamble combinations. No positive2M reference obtained; this compares alternatives, not calibrates reception.

## K03797
Source `D0498`, original line378, SHA256 `3cb846f9d77424aad9e8273ba01281b264317522fbafcec7fbe84aa414c9b450`. Status: historical/unadjudicated narrative; English source prose.

Attempted upload and run76 failed because SSH[DEVICE_IP] unreachable (explicit5sec connection timeout reproduced). ADB listsPICOonly; PICOAwake. Upload failed, SSH run failed before starting remote command; no76RFexperiment occurred and no76artifact downloaded. Stopped only the owned hanging sftp download transport after checking PID/commandline; no device processes stopped. Local changes preserved; Frame may still have older loader/harness. No new SRAM/MPU/RADIO or flash changes. User asked for active Frame/IP or USB to restore access. Resume by verifyingconnection,uploading files with bounded SSH timeout, then compareprofiles; do not label76 as executed.

## K03798
Source `D0501`, original line1, SHA256 `ca1dc9ba72e7951b0340871ee226e8be66119816cc7b523dceb17f6c07fd427a`. Status: historical/unadjudicated narrative; English source prose.

# Single-session station handover draft225

## K03799
Source `D0501`, original line3, SHA256 `ca1dc9ba72e7951b0340871ee226e8be66119816cc7b523dceb17f6c07fd427a`. Status: historical/unadjudicated narrative; English source prose.

ROY_SWIFT_STATION_HANDOVER implements first12bootstrapoffers then passiveworkingRX BASE[PRIVATE_LINK_VALUE]/prefix40 rotating2/6/8/10/12 withoutloadinggap. FastRX shortcut retainedonlyinTXstage;passiveRXEN resumesafter12. SeparateROYSTATIONNEXT1tag and --station-handover/--station-offer flags required; explicitidentity/no-op lifecycle remainsrequired, redundanttagselidedtofitRAM.

## K03800
Source `D0501`, original line5, SHA256 `ca1dc9ba72e7951b0340871ee226e8be66119816cc7b523dceb17f6c07fd427a`. Status: historical/unadjudicated narrative; English source prose.

Firstcompilebeforetagrename2044B fits2045 by1byte; finaltaggedbuild mustbe recompiledandcheckedbeforeupload. No deployment225. Loader tests pass. TX diagnostics maximum andreceivephase attribution stillneedupdatebeforehardware; do not claim actualhandover validated. Existing223/224negativecapturesunprovenacceptance.

## K03801
Source `D0502`, original line1, SHA256 `8b27d9c927ed8e3a1929d780d67c6c2532ac5bc53729a81eb82a67a7d1a9b6de`. Status: historical/unadjudicated narrative; English source prose.

# Single-session handover226 hardware result

## K03802
Source `D0502`, original line3, SHA256 `8b27d9c927ed8e3a1929d780d67c6c2532ac5bc53729a81eb82a67a7d1a9b6de`. Status: historical/unadjudicated narrative; English source prose.

FinalROYSTATIONNEXT1module2043B fits2045.25loader/probetests pass; missing--station-handover/offer combinationrejectsbeforehardware. Initial wrapper generation accidentally substituted--station-offer; loader rejected before creatingreport. Correctedwrapper thenranonehardwaretest.

## K03803
Source `D0502`, original line5, SHA256 `8b27d9c927ed8e3a1929d780d67c6c2532ac5bc53729a81eb82a67a7d1a9b6de`. Status: historical/unadjudicated narrative; English source prose.

12targetedbootstrapoffers,12TXEND,12fastRXhandovers;323sampledslots total, thereafterpassiveBASE[PRIVATE_LINK_VALUE]/prefix40 over2/6/8/10/12.0ADDRESS/END/CRCframes. No hostretry in correctedrun. Cleanupsession/RAM/MPUalltrue. OnlyHMD74pose rows. NoIRcommand; no confirmedpairing or6DoF. Rawoutputs/station-handover226.json. Awake tracker state at exacttimeunknown. Do notclaimnegativeproveswrongaddress or failednegotiation. Nextconfirmawake and inspect actualstationoffer fields/ACK semantics ratherthan unchangedrepeat.

## K03804
Source `D0503`, original line1, SHA256 `e43e0f705bed14a45edb730892ed68f646e57274d8f9a9a907fbc96dd46e8c4d`. Status: historical/unadjudicated narrative; English source prose.

# First targeted station offer223

## K03805
Source `D0503`, original line3, SHA256 `e43e0f705bed14a45edb730892ed68f646e57274d8f9a9a907fbc96dd46e8c4d`. Status: historical/unadjudicated narrative; English source prose.

User permitted ordinary tracker pairing-record writes221; Frameflash and firmware images unchanged. ROYSTATIONOFFER1/explicit --station-offer transmits21-byte candidate only to observed FICR[TRACKER_ID] on bootstrap1/37/77, max40. Parameters reviewed PhoenixBD20/Swift68F0:040000000000553D000000007473541540AD1DA201. Mask selects2/6/8/10/12, prefix40, historicalseed74735415. No IR command in this test. Identity/fast/native/MPU lifecycle flags required; mismatchingtag/flag rejectsbeforehardware. Receiver remainsbootstrap forresponse; no autonomousworking-address transition yet.

## K03806
Source `D0503`, original line5, SHA256 `e43e0f705bed14a45edb730892ed68f646e57274d8f9a9a907fbc96dd46e8c4d`. Status: historical/unadjudicated narrative; English source prose.

ARMmodule2009Bfits2045.25loader/probetests and13framingtests pass. Hardware223:13sampledslots,0records,13hostretries,onlyHMD134pose rows. Cleanupsession/RAM/MPUalltrue. Rawstation-offer223.json contains actualTX andhandover counts; no commanddelivery/paircompletionclaim. Tracker awake at exacttesttime unverified, lastuserreportedindicatoroffagain. Needboundedfollowing receiver onworkingBASE[PRIVATE_LINK_VALUE]/prefix40 afteroffer and targeted IR completion. Do not conflate lack of bootstrap response with failure to switchaddress.

## K03807
Source `D0504`, original line1, SHA256 `e82c323c870c53b98a5dc07ab0aefe227332191211f6ab5f0a392c21ed86b45f`. Status: historical/unadjudicated narrative; English source prose.

# Storage reachability audit214

## K03808
Source `D0504`, original line3, SHA256 `e82c323c870c53b98a5dc07ab0aefe227332191211f6ab5f0a392c21ed86b45f`. Status: historical/unadjudicated narrative; English source prose.

Frame SSH and diagnostic agent verified live; /status reports no active capture. No loader or OpenOCD process appeared in the process check. Another completed test_slot0 capture existed remotely and was copied to outputs/external-slot0-214.json; it was not launched by this investigation and does not establish a tracker connection. Do not run concurrent radio loaders.

## K03809
Source `D0504`, original line5, SHA256 `e82c323c870c53b98a5dc07ab0aefe227332191211f6ab5f0a392c21ed86b45f`. Status: historical/unadjudicated narrative; English source prose.

Swift storage context references were enumerated from every halfword boundary (candidate scan, not a proof that every decoded hit is executable). Explicit code ranges confirm initialization8C8C sets method+20=9DDC (save paired record),+24=9E08 (invalidate record),+2C=9E64 (storage rollover),+28=9E24 (load journal). 9E24 walks eight-byte entries and copies the last AAAA record into RAM; no store backend call in that method. StartupBC8C invokes load then rollover with boundary267FF. Rollover9E64 invokes erase method+18 when cursor>=boundary and re-saves AAAA record via+20. This adds a storage write path even with a paired marker, but its observed caller is startup, not proof that a negotiation reply invokes rollover.

## K03810
Source `D0504`, original line7, SHA256 `e82c323c870c53b98a5dc07ab0aefe227332191211f6ab5f0a392c21ed86b45f`. Status: historical/unadjudicated narrative; English source prose.

Known radio scheduler6CF6 save path still requires pending E49, set by unpaired transition605C. Existing paired reconnect may avoid that path; complete reachability and current physical bond state remain unverified. No TX or tracker configuration mutation214. Do not confuse startup housekeeping with a flash write caused by every reconnect.

## K03811
Source `D0504`, original line9, SHA256 `e82c323c870c53b98a5dc07ab0aefe227332191211f6ab5f0a392c21ed86b45f`. Status: historical/unadjudicated narrative; English source prose.

Evidence: swift-storage-refs214.txt, swift-storage-callers214.txt, swift-storage-load214.txt under outputs/firmware-standalone. Standalone6DoF remains incomplete.

## K03812
Source `D0510`, original line1, SHA256 `d27d3d153672b9ba96aa1c439353843d0b9385cd6189d5113b98e26438e4e0f7`. Status: historical/unadjudicated narrative; English source prose.

# Command 16 & RX Window Analysis (FW 189)

## K03813
Source `D0510`, original line3, SHA256 `d27d3d153672b9ba96aa1c439353843d0b9385cd6189d5113b98e26438e4e0f7`. Status: historical/unadjudicated narrative; English source prose.

## 1. Command 16 Handler & Fields
* **Dispatch Table:** The command handler dispatch table is a 16-byte per entry array in RAM at `0x20001868`. It is dynamically initialized at `0x90A8`. Command 16 is at offset `16 * 16 = 0x100` (`0x20001968`).
* **Handler Address:** The function pointer for command 16 is stored at `0x20001868 + 0x10C`, and points to `0x00008F0C` (thumb `0x8F0D`).
* **Storage:** The handler at `0x8F0C` extracts `N` (body[0]) and `M` (body[1]) and calls `0x6100`. 
  `0x6100` stores these fields in RAM:
  * `N` is stored at `0x20000F21 + 3` (`0x20000F24`)
  * `M` is stored at `0x20000F21 + 2` (`0x20000F23`)
  * A `flag = 1` is stored at `0x20000F21 + 1` (`0x20000F22`)

## K03814
Source `D0510`, original line12, SHA256 `d27d3d153672b9ba96aa1c439353843d0b9385cd6189d5113b98e26438e4e0f7`. Status: historical/unadjudicated narrative; English source prose.

## 2. RX/TX Scheduling Timer
* **Timer Use:** The values are read by `0x61F4` -> `0x6214`. The hardware timer used is `TIMER0` (`0x40008000`) with a prescaler of 4 (16 MHz / 16 = 1 MHz, so 1 tick = 1 µs).
* **N is NOT a multiplier:** The firmware reads the current effective `N` (from offset `0x20000F21`). 
  * If `N == 0`, it sets `TIMER0 CC[0] = 4727` (`0x1277`).
  * If `N != 0`, it sets `TIMER0 CC[0] = 18910` (`0x49DE`).
  * *Important:* `18910` is exactly $4 \times 4727.5$. The firmware simply hardcodes a 4x slowdown if `N` is non-zero, rather than using `N` as a literal multiplier for the window!
* **Anchoring:** The schedule is **anchored to the last received packet**. Upon packet processing, `TIMER0`'s `TASKS_CLEAR` and `TASKS_START` are triggered (at `0x623E`), resetting the timer to 0. It is NOT free-running independently, nor is it anchored to the 533333-tick frame.

## K03815
Source `D0510`, original line20, SHA256 `d27d3d153672b9ba96aa1c439353843d0b9385cd6189d5113b98e26438e4e0f7`. Status: historical/unadjudicated narrative; English source prose.

## 3. RX Window Length
Because `TIMER0 CC[0]` is explicitly set to `4727` or `18910`, the RX interval simply becomes precisely 4727 µs or 18910 µs. The timeout handler fires at this exact time to re-evaluate state. The actual hardware RX window is only opened shortly before this timeout fires (to catch the packet). N=8 does NOT give an 8x window (which would be ~37.8ms); it simply enables the `18910` µs interval.

## K03816
Source `D0510`, original line23, SHA256 `d27d3d153672b9ba96aa1c439353843d0b9385cd6189d5113b98e26438e4e0f7`. Status: historical/unadjudicated narrative; English source prose.

## 4. What M=2 means
* `M` is a **down-counter for missed polls**.
* In the timeout handler (`0x6DF6`), if the command 16 flag is 1, it checks `M`.
* If `M > 0`, it decrements `M` and continues using the fast `4727` µs cycle.
* When `M` reaches `0`, it clears the flag, copies `N` into the active slot (`0x20000F21`), and from then on, if `N != 0`, it uses the `18910` µs cycle.
* **Meaning:** `M=2` tells the tracker: "If you miss a poll, listen again at the normal 4.7ms cadence for 2 more cycles. If you still hear nothing, switch to the slower N-based cadence (18.9ms)."

## K03817
Source `D0510`, original line30, SHA256 `d27d3d153672b9ba96aa1c439353843d0b9385cd6189d5113b98e26438e4e0f7`. Status: historical/unadjudicated narrative; English source prose.

## 5. Autonomous IMU TX
* **Condition:** Autonomous transmission is triggered by the timeout handler when no packet is received (`0x20000EF4 == 0`).
* **Does Command 16 disable it?:** **No.** Command 16 does not disable the autonomous TX. However, because Command 16 (once `M` expires) changes the timeout interval from 4727 µs to 18910 µs, the timeout handler runs 4 times less frequently. As a result, the autonomous TX occurs every 18.9 ms instead of every 4.7 ms.

## K03818
Source `D0510`, original line34, SHA256 `d27d3d153672b9ba96aa1c439353843d0b9385cd6189d5113b98e26438e4e0f7`. Status: historical/unadjudicated narrative; English source prose.

## Concrete Recommendation
Because the tracker's firmware hardcodes the "slow" interval to exactly `18910` µs (regardless of the specific non-zero value of N), polling at intervals like 36000, 42000, 48000, or 54000 µs **will mathematically never cleanly align** with the tracker's wake windows once it falls into the slow `18910` µs mode.

## K03819
Source `D0510`, original line37, SHA256 `d27d3d153672b9ba96aa1c439353843d0b9385cd6189d5113b98e26438e4e0f7`. Status: historical/unadjudicated narrative; English source prose.

This is why `N=8` giving 10-30% replies is expected: the tracker is sleeping for 18.9 ms repeatedly, completely unaware of your 42 ms grid, and only replies when the two drifting schedules happen to phase-align by chance.

## K03820
Source `D0510`, original line39, SHA256 `d27d3d153672b9ba96aa1c439353843d0b9385cd6189d5113b98e26438e4e0f7`. Status: historical/unadjudicated narrative; English source prose.

**To fix this and get 100% replies with your station polling every `P` µs:**
1. **Force the tracker to never enter the slow mode:** Set **`N=0`** and **`M=255`** (or a similarly high value). 
2. This will force the tracker to continually wake up every `4727` µs to listen.
3. Since it wakes up every `4727` µs and resets its timer upon receiving your packet, it will happily catch your poll at 36000, 42000, 48000, or 54000 µs (with ± ~400µs jitter tolerance), because it is always listening at 4.7ms increments.
4. The tracker's disconnect timeout is calculated as `200 / (N+1)` missed cycles. If `N=0`, the timeout is `200` cycles of `4727` µs = `945 ms`. Since your maximum polling interval is `54 ms`, this is perfectly safe and the tracker will never drop the connection.

## K03821
Source `D0510`, original line45, SHA256 `d27d3d153672b9ba96aa1c439353843d0b9385cd6189d5113b98e26438e4e0f7`. Status: historical/unadjudicated narrative; English source prose.

 
 # #   C o r r e c t i o n   o n   P o l l i n g   I n t e r v a l s 
 B e c a u s e   N   d o e s   n o t   s t r e t c h   t h e   R X   w i n d o w   b u t   m e r e l y   s e l e c t s   b e t w e e n   a   4 7 2 7   � s   a n d   1 8 9 1 0   � s   p e r i o d ,   t h e   r e c e i v e r   w i l l   O N L Y   b e   o n   d u r i n g   t h e   ~ 1 . 2 m s   w i n d o w s   s u r r o u n d i n g   m u l t i p l e s   o f   4 7 2 7   � s .   S p e c i f i c a l l y ,   t h e   R X   i s   o n   r o u g h l y   [ k   *   4 7 2 7   -   7 3 7 ,   k   *   4 7 2 7   +   4 6 0 ]   � s   a f t e r   t h e   l a s t   p a c k e t . 
 
 T h e r e f o r e ,   y o u r   p r o p o s e d   p o l l i n g   i n t e r v a l s : 
 -   * * 4 2 0 0 0   � s * * :   W o r k s   p e r f e c t l y .   I t   f a l l s   i n s i d e   t h e   = 9 $   w i n d o w   ( [ 4 1 8 0 6 ,   4 3 0 0 3 ] ) .   J i t t e r   t o l e r a n c e :   ~ 1 9 4   � s   e a r l y ,   ~ 1 0 0 0   � s   l a t e . 
 -   * * 3 6 0 0 0   � s * * :   W i l l   * * F A I L * * .   I t   f a l l s   i n   t h e   d e a d   z o n e   b e t w e e n   = 7 $   ( [ 3 2 3 5 2 ,   3 3 5 4 9 ] )   a n d   = 8 $   ( [ 3 7 0 7 9 ,   3 8 2 7 6 ] ) .   Y o u   m u s t   u s e   ~ 3 8 0 0 0   � s   i n s t e a d . 
 -   * * 4 8 0 0 0   � s * * :   W i l l   * * F A I L * * .   F a l l s   i n   t h e   d e a d   z o n e   b e t w e e n   = 1 0 $   ( [ 4 6 5 3 3 ,   4 7 7 3 0 ] )   a n d   = 1 1 $   ( [ 5 1 2 6 0 ,   5 2 4 5 7 ] ) .   U s e   ~ 4 7 0 0 0   � s   i n s t e a d . 
 -   * * 5 4 0 0 0   � s * * :   W i l l   * * F A I L * * .   D e a d   z o n e   b e t w e e n   = 1 1 $   a n d   = 1 2 $ .   U s e   ~ 5 2 0 0 0   � s   i n s t e a d . 
 
 * * R e c o m m e n d a t i o n : * *   U s e   [ N = 0 ,   M = 2 ]   ( o r   M = 2 5 5 )   a n d   s t r i c t l y   a l i g n   y o u r   p o l l i n g   i n t e r v a l   P   t o   l a n d   i n s i d e   o n e   o f   t h e s e   k   *   4 7 2 7   w i n d o w s   ( e . g . ,   e x a c t l y   4 2 0 0 0 ,   o r   3 7 8 1 6 ,   4 7 2 7 0 ) . 
 
 

## K03822
Source `D0511`, original line1, SHA256 `2722d677ebb9f384abb915b62c6cfc8465e80a350e8bb68b1647115b6c096ac4`. Status: historical/unadjudicated narrative; English source prose.

Offline review snapshot; run tests in the full project checkout with its dependencies. Hardware/OTA not approved. Final image and report are under their original relative paths.

## K03823
Source `D0514`, original line1, SHA256 `52022a1064c1fcd84b88ff8fe542f66bfe1bc3d7c95ec229fd724b26053db958`. Status: historical/unadjudicated narrative; English source prose.

# FW189 GRID3000 RC1 — iteration02

## K03824
Source `D0514`, original line3, SHA256 `52022a1064c1fcd84b88ff8fe542f66bfe1bc3d7c95ec229fd724b26053db958`. Status: historical/unadjudicated narrative; English source prose.

Review snapshot, not a hardware-approved release. Firmware stays at the original
30-byte candidate; host preparation and SRAM deadline handling were revised.
145 offline tests passed. No device was touched.

## K03825
Source `D0514`, original line7, SHA256 `52022a1064c1fcd84b88ff8fe542f66bfe1bc3d7c95ec229fd724b26053db958`. Status: historical/unadjudicated narrative; English source prose.

Start with docs/FW189_ITERATION_02_2026-10-07.md and release-status.json.
Reproduce tests in the original project with its .venv and full dependencies;
this is a source/evidence snapshot, not a standalone Python installation.
No private registry, seed/config, OTA job or automatic launcher is included.
The native queued-ACK case is mitigated; pending old OTA work is still a
counterexample. Fresh application startup clears state in the bounded model,
but bootloader/recovery and real timing/controller coexistence are unverified.
USER HARDWARE PAUSE REMAINS IN FORCE.

## K03826
Source `D0522`, original line1, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

<img src="https://raw.githubusercontent.com/skylot/jadx/master/jadx-gui/src/main/resources/logos/jadx-logo.png" width="64" align="left" />

## K03827
Source `D0522`, original line3, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

## JADX

## K03828
Source `D0522`, original line5, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

[Excluded original image: Build status]
[Excluded original image: GitHub contributors]
[Excluded original image: GitHub all releases]
[Excluded original image: GitHub release (latest by SemVer)]
[Excluded original image: Latest release]
[[Excluded original image: Maven Central]](https://search.maven.org/search?q=g:io.github.skylot%20AND%20jadx)
[Excluded original image: Java 11+]
[[Excluded original image: License]](http://www.apache.org/licenses/LICENSE-2.0.html)

## K03829
Source `D0522`, original line14, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

**jadx** - Dex to Java decompiler

## K03830
Source `D0522`, original line16, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

Command line and GUI tools for producing Java source code from Android Dex and Apk files

## K03831
Source `D0522`, original line18, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

> [!WARNING]
> Please note that in most cases **jadx** can't decompile all 100% of the code, so errors will occur.<br />
> Check Troubleshooting guide (original reference: `https://github.com/skylot/jadx/wiki/Troubleshooting-Q&A#decompilation-issues`) for workarounds.

## K03832
Source `D0522`, original line22, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

**Main features:**
- decompile Dalvik bytecode to Java code from APK, dex, aar, aab and zip files
- decode `AndroidManifest.xml` and other resources from `resources.arsc`
- deobfuscator included

## K03833
Source `D0522`, original line27, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

**jadx-gui features:**
- view decompiled code with highlighted syntax
- jump to declaration
- find usage
- full text search
- smali debugger, check wiki page (original reference: `https://github.com/skylot/jadx/wiki/Smali-debugger`) for setup and usage

## K03834
Source `D0522`, original line34, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

Jadx-gui key bindings can be found here (original reference: `https://github.com/skylot/jadx/wiki/JADX-GUI-Key-bindings`)

## K03835
Source `D0522`, original line36, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

See these features in action here: jadx-gui features overview (original reference: `https://github.com/skylot/jadx/wiki/jadx-gui-features-overview`)

## K03836
Source `D0522`, original line38, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

<img src="https://user-images.githubusercontent.com/118523/142730720-839f017e-38db-423e-b53f-39f5f0a0316f.png" width="700"/>

## K03837
Source `D0522`, original line40, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

### Download
- release
  from [github: [Excluded original image: Latest release]](https://github.com/skylot/jadx/releases/latest)
- latest [unstable build [Excluded original image: GitHub commits since tagged version (branch)]](https://nightly.link/skylot/jadx/workflows/build-artifacts/master)

## K03838
Source `D0522`, original line45, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

After download unpack zip file go to `bin` directory and run:
- `jadx` - command line version
- `jadx-gui` - UI version

## K03839
Source `D0522`, original line49, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

On Windows run `.bat` files with double-click\
**Note:** ensure you have installed Java 11 or later 64-bit version.
For Windows, you can download it from oracle.com (original reference: `https://www.oracle.com/java/technologies/downloads/#jdk17-windows`) (select x64 Installer).

## K03840
Source `D0522`, original line53, SHA256 `3cea1d7e9ff4f7c041c3d802be0d237f9931fba823b9a608bd0f74c6d3ec5ae8`. Status: historical/unadjudicated narrative; English source prose.

### Install
- Arch Linux
  [[Excluded original image: Arch Linux package]](https://archlinux.org/packages/extra/any/jadx/)
  [[Excluded original image: AUR Version]](https://aur.archlinux.org/packages/jadx-git)
