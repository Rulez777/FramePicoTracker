# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K03681
Source `D0496`, original line36, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

We use its existing onboard SWD connection. Our loader starts OpenOCD with initialization and polling configuration, without an erase/program/reset sequence. It binds its TCL interface to localhost; GDB and telnet ports are disabled. The Linux account needs access to the underlying debug resources; the loader is written for a privileged environment.

## K03682
Source `D0496`, original line38, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

## 2. Loading the module into RAM

## K03683
Source `D0496`, original line40, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

We identified manufacturing RTT buffers that we could temporarily reserve in this build, while retaining normal console channel 0.

## K03684
Source `D0496`, original line42, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

| Purpose | Address / limit on our firmware |
|---|---|
| RTT control block | `0x2001064C` |
| Module code | `0x2001151C`, limited to 2045 bytes |
| State, DMA and packet ring | `0x20010C18`, 2048 bytes |
| Stock MPSL context used for validation | `0x2001056C` |

## K03685
Source `D0496`, original line49, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Before loading, we save the original code/data buffers, RTT channel counts, and relevant MPU metadata to a Linux-side recovery checkpoint. We require the expected RTT signature/layout and an unused MPSL session. We then temporarily reduce the RTT channel counts from `3/3` to `1/1`, reserving the manufacturing channels but preserving the ordinary RTT shell.

## K03686
Source `D0496`, original line51, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

The module is a small freestanding Cortex-M4 Thumb ELF, linked at the code address above. We load its allocatable sections through OpenOCD memory writes and verify them by readback. Our loader rejects sections outside the reserved RAM range. Our current successful station-sequence image is **1952 bytes**, compiled with Clang `-Oz`.

## K03687
Source `D0496`, original line53, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

There are no NVMC writes, firmware flashing, or MCU reset in this sequence. Separately, we allow ordinary pairing-record writes made by the tracker's own firmware during normal pairing; that is not a Roy firmware modification.

## K03688
Source `D0496`, original line55, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

## 3. Entering through the stock shell, rather than moving the CPU PC

## K03689
Source `D0496`, original line57, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Early experiments used debugger-directed CPU invocation. The current path uses the existing Zephyr RTT shell's bypass callback instead, so the entry runs in the stock shell thread.

## K03690
Source `D0496`, original line59, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

For this image:

## K03691
Source `D0496`, original line67, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

We check that the bypass pointer is zero, temporarily write our module's Thumb entry pointer there, then write a small operation string to the normal RTT input ring. The stock shell invokes our callback with its usual `(shell, bytes, count)` arguments.

## K03692
Source `D0496`, original line69, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

The module immediately removes its bypass callback through the stock setter, executes the requested operation, and returns normally to the shell. Thus the bypass is a one-shot entry mechanism, not a replacement console. Operations are open/request, retry, and close. A retry reuses the same one-shot mechanism.

## K03693
Source `D0496`, original line71, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

This avoids deliberately halting the MCU or replacing its CPU register context for each operation. It does **not** mean SWD access or MPU changes have zero timing impact.

## K03694
Source `D0496`, original line73, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

## 4. The MPU / Zephyr context-switch issue

## K03695
Source `D0496`, original line75, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Copying code into SRAM is not enough: SRAM execution is restricted, and Zephyr's PendSV MPU setup can replace a manually installed region with its stack guard.

## K03696
Source `D0496`, original line77, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Our working reservation uses:

## K03697
Source `D0496`, original line84, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

On this exact build, the static count is initially 2. We temporarily reserve executable MPU region 2 for the 4 KiB block at `0x20011000` and change the count to 3. Zephyr then places its dynamic stack guard in region 3 instead of overwriting our executable region.

## K03698
Source `D0496`, original line86, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

The native entry performs this reservation atomically with interrupts briefly masked, reinstates the stock RAM policy, and re-enables the MPU. We temporarily disable the MPU to reach the first SRAM entry; it is not left disabled during the radio experiment. Retries and closing use the already reserved executable region.

## K03699
Source `D0496`, original line88, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

On cleanup we restore the original static count and let the stock context-switch path restore its usual guard layout, then verify restoration. This part is particularly firmware-specific; changing only an MPU selector through SWD was not a reliable solution.

## K03700
Source `D0496`, original line90, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

## 5. Requesting MPSL timeslots

## K03701
Source `D0496`, original line92, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

We call the existing firmware functions rather than replacing MPSL or interrupt vectors:

## K03702
Source `D0496`, original line94, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

| Function | Thumb function pointer on this image |
|---|---|
| Session open | `0x00010581` |
| Timeslot request | `0x00018949` |
| Session close | `0x000189BF` |

## K03703
Source `D0496`, original line100, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

We reconstructed the request and callback-result ABIs: a 16-byte request and an 8-byte result with a next-request pointer. Open/request/close are serialized using the same semaphore path as the stock radio-test shell, using a nonblocking acquisition.

## K03704
Source `D0496`, original line102, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

The initial request is an earliest request. In the module callback, START initializes our RADIO/TIMER work. Near the slot end we disable our radio activity and return either END (action 2) or REQUEST (action 3) with the next normal request. A normal chain lets the stock MPSL scheduler grant the next slot; it does not give us continuous radio ownership.

## K03705
Source `D0496`, original line104, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

For the successful direct connection we requested **2000 microsecond slots at 10000 microsecond intervals**, priority value 0 in this firmware's ABI. A 5000 microsecond interval did not reliably produce a chain in our tests. BLOCKED/CANCELLED/IDLE events are logged; the Linux host can retry an earliest request through the shell. We do not assume a rejected request executed radio code.

## K03706
Source `D0496`, original line106, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

The module only uses RADIO inside a granted slot. We stop it before returning the slot to MPSL and leave the stock BLE/controller stack installed. No flash patch or permanent RADIO interrupt-vector replacement is involved.

## K03707
Source `D0496`, original line108, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

## 6. Receiving packets and exporting diagnostics

## K03708
Source `D0496`, original line110, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

A practical detail: the RADIO END callback was not a reliable delivery path for our initial module. We therefore inspect END from our owned TIMER0 callback every 100 microseconds. Before the 2 ms slot expires, we stop the radio (our usual teardown deadline is 1800 microseconds).

## K03709
Source `D0496`, original line112, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

For TX-to-RX turnaround we use the nRF RADIO shortcuts:

## K03710
Source `D0496`, original line120, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

We wait until the peripheral has actually left TX state before removing the `DISABLED -> RXEN` shortcut. Removing it immediately on END could cancel the handover before DISABLED occurred.

## K03711
Source `D0496`, original line122, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Received DMA is copied into a four-record SRAM ring with CRC status, RSSI, channel, slot ordinal and a timer observation. Linux reads and drains that ring over SWD. The timer observation is a **callback observation**, not a hardware RF-edge timestamp. We truncate each payload using its actual length and never treat stale trailing DMA bytes as part of the packet.

## K03712
Source `D0496`, original line124, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Your protocol notes helped us move from passive reception to discovery, a targeted negotiation offer, command 9, settings 17/25/19, phase 10 and notification completion via command 4 / S1 ACK. The current experiment uses a fixed phase value; it is not yet camera-exposure synchronization. Its repeated seq-0 settings are an experimental retry sequence, not a finished per-command ACK engine.

## K03713
Source `D0496`, original line126, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

## 7. What we can and cannot claim about controller coexistence

## K03714
Source `D0496`, original line128, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

The intended coexistence mechanism is MPSL arbitration, not time-sharing by suspending the controller firmware. We do not stop the stock SPI-HID service, reflash Roy, or take RADIO ownership outside a granted slot.

## K03715
Source `D0496`, original line130, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

We have earlier passive-reception experiments with both Valve controllers tracked. In the current connection work, a separate 9-second scheduling test produced 919 granted slots while poses from the HMD and both controllers continued to arrive; it did **not** receive tracker IMU in that same run. Another saved-address test had 922 slots and both controllers' pose streams, also without tracker replies.

## K03716
Source `D0496`, original line132, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

The successful direct IMU capture had only HMD pose rows in its accompanying pose recording. Therefore we cannot yet claim that the new active station sequence, useful tracker data, and both tracked Valve controllers have all been verified together in one sustained run. MPSL arbitration is necessary, but sustained coexistence still needs latency/loss and controller tracking validation.

## K03717
Source `D0496`, original line134, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

## 8. Successful direct-radio result

## K03718
Source `D0496`, original line136, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

With the PICO headset off, after providing USB power and entering pairing on one tracker, we obtained:

## K03719
Source `D0496`, original line138, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

- 12 CRC-checked bootstrap packets establishing the actual responding hardware ID.
- A following correctly targeted session: 318 granted slots and 94 CRC-checked received packets.
- 90 replies on that tracker's individual working address; all 90 passed the protocol's additive checksum.
- 86 replies with real gyro/accelerometer samples: 43 with 18-byte sensor bodies and 43 with 24-byte combined bodies; two aux-only replies and two empty replies.

## K03720
Source `D0496`, original line143, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

We omit our device IDs, seed and raw captures here. This is a bounded direct IMU connection through onboard Roy, not yet continuous 6DoF. In particular, reconnecting after unloading the module remains unresolved in our follow-up probes.

## K03721
Source `D0496`, original line145, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

## 9. Cleanup, host failure and restarts

## K03722
Source `D0496`, original line147, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

For normal teardown:

## K03723
Source `D0496`, original line149, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

1. Set the module stop flag and let its current slot end.
2. Close the MPSL session through the native shell path.
3. Verify the stock session-status byte indicates closed and the shell no longer references our entry.
4. Restore the original MPU metadata and the saved RAM/RTT buffer contents.
5. Verify readback and end the OpenOCD process.

## K03724
Source `D0496`, original line155, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

If a session/callback is still active, the loader **retains the module and recovery checkpoint** instead of restoring code underneath a live callback. Killing a Linux loader alone is not a safe unload procedure; a supervisor/recovery path is still needed for production use.

## K03725
Source `D0496`, original line157, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Restart cases have different implications:

## K03726
Source `D0496`, original line159, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

| Event | Expected behavior / verification limit |
|---|---|
| Normal bounded capture ends | Explicit close and restore; tested by readback in our completed runs. Tracker keepalive stops, so a connection is not maintained afterward. |
| Roy MCU reset / headset power cycle | The runtime RAM patch must be treated as lost. Reload and renegotiate from stock state. We have not implemented automatic reinstallation. |
| SteamVR process restart alone | It does not inherently imply the separate MCU reset. Its driver may reinitialize/reset Roy or alter its schedule; we have not experimentally established the exact restart behavior. Do not reuse a stale session ID or assume callbacks survived. |
| SSH/network loss / Linux loader crash | The MCU may continue executing an installed callback. Preserve the checkpoint and inspect the live MCU/session before recovery. |

## K03727
Source `D0496`, original line166, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Before every new load we recheck the firmware anchors, RTT layout, MPU assumptions, and session state. We do not automatically overwrite an active or unknown session. A future always-on service should add bounded MCU-side expiry, recovery after Linux crashes, restart detection and a reliable reconnect/keepalive state machine.

## K03728
Source `D0496`, original line168, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

## 10. Code pointers

## K03729
Source `D0496`, original line170, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Our source files are currently in the working project:

## K03730
Source `D0496`, original line180, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

The experimental station module can reproduce the successful first-tracker build with `ROY_STATION_FIRST`, the native/MPU-reserve/poll/offer/handover build options, and the loader's explicit `--station-sequence --station-target first` flags. The integrated image is byte-identical to our successful 1952-byte module. Private tracker profiles currently need to be replaced with your own; these files are research code rather than an install-and-run package.

## K03731
Source `D0496`, original line182, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

The public project is https://github.com/Rulez777/Frame-Pico-Tracker. This latest local implementation is not automatically synchronized to the public snapshot, so the outline should not be read as a claim that the latest loader is already published there.

## K03732
Source `D0496`, original line184, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

If useful, we can compare firmware layouts and exchange a cleaned, version-gated loader next. Your packet-level documentation was particularly useful; our remaining work is sustained reconnect/keepalive, controller coexistence under active traffic, and exposure-aligned optical tracking.

## K03733
Source `D0497`, original line1, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

# Steam Frame onboard Roy as a PICO Swift station (technical notes)

## K03734
Source `D0497`, original line3, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

2026-10-07. Companion to `pico_tracker_packets_en.md`. It describes how PICO Motion Trackers (C2310, FW 189) are driven from the Steam Frame's own Roy radio MCU, with no dongle and no flash writes, while both Valve controllers keep tracking. Written as reference data for people and coding agents; numbers are given as measured.

## K03735
Source `D0497`, original line5, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

## How each fact is known

## K03736
Source `D0497`, original line7, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

| Label | Meaning |
|---|---|
| Hardware | Measured on a Steam Frame devkit with 3 C2310 trackers (FW 189) and 2 Valve controllers |
| Static | Read from the Roy firmware image or the tracker FW 189 image |
| Inferred | Reasoned, not tested |

## K03737
Source `D0497`, original line13, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

All Roy addresses below are valid for **one Roy build only** (the module refuses any other build after reading flash markers). On another build every firmware address must be found again.

## K03738
Source `D0497`, original line17, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

## 1. Roy hardware and access

## K03739
Source `D0497`, original line19, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

| Item | Value |
|---|---|
| MCU | nRF52833 (Cortex-M4F, 64 MHz, 128 KB SRAM at 0x20000000) |
| Stack | Zephyr + MPSL + SoftDevice Controller (BLE) |
| Debug access | SWD bit-banged over Frame SoC GPIO; stock config `/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg` |
| SWD speed | 5 to 8 ms per memory read round trip from Python through the OpenOCD TCL port under load. Batch reads into one TCL `list [read_memory ...] ...` command |
| CPU halt | Never needed. All reads and writes are done on the running core (AHB-AP) |
| Flash | Never written. No NVMC access in the module |

## K03740
Source `D0497`, original line28, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Rule: only one OpenOCD owner at a time; refuse to start if a listener already exists.

## K03741
Source `D0497`, original line32, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

## 2. Memory map used by the module (Static, Hardware)

## K03742
Source `D0497`, original line34, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

| Name | Address | Size | Use |
|---|---|---|---|
| CODE | 0x2001151C | 2045 B usable (module build is ~2016 B) | Module code, inside unused manufacturing RTT buffers |
| DATA | 0x20010C18 | 2048 B | Module state, ring, peers, mailboxes |
| RTT control block | 0x2001064C | | Must start with `SEGGER RTT`. Word at +16 holds up/down buffer counts, stock `3,3`; set to `1,1` while the module lives there so the manufacturing channels 1 and 2 are not used. Console buffer 0 stays |
| MPSL state | 0x2001056C | 48 B read | Checked to make sure no timeslot session is already open (MPSL allows one) |
| Shell bypass hook | 0x2000D8CC | 4 B | shell_rtt context 0x2000D894 + 0x38. Must be 0 at start. Writing an entry address here makes the stock RTT shell thread call it with the next received line |
| Zephyr static MPU count | byte 2 of word 0x20010A24 | 1 B | Expected 2 |

## K03743
Source `D0497`, original line43, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Everything in CODE and DATA (2044 + 2048 B), the RTT counts and the DWT/DEMCR words are saved to a recovery file **before** any write and restored after stop.

## K03744
Source `D0497`, original line45, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

### 2.1 Entering the module

## K03745
Source `D0497`, original line47, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

1. Write module to CODE, initial state to DATA.
2. Write `CODE entry | 1` (Thumb) to the shell bypass hook.
3. Send one character over RTT console: `'0'` open session + request first slot, `'1'` request slot, `'2'` close session, `'3'` no op.
4. The entry runs on the stock shell thread (privileged thread mode, no halt).

## K03746
Source `D0497`, original line52, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

### 2.2 MPU (Static)

## K03747
Source `D0497`, original line54, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Inside `entry`, with interrupts masked:

## K03748
Source `D0497`, original line65, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

### 2.3 Firmware functions called (Static, this build only)

## K03749
Source `D0497`, original line67, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

| Address (Thumb) | Use |
|---|---|
| 0x2EE2F | shell helper, called with `(shell, NULL)` to release the bypass |
| 0x28E2D | lock `(0,0)`, returns 0 on success |
| 0x28E3D | unlock |
| 0x10581 | `mpsl_timeslot_session_open(callback, &session_id)` |
| 0x18949 | `mpsl_timeslot_request(session_id, &request)` |
| 0x189BF | `mpsl_timeslot_session_close(session_id)` |

## K03750
Source `D0497`, original line76, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

On entry `'0'` the module also enables DWT CYCCNT (DEMCR bit 24, DWT_CTRL bit 0) and restores both words on `'2'`. DWT at 64 MHz is the time base for everything on the MCU side.

## K03751
Source `D0497`, original line80, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

## 3. DATA layout (ABI 4)

## K03752
Source `D0497`, original line82, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

### 3.1 State (offset 0, 1500 B)

## K03753
Source `D0497`, original line84, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

| Offset | Field |
|---|---|
| 0 | magic `0x314D5250` |
| 4 | stop flag |
| 12 | slots granted |
| 16 | packets received |
| 20 | dropped |
| 24 | blocked signals (MPSL 5..7) |
| 28 | phase (0 closed, 1 idle, 2 in slot, 3 blocked, 4 cancelled) |
| 32 | last API result |
| 36 / 40 | slot_us / interval_us |
| 44 | 10 radio profile words |
| 84 | session id (255 = none) |
| 88 | MPSL request struct (type, clock, priority, a, b) |
| 104 | MPSL return struct |
| 112 / 116 | ring write_count / read_count |
| 120 | 258 B radio DMA buffer |
| 380 | 10 records x 112 B |

## K03754
Source `D0497`, original line103, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Record (112 B): `number, crc|tag<<16, rssi|peer<<8, channel, length(u16), slot<<1(u16), bytes[80], chip_id, dwt_cycles, timer_us(u16), pad`. The host reads the whole ring in one 416 B SWD read when possible.

## K03755
Source `D0497`, original line105, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

### 3.2 Control words

## K03756
Source `D0497`, original line107, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

| Offset | Meaning |
|---|---|
| 1600 | `0x52455431` written when `entry` returns |
| 1604 | entry call counter |
| 1608 | slot budget; module stops when slots >= this. Host heartbeat extends it by 1000 slots |
| 1612 | reply timeout for a working peer, DWT cycles (192 000 000 = 3 s) |
| 1616 | MPSL priority (0 = HIGH) |
| 1620 | chip id of the peer in the current slot |
| 1644 | peer index in current slot |
| 1648 | mailbox generation used in current slot (ACK tag) |
| 1856 | 4 B station id (random per install) |
| 1876 | slot length for type-1 requests (us) |
| 1880 | one-shot signed distance correction for the next slot (us), written by the module itself |
| 1884 | `bit31 | RTC1 tick` target of the slot start on the BLE grid |
| 1888 | slot cycle length (default 7) |
| 1892 | RX end inside the slot, us from slot start (slot length minus 200 us margin) |

## K03757
Source `D0497`, original line124, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

### 3.3 Peers (offset 1504, 3 x 32 B)

## K03758
Source `D0497`, original line126, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

`id, base, prefix, stage, last_rx (DWT), search, reply_s1 | tag<<16, reserved` where `reserved bit0 = pause`.

## K03759
Source `D0497`, original line128, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

Stages: 0 searching, 2 offered (connect request sent), 3 working. Stage 3 is entered only on a valid reply received on channel 2 (working channel), never on a reply to the offer on a startup channel. A stage 2 peer that does not reply within 1 472 000 cycles (23 ms) returns to 0.

## K03760
Source `D0497`, original line130, SHA256 `722d28536824ee0e61735d8073d17f232b6fc98ee403ef229ff60b700f20e196`. Status: historical/unadjudicated narrative; English source prose.

### 3.4 Mailboxes (offset 1664, 3 x 64 B, seqlock)
