# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K00481
Source `D0199`, original line52, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

The builder refuses an image that reaches `0x2F000`. The edit list is application trampolines plus the eight-byte BSS reservation at `0x1348`/`0x134c` (`20005150`, heap 512→504). No ATA/bootloader bytes are in the manifest. Iteration 06/15 still have no verified FW189 bootloader, and iteration 39’s successful OTA was installed05 `395ce84b…`, with no full flash readback. Whitelist and hardware clearance are still that image. CRC8 finish is not SHA256 identity. This candidate is a different 97,736-byte image.

## K00482
Source `D0199`, original line54, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

## What holds up

## K00483
Source `D0199`, original line56, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

The setter `0x7d/0x02` checks length 24, mode ≤ 1, N ≤ 40, and all four bytes at `20000efe` before it stages. Under PRIMASK it writes pending as `mode+1` and target N at `20005152`; live mode is not written in that callback. OTA-busy returns status 2 and clears pending. The boundary then writes live mode, live N, and `f24` together, and clears command-16 pending and countdown, before any later `0x6df6` can apply the old countdown. Schema 2 replies echo nonce, chip, revision 7, live N, and `20000f22`. Wrong length, schema, identity, or range falls through to native `0x8d1b`. The host accepts a mode only after a fresh query matches chip, nonce, revision, desired mode, actual N, and pending 0. A setter reply only causes that query. The loader rejects automatic mode in an OTA session and rejects unequal initial/steady poll intervals before debugger open. The 8800 µs Stock-N1/Frame-N2 figure is an offline window check against 4727.5/3934.5 and 3000/2207, not a measured RF plan.

## K00484
Source `D0199`, original line60, SHA256 `1cebe05c92e94d6ce0f14ae69e30c888200c46b2ed270817cfee528a07474a60`. Status: historical/unadjudicated narrative; English source prose.

Do not authorize OTA of `3957a3da…`. Fix the hook so the disabled-radio path cannot start TIMER0, and fix the host so pending N waits instead of resending command 16. Re-run the native boundary test on that exact image with `0x57ec` and `EVENTS_DISABLED` actually executed and with COMPARE1/`0x5e9c` observable. Bootloader, recovery, and a hardware transition remain separate gates after that.

## K00485
Source `D0201`, original line1, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

# FW189 UART activation and revision08 candidate

## K00486
Source `D0201`, original line3, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

## Scope and provenance

## K00487
Source `D0201`, original line5, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

The user explicitly authorized sending UART enable, confirmed controllers OFF, and connected the tracker to the PC. No OTA was requested in this specific activation step. Earlier request to build revision08 produced an offline candidate; it has not been flashed.

## K00488
Source `D0201`, original line7, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

Installed revision07 lineage: outputs/remote44/candidate07-initial-connection/fw189-dual-mode.bin,97832B,SHA25640941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5. This is the historically transferred image, not the superseded97848B audit-final candidate. Original command audit15 and server audit16 were read before modification. Source firmware SHA256477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89.

## K00489
Source `D0201`, original line9, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

## Native UART path: static and offline evidence

## K00490
Source `D0201`, original line11, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

Command1/subcommand0x50 already exists in stock FW189 and installed07. Dispatcher8c10 calls8814, writes first input byte intoRAM20001d45. Nonzero calls nativea144; zero calls1918. ACK body is5000. Native handler also logs the next input byte, so the experiment uses body500100 with explicit zero padding.

## K00491
Source `D0201`, original line13, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

Nativea144 reads the selected board descriptor through200005bc, uses+0x74/+0x78 pin fields, configures the pins and calls driver initializer17d8 with baud register03afb000 and256-byte buffers. ROM board tables17d24/17da4/17e24 giveTX17;RX4/18/18 respectively. The target's selected board descriptor and physical Type-C contact routing have NOT been read or electrically verified. A UART GPIO configuration is not USB enumeration support.

## K00492
Source `D0201`, original line15, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

The flag20001d45 affects other native paths (includingc788's early exit); UART mode must not be assumed transparent to normal tracker operation. The receive callback9fe0 parses framed/checksummed serial input and sets state consumed bya400. Further UART command execution analysis is outstanding. No serial input or malformed command was sent in this experiment.

## K00493
Source `D0201`, original line17, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

Seven offline tests passed for the candidate wrapper and native setter/configuration: native initialization retained, no duplicate UART init if already active, preservedR4, native0/1 setter ACK and callbacks, board pin selection and driver configuration. Driver calls are intercepted in these tests; they do not prove physical UART signals.

## K00494
Source `D0201`, original line19, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

## Hardware activation result

## K00495
Source `D0201`, original line21, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

Exact target [TRACKER_SERIAL]/chip[TRACKER_ID] was identified and Frame mode freshly proven. A bounded30-second controller-OFF service session used the existing2032B Phase103 module, no command19, optics or OTA. After bootstrap the reservation remained900us/RX800 for the UART exchange. Two host tests passed, including actual script imports and one logical UART request with no19.

## K00496
Source `D0201`, original line23, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

At3.104316s the coordinator queued one logical command1 body500100, sequence_before2. Sequence advanced at3.223325s. Retained raw record43, slot361, contained command1 response5000. This application response is stronger evidence than a transport ACK: the native setter reached its response path. It does not measure electrical UART output, connector routing, baud stability, or a host-accessible debug console.

## K00497
Source `D0201`, original line25, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

The tracker continued replying until intentional shutdown.603 retained replies/3403 scheduled slots, IMU0, ringdrops0; the native observer/counter acquisition was disabled, so this is NOT a CRC/TX_END delivery metric. Exit0; session_closed_verified,ram_restored_verified,mpu_restored,trace_controls_restored_verified alltrue. No flash writes. UART disable was not sent: the requested application setting was left enabled. Persistence through reboot is not demonstrated; do not assume it survives native reinitialization.

## K00498
Source `D0201`, original line27, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

Windows present USB/HID/Ports snapshots before/during/after each contain29devices, with no added instance IDs. Thus no new USB or COM enumeration was observed in this setup. This does not prove the Type-C connector is power-only, nor rule out proprietary strap transport or UART on selected contacts. The user's report that a strap exchanges data through this connector is retained as user-supplied hardware context, not an independently measured pinout.

## K00499
Source `D0201`, original line29, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

Raw evidence and inventory: outputs/uart-enable-radio/attempt,usb-before.json,usb-during.json,usb-after.json,analysis.json,evidence-sha256.json. Hardware runner never writes tracker firmware and prohibits tracker-ota-job. Stock radio was restored after the run.

## K00500
Source `D0201`, original line31, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

## Revision08 offline candidate, NOT installed

## K00501
Source `D0201`, original line33, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

outputs/uart-revision08/fw189-dual-mode.bin:97864B,SHA2561e111618677983dfaf9be1a4c875463d63bebf2b21094dd3fcfd4aaf7f557078.

## K00502
Source `D0201`, original line35, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

Derived from exact installed07. Existing appended code changes only the two revision reply constants7->8; a new32B wrapper is appended. Native subsystem init call atbc1c is redirected to wrapper18e29: call originald408, check20001d45, and if inactive set1/calla144. Native0x50disable remains available until subsystem reinitialization/reboot. This default-on behavior is stronger than the single live activation and has not been hardware-tested. It may alter normal tracker subsystems through the shared native flag.

## K00503
Source `D0201`, original line37, SHA256 `900c119ef5cfd63b4da1caf2cf5ef7c5011aba69b728fd5c6b67859cca32e50c`. Status: historical/unadjudicated narrative; English source prose.

The patch manifest reverse check recovers exact stock prefix. Source, ELF, disassembly, manifest, tests and builder are preserved. An initial test fixture overwroteR4 through Firmware.call's default argument; the fixture was corrected and final7tests passed. No bootloader/emergency recovery, physical port routing or continuous tracking proof is supplied. approved_for_ota=false; no external final-image review or OTA job exists. Reflashing just to enable UART is currently unnecessary because the existing native command returned its response. Determine the actual connector transport and usefulness of native UART before any default-on firmware deployment.

## K00504
Source `D0204`, original line1, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

# Mode53: revision07 execution and remote mode1 confirmed, weak RF grid

## K00505
Source `D0204`, original line3, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User authorizes remote mode switch and test. Read mode44 audit,
native receive-window constraints and failed post-OTA wrapper history first.
No firmware changes/OTA.48 focused host/remote tests passed before deployment.

## K00506
Source `D0204`, original line7, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

Isolated mode53 release contains the complete audited peer_engine,
station_v4_runtime and roy_ram_loader, not the earlier schema2 wrapper.
Normal release unchanged. Desired frame policy, exact chip[TRACKER_ID],
stockN1/frameN2, common8800us period,900us slots, optics/controllers OFF.
Service38's stock-only guard was NOT bypassed: use ordinary service27 ELF
65d217e5...f64a44 without OTA, because service38 is fixed to4727us.
Common grid preflight passed; this is the first measured common-grid experiment.

## K00507
Source `D0204`, original line15, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

## Actual command/reply evidence

## K00508
Source `D0204`, original line17, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

CRC-valid raw records in `outputs/mode53/attempt/radio-live-records.jsonl`:

## K00509
Source `D0204`, original line19, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

- fe02e414ebd9443bc2452a4683a31af1bb363eed197900070000: mode0,revision7,N0,pending0.
- fe021b27cecf466a9819162208831b6aeedf3eed197900070100: mode0,revision7,N1,pending0.
- fd02147cd3b26739e57249309ee657efb6893eed197900010107: setter ACK actual0,requested1,status1 staged,revision7.
- fe025ae40c2018d13bd0e1048f1d564745613eed197901070200: actualmode1,revision7,N2,nativepending0.

## K00510
Source `D0204`, original line24, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

Host validated fresh nonce/schema/revision/chip, and reported radio_mode=frame,
verified=true while running. Setter ACK alone was not accepted as applied-mode
proof. This confirms07-specific code executes and remote0->1 was applied.
Native command16 currentN preparation and required post-switch configuration
were used. This is not cryptographic full internal-flash SHA readback.

## K00511
Source `D0204`, original line30, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

## Reception and expiry

## K00512
Source `D0204`, original line32, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

60-second complete run:1601 TX-END,283 CRC RX=17.6765%,236 retained unique
answered slots=14.7408%, zero ring drops,251 IMU samples. Whole-run statistics
include enrollment and switching, not an isolated stable mode1 interval.
This does not reproduce service52's98.68% stock-grid reception or prove100%
custom-mode RF. Common-window arithmetic was insufficient for good RF here.
No pose/optical/controller coexistence proof; controllers OFF throughout.

## K00513
Source `D0204`, original line39, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

The initial fresh mode1 proof later expired at30s because subsequent query
responses were not obtained reliably. Final live snapshot reports mode=null,
verified=false and two query failures, correctly refusing stale proof.
Last verified applied state is mode1/N2/revision7; current state after cleanup
is not re-queried. Do not describe the final snapshot as currently verified.

## K00514
Source `D0204`, original line45, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

## Cleanup and next work

## K00515
Source `D0204`, original line47, SHA256 `1d883abcc617b2aea9ef2c8394a44041823f264ffe13092fb6cbb5d04ead8e9a`. Status: historical/unadjudicated narrative; English source prose.

Session closed, SRAM/MPU/trace restored, exit0. Module removed after test.
Evidence includes frozen host hashes/invocation/config, raw radio records,
live report, recovery and cleanup. Earlier unsuccessful probes preserved.
Investigate actual RX window/poll phase and sequence/ACK scheduling before
another cadence experiment; neither old10% nor service98% identifies a mode.
A1 staging limitation and recovery/coexistence unknowns remain.

## K00516
Source `D0205`, original line1, SHA256 `0c7dc2dc705beabdb3abd8fe38fe3e1bda82ddd62ad47b4967835dfd7b3b5abe`. Status: historical/unadjudicated narrative; English source prose.

# Mode check40 after requested Frame gesture

## K00517
Source `D0205`, original line3, SHA256 `0c7dc2dc705beabdb3abd8fe38fe3e1bda82ddd62ad47b4967835dfd7b3b5abe`. Status: historical/unadjudicated narrative; English source prose.

User requested verification after instructions for nominal1.0-1.49s startup
gesture. Exact button timing/full-off/resetreason not measured. One45s no-OTA
service38 query check, target [TRACKER_SERIAL]/chip[TRACKER_ID]. Fresh nonce/chip
validated response reports mode0(stock), revision1. Frame mode NOT selected.
Do not infer the cause: off/startup branch and physical count duration unknown.
No firmware changes/OTA or new Frame cadence proof. Evidence outputs/mode40/
mode-proof.json and raw capture; cleanup session/SRAM/MPU/trace all true, unit
inactive. Original module preserved. pose_valid=false.

## K00518
Source `D0206`, original line1, SHA256 `10667347196e85c99d7f4d3f83522a0744c0ddc0fcae620b1fcbd9f424b39972`. Status: historical/unadjudicated narrative; English source prose.

# Mode41 hardware check and exact startup limitation

## K00519
Source `D0206`, original line3, SHA256 `10667347196e85c99d7f4d3f83522a0744c0ddc0fcae620b1fcbd9f424b39972`. Status: historical/unadjudicated narrative; English source prose.

User reports tracker switched on and requests mode check/instructions. One45s
no-OTA service38 run confirmed exact serial/chip and fresh nonce response:
mode0(stock), query protocol revision1. These fields are independent.
Evidence outputs/mode41/mode-proof.json and raw capture. Cleanup all true,
unit inactive. No firmware change/OTA.

## K00520
Source `D0206`, original line9, SHA256 `10667347196e85c99d7f4d3f83522a0744c0ddc0fcae620b1fcbd9f424b39972`. Status: historical/unadjudicated narrative; English source prose.

Read final05 dual-mode.S profile_boot_hold/release and native swift189-full.asm
c3ec..c4ac,8474..8490. Mode flag set1 only when r4(resetreason-derived value)
is0 and button count100..149 on release. Native8474 reads POWER RESETREAS
at40000400, clears it, maps raw bits into r0; c426 stores into r4. Native loop
delay uses10dd0(10); nominal1.0-1.49s not hardware calibrated. Startup150
cap applies only r4=0. Other reset reasons retain native button logic and
cannot select Frame through this patch. Actual live resetreason unmeasured.
Earlier blanket off/on1.2s instruction was incomplete: button off does not
prove resetreason0/full power removal. Do not claim repeated timing trials
will switch it. A practical button wake path needs a reviewed modification
covering its measured reset reason or a separately authorized runtime mode
setter; installed05 provides read-only7e query, no remote mode-set command.
Firmware remains unchanged.

## K00521
Source `D0207`, original line1, SHA256 `f4f0077281d4befa5762eb95925326fa62434203fccbd299fbfbdcc362875d1b`. Status: historical/unadjudicated narrative; English source prose.

# Iteration43: prepare repeated-start counter repair, no tracker write

## K00522
Source `D0207`, original line3, SHA256 `f4f0077281d4befa5762eb95925326fa62434203fccbd299fbfbdcc362875d1b`. Status: historical/unadjudicated narrative; English source prose.

User requests trying mode1. Read root AGENTS, mode41 startup gate, failed RX42,
iteration05 assembly/native disassembly and iteration06 power/startup evidence.
No additional unchanged RF experiment or tracker OTA in this iteration.

## K00523
Source `D0207`, original line7, SHA256 `f4f0077281d4befa5762eb95925326fa62434203fccbd299fbfbdcc362875d1b`. Status: historical/unadjudicated narrative; English source prose.

## Additional offline defect

## K00524
Source `D0207`, original line9, SHA256 `f4f0077281d4befa5762eb95925326fa62434203fccbd299fbfbdcc362875d1b`. Status: historical/unadjudicated narrative; English source prose.

Native startup c3ec loads hold counter20001ff2 but does not initialize it on
entry; installed05 profile_boot_entry clears only mode. Native main feaa calls
c3ec from the application power-state path, so repeated button starts need not
be C runtime initialization. Inspect startup callers/c4d8/c0f0 as well; those
shown initialization routines do not directly reset this counter. Earlier Button
tests called C startup first each time and missed retained counter scenarios.

## K00525
Source `D0207`, original line16, SHA256 `f4f0077281d4befa5762eb95925326fa62434203fccbd299fbfbdcc362875d1b`. Status: historical/unadjudicated narrative; English source prose.

Execute exact installed05 in Button model with simulated prior counter150 and
120 additional held cycles: cap returns at151,mode0, before release. With zero
prior count,120 selects mode1. Prior99/120/150/600 with120 additional cycles all
select stock through the150 cap. Peripherals, delays and selected initialization
calls remain mocked; actual live counter/resetreason unknown. This is an offline
counter defect, not proof of the specific hardware failure cause.

## K00526
Source `D0207`, original line23, SHA256 `f4f0077281d4befa5762eb95925326fa62434203fccbd299fbfbdcc362875d1b`. Status: historical/unadjudicated narrative; English source prose.

## Prepared revision06 candidate

## K00527
Source `D0207`, original line25, SHA256 `f4f0077281d4befa5762eb95925326fa62434203fccbd299fbfbdcc362875d1b`. Status: historical/unadjudicated narrative; English source prose.

pc/radio/tracker_dual_mode_boot43.py wraps preserved revision05 builder and
adds ONLY hold-counter clear at profile_boot_entry, alongside mode clear.
Registers/APSR are restored before original native prologue. Reason0-only
gesture gate and100..149 interval remain unchanged; other reset reasons still
cannot choose Frame. No runtime mode setter, LED change or native pairing
rewrite. Thus candidate addresses stale count; it does not establish measured
button timing or guarantee all wake/reset paths. Do not describe it as universal.

## K00528
Source `D0207`, original line33, SHA256 `f4f0077281d4befa5762eb95925326fa62434203fccbd299fbfbdcc362875d1b`. Status: historical/unadjudicated narrative; English source prose.

Image outputs/boot43/fw189-dual-mode-rev06-counter43/fw189-dual-mode.bin,
97328B, SHA2566d1424d7202b80e94bda23bb941eb35b07631b1e695417dbd3e4851f3d53f2a6.
Manifest/source/ELF/object/compiler commands alongside; reverse restores exact
stock477000fc...a89. Original installed05 SHA395ce84b...14f untouched.

## K00529
Source `D0207`, original line38, SHA256 `f4f0077281d4befa5762eb95925326fa62434203fccbd299fbfbdcc362875d1b`. Status: historical/unadjudicated narrative; English source prose.

146 tests passed including old05 regression suite plus candidate counter cases;
57 candidate tests then passed including full new97328 native transfer/exact
staging/reverse, mode query both profiles, native cadence/conversion paths and
counter priors0/4/99/120/150/600/65535 with boundaries4/99/100/120/149/150/600.
Reason gates1/4/16/32/256 explicitly remain closed. Tests are offline, not
bootloader, hardware button timing, heap or physical RF/recovery validation.

## K00530
Source `D0207`, original line45, SHA256 `f4f0077281d4befa5762eb95925326fa62434203fccbd299fbfbdcc362875d1b`. Status: historical/unadjudicated narrative; English source prose.

No OTA package or live authorization exception for candidate06 was created.
Existing hardware allowance is pinned to exact installed05 image; do not
silently broaden it. Candidate needs explicit selection for next physical OTA.
Bootloader/recovery still unknown despite successful05 transfer/query execution.
Frame used only offline clang/linker build; no SWD/RF session or Frame flash.
Durable evidence outputs/boot43/*tests.xml and image manifest. User selection
of replacement image pending; current tracker remains unchanged.

## K00531
Source `D0208`, original line1, SHA256 `b4d4440f3cc7b42420b3e708a1bbadf81937061b5f04af10cc67bbc2e89c227f`. Status: historical/unadjudicated narrative; English source prose.

# Nordic47: user correction, RAM module stopped and stock state restored

## K00532
Source `D0208`, original line3, SHA256 `b4d4440f3cc7b42420b3e708a1bbadf81937061b5f04af10cc67bbc2e89c227f`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User requested stock Frame Nordic operation by removing the RAM
module, not running the older service27 RAM module. Lead initially chose
service27 incorrectly, then stopped that exact loader with SIGINT on correction.

## K00533
Source `D0208`, original line7, SHA256 `b4d4440f3cc7b42420b3e708a1bbadf81937061b5f04af10cc67bbc2e89c227f`. Status: historical/unadjudicated narrative; English source prose.

Cleanup confirms session closed, SRAM restored, MPU restored and trace controls
restored. Radio service inactive, no loader/OpenOCD remains. No Frame flash write
or tracker OTA. Thus the temporary module is removed; do not reset or reflash
Frame to achieve this. No new RAM radio module is left running.

## K00534
Source `D0208`, original line12, SHA256 `b4d4440f3cc7b42420b3e708a1bbadf81937061b5f04af10cc67bbc2e89c227f`. Status: historical/unadjudicated narrative; English source prose.

The interrupted service27 sample (1220 TX,1064 CRC-valid RX) is preserved in
outputs/nordic47/nordic47, but it is NOT a stock Frame measurement and does not
identify the tracker mode. After restoring stock state, no native Frame tracker
reception/count/loss metric has yet been measured. Installed07 activation and
mode remain unconfirmed. Do not invent a stock10%/custom100% classifier.

## K00535
Source `D0209`, original line1, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; English source prose.

# OTA45 revision07: partial transfer, no finish

## K00536
Source `D0209`, original line7, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; English source prose.

## Measured hardware outcome

## K00537
Source `D0209`, original line9, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; English source prose.

Exact target [TRACKER_SERIAL]/chip[TRACKER_ID], controller-free service38,
image97832B SHA256
`40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5`.
The guarded live adapter passed identity/configuration and began OTA.

## K00538
Source `D0209`, original line14, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; English source prose.

2559 of4077 indexed blocks acknowledged (62.77%). Block2560 reached its
10-second application ACK deadline. Final state failed, error
`OTA data block 2560: application ACK deadline expired`.
Retries increased from7 during successful progression to45 at failure;
2598 response polls and4041 rejected observations recorded.
No finish ACK or finish_verified state. No new application startup was tested.
Do not describe revision07 as installed; current installed-image execution
after this partial transfer is not freshly verified.

## K00539
Source `D0209`, original line23, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; English source prose.

The transition to block2560 occurred at unix1791494840.8287203, sequence0,
generation10260; failure at1791494850.8308513. Prior blocks2558/2559 used
sequences62/63. Later mailbox publications repeat the same indexed2560
payload and short response polls. This establishes an association with a
sequence wrap, not its cause: many earlier wraps succeeded and original05
OTA39 completed4056 blocks. Do not assume a sequence-zero or page-boundary
defect without examining retained RF/native scheduling evidence.
Host mailbox publication is not proof of actual RF delivery.

## K00540
Source `D0209`, original line32, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; English source prose.

## Cleanup and preserved evidence

## K00541
Source `D0209`, original line34, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; English source prose.

Loader exited1. All cleanup checks true: session closed, SRAM restored,
MPU restored, trace controls restored. Frame radio unit inactive and no
loader/OpenOCD process remains. No Frame flash write occurred.
These checks establish station cleanup, not tracker OTA-idle state.
Tracker internal OTA state after interruption remains unknown.

## K00542
Source `D0209`, original line40, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; English source prose.

Complete copied evidence: `outputs/ota45/attempt/`, including frozen job/image,
invocation hashes, radio-run/recovery/manifest, stdout/OpenOCD logs,
`job/ota-result.json`, `.events.jsonl`, `.tx.jsonl`, and cleanup.json.
`outputs/ota45/evidence-manifest.json` indexes preparation and attempt evidence.
Prior39 success and45 pause evidence remain preserved.

## K00543
Source `D0209`, original line46, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; English source prose.

Offline4077-block service38 campaign succeeded but did not predict this
physical timeout; peripherals, IRQ timing, queue admission and loss model
remain incomplete. No recovery or revision07 activation proof follows from it.

## K00544
Source `D0209`, original line50, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; English source prose.

## Next boundary

## K00545
Source `D0209`, original line52, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; English source prose.

Do not replay this result-bearing job. Do not infer a safe resume offset or
issue finish after the partial transfer. Any new transfer requires a fresh
job and explicit user decision after diagnosing the failed ACK and checking
target state. No automatic second attempt is authorized by run45.
`approved_for_ota=false` remains. A1 and hardware recovery limitations remain.

## K00546
Source `D0210`, original line1, SHA256 `4c5b9509adbdbc2878033d61da66857f193008725c81ca14249f155508d863ce`. Status: historical/unadjudicated narrative; English source prose.

# OTA45 focused Grok review

## K00547
Source `D0210`, original line3, SHA256 `4c5b9509adbdbc2878033d61da66857f193008725c81ca14249f155508d863ce`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. Cursor grok-4.7-medium, read-only. Scope: exact-image admission,
isolated preparation, and pause boundary. No hardware or independent test run.
Verbatim reviewer output follows. No OTA or safety approval is implied.

## K00548
Source `D0210`, original line7, SHA256 `4c5b9509adbdbc2878033d61da66857f193008725c81ca14249f155508d863ce`. Status: historical/unadjudicated narrative; English source prose.

No concrete code blocker in this OTA45 pause preparation. The risky path still needs an explicit CLI flag, the prepared job stays on the exact revision 07 image and service38 profile, and the pause path returns before any `OtaSession` is constructed. `approved_for_ota` stays false. I did not re-run the suite or any hardware.

## K00549
Source `D0210`, original line9, SHA256 `4c5b9509adbdbc2878033d61da66857f193008725c81ca14249f155508d863ce`. Status: historical/unadjudicated narrative; English source prose.

**CLI.** `load_job` still rejects a fork unless `allow_unverified_fork=True`, then forces `unverified_fork_authorized` to false. Clearance passes only after `apply_cli_authorization` sees `--authorize-unverified-fork`. A true value in job JSON is overwritten. `--pause-before-ota-start` does not authorize anything by itself. `prepare45.py` keeps that authorize flag by copying the OTA39 command and only then appends the pause flag, sets the duration to 45 seconds, and points the loader at the isolated tree.

## K00550
Source `D0210`, original line11, SHA256 `4c5b9509adbdbc2878033d61da66857f193008725c81ca14249f155508d863ce`. Status: historical/unadjudicated narrative; English source prose.

**Target, profile, and hash.** Clearance still requires slot 0, chip `[TRACKER_ID]`, serial `[TRACKER_SERIAL]`, grid 4727.5 µs, `preparation_only` false, and a recomputed SHA-256 whose length matches `RISK_ACCEPTED_FORKS`. That map is exactly the installed fork 05 (`395ce84b…14f`, 97320 bytes) and revision 07 (`40941b34…56d5`, 97832 bytes). The copied job keeps `radio_profile` `service38`. The same gate still allows `service27` and `service36` for either hash; this prepared job does not use them. The measured profile in the copied station config is still service poll, optics off, controllers off, peers 1 and 2 paused, 4727/900 µs, margin 100.

## K00551
Source `D0210`, original line13, SHA256 `4c5b9509adbdbc2878033d61da66857f193008725c81ca14249f155508d863ce`. Status: historical/unadjudicated narrative; English source prose.

**Reverse check.** For a fork, `load_job` calls `reverse_dual`. That checks the manifest `patched_sha256`, replays every edit on the 96532-byte stock prefix, and requires frozen stock `477000fc…0a89`. The candidate manifest is revision 7, 97832 bytes, the same patched hash, and `approved_for_ota` false. `prepare45.py` checks revision 7 and that hash before writing the job. The unit test’s flipped last byte fails the image hash check; the successful `load_job` is what actually runs reverse.

## K00552
Source `D0210`, original line15, SHA256 `4c5b9509adbdbc2878033d61da66857f193008725c81ca14249f155508d863ce`. Status: historical/unadjudicated narrative; English source prose.

**Pause and sender.** After a live FW189 serial match and clearance, `pause_before_ota_start` sets `paused_before_ota_start`, writes `ota_start_sent` false, and returns before `OtaSession(...)`. Later polls leave `self.ota` as null. The unit test mocks `OtaSession` and expects no call. `pause45.py` refuses to launch unless the pause flag is on the command, then requires that same result. `prepare45.py` does not start the loader. The isolated copy replaces only `tracker_ota_runtime.py` and `tracker_ota_readiness.py`. `radio/tracker_ota.py` and `--module` `ota38/build/roy_station.elf` stay as copied. For `service38`, runtime still sets `capture_ack=True` with response polls and a 7200-second cap. The script does not re-check the OTA39 sender digest `08d4fc51…bca8`; preservation is by leaving that file in place.

## K00553
Source `D0210`, original line17, SHA256 `4c5b9509adbdbc2878033d61da66857f193008725c81ca14249f155508d863ce`. Status: historical/unadjudicated narrative; English source prose.

**Unchanged limits.** Inventory still prints `approved_for_ota` false with the same three blockers. A1 stays open: a staged revision 07 transition can remain at `0x20005151/52` and is not visible in the state reply. Bootloader acceptance, recovery, clean native startup, and the pending-worker ACK race are still unknown. This review did not start OTA.

## K00554
Source `D0211`, original line1, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

# OTA45: revision07 preparation and explicit pre-start pause

## K00555
Source `D0211`, original line3, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

2026-10-08. User selected proceeding at accepted risk, then confirmed both Frame
controllers OFF and the target tracker ON. The earlier instruction to stop
immediately before OTA remains in force. No OTA command was sent.

## K00556
Source `D0211`, original line7, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

## Evidence and historical dependencies

## K00557
Source `D0211`, original line9, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

Read root AGENTS, revision07 audit entry (original reference: `FW189_REMOTE_MODE44_AUDIT_2026-10-08.md`),
Claude disposition (original reference: `FW189_REMOTE_MODE44_AUDIT_DISPOSITION_2026-10-08.md`),
successful OTA39 and prior failure references (original reference: `OTA_INDEXED_CAPTURE39_2026-10-08.md`),
and the revision11 SWD plan/report. No SWD connection to the tracker is available.
Stock/assembly provenance remains in the snapshot catalog/manifest and the
preserved `outputs/remote44/evidence-manifest.json`; this new record describes
post-snapshot changes and measurements, not a revision of historical evidence.

## K00558
Source `D0211`, original line17, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

## Offline changes and tests

## K00559
Source `D0211`, original line19, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

The exact-image risk exception now admits audited revision07 in addition to05:
97,832 bytes, SHA256
`40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5`.
Both load_job and hardware-clearance use the same hash-to-size mapping.
Manifest reversal still verifies the frozen stock source. CLI authorization,
exact chip[TRACKER_ID]/serial[TRACKER_SERIAL], stock cadence, slot0, controller-free
profile and measured ELF checks remain. JSON cannot self-authorize the job.
This is explicit risk acceptance, not a finding that hardware is safe.

## K00560
Source `D0211`, original line28, SHA256 `0acdfe9b767a3b80bd8391cc374820725378ff494243ae2e01c780f766f229d2`. Status: historical/unadjudicated narrative; English source prose.

36 focused authorization/runtime/pause/stock-regression tests passed in0.52s.
Additional exact07 transfer/pause/integrity suite:3 passed in39.79s, with
`outputs/ota45/offline-tests-service38.xml`. Full native receiver plus compiled
service38 model acknowledged4077/4077 blocks with modeled10% directional loss,
backlog/ring overflow and exact staging. Peripherals/time/SPI remain modeled.
The earlier `offline-tests.xml` used the service36 model; it is preserved but
is superseded for service38 coverage by the explicitly selected ELF38 test.
