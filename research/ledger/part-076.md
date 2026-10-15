# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K06001
Source `D0761`, original line526, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Remaining checks

## K06002
Source `D0761`, original line544, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Audit corrections

## K06003
Source `D0761`, original line557, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Full technical data of Roy used in the experiment

## K06004
Source `D0761`, original line559, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Roy version 1788551713, date Sep 4 2026, Zephyr string 3.3.99-ncs1;
serial number/BLE address: `cf714aecbacb` / `[DEVICE_MAC]`.
Stock controllers: `b4eda8f6a5eb` (Left) and `95f963bc6ce4` (Right).
They were present in the pairing table before the experiments and after recovery.
Early controller disconnections were noted prior to the first SRAM tests; during
the experiments, Roy also rebooted, so the connection cannot be declared continuous.

## K06005
Source `D0761`, original line566, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

FICR starting from `0x10000100`: PART=`0x52833`, VARIANT=`0x41414230`,
PACKAGE=`0x2008`, RAM=`0x80` KiB, FLASH=`0x200` KiB.
Running processes: `vrserver` and `XRService`; PIDs and fd numbers should not be recorded
as permanent. Node `/dev/spidev0.1` has modalias `hid-over-spi`, but is bound
to the `spidev` driver; this is not a ready-made API for reading arbitrary radio packets. `hidraw`
was not found during the initial inventory. Valve implementation is `driver_cv.so`;
`frame_hmd` manifest has `resourceOnly=true`.

## K06006
Source `D0761`, original line574, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Resources: `/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/`.
Related copies exist in `deckard` resources. Copied without running:
`roy_dongle_fw.hex`, `roy_dongle_timesync_client_fw.hex`,
`setup_controller_radio.sh`, `disable_spi_hid.sh`, `program_ble.sh`, `openocd.cfg`.
Setup/program/disable scripts can program the device or change the SPI binding;
these are not safe inventory tools and were not executed in this session.

## K06007
Source `D0761`, original line581, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The RADIO snapshot was taken during BLE scanning/unconnected controllers, not during
PICO RX. Values may change dynamically during normal BLE:

## K06008
Source `D0761`, original line584, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Register | Value |
|---|---|
| FREQUENCY | 2 |
| TXPOWER | 0 |
| MODE | 3, BLE1M |
| PCNF0 | 0x00100108 |
| PCNF1 | 0x02030040 |
| BASE0 | 0x89bed600 |
| PREFIX0 | 0x8e |
| RXADDRESSES | 1 |
| CRCCNF | 0x103 |
| CRCPOLY | 0x65b |
| CRCINIT | 0x555555 |
| DATAWHITEIV | 0 |

## K06009
Source `D0761`, original line599, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`BASE0/PREFIX0` correspond to the BLE advertising address, not the PICO pairing address.
Previously claimed Gazell/Valve ESB/CRC16 are not confirmed by this snapshot and Roy image.
The term "integration into VALVE ESB" remains a user goal formulation;
the stock Roy stack actually discovered is BLE/MPSL.

## K06010
Source `D0761`, original line604, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### SWD and memory

## K06011
Source `D0761`, original line606, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

OpenOCD on Frame: 0.12.0+dev-g64fb747da; adapter `linuxgpiod`, SWD:
SWCLK GPIO51 chip9, SWDIO GPIO50 chip9, SRST GPIO164 chip9,
`reset_config srst_push_pull srst_nogate`, target `nrf52.cfg`.
Access was obtained without `nrf52 recover`, erase, flash programming, or explicit reset commands.
The fact of spontaneous/stock reboots of Roy during tests is separately reflected above.

## K06012
Source `D0761`, original line612, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Read 262144 bytes of live flash (0..0x40000), live RAM — 131072 bytes
(`0x20000000..0x20020000`). The remaining flash regions, UICR, pairing storage
are not declared completely read/saved.

## K06013
Source `D0761`, original line616, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

RTT control block=`0x2001064c`, originally MaxUp=3/MaxDown=3; descriptor=24 bytes.

## K06014
Source `D0761`, original line618, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| RTT buffer | Address | Size |
|---|---|---|
| Terminal Up0 | 0x20012060 | 4096 |
| Terminal Down0 | 0x20012020 | 64 |
| MFG Up1 | 0x20011919 | 1024 |
| MFG Down1 | 0x20011519 | 1024 |
| MFG Up2 | 0x20011018 | 1024 |
| MFG Down2 | 0x20010c18 | 1024 |

## K06015
Source `D0761`, original line627, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Code is located starting at `0x2001151c` in the reserved manufacturing buffers area;
data — `0x20010c18`, 2048 bytes; private stack top=`0x20011408`.
For the duration of the test, MaxUp/MaxDown=1/1; Terminal0 is preserved. The reservation is based
on the investigated current layout and is not a guarantee that these buffers are free
on any build or under any manufacturing commands.

## K06016
Source `D0761`, original line633, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

MPU initially CTRL=5. Region0 flash: base=0, RASR=0x02020025;
region1 RAM: base=0x20000001, RASR=0x110b0021 (XN);
region2 guard: base=0x20018302, RASR=0x150b000b; 3..7 disabled.
The SRAM call temporarily sets region1 RASR=0x010b0021. An MPU snapshot requires
a stable RNR selection; reading while the CPU is running may conflict with the core.
The cause of the reboots could not be localized; removing XN or disabling the MPU
does not prove a successful MPSL callback.

## K06017
Source `D0761`, original line641, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Thumb entry points found: open 0x10581, request 0x18949, close 0x189bf.
MPSL configuration `0x200003b0`: count=1, contexts=`0x2001056c`;
one session context 48 bytes, callback pointer +0x20, status +0x26.
Stock timesync callback found at 0x19c45. These addresses and sizes pertain
to this image and are not guaranteed by another MPSL release.

## K06018
Source `D0761`, original line647, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The experiment uses a 16-byte request structure and a 8-byte callback result.
Assumed signals: START0, TIMER0=1, RADIO2, EXTEND_FAILED3,
EXTEND_SUCCEEDED4, BLOCKED5, CANCELLED6, SESSION_IDLE7, INVALID_RETURN8,
SESSION_CLOSED9, OVERSTAY10. Actions: NONE0, EXTEND1, END2, REQUEST3.
Verification by header and disassembler does not replace hardware verification of the ABI.
The module contains a raw RX callback and a ring of four entries with DMA up to 258 bytes;
real RX was not enabled. Initial scheduling test: slot=500 µs,
interval=10000 µs, duration=0.5 s; it registered no successful slots.

## K06019
Source `D0761`, original line656, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The loader saves CPU registers and a separate stack; verifies privileged Thread
mode; executes entry up to BKPT and restores original registers. Fixed OpenOCD
register name (`xpsr`), target state initialization via `poll`, TCP_NODELAY,
chunking of RAM writes, and mandatory code readback after writing. These fixes cannot
be considered proof of the correct calling context for all MPSL APIs.
Upon detecting the disappearance of the magic, the module is considered lost, and the test failed.

## K06020
Source `D0761`, original line663, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Direct RADIO profiling from JSON is provided only as a research interface;
no verified PICO profile exists. Do not run it with previous fabricated
parameters and do not change RADIO outside the BLE co-existence mechanism.

## K06021
Source `D0761`, original line667, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stock image capabilities found and their limitations

## K06022
Source `D0761`, original line669, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Image strings: shell `Dongle~$`, `devmem read/write/load`, RTT, wireless
state/enable/disable/scan/disconnect, pairing, manufacturing/DTM.
The presence of a string does not confirm an accessible host dispatcher or its safe ABI.
`vrcmd --cmdhmd help` returned unknown command; `get_radio_mode` — 0.
Disassembled `set_radio_mode` is also linked to old USB product IDs
0x2101/0x2102; this is not a proven onboard Roy switch to PICO/ESB.
Modes/pairing were not changed by this API, and processes and SPI binding were not disabled.

## K06023
Source `D0761`, original line677, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## PICO SDK and separate official path via PICO headset

## K06024
Source `D0761`, original line679, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

SDK sources 3.4.0 confirm Object Tracking flow:
`CheckMotionTrackerNumber` → event `RequestMotionTrackerCompleteAction`
with trackerCount/trackerIds → `GetMotionTrackerLocation`.
`RequestMotionTrackerCompleteAction` here is an event, not a separate call command.
Example: `pico-sdk/Runtime/BuildingBlocks/PXR_ObjectTrackingBlock.cs`;
in the read example, `MotionTrackerNum.TWO` was requested.
`MotionTrackerLocation` in `Runtime/Scripts/Utils/PXR_Type.cs` contains pose,
linear/angular velocity and linear/angular acceleration (fixed float[3]).
Return code and isValidPose must be checked separately. The presence of the API does not confirm
transmission of real poses from an installed application to SteamVR in this session.

## K06025
Source `D0761`, original line690, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The project contains Unity streamer and Python receiver/calibration/VMT modules. This is
a separate architecture utilizing the PICO runtime, and it does not satisfy
the standalone onboard Frame requirement. OpenVR-SpaceCalibrator is a possible source
of space alignment methods, but here it was not deployed/tested on two HMDs.

## K06026
Source `D0761`, original line697, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Header | Type | Value |
|---|---|---|
| magic | u32 | 0x31425450 |
| version | u16 | 1 |
| type | u16 | 1 Frame / 2 Hello |
| length | u32 | payload size |

## K06027
Source `D0761`, original line704, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Header=12 bytes, little-endian. Hello=UTF-8 JSON. Frame: t f64, frame u32,
hmd 7×f32 (px,py,pz,qx,qy,qz,qw), count u8; then count entries of 65 bytes each:
id i64, valid u8, pose 7×f32, linVel 3×f32, angVel 3×f32, battery f32.
SDK accelerations exist, but protocol v1 does not transmit them. Unity pose is
left-handed, +Y up/+Z forward, meters. Raw SDK velocities require separate
unit verification and conversion; do not declare them already in an aligned system.
Pose conversion to PC: position=(x,y,-z), quaternion=(-qx,-qy,qz,qw).
Space alignment uses R,T/Kabsch: p'=R p+T, q'=q_align q.
Does not promise accuracy without real calibration and verification of HMD temporal alignment.

## K06028
Source `D0761`, original line714, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For a separate Pico-bridge, USB reverse must explicitly address PICO:

## K06029
Source `D0761`, original line720, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is a reference note on the existing path, not a standalone configuration command, nor
confirmation of end-to-end APK→VMT launch. The native C++ framework and VMT OSC are different
interfaces: their compatibility cannot be assumed without IPC. Tracker
roles/serial numbers saved by Python are not registered in SteamVR by themselves.

## K06030
Source `D0761`, original line725, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Unconfirmed information from the previous artifact

## K06031
Source `D0761`, original line727, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

They are preserved to continue research, but are not used as technical facts.
The full previous revision is saved alongside in a separate `before-codex` file.

## K06032
Source `D0761`, original line730, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Previously claimed | Correct status |
|---|---|
| PICO Station nRF52840/PhoenixSta2.96, IRQ84, stationservice/Binder/pvrtrackingservice | information from old research; not fully established against the live device in this session |
| TR0189 HW03, ICM-42688, Keil RTX5.7, magnetometer, SGM41562, bracket I2C | require separate verification of sources/binaries; cannot be deduced from model A8110 |
| RSA-2048 and public.pem confirm Swift signature | presence of container/file does not replace signature and structure verification |
| Pairing address 0x76412900/0x71, tracker ASCII "1233", channel2424MHz | not confirmed for real PICO RX; do not use the old C example as a working profile |
| 64-byte packet Q/acc/gyro/mag/temp/battery/LED | experimental Python codec scheme; over-the-air capture and matching ID are absent |
| Commands 0x20/0x30, packet [03,03,20,01,00] | not confirmed by tracker transmission/response; serial write does not imply RF TX |
| Static ON=0x01/SetTrackerLed | string/software clues; successful switching of a live tracker is not proven |
| 200Hz IMU, 60–90Hz real Frame cameras | model frequencies/assumptions, not measurements of a real standalone pipeline |
| 850/940nm, global shutter, UVC/OpenXR/Vulkan camera access | requirements/options, not established properties of the available camera API |
| 25+ hours or 3–5 hours Static ON, 60–80mA | real power measurements are absent |
| LED non-coplanarity eliminates any PnP flips | overly strong claim; does not guarantee absence of ambiguities, correspondence errors, and noisy solutions |
| PnP <0.1ms, distortion <1µs, 0.42mm, 0.07px,100% capture | synthetics/unverified performance assertions, not measurements on Frame |
| 11/19/23/27 tests,100% coverage | outdated counters; currently 45 pass, coverage100% was not measured |
| nRF52840/nRF5340 template — firmware for Frame | this is a template; real Roy is nRF52833, firmware was not installed and is not required as a permitted method |
| timeshare is impossible | unproven conclusion: stock has MPSL, SRAM execution is permitted and confirmed; stable concurrent servicing has not yet been implemented |
| external USB station solves the current task | ruled out by the user; do not consider it a completion option |

## K06033
Source `D0761`, original line749, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

LED coordinates preserved in previous research, **physical tracker calibration not
validated in this session** (units and reference frame require verification):

## K06034
Source `D0761`, original line752, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Index | X | Y | Z |
|---|---|---|---|
| 0 | +0.007426 | +0.015968 | -0.009357 |
| 1 | -0.004626 | +0.013630 | -0.004681 |
| 2 | -0.009299 | -0.001385 | -0.009882 |
| 3 | -0.003668 | -0.014409 | -0.009500 |
| 4 | +0.010146 | -0.018294 | -0.009431 |
| 5 | +0.022000 | -0.013442 | -0.004945 |
| 6 | +0.024650 | -0.001246 | -0.009246 |
| 7 | +0.019313 | +0.013377 | -0.004308 |

## K06035
Source `D0761`, original line763, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Old serial/role mappings `[TRACKER_SERIAL]`→waist,
`[TRACKER_SERIAL]`→left foot, `[TRACKER_SERIAL]`→right foot are configuration
details from earlier research, not the current list of trackers discovered via Roy.

## K06036
Source `D0761`, original line767, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Existing optical modules (`constellation`, `blob_detector`, `distortion`,
`pnp_solver`, `stereo_pnp`, `fusion_ekf`, `camera_stream`, `steam_frame_tracker`)
are useful for development and synthetics. The presence of source code does not confirm access
to real Frame cameras, factory calibration, strobe synchronization, and pose
publication. IMU double integration does not provide stable absolute position.

## K06037
Source `D0761`, original line773, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Next plan and completion criteria

## K06038
Source `D0761`, original line775, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Localize the cause of failure after MPSL request: calling context, ABI,
   callback permissions/MPU, and fault/reset source. Avoid repeating a single
   failing test without new data, especially with active controllers.
2. Verify stable timeslots with both Valve controllers turned on,
   with guaranteed session closing/RAM return and without reflashing.
3. On PICO and Swift, confirm actual application configuration:
   PHY, PCNF, CRC, addressing, hopping/pairing, and packet framing. Correlate
   real captures with SDK ID/time and readings during known motion.
4. Implement a safe path for outputting raw packets from Nordic to Frame SoC:
   a proven stock transport or temporary RAM/RTT channel. SWD polling is
   a diagnostic mechanism, not a proven production latency transport.
5. Confirm camera access/calibration/IR LED visibility and necessary
   sync commands, then fuse optics with IMU in the Frame system.
6. Finish the native linuxarm64 SteamVR provider with real IPC or verify
   a suitable VMT backend on the Frame itself. Verify IDs, roles, pose validity,
   battery, timestamps, disconnect/reconnect. Mock must not turn on by itself.
7. Definition of done: real PICO tracker poses in SteamVR on Frame,
   PICO headset and PC do not participate at runtime; no dongle; Valve controllers
   continue working; real loss/jitter/latency/accuracy measured;
   reboot restores stock state without flash patch.

## K06039
Source `D0761`, original line796, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Files, evidence and checksums

## K06040
Source `D0761`, original line798, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Project: `K:\AI\pico`; durable knowledge is its `AGENTS.md`, summary status is
`docs/onboard-roy-status.md`. The original Desktop plan:
`F:\Shlik\Desktop\pico-tracker-steam-frame-plan.md` remains a historical plan;
this revised artifact describes the actual result.

## K06041
Source `D0761`, original line803, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

New sources: `pc/radio/roy_probe.py`, `pc/radio/ram/roy_ram_rx.c`,
`roy_ram_rx.ld`, `roy_ram_loader.py`, `roy_ram_recover.py`;
tests `pc/tests/test_roy_probe.py`, `test_roy_ram_loader.py` and fixed
radio/interpreter tests. Do not confuse the read-only probe with the RAM loader, which
intentionally modifies RAM/MPU and requires runtime recovery.

## K06042
Source `D0761`, original line809, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Local live dumps/logs and experiment JSON are located in the working folder below.
`trap.json` only proves RAM debug-return; `scheduling8.json` contains
an old false-positive exit0 and lost magic, therefore does not prove MPSL;
`scheduling9.json` is a failed test; `final-mpu.json` confirms the final MPU.
Checkpoints `*.recovery.json` contain the original RAM buffers/RTT and MPU attributes;
OpenOCD logs reflect restarts and errors, not only successful stages.

## K06043
Source `D0761`, original line817, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Working folder: `[USER_HOME]\Documents\Codex\2026-10-05\unity-plugin-unity-claude-plugins-official-2`.

## K06044
Source `D0761`, original line819, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| File | Bytes | SHA256 |
|---|---|---|
| `work\firmware\Swift2.0.bin` | 193385 | `41bd2d24dd9293e7d17c62ab18acd1001d6085da393123a67d226b28d2132e54` |
| `work\firmware\Swift2.0-image.bin` | 96532 | `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89` |
| `work\frame-stock\roy\roy_dongle_fw.hex` | 684106 | `8f6f382c6715f28dfb9f3f1d9f26dfad29f147f8fffcff7fafdbaebe2fa8436e` |
| `work\frame-stock\roy\roy_dongle_timesync_client_fw.hex` | 587092 | `57ed14c0999611ac1407c786eaeada96779cdf7be637ba2befb1386241016e53` |
| `work\frame-stock\roy\roy_dongle_fw.hex.bin` | 237920 | `0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50` |
| `work\frame-stock\roy-live-flash.bin` | 262144 | `24e8b9bf929edc4780b331ecfaeeb3c05211c602ed36621262c71e648d97d470` |
| `work\frame-stock\roy-live-ram.bin` | 131072 | `1701354dfcfc9e3c8252a102785b7bb452e1c232cecb193def9b2af4ab063a43` |
| `work\frame-stock\driver_cv.so` | 4879896 | `001ea72f73c613bf5377a735a129f22f21da247b285dc38ede8b318dc7e07eb2` |
| `work\frame-stock\XRService` | 50417472 | `9e1c6f57a7e3613c69dbf7f61c62d0cfd4b8a664889791907772b03a62f9f6d8` |
| `work\frame-stock\roy_ram_rx.elf` | 6592 | `f07eb5bf1e4b97129ad62fa7972c6045098c1c6c9cf01c66fc32475ad3e63b74` |

## K06045
Source `D0761`, original line832, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Previous revision saved: `[USER_HOME]\.gemini\antigravity\brain\a953ec36-1362-4486-a187-67774da38931\pico_tracker_steam_frame_research.before-codex-20261005T181447Z.md`.

## K06046
Source `D0761`, original line834, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The updated artifact does not declare the current implementation complete.

## K06047
Source `D0761`, original line1591, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Resolution of core uncertainties: the exact method of Swift packet reception and pairing address (2026-10-06)

## K06048
Source `D0761`, original line1593, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Based on static analysis (Capstone) of the PhoenixSta station firmware (`PhoenixSta_sv2.96_PR_20250805_b296_app.bin`), Swift 2.0 tracker firmware (`Swift2.0_sv1.89_20241213_b189_app.bin`), tracking libraries (`libpvrtrackingservice.so`, `libtrackerclient.pxr.so`), and an independent review:

## K06049
Source `D0761`, original line1595, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 1. Separation of DTM test mode and Swift ESB operating radio protocol

## K06050
Source `D0761`, original line1597, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Analysis of cross-references and string tables of `PhoenixSta` revealed the presence of two independent radio configurations in the station firmware:

## K06051
Source `D0761`, original line1599, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Bluetooth Direct Test Mode (DTM / RF Certification, function `0x13144`):**
   - Used by certification radio measurement procedures (`DTM op test!`, `DTM recv test!` at addresses `0x1a960` / `0x1a99c`).
   - Registers: `PCNF0 = 0x108`, `PCNF1 = 0x300FF` (`WHITEEN` bit 25 is 0, whitening disabled), `CRCCNF = 0x103` (24-bit CRC, $SKIPADDR=1$), `CRCPOLY = 0x65B`, `CRCINIT = 0x555555`, `MODE = 4` (`Ble_2Mbit`).
   - Address: `BASE0 = 0x76412900`, `PREFIX0 = 0x71` $\to$ Access Address **`0x71764129`**. This is the standard Direct Test Mode sync word from the Bluetooth Core Specification (PRBS9 test pattern), and **not** the Swift operating pairing address.

## K06052
Source `D0761`, original line1604, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2. **Pico Swift operating protocol (Nordic Enhanced ShockBurst — ESB):**
   - Implemented in `PhoenixSta` at addresses `0x924c..0x9266`, `0x9830..0x9848`, `0x9e8c..0x9ebc` and in `Swift2.0` at addresses `0x50ec`, `0x5480..0x54a6`, `0x7560..0x7600`.
   - **Packet framing:**
     - `PCNF0 = 0x00040008`: $LFLEN = 8$ bits, $S_0 = 0$ byte, $S_1 = 4$ bits (ESB PID + NO_ACK), $PLEN = 0$ (8-bit preamble).
     - `PCNF1`: `ENDIAN = 1` (bit 24 = 1, Big-Endian over the air transmission), `WHITEEN = 0` (bit 25 = 0, hardware whitening disabled), `BALEN = 4` (or 3).
   - **CRC checksum:**
     - `CRCCNF = 0x00000002` (16-bit CRC, $SKIPADDR = 0$ — **over-the-air address is included in the CRC calculation**).
     - `CRCPOLY = 0x00011021` (CRC16-CCITT: $x^{16} + x^{12} + x^5 + 1$).
     - `CRCINIT = 0x0000FFFF`.
   - **Default base addresses (Nordic ESB):**
     - `BASE0 = 0xE7E7E7E7` (canonical Nordic ESB base address).
     - `BASE1 = 0x43434343` (base address of logical channels 1..7).
     - `PREFIX0 = 0x23C343E7` (Pipes 0..3: prefixes `0xE7`, `0x43`, `0xC3`, `0x23`).
     - `PREFIX1 = 0x13E363A3` (Pipes 4..7: prefixes `0xA3`, `0x63`, `0xE3`, `0x13`).
     - Default full Pipe 0 address: `0xE7, 0xE7, 0xE7, 0xE7, 0xE7`.

## K06053
Source `D0761`, original line1620, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 2. End-to-end telemetry reception path: from tracker to host

## K06054
Source `D0761`, original line1646, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- **AP SPI (`/dev/stationdev`, IRQ 84):** used by Android userspace services (`stationservice`, `libstationclient`, `tracker_test`) for pairing commands, calibration, status, and OTA tracker updates.
- **BP SPI (Baseband / SLPI DSP):** serviced by `bp_spis_event_handler` and `bp_spi_send_with_timestamp`. Transmits raw 200 Hz IMU packets with microsecond timestamps `time_ticks0` directly to the Qualcomm Hexagon DSP (SLPI) sensor processor, bypassing the Android OS scheduler.
- **Parsing in `libpvrtrackingservice.so` (SlpiDataParser::trackerDataParserV2, VA `0x96e44`):**
  - Slot index: `(payload[2] & 0x3F) - 2` (0 = Tracker 1, 1 = Tracker 2, 2 = Tracker 3).
  - IMU offset: 5 or 7 bytes (depends on bits 6/7 of descriptor byte).
  - IMU axes: Gyro XYZ (`int16_t` $\times 3$), Accel XYZ (`int16_t` $\times 3$), Mag XYZ (`uint16_t` $\times 3$).
  - Temperature: `int16_t`, recalculation $T = \text{raw} / 132.48 + 25.0^\circ\text{C}$ (ICM-42688).
  - Offset `0x1B`: **4 $\times$ `float32`** ($q_0, q_1, q_2, q_3$), computed on the tracker (Onboard Sensor Fusion) and valid when flag `payload[0x19] != 0`.

## K06055
Source `D0761`, original line1655, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 3. Solution to address `31:32:33:33` and pairing mechanism

## K06056
Source `D0761`, original line1657, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Solution to `addr: 31:32:33:33`:**
   - In the `Swift 2.0` tracker firmware at VA `0x16515`, there is a string constant `"1233\0"` (`0x31, 0x32, 0x33, 0x33, 0x00`).
   - Function VA `0x8ac4..0x8acc` reads the pointer to `"1233"`, reads it as a 32-bit word `0x33333231`, and writes it with the instruction `str.w r3, [sp, #0x17]` at offset +3 into the telemetry packet payload.
   - The `tracker_test` utility (function `0x7314..0x7340`) reads these 4 bytes and prints `addr:%02x:%02x:%02x:%02x`.
   - **Conclusion:** `31:32:33:33` is a **static protocol stub marker (ASCII "1233")**, and not a radio hardware MAC address.

## K06057
Source `D0761`, original line1663, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2. **Over-the-air addressing and pairing:**
   - **Pipe 0 (discovery and pairing):** starting channels `1, 37, 77` with Pipe 0 base address `0xE7, 0xE7, 0xE7, 0xE7, 0xE7`.
   - **Identity Response (13 bytes, VA `0x6844`):** the tracker sends a response to the pairing request (length 9), where bytes 5..8 contain the hardware chip ID from Nordic FICR `DEVICEID[1]` (`0x10000064` via VA `0x7d18`), and prints `"pair fine !!!!!"` to the log.
   - **Network Config (17 or 21 bytes, VA `0x68f0`):** the station transmits a 40-bit even frequency mask (bytes 7..11), a 4-byte `peer_seed` (bytes 12..15), and a prefix (byte 16).
   - **Symmetric calculation of BASE1 address:**
     $$\text{word}[i] = (\text{peer\_seed}[i] + \text{device\_id}[i]) \pmod{256} \quad (i \in [0..3])$$
     followed by bitwise inversion of bit order (`RBIT`, VA `0x58f8`).
   - The tracker and station switch to address `BASE1` (`pair_switch2_connect`) and transition to continuous 200 Hz IMU telemetry exchange mode.

## K06058
Source `D0762`, original line3, SHA256 `8add8476e368cd6234682c46be8e3d017b10a157f892ad51f3a4622ecab7f544`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Date:** 2026-10-05 (Europe/Paris)  
**Frame:** `[DEVICE_IP]` (wlan0), USB NCM `10.86.200.233`  
**PC:** `[DEVICE_IP]`, USB NCM `10.86.200.234`

## K06059
Source `D0762`, original line7, SHA256 `8add8476e368cd6234682c46be8e3d017b10a157f892ad51f3a4622ecab7f544`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Status (done)

## K06060
Source `D0762`, original line9, SHA256 `8add8476e368cd6234682c46be8e3d017b10a157f892ad51f3a4622ecab7f544`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Component | State |
|-----------|-----------|
| `steamos-devkit-mode` | ON (`/etc/steamos-devkit-enabled`) |
| `steamos-devmode` | ON (writable root, pacman keyring) |
| `sshd` | active, port 22 |
| SSH key PC → Frame | passwordless (`steamos@[DEVICE_IP]`) |
| `adbd` | active, USB gadget `ffs.adb` |
| `ADB_ALLOW_ROOT` | true |
| ADB auth prompts | disabled ("authentication not required") |
| ADB serial | **`frame`** |
| `adb root` | works (`uid=0`) |
| ADB TCP `:5555` | **none** (this `adbd` has no working `tcpip` / `setprop`) |

## K06061
Source `D0762`, original line22, SHA256 `8add8476e368cd6234682c46be8e3d017b10a157f892ad51f3a4622ecab7f544`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## How to connect from HOME-PC

## K06062
Source `D0762`, original line37, SHA256 `8add8476e368cd6234682c46be8e3d017b10a157f892ad51f3a4622ecab7f544`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

USB: Frame↔PC data cable; NCM + ADB interface simultaneously.

## K06063
Source `D0762`, original line39, SHA256 `8add8476e368cd6234682c46be8e3d017b10a157f892ad51f3a4622ecab7f544`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What was done

## K06064
Source `D0762`, original line41, SHA256 `8add8476e368cd6234682c46be8e3d017b10a157f892ad51f3a4622ecab7f544`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Devkit services: sshd, steamos-devkit-service, xrdp, web-debug portforwards.  
2. SSH ed25519 key `Shlik@HOME-PC-frame` → `~/.ssh/authorized_keys`.  
3. PC `adbkey.pub` → `/adb_keys` and `/data/misc/adb/adb_keys`.  
4. `steamos-devmode enable --no-prompt` — readonly removed, pacman-key populate.  
5. Verified ADB USB: shell + root.

## K06065
Source `D0762`, original line47, SHA256 `8add8476e368cd6234682c46be8e3d017b10a157f892ad51f3a4622ecab7f544`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## TCP ADB limitation

## K06066
Source `D0762`, original line49, SHA256 `8add8476e368cd6234682c46be8e3d017b10a157f892ad51f3a4622ecab7f544`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`adbd` listens only on USB FFS. The command `adb tcpip 5555` closes the transport (`error: closed`), `setprop` is not present in the image. For network, use **SSH**; for ADB — **USB**.

## K06067
Source `D0762`, original line51, SHA256 `8add8476e368cd6234682c46be8e3d017b10a157f892ad51f3a4622ecab7f544`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Useful information

## K06068
Source `D0765`, original line84, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**In plain Russian:** the project goal is standalone PICO trackers on Steam Frame via the built-in Nordic without a dongle or reflashing. Real radio reception, decoding, and a diagnostics app in SteamVR are already working. Full standalone 6DoF is not yet ready. This publication contains our code and tools, not device dumps or a finished product.

## K06069
Source `D0149`, original line327, SHA256 `4abccf72cd74718dc3ef63c72346a6db8c42c6b738c6c85cdfdd69afcc2c0139`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 8.5. Confirmed hardware identifiers of three trackers and over-the-air addresses

## K06070
Source `D0149`, original line329, SHA256 `4abccf72cd74718dc3ef63c72346a6db8c42c6b738c6c85cdfdd69afcc2c0139`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Based on data from the Pico 4 Enterprise headset (`[HEADSET_ADB_SERIAL]`) and inverse conversion of over-the-air addresses with the known Station Seed `[PRIVATE_LINK_VALUE]` (`[0x74, 0x73, 0x54, 0x15]`):

## K06071
Source `D0149`, original line331, SHA256 `4abccf72cd74718dc3ef63c72346a6db8c42c6b738c6c85cdfdd69afcc2c0139`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Slot | Serial number | Lower 16 bits (`chipid`) | Full 32-bit Chip ID (`DEVICEID[1]`) | Slot byte | Over-the-air address for reception (`BASE1` + `PREFIX`) |
|---|---|---|---|---|---|
| **Slot 0 (Tracker 1)** | `[TRACKER_SERIAL]` | `0x3E, 0xED` (`0xED3E`) | **`0xEEACED3E`** (`[0x3E, 0xED, 0xAC, 0xEE]`) | `0x00` (`rbit(0)`) | `C0 00 06 4D` |
| **Slot 1 (Tracker 2)** | `[TRACKER_SERIAL]` | `0xC3, 0xBF` (`0xDFC3`) | **`0xEEADDFC3`** (`[0xC3, 0xBF, 0xAD, 0xEE]`) | `0x80` (`rbit(1)`) | `C0 80 4C EC` |
| **Slot 2 (Tracker 3)** | `[TRACKER_SERIAL]` | `0xAD, 0x1D` (`0x1DAD`) | **`0xEEAE1DAD`** (`[0xAD, 0x1D, 0xAE, 0xEE]`) | `0x40` (`rbit(2)`) | `C0 40 09 84` |

## K06072
Source `D0149`, original line337, SHA256 `4abccf72cd74718dc3ef63c72346a6db8c42c6b738c6c85cdfdd69afcc2c0139`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### Why the full 32-bit ID is critically needed: Command 0x1A (enabling IR flashes)
In the firmware `Swift2.0_sv1.89_app.bin`, the handler for command `0x1A` (`VA 0x8d88`):
1. Checks for a match: `memcmp(cmd + 2, FICR->DEVICEID[1], 4) == 0`.
2. Only on an exact match of all 4 bytes of the Chip ID is the function `0x78f4` called (setting the LED strobing period: in mode 1 — period of 533333 ticks = 30 Hz; in mode 2 — 640000 ticks = 25 Hz).
3. If the ID does not match or format is incorrect, the tracker does not turn on the light. Now, having the exact 32-bit Chip IDs, Frame can transmit the valid command `0x1A` autonomously.

## K06073
Source `D0150`, original line1, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Complete technical specification: Pico Motion Tracker (Swift 2.0) → Steam Frame / SteamVR

## K06074
Source `D0150`, original line3, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Document date:** 2026-10-05  
**Status:** Research completed, radio-level protocol and IMU telemetry decompiled, working Unity/PC bridge v1 implemented and tested.  
**Devices:** Pico 4 Enterprise (`A8110`, OS 5.13.7), Pico Motion Tracker Swift 2.0 (`TR0189`, HW `03`), Steam Frame dev kit.  
**Key repositories:**  
- Raw data and dumps: `H:\_other\AI\pico_tracker_research\`  
- Implemented bridge project: `K:\AI\pico\` (specification `AGENTS.md`, `pc/`, `unity/`, `pico-sdk/`)

## K06075
Source `D0150`, original line12, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 1. Overview of architecture and end-to-end data path

## K06076
Source `D0150`, original line14, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the Pico ecosystem, trackers **do not connect via standard Bluetooth/BLE**. They operate via a dedicated 2.4 GHz radio channel based on specialized Nordic Semiconductor peripherals.

## K06077
Source `D0150`, original line56, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2. Hardware layer of tracker and receiver

## K06078
Source `D0150`, original line58, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 2.1. Tracker (Swift 2.0)
- **MCU:** Nordic Semiconductor nRF52 series (Cortex-M4F, Keil RTX RTOS v5.7.0).
- **Sensors:**
  - 6-axis IMU **TDK InvenSense ICM-42688** (connected via SPI). In older versions, ICM-42686/BMI270 was encountered.
  - 3-axis magnetometer (calibration `mag cali %f %f %f %f`).
- **Power:** Charge controller **SGM41562**, battery voltage monitoring via SAADC, NTC thermistor.
- **Dock/mount interface:** I2C pad with detection of insertion into strap/base:
  `bracket_status: insert/pull`, `bracket_type: 3`. Supports passthrough data transfer (`externdev passthrough`).
- **LEDs:** Pulsed infrared (IR) LEDs around the body + one RGB status indicator.
- **Firmware:** Not encrypted (`Swift2.0_sv1.89_20241213_b189_app.bin`), but signed with RSA-2048 (public key is located in `/system/etc/trackerbin/public.pem`).

## K06079
Source `D0150`, original line69, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 2.2. Headset receiver node ("Station")
- **MCU:** Nordic Semiconductor **nRF52840** (firmware `PhoenixSta_sv2.96_PR_20250805_b296_app.bin`).
- **Communication bus with SoC:** SPI (`/dev/spidev0.0`, alias `/dev/stationdev`), clocked from Snapdragon XR2, interrupt line GPIO 84 (IRQ 84).
- **Device servicing:** The same nRF52840 simultaneously receives both Pico Neo 3 / Pico 4 controllers and up to 3 Swift trackers.
- **Camera hardware synchronization:** Station generates a hardware strobe `fsin` (Frame Sync In) for camera exposure and sends timing markers to trackers over radio: `set tracker %d sync light ok time %u fsin_count %d`.

## K06080
Source `D0150`, original line77, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. Reverse engineering the radio protocol (Nordic nRF52 NRF_RADIO)
