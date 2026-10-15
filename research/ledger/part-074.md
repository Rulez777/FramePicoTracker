# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05841
Source `D0480`, original line87, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- SteamOS VR 0.4.3, build 20260930.6234839, aarch64; Roy version 1788551713.
- FICR of the built-in Nordic: nRF52833, RAM 128 KiB, flash 512 KiB.
- `/dev/spidev0.1` is served by `spidev`; `vrserver` and `XRService` use
  this interface. `frame_hmd` is a resource driver, implementation is in `driver_cv.so`.
- Roy uses SoftDevice Controller/MPSL and BLE. Live RADIO in BLE1M mode:
  MODE=3, CRC24, poly=0x65b, init=0x555555, advertising access address.
  This is not a confirmation of ESB compatibility with Pico.
- SWD via stock OpenOCD/GPIO is accessible without unlock/recover. Flash region
  0..0x40000 was read, first 237920 bytes matched the resource HEX:
  SHA256 `0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50`.
  This is a check of the application region, not a backup copy of the entire flash 512 KiB.
- Firmwares, NVMC, pairing flash were not written; stock scripts `program_ble.sh`,
  `setup_controller_radio.sh`, `disable_spi_hid.sh` were not run.

## K05842
Source `D0480`, original line101, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The user permitted temporary execution of the module in SRAM, with loss after
reboot and unchanged flash. `pc/radio/ram/roy_ram_rx.c`, linker
script, loader, and checkpoint recovery were implemented. Clang on the Frame itself built
an ARM Cortex-M4 Thumb ELF: 620 bytes of code, entry 0x2001151d.

## K05843
Source `D0480`, original line106, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Manufacturing RTT buffers 1/2 are reserved; Terminal buffer 0 is preserved.
Original buffers and counters are saved before writing. Loader writing is limited
to these RAM regions; individual SWD commands temporarily modify MPU and CPU registers.
RAM is initially XN: region 1 RASR=0x110b0021, MPU CTRL=5. To invoke code, execution
prevention is removed immediately before entry. Upon restoration, MPU
is returned to its original state. Diagnostic disabling of MPU did not resolve the failure
and is not a workable method of integration.

## K05844
Source `D0480`, original line114, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Hardware test results:**

## K05845
Source `D0480`, original line116, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Test | Result |
|---|---|
| SRAM loading and execution, return via BKPT only, without MPSL | Confirmed: `trap.json`, `trap_returned=true`; original RAM restored |
| MPSL open/request via discovered flash entry points | One run returned 0 from API; then RAM state disappeared, Roy rebooted; no working slots registered |
| RADIO RX Pico | Did not run: not a single confirmed Pico packet |
| Concurrent receiver operation with controllers | Not confirmed; after restoration SteamVR sees both stock controllers |
| Final restoration | RAM/RTT checkpoint verified byte-by-byte, no active MPSL session, MPU CTRL=5 and RASR=0x110b0021 |

## K05846
Source `D0480`, original line124, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

During SWD experiments Roy rebooted repeatedly; the log reports Reset=4.
The cause is not yet established: neither watchdog, nor MPU fault,
nor MPSL ABI error can be considered proven. The crash does not imply a need for re-flashing. Prolonged
flash reads and CPU halts affect stock timesync; the loader now
checks five short flash anchors, relying on the previously verified full
dump of the application region. If the state magic disappears, the test terminates with an error,
rather than reporting successful radio exchange. `scheduling8.json` was recorded prior to fixing this
check: exit code 0 in it **does not signify a successful MPSL test**.

## K05847
Source `D0480`, original line133, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

ABI addresses pertain only to the verified image: open=0x10581,
request=0x18949, close=0x189bf, one 48-byte session context=0x2001056c.
They cannot be used on another build. Official MPSL header (original reference: `https://github.com/nrfconnect/sdk-nrfxlib/blob/main/mpsl/include/mpsl_timeslot.h`)
confirms the timeslots architecture, but does not replace verification of the ABI of this particular
image and real callbacks.

## K05848
Source `D0480`, original line139, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Container Swift2.0 contains header 0x141 and two identical images of 96532
bytes each. In its RADIO setup, PCNF0=0x40008 and generation of PCNF1 with BIG ENDIAN were found.
The previously assumed profile S0LEN=1/little-endian is not confirmed. Specific
channels, addresses, CRC, and pairing transitions still need to be established from the application
configuration and live air. The assumed payload of 64 bytes is not a
confirmed RF format.

## K05849
Source `D0480`, original line146, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

45 Python tests pass; this is a check of software boundaries, mathematics, and
error handling, not proof of standalone tracking.

## K05850
Source `D0480`, original line149, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Remaining verifications

## K05851
Source `D0480`, original line151, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Localize MPSL callback failure after successful entry into SRAM; check
   call context, actual MPU permissions, and ABI, without modifying flash.
2. Analysis of Roy host command dispatcher and its calls in `frame_hmd` driver:
   runtime radio config, diagnostic RX, CRC-error payload export, scheduler.
3. Verification of exact Pico PHY/channels/addressing/pairing and live packet format.
4. If stock compatible entry is confirmed, implement backend through it.
   First record real packet + id + timestamp; then decoder and SteamVR.
5. Verify movement of real tracker, preservation of Valve controller communication,
   loss/jitter, disconnect, coordinates and roles in SteamVR. For 6DoF, separately
   confirm access to camera frames/calibration; IMU integration does not provide
   a reliable absolute position.

## K05852
Source `D0480`, original line163, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

SRAM execution has already been proven, so the absence of a stock RX API does not in itself
close the path authorized by the user. But prior to a working scheduler and
a live RF packet, this experiment cannot be passed off as PICO/Valve integration.

## K05853
Source `D0480`, original line167, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Fixes during audit

## K05854
Source `D0480`, original line169, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Missing serial no longer starts an inoperative worker as successful RX.
- The daemon does not select mock automatically in the absence of a receiver.
- Explicit mock is launched independently and does not open a physical COM port.
- The interpreter checks exact length, finiteness of float fields, and raw
  quaternion norm BEFORE normalization by decoder.
- New tests cover HEX, string addresses, absence of device I/O,
  corrupted float fields, and receiver open error.

## K05855
Source `D0480`, original line177, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

These fixes and tests do not mean that reception via Roy is already working.

## K05856
Source `D0480`, original line180, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Addendum: stock shell, MPSL, and verification with active controllers

## K05857
Source `D0480`, original line182, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Update as of 2026-10-05 after user reply "ready". This section refines
the previous SRAM/BKPT result and takes precedence over earlier conclusions
regarding the inability to invoke a callback or MPSL being busy based on a non-zero structure.

## K05858
Source `D0480`, original line186, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### New confirmed execution path

## K05859
Source `D0480`, original line188, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Stock RTT Terminal0 contains a working Zephyr shell. Responses were received for
  `help`, `kernel threads`, `devmem -h`, `rtt_control -h`, without stopping the CPU.
- shell_rtt constant: 0x34b0c; ctx: 0x2000d894; temporary bypass callback:
  0x2000d8cc (ctx+0x38). This is specifically the RTT shell, not the wireless shell.
- A temporary write of the SRAM entry to bypass allows the stock shell to call a regular
  C function `(shell, bytes, count)`. The module immediately removes the hook via stock
  shell_set_bypass at Thumb address 0x2ee2f, calls the API, and returns normally.
  This variant does not alter CPU registers via a debugger and does not use BKPT for returning.
- The initial erroneous write to wireless bypass 0x20010264 did not invoke the module;
  the pointer was cleared, checkpoint restored. This address is no longer used.
- `native-counter1.json`: START=1, API result=0, followed by slot completion and closing;
  the module did not touch radio or TIMER registers. `native-repeat2.json` also gave only
  one START. These results do not prove stable repeating slots.
- `native-entry-only4.json`: new module of 272 bytes, without MPSL; entry_count=1,
  return marker=0x52455431, RAM not lost, hook removed, MPU restored.

## K05860
Source `D0480`, original line204, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### MPSL Session and API Calls

## K05861
Source `D0480`, original line206, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the verified firmware, the only session structure is located at 0x2001056c,
size 48 bytes. The activity flag is byte +0x26. In a closed session, the old callback
at +0x20 and temporary fields may remain non-zero. The `any(context)` check
erroneously declared such a session busy. Static stock close code 0x189be
and pool checks 0x18a00 use specifically +0x26. Loader/recovery have been fixed:
unloading is allowed only when status=0 and shell hook is zero. The old callback
0x200115a9 after closing is a remnant of our module, not proof of
an active stock timesync session. A non-zero status always prohibits deleting code.

## K05862
Source `D0480`, original line215, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Stock radio-test code 0x29d00 protects open/request/close with a shared lock:
0x28e2c → semaphore take, 0x28e3c → release. The new native module uses
the same functions with K_NO_WAIT (timeout 0) to avoid blocking the shell. The callback itself
does not acquire the semaphore. This was verified via stock disassembly; stability has not yet been proven.

## K05863
Source `D0480`, original line220, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Results Near Active Devices

## K05864
Source `D0480`, original line222, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The user confirmed turning on the PICO trackers and waking up two controllers.
SteamVR before/after the experiments showed HMD cv.FPPAE61400039, Roy_Left
b4eda8f6a5eb, and Roy_Right 95f963bc6ce4. The presence of both headsets was confirmed via
`adb devices -l`: Frame `frame`, Pico `[HEADSET_ADB_SERIAL]` / A8110.

## K05865
Source `D0480`, original line227, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Evidence | Result |
|---|---|
| native-host-repeat3.json | RAM magic disappeared as early as open/request; stock restarted; raw RX disabled |
| native-entry-only4.json | Normal entry/return via shell was successful without calling MPSL |
| native-locked5.json | RAM preserved, API=0, START=0, last signal 6 (CANCELLED), bitmap=64; closing confirmed |
| native-locked-repeat6.json | RAM lost again on first open/request; did not reach host retry |

## K05866
Source `D0480`, original line234, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Consequently, the successful result of the native-locked5 process signifies a processed
request and recovery, rather than a granted slot or successful radio reception.
The semaphore did not eliminate all restarts. The new loader verifies magic immediately
after the API, reports slots_granted separately, and checks recovered RAM/RTT
byte-for-byte. These new recovery checks have not yet undergone a separate live run.

## K05867
Source `D0480`, original line240, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

OpenOCD registered `external reset detected`, which means a reset noticed by
the debugger, rather than proof of an external reset source. Stock vrserver at
20:40:26.819 received a startup message and disconnected the controllers; Roy reported Reset=4;
by 20:40:29.95–30.01 both controllers reconnected. A later timesync reset
after ten errors is also present in the log. The source of the first reset has not been determined:
it cannot be asserted that it was specifically an MPU, watchdog, host, or MPSL assert.

## K05868
Source `D0480`, original line247, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In an earlier diagnostic run, a stock MPSL assert was caught at 0x13e54:
r0=112, r1=2097, LR=0x11887, IRQ TIMER0; CFSR/HFSR=0. START did not arrive then.
A late HardFault in the stock reset handler does not prove an initial
MPU fault. Direct halted-injection is not suitable for proving compatibility.

## K05869
Source `D0480`, original line252, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Limitations, Recovery, and the Next Technical Stage

## K05870
Source `D0480`, original line254, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- In all new experiments, RF is disabled; there are still no real PICO packets.
- No erase/program/reset commands, reflashing, SPI unbinding,
  or XRService/vrserver shutdown were performed. However, Roy indeed restarted during
  failed experiments, with a temporary disconnection of the controllers: this is part of the result.
- The loader reported unload, free session, and restored MPU in all new tests;
  after them, SteamVR sees both controllers again. No open OpenOCD was left running.
- Existing applications/firmware on both headsets were not replaced. The SDK was not changed.
- The complete previous flash capture is 256 KiB, not the full 512 KiB. The new loader checks
  five exact flash anchors and the SHA of the saved application image; this is not a new
  full hash of the entire device. Do not claim a full flash verification for each experiment.
- 46 Python tests passed, including regression: a closed MPSL session with a non-zero
  old callback is considered free. These are offline checks, not radio proof.
- Next stage: diagnose the first reset in the native MPSL call by intercepting
  early fault/assert paths and comparing with the stock radio-test path; then prove
  repeating START with two BLE connections. Do not enable raw RX before then.
- Following stable scheduling, real Swift framing/pairing settings,
  RF capture, a proper IMU decoder, access to optical tracking, and an independent
  SteamVR backend are needed. PICO/Valve ESB compatibility and full-fledged standalone 6DoF
  remain a hypothesis and an incomplete implementation.

## K05871
Source `D0480`, original line274, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Evidence is saved in the workspace `work/frame-stock/ram-experiments/latest/`
(upon repeated adb pull, some files are in the nested pico-ram-build), including ELF,
JSON/checkpoints/OpenOCD logs, and vrserver-after-coexistence.txt. Sources:
`K:/AI/pico/pc/radio/ram/roy_ram_rx.c`, loader/recovery, and roy_rtt_console.py.

## K05872
Source `D0480`, original line279, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Final recovery check: final-recovery7.log / roy_ram_recover.py passed on Frame: MPSL status=0, shell hook=0; CODE 2044 bytes, DATA 2048 bytes, and RTT counts matched the checkpoint byte-for-byte; MPU restored. Flash was not written.

## K05873
Source `D0480`, original line283, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Roy: MPU, Wi-Fi, and Coexistence — 2026-10-05

## K05874
Source `D0480`, original line285, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This section updates the previous findings regarding native MPSL failure. PICO RF
has not yet been received, real standalone 6DoF poses have not been acquired. Flash was not written.

## K05875
Source `D0480`, original line288, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Captured Cause of One Reset

## K05876
Source `D0480`, original line290, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

early-reset8.json: stop at MemManage entry PC=0x20860, CFSR=1
(IACCVIOL), HFSR=0. PSP frame: fault PC=0x2001151c, LR=0x1e5ed.
Magic preserved, session=255, MPSL context=0: error BEFORE open/request.
MPU CTRL=5, region 1 RASR=0x110b0021 (XN). The previous TIMER0 MPSL assert
remains a separate observation: not all old failures are proven to be an MPU fault.

## K05877
Source `D0480`, original line296, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The loader now intercepts MemManage/BusFault/other fault classes and
stock assert/reset points 0x13e54, 0x130a, 0x13de8, 0x20c3a, 0x20c9e,
0x29e14. The snapshot contains registers, stacks, MPU, module/session RAM. After
saving evidence, traps are removed, and the stock fatal handler continues operation.

## K05878
Source `D0480`, original line301, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Clarification: Zephyr does not necessarily rewrite the entire RAM region on thread
switch. PendSV 0x2049a calls 0x209bc; dynamic MPU code 0x20aa8 uses
RAM byte 0x20010a26. It changes MPU RNR and dynamic regions. Previous SWD
writes of RNR/RASR with a running CPU might end up in a different region due to
a race with PendSV. IACCVIOL is proven; this race mechanism is supported by disassembly,
but a separate DWT trace of competing writes has not been obtained.

## K05879
Source `D0480`, original line308, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Temporary MPU Region Without Flash Patch

## K05880
Source `D0480`, original line320, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The cleanup audit found a remnant of the old path: reserved retries/close still performed
an SWD MPU write and could temporarily extend EXEC to RAM. Fixed: these operations
no longer write the MPU selector; CPU entry atomically restores stock region 1.
permission-restoration22 (without MPSL/RF) passed on the fixed module. A full
MPSL repeat of the latest version was executed: revalidation23, 222 START,
337/337/337 pose rows; session closure and recovery succeeded.
Regression prohibits MPU register writes in reserved native close.

## K05881
Source `D0480`, original line328, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Live Evidence, RF Disabled

## K05882
Source `D0480`, original line330, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Evidence | Result |
|---|---|
| diagnostic-mpu-off9 | MPU temporarily off; 5 retries, 0 START, CANCELLED; restored |
| counter-short10 | 100 us/no-guarantee, priority 1: 0 START |
| reserved-entry11 | 356-byte entry, marker and CTRL=5/count=3 confirmed |
| reserved-counter13 | Via SSH, 8 START with host retry; controllers were sleeping |
| reserved-repeat14 | NORMAL chain priority 1 BLOCKED, 8 START via host retry |
| coexistence15 | Two controllers, 500 us priority 1: 0 START, without reset |
| coexistence-short16 | Two controllers, 100 us/no-guarantee: 0 START, without reset |
| priority-compare17 | 100 us priority 0: single START=1 |
| priority-repeat18 | Autonomous NORMAL chain, XTAL, distance=10000 us: START 22→222 in 2 s |
| timer-coexist19 | Request 500 us, TIMER compare at 300 us: START 21→221 in 2 s |
| timer-with-poses21 | 221 START; concurrent CSV of HMD/Left/Right poses: 341/341/340 rows |
| permission-restoration22 | CPU MPU entry-only, marker and recovery confirmed |

## K05883
Source `D0480`, original line345, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Priority 0 is the priority of stock timesync; low requests in these experiments were cancelled
with two BLE connections. This is not a universal guarantee of radio budget. Counter
immediately ends the slot; timer actually waits for compare, but does not touch RADIO.
221/222 is the total counter; the increment over the measured two seconds is 200 (~100 Hz).
Before and after the experiments, both controllers were in SteamVR. CSV proves the arrival
of poses during the experiment, but does not measure packet loss, latency, or extended gameplay quality.
Successful reserved experiments ran without detected reset/fault, closed the session, and
restored the checkpoint. OpenOCD was not left running.

## K05884
Source `D0480`, original line354, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Wi-Fi and Two Headsets

## K05885
Source `D0480`, original line356, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

SSH steamos@[DEVICE_IP] verified live. sudo -n requires a password, SSH is not root.
However, existing gpiod access is sufficient for OpenOCD/RTT/RAM loader without sudo.
User-owned working directory /home/steamos/pico-ram-work; Python and clang work
on Frame. Windows sends files/launches, autonomous callbacks do not require
a command from Windows each slot. USB Frame can be disconnected; TCP ADB was not enabled.
Runtime tracking bridge/driver is not yet installed. Pico [HEADSET_ADB_SERIAL]
remains a second research headset, not part of the final standalone.
No reliable current paired radio parameters were obtained from logcat.

## K05886
Source `D0480`, original line365, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Swift: Corrected LOAD_BASE and Protocol

## K05887
Source `D0480`, original line367, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Extracted update image LOAD_BASE=0x1000. RESET 0x13a5 corresponds to file offset
0x3a4 with startup branch; NMI 0x135d — offset 0x35c with infinite branch. Old
Swift disassembly addresses were file offsets: absolute VAs are higher by 0x1000.
With base=0, configuration pointers mistakenly fell into strings.

## K05888
Source `D0480`, original line372, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- PCNF setup file 0x40e8 / VA0x50e8: PCNF0=0x40008, PCNF1 big-endian.
- Config VA0x16074/file0x15074, 24 bytes:
  010000000000000004020000550003000001020000280000.
- Startup copy ROM0x18318→RAM0x2000006c..20000668.
- Address initializer VA0x1832c→RAM0x20000080:
  e7e7e7e7c2c2c2c2e7c2c3c4c5c6c7c80805ff0213010000.
  Initial bases, prefixes E7/C2/C3/C4/C5/C6/C7/C8, 8 pipes, 5-byte address,
  RX mask FF, channel 2: startup defaults, not confirmed live values.
- App initializer VA0x6658 sets mode byte=1, callback VA0x69c1,
  calls BASE0 setter VA0x58c4 with word0x785634aa. After RBIT BASE0=0x552c6a1e.
- App changes prefixes; conversion VA0x51ec uses RBIT+REV.
- Generic CRC=2 branch: INIT FFFF, poly0x11021. Bitrate byte=4 goes into RADIO MODE;
  do not replace generic nRF 2M with setting solely by bitrate name.
- Initialization, setters, and ACK FIFO strongly resemble Nordic ESB; mode=1
  corresponds to PRX according to official definitions. This is an inference, not proof of
  the tracker's current state. It is possible that Frame should poll the PRX tracker
  rather than merely listening for spontaneous IMU packets.
- Channel setter VA0x597c is called later from pairing/runtime code. Current
  channel/paired BASE1/prefix is unknown. No ready RX profile has been created.

## K05889
Source `D0480`, original line392, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

pc/radio/pico_swift_probe.py is a stdlib-only offline inspector, checks container,
duplicate images, vectors/config anchors; outputs/swift-firmware-evidence.json.
No device I/O. The update file on Pico does not prove the firmware currently installed
on the trackers. All 50 offline tests passed.

## K05890
Source `D0480`, original line402, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Next: reconstruct polling/pairing/channel changes and payload layout, then
verify a real RF packet in a constrained experiment with Valve BLE. Optical tracking
and standalone SteamVR poses are not yet implemented. Stock Valve radio was observed
as BLE; this work does not prove a shared PICO/Valve ESB wire protocol.
Evidence: workspace work/frame-stock/ram-experiments/wifi-latest and early-reset8.*.

## K05891
Source `D0484`, original line1, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Specification of Pico Swift 2.0 Radio Protocol, Packet Reception, and Pairing Procedure

## K05892
Source `D0484`, original line3, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Date:** 2026-10-06  
**Status:** Verified based on static disassembly results (Capstone) and live device inspection (Pico 4 Enterprise `[HEADSET_ADB_SERIAL]`, Steam Frame `frame`).  
**Investigated binary files:**
- `PhoenixSta_sv2.96_PR_20250805_b296_app.bin` (firmware for MCU of nRF52840 station in Pico 4 Enterprise headset)
- `Swift2.0_sv1.89_20241213_b189_app.bin` (firmware for Pico Motion Tracker Swift 2.0 tracker)
- `libpvrtrackingservice.so` (Pico OS system tracking service)
- `libtrackerclient.pxr.so` (client library for interaction with the service)
- `tracker_test` (native tracker diagnostics utility)

## K05893
Source `D0484`, original line14, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 1. Architectural Overview of End-to-End Data Path

## K05894
Source `D0484`, original line16, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the Pico Motion Tracker tracking system, data transmission from the sensor to user applications is divided into three isolated domains:

## K05895
Source `D0484`, original line44, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2. Physical Layer and Radio Configuration (Nordic RADIO)

## K05896
Source `D0484`, original line46, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Trackers and station operate via the `NRF_RADIO` hardware peripheral (base address `0x40001000`) without using the BLE SoftDevice stack.

## K05897
Source `D0484`, original line48, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### A. Radio Interface Registers

## K05898
Source `D0484`, original line50, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Register | Offset | Value | Description and Purpose |
|---|---|---|---|
| `MODE` | `0x510` | `4` or `0` | `MODE=4`: 2 Mbit/s Nordic proprietary. `MODE=0`: 1 Mbit/s Nordic. |
| `TXPOWER` | `0x50c` | `0x08` / `0x00` | Transmitter power (+8 dBm during pairing / discovery, 0 dBm in normal mode). |
| `PCNF0` | `0x514` | `0x00000108` | `S0LEN = 1` bytes, `LENGTH = 8` bits, `PREAMBLE = 8` bits (1 bytes). |
| `PCNF1` | `0x518` | `0x000300FF` | `MAXLEN = 255`, `STATLEN = 0`, `BALEN = 3` (3 bytes base + 1 byte prefix = 4-byte on-air address), `ENDIAN = 0` (Little-Endian), `WHITEEN = 1` (Hardware Data Whitening). |
| `BASE0` | `0x51c` | `0x76412900` | Base address 0 (pairing / discovery). |
| `PREFIX0` | `0x524` | `0x71` (byte 0) | Address prefix 0. Full Access Address 0 = `0x71764129`. |
| `CRCCNF` | `0x534` | `0x00000103` | `LEN = 3` (24-bit CRC), `SKIPADDR = 1` (address is not included in CRC calculation). |
| `CRCPOLY` | `0x538` | `0x0000065B` | CRC24 polynomial: $x^{24} + x^{10} + x^9 + x^6 + x^4 + x^3 + x + 1$. |
| `CRCINIT` | `0x53c` | `0x00555555` | Initial value of CRC generator. |
| `RXADDRESSES` | `0x530` | `0x00000001` / `0x03` | Enabling reception on logical address 0 (pairing) or 0+1 (tracking). |
| `TXADDRESS` | `0x52c` | `0x00000000` / `0x01` | Transmission logical address. |

## K05899
Source `D0484`, original line64, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### B. Frequency Grid and Channels
1. **Startup Channels (Startup Groups):**
   - **Pipe 0 (pairing and service discovery):** channels `1, 37, 77` (correspond to carrier frequencies 2401, 2437, 2477 MHz).
   - **Pipe 1:** channels `4, 78` (2404, 2478 MHz).
2. **Operating Frequency Hopping Grid (Channel Hopping):**
   - Frequencies are calculated by the formula:
     $$F = 2400 + 2 \times (\text{channel\_index} + 1) \quad \text{MHz} \quad (\text{range 2402..2480 MHz, even steps})$$
   - The active channel set is defined by a 40-bit bitmask (5 bytes), transmitted by the station during the pairing phase.

## K05900
Source `D0484`, original line75, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. Over-the-Air Addressing and Solving Address `31:32:33:33`

## K05901
Source `D0484`, original line77, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### A. Solving Address `31:32:33:33`
When requesting status via `tracker_test tracker` or the `GetTrackerStatus` API, the headset outputs:

## K05902
Source `D0484`, original line85, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Disassembly of the `Swift 2.0` tracker firmware (VA `0x8ac4..0x8acc`) determined:
- At address VA `0x16515`, the string constant `"1233\0"` (`0x31, 0x32, 0x33, 0x33, 0x00`) is located.
- The telemetry packet assembly function reads this word:

## K05903
Source `D0484`, original line93, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- The `tracker_test` utility (function `0x7314..0x7340`) reads these 4 bytes and formats them with the string `addr:%02x:%02x:%02x:%02x`.
- **Conclusion:** `31:32:33:33` is a **static protocol token (ASCII "1233")** transmitted in the packet header, and **not** a unique hardware radio MAC address.

## K05904
Source `D0484`, original line96, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### B. Over-the-Air Pairing Access Address (Pairing Access Address)
- In the station initialization function `PhoenixSta` (address `0x13144`):
  - Base address: `BASE0 = 0x76412900`.
  - Prefix: `PREFIX0 = 0x71`.
  - With `BALEN = 3`, the address is formed from the prefix (`0x71`) and the 3 least significant bytes of the base (`0x29`, `0x41`, `0x76`).
  - **Over-the-air 32-bit Pairing Access Address:** **`0x71764129`** (transmitted over the air as the sequence `0x71, 0x76, 0x41, 0x29`).

## K05905
Source `D0484`, original line103, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### C. Dynamic Over-the-Air Tracking Address (BASE1)
The operating address `BASE1` and prefix `PREFIX1` are uniquely generated during the pairing phase for each "Station — Tracker" pair:
1. The station generates or transmits its 4-byte identifier (`peer_seed`, on the headset equal to `[PRIVATE_LINK_VALUE]` / `74:73:54:15`).
2. The tracker reads its unique hardware chip identifier from Nordic FICR `DEVICEID[1]` (at address `0x10000064` via VA function `0x7d18`).
3. Both sides perform byte-by-byte addition modulo 256:
   $$\text{combined}[i] = (\text{peer\_seed}[i] + \text{device\_id}[i]) \pmod{256} \quad \text{for } i \in \{0, 1, 2, 3\}$$
4. The resulting 32-bit word is bit-reversed by the ARM `RBIT` instruction (VA function `0x58f8`).
5. The result is written to register `BASE1` on the station and tracker, and the negotiated `peer_prefix` into `PREFIX1`.

## K05906
Source `D0484`, original line114, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 4. Pairing Protocol (Pairing Handshake State Machine)

## K05907
Source `D0484`, original line140, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Byte-by-Byte Structure of Pairing Packets

## K05908
Source `D0484`, original line142, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### 1. Pairing Beacon / Request from Station (9 bytes)
- `Byte 0`: Payload length = `0x09`.
- `Byte 1`: Pairing command code.
- `Bytes 2..8`: Station service parameters and protocol marker.

## K05909
Source `D0484`, original line147, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### 2. Tracker Identity Response from Tracker (13 bytes)
Formed by subroutine VA `0x6844`:
- `Byte 0`: Payload length = `0x0D` (13 bytes).
- `Bytes 1..2`: Type field `0x0002` (Little-Endian `0x02, 0x00`).
- `Bytes 3..4`: Protocol version field `0x000A` (Little-Endian `0x0A, 0x00`).
- `Bytes 5..8`: **Tracker hardware chip ID (Nordic FICR `DEVICEID[1]`)**.
- `Bytes 9..12`: Battery status, bracket status, and tracker hardware revision.

## K05910
Source `D0484`, original line155, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### 3. Network Configuration from Station (17 or 21 bytes)
Processed by subroutine VA `0x68f0` (branch `(len & 0xFB) == 0x11`):
- `Byte 0`: Payload length (`0x11` = 17 or `0x15` = 21).
- `Bytes 1..6`: Pairing service flags and tracker ID confirmation.
- `Bytes 7..11`: **40-bit operating frequency mask**. Function VA `0x7508` translates each set bit into an even radio channel: $\text{Channel} = 2 \times (\text{bit\_index} + 1)$ (from 2 to 80).
- `Bytes 12..15`: **Station `peer_seed`** (stored in tracker RAM `0x20000fd8`).
- `Byte 16`: **`peer_prefix`** (stored in tracker RAM `0x20000fd0`).

## K05911
Source `D0484`, original line165, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 5. Headset Hardware Receive Pipeline: Station MCU and Dual-Channel SPI

## K05912
Source `D0484`, original line167, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the Pico 4 Enterprise headset, the station radio module is implemented on a separate **Nordic nRF52840** microcontroller, physically connected to the Qualcomm Snapdragon XR2 processor by two independent SPI buses:

## K05913
Source `D0484`, original line169, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### A. AP SPI Interface (`/dev/stationdev`)
- **Bus in Linux:** node `/dev/stationdev` (Qualcomm QUP SPI master to nRF52 SPIS slave).
- **Interrupt:** GPIO 84 (`station`).
- **Handlers in MCU firmware:** `ap_spis_event_handler`, `ap_spi_recv_analysis`, `ap_spi_reply`, `ap_spi_report`.
- **Purpose:** Asynchronous control commands initiated by the Android process `stationservice` and utilities:
  - `StartTrackerPairing` / `stop pairing` — starting and stopping tracker search.
  - `UnbondTracker` — removing the pairing from station flash memory.
  - `SetTrackerWorkMode` — switching radio frequency and interval mode.
  - `UpdateFWTracker` — transmitting the tracker firmware update binary image (`tracker_ota_process`).
  - `GetTrackerStatus` — reading the cached table of paired trackers (64-byte records).

## K05914
Source `D0484`, original line180, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### B. BP SPI Interface (Baseband Processor / Qualcomm Hexagon SLPI DSP)
- **Handlers in MCU firmware:** `bp_spis_event_handler`, `bp_spi_send_with_timestamp`, `tracker_imu_info`.
- **Purpose:** High-speed synchronous IMU data delivery channel at a frequency of 200 Hz.
- **Implementation Details:**
  - Each packet received over the radio is immediately tagged with the hardware timestamp of the station timer `time_ticks0` and transmitted over SPI directly to the Qualcomm Hexagon DSP (SLPI) sensor processor.
  - The pipeline is completely isolated from the Linux / Android kernel scheduler, which eliminates OS jitter and latency during motion transmission.
  - Synchronized with optical flashes of the tracker LEDs via the camera frame synchronization signal (`fsin_count`).

## K05915
Source `D0484`, original line190, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 6. Telemetry Packet Format and Parsing in `libpvrtrackingservice.so`

## K05916
Source `D0484`, original line192, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

From the Hexagon DSP, packets are transmitted via the FastRPC interface (`mSlpiDataReader` / `SnsLowLatBridge`) to the userspace library `libpvrtrackingservice.so`.  
Primary parsing is performed by the function `pvr::SlpiDataParser::trackerDataParserV2(slpi_payload_data_t*, unsigned long)` (virtual address `0x96e44`, size 1004 bytes).

## K05917
Source `D0484`, original line195, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### A. Packet Header `slpi_payload_data_t`

## K05918
Source `D0484`, original line197, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Offset | Type | Purpose |
|---|---|---|
| `+0x00..+0x01` | `uint16_t` | Station packet service header. |
| `+0x02` | `uint8_t` | **Tracker descriptor byte:**<br/>- Bits `0..5`: `id = (byte & 0x3F) - 2` (0=Tracker 1, 1=Tracker 2, 2=Tracker 3).<br/>- Bit `6` (`0x40`): timestamp bit width (0 = 16 bits, 1 = 32 bits).<br/>- Bit `7` (`0x80`): presence of extended header (IMU offset: +5 or +7 bytes). |
| `+0x03..` | `uint16_t` / `uint32_t` | Hardware packet timestamp in microseconds (`time_ticks0`). |

## K05919
Source `D0484`, original line203, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### B. Byte-by-Byte Layout of IMU Payload (Starting from IMU Data Offset)

## K05920
Source `D0484`, original line205, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Offset | Type | Purpose | Note |
|---|---|---|---|
| `+0x00` | `int16_t` | Gyroscope $X$ (raw) | Sampling frequency 200 Hz, sensor ICM-42688 |
| `+0x02` | `int16_t` | Gyroscope $Y$ (raw) | |
| `+0x04` | `int16_t` | Gyroscope $Z$ (raw) | |
| `+0x06` | `int16_t` | Accelerometer $X$ (raw) | Sensitivity $\pm 16$g |
| `+0x08` | `int16_t` | Accelerometer $Y$ (raw) | |
| `+0x0A` | `int16_t` | Accelerometer $Z$ (raw) | |
| `+0x0C` | `uint16_t` | Magnetometer $X$ (raw) | Internal magnetometer of tracker |
| `+0x0E` | `uint16_t` | Magnetometer $Y$ (raw) | |
| `+0x10` | `uint16_t` | Magnetometer $Z$ (raw) | |
| `+0x12` | `int16_t` | Sensor temperature | Formula: $T = \text{raw} / 132.48 + 25.0^\circ\text{C}$ |
| `+0x14` | `uint8_t` | Packet counter / Sequence ID | Incremented on every transmitted frame |
| `+0x15` | `uint8_t` | Power flags | Bit 0: charging status (1=charging); bits 1..7: mode |
| `+0x16` | `uint16_t` | Internal tracker timestamp | Local timer ticks of nRF52 |
| `+0x18` | `uint8_t` | Calibration readiness status | |
| `+0x19` | `uint8_t` | **Fusion validity flag** | If value `!= 0`, orientation quaternion is valid |
| `+0x1A` | `uint8_t` | Bracket operating mode | 0=detached, 3=inserted into mount |
| `+0x1B` | `float` (32b LE) | **Orientation quaternion $q_0$ / $w$** | **Calculated on tracker (Onboard Fusion)** |
| `+0x1F` | `float` (32b LE) | **Orientation quaternion $q_1$ / $x$** | **Calculated on tracker (Onboard Fusion)** |
| `+0x23` | `float` (32b LE) | **Orientation quaternion $q_2$ / $y$** | **Calculated on tracker (Onboard Fusion)** |
| `+0x27` | `float` (32b LE) | **Orientation quaternion $q_3$ / $z$** | **Calculated on tracker (Onboard Fusion)** |
