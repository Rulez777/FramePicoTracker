# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K00241
Source `D0166`, original line287, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration28 retained service replies99.295%, 2026-10-08

## K00242
Source `D0166`, original line289, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read Iteration28 measurements and negative coexistence result (original reference: `SERVICE_FAST_POLL_ITERATION28_2026-10-08.md`).
150s:7187 validated unique replies /7238 TX-END=99.295385%.
Stable118.10s:5676/5677=99.982385%; CRC delta100%, one ring drop.
Pending-record early acknowledgment and compact state reads; service27 ELF unchanged.
90s two-controller experiment: valid controller poses throughout, but only one
tracker reply and zero IMU. Useful coexistence FAILED. OpenVR validity does not
measure controller RF packet loss. Session/RAM/MPU/trace and service ELF/config
restored; custom radio inactive because controllers remain connected.
83 focused tests pass. Bootloader readback and OTA remain unproven; noflash.
Evidence, source and Grok4.7 medium review: outputs/service-fast-poll28/.

## K00243
Source `D0166`, original line300, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration27 service radio above60 percent, 2026-10-08

## K00244
Source `D0166`, original line302, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Current hardware result and implementation:
Iteration27 research report (original reference: `SERVICE_FAST_POLL_ITERATION27_2026-10-08.md`).
Two-minute confirmation:99.881% CRC RX/TX-END; conservative unique retained
answers5491/5876=93.448%. Exact measured SRAM service ELF installed with backups.
Last saved installed live state:95.703% retained replies,99.537% raw RX/TX-END,
9077 completed TX, running/connected true,error null. These are saved observations,
not a new device query. Combat preserved; noflash/OTA. Older entries below remain
historical evidence. See the report for source/build/capture hashes and limits.

## K00245
Source `D0166`, original line311, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration23 phase-only command10 hardware test, 2026-10-08
Read docs/SERVICE_FAST_POLL_ITERATION23_2026-10-08.md.
User authorized one bounded RF test; noflash/OTA/reset. Cameras/optics/command14 off.
20s baseline11.589% RX/slots;11.5s phase-enabled11.612%; no material uplift.
15command10 attempts,14transport ACK; physical phase/application acceptance unproven.
Planned60s stopped~32s on stock assert106/501, samePC13e54/LR10507 as20.
Camera worker is not necessary for fault; rootcause unknown; no repeat run.
Session/RAM/MPU/trace restored; ELF/config restored; service inactive,no loader/OpenOCD.
13tests +native retiming/identity/checksum +probe delay/ACK/refresh passed.
Command10 concerns sync timer; NOT proven RX cadence control. pose_valid remainsfalse.
Next: offline assert/lifecycle audit before persistent RF; OTA gate unchanged.

## K00246
Source `D0166`, original line323, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration22 native configuration audit, 2026-10-08
Read docs/SERVICE_FAST_POLL_ITERATION22_2026-10-08.md.
Native e700 forms command16; old Gemini claim absent is false.
10f48 -> eadc -> e6f0 gives verified [N,8] path; ours [N,2]. C is apply countdown.
Native command10 uses TIMER4 CC1, not TIMER3; last service test disabled phase sync.
Native PPI TX path rounds625 units; units/path coverage not proven, no625us BLE claim.
Saved commands21 logs ONLY instrumented16, not full sequence timing evidence.
42.5ms gaps do not establish tracker RX duty cycle; station interruptions confirmed.
S1 full native equivalence and phase-only sync remain next audit tasks.
Offline only: code/images/device/service/OTA gate unchanged; no RF/SSH.

## K00247
Source `D0166`, original line334, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration21 TX timing / fresh command16 test, 2026-10-08
Read docs/SERVICE_FAST_POLL_ITERATION21_2026-10-08.md.
Two30s bounded runs, one stock tracker, controllers_off guard, optics disabled.
N8 seq1:3788slots/190RX=5.02%; N0 seq2:3777slots/446RX=11.81%; both~125slots/s.
Separate2036B ELF timestamps software TXEN in Record.number; loader names tx_enable_cycles.
Source identity and CRC guards preserved; native timestamp/MMIO test passes.
N0 adjacent answered TXEN99pairs median4726.69us; not timestamps of every TX.
Gap7 answered slots often42.5ms instead33.1ms: actual scheduler interruptions.
RX callback delay203..734us, median421; RX deadline800us, late replies not observable.
Fresh sequence test did not fix loss; N not directly read back, no sole-cause proof.
DroppedN8=8,N0=1: use MCU RX totals, not ring-record count. No TX-END counter this build.
Noassert, session/RAM/MPU cleanuptrue; module/config restored, combat preserved.
Radio stopped, noflash/OTA/continuous operation; no6DoF/optics/coexistence proof.

## K00248
Source `D0166`, original line348, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration20 service fast-poll hardware test, 2026-10-08
Read docs/SERVICE_FAST_POLL_ITERATION20_2026-10-08.md.
User authorized service SRAM build/swap/RF measurement; no tracker/Roy firmware flash.
1988 service ELF was already installed: forbidden low-priority REQUEST asserted106/472.
Service callback now returnsNONE, uses request API18949 on5/6/7; API failure stops.
Final service2020B, measurement2036B. Battle ELF a911666f... preserved.
Native11cases and25loader/runtime tests pass; session/RAM/MPU cleanup true every run.
Baseline6.74slots/s,1.96RX/s; corrected125.98slots/s,14.40RX/s (7584TX-END/864RX/60s).
Explicit requestedN0:5035TX-END/590RX/40s,11.72%; applied trackerN not read back.
RX gaps often42.5ms or4.7ms; actual START grid/receive windows unresolved, not RF interference proof.
Optics-on reachedstreaming then stockassert106/501 after~10s; rootcause unproven.
Fault guards were preexisting, not added. No continuous RF left running or autostart enabled.
Frame active/service ELF now corrected2020, radio-onlyN0 config installed, controllers_off guard.
Bootloader/OTA readiness unchanged;126slots/s is not120IMU/s or reliable OTA/6DoF.

## K00249
Source `D0166`, original line363, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration19 JNI / DEX audit, 2026-10-08
Read docs/FW189_ITERATION_19_JNI_DEX_AUDIT_2026-10-08.md.
Factory APK uses external system ARM libs; nine ARM + two AArch64 ELF inspected.
1454 ARM PLT GOT targets verified. HardwareBuffer reads Android buffers.
Encryption getter returns five station/controller status bytes, not a Swift dump.
Six MucTestSpi constructors emulated: fixed131B version/MAC/SN packets.
checkStationLinkStatus hit200000 instruction limit: not validated; no hardware replies.
UpdateManager failed extract branch copies assets to local files and checks hashes.
C2S1 repair uses /system/etc/ctlbin_bak and controller/station upgrade, not Swift recovery.
General cCmd server/indirect/legacy/IL2CPP paths not fully reconstructed.
No verified Swift loader/readback/recovery found in reviewed scope.
Offline only; firmware/code/whitelist/readiness gate unchanged.

## K00250
Source `D0166`, original line376, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration18 offline partitions / APK audit, 2026-10-08
Read docs/FW189_ITERATION_18_OFFLINE_VENDOR_APK_AUDIT_2026-10-08.md.
PICO5.13.8 vendor/product/odm CRC/Brotli/transfer/ext4 verified;2994 regular paths read.
System content scan5097 regular paths + ZIP/APK entries; no verified Swift loader.
8 vector hits:4known Phoenix +4duplicated DEX code-item coincidences, not new firmware.
Six APK sources added/decompiled; errors disclosed, key classes fallback checked.
McuUpgradeManager selects4-field hm01 names; FW1895-field name excluded in THIS path.
Binder21 startMcuOTA delegates BLE. READ6/8 constants not implemented by builder/parser.
BLE GET_BASE_ADDR response automatically queues PAGE_ERASE: never invoke as read-only.
FirmwareUpdate embedded config hash_info empty; runtime/data not read.
No hardware/OTA; app/fork hashes unchanged, readiness gate and whitelist unchanged.

## K00251
Source `D0166`, original line388, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration17 full OS package / local inventory, 2026-10-08
Read docs/FW189_ITERATION_17_FULL_OS_PACKAGE_SEARCH_2026-10-08.md.
Three PICO CDN ZIP directories inventoried (5.12.0/5.13.3/5.13.8),41 entries each.
Only5.13.8 system fully extracted: CRC/Brotli/transfer checked,5991 ext4 paths.
All3 trackerbin files byte-identical to saved pico4e; no new Swift loader.
Entire local236-file pico4e catalog +3APK entry strings/paths scanned.
Not exhaustive code/payload analysis; vendor/product and older system not extracted.
No hardware/OTA; firmware/code/whitelist/readiness gate unchanged.

## K00252
Source `D0166`, original line397, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration09 SWD plan clarification, 2026-10-08

## K00253
Source `D0166`, original line399, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration16 PICO server/boot search, 2026-10-08
Read docs/FW189_ITERATION_16_SERVER_BOOT_SEARCH_2026-10-08.md.
Real server in libstationopticsservice.so. DirectCmd sends40B, not dump.
IN618 BootromCmdDownload sends input to target, NOT Swift flash readback.
Uart setter command50 not proof USB diagnostics. HWInFlash returns halfword.
Container host parser89570 yields two identical96532app segments; no loader.
Five binary plain-vector scans found0; compressed/custom artifacts not excluded.
No new OS packages downloaded, no RF/SSH/ADB/OTA, gate unchanged.

## K00254
Source `D0166`, original line408, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration15 stock command/readback audit, 2026-10-08
Read docs/FW189_ITERATION_15_COMMAND_READBACK_AUDIT_2026-10-08.md.
Native901c init yields31entries; incoming callback+12, aux+8 ACK/event.
Command1 has13subcommands. No supported arbitrary flash/boot read found.
Sub02 indexed32B RAM read: index0 underflow,13..255 silent; not flash.
Sub29 consumes fixed34B RAM mailbox onAA00 branch. Sub14 SPI log getter
WRITES via d130 and advances cursor; do not treat as read-only dump.
Sub0c includes real SYSRESETREQ; not transmitted. No RF/SSH/OTA this iteration.
User authorized bounded prior RF, NOT OTA; gate unchanged. Avoid blind repeats.

## K00255
Source `D0166`, original line418, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration14 identity probe, 2026-10-08
See FW189_ITERATION_14_IDENTITY_PROBE_2026-10-08.md. 40bounded bootstrap polls,
0records; controller-off baseline monitored, guard not interrupted; all cleanup true.
PairedAAAA ignores length9 by native code; no unchanged repeats/pairing reset.
OTA gate unchanged.

## K00256
Source `D0166`, original line424, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration13 wake capture, 2026-10-08
User woke tracker. Passive bootstrap1/37/77 at600us:46slots,0ADDRESS/END/records.
Flash false, all cleanup true. See FW189_ITERATION_13_WAKE_CAPTURE_2026-10-08.md.
No unchanged passive repeats; audit identity/no-op and readback paths next.

## K00257
Source `D0166`, original line429, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration12 passive radio, 2026-10-08
User authorized radio sniff, not OTA. See FW189_ITERATION_12_RADIO_SNIFF_2026-10-08.md.
First-address RX600us/10ms/10s:42slots,39retries,0records; all cleanup true.
Controllers disconnected at baseline; no coexistence claim. No unchanged repeat.

## K00258
Source `D0166`, original line434, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration11 SWD debug power, 2026-10-08
Read docs/FW189_ITERATION_11_SWD_DEBUG_POWER_2026-10-08.md.
No DHCSR writes by default; stop if tool requires core debug changes.
Detach clears both asserted DP power requests with verified completion.
Nordic nRF52833 spec says normal mode returns after requests clear; no
mandatory power-cycle claim. Unconfirmed detach means power-state uncertainty.
Docs only; no tests/code/image/hardware changes; user pause persists.

## K00259
Source `D0166`, original line442, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration10 unified SWD operator plan, 2026-10-08
Read docs/FW189_ITERATION_10_SWD_OPERATOR_PLAN_2026-10-08.md.
SWD plan now has one integrated sequence. Never reuse/include Roy configs.
Audit DP/AP and DHCSR writes; no halt/step/reset; DEMCR/trace default prohibited.
Nordic revision2 configuration249 verified as APPROTECT-related.
Docs only; no tests/code/image/hardware changes; user pause persists.
Read docs/FW189_ITERATION_09_SWD_PLAN_REVIEW_2026-10-08.md and updated
 docs/FW189_SWD_READONLY_PLAN_2026-10-08.md before any hardware stage.
pyOCD attach MUST also set auto_unlock=false; OpenOCD reset_config none
alone does not prevent software reset. Audit tool hooks/config before attach.
CTRL-AP APPROTECTSTATUS read only; no erase/reset/unlock. UICR NRFFW0/1
are hints, not proof custom loader exists/absent. Dump stays private locally;
never include raw dump/keys/calibration in external review packages.
Battery/case/VTref precautions and SPI-staging limitation documented.
Docs only: no test rerun/code/image/hardware changes; user pause persists.

## K00260
Source `D0166`, original line458, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration08 refresh retry correction, 2026-10-08
Read docs/FW189_ITERATION_08_REFRESH_RETRY_2026-10-08.md.
Host defaults refresh10/querytimeout8/retrybackoff2/lease30 seconds.
Nominal first timeout18,second request20,answer before28; test proves second
success preserves phase/streaming. Busy pipeline may shorten remaining lease.
139 tests pass. Firmware unchanged. Read-only SWD plan prepared at
 docs/FW189_SWD_READONLY_PLAN_2026-10-08.md; target/probe/pinout not assigned.
No hardware actions; pause,whitelist,and readiness gate remain.

## K00261
Source `D0166`, original line467, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration07 station mode review, 2026-10-08
Read docs/FW189_ITERATION_07_STATION_MODE_REVIEW_2026-10-08.md.
Firmware iter05 unchanged. Host defaults refresh10s/querytimeout10s/lease30s.
One missed background query retains still-valid proof, releases pending,
advances command1 sequence, backoff10s; no immediate disconnect. Lease remains
anchored to request, clears after30s; real reconnect still requires new proof.
Missing7e diagnostics distinguish unknown unsupported/config/loss, not guessed stock.
Native9368 response uses fixed pool descriptor20000158:20nodes,stride101,
base20001083,NOT504byte heap. Native allocator/free/dedup/ACK tests prove
3queue heads+17response capacity, same-query duplicates do not allocate,
exhaustion recovers afterACK. General heap adequacy still unknown.
138 focused tests pass. No hardware changes; OTA readiness gate stays closed.

## K00262
Source `D0166`, original line480, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration06 boot/recovery audit, 2026-10-08
Read docs/FW189_ITERATION_06_BOOT_RECOVERY_2026-10-08.md.
68 focused tests pass. Firmware iter05 unchanged. Enlarged app activation
97320/CRC1f/AAAA or BBBB metadata and SYSRESETREQ execute with mocked NVMC.
No verified FW189 bootloader found in saved files; b97 unknown format is NOT
recognized as loader. Known standalone Swift files duplicate app/container.
Native bcd0 full-off returns and retains synthetic pending OTA state; only
executed C startup clears it. Button off/on is not proven fresh boot.
Pending-worker stale ACK race remains unresolved, not fixed by mode nonce.
Hardware OTA run/preflight/runtime now fail before hardware or OtaSession via
tracker_ota_readiness.require_hardware_ota_clearance; no JSON bypass.
plan_preflight remains offline configuration planner. Whitelist unchanged.
Need actual loader/recovery and clean-start evidence before clearing gate.
No device/SSH/SWD/RF/OTA/service changes; user pause persists.

## K00263
Source `D0166`, original line495, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration05 mode handshake, 2026-10-07
Read docs/FW189_ITERATION_05_MODE_HANDSHAKE_2026-10-07.md first.
Final fork outputs/fw189-dual-mode-iter05-final-20261007/fw189-dual-mode.bin:
97320bytes,SHA395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f,
CRC8=1f,4055full blocks + empty sentinel4056. Binary reproducible/reversible.
Ordinary resetreason0 startup caps hold150 with event1; Frame release100..149.
Other reset reasons retain native event3/pairing; runtime button unchanged.
Read-only command1/subcommand7e schema1 nonce16 at native wrapper8d16;
replyfe01+nonce16+chipLE4+mode+revision1 via native reliable queue9368.
Station opts in via tracker_dual_mode_ids + explicit tracker_mode_decimation
stock/frame N map; manual per-ID N does not override proof for those peers.
Initial transport ACK cannot complete mode query. Random nonce+identity checked,
background refresh2s,lease3s anchored to request creation; late query rejected.
Same mode renewal preserves phase; changed mode reconfigures. Unknown/expired
mode gates config/sync/pose. Not an instantaneous silent-reboot detector.
161 final regression tests pass. Whitelist unchanged; exclusive OTA adapter
rejects dual-mode config. Heap504,bootloader/recovery/RF/UX still unverified.
No hardware/SSH/RF/SWD/OTA/service operations; user pause remains in force.

## K00264
Source `D0166`, original line514, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration04 independent dual review, 2026-10-07
Read docs/FW189_ITERATION_04_DUAL_REVIEW_2026-10-07.md.
Dual binary unchanged; no whitelist/hardware changes. Seven added tests pass:
full97152-byte native application transfer uses4048full blocks + empty sentinel4049,
CRC8=0x30, finish_verified; SPI/allocator/IRQ hardware mocked.
Native event3 dispatcher confirmed transition to STA_PAIRING(state2).
Deep subsystem/bond effects not fully audited. Ordinary resetreason0 now reaches
this event under long startup hold: must redesign before release.
Mode needs explicit identity/session-correlated handshake; version189/manualN
and response timing alone do not prove current mode. No handshake implemented.
Stock profile is not stock rollback. User hardware pause remains in force.

## K00265
Source `D0166`, original line526, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

## Iteration03 dual-mode fork, 2026-10-07
Read docs/FW189_ITERATION_03_DUAL_MODE_2026-10-07.md before continuing.
Experimental image outputs/fw189-dual-mode-20261007d/fw189-dual-mode.bin:
97152 bytes, SHA256 c5158c9f26f8bf5fedbea1d4c497fd651d1188fac11b8bd1bf3389132a064c0e.
62 offline tests pass; repeat build is byte-identical. Default stock, startup
release counts100..149 selects Frame. Physical timing not verified. Ordinary
startup now waits for release/count600; this is a deliberate lifecycle change.
Native runtime button untouched. Full startup/full-off clears profile;
fake-off may retain it. Reserve8 BSS bytes at20005150, heap512->504.
Old RC1 untouched. New expanded image NOT accepted by current OTA loader.
Bootloader, heap worst case, physical gesture and RF coexistence unverified.
No hardware operations; approved_for_ota=false; user pause remains.

## K00266
Source `D0166`, original line946, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

# 2026-10-09: phase71/72 quiet-window admission

## K00267
Source `D0166`, original line948, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

See measured results and deployment correction (original reference: `PHASE71_72_WINDOW_RESULTS_2026-10-09.md`).
Both initial controller checks passed. Corrected phase72 required 700 us but measured
600 us, so no RF session began; >=60 percent coexistence remains unresolved.

## K00268
Source `D0166`, original line952, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

# 2026-10-09: phase73 sampler diagnostic

## K00269
Source `D0166`, original line954, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

See raw timing uncertainty (original reference: `PHASE73_BLE_SAMPLER_UNCERTAINTY_2026-10-09.md`).
Both controllers stayed valid throughout register-read-only observation; the
asymmetric RTC bracket median was1648us. Do not treat finer histogram as RF proof.

## K00270
Source `D0166`, original line958, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

# 2026-10-09: phase74 offline native sampler

## K00271
Source `D0166`, original line960, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

See compiled CPU-side measurement candidate (original reference: `PHASE74_NATIVE_BLE_SAMPLER_CANDIDATE_2026-10-09.md`).
Four exact-binary tests pass; no live execution or RF result. Review restoration
and callback lifecycle before diagnosing true quiet-window timing with it.

## K00272
Source `D0166`, original line964, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

# 2026-10-09: current English review handoff / phase75-76 results

## K00273
Source `D0166`, original line966, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Read self-contained connection and research state (original reference: `CONNECTION_AND_RESEARCH_STATE_FOR_EXTERNAL_REVIEW_2026-10-09.md`).
Fresh native controller snapshot100/100 valid; custom receiver stopped, mode
unknown. Native phase76 sampler completed/restored,2112records/2090zero-tick
accepted,12healthy metricwindows; no PICO RF run or >=60% result. Phase75 cleanup
regression and phase74 pre-entry mistake preserved with quantitative evidence.

## K00274
Source `D0166`, original line973, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Phase79/80 controllers-OFF controls: see PHASE79_80_RX_WINDOW_CONTROL_2026-10-09.md. Fresh mode1/rev7 proof, 11.73% vs13.30% nominal opportunity ratios; no stable connection, both cleanup fully verified. No OTA. Goal remains active.

## K00275
Source `D0166`, original line975, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Service52/Frame3000 static comparison: SERVICE52_TO_FRAME3000_COMPARISON_2026-10-09.md; dense N0/M2, cycle1, RX800 candidate requires phase-specific fresh-mode policy and native RTC reconstruction. No RF/OTA performed.

## K00276
Source `D0166`, original line977, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Phase81/82 dense Frame service: PHASE81_82_DENSE_FRAME_SERVICE_2026-10-09.md. Exact Service52 module with mode1/N0/M2:94.3277% CRC/TX,84.8930% retained/TX,2710IMU,493ring drops. Controllers OFF; all cleanup verified;99%/coexistence goal not achieved.

## K00277
Source `D0166`, original line979, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Phase83 host batching regressed to13.00% CRC/TX and target ceased replying. Phase84 RTC-derived2989us prepared only; waiting for target-wake confirmation. See PHASE83_BATCH_REGRESSION_AND_84_PREPARATION_2026-10-09.md. No OTA; controller measurement stage remains pending.

## K00278
Source `D0166`, original line981, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Controller-only sampler85 prepared, not installed/run; PHASE85_CONTROLLER_SCHEDULE_PREPARED_2026-10-09.md. Pending controller-OFF84 wake confirmation remains.

## K00279
Source `D0166`, original line983, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Latest84/86/87 results: PHASE84_86_87_MODE_RESTORE_AND_CADENCE_2026-10-09.md.2989us89.3422%CRC/TX;2600us14.9674%;99% not achieved. Controller native85: PHASE85_CONTROLLER_SCHEDULE_HARDWARE_2026-10-09.md;2176raw, both0invalid/0loggedfreeze, fine second-half520--550us fold, no robust900us service gap. All cleanup verified, no combined-RF test.

## K00280
Source `D0166`, original line985, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Phase88: fine RX measurement and coexistence budget (original reference: `PHASE88_FINE_RX_AND_COEXIST_BUDGET_2026-10-09.md`). Controller-OFF96.5646%CRC; joint admission unresolved.

## K00281
Source `D0166`, original line987, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Phase89: extended controller gaps (original reference: `PHASE89_EXTENDED_CONTROLLER_GAPS_2026-10-09.md`),600us shared native-observation gap; short RF profile remains unvalidated.

## K00282
Source `D0166`, original line989, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Phase89 Grok review disposition (original reference: `PHASE89_GROK_REVIEW_DISPOSITION_2026-10-09.md`): review completed, proposal not ready for RF.

## K00283
Source `D0166`, original line991, SHA256 `e4f4de3ab665a6f7b8be9fa69df005f7e06a83d7d8b3c800346d218a596039dc`. Status: historical/unadjudicated narrative; English source prose.

Current target98–100% with BOTH controllers; complete Phase90/new-chat handoff (original reference: `RESUME_COEXISTENCE_98_100_2026-10-09.md`). Offline107passed,90d1773B; no integrated coordinator or RF run. Work stopped for context transfer at user's request.

## K00284
Source `D0167`, original line1, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

# Full-body execution contract after user correction

## K00285
Source `D0167`, original line3, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

User explicitly prohibits circular, unstructured probing. This contract governs continuation. Goal remains a real PICO tracker in SteamVR alongside two healthy Frame controllers, with 98-100% measured sensor delivery. Current status: NOT achieved, pose_valid=false, installed tracker revision07 unchanged.

## K00286
Source `D0167`, original line5, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

## Immediate decision

## K00287
Source `D0167`, original line7, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

No further exploratory RF sweeps, repeated command19/65 loops, identical sensor reinitialization tests, or controller OFF/ON cycles without a new evidenced causal reason. Phase156 completed and all four restoration checks passed. Candidate09 diagnostics is OFFLINE and unflashed. Requested Grok4.7-medium diagnostic-sufficiency review is pending; finish it rather than launch another radio profile.

## K00288
Source `D0167`, original line9, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

## Ordered work and exit criteria

## K00289
Source `D0167`, original line11, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

|Step|Concrete work|Evidence required to advance|
|---|---|---|
|1: localize missing IMU|Audit minimal internal diagnostics; inspect cached sensor identity, init flag, permission and actual producer/sample-counter path. Prefer an existing verified readback if available. Prepare one reviewed diagnostic image only if installed07 cannot expose the state.|A concrete branch decision from actual internal values. Empty ACK/transport ratios do not qualify. Additional fields should avoid serial diagnostic OTA iterations.|
|2: repair that branch|Implement the specific fault demonstrated in step1, preserve button/remote mode compatibility, validate actual native paths, review final exact image.|Fresh real IMU samples on target, with sequence progression and no reported physical hang. No invented/default sensor output.|
|3: joint game transport|Use the already measured6000/N1 game route; address actual host-ring loss and measure true sample counter gaps, acquisition cadence, latency and both controller health.|Sustained real sensor delivery at98-100%, controller validity/disconnect/freeze metrics, and physical motion checks. Radio empty-response99% is insufficient.|
|4: SteamVR full-body acceptance|Enable current-boot calibrated cameras, identified factory LED model, measured IMU axes/time alignment and real metric world pose; publish to GenericTracker driver.|The user sees the physical tracker moving correctly in SteamVR during simultaneous controller operation. Registration/synthetic benchmarks do not count.|

## K00290
Source `D0167`, original line18, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

## Admission rule for every hardware action

## K00291
Source `D0167`, original line20, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

Before a run, record: existing evidence and relevant failed history; exactly one changed factor; predicted distinguishing result; bounded duration; abort/restoration conditions; required output artifact. Afterward, record the result and choose an explicit next branch. An inconclusive result does not authorize an unchanged rerun.

## K00292
Source `D0167`, original line22, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

Any proposed tracker image needs exact-hash review and the user's previously required pause at the actual OTA boundary. Candidate09 currently has no new-image authorization; whitelist remains unchanged. A diagnostic image is not a full-body fix. Do not turn the user's insistence on completion into authorization to silently flash a new image.

## K00293
Source `D0167`, original line24, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

## Current measured facts

## K00294
Source `D0167`, original line26, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

- Phase151 game:18170CRC/18325TX=99.15416%,IMU0.
- Phase154 full sensor-init request:2574CRC/5139TX,IMU0,main20responses0..3,allcleanuptrue.
- Phase156 UART-off factor:2330CRC/5140TX,IMU0,actualnative5000response,main20responses0..4,allcleanuptrue. UART-off alone insufficient.
- Prior Phase132 did produce5461uniqueIMU with controllers, but controller-health failed. This is relevant evidence that the firmware can produce IMU; it does not prove current state or readiness.
- Candidate09 diagnostic base hash298d6ebb7caa19b66e9952895efa22c09c531b88b07d0c4439d351dacbf45c65;97912bytes; unflashed. First external ABI review complete, sufficiency follow-up pending.

## K00295
Source `D0167`, original line32, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

Physical state is uncertain: user reported Frame turned on, followed by an asynchronous answer that it was off/unresponsive. SSH timed out. This does not block offline review; do not repeatedly ask the user to wear or toggle hardware during offline work.

## K00296
Source `D0167`, original line35, SHA256 `5c4fb05f642c24ba9b61e7b568b08122ad0e5e28343275f76ecd27955416db71`. Status: historical/unadjudicated narrative; English source prose.

## Additional explicit user constraint
Any unsupported enumeration or parameter sweep is prohibited, including a superficially bounded sweep. A launch needs a measured fact or verified native-code dependency that justifies its ONE factor and an explicit distinguishing outcome. Past results must be accepted and reused: radio99% was observed, real5461IMU in132 was observed with failedcontrollerhealth; currentIMU0inbothmodes and156UARTofffailure narrow work to sensorproduction/applicationstate. These are not invitations to restart RFfrequency/timingsearch. Offline test count cannot substitute for repair progress.

## K00297
Source `D0169`, original line7, SHA256 `728c436d031b9cae1a8f0a261a3d6a4c4f43786fcd2df372997e65b9cc42b59c`. Status: historical/unadjudicated narrative; English source prose.

---

## K00298
Source `D0170`, original line191, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

Application record:

## K00299
Source `D0170`, original line264, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

Native path: command1 subcommand dispatcher`0x8c10` → table entry`0x16558` → callback`0x88cc` → OTA handler`0xfbcc`; background processing`0xf974`.

## K00300
Source `D0170`, original line293, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

States:

## K00301
Source `D0170`, original line309, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

### 10.1. Job gates

## K00302
Source `D0170`, original line353, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

Unicorn model:

## K00303
Source `D0170`, original line459, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

SHA256: `3ac2a6b141903c8c60b4505de0934dbaf15aca2e00b4607fde95a03f27d5f694`.

## K00304
Source `D0170`, original line556, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

SHA256: `12f3c5f6ca1d80a2491b218b5f083d6f8fe98b4cf29456f5d011a991571164ac`.

## K00305
Source `D0170`, original line575, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/radio/tracker_grid_patch.py`

## K00306
Source `D0170`, original line674, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/radio/tracker_image_audit.py`

## K00307
Source `D0170`, original line736, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/radio/tracker_ota.py`

## K00308
Source `D0170`, original line903, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/radio/tracker_ota_prepare.py`

## K00309
Source `D0170`, original line966, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/radio/ram/tracker_ota_runtime.py`

## K00310
Source `D0170`, original line1121, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/radio/swift_light_sync.py`

## K00311
Source `D0170`, original line1151, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/radio/swift_records.py`

## K00312
Source `D0170`, original line1191, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/radio/ram/peer_engine.py`

## K00313
Source `D0170`, original line1454, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/radio/ram/roy_ram_station_v4.c`

## K00314
Source `D0170`, original line1672, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/radio/ram/ble_phase.py`

## K00315
Source `D0170`, original line1779, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/radio/ram/controller_guard.py`

## K00316
Source `D0170`, original line1924, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/tests/fw189_emulator.py`

## K00317
Source `D0170`, original line2015, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/tests/test_tracker_grid_patch.py`

## K00318
Source `D0170`, original line2073, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `pc/tests/test_tracker_ota.py`

## K00319
Source `D0170`, original line2177, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `roy_ram_loader.py:423-505`

## K00320
Source `D0170`, original line2265, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; English source prose.

#### `roy_ram_loader.py:600-665`
