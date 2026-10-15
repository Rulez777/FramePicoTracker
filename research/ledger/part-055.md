# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K04321
Source `D0752`, original line103, SHA256 `697f1481026cb3eab5b2d5f92401f6fc4862df8eb252e8f439dcd17ab573ae33`. Status: historical/unadjudicated narrative; English source prose.

## Emulation tests that would show the bug

## K04322
Source `D0752`, original line105, SHA256 `697f1481026cb3eab5b2d5f92401f6fc4862df8eb252e8f439dcd17ab573ae33`. Status: historical/unadjudicated narrative; English source prose.

These are host-side models, not another RF run.

## K04323
Source `D0752`, original line107, SHA256 `697f1481026cb3eab5b2d5f92401f6fc4862df8eb252e8f439dcd17ab573ae33`. Status: historical/unadjudicated narrative; English source prose.

1. **Stuck END.** Signal 0, `EVENTS_END` held 0, virtual TIMER0 at 1 µs/tick, granted length 900, `D(1892)=800`. Fail if the callback is still inside START at 900 µs, if COMPARE0 is enabled before the wait ends, or if a nested signal 1 runs before START returns. The current loop fails this as soon as the model charges a few cycles per peripheral read.
2. **Overstay precondition.** Session state stays 4 until START returns, then becomes 5. Inject internal event 8 at 900 µs. Expect the test to fail on duration. Do not model “assert skipped” as success.
3. **Single request.** Signal 5 sets `phase=3`, calls `0x18949` once, then `phase=1`, returned action 0. A concurrent operation 1 while `phase==3` must be refused. Today’s loader does the opposite: `phase==3` is the retry trigger. Assert `host_retries` stays 0 for a service-mode profile.
4. **Return shape.** Existing `outputs/service-fast-poll20/test_callback.py` already checks NONE plus one EARLIEST call for signals 5–7, and that the old module returns action 3 on signal 5. Extend it so operation 1 and the callback cannot both be in `0x18949`, and so `stop==1` skips both.
5. **Denominator.** A slot that aborts at `now>=800` without `EVENTS_END` increments START and does not increment a TX-completed counter. The 11.6% figure must not be reported as answers per completed poll until that counter exists. Command 10’s body rewrite must not change `interval_us` or CC0; that belongs next to `test_native_phase.py`, and it explains why 11.589% and 11.612% match.

## K04324
Source `D0752`, original line113, SHA256 `697f1481026cb3eab5b2d5f92401f6fc4862df8eb252e8f439dcd17ab573ae33`. Status: historical/unadjudicated narrative; English source prose.

The 60% target needs a later measurement of completed TX versus tracker listen windows (N actually applied, channel, unanswered slots). That is outside this assert. The evidence in hand says the crash is an overstay while the START callback is still active, with a separate and already-demonstrated host re-entry into `0x18949`.

## K04325
Source `D0753`, original line1, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

The 11–12% RX/TX ratio is a phase and turnaround problem. Requested N=0 and command 10 already failed to move it. The useful change is to keep S1 bit1 set, poll at a 4727.5 µs average, start RX with the hardware shortcut, and treat NORMAL `-35` in session state 1 as a one-shot earliest re-anchor.

## K04326
Source `D0753`, original line3, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

## What the measurements already fix

## K04327
Source `D0753`, original line5, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

Iteration 20, radio-only, one stock FW189 tracker, controllers off: about 126 slots/s and 11.4–11.7% RX per completed TX. Explicit command 16 `[N=0, M=2]` stayed at 11.72% (5035 TX-END / 590 RX / 40 s). Captured reply callbacks sat 226–759 µs after slot start, median 447 µs, inside the 800 µs deadline. Gaps of about 4.7 ms and 42.5 ms were real, and a minute of fast polling produced 5998 low-priority blocked/cancelled/idle signals. Iteration 21: adjacent answered software TXEN intervals median 4726.69 µs; some index-gap-7 pairs were 42.5 ms instead of 7×4727 µs. Iteration 23: command 10 did not move the ratio (11.589% then 11.612%). Those are answered-slot and counter facts, not a full START grid.

## K04328
Source `D0753`, original line7, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

## Tracker period and who is listening

## K04329
Source `D0753`, original line9, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

`outputs/swift189-full.asm`, image `outputs/swift189-image.bin` (manifest SHA `477000fc…`):

## K04330
Source `D0753`, original line11, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

- `0x6184–0x6194` writes TIMER0 MODE=0, BITMODE=3 at `+0x508`, PRESCALER=4 at `+0x510`. At 16 MHz that is a 1 µs tick. Iteration 22’s “PRESCALER=3, so 18910 ticks = 9455 µs” does not match this init. `docs/FW189_GRID3000_FINAL_VERDICT_2026-10-07.md` reached the same register conclusion; a later writer of PRESCALER is not ruled out.
- `0x61f4` and `0x62e4`: if `0x20000f21` byte 0 is 0, CC[0] = `MOVW #0x1277` (4727); otherwise `#0x49de` (18910). SHORTS bit 8 is COMPARE0_STOP (`orr #0x100` at `0x622a` / `0x6320`). `0x6334` also sets CC[1]=80. So N=0 autonomous TX is 4727 µs, not 4727.5, and a compare stops that timer. 18910 = 4×4727.5 only as arithmetic.
- Sleep math uses the double, not that MOVW. `0x6cb8` / `0x6d2a` ADR to `0x6d40` (documented bytes `000000008077b240` = 4727.5). `0x6cc2` ADR to `0x6d48` = 3538.0. `0x6d34` ADR to `0x6d50` = 3934.5. `0x6ce8` passes the integer result to `0x641c`. If `0x20000f02` is nonzero, `0x641c` scales by 30.5 into an RTC compare; if it is zero, another path is used. Microseconds computed before `0x641c` are not automatically the open-window time.

## K04331
Source `D0753`, original line15, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

Command 16 does not change this immediately. `0x6100` stores pending N and C at `0x20000f21`. `0x6df6–0x6e0a` copies pending N into the live byte only after C has counted down to 0. `0x6e94` decrements C. C is a transition count. Native Phoenix builds `[N, 8]`: `0x10f48` passes 8 into `0xeadc`, and `0xe6f0–0xe6f8` emits the two body bytes reversed. Our mailbox default is `[N, 2]` (`station_v4_runtime.py`). C=8 reproduces that countdown. It is not evidence that the ratio will rise.

## K04332
Source `D0753`, original line17, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

## S1, and which delay it selects

## K04333
Source `D0753`, original line19, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

On RX, packet base `0x20000abd`:

## K04334
Source `D0753`, original line21, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

- `0x5ad2` and `0x5b7e`: `ubfx` bit1 into `0x20000858`. `0x59d8` returns that byte.
- `0x5ae0` and `0x5b84`: S1 bit3 sets `0x20000e08`. `0x59cc` returns that byte. `0x51c0` clears it on one re-arm path.
- `0x5aea`: bits 1..2 are stored on a side record. Consumers of that byte were not followed here.

## K04335
Source `D0753`, original line25, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

Scheduler `0x6ca4`, only while `0x20000f6a` is 0:

## K04336
Source `D0753`, original line27, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

| Latch `0x20000e08` (bit3) | Saved bit1 | Delay passed toward `0x641c` |
|---|---|---|
| 0 | 0 | `int(N*4727.5 + 3538)` |
| 0 | 1 | `int(N*4727.5 + 3934.5)` |
| 1 | ignored | same 3538 path |

## K04337
Source `D0753`, original line33, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

If `0x20000f6a` is nonzero, the same fork uses fixed `MOVW #3355` (`0x6ce4`) or `#3751` (`0x6cde`) instead of the doubles. That byte also makes `0x6202` skip autonomous TX. Its full meaning is not established.

## K04338
Source `D0753`, original line35, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

Our host always publishes S1 2 or 3 (`peer_engine.py` `mailbox`: bit1 set, bit0 only as the reliable-reply flag). The MCU copies `(meta>>8)&3`, so bit3 cannot be set. With bit1 set and the bit3 latch clear, the tracker is on the **3934.5** side. For N=0 that argument is 3934 before `0x641c`. The remainder `4727.5−3934.5 = 793` µs is the distance from that delay to the next double period. It is not a measured listen aperture. Clearing bit1 would move the delay to 3538. That is a 396.5 µs schedule change, not a justified experiment; iteration 22 already says not to flip bit1 blindly.

## K04339
Source `D0753`, original line37, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

Reply TX is immediate in the RX handler: `0x5b0e–0x5b2a` copies RXMATCH to TXADDRESS, sets PACKETPTR, waits STATE, then `TASKS_START`. `0x5138` ORs SHORTS bit 2 (DISABLED_TXEN) before that arm. There is a separate `MOVW #457` into `0x641c` at `0x6a08` when `0x56b8` returns nonzero. A different site, `0x5a64–0x5a78`, converts `length<<2` and combines it with the double at ADR `0x5c78`. Those two were not reduced to one “457.5−4L” formula here. `0x5a54` does set the reply budget to `min(54, 65−length)`.

## K04340
Source `D0753`, original line39, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

Command 10 is a different timer. Phoenix `0xac86` reads TIMER4 CC1 (`0x4001a544`) into `0x16084` (command 10, length 4). Tracker `0x79b8` checks PPI channel endpoint `0x40009048` and adjusts a compare. Iteration 23 already showed that completing command 10 does not raise RX/TX.

## K04341
Source `D0753`, original line41, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

## Why our service TX misses that schedule

## K04342
Source `D0753`, original line43, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

Phoenix `0x14880–0x148c8` rounds a count up by steps of `0x271` (625), writes TIMER0 CC[0], and PPI-connects COMPARE0 (`0x40008140`) to RADIO `TASKS_TXEN` (`0x40001000`). The 625 figure is in timer units; this pass did not prove that this path’s prescaler makes that 625 µs.

## K04343
Source `D0753`, original line45, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

`outputs/service-fast-poll24/roy_ram_station_v4_grid.c` and `pc/radio/ram/roy_ram_station_v4.c` do the opposite: at MPSL START they write `TASKS_TXEN` immediately (`RADIO+0 = 1`). MPSL distance 4727 µs is not Phoenix’s PPI phase. Service mode also forces `D(1880)=0`, so the distance stays the integer 4727. The tracker’s sleep double advances 4727.5 µs. That is 0.5 µs per poll, about 106 µs/s, and the ~793 µs remainder above is gone after roughly 8 s even if the first hit was centered. The non-service comment in the same C file already describes alternating 4727/4728 for a 4727.5 µs mean. Service builds dropped that correction.

## K04344
Source `D0753`, original line47, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

RX open is late relative to the tracker’s hardware turnaround. SHORTS is set to 11 (READY_START | END_DISABLE | DISABLED_RXEN), then replaced with 19 before END, which clears DISABLED_RXEN. `TASKS_RXEN` happens later, from the 100 µs TIMER0 poll, and only when `STATE==0`. The tracker can already have `TASKS_START` from `0x5b2a`. Captured replies at 226–759 µs show the deadline is wide enough for packets that were received. It does not show when the radio became ready. The C source never clears or starts TIMER0; it treats CC[1] as microseconds from slot start versus `D(1892)`. Received packets imply that counter was still below 800 µs for those slots. That does not prove MPSL resets TIMER0 on every grant.

## K04345
Source `D0753`, original line49, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

Slot length must stay inside the grant. Optics-off and optics-on both hit stock assert 106/501 at `PC=0x13e54`, `LR=0x10507` (`0x10428–0x10434` overstay). A longer slot to “cover” the window is the wrong lever.

## K04346
Source `D0753`, original line51, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

## State 1 and `-35`

## K04347
Source `D0753`, original line53, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

`pc/tests/test_service_grid_native_request.py` runs the iteration 25 callback against `outputs/service-fast-poll20/roy-stock.bin`. On signals 5, 6, and 7 with request type 1 (NORMAL) and session state 1, the real API returns `-35`, the callback sets `stop`, and nothing is submitted. State 2 accepts NORMAL: distance is a 4727 µs multiple, at least 300 µs ahead, length 900. Type 0 (EARLIEST) is accepted in both states. `0x18924` is a `tbh` table, so the linear listing around `0x18948` is not reliable instruction text; the Unicorn run is the evidence. Iteration 26’s reading of that API matches the test: states 1 and 2 are requestable; a nonzero type is accepted only in state 2; otherwise `-35`. Dispatch `0x1038e–0x10394` can deliver the low-priority callback in state 1.

## K04348
Source `D0753`, original line55, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

The grid file does this unsafely: on signals 5–7 it keeps `request.type==1` and calls `0x18949`. Any nonzero result sets `stop`. One BLOCKED/CANCELLED while the session is still state 1 ends the run. Production `roy_ram_station_v4.c` already avoids that by forcing type 0 there. That also drops the 4727 µs phase.

## K04349
Source `D0753`, original line57, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

Low-priority signals must still return action NONE. Returning REQUEST is assert 106/472 (`0x103b6`).

## K04350
Source `D0753`, original line59, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

## Recipe

## K04351
Source `D0753`, original line61, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

1. Keep S1 at 2 or 3. Leave bit3 clear. Do not try bit1=0 as a ratio fix.
2. On the high-priority slot end, keep NORMAL distance `interval + D(1880)` with host `D(1880)` alternating 0 and 1 so the mean is 4727.5 µs. Re-base that phase from the last software TXEN (`DATA+1904`) or `Peer.last_rx`, not from a fresh MPSL earliest grant. Adjacent answered TXEN already lands near 4727 µs; the failure is drift and missed grants, not an unknown period.
3. Leave DISABLED_RXEN set through TX END (do not swap SHORTS 11 for 19 before END). Add ADDRESS_RSSISTART without clearing bit 3. Keep the 800 µs deadline and the 100 µs margin so the callback still fits the 900 µs grant.
4. On signals 5–7: action NONE. If session state is 2, one NORMAL request on the same 4727.5 µs rule, length 900, at least 300 µs lead. If state is 1 or the API returns `-35`, one EARLIEST (`a=slot_us`, `b=100000`), do not set `stop`, and record a re-anchor. Any other API error still sets `stop`. After the next START, slot-end may request NORMAL again.
5. Do not sweep N or the period. Do not use command 10 for this. C=8 only if the goal is to copy Phoenix’s countdown, after the phase and turnaround changes, and it still does not prove the live N.

## K04352
Source `D0753`, original line67, SHA256 `654cf8b3f1df7025d4d904e0762e71d0b93c40a253e5820818e89d75837a057b`. Status: historical/unadjudicated narrative; English source prose.

`0x641c`’s RTC path, the exact start instant of the 3538/3934.5 timer, and Phoenix’s 625-unit prescaler are still open. This is a source-level schedule correction, not a prediction that the ratio will reach 60%.

## K04353
Source `D0756`, original line1, SHA256 `a0feae95a690332ba70d55191a334c0c29b2ad764c826cbd2be5c4edff28aa3d`. Status: historical/unadjudicated narrative; English source prose.

# Tracker OTA Analysis (FW189)

## K04354
Source `D0756`, original line3, SHA256 `a0feae95a690332ba70d55191a334c0c29b2ad764c826cbd2be5c4edff28aa3d`. Status: historical/unadjudicated narrative; English source prose.

Based on a read-only analysis of the disassembled tracker application and provided clues, here are the findings regarding the OTA firmware update mechanism:

## K04355
Source `D0756`, original line5, SHA256 `a0feae95a690332ba70d55191a334c0c29b2ad764c826cbd2be5c4edff28aa3d`. Status: historical/unadjudicated narrative; English source prose.

### 1. OTA Radio Commands and Body Layout
- **Container Structure:** The OTA container (`Swift2.0_sv1.89_20241213_b189_app.bin`) starts with a 321-byte (`0x141`) header. The header contains two duplicate `0x17914` (96532) size indicators and a 256-byte RSA signature (offset `0x40` to `0x13F`).
- **Image Body:** Immediately after the header (at `0x141`) is the raw firmware image starting with the vector table (`00 60 00 20 A5 13 00 00...`). The container includes two exactly identical copies of this 96532-byte image. Both copies are linked for base `0x1000` (Bank 0 execution).
- **Radio Protocol:** The headset service (Android) parses the header and verifies the RSA signature. It then streams the raw image chunks over the radio to the tracker using specific radio commands. The tracker app receives these chunks and writes them to flash without further signature verification.

## K04356
Source `D0756`, original line10, SHA256 `a0feae95a690332ba70d55191a334c0c29b2ad764c826cbd2be5c4edff28aa3d`. Status: historical/unadjudicated narrative; English source prose.

### 2. Where the Image is Written (A/B Banks)
- The tracker uses a **dual-bank** update strategy.
- **Bank 0 (Active):** The running application is at `0x1000` to `0x18914` (size `0x17914`).
- **Settings Page:** The bootloader metadata/settings page is located at `0x2F000` (the last page of a 192KB flash layout, implying the bootloader was originally sized for nRF52810/811).
- **Bank 1 (Update):** The update chunks are written to a secondary bank. Since Bank 0 ends at `0x18914` and the settings are at `0x2F000`, the space between them (`0x19000` to `0x2EFFF` = 90112 bytes) is **too small** to hold the 96532-byte update. Therefore, Bank 1 must be located above the settings page (e.g., starting at `0x30000`), leveraging the larger 512KB flash of the nRF52833.

## K04357
Source `D0756`, original line16, SHA256 `a0feae95a690332ba70d55191a334c0c29b2ad764c826cbd2be5c4edff28aa3d`. Status: historical/unadjudicated narrative; English source prose.

### 3. What is Validated Before Activation
- **Tracker App Validation:** The tracker app performs minimal validation. It relies on the BLE/ESB radio link-layer CRC for chunk integrity. 
- **Signature:** As noted, the RSA signature is ONLY validated by the headset. The tracker app completely ignores it.
- **CRC/Checksum:** Before resetting, the app prepares a 24-byte (`0x18`) metadata buffer in RAM. It sets the image size at offset `+0x4` and writes the magic activation word `0xAAAAAAAA` at `+0xC`. It then writes this buffer to the flash settings page at `0x2F000`. Finally, it verifies the flash write by computing a CRC8 (via function `0x7d30`) over the `0x2F000` flash page and comparing it to the RAM buffer. It does **not** perform a cryptographic hash or even a full CRC32 check of the entire downloaded image (the CRC field at offset `+0x10` is explicitly zeroed out).

## K04358
Source `D0756`, original line21, SHA256 `a0feae95a690332ba70d55191a334c0c29b2ad764c826cbd2be5c4edff28aa3d`. Status: historical/unadjudicated narrative; English source prose.

### 4. How Activation/Reset Happens and Bootloader Interactions
- **Activation Sequence:** Once the entire image is written to Bank 1, the app updates the settings page at `0x2F000`. It writes the magic word `0xAAAAAAAA` (represented as `-0x55555556` in the disassembly at `0x00facc`) to `0x2F00C`.
- **System Reset:** After verifying the settings page write, the app calls `NVIC_SystemReset()` (triggering the system resets seen at `0xFB0C` or `0xFDFC`).
- **Bootloader Actions:** Upon reboot, the bootloader reads the settings page at `0x2F000`. It checks the word at `0x2F00C`. Seeing `0xAAAAAAAA`, the bootloader knows a valid update is waiting in Bank 1. It then erases Bank 0, copies the image from Bank 1 to `0x1000`, clears or updates the magic word (e.g., to `0xBBBBBBBB` or `0xDEADBEAF`), and boots the new application.

## K04359
Source `D0756`, original line26, SHA256 `a0feae95a690332ba70d55191a334c0c29b2ad764c826cbd2be5c4edff28aa3d`. Status: historical/unadjudicated narrative; English source prose.

### 5. Interrupted Transfers and Custom Images
- **Interruption Safety:** Yes, an interrupted transfer **leaves the running app intact**. Because this is a dual-bank system, the active application at `0x1000` is never touched during the download. The magic word `0xAAAAAAAA` is only written to `0x2F000` after the transfer completes. If it fails midway, the tracker simply discards the partial Bank 1 data and reboots normally.
- **Booting a Modified Image:** **Yes**, the tracker **would boot a slightly modified application image**. Since the cryptographic signature is stripped and checked only by the headset service, an attacker who bypasses the headset (e.g., by speaking directly to the tracker over the radio using the station MCU or a custom transmitter) can send a modified image. As long as the basic OTA radio protocol is followed, the tracker will happily write the payload to Bank 1, stamp it with `0xAAAAAAAA`, and reboot. The bootloader will then blindly copy and execute the modified firmware.

## K04360
Source `D0757`, original line1, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

# PICO Swift Tracker FW189 RX/TX Timeline

## K04361
Source `D0757`, original line3, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

## 1. Precise State Machine

## K04362
Source `D0757`, original line5, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

The tracker's radio timeline is a classic state machine built around the `RTC0` (or `TIMER2`) one-shot sleep timer and the `4727.5` us base slot grid. It is completely independent of the 533333-tick optical frame timer and anchors to the **most recently received station packet**.

## K04363
Source `D0757`, original line7, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

**State 1: Packet Received & Autonomous TX**
- **Anchor Event**: On a hardware `ADDRESS` match, `TIMER0` (1MHz) is cleared and restarted with `CC0=4727` (if N=0) or `18910` (if N!=0).
- Exactly `CC0` microseconds later, `TIMER0` triggers handler `0x61F4` which calls `0x5E9C` to perform the autonomous TX reply.
- Sometime after the packet, `0x6CAC` computes the sleep time before opening the next RX window. For N=0, the sleep is **`3538` us**.
- The tracker disables RX (`0x57EC`) and arms the one-shot timer (`0x641C`) for `3538` us.

## K04364
Source `D0757`, original line13, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

**State 2: RX Window Open (Timeout Wait)**
- After the `3538` us sleep, the timer interrupt fires and calls `0x5730` to **enable RX**.
- The tracker is now listening. The timeout is exactly the time remaining in the `4727.5` us slot (i.e., `4727.5 - 3538 = 1189.5` us).
- **If a packet arrives**: We go back to State 1. The tracker's 4727.5us grid is effectively reset/anchored to this new packet.
- **If no packet arrives (Timeout)**: The timer fires again. The unified RADIO/RTC interrupt `0x6AB8` detects the timeout (via state flag `[20000ef5] == 2`), disables RX, and enters State 3.

## K04365
Source `D0757`, original line19, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

**State 3: Missed Poll Shift**
- The tracker computes a slightly modified sleep time to hunt for the station: **`3751`**, **`3355`**, or **`3934`** us (depending on flags `[20000f6a]` and `[20000858]`). 
- Notice how these mathematically preserve the grid width: `3751 + 976.5 = 4727.5` us. The tracker strictly maintains the 4727.5us slot duration but shifts the RX window opening time within that slot.

## K04366
Source `D0757`, original line23, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

## 2. Relation to Command 10 (533333-tick timer)

## K04367
Source `D0757`, original line25, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

The RF RX schedule is **NOT** tied to the 533333-tick frame timer. The 533333-tick (33.333 ms at 16MHz) and 640000-tick (40.0 ms) timers are strictly used for optical LED exposure synchronization. The radio timeline runs freely on its 4727.5us slot grid and anchors purely to the last received RF packet. Command 10 aligns the LEDs to the camera, but does not move the RF RX windows.

## K04368
Source `D0757`, original line27, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

## 3. Explanation of Hardware Observations

## K04369
Source `D0757`, original line29, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

* **P=36000us -> exactly 3 consecutive replies then ~300ms silence**
  The tracker's RX window is small (~1189us). A 36000us poll phase shifts by $36000 \pmod{4727.5} = 2907.5$ us per poll. However, in SEARCH mode (after ~300ms of misses, `200/(N+1)` threshold reached), the tracker opens its RX wider or stops hopping. It catches the first poll, anchors its grid, and because 36000 is not a multiple of 4727.5, the subsequent polls quickly drift out of the narrow 1189us window after exactly 3 hits. It then misses enough polls to reset back to SEARCH mode.

## K04370
Source `D0757`, original line32, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

* **P=12000us -> runs of 8 consecutive replies then a gap ~120ms, and a 624ms periodic pattern**
  132 slots * 4727.5 us = 624030 us. This proves the tracker's channel hopping sequence is 132 slots long. At P=12000, the station polls every 12ms. The phase shift $12000 \pmod{4727.5} = 2545$ us causes the poll to alias perfectly across the tracker's hopping channels. When a poll finally lands inside the window on the correct channel, the tracker anchors to it. The gap of 120ms matches the time it takes for the tracker to exhaust its `200/(N+1)` missed-poll limit (about 25-30 slots) and drop the connection before finding the station again.

## K04371
Source `D0757`, original line35, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

* **P=42000 with N=8 -> 10-30% replies**
  If N=8, the tracker sleeps for `8 * 4727.5 + 3538 = 41358` us. The RX window is `1189.5` us. $42000 \pmod{42547.5} = -547.5$ us. The poll drifts slowly backwards through the 1189us window, hitting it for several polls. However, the tracker is hopping channels every 4727.5us. Since it's only on the correct channel a fraction of the time during that 41ms sleep, the station only gets replies 10-30% of the time.

## K04372
Source `D0757`, original line38, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

* **P=9000 with N=1 -> 0 replies**
  For N=1, the frame is $2 \times 4727.5 = 9455$ us. Sleep is `4727.5 + 3538 = 8265.5` us. Window is `1189.5` us. P=9000 actually hits the window perfectly ($9000 \in [8265.5, 9455]$). The reason for **0 replies** is that the station is hitting the tracker exactly 2 slots later, meaning the tracker has hopped 2 channels away. If the station doesn't advance its frequency by exactly 2 hops, it will transmit on the wrong channel and get 0 replies.

## K04373
Source `D0757`, original line41, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

## 4. Recommended Poll Schedule

## K04374
Source `D0757`, original line43, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

Given your constraint: transmissions must start in a ~900us quiet window repeating every 3000us (i.e., `[2100, 3000]`, `[5100, 6000]`, `[8100, 9000]`, etc.).

## K04375
Source `D0757`, original line45, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

You **cannot** hit the tracker's very first window (`W1 = [3538, 4727.5]`) because the quiet windows around there are `3000` (too early, RX is OFF) and `6000` (too late, RX is OFF).

## K04376
Source `D0757`, original line47, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

However, if you allow the tracker to miss exactly one poll, it will sleep for `3751` us and open its second window:
`W2 = 4727.5 + 3751 = 8478.5` us. 
This window stays open until the end of the second slot at `9455` us.

## K04377
Source `D0757`, original line51, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

The third quiet window is exactly **`[8100, 9000]`**. 
The overlap between the quiet window and the RX window is **`[8478.5, 9000]`**.

## K04378
Source `D0757`, original line54, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

**The Strategy:**
1. Set your station polling interval to exactly **9000 us** (every 3rd quiet window).
2. Start your transmission at **`+8700 us`** relative to your last packet. This safely lands in both the 900us quiet window and the tracker's `W2` RX window.
3. Because the tracker misses `W1`, it will advance its hopping sequence by 1 hop before `W2`. Your station must therefore **advance its frequency by 2 hops** every 9000us poll to intersect the tracker perfectly in `W2`.

## K04379
Source `D0757`, original line59, SHA256 `7fdf8e8e6e48e35c962f3e9a944d885268adb5df950e6d8a9d4796a7dffc1f0d`. Status: historical/unadjudicated narrative; English source prose.

This guarantees ~100% replies without fighting the Roy BLE grid.

## K04380
Source `D0760`, original line1, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04381
Source `D0760`, original line3, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04382
Source `D0760`, original line7, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04383
Source `D0760`, original line13, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04384
Source `D0760`, original line19, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04385
Source `D0760`, original line21, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04386
Source `D0760`, original line23, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04387
Source `D0760`, original line34, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04388
Source `D0760`, original line36, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04389
Source `D0760`, original line40, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04390
Source `D0760`, original line42, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04391
Source `D0760`, original line51, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04392
Source `D0760`, original line53, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04393
Source `D0760`, original line67, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04394
Source `D0760`, original line69, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04395
Source `D0760`, original line84, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04396
Source `D0760`, original line86, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04397
Source `D0760`, original line92, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04398
Source `D0760`, original line99, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04399
Source `D0760`, original line101, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

## K04400
Source `D0760`, original line103, SHA256 `2a6254973afa7c1b4d015732877d22932a46d4aa5d94f53ba4f5272865ce4841`. Historical third party packet reference supplied by Hinata Reira.

The original wording is not reproduced or relicensed in this public edition. See the [attributed factual digest](../../docs/HINATA_REFERENCE_DIGEST.md) and [specific credits](../../CREDITS.md). This entry retains its ID, source hash and original line for retrieval.

