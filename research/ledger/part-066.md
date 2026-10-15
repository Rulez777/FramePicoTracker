# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05201
Source `D0179`, original line42, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**138 passed, 8.96s**, JUnit outputs/fw189-iteration07-tests.xml. Suite: new lossyrefresh/missing7e/native pool tests + dualv2 ARM/protocol/OTA, peer engine, ABI 4 optics integration, OTA runtime, and readiness hardware-gate regressions. The final tiny edit of the expiry text after this run did not change transitions/conditions; formatting is verified by the saved f-string.

## K05202
Source `D0179`, original line44, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Bootloader/recovery, clean-start pending worker ACK race, heap worst case, physical gesture/RTC/RX400us/coexistence are not closed. Firmware was not rebuilt; the iter05 package stores historical host code and does not match the new temporal defaults. Current sources and this report are the up-to-date host revision 07.

## K05203
Source `D0180`, original line1, SHA256 `e2174ebdaba6aa0b62bb97bfa252b14989f9c31667d413de033b1d6a99619662`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 08 — achievable second mode re-check

## K05204
Source `D0180`, original line3, SHA256 `e2174ebdaba6aa0b62bb97bfa252b14989f9c31667d413de033b1d6a99619662`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. The reviewer is right: in revision 07, backoff 10s after timeout at t0+20 fell into t0+30, where lease expiration previously occurred. A second chance was absent under defaults. Only host sources/tests have been fixed; firmware97320/SHA395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f was not changed.

## K05205
Source `D0180`, original line5, SHA256 `e2174ebdaba6aa0b62bb97bfa252b14989f9c31667d413de033b1d6a99619662`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

New defaults: refresh 10s, query timeout 8s, retry backoff 2s, lease 30s. Nominal path: query t0+10; timeout t0+18; retry with new nonce/sequence t0+20; reception until t0+28. If the pipeline is busy, less room before lease may remain: the deadline is intentionally not extended without a response. Guaranteed 2–3 attempts under any delays are not promised. RF liveness 3s is retained.

## K05206
Source `D0180`, original line7, SHA256 `e2174ebdaba6aa0b62bb97bfa252b14989f9c31667d413de033b1d6a99619662`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The new test_first_refresh_times_out_second_succeeds_before_lease verifies: first attempt timeout, unconfirmed old nonce is not accepted, sequence is incremented, second begins at t20, response at t27.9 preserves STREAMING/phase and refreshes proof from t20. At t30 the connection is not dropped. **139 passed, 8.86s**; JUnitoutputs/fw189-iteration08-tests.xml.

## K05207
Source `D0180`, original line9, SHA256 `e2174ebdaba6aa0b62bb97bfa252b14989f9c31667d413de033b1d6a99619662`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Reported reviewer attributes b97 (GATT/bond manager, other sensors/addresses) were not independently re-verified in this iteration. b97 is still not used as FW189 bootloader/recovery.

## K05208
Source `D0180`, original line11, SHA256 `e2174ebdaba6aa0b62bb97bfa252b14989f9c31667d413de033b1d6a99619662`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The hardware read plan is prepared in docs/FW189_SWD_READONLY_PLAN_2026-10-08.md. There is no connection/commands to the hardware. Hardware gate, whitelist, and pause are maintained.

## K05209
Source `D0181`, original line1, SHA256 `ee54c698683a464c72903227611db805703fbd8d94ec39203b37a21b83099801`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration09 — refinement of read-only SWD plan

## K05210
Source `D0181`, original line3, SHA256 `ee54c698683a464c72903227611db805703fbd8d94ec39203b37a21b83099801`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. Only documents were modified. Firmware/host code/whitelist/hardware gate were not changed. Tests were not re-run: there are no runtime changes. Reviewer's139passed and unchanged SHA are external confirmation, not a new run of this iteration.

## K05211
Source `D0181`, original line5, SHA256 `ee54c698683a464c72903227611db805703fbd8d94ec39203b37a21b83099801`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Added to the SWD plan are an audit of automatic reset/unlock, pyOCD attach+auto_unlock=false, reset_config none restriction, CTRL-AP APPROTECTSTATUS, reading UICR NRFFW0/1, dump privacy, battery/VTref/enclosure, and external SPI staging boundary. Official pyOCD/OpenOCD/Nordic documents were verified, links are placed in the plan.

## K05212
Source `D0181`, original line7, SHA256 `ee54c698683a464c72903227611db805703fbd8d94ec39203b37a21b83099801`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Two significant clarifications: attach without auto_unlock=false is insufficient; NRFFW values are useful for searching, but do not prove the presence/absence of a custom bootloader. J-Link unlock prompt is not considered guaranteed protection against auto-erase for any frontend/version.

## K05213
Source `D0181`, original line9, SHA256 `ee54c698683a464c72903227611db805703fbd8d94ec39203b37a21b83099801`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

No tools were connected to the hardware. Specific target/debugger/pinout have not been selected yet. User hardware pause is in effect. The next stage requires a separate user command for reading; erase/recover/unlock/firmware writes are not in scope.

## K05214
Source `D0182`, original line1, SHA256 `db35441983f7228d762cc68cf866c653b2ac3eb7b65043d5a6426718e8f23429`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 10 — unified operator SWD plan

## K05215
Source `D0182`, original line3, SHA256 `db35441983f7228d762cc68cf866c653b2ac3eb7b65043d5a6426718e8f23429`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. Documents only; hardware pause is maintained.

## K05216
Source `D0182`, original line5, SHA256 `db35441983f7228d762cc68cf866c653b2ac3eb7b65043d5a6426718e8f23429`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Plan FW189_SWD_READONLY_PLAN_2026-10-08.md has been rewritten into a single sequence of steps 1–8: attach/auto_unlock settings, protection, UICR, privacy, and physical constraints are located directly in the executable steps. The old alternative sequence no longer exists.

## K05217
Source `D0182`, original line7, SHA256 `db35441983f7228d762cc68cf866c653b2ac3eb7b65043d5a6426718e8f23429`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

It is explicitly forbidden to use or include ready-made Roy/Frame cfg/scripts. A separate verified minimal config is required for the tracker.

## K05218
Source `D0182`, original line9, SHA256 `db35441983f7228d762cc68cf866c653b2ac3eb7b65043d5a6426718e8f23429`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Service DP/AP writes and application memory are separated. Minimal debug enable via DHCSR is permitted by future approval only without halt/step/masking and after verification of exact values. DEMCR/trace/vector catch are prohibited by default; necessity requires separate coordination. The term attach by itself does not permit arbitrary debug writes.

## K05219
Source `D0182`, original line11, SHA256 `db35441983f7228d762cc68cf866c653b2ac3eb7b65043d5a6426718e8f23429`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The official Nordic page was checked: configuration 249 indeed refers to APPROTECT nRF52833 Revision 2. The link, name, and applicability limitation are retained in the plan. Workaround is not executed.

## K05220
Source `D0182`, original line13, SHA256 `db35441983f7228d762cc68cf866c653b2ac3eb7b65043d5a6426718e8f23429`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Code, image, whitelist, and readiness gate are unchanged. Tests were not run: changes are strictly documentational. There were no connections, opening of enclosures, SWD/SSH/RF/OTA, or actions with devices. Specific tracker/probe/tool/pinout have not been assigned yet; there are no ready run commands.

## K05221
Source `D0183`, original line1, SHA256 `ac0810360cd2e99a93f31041cfff5757a143bff0cfcece923652ef9859555fd4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 11 — DHCSR and SWD completion

## K05222
Source `D0183`, original line3, SHA256 `ac0810360cd2e99a93f31041cfff5757a143bff0cfcece923652ef9859555fd4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. Documentation only, hardware pause is in effect.

## K05223
Source `D0183`, original line5, SHA256 `ac0810360cd2e99a93f31041cfff5757a143bff0cfcece923652ef9859555fd4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

DHCSR write is prohibited by default in the plan; C_DEBUGEN is not enabled. If the selected tool requires DHCSR/DEMCR, the connection is halted until separate coordination. Service DP/AP operations and debug power are distinguished from halting debug. Step 3 preserves the list of asserted power requests.

## K05224
Source `D0183`, original line7, SHA256 `ac0810360cd2e99a93f31041cfff5757a143bff0cfcece923652ef9859555fd4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Step 8 requires deasserting CDBGPWRUPREQ and CSYSPWRUPREQ, if asserted, and documenting completion. The official nRF52833 specification was verified: normal mode returns after deasserting CxxxPWRUPREQ. Therefore, the statement that debug mode inevitably persists until power is completely removed was not transferred to the plan. If detach is not confirmed, possible increased battery consumption is recorded as an uncertainty. Power down and battery disconnection remain out of scope for this stage.

## K05225
Source `D0183`, original line9, SHA256 `ac0810360cd2e99a93f31041cfff5757a143bff0cfcece923652ef9859555fd4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Source: https://docs.nordicsemi.com/r/bundle/ps_nrf52833/page/dif.html (Debug Interface mode section, verified 2026-10-08). System OFF in debug mode is emulated; verification of standard consumption/power-off is not performed in this session.

## K05226
Source `D0183`, original line11, SHA256 `ac0810360cd2e99a93f31041cfff5757a143bff0cfcece923652ef9859555fd4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Code, image, whitelist, and gate were not changed. Tests were not run. Connection, opening of enclosures, and hardware operations were not performed. Tracker/probe/tool/pinout have not been assigned.

## K05227
Source `D0184`, original line1, SHA256 `f7b7c8951267e7a045211fc3e0192a3b077fe9fbe98d9c535b192ca52084fdf7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 12 — passive radio capture via Frame

## K05228
Source `D0184`, original line3, SHA256 `f7b7c8951267e7a045211fc3e0192a3b077fe9fbe98d9c535b192ca52084fdf7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. The user explicitly authorized radio investigation; this is not authorization for OTA, erase, reset, or unknown commands to the tracker.

## K05229
Source `D0184`, original line5, SHA256 `f7b7c8951267e7a045211fc3e0192a3b077fe9fbe98d9c535b192ca52084fdf7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

USB: three Windows PnP reads yielded identical 39 USB/HID/Ports devices; disconnecting the tracker changed nothing. This proves the absence of observable USB enumeration in this configuration, not the absence of USB data in hardware.

## K05230
Source `D0184`, original line7, SHA256 `f7b7c8951267e7a045211fc3e0192a3b077fe9fbe98d9c535b192ca52084fdf7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

SSH steamos@[DEVICE_IP] is reachable. Radio service is failed/inactive, no loader/OpenOCD process prior to investigation. Read-only tracking_watch showed HMD valid and both controllers disconnected/invalid; not proof of compatibility with active controllers. Monitor launched with --help (it outputs a stream instead of help) was stopped by exact process matching.

## K05231
Source `D0184`, original line9, SHA256 `f7b7c8951267e7a045211fc3e0192a3b077fe9fbe98d9c535b192ca52084fdf7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

One passive capture performed: existing roy_ram_loader.py and roy_passive.elf, stock.bin, fixed1-195.profile.json; RX-only/native-shell/reserve-mpu/repeat-host, address BASE[PRIVATE_LINK_VALUE]/prefix0, channels 1,2,6,8,10,12,37,77, 600us slots, 10000us interval, 10s. This is the address of the first previously identified tracker; bootstrap and other addresses are not covered. TX was not enabled.

## K05232
Source `D0184`, original line11, SHA256 `f7b7c8951267e7a045211fc3e0192a3b077fe9fbe98d9c535b192ca52084fdf7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Result: 42 slots, 39 host retries, 0 raw records. session_closed_verified, ram_restored_verified, mpu_restored, unloaded — true. No error in report, flash_written is verified in saved raw JSON. Zero reception does not mean tracker absence: short duty cycle, address, sleep/search mode, and power state limit the conclusion.

## K05233
Source `D0184`, original line13, SHA256 `f7b7c8951267e7a045211fc3e0192a3b077fe9fbe98d9c535b192ca52084fdf7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Preliminary runs did not reach the radio: sudo -n required a password; regular loader rejected duration15 by limit <=10. The working run was performed without sudo. Service was not restarted. Images were not flashed. Fresh evidence: outputs/radio-sniff-20261008.json; remote log/stdout in /home/steamos/pico-ram-work/radio-sniff-20261008-*.

## K05234
Source `D0184`, original line15, SHA256 `f7b7c8951267e7a045211fc3e0192a3b077fe9fbe98d9c535b192ca52084fdf7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Passive radio does not provide a bootloader dump automatically. The next useful check requires confirmation that the required tracker is powered on/awake, followed by a single capture of the appropriate address/state. Do not repeat an unchanged short probe in series. Standard command for reading arbitrary flash has not yet been found; OTA gate is maintained.

## K05235
Source `D0185`, original line1, SHA256 `29e04e7155d3a763102c4ebf8f82ab58873d2a8d88c2064d3644b49bb2ad8d77`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 13 — passive capture after power-on

## K05236
Source `D0185`, original line3, SHA256 `29e04e7155d3a763102c4ebf8f82ab58873d2a8d88c2064d3644b49bb2ad8d77`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. User confirmed tracker power-on. One limited RX-only capture via SSH on Frame, without TX/OTA.

## K05237
Source `D0185`, original line5, SHA256 `29e04e7155d3a763102c4ebf8f82ab58873d2a8d88c2064d3644b49bb2ad8d77`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Existing roy_passive.elf and loader: bootstrap205.profile.json, BASE0=0x552c6a1e/prefix0=0xc0, channels1/37/77, 600us slot, 10000us interval, duration10s, native-shell/reserve-mpu/repeat-host. Address change is justified by search state after power-on; identity of the powered-on physical tracker is unknown.

## K05238
Source `D0185`, original line7, SHA256 `29e04e7155d3a763102c4ebf8f82ab58873d2a8d88c2064d3644b49bb2ad8d77`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

46 granted slots,39 host retries,0 raw records. ADDRESS=0,END=0,CRC errors=0; this is the absence of observed address match in the provided windows, not CRC rejection and not proof of absence of transmission outside the windows. flash_written=false; session_closed_verified/ram_restored_verified/mpu_restored/unloaded=true.

## K05239
Source `D0185`, original line9, SHA256 `29e04e7155d3a763102c4ebf8f82ab58873d2a8d88c2064d3644b49bb2ad8d77`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Evidence outputs/radio-wake-20261008.json; remote log/stdout radio-wake-20261008-* in /home/steamos/pico-ram-work. Image and host source were not changed, tests were not run. Normal communication/bootloader dump/controller compatibility are not proven.

## K05240
Source `D0185`, original line11, SHA256 `29e04e7155d3a763102c4ebf8f82ab58873d2a8d88c2064d3644b49bb2ad8d77`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Reception occupies a small fraction of time and tracker may listen instead of transmitting. A further series of identical passive probes is not justified. The next task is to verify existing limited identity/no-op exchange and read options in stock dispatcher offline before transmitting requests. OTA gate is maintained.

## K05241
Source `D0186`, original line1, SHA256 `cc8564298db5917baa5d893ebd390403232d899b69f1d2499eb76676a7985e1a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 14 — limited identity exchange

## K05242
Source `D0186`, original line3, SHA256 `cc8564298db5917baa5d893ebd390403232d899b69f1d2499eb76676a7985e1a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. User authorized verified active exchange. Native path of length9 in FW189 0x68f0..0x691a was checked against saved disassembly: when paired marker !=AAAA it calls0x6844; when AAAA it silently returns. This is not a flash read command. Zero9 bytes do not contain settings17/21, pairing offer, or OTA subcommand25. Modification of RAM/radio state by the handler itself is possible; the request is not claimed to be completely side-effect free.

## K05243
Source `D0186`, original line5, SHA256 `cc8564298db5917baa5d893ebd390403232d899b69f1d2499eb76676a7985e1a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Used saved roy_fast_identity.elf (1927B, SHA c1f63fba5ef4c2e0a972c2e784efb17d2e06288ec854b9da3e2faea06ffb92cc), stock.bin SHA0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50; bootstrap BASE552c6a1e/prefixc0,1/37/77, PID rotation, native fast TX->RX, max40 polls. Existing loader requires2000us slot; used only with both controllers disconnected, checked fresh tracking_watch baseline and monitored throughout. Uploaded wrapper radio_identity14.py records poses, refuses connected-controller baseline and sends SIGINT if controller connects or monitor becomes stale. guard_interrupted=false, exit0. Duration10s,interval10000us; no service restart.

## K05244
Source `D0187`, original line1, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 15 — standard FW189 commands and memory read capability

## K05245
Source `D0187`, original line3, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. Offline analysis only; radio/SSH/ADB/OTA were not run in this iteration. Firmware image, production code, whitelist, and OTA gate were not changed.

## K05246
Source `D0187`, original line7, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**In the disassembled standard FW189 entry points, no request for reading arbitrary internal flash or bootloader was found.** Fixed RAM readbacks and a consuming external SPI log were found, but none of them provides a bootloader dump. Completeness of the reconstructed table does not constitute proof of the absence of any hidden/undisassembled protocol, callback, or vulnerability.

## K05247
Source `D0187`, original line9, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Brute-forcing opcodes/addresses on a live tracker is not justified: here there are reset, OTA erase/write, settings, idle, and a consuming SPI getter. The name Get/Read by itself does not imply the absence of writing.

## K05248
Source `D0187`, original line11, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## How the table was reconstructed

## K05249
Source `D0187`, original line13, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

From frozen stock outputs/swift189-image.bin SHA477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89, native init0x901c was executed in Unicorn; RAM20001868 was dumped, 31 records of 16B each. Raw first 8 bytes and words +8/+12 were preserved. Inbound dispatcher0x94fc..958a: opcode0 terminates the stream, >0x1e does not call the table handler, callback is taken from+12; when flags[0]==0, it executes via scheduler15d4, otherwise directly. Aux+8 is used by a separate ACK/event path0x8d6c. Do not consider aux an inbound memory-read handler.

## K05250
Source `D0187`, original line15, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Command1 wrapper8d16 copies body to stack and calls8c10; 8c10 compares the first byte with 13 records of ROM16528..16590 (step 8B), passes code to r0 and body+1 to r1. Separate bootstrap length9 path68f0 is not part of this table; it responds only when marker!=AAAA. Length17/21 changes connection/parameters; identity silence of paired tracker is explainable by this gate, but the actual marker of the live instance was not read.

## K05251
Source `D0187`, original line17, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## All entries of main input

## K05252
Source `D0187`, original line19, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Opcode (decimal) | Callback +12 | Aux +8 | Verified meaning / limitations |
|---|---|---|---|
| 0 | 0x0 | 0x0 | Terminates stream parsing; not a request |
| 1 | 0x8d16 | 0x8d15 | 8d16 → 8c10: dispatch of 13 subcommands; OTA/reset/settings and fixed data |
| 2 | 0x0 | 0x0 | No inbound callback |
| 3 | 0x0 | 0x0 | No inbound callback |
| 4 | 0x8d6c | 0x0 | 8d6c: selects callback +8 by record number <=30, queues via 15d4; this is ACK/event dispatch |
| 5 | 0x900c | 0x0 | 900c: bx lr, empty handler |
| 6 | 0x8e34 | 0x0 | 8e34: packs fixed 6 body bytes, d890; state/subsystem modification |
| 7 | 0x0 | 0x0 | No inbound callback |
| 8 | 0x8f48 | 0x0 | 8f48: channel mask and radio RAM, calls7508 |
| 9 | 0x8d88 | 0x0 | 8d88: chip ID check, 78f4/7c44; under conditions605c or6588, modifies radio/state |
| 10 | 0x8df0 | 0x0 | 8df0: when59cc==0 passes input 32-bit value79b8 (synchronization) |
| 11 | 0x0 | 0x0 | No callback; special ACK path94dc → 97f4 |
| 12 | 0x8e06 | 0x0 | 8e06 →7060: one-shot timer/state setter |
| 13 | 0x8e14 | 0x0 | 8e14 →73c0: block state machine, input blocks are copied into fixed RAM; downstream715c/7150, not a safe getter |
| 14 | 0x8e0a | 0x0 | 8e0a →7af4: three bytes configure synchronization mode/state; ff88 events |
| 15 | 0x0 | 0x0 | No inbound callback; outbound queue path93a8 |
| 16 | 0x8f0c | 0x0 | 8f0c →6100: interval/decimation and switch counter |
| 17 | 0x8ee0 | 0x0 | 8ee0 →d500: IMU work mode, task/event d4b0 |
| 18 | 0x8f74 | 0x0 | 8f74 →6030: enter_idle_mode and event/state modifications |
| 19 | 0x8efc | 0x0 | 8efc: flag20000fe1 and aca4 |
| 20 | 0x93c8 | 0x0 | 93c8: enqueues a fixed one-byte response, not a dump |
| 21 | 0x8d40 | 0x0 | 8d40: chip ID check, 78f4/7c44, radio/state modification |
| 22 | 0x0 | 0xacb5 | No inbound callback; aux acb4, not to be confused with handler +12 |
| 23 | 0x0 | 0x0 | No inbound callback |
| 24 | 0x0 | 0x8fed | No inbound callback; aux8fec, modifies state on ACK/event |
| 25 | 0x8e18 | 0x0 | 8e18: set_irled_onoff_mode in20001080 |
| 26 | 0x0 | 0x0 | No inbound callback; may be an outbound record type |
| 27 | 0x0 | 0x0 | No inbound callback |
| 28 | 0x8e84 | 0x0 | 8e84: limits length <=15, data wrapper in d890; not a flash read |
| 29 | 0x0 | 0x0 | No inbound callback |
| 30 | 0x8f2c | 0x0 | 8f2c →8640: received_charge_poweron writes fixed RAM flag |

## K05253
Source `D0187`, original line53, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## All subcommands of command 1

## K05254
Source `D0187`, original line55, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Subcommand | Callback | Verified meaning / limitations |
|---|---|---|
| 0x2 | 0x89d0 | 89d0: selection of 32B records of fixed RAM20001bb0; response is 34B. Index0 passes and reads previous record20001ba8; not arbitrary flash. Magic gates. |
| 0x12 | 0x877c | 877c: response of 6B from b690/81d0, hardware/product info, without input address. |
| 0x29 | 0x89a4 | 89a4 →9c78: response of 35B from fixed 34B mailbox20005bde; some branches consume/clear the mailbox. |
| 0xc | 0x8918 | 8918: ACK, delay; body==1 state event9a94, otherwise AIRCR SYSRESETREQ. NOT a getter, was not sent. |
| 0x13 | 0x8754 | 8754: modifies flag20000fcc, ACK1B. |
| 0x14 | 0x88d2 | 88d2 →102dc/1002c: SPI/queue log; 102dc reads 40hex bytes from 0x40000+RAM cursor and writes the first 4 bytes back via d130, advances cursor. NOT read-only and NOT an OTA staging bank getter. |
| 0x25 | 0x88cc | 88cc →fbcc: OTA start/data/finish, erase/write staging, possible activation. Forbidden to run. |
| 0x30 | 0x8860 | 8860 →e168: input 17B modify fixed calibration RAM200005a4; ACK and record1a with the same data. |
| 0x31 | 0x8680 | 8680: set independent, subsystem callsc82c/7968/dab8/c748, ACK2B. |
| 0x2f | 0x8724 | 8724: recv body info, log of input byte and fixed ACK2B. |
| 0x50 | 0x8814 | 8814: RAM20001d45 and a144/1918, ACK2B; setter. |
| 0x51 | 0x87d4 | 87d4: ACK, delays, indirect callback20001074 and b5c0; storage/restart consequences not proven safe. |
| 0x52 | 0x86d4 | 86d4: set power off time; 16-bit input sets2000009c relative to20000098; ACK2B. |

## K05255
Source `D0187`, original line71, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Verification of fixed reading

## K05256
Source `D0187`, original line73, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Executed outputs/fw189-command-audit15/audit.py with .venv Python and PYTHONPATH=project root. It rebuilds the table, verifies31 handlers with expected addresses, extracts13 subcommands, and saves annotated disassembly.

## K05257
Source `D0187`, original line75, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Sub02 was run for **all256 index byte values** with sentinel RAM and valid magic. Indices0..12 return34B with32B of strictly corresponding RAM record; 13..255 do not respond. Index0 has an underflow of one record, but the source is still fixed RAM, not bootloader. This is an offline finding, not a way to obtain an arbitrary address and not authorization of malformed RF requests.

## K05258
Source `D0187`, original line77, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

9c78 was executed separately with markerAA00: exactly 34B of the mailbox was copied to output, the source was then zeroed out. For this check, the stack was moved 20005000 so as not to overlap the native mailbox near 20005c00. There are other marker branches; identical clearing across all branches is not claimed.

## K05259
Source `D0187`, original line79, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The Unicorn model intercepts response queue/logging/SPI; this is a check of native application instructions, not proof of operation of RF, bootloader, or physical memory. The previous full pytest suite was not rerun, production code was not changed. Assertion results are in checks.json.

## K05260
Source `D0187`, original line81, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What was checked in PICO

## K05261
Source `D0187`, original line83, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In outputs/pico4e/libtrackerclient.pxr.so, libstationclient.pxr.so, and libswift.so, .dynsym/.symtab were extracted, and defined/imported symbols were checked separately. The saved outputs/pico4e/root/stationservice was also checked. Symbol lists and AArch64 disassembly candidates were saved; string searching in itself is not considered proof of the absence of an API.

## K05262
Source `D0187`, original line85, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Candidate | Result |
|---|---|
| GetControllerHwInFlash(type) | Client wrapper libtrackerclient VA37124 accepts type and returns scalar status from Binder; there are no address/length/output dump parameters. Specific server meaning is not fully analyzed; tracker flash reader is not declared. |
| GetTrackerOfflineInfo(type,out) | Wrapper VA3cab4 copies 16B of status from Parcelable, not a bootloader image. |
| GetTrackerCalibration(type,out) | Wrapper VA3be64 copies fixed 0x294B calibration structures from Parcelable. Not an arbitrary memory read. |
| RssiDebug(int) | Wrapper VA3926c passes int and returns scalar; debug setter/query, no buffer for flash dump. Was not run. |
| DumpStaTrackPointThread | Entry and AArch64 body were examined, name refers to track-point thread; interface address/length for tracker flash was not found. Full data-flow of all virtual calls is not proven. |
| UpdateFWTracker(type,path) | Wrapper VA382d0 reads input file and passes update via Binder. This is an OTA upload, not download/readback. |
| libswift JNI updateFw/checkFwUpdate | Update callbacks and update check; exported functions do not provide a memory getter. |
| stationservice symbols/strings | Binder proxy/default functions and track-point dump found. Separate standard bootloader download is not established; stripped server implementations/other .so are not excluded. |

## K05263
Source `D0187`, original line96, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What can realistically be done next

## K05264
Source `D0187`, original line98, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

With this set of confirmations, there is no justified new command worth sending for the sake of the bootloader. Communication via the saved personal startup address is a separate task of obtaining routine telemetry, not a path for reading flash. Do not run it under the guise of a dump.

## K05265
Source `D0187`, original line100, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A path without a physical programmer that can still be explored offline: **full PICO OS packages/factory updaters and StationService server implementations**, where a separate loader artifact or diagnostic protocol might turn up. The presence of a loader artifact still needs to be linked with the exact tracker hardware and FW189; it does not prove recovery. In the current FW189 container there are two app copies, there is no bootloader of the 0..fff region (verified earlier 06).

## K05266
Source `D0187`, original line102, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Adding a dump command to our fork does not remove the blocker of the initial installation: first, an unverified image would have to be flashed. A bootloader without app-independent recovery remains unverified. The Reset subcommand 0c is an interesting standard path for a separate analysis of a clean startup, but it alters execution and was not sent in this study.

## K05267
Source `D0187`, original line104, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A new radio probe is justified only after establishing a specific new standard readback handler or a separate agreed-upon plan for a normal connection. Serial enumeration has been discontinued.

## K05268
Source `D0187`, original line106, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Artifacts

## K05269
Source `D0187`, original line108, SHA256 `f0a8b2a5feecb4da8fe2ec29a0131aa53d975d1ae854d693190b7a8ca6e9a9af`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

outputs/fw189-command-audit15/: table.json, subcommands.json, checks.json, audit.py, native-handler-ranges.asm, full symbols JSON and candidate ASM/filtered strings across libraries. Hashes of source binaries are in inputs.json. External agent requests were not performed; private radio captures/keys were not included in this directory.

## K05270
Source `D0188`, original line1, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 16 — PICO server, updaters and separate bootloader

## K05271
Source `D0188`, original line3, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. Static analysis of preserved PICO binaries; firmware/code/whitelist/readiness gates are unchanged. Radio, SSH/ADB, and device commands were not run. No reset, wipe, or OTA.

## K05272
Source `D0188`, original line7, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The actual StationService server in **outputs/pico4e/libstationopticsservice.so** was found and disassembled, not just Binder client proxies. This closes several false candidates: BootromCmdDownload transmits data to IN618; DirectCmd forwards the station packet; GetControllerHwInFlash returns a scalar. None of these examined paths reads the Nordic Swift2.0 bootloader.

## K05273
Source `D0188`, original line9, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A separate FW189 bootloader was not found in the examined local updaters/service binaries or in the saved FW189 container. This is a negative result of a specific search, not proof of the absence of a loader in the device or in all PICO OS packages.

## K05274
Source `D0188`, original line11, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Sources and method

## K05275
Source `D0188`, original line13, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In addition to 15, station_upgrade, root/stationservice, libcontroller_service.so, libpxrcontrollerservice.so, and libstationopticsservice.so were examined. Symbols/strings, DT_NEEDED, and AArch64 candidate ranges were extracted. In libstationopticsservice, the PLT corresponds to a 32B header +750entries*16B (section size12032), calls were resolved via .rela.plt/.dynsym and saved in plt.json. In the final ASM unresolved/internal, only unresolved addresses remain; names must not be assigned based on conjecture.

## K05276
Source `D0188`, original line15, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Specific paths

## K05277
Source `D0188`, original line17, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Function / VA | What it does | Conclusion |
|---|---|---|
| StationService::SetTrackerDirectCmd, 0x7b6d8 | When field0x34ec==0, copies 39B from Parcelable0x34f0, calculates crc8, appends byte39, calls MCUServer::mcuSend(buffer,40,mode1). Returns send result, there is no separate output dump. | Raw transport, not a flash reader; danger depends on the forwarded command. Was not run. |
| StationService::SetTrackerUart, 0x7c30c | Selects role4/5/6 by tracker type, packetSendData with command0x50 and input byte; mcuSend40B; waits for scalar status. | Control setter, not a USB flash download. The name Uart itself does not confirm UART on tracker USB. |
| StationService::GetControllerHwInFlash, 0x6f6ac | packetSendData command0x0a, role2/3, body 1B; mcuSend40B; waits for and returns halfword server+0x195a. | Controller info/status, not an arbitrary flash read. |
| MCUOtaIN618::McuOtaBootromCmdDownload, 0x855e8 | Assembles packet header0x0f7e, address4B and input buffer at offset0xa, tail0x7e; memcpy input→packet; mcuSend; WaitAck. | Download means loading INTO target, not reading FROM target. Class IN618, not proof of Nordic loader. |
| StationService::StationBootromStart, 0x77f08 | Starts worker/thread and station bootloader actions; nearby IN618 erase/write/upgrade API. | Not a safe getter. The exact worker path is not yet fully recovered; do not invoke on tracker by similar name. |
| MCUOtaNRF52832::UpdateFWTracker, 0x8a048 | lseek, input-container merge_bin_parse, rsa_verify, crc8, SendFWTrackerOTAPacket start/data/finish and ACK retry. | Host uploader, not bootloader code. Named NRF52832, this is not proof of tracker part. |
| station_upgrade | Paths/system/etc/ctlbin, MCUOtaFWChipUpgrade, MCUNRF52832/MCUServer. | Station MCU updater; raw Swift loader bytes not established. Was not run. |

## K05278
Source `D0188`, original line27, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Container parser: what was rechecked

## K05279
Source `D0188`, original line29, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

merge_bin_parse VA0x89570 reads the input file into buffer0x64000. Checks byte[0]==1, byte[0x20]==2, byte[0x40]==3; takes 256B signature from offset0x41; total payload = LE32[7]+LE32[0xb], and requires file size==0x141+payload. Then copies the entire payload from offset0x141, separately returning the first segment length. This is **host parsing**, not the bootloader's ability to accept a new size.

## K05280
Source `D0188`, original line31, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Script assertions on the saved FW189: size193385, two segment length96532, equal app bytes, header0x141. Both copies matched outputs/swift189-image.bin SHA477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89. Consequently, there is no separate lower loader0..fff region in these segments. The host protection model is verified by rsa_verify; the absence of this check in Frame transmitter does not prove the absence of a check in the unknown bootloader.
