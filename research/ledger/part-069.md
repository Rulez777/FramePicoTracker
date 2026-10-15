# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05441
Source `D0193`, original line77, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Application path 0xfa98 was executed for arguments 0/1 with verified/state gates set. It forms 24-byte metadata: size 96532, CRC 0xe6, magic 0xbbbbbbbb or 0xaaaaaaaa, then requests a system reset. When the gate is unset, the function does not write metadata and does not request a reset.

## K05442
Source `D0193`, original line79, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

NVMC operation and delay are mocked with a model. Real metadata CRC and ARM control flow are executed. Bootloader, SPI→application transfer, signature, power loss, and recovery were not executed. Consequently, this is a clarification of the activation mechanism, not proof of booting or rollback.

## K05443
Source `D0193`, original line81, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The available raw app starts from 0x1000; the bootloader is absent in this image. Path 0x8548 → 0x855e invokes activation with 1, 0xa638 → 0xa648 — with 0. The final application ACK should not be considered a harmless staging check.

## K05444
Source `D0193`, original line83, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Reproduction and package

## K05445
Source `D0193`, original line85, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

From the project root:

## K05446
Source `D0193`, original line92, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- JUnit: `outputs/fw189-offline-revision-20261007-tests.xml` — 104 passed.
- Evidence/source hashes: `outputs/fw189-offline-revision-20261007-evidence.json`.
- Tests: `test_fw189_timer_units.py`, `test_fw189_activation_boundary.py`, supplemented OTA/runtime tests.
- The original frozen validation MD and candidate were not regenerated and not substituted by this report.

## K05447
Source `D0193`, original line97, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Stopping point

## K05448
Source `D0193`, original line99, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Safe and justified host fixes and a deeper offline verification have been made. For the new image, real questions remain regarding the purpose of timer branches and RX phase; for the sender — freshness of the ACK from the native queue; for OTA — bootloader/recovery. Hardware launch is not yet prepared for authorization. The next offline step is the native queue model and RTC/TIMER2 transitions, without additional RF probes.

## K05449
Source `D0196`, original line1, SHA256 `61ea3c439c9254004819a5da6a5c13c2ecc12312b89e13855893a1ecffa3afd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# FW189 remote mode44: third-party audit by Claude Opus 5.5

## K05450
Source `D0196`, original line3, SHA256 `61ea3c439c9254004819a5da6a5c13c2ecc12312b89e13855893a1ecffa3afd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. Read-only and offline checks: without OTA, without flashing, without RF, and without
restarting runtime. This is not hardware confirmation and not authorization for OTA.

## K05451
Source `D0196`, original line6, SHA256 `61ea3c439c9254004819a5da6a5c13c2ecc12312b89e13855893a1ecffa3afd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Object: `outputs/remote44/candidate07-initial-connection/fw189-dual-mode.bin`,
97 832 bytes, SHA256 `40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5`
(recalculated, matches). Stock `outputs/swift189-image.bin` 477000fc…a690 also matches.

## K05452
Source `D0196`, original line10, SHA256 `61ea3c439c9254004819a5da6a5c13c2ecc12312b89e13855893a1ecffa3afd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What was verified

## K05453
Source `D0196`, original line25, SHA256 `61ea3c439c9254004819a5da6a5c13c2ecc12312b89e13855893a1ecffa3afd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Confirmed (no defects found)

## K05454
Source `D0196`, original line27, SHA256 `61ea3c439c9254004819a5da6a5c13c2ecc12312b89e13855893a1ecffa3afd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **`profile_rx_boundary`.** Stack is balanced: 16+24 bytes, alignment by 8
   is preserved. Registers r0–r3, r12, APSR flags, and input PRIMASK are restored.
   Code at address 6c9c does not use the return of r0 from 57ec. Register r6 is callee-saved,
   and 57ec, 852c, 7fd4, and 7fa8 do not touch it. Under the mask, 57ec spins only
   waiting for RADIO `EVENTS_DISABLED`, while 852c and 7fd4 only write GPIO OUTCLR.
   Neither of them waits for an interrupt, so there is no deadlock.
2. **Stopping TIMER0** (SHUTDOWN 0x010 + CLEAR 0x00C) matches the native
   function 6380. The only difference: native also clears `20000eea`, but at
   the absolute address this byte is only written (5fce, 6388) and nowhere read.
   Possible re-triggering of the TIMER0 interrupt after ICPR due to the write buffer
   is harmless: 626c checks `EVENTS_COMPARE[n]` and INTEN before calling 61f4 or 5e9c.
3. **Record 20000f21..f25.** Layout is native: f21 — current N, f22 — pending
   flag, f23 — countdown, f24 — target N, f25 — "first commit" flag
   (functions 6100, 6110, 6df6, 6e94). Hook commit sets f21=f24=N and f22=f23=0,
   this is consistent with native commit in 6e04..6e0a.
4. **Setter.** Length, schema, chip, and ranges mode ≤ 1, N ≤ 40 are checked.
   Enqueueing and response are formed under PRIMASK, and the setter does not write live mode.
   The snapshot for schema2 is atomic. The schema1 response has not changed byte-for-byte.
5. **Absence of redundant transmission before command19.** 61f4 and 62e4 check fe1
   (`20000fd0+0x11`), the hook does not touch this byte and does not start TIMER0.
   Command19 (8efc) sets fe1=1 regardless of the command body.
6. **Connection drop (64ac)** clears the N record via 6110 and stops TIMER0,
   but preserves the mode. The host takes this into account: upon N mismatch, it resends
   command16 of the current profile.
7. **Host.** Setting confirmation (ACK) is never considered proof of
   mode. As long as native pending ≠ 0, the host only repeats the request and does not reset
   the countdown. A budget of 20 s and a timeout of 8 s per request are in effect.
   `validate_remote_mode_grid` checks conservatively, using the alternative window:
   stock 3934.5 = 4727.5−793, frame 2207 = 3000−793. Example N1/N2 at 8800 µs
   arithmetically fits into both windows.

## K05455
Source `D0196`, original line58, SHA256 `61ea3c439c9254004819a5da6a5c13c2ecc12312b89e13855893a1ecffa3afd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Remarks

## K05456
Source `D0196`, original line60, SHA256 `61ea3c439c9254004819a5da6a5c13c2ecc12312b89e13855893a1ecffa3afd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| # | Level | Substance | Recommendation |
|---|---|---|---|
| A1 | Low | Status response (`fe 02`, 26 bytes) does not contain remote pending (`20005151/52`). Native drop 64ac does not clear it. If setter set a transition and before boundary 6c98 connection dropped, the deferred transition is preserved: at e10=0 the hook will cancel it, but if the boundary passes after reconnection, it will be committed. Then mode and N will change after the host has already confirmed and configured them. This will be detected only upon lease renewal after 10 s. The window is narrow, and under unchanged policy the transition matches the desired one, but the host cannot prove "no deferred transition". | Add pending byte to status response (new revision) or clear 8 mode bytes in the 64ac/command9 branch. At minimum record this as a known limitation. |
| A2 | Low | Hook commit does not reproduce native one-time branch f25 (6e0c..6e1a: when f25=0 and `fd0+0x12`, report 8a71 is queued to scheduler 15d4). Currently this is masked by the fact that after switching, the host in CONFIGURING resends command16 (test `test_remote_mode44.py:90`), and then the report is sent natively. | Keep repeated command16 after switching as a mandatory requirement and do not "optimize" it. |
| A3 | Info | After successful return of 57ec, the check `RADIO.STATE==0` is always true, because 57ec returns 0 only after `EVENTS_DISABLED`. In practice there is no "active radio → defer" branch; the wording in the document should be weakened. Furthermore, waiting for `EVENTS_DISABLED` inside 57ec now occurs under PRIMASK: if the radio hangs, nothing will be able to preempt it. Without a transition, this is native behavior, but a higher-priority interrupt could have executed back then. | No code fix required. Record as a hardware unknown. |
| A4 | Info | After commit, TIMER0 remains stopped until the nearest ADDRESS. If host polling does not hit the new RX window, there will be no autonomous fallback transmission (compare0 → 61f4). This is a consequence of the chosen design, and it is covered by the common grid requirement, but tracker behavior without polling in this state has not been hardware-verified. | Verify during the first hardware test. |

## K05457
Source `D0196`, original line69, SHA256 `61ea3c439c9254004819a5da6a5c13c2ecc12312b89e13855893a1ecffa3afd7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

I found no obvious code defects blocking candidate 40941b34…56d5. I independently confirm fixes on the final image from the first two Grok reviews (redundant transmission via 62e4, countdown reset, false gate fe1). A1 and A2 do not block the candidate, but they must either be closed or recorded as known limitations before the OTA decision. Hardware gates remain closed: bootloader and recovery, transition sequence on real RF, interoperability with controllers. The `approved_for_ota=false` flag does not change.

## K05458
Source `D0200`, original line1, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Plan for reading tracker via SWD — single sequence

## K05459
Source `D0200`, original line3, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Revision 11, 2026-10-08. Hardware pause is in effect. The plan does not authorize connection, teardown, or command execution. Specific tracker, probe, tool, and pinout are not yet assigned.

## K05460
Source `D0200`, original line5, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Scope of the stage

## K05461
Source `D0200`, original line7, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Read-only of internal flash, UICR/FICR, and debug access state. CPU continues running. Halt, reset, unlock, recover, ERASEALL, writing to flash/UICR/RAM, loading code, and modifying application peripherals are prohibited. Ready-made Roy/Frame configs must not be run: a separate minimal config with verified dependencies is needed for the tracker.

## K05462
Source `D0200`, original line9, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Read-only means absence of writes to application memory and changes to CPU control, but not absence of service SWD transactions. Necessary DP/AP operations of selection, read addressing, and power requests via DP CTRL/STAT are permitted. **Do not write to DHCSR by default**, do not enable C_DEBUGEN: access via AHB-AP does not require enabling halting debug. C_DEBUGEN may alter BKPT handling and halt the core on assert/error. DEMCR, trace, and vector catch must also not be changed. If a tool requires such writes, stop prior to connecting and separately agree on exact addresses/values and consequences. The name attach does not prove compliance with these restrictions. Verify that debug enable was not left behind by a previous session; if this cannot be established without intervention, record the uncertainty and stop.

## K05463
Source `D0200`, original line11, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Debug power via CxxxPWRUPREQ affects chip behavior: consumption increases, System OFF is emulated; a request to a sleeping chip may wake it up. These are expected limitations even without DHCSR. Do not use this session as proof of normal shutdown or power consumption.

## K05464
Source `D0200`, original line13, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Steps 1–8 after separate permission

## K05465
Source `D0200`, original line15, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Assign instance and physical access.** Record marking, serial number, known chip ID, probe model, tool/version, and whether access with teardown is permitted. Confirm SWDIO/SWDCLK/GND/VTref pinout; do not guess. The tracker is powered from battery, VTref is reference input/measurement only: do not supply power from probe. Prior to connecting, assess the risk of battery short circuit, enclosure damage, and contact stability. One tracker, adjacent devices do not participate.

## K05466
Source `D0200`, original line17, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2. **Verify separate configuration prior to launch.** Do not copy or include Roy/Frame cfgs or scripts. Check all target init, user/project YAML, hooks, and event handlers, including shutdown. For pyOCD, both `connect_mode=attach` and `auto_unlock=false` are mandatory simultaneously. For OpenOCD, `reset_config none` does not prohibit software SYSRESETREQ: separately exclude reset/halt/erase/unlock/program/load and automatic IDE/GDB actions. For J-Link, there must be verified refusal of automatic Unlock/Unsecure/Recover; do not rely solely on confirmation window. `nrfjprog --recover`, `pyocd erase`, and mass erase are prohibited. Check debug register writes according to boundaries above. If an unnecessary nRF52 board is available, verify the exact config on it with logging after permission for such a check is granted. Unknown tool behavior is a reason to stop prior to connection.

## K05467
Source `D0200`, original line19, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

3. **Connect and read access state.** Without reset/halt and writing DHCSR, determine DP/AP identity. Perform power requests CDBGPWRUPREQ and, if required by the selected access, CSYSPWRUPREQ only via verified service DP operations; record which requests are asserted. For a confirmed corresponding Nordic CTRL-AP, read APPROTECTSTATUS, offset `0x00C`: this is an AP register, not a CPU address. Do not write CTRL-AP ERASEALL, reset, or protection control. If identification/address map is not confirmed, stop. On individual revisions of nRF52833, APPROTECT is enabled by default; UICR by itself does not guarantee access.

## K05468
Source `D0200`, original line21, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

4. **In case of protection or error, stop.** Save exact error and safe log. Do not accept Unlock, do not apply recover/mass erase, do not repeat risky operations automatically. Locked access is an acceptable result of this stage.

## K05469
Source `D0200`, original line23, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

5. **If MEM-AP is accessible, confirm map and read landmarks.** Read FICR INFO.PART/VARIANT/CODEPAGESIZE/CODESIZE; do not assume part/flash size from previous notes. Mismatch means stop. For a confirmed nRF52833 map, read UICR NRFFW[0] `0x10001014` and NRFFW[1] `0x10001018`. In Nordic MBR/SDK convention, this is the address of bootloader and MBR parameter page. Save raw values: `0xffffffff` or a non-standard value does not prove the absence of a custom bootloader. Correlation with code is performed later offline.

## K05470
Source `D0200`, original line25, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

6. **Take a local private dump.** Save all internal flash and separately UICR/FICR with base/length, including the area below `0x1000` and metadata `0x2f000`, only if addresses are confirmed by the map. Flash/UICR may contain pairing keys, serial numbers, and calibration. Do not include raw dumps and secret logs in external ZIPs, reports, or agent queries. To the outside: hashes, sizes, and minimal code snippets after verifying absence of secrets. External SPI staging memory is not read in this stage; its content is not confirmed by internal dump. Do not inject code to read it.

## K05471
Source `D0200`, original line27, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

7. **Repeat read and verify consistency.** Compare SHA256 of flash/UICR, record commands, tool/version, addresses, and lengths. In case of difference, do not declare dump golden, do not halt for stability: stop and explain differences offline. CPU is running, so data can potentially be modified by application itself. Also check logs for secrets prior to external transfer.

## K05472
Source `D0200`, original line29, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

8. **Deassert power requests and disconnect.** Using a previously verified procedure, deassert CDBGPWRUPREQ and CSYSPWRUPREQ, if it was asserted, in DP CTRL/STAT; verify completion according to documented handshake of the chosen tool and record the result. Do not write DHCSR/DEMCR and do not perform reset/halt/erase. Simple cable unplugging does not replace deasserting requests. According to nRF52833 specification, deasserting CxxxPWRUPREQ returns the chip to normal mode: mandatory complete de-energization for this is not stated. If deasserting requests is not confirmed, record possible remaining debug mode and increased battery consumption; do not consider normal shutdown confirmed. Even a successful detach is not a measurement of normal current. Disconnecting battery/power removal is a separate action and is not part of this stage. Record state, remove probe; upon anything unexpected, stop and save the log.

## K05473
Source `D0200`, original line31, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## General stop conditions

## K05474
Source `D0200`, original line33, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Unknown pinout or voltage; risk to battery/enclosure/short circuit; unstable contacts/power; chip mismatch; access protection; requirement of reset/halt/unlock/erase or unverified write; read errors; heating or behavior change. Do not attempt to fix the situation by erasing or resetting.

## K05475
Source `D0200`, original line35, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Result and next stage

## K05476
Source `D0200`, original line37, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Result is a local verified dump or recorded access denial. This is not OTA authorization. If code is accessible, next analyze offline reset path, staging copy, length/CRC/signature/version, protected areas, and recovery. Clean reset of the first OTA and hardware recovery are separate stages. OTA prohibition remains until corresponding reasons are closed.

## K05477
Source `D0200`, original line39, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Verified sources

## K05478
Source `D0200`, original line41, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- pyOCD options (original reference: `https://pyocd.io/docs/options.html`) and security (original reference: `https://pyocd.io/docs/security.html`): attach and auto-unlock prohibition are verified together.
- OpenOCD Reset Configuration (original reference: `https://openocd.org/doc/html/Reset-Configuration.html`), CPU Configuration (original reference: `https://openocd.org/doc/html/CPU-Configuration.html`): audit of reset and events is mandatory.
- SEGGER security options (original reference: `https://kb.segger.com/MCU_Security_Options`).
- Nordic nRF52833 product specification v1.4 (original reference: `https://docs-be.nordicsemi.com/bundle/nRF52-Series-PS/raw/resource/enus/nRF52833_PS_v1.4.pdf`): CTRL-AP/APPROTECT/UICR.
- nRF52833 Revision 2, configuration 249 (original reference: `https://docs.nordicsemi.com/r/bundle/errata_nrf52833_rev2/page/err/nrf52833/rev2/latest/config_833_249.html`): "DIF: Access port protection needs software interface configuration". Link verified 2026-10-08; relates to APPROTECT, applicability depends on revision/build code. The workaround specified there is not executed in this stage.

## K05479
Source `D0200`, original line47, SHA256 `1eac49fecc733365225e1be842d6cc2c604da464dc6242b945658d90e19899e3`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Nordic nRF52833 Debug Interface mode (original reference: `https://docs.nordicsemi.com/r/bundle/ps_nrf52833/page/dif.html`): normal mode returns after deasserting CxxxPWRUPREQ; verified 2026-10-08.

## K05480
Source `D0202`, original line1, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Frame Pico Tracker: full plan and project handoff

## K05481
Source `D0202`, original line3, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-06. Working directory: `K:\AI\pico`.

## K05482
Source `D0202`, original line5, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Current execution update

## K05483
Source `D0202`, original line7, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Current plan and continuation point: EXECUTION_STATUS_2026-10-06.md (original reference: `EXECUTION_STATUS_2026-10-06.md`). It takes precedence over the historical inventory below.**

## K05484
Source `D0202`, original line9, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

New ABI4 and radio/cameras/measured pose components are implemented in source code, but the common loop is not yet installed or verified on Frame. 177 offline tests pass; SRAM ABI4 is built at 1928 bytes. Camera-only live verification yielded 1372 IR frames in 15 seconds; source clock is distinguished as MONOTONIC_RAW. New RF loop was not run, real 6DoF in SteamVR has not been obtained. Current work: integration audit B–C–D, completion of IMU calibration/fusion wiring, unified release build, then one joint hardware session. The user was prematurely invited to verification; repeating this before a working verifiable build is prohibited.

## K05485
Source `D0202`, original line11, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 1. Summary at the time of handoff

## K05486
Source `D0202`, original line13, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Three standalone PICO Motion Trackers with working 6DoF in SteamVR are NOT implemented.**

## K05487
Source `D0202`, original line15, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

There are real RF packets and IMU from trackers via the embedded Nordic of Frame, separate limited connections, reconnection of a single device, access to recorded Frame camera images, and an installed SteamVR driver framework. These components do not form a finished system.

## K05488
Source `D0202`, original line17, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Do not consider statements of the previous assistant of "implemented", "ready", "works" as sufficient proof. Verify source code, exact build, raw data, and device states. The assistant repeatedly finished work on an intermediate result contrary to the user's command to continue until ready.

## K05489
Source `D0202`, original line19, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The document is compiled based on current source code, saved results, and the latest read-only check of Frame. Radio tests were not run during compilation. Latest check: `frame-pico-api.service` active, `frame-pico-radio.service` inactive, `pico_tracker` driver registered via vrpathreg, no OpenOCD process visible. This is a snapshot; the next implementer must re-verify the state.

## K05490
Source `D0202`, original line21, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2. Required result and user constraints

## K05491
Source `D0202`, original line23, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Three PICO Swift Motion Trackers connect directly to onboard Roy Nordic Steam Frame.
2. No external dongle. PICO headset and Windows are not needed in the working cycle.
3. Do not reflash Frame firmware/flash. A temporary SRAM module via onboard SWD is permitted.
4. Normal pairing records in the memory of the trackers themselves are allowed. Do not change tracker firmware.
5. Both stock Valve controllers must operate normally at the same time as the trackers.
6. Trackers must provide real poses as GenericTracker in SteamVR. Do not substitute position with zero, constant, synthetics, or unbounded IMU double integration.
7. A new device is discovered in pairing mode; after pairing, normal power-on triggers reconnection while the service is running.
8. Frame Pico Tracker UI: only useful elements, hover, real online devices, charge level when format is confirmed, clear errors, service restart.
9. Closing the window does not stop background services. If a future architecture changes this, confirmation of closure with a warning is required.
10. The user prohibited repeating multiple short radio probes. First finish implementation and offline audit, then one end-to-end hardware verification. If it reveals a defect, the next check only after a specific fix and with the purpose of verifying specifically that.
11. Do not use em dashes in communication. Read AGENTS.md, preserve knowledge in the project.

## K05492
Source `D0202`, original line35, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. Where the materials are located

## K05493
Source `D0202`, original line37, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Path | Purpose |
|---|---|
| `K:\AI\pico\AGENTS.md` | Rules and accumulated log; old sections contradict new ones |
| `K:\AI\pico\pico_tracker_packets_en.md` | External developer document, primary source of protocol scheme |
| `K:\AI\pico\docs\current-status.md` | Historical results, not a single current status |
| `K:\AI\pico\pc\radio\ram\` | Roy SRAM modules, loader, recovery, live state |
| `K:\AI\pico\pc\frame_app\` | Native OpenVR overlay, unit files, device monitor |
| `K:\AI\pico\pc\radio\frame_debug_agent.py` | HTTP API on Frame, camera/diagnostics, radio unit control |
| `K:\AI\pico\pc\radio\frame_occ_live.py` | Incremental OCC reading and empirical metadata |
| `K:\AI\pico\pc\radio\frame_live_camera.py` | Demultiplexing H264 from growing OCC into files |
| `K:\AI\pico\pc\optical\` | Geometry, filters, PnP; integration with live cameras is not complete |
| `K:\AI\pico\pc\driver\steamvr_driver\` | Native OpenVR driver, UDP IPC |
| `K:\AI\pico\outputs\` | Private RF recordings, builds, launch scripts, firmware, calibrations |
| `[USER_HOME]\Documents\antigravity\quick-bohr` | Extracted PICO libraries/firmware/Gemini research |
| `K:\AI\Frame-Pico-Tracker` | Separate copy for public GitHub, not guaranteed to be synchronized |

## K05494
Source `D0202`, original line53, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Research artifacts:
- `K:\AI\pico\pico_tracker_steam_frame_research.md`.
- `[USER_HOME]\.gemini\antigravity\brain\a953ec36-1362-4486-a187-67774da38931\pico_tracker_steam_frame_research.md`.

## K05495
Source `D0202`, original line57, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Git working copy contains many uncommitted and untracked files. Do not reset/restore and do not publish all outputs. Firmware, system libraries, raw recordings, individual IDs and calibrations are not included in the permitted "our code only" publication.

## K05496
Source `D0202`, original line59, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 4. Access to Frame

## K05497
Source `D0202`, original line61, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- SSH: `ssh steamos@[DEVICE_IP]`, previously without password.
- Working folder on Frame: `/home/steamos/pico-ram-work`.
- Python: `/home/steamos/pico-ram-work/venv/bin/python`.
- OpenVR: `/opt/steamvr/bin/linuxarm64`, headers in `frame-mic-reference/third_party/openvr` and `driver-build`.
- Native UI: `/home/steamos/.local/bin/FramePicoTracker`.
- HTTP API: `http://127.0.0.1:18741/status`, local listener. SSH tunnel is used for Windows.
- API unit: `~/.config/systemd/user/frame-pico-api.service`, enabled, Restart=on-failure.
- Radio unit: `~/.config/systemd/user/frame-pico-radio.service`, currently only 10-second diagnostics, Restart=no, not a ready permanent station.
- Driver: `/home/steamos/pico-ram-work/steamvr-driver/bin/linuxarm64/driver_pico_tracker.so`.
- Camera recordings: `~/.local/share/Steam/config/cv/xrservice/datasets/*.dataset/*.occ`.
- USB ADB Frame: previously `adb -s frame shell`, root; no TCP ADB Frame. Wi-Fi operation via SSH.
- PICO: previously USB serial `[HEADSET_ADB_SERIAL]`, root obtained by user. PICO root is not needed in the final runtime. Do not include it as a pose source.

## K05498
Source `D0202`, original line74, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 5. What is really confirmed

## K05499
Source `D0202`, original line76, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 5.1 Roy and SRAM execution

## K05500
Source `D0202`, original line78, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Roy is identified as nRF52833, stock build1788551713, Zephyr/MPSL/SoftDevice Controller. This is NOT confirmation that stock controllers use ESB.

## K05501
Source `D0202`, original line80, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Built-in Linux GPIO SWD is accessible via stock OpenOCD config:
`/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg`.

## K05502
Source `D0202`, original line83, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Native RTT shell allows calling SRAM entry from the stock thread, without debugger halt on each entry. A temporary MPU region allows SRAM execution. MPSL provides radio slots, RX/TX operate inside callback.

## K05503
Source `D0202`, original line85, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Fixed map:
- CODE `0x2001151C`, limit2045 bytes.
- DATA `0x20010C18`, 2048 bytes.
- RTT `0x2001064C`.
- MPSL context `0x2001056C`, active is determined by byte+0x26, not the whole structure.
- Shell hook `0x2000D8CC`.
- MPU static word `0x20010A24`, byte+2 temporarily2->3.
- Private peer table DATA+1504, native marker+1600, count+1604, heartbeat budget+1608.
- Ring4 records of280 bytes each, DATA+380; DMA258 bytes.

## K05504
Source `D0202`, original line95, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Loader is tied to stock SHA256:
`0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50`.

## K05505
Source `D0202`, original line98, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

After limited successful runs, confirmed closure of MPSL, restoration of SRAM/RTT by reading and MPU. The module must not be removed while callback is alive.

## K05506
Source `D0202`, original line100, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 5.2 Radio data

## K05507
Source `D0202`, original line102, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Capture | Observation | Limitation |
|---|---|---|
| 245 | IDs of all three devices in bootstrap packets | Not full poses |
| 249 | 915slots,387RF records,380sum8-valid replies;204/212/192IMU samples across three profiles | 9seconds, only HMD+left controller pose evidence |
| 252 | 160slots,39records,54IMU samples, increased interval20ms | One responding profile, only HMD in recording |
| 254 | Reply of thirdID on personal address/channel78, then IMU on2 | Short-term transition, not a stable reconnect |
| 258 | 3seconds,34records,29samples, replies up toslot159 | HMD/left48/48valid,right45/48valid |
| 260 | 9seconds after short wake,56records,51samples | Both controllers106/106valid |
| 261 | Repeat without new wake,88records,77samples | Right108/111valid,left111/111valid |

## K05508
Source `D0202`, original line112, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

During research, PICO was turned off according to the user's message. This is proof of a direct radio source in these sessions, not a finished autonomous system.

## K05509
Source `D0202`, original line114, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Do not confuse the latest remote259 files with the first unsuccessful259: successful results are saved locally as `outputs/connection260.json`, `connection261.json` and corresponding `.poses.csv`.

## K05510
Source `D0202`, original line116, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 5.3 Known private IDs

## K05511
Source `D0202`, original line118, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Historical connection with SN | LE32 ID | LE bytes | Derived BASE / prefix |
|---|---|---|---|
| [TRACKER_SERIAL] | [TRACKER_ID] | [TRACKER_ID] | [PRIVATE_LINK_VALUE] /00 |
| [TRACKER_SERIAL] | [TRACKER_ID] | [TRACKER_ID] | 0xEC4C66FB /80 |
| [TRACKER_SERIAL] | [TRACKER_ID] | [TRACKER_ID] | [PRIVATE_LINK_VALUE] /40 |

## K05512
Source `D0202`, original line124, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Verify the historical binding to physical device using product-info, not user's "first/third" naming. IDs were obtained from radio producer layout, not from JSON, and were not restored entirely by CRC.

## K05513
Source `D0202`, original line126, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Seed of the used station is defined by bytes `74 73 54 15`, LE integer0x15547374. Do not confuse with[PRIVATE_LINK_VALUE] as text representation of bytes.

## K05514
Source `D0202`, original line128, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Previously suggested0xEEACED3E/0xEEADDFC3/0xEEAE1DAD were unconfirmed reconstructions and are incorrect. Lower word of the second: BFC3, not DFC3.

## K05515
Source `D0202`, original line130, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 5.4 Cameras

## K05516
Source `D0202`, original line132, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

OpenVR HasCamera returned true, but CameraFrameSize error100 and0sizes. Raw video was not obtained via this interface.

## K05517
Source `D0202`, original line134, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Stock XRService creates OCC datasets. Extracted from them were4H264 streams of1056x1024 and2streams of640x480. ffmpeg decoded IR gray frame of1081344bytes.

## K05518
Source `D0202`, original line136, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The last parsing of264 read187185108bytes and3243chunks/stream, but included old part of the recording. **This is not proof of obtaining3243new frames in8seconds and not a measurement of live-loop latency.**

## K05519
Source `D0202`, original line138, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In OCC, field17selector, field15cameraID, field3double time, field4double exposure, field55sensoridentifier are empirically identified. Their relation to sourceCamera and exact time units require verification. The assistant's assertion "there is no new recording" was an error of searching only for a new folder: the recording already existed after wake-up.

## K05520
Source `D0202`, original line140, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 5.5 UI and driver
