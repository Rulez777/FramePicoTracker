# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05041
Source `D0174`, original line41, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is a real execution of the corresponding application/queue instructions with a controlled worker order; this is not an over-the-air measurement. Consequently, pre-drain closes the old, already queued ACK, but is not proof of the absence of pending OTA work.

## K05042
Source `D0174`, original line43, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For the first hardware run, a confirmed fresh launch of the tracker application prior to OTA, the absence of other updating stations, and a prohibition of an automatic new session after an error will be required. A single reconnect/pairing is not proof of a new launch. The recovery procedure after a failed boot remains unknown.

## K05043
Source `D0174`, original line45, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### What is known about a fresh application launch

## K05044
Source `D0174`, original line47, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The new test executes C runtime startup0x1200 with pre-polluted RAM and stops before main/RTOS. On stock and candidate, OTA state/worker flags, queues, the transmitted reliable records table, scheduler flags, and f69 are cleared. The `.bss` clearing covers 20000668..2000514c.

## K05045
Source `D0174`, original line49, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This confirms the clearing upon execution of this startup, but does not prove that the hardware bootloader actually launches the candidate. Reset/cold power-on cannot be considered verified based on product version189 alone.

## K05046
Source `D0174`, original line51, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Roy radio slot: specific bugs fixed

## K05047
Source `D0174`, original line53, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In `roy_ram_station_v4.c`:

## K05048
Source `D0174`, original line55, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- The first compare after TX is now bounded by the RX deadline. With now350 and end400, the result is 400, not 450.
- If TX has already ended at/after the deadline, the radio is turned off and the current slot is terminated, without stopping the entire session. A compare in the past is not programmed.
- In the timer callback, the current time is read via CAPTURE1. The software CC value is no longer taken as the actual IRQ servicing time.
- A late IRQ terminates the slot before capture: a response after the boundary does not enter the ring.
- The deadline is fixed for a single callback; its configuration must remain constant during this hardware session.

## K05049
Source `D0174`, original line61, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Before/after ARM ELFs have been compiled. For the new ABI4, `-fomit-frame-pointer` has been added: this freestanding module is already built without unwind tables. Local clang from the Android NDK is normally available; a standard build without this flag did not fit even the previous source. The linker script limit was NOT increased.

## K05050
Source `D0174`, original line63, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Artifact | Code size |
| --- | ---: |
| Before, the same local compiler + omit-frame-pointer | 2008 B |
| After, fixed callback | 2040 B |
| Limit | 2045 B |

## K05051
Source `D0174`, original line72, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`test_roy_abi4_deadline.py` executes this exact ELF in Unicorn with an MMIO model. In before, compare450 at TX end350 and capture at actual time450/old compare300 are reproduced. In after, early/boundary/late TX, late IRQ, and preservation of the next slot have been verified.

## K05052
Source `D0174`, original line74, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This eliminates software counterexamples. Hardware latency of IRQ and RADIO DISABLE, MCU build compatibility, and coexistence with BLE have not been measured. The code cannot be declared a physically guaranteed cutoff of 400 µs based on this model.

## K05053
Source `D0174`, original line76, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## FW189 timers and binary candidate

## K05054
Source `D0174`, original line78, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Additionally, transition 0x6ab8 was executed with f02=1, activity flag e4d=1, and corresponding fixture conditions. The scheduler resets f02 to 0, returns TIMER2, and selects the unchanged delay 9922. The RTOS deferred callback and two extraneous timer handles are substituted in this bounded test. This explains the specific transition between clock domains, but not all streaming/recovery modes.

## K05055
Source `D0174`, original line80, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

No new replacements 9922/10980/30.5 have been made. The original minimal candidate has the former SHA256 `6a82a89206a609fdce92a3de764882ce53f79f0360b925bebe1ce7853cde6a0d`: 30 differing bytes in 10 locations, an exact reverse to stock. Native version189 is retained. Nominal TIMER0=3000/12000 µs and normal TIMER2 conversion are confirmed; RTC quantization, absolute RX phase, and over-the-air effects remain separate questions.

## K05056
Source `D0174`, original line82, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Checks and RC1 package

## K05057
Source `D0174`, original line84, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Full focused run: **145 passed**, including full-size4023blocks through the real dispatcher, native command dedup, and ACK queues. SPI/RF/RTOS physical effects remain model limitations.

## K05058
Source `D0174`, original line86, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- JUnit: `outputs/fw189-iteration02-20261007-tests.xml`.
- Source snapshot, firmware/stock/manifest, before/after ELF, and reports: `outputs/fw189-grid3000-rc1-20261007/`.
- Archive: `outputs/fw189-grid3000-rc1-20261007.zip`.
- `release-status.json` explicitly stores `approved_for_ota=false` and package hashes.

## K05059
Source `D0174`, original line91, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Firmware bytes have not been renamed to "verified production release". In RC1, there is no automatically starting job, private registry/seed/config, or hardware launcher.

## K05060
Source `D0174`, original line93, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What else separates RC1 from release

## K05061
Source `D0174`, original line95, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Proven boundary of a fresh OTA session: pending old work after one reconnect is not ruled out. Fresh startup resets state in the application; hardware confirmation of such startup has not yet been obtained.
2. Bootloader/activation/recovery: application metadata/reset have been analyzed, bootloader is not available in the current app image.
3. Radio phase, OTA response duration, and controllers: after a separate user signal, a single prepared integrated run with coordinated firmware/sender/SRAM versions is needed, rather than a series of short trials.

## K05062
Source `D0174`, original line99, SHA256 `a909298ff3026cda9f3bf4f56ad3a788f5ea7dccc3ded5641de639538d7c429a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The release goal today remains a goal. This package allows verifying specific fixes and moving toward it, but the listed limitations are not lifted by setting a date.

## K05063
Source `D0175`, original line1, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 03 — FW189: stock / Frame mode in a single image

## K05064
Source `D0175`, original line3, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-07. Status: experimental offline fork for independent verification, not a hardware release. `approved_for_ota=false`. SSH, RF, SWD, OTA, uploading to the device, and modifying services were not performed. The user pause remains in effect.

## K05065
Source `D0175`, original line5, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Result and usage

## K05066
Source `D0175`, original line7, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A single application image contains two timing selection branches. A full startup clears the flag: a normal short power-on selects **stock**. For **Frame**, the design is to hold the button during power-on and release it at counts 100–149. Nominally, this is about 1,0–1,49 seconds; the actual duration and accessibility of the gesture across different startup sources require hardware verification. Holding for 150 counts or more does not select Frame. The branch for 600 counts triggers the standard event 3.

## K05067
Source `D0175`, original line9, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The selection is not written to flash. After selection, the profile applies to the session; standard software pseudo-power-off may preserve the flag. The verified full C startup and the full power-off branch reset it. Therefore, the promise "any power-off always returns stock" is currently not true. A new LED signal has not been added: indication remains native. Stock here means original radio constants, not byte-for-byte stock firmware: startup, memory, and transitions are modified.

## K05068
Source `D0175`, original line11, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Files and identification

## K05069
Source `D0175`, original line13, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Builder: `pc/radio/tracker_dual_mode_patch.py`.
- Tests: `pc/tests/test_fw189_dual_mode.py`.
- Verified build: `outputs/fw189-dual-mode-20261007d/` — image, stock copy, manifest of each change, original assembler, linker script, object and ELF.
- Size: **97152 bytes**, base **0x1000**, end **0x18b80**.
- SHA256 fork: `c5158c9f26f8bf5fedbea1d4c497fd651d1188fac11b8bd1bf3389132a064c0e`.
- SHA256 stock: `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89`; size 96532.
- Added 616 bytes of code/literals at 0x18918 plus 4 bytes of alignment. Product/version 189 and vector table are unchanged.
- Repeated independent local build in `outputs/fw189-dual-mode-repro-20261007/` yielded the same binary hash. Reverse transformation restores the exact stock hash. This is offline reversibility, not proof of hardware recovery.

## K05070
Source `D0175`, original line22, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Intermediate directories without suffix, b, and c are not the final build. In b/c, startup transition / gesture unreachability errors are fixed. The old ten-pad candidate and RC1 are not overwritten.

## K05071
Source `D0175`, original line24, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Profiles and addresses

## K05072
Source `D0175`, original line30, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The selectors preserve APSR NZCVQ, the remaining GPRs, SP, and LR. They temporarily use 8 bytes of stack. Offline comparison verifies the complete register state after each of the 11 selectors for flags 0/1/2/255.

## K05073
Source `D0175`, original line32, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The TIMER0 unit was confirmed by the previous iteration with real init: PRESCALER=4 (BITMODE=3 is another write), tick=1 µs. The RTC path 0x641c remains standard and divides by 30.5 with integer conversion: the 3000 µs grid does not become an exact RTC grid. Radio compatibility and phase lock are not proven by this fork.

## K05074
Source `D0175`, original line34, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Button and lifecycle changes

## K05075
Source `D0175`, original line36, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The stock board defines pin13 via ROM descriptor 0x17da4 and RAM pointer 0x200005bc; active-low input. Firmware does not add an independent hardcoded GPIO driver.

## K05076
Source `D0175`, original line38, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- 0xc3ec: start button entry, flag clearing; both displaced instructions are reproduced — push and loading r6. The first prototype skipped loading r6; the error was identified and fixed before the final build.
- 0xc444: store of counter 0x20001ff2 and transition to the standard check 600. This is a **deliberate startup behavior change**: the original with resetreason=0 after five counts proceeded further even with the button held down. The fork waits for release or threshold600, otherwise the selection gesture is impossible. Consequence: holding the button delays startup and allows reaching the standard event3 branch; its practical effect and UX still require verification.
- 0xc4aa: upon release, count100..149 sets Frame, then standard event1. Other releases select stock after clearing at the entry.
- 0xbce6: original HFCLKSTOP 0x5fe0 is called, then the profile is cleared and the original path continues.

## K05077
Source `D0175`, original line43, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The runtime button 0xc28c with a separate counter 0x20001ff4 is not patched. The standard IMU, LEDs, camera, and commands have not been rewritten. Their behavior after a delayed start and heap reduction has not been fully verified; the absence of driver modifications does not equate to the absence of potential impact.

## K05078
Source `D0175`, original line45, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## RAM reserve

## K05079
Source `D0175`, original line47, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The flag is located in 0x20005150. Literals 0x1348 and 0x134c change from 0x20005150 to 0x20005158: BSS expands by8, heap starts 8 later, end0x20005350 is preserved. Native heap decreases **512→504 bytes**. Full C startup indeed zeroes out8 reserved bytes and creates a heap header with size504. A complete worst-case analysis of all dynamic allocations has not been performed; this is a separate risk prior to release.

## K05080
Source `D0175`, original line49, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Validation and its limits

## K05081
Source `D0175`, original line51, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Result: **62 passed**, 34.18 seconds. JUnit: `outputs/fw189-dual-mode-20261007-tests.xml`. Real Thumb code and application arithmetic were verified with Unicorn; GPIO, delays/peripheral setup, and certain external calls are mocked. IRQ, physical time, reset reason, bootloader, electronics, and RF/air are not modeled.

## K05082
Source `D0175`, original line53, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The checks cover:

## K05083
Source `D0175`, original line55, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Initial gesture count4/99/100/120/149/150/599/600, pre-polluted flag, standard six-second branch.
2. All N=0..255, both TX functions and RX normal/alternate/miss/both hunt paths; comparison of stock mode with original and Frame mode with the former fixed-grid candidate.
3. All 11 transitions: GPR, APSR, SP, LR for 0/1/2/255.
4. Real TIMER2/RTC conversions for both modes, N=0..255, and three RX branches.
5. Full C startup, BSS/heap, execution of standard HFCLKSTOP before profile cleanup, exact inverse conversion.

## K05084
Source `D0175`, original line61, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The first pytest attempt crashed inside Windows Unicorn mem_map with an access violation. A rerun with pytest faulthandler disabled runs separately; an environment error is not considered a firmware verification result. The final JUnit is the source for the number of passed tests.

## K05085
Source `D0175`, original line63, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Why flashing is not yet allowed

## K05086
Source `D0175`, original line65, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The current load_job permits only the former 96532-byte images. The new 97152-byte fork was intentionally not added to the whitelist. The delivery/CRC format of the expanded image and bootloader activation must be verified separately. There is no proof of bootloader support for the added code, signature/container, and recovery. The previously reproduced issue of the old OTA worker/ACK after an empty barrier persists; reconnecting does not prove a clean session.

## K05087
Source `D0175`, original line67, SHA256 `1939d6b4a70e38bd47ad7a316929a56d920a3a22ba170d6f697f8cbf59e52d6f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Next steps: independent review of addresses and branches; analysis of heap and pseudo-power-off; extended OTA/activation model specifically for this size; upon a separate user command — hardware verification of startup/button/power-off and RF. Currently, the result is a testable fork, not a release confirmed on the tracker.

## K05088
Source `D0176`, original line1, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration04: dual-mode independent review comments

## K05089
Source `D0176`, original line3, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-07. The image has NOT changed. SHA256 remains c5158c9f26f8bf5fedbea1d4c497fd651d1188fac11b8bd1bf3389132a064c0e. Whitelist has not been expanded. No SSH/RF/OTA/SWD/hardware launch. User pause remains in effect.

## K05090
Source `D0176`, original line5, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Verdict

## K05091
Source `D0176`, original line7, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The reviewer's comments were accepted. A clean build and 62 tests do not make this fork a safe stock rollback or a ready release. Our own chat statement about an incomplete final block was incorrect: 97152=24×4048; an empty sentinel4049 is required. The independent reviewer reported rebuilding616bytes, verifying17patches/15transitions, and repeating62tests. His temporary scripts were not run here; a separate independent verification of our own is described below.

## K05092
Source `D0176`, original line9, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Full size: additional verification performed

## K05093
Source `D0176`, original line11, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The new test pc/tests/test_fw189_dual_review.py executes the real native dispatcher, dedup, reliable queue, and OTA worker on the full fork image. The emulator's ROM contains the dual-image itself. SPI peripheral and allocation/IRQ helpers are mocked: this is an application test, not real flash/bootloader.

## K05094
Source `D0176`, original line13, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Size0x17b80=97152; start size/CRC bytes: 00 01 7b 80 30.
- 4048 full data blocks and block4049 (0x0fd1) with length0; sentinel body prefix 25 02 0f d1 00, padding24×ff.
- Sender reached finish_verified; received4049blocks, erase_count=1, write_count=4049. The sentinel indeed passes through the native worker as a separate24byte staging write, but is not included in the final image length/CRC.
- Reconstruction of4048SPI blocks with a ten×24 layout across256bytes matches the fork byte-for-byte. The ff-write of the sentinel was verified separately.

## K05095
Source `D0176`, original line18, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Consequently, the open question specifically regarding the full application sentinel path is closed in the offline scope. Bootloader acceptance of this size is not resolved; the whitelist still prohibits the image.

## K05096
Source `D0176`, original line20, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What event3 does

## K05097
Source `D0176`, original line22, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

After real C startup, the transition table is located at0x20000160. Dispatcher0x9ae8 selects the entry (current_state×10+event)×8, calls the callback, and on successful return writes the destination state. State names reside in table0x2000036c; index2 is STA_PAIRING.

## K05098
Source `D0176`, original line24, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For event3:

## K05099
Source `D0176`, original line26, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Initial state | Native callback | Next state |
| --- | --- | --- |
| 0 STA_POWER_OFF | 0x9988 | 2 STA_PAIRING |
| 1 STA_STANDBY | 0x9a4c | 2 STA_PAIRING with gate0x20000f6a open |
| 2 STA_PAIRING | absent | remains2 |
| 3 STA_CONNECTED | 0x9a4c | 2 STA_PAIRING with gate open |
| 4 STA_SLOW_ADV | 0x996c | 2 STA_PAIRING |
| 5 STA_FAKE_POWER_OFF | 0x994c | 2 STA_PAIRING |

## K05100
Source `D0176`, original line35, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Additional6tests execute the real table, event queue, and callback of each state; external subsystem calls are intercepted. Handlers0x9988/0x996c/0x994c set RAM0x20001ba8=1 and call0xa5e8. Handler0x9a4c checks the gate, changes the radio context, and calls0xc138. With gate!=0, it returns1, suppressing the transition: this follows from instructions0x9a50..0x9a70, not a separate test in this iteration.

## K05101
Source `D0176`, original line37, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

0xa5e8 → 0xa558 switches native subsystem callbacks. Deep implications for saved pairing/bond records are not completely understood. One cannot claim that event3 merely turns on harmless indication, or that it is guaranteed to erase pairing: the proven effect here is a transition to pairing with native side effects, some of which were not executed in the test.

## K05102
Source `D0176`, original line39, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the original startup, resetreason=0 produced a normal boot after five counts even with the button held. The fork delays it until release/600 and makes this pairing transition reachable during a standard initial hold. This is a significant change in UX and behavior. It must be fixed or explicitly agreed upon after complete analysis before advancing the fork toward release. Preferred next design: a separate bounded Frame selection branch without adding event3 to resetreason=0; for other resetreasons, preserve the standard branch. The binary was not modified in this iteration.

## K05103
Source `D0176`, original line41, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## How the station learns the profile

## K05104
Source `D0176`, original line43, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The comment is correct: version189 and the manual map of N do not prove the current mode. Automatic reliable classification based solely on response rhythm is impossible without a new loss/phase model: the absence of responses is ambiguous.

## K05105
Source `D0176`, original line45, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The preferred design is an explicit read-only mode query with a response tied to the current query's nonce and the tracker's identity. The station confirms the mode upon each new connection/restart, clears the previous confirmation, and only then selects the cadence/N. A separate review of an unassigned opcode, the ARM response patch, and host handling will be required; a specific opcode is not assigned here. Rhythm measurement may serve as an additional check, but not as the sole source of mode. Until such a handshake exists, dual-mode will not be included in the station's automated path. This is a design decision, not an implemented capability.

## K05106
Source `D0176`, original line47, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Remaining release conditions

## K05107
Source `D0176`, original line49, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Eliminate the expanded pairing entry from a normal hold and verify the full effect of event3/native callbacks.
2. Analyze the impact of heap512→504 and pseudo-power-off; stock constants inside the fork do not replace the real stock image.
3. Implement and verify the station's explicit mode/identity/session handshake.
4. Resolve bootloader limitations, recovery, and the former OTA ACK race; having a full-size native test does not substitute for this.
5. Upon a separate user command, verify physical startup/button/power-off and radio: RTC quantization, RX phase, window400 µs, and controller coexistence.

## K05108
Source `D0176`, original line55, SHA256 `42e7e29cf681e1ce6337924d16c46b085e07791b066731de5730667b3dab7edf`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

JUnit outputs/fw189-dual-review-20261007-tests.xml: **7 passed**, 5.16s. Previously, 62 tests were not rerun in this iteration; the new additional scope was verified separately. It is too early to ship dual-mode right now. Real stock remains the only byte-for-byte stock rollback, but hardware recoverability is still unproven.

## K05109
Source `D0177`, original line1, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration05 — bounded boot gesture and mode confirmation by the station

## K05110
Source `D0177`, original line3, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-07. Offline changes to the application fork, host station, and tests were completed. **Not a hardware release. approved_for_ota=false.** The device was not flashed, SSH/RF/SWD/services were not launched; the user's hardware pause remains in effect. Old binaries/reports/snapshots were not overwritten.

## K05111
Source `D0177`, original line5, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What has been fixed

## K05112
Source `D0177`, original line7, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. In the startup function for resetreason=0, a long press no longer reaches the added event3/pairing. At count150, the function terminates with event1, keeping stock. Frame is selected only upon release at count100..149. For other reset reasons, the original branch is reproduced: hold600 still has standard event3. The Frame gesture is not applied to them.
2. Added read-only profile request and verification of its response by the station. Until the first confirmation, a dual-mode peer receives no command16/N or camera-sync. A transport ACK does not satisfy the request.
3. The station periodically rechecks the profile. Confirmation has a limited lifetime; disconnect/reconnect clears it. When changing profile, configuration and phase are re-established from scratch.

## K05113
Source `D0177`, original line11, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The stock profile remains the profile of a modified application: up to150 counts startup delay is modified, native heap is 504 instead of512. This is not a byte-for-byte stock rollback. During operation, the standard button is not modified: further holding after startup completes can still activate native runtime pairing. The assertion that 'any long hold never activates pairing' would be false.

## K05114
Source `D0177`, original line13, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Identification of the final build

## K05115
Source `D0177`, original line15, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Directory **outputs/fw189-dual-mode-iter05-final-20261007/**:

## K05116
Source `D0177`, original line17, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- fw189-dual-mode.bin: **97320 bytes**, base0x1000, end0x18c28.
- SHA256: **395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f**.
- CRC8/MAXIM: **0x1f**. 97320=24×4055, empty sentinel4056 (0x0fd8), length0. New start length0x17c28.
- Stock source96532bytes: 477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89.
- 784bytes of appended .text/literals at0x18918 plus4ff alignment; total of788bytes expansion.
- 18original ranges: 16transitions and2RAM-boundary literals. Schema2 manifest contains every before/after, builder/assembly hashes, symbols, compiler/build commands.
- Rebuild in outputs/fw189-dual-mode-iter05-repro-20261007/ matched byte-for-byte. Reverse restores the exact stock hash.

## K05117
Source `D0177`, original line25, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Directories iter05 without final and iter05b are intermediate. The first variant intercepted0x8c10 and incorrectly considered r1 to be the length: in the real wrapper, the length there is already lost after memcpy. Native integration test revealed the lack of response. The final version intercepts0x8d16 prior to copying, where the real dispatcher passes r0=body,r1=length. The final image matches the corrected iter05b; final adds an extended build provenance manifest.

## K05118
Source `D0177`, original line27, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Boot branch and preservation of native paths

## K05119
Source `D0177`, original line29, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Patches0xc3ec (clear mode + original push/ldr), 0xc444 (counter store + bounded wait), 0xc4aa (release selection), 0xbce6 (native HFCLKSTOP, then clear) are preserved at their addresses.

## K05120
Source `D0177`, original line31, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

0xc444 after writing count:
