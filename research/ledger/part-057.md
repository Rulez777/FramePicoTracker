# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K04481
Source `D0765`, original line21, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

## What works

## K04482
Source `D0765`, original line23, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

- CRC-checked reception of real Swift packets through temporary SRAM code on the onboard Roy Nordic.
- IMU decoding and offline orientation, camera geometry and fusion tools.
- Native **FramePicoTracker** dashboard panel with diagnostic status, bounded camera/radio captures and sensor-vector visualization.
- Local diagnostic HTTP API for retrieving capture jobs and correlating radio, camera and headset traces.

## K04483
Source `D0765`, original line28, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

## Still being implemented

## K04484
Source `D0765`, original line30, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

Sustained radio cadence, independent station commands and acknowledgement, tracker IR synchronization, identification of the Motion Tracker optical constellation, and validated live SteamVR 6DoF output. A displayed acceleration vector is in sensor coordinates; it is not a world-space tracker pose.

## K04485
Source `D0765`, original line32, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

## Repository map

## K04486
Source `D0765`, original line34, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

| Folder | Contents |
|---|---|
| `pc/frame_app/` | Native C++ SteamVR dashboard application |
| `pc/radio/` | Diagnostic API, capture readers, protocol inspection and decoding |
| `pc/radio/ram/` | Firmware-specific temporary Roy SRAM loader and receiver |
| `pc/optical/` | Camera models, blob/PnP research, IMU orientation and fusion |
| `pc/driver/` | Experimental output adapters and native driver prototype |
| `pc/tests/` | Offline tests using synthetic fixtures |
| `scripts/` | Dependency preparation and native application build |

## K04487
Source `D0765`, original line44, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

## Python development

## K04488
Source `D0765`, original line46, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

Python 3.12 or newer:

## K04489
Source `D0765`, original line56, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

Tests do not require headset recordings or proprietary firmware. Some optional camera workflows also need OpenCV and ffmpeg installed locally.

## K04490
Source `D0765`, original line58, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

## Build the dashboard on Steam Frame

## K04491
Source `D0765`, original line60, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

Requires a C++17 compiler, pkg-config, Python, Git, Cairo, libcurl, Vulkan headers and installed SteamVR with its Linux ARM64 OpenVR runtime.

## K04492
Source `D0765`, original line68, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

Launch FramePicoTracker and open its panel in the SteamVR dashboard. The panel connects to `http://127.0.0.1:18741`. Its companion agent is `pc/radio/frame_debug_agent.py`; inspect `--help` for the local capture configuration. The existing development layout uses `/home/steamos/pico-ram-work`. The panel can build without the radio experiment; recording requires the matching local tools and configuration.

## K04493
Source `D0765`, original line70, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

The build fetches a pinned MIT-licensed overlay dependency into ignored local directories. No third-party source is committed here. See acknowledgements (original reference: `ACKNOWLEDGEMENTS.md`).

## K04494
Source `D0765`, original line72, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

## Hardware boundary

## K04495
Source `D0765`, original line74, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

The SRAM experiment is tied to a particular stock Roy build and verified RAM layout. It is not a general installer. Review loader guards and obtain your own matching stock reference locally before use. Preserve stock flash and existing pairing. Captures are bounded and restoration must be checked. Do not run unverified profiles on another firmware build.

## K04496
Source `D0765`, original line76, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

Device-specific addresses in protocol tests are examples, not universal pairing credentials. Reconstructed IDs are hypotheses until independently verified. The legacy PC/VMT bridge and external-radio template are research references and do not satisfy the standalone goal. The native driver prototype is not connected to a working pose pipeline.

## K04497
Source `D0765`, original line78, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

## Source-only publication

## K04498
Source `D0765`, original line80, SHA256 `4c81a362f43e6c8616d8f390c815a10e1bea50081844686bd7e4a129a494b975`. Status: historical/unadjudicated narrative; English source prose.

Firmware images, vendor SDKs/libraries, device calibration, camera recordings, runtime logs, binaries and local credentials are excluded. Serial-number fixtures are anonymized. Dependencies retain their own licenses; no project-wide license is selected in this snapshot.

## K04499
Source `D0005`, original line1, SHA256 `6bec1d6b17f6dceb13561ce31c08e2c91be7516a2427dfd47efa8cb48a20c96f`. Status: historical/unadjudicated narrative; English source prose.

# PICO / Steam Frame research library — 2026-10-08

## K04500
Source `D0005`, original line3, SHA256 `6bec1d6b17f6dceb13561ce31c08e2c91be7516a2427dfd47efa8cb48a20c96f`. Status: historical/unadjudicated narrative; English source prose.

Start with AI_READ_FIRST.md (original reference: `AI_READ_FIRST.md`). Use CURRENT.md (original reference: `CURRENT.md`) for present conclusions, RESEARCH_MAP.md (original reference: `RESEARCH_MAP.md`) to locate earlier work, and SUPERSEDED.md (original reference: `SUPERSEDED.md`) to avoid repeating disproved assumptions.

## K04501
Source `D0005`, original line5, SHA256 `6bec1d6b17f6dceb13561ce31c08e2c91be7516a2427dfd47efa8cb48a20c96f`. Status: historical/unadjudicated narrative; English source prose.

All 118 collected Markdown originals are preserved verbatim. Current summaries do not replace the research history. Source snapshots include working code, vendor binaries, radio captures, disassembly, camera recordings, calibration, native audits, firmware candidates and saved tests. Originals at their previous locations were not edited or removed.

## K04502
Source `D0005`, original line7, SHA256 `6bec1d6b17f6dceb13561ce31c08e2c91be7516a2427dfd47efa8cb48a20c96f`. Status: historical/unadjudicated narrative; English source prose.

The main file snapshot was collected on October 8, 16:51–16:53 Europe/Warsaw; two additional historical brain sessions were collected at 16:58. This is a sequential snapshot, not an atomic development checkout. Later work in the live project is outside this snapshot unless explicitly added.

## K04503
Source `D0005`, original line9, SHA256 `6bec1d6b17f6dceb13561ce31c08e2c91be7516a2427dfd47efa8cb48a20c96f`. Status: historical/unadjudicated narrative; English source prose.

| Resource | Purpose |
|---|---|
| Research map (original reference: `RESEARCH_MAP.md`) | Topic-specific entry points and historical evidence |
| Development history (original reference: `DEVELOPMENT_HISTORY.md`) | Complete Markdown inventory, including superseded reports |
| Current document selection (original reference: `current/INDEX.md`) | 17 principal original reports; read the supersession notes first |
| Key artifacts (original reference: `current/ARTIFACTS.md`) | Verified firmware, ELF, configuration and calibration references |
| Artifact catalog (original reference: `provenance/artifact-catalog.csv`) | Searchable original/copy paths, hashes and artifact classifications |
| Full manifest (original reference: `provenance/manifest.csv`) | Every collected file, source location, SHA256 and exact-content duplicates |
| Collection verification (original reference: `provenance/collection-summary.json`) | 13,469 files, 12,095,264,330 bytes, zero copy errors |
| sources/pico/ | Main private development snapshot, including its full docs/ history |
| sources/quick-bohr/ | Original extracted libraries, firmware and research tools |
| sources/brain-main/ | Main Antigravity research and protocol notes |
| sources/brain-fw189-review/ | Independent FW189 review and supporting material |
| sources/brain-validation-review/ | Separate October 7 grid3000 validation transcript |
| sources/brain-pico-root-history/ | Historical September 28 PICO root investigation |
| sources/Frame-Pico-Tracker/ | Separate code-publication snapshot; its status is older |
| sources/pico-wt-pc/ | Separate SDK-to-PC bridge architecture |
| reviews/ | Helper results, checked corrections and Gemini connection diagnostics |

## K04504
Source `D0005`, original line28, SHA256 `6bec1d6b17f6dceb13561ce31c08e2c91be7516a2427dfd47efa8cb48a20c96f`. Status: historical/unadjudicated narrative; English source prose.

Dependencies, caches, Git internals, linked directories and pico-sdk were excluded; see provenance/exclusions.json. Firmware, calibration, private IDs and recordings are retained locally. This library is private and must not be published wholesale.

## K04505
Source `D0005`, original line30, SHA256 `6bec1d6b17f6dceb13561ce31c08e2c91be7516a2427dfd47efa8cb48a20c96f`. Status: historical/unadjudicated narrative; English source prose.

The lead verified source-to-copy SHA256 for all collected files, the curated links, and ten key artifacts. This verifies preservation, not execution of historical tests or current device state. No device operation was performed for collection.

## K04506
Source `D0005`, original line32, SHA256 `6bec1d6b17f6dceb13561ce31c08e2c91be7516a2427dfd47efa8cb48a20c96f`. Status: historical/unadjudicated narrative; English source prose.

Grok 4.7 medium completed its audit through Cursor without fast. Gemini 3.8 Flash High initially timed out, then successfully answered a connection probe and read the collection metadata and current status. See Gemini connection report (original reference: `reviews/GEMINI-CONNECTION.md`).

## K04507
Source `D0005`, original line34, SHA256 `6bec1d6b17f6dceb13561ce31c08e2c91be7516a2427dfd47efa8cb48a20c96f`. Status: historical/unadjudicated narrative; English source prose.

All new maintained summaries and development entries use English. Historical source documents and raw model replies keep their original language to preserve evidence.

## K04508
Source `D0152`, original line2476, SHA256 `6723ba8fba11f8907a093cb49a32d21af2b162a2ffc86189e44614cfaf4eebbf`. Status: historical/unadjudicated narrative; English source prose.

## Root extraction and 32-bit Hardware IDs confirmed, 2026-10-06
Root was successfully obtained on the Pico 4 Enterprise headset via command injection
into pvrtracking service call 17 (context: u:r:pvrtrackingservice:s0).
The filesystem data and live station structures were extracted and verified:
- /mnt/vendor/pvr/tracker/tracker_info_data.json and /data/stationservice/calibration_data.json:
  Confirmed serials, bond status, and full factory IMU/optical calibration matrices for all 3 slots.
- Station Seed confirmed: [PRIVATE_LINK_VALUE] ([0x74, 0x73, 0x54, 0x15]), corresponding to Station MCU FICR DEVICEID[1].
- Deconstructed mystery '1233' string in tracker_test: in libstationopticsservice.so,
  TrackerManager::SetTrackerJsonInfo(w1, w2) converts two integer byte arguments to string
  (std::to_string(w1) + std::to_string(w2)), yielding '1233' as a formatting side-effect,
  which tracker_test rendered as 'addr: 31:32:33:33'.
- In Swift firmware (VA 0x7d18), NRF_FICR->DEVICEID[1] (0x10000064) is loaded into RAM 0x20000efe.
  The address generation formula addr[i] = (seed[i] + chip_id[i]) & 0xFF is inverted using known
  air addresses and Station Seed: chip_id[i] = (addr[i] - seed[i]) & 0xFF:
  * Slot 0 ([TRACKER_SERIAL]): Low ID = [0x3E, 0xED], Full 32-bit Chip ID = 0xEEACED3E, Air Addr = C0 00 06 4D
  * Slot 1 ([TRACKER_SERIAL]): Low ID = [0xC3, 0xBF], Full 32-bit Chip ID = 0xEEADDFC3, Air Addr = C0 80 4C EC
  * Slot 2 ([TRACKER_SERIAL]): Low ID = [0xAD, 0x1D], Full 32-bit Chip ID = 0xEEAE1DAD, Air Addr = C0 40 09 84
- Command 0x1A (VA 0x8d88): full 32-bit Chip ID verification requirement is confirmed.
  Frame can now generate valid 0x1A commands containing the exact 32-bit chip ID to trigger
  IR strobing autonomously without the Pico headset in the loop.

## K04509
Source `D0153`, original line18, SHA256 `0fc20893d589194e77688637a71f2174332a7ced2594a40a3fc07be292cb8a57`. Status: historical/unadjudicated narrative; English source prose.

***

## K04510
Source `D0008`, original line1, SHA256 `759e0b2411a1785f83412317df4b872bb736aba08d1e93fa14276d2ad61b5f3f`. Status: historical/unadjudicated narrative; English source prose.

# Key artifacts

## K04511
Source `D0008`, original line3, SHA256 `759e0b2411a1785f83412317df4b872bb736aba08d1e93fa14276d2ad61b5f3f`. Status: historical/unadjudicated narrative; English source prose.

This selection includes experimental builds and local private data. Read ../CURRENT.md and ../AI_READ_FIRST.md for scope and evidence limits.

## K04512
Source `D0008`, original line5, SHA256 `759e0b2411a1785f83412317df4b872bb736aba08d1e93fa14276d2ad61b5f3f`. Status: historical/unadjudicated narrative; English source prose.

- fw189-dual-mode.bin (original reference: `../sources/pico/outputs/fw189-dual-mode-iter05-final-20261007/fw189-dual-mode.bin`) — 97320 bytes, SHA256 `395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f`
- patch-manifest.json (original reference: `../sources/pico/outputs/fw189-dual-mode-iter05-final-20261007/patch-manifest.json`) — 6457 bytes, SHA256 `cbdb3a766de6eb3d5bf9abb8edd83349e1fe66fbd297a42e2bd82b394624751e`
- roy_station.elf (original reference: `../sources/pico/outputs/service-fast-poll24/build/roy_station.elf`) — 8004 bytes, SHA256 `ee5cc5d3e19c35cd80d00474e6f9b03d11530553442cff771dfdb1e60abd4084`
- analysis.json (original reference: `../sources/pico/outputs/service-fast-poll24/service-measure24/analysis.json`) — 895 bytes, SHA256 `7ff29869bb33b4bf8d6ccd243f389d5e9630a49f812895ab1be1d8a6a1359ab0`
- host-restoration.json (original reference: `../sources/pico/outputs/service-fast-poll24/service-measure24/host-restoration.json`) — 117 bytes, SHA256 `1be354c1929359de034bf58620bdef81915cb6aaa8aaa4c1ee454d0e7e66d6e7`
- roy_station.elf (original reference: `../sources/pico/outputs/service-fast-poll25/build/roy_station.elf`) — 8044 bytes, SHA256 `a7de084a9e9697e1702b7829d89cbe67577945f14e7490ff0ec376cd9fec8305`
- deployment.json (original reference: `../sources/pico/outputs/service-fast-poll25/build/deployment.json`) — 1663 bytes, SHA256 `193b34bd076f16287601530b06b5b004aabd63648f8269d2ad8dada91386aa2e`
- roy_ram_station_v4_grid.c (original reference: `../sources/pico/outputs/service-fast-poll24/roy_ram_station_v4_grid.c`) — 12384 bytes, SHA256 `e508c9016297fd02c12677c5a6d3837237d31656232f70b3df60156a26b8a1f3`
- pico-calibration-root.json (original reference: `../sources/pico/outputs/pico-calibration-root.json`) — 4996 bytes, SHA256 `13aa8a713128c7cfbf72df3c90a10400753c3d40061adb360730c224e17f6589`
- frame-factory-20261006.json (original reference: `../sources/pico/outputs/frame-factory-20261006.json`) — 16513 bytes, SHA256 `ca0e6db0dda224e7e0383602a5ccfa14511e5af44f027abfff4c0c75a207bb16`

## K04513
Source `D0009`, original line1, SHA256 `c946f86d12b3afe39aa2b3846481bee67b79b4bbabed9e8d0327f62d24fd8e66`. Status: historical/unadjudicated narrative; English source prose.

# Principal original documents

## K04514
Source `D0009`, original line3, SHA256 `c946f86d12b3afe39aa2b3846481bee67b79b4bbabed9e8d0327f62d24fd8e66`. Status: historical/unadjudicated narrative; English source prose.

Read ../AI_READ_FIRST.md, ../CURRENT.md and ../SUPERSEDED.md first. These selected originals are preserved verbatim, including historical paragraphs. The complete history is in ../DEVELOPMENT_HISTORY.md; topic lookup is in ../RESEARCH_MAP.md.

## K04515
Source `D0009`, original line7, SHA256 `c946f86d12b3afe39aa2b3846481bee67b79b4bbabed9e8d0327f62d24fd8e66`. Status: historical/unadjudicated narrative; English source prose.

- SERVICE_FAST_POLL_ITERATION23_2026-10-08.md (original reference: `radio/SERVICE_FAST_POLL_ITERATION23_2026-10-08.md`)
- SERVICE_FAST_POLL_ITERATION22_2026-10-08.md (original reference: `radio/SERVICE_FAST_POLL_ITERATION22_2026-10-08.md`)
- SERVICE_FAST_POLL_ITERATION21_2026-10-08.md (original reference: `radio/SERVICE_FAST_POLL_ITERATION21_2026-10-08.md`)
- SERVICE_FAST_POLL_ITERATION20_2026-10-08.md (original reference: `radio/SERVICE_FAST_POLL_ITERATION20_2026-10-08.md`)

## K04516
Source `D0009`, original line12, SHA256 `c946f86d12b3afe39aa2b3846481bee67b79b4bbabed9e8d0327f62d24fd8e66`. Status: historical/unadjudicated narrative; English source prose.

## firmware

## K04517
Source `D0009`, original line14, SHA256 `c946f86d12b3afe39aa2b3846481bee67b79b4bbabed9e8d0327f62d24fd8e66`. Status: historical/unadjudicated narrative; English source prose.

- FW189_ITERATION_05_MODE_HANDSHAKE_2026-10-07.md (original reference: `firmware/FW189_ITERATION_05_MODE_HANDSHAKE_2026-10-07.md`)
- FW189_ITERATION_06_BOOT_RECOVERY_2026-10-08.md (original reference: `firmware/FW189_ITERATION_06_BOOT_RECOVERY_2026-10-08.md`)
- FW189_ITERATION_08_REFRESH_RETRY_2026-10-08.md (original reference: `firmware/FW189_ITERATION_08_REFRESH_RETRY_2026-10-08.md`)
- FW189_ITERATION_19_JNI_DEX_AUDIT_2026-10-08.md (original reference: `firmware/FW189_ITERATION_19_JNI_DEX_AUDIT_2026-10-08.md`)
- FW189_ITERATION_18_OFFLINE_VENDOR_APK_AUDIT_2026-10-08.md (original reference: `firmware/FW189_ITERATION_18_OFFLINE_VENDOR_APK_AUDIT_2026-10-08.md`)
- FW189_ITERATION_17_FULL_OS_PACKAGE_SEARCH_2026-10-08.md (original reference: `firmware/FW189_ITERATION_17_FULL_OS_PACKAGE_SEARCH_2026-10-08.md`)
- FW189_ITERATION_16_SERVER_BOOT_SEARCH_2026-10-08.md (original reference: `firmware/FW189_ITERATION_16_SERVER_BOOT_SEARCH_2026-10-08.md`)
- FW189_ITERATION_15_COMMAND_READBACK_AUDIT_2026-10-08.md (original reference: `firmware/FW189_ITERATION_15_COMMAND_READBACK_AUDIT_2026-10-08.md`)
- FW189_SWD_READONLY_PLAN_2026-10-08.md (original reference: `firmware/FW189_SWD_READONLY_PLAN_2026-10-08.md`)
- FW189_GRID3000_FINAL_VERDICT_2026-10-07.md (original reference: `firmware/FW189_GRID3000_FINAL_VERDICT_2026-10-07.md`)

## K04518
Source `D0009`, original line25, SHA256 `c946f86d12b3afe39aa2b3846481bee67b79b4bbabed9e8d0327f62d24fd8e66`. Status: historical/unadjudicated narrative; English source prose.

## vision

## K04519
Source `D0009`, original line27, SHA256 `c946f86d12b3afe39aa2b3846481bee67b79b4bbabed9e8d0327f62d24fd8e66`. Status: historical/unadjudicated narrative; English source prose.

- HANDOFF_VISION_2026-10-07.md (original reference: `vision/HANDOFF_VISION_2026-10-07.md`)

## K04520
Source `D0009`, original line29, SHA256 `c946f86d12b3afe39aa2b3846481bee67b79b4bbabed9e8d0327f62d24fd8e66`. Status: historical/unadjudicated narrative; English source prose.

## identity-protocol

## K04521
Source `D0009`, original line31, SHA256 `c946f86d12b3afe39aa2b3846481bee67b79b4bbabed9e8d0327f62d24fd8e66`. Status: historical/unadjudicated narrative; English source prose.

- full-id-acquisition.md (original reference: `identity-protocol/full-id-acquisition.md`)
- pico_tracker_packets_en.md (original reference: `identity-protocol/pico_tracker_packets_en.md`)

## K04522
Source `D0009`, original line34, SHA256 `c946f86d12b3afe39aa2b3846481bee67b79b4bbabed9e8d0327f62d24fd8e66`. Status: historical/unadjudicated narrative; English source prose.

## Latest primary evidence

## K04523
Source `D0009`, original line36, SHA256 `c946f86d12b3afe39aa2b3846481bee67b79b4bbabed9e8d0327f62d24fd8e66`. Status: historical/unadjudicated narrative; English source prose.

- Iteration24 analysis (original reference: `../sources/pico/outputs/service-fast-poll24/service-measure24/analysis.json`)
- Iteration24 run and cleanup (original reference: `../sources/pico/outputs/service-fast-poll24/service-measure24/run-summary.json`)
- Iteration24 restoration (original reference: `../sources/pico/outputs/service-fast-poll24/service-measure24/host-restoration.json`)
- Build25: hardware_validated=false (original reference: `../sources/pico/outputs/service-fast-poll25/build/deployment.json`)
- Experimental dual-mode final (original reference: `../sources/pico/outputs/fw189-dual-mode-iter05-final-20261007/`)
- Stock extraction and libraries (original reference: `../sources/pico/outputs/pico4e/`)
- quick-bohr tools (original reference: `../sources/quick-bohr/`)
- Antigravity protocol notes (original reference: `../sources/brain-main/pico_swift_protocol_and_pairing_spec.md`)
- Main project source and tests (original reference: `../sources/pico/pc/`)
- Every source and SHA256 (original reference: `../provenance/manifest.csv`)

## K04524
Source `D0766`, original line1, SHA256 `eac30c934a4180adabaeafd17aa320bc639c0af0f577e2fe23b220cae864ae5c`. Status: historical/unadjudicated narrative; English source prose.

# Implementation status — 2026-10-06

## K04525
Source `D0766`, original line3, SHA256 `eac30c934a4180adabaeafd17aa320bc639c0af0f577e2fe23b220cae864ae5c`. Status: historical/unadjudicated narrative; English source prose.

Confirmed: real CRC-checked radio packets on the onboard Roy; temporary SRAM execution and cleanup; camera capture through the headset recording service; native dashboard diagnostics; offline parsers and camera/IMU tools.

## K04526
Source `D0766`, original line5, SHA256 `eac30c934a4180adabaeafd17aa320bc639c0af0f577e2fe23b220cae864ae5c`. Status: historical/unadjudicated narrative; English source prose.

Not confirmed: continuous reception at the required rate, a fully independent station, authenticated full IDs for every tracker, synchronized IR on Frame, identified Motion Tracker markers in camera frames, or live autonomous 6DoF output.

## K04527
Source `D0766`, original line7, SHA256 `eac30c934a4180adabaeafd17aa320bc639c0af0f577e2fe23b220cae864ae5c`. Status: historical/unadjudicated narrative; English source prose.

Recent investigation established that PICO sleep gates some station commands before SPI, temporary fast-mode readback can be held and restored, and a dual-address passive receiver has been built. The dual receiver's hardware test remains pending. Low reception cadence remains unresolved.

## K04528
Source `D0766`, original line9, SHA256 `eac30c934a4180adabaeafd17aa320bc639c0af0f577e2fe23b220cae864ae5c`. Status: historical/unadjudicated narrative; English source prose.

Next milestones: capture reconnect traffic, establish station behavior without PICO, validate IR identification, then integrate and measure live poses while retaining both stock controllers.

## K04529
Source `D0766`, original line11, SHA256 `eac30c934a4180adabaeafd17aa320bc639c0af0f577e2fe23b220cae864ae5c`. Status: historical/unadjudicated narrative; English source prose.

All hardware claims require captured evidence and restoration checks. Unit-test success alone is not hardware tracking proof.

## K04530
Source `D0767`, original line1, SHA256 `4616f6e0e47d1f1044396f19f8f2693bc012500b55289d9625ce6b035f65d49f`. Status: historical/unadjudicated narrative; English source prose.

# Native FramePicoTracker dashboard

## K04531
Source `D0767`, original line3, SHA256 `4616f6e0e47d1f1044396f19f8f2693bc012500b55289d9625ce6b035f65d49f`. Status: historical/unadjudicated narrative; English source prose.

See the root README for dependency preparation, building and installation. The panel communicates with the local diagnostic agent on port 18741. It can request a bounded ten-second combined camera/radio capture when the configured hardware harness is available. Preview imagery comes from completed captures; it is not a live camera stream. The sensor vector is normalized acceleration in sensor axes, not a 6DoF pose.

## K04532
Source `D0027`, original line1, SHA256 `0b7fbbf29a37ec7dbd8ecf30d0ef828d5666735c37ff44965ed6a063b544cad8`. Status: historical/unadjudicated narrative; English source prose.

# Historical Gemini timeout record — superseded connection status

## K04533
Source `D0027`, original line3, SHA256 `0b7fbbf29a37ec7dbd8ecf30d0ef828d5666735c37ff44965ed6a063b544cad8`. Status: historical/unadjudicated narrative; English source prose.

The two original read-only/PLAN attempts timed out at the MCP client after300seconds. They did not provide a completed answer to the original broad audit at that time. This failure record is preserved for chronology.

## K04534
Source `D0027`, original line5, SHA256 `0b7fbbf29a37ec7dbd8ecf30d0ef828d5666735c37ff44965ed6a063b544cad8`. Status: historical/unadjudicated narrative; English source prose.

Current status: subsequent connection and file-read probes succeeded. See GEMINI-CONNECTION.md. The historical timeout must not be interpreted as current authentication failure or completed comprehensive Gemini review.

## K04535
Source `D0028`, original line1, SHA256 `1d2b22df6a0fbb08706ddf859c9a275f182a87504f6727c64e91005447ccfbc5`. Status: historical/unadjudicated narrative; English source prose.

# Gemini connection diagnosis and verification — 2026-10-08

## K04536
Source `D0028`, original line3, SHA256 `1d2b22df6a0fbb08706ddf859c9a275f182a87504f6727c64e91005447ccfbc5`. Status: historical/unadjudicated narrative; English source prose.

Model: gemini-3.8-flash-high, through agent-intern/Antigravity. bridge0.32.1; agy1.3.1.

## K04537
Source `D0028`, original line5, SHA256 `1d2b22df6a0fbb08706ddf859c9a275f182a87504f6727c64e91005447ccfbc5`. Status: historical/unadjudicated narrative; English source prose.

The earlier broad call requested600seconds. Codex's MCP transport stopped waiting after300seconds, but the CLI continued for10minutes and held the bridge's global _AGY_LOCK. A second Gemini request queued behind it; timeout_s applies to CLI execution and does not bound lock wait. Old logs show silent authentication succeeded and actual model requests were made. These were execution/queue/transport timeouts, not demonstrated credential failures.

## K04538
Source `D0028`, original line7, SHA256 `1d2b22df6a0fbb08706ddf859c9a275f182a87504f6727c64e91005447ccfbc5`. Status: historical/unadjudicated narrative; English source prose.

Verified live:
- antigravity_status: OverallOK, quota available, readable completed transcript.
- MCP no-tool request returned exactly GEMINI_CONNECTION_OK.
- MCP bounded file-read request read collection-summary.json and CURRENT.md, returning the exact13469files and12095264330bytes and the correct distinction between a bounded iteration24 result and an unvalidated iteration25 build.

## K04539
Source `D0028`, original line12, SHA256 `1d2b22df6a0fbb08706ddf859c9a275f182a87504f6727c64e91005447ccfbc5`. Status: historical/unadjudicated narrative; English source prose.

Persistent fix: added tool_timeout_sec=1200 only to [mcp_servers.agent-intern] in [USER_HOME]\.codex\config.toml. Backup: config.toml.before-gemini-timeout-20261008. Parsed TOML validation confirms all other settings unchanged; credentials/accounts were not changed. The new transport limit takes effect on MCP reload; the live probes above succeeded on the already running connection.

## K04540
Source `D0028`, original line14, SHA256 `1d2b22df6a0fbb08706ddf859c9a275f182a87504f6727c64e91005447ccfbc5`. Status: historical/unadjudicated narrative; English source prose.

Operating rule: serialize Gemini requests, prefer bounded90–180second tasks and explicit file lists, wait for termination before retrying, and use status/logs to distinguish queueing, execution timeout and authentication problems. A ping alone is insufficient; the file-read probe also passed. No CLI or package downgrade was needed.

## K04541
Source `D0029`, original line1, SHA256 `d500eeeefae5d70b8c165aa30c57dc4f88d618be3043d8734cb8ce6eeb8c5516`. Status: historical/unadjudicated narrative; English source prose.

# Grok 4.7 medium audit

## K04542
Source `D0029`, original line3, SHA256 `d500eeeefae5d70b8c165aa30c57dc4f88d618be3043d8734cb8ce6eeb8c5516`. Status: historical/unadjudicated narrative; English source prose.

The original Russian response is preserved verbatim at raw/GROK-4.7-medium-original.txt (original reference: `raw/GROK-4.7-medium-original.txt`).

## K04543
Source `D0029`, original line5, SHA256 `d500eeeefae5d70b8c165aa30c57dc4f88d618be3043d8734cb8ce6eeb8c5516`. Status: historical/unadjudicated narrative; English source prose.

Checked findings: the journal ends at23, while saved primary evidence includes a bounded fault-free run24 and an unvalidated build25. Radio reply ratios remain about10–12% at requestedN0. Application acceptance of command10, actual appliedN, controller coexistence, continuous reliability and6DoF remain unverified. Native command16 exists; TIMER0 is1us; the OTA gate remains closed.

## K04544
Source `D0029`, original line7, SHA256 `d500eeeefae5d70b8c165aa30c57dc4f88d618be3043d8734cb8ce6eeb8c5516`. Status: historical/unadjudicated narrative; English source prose.

The audit identified conflicting ELF deployment metadata for24 and gaps in restoration hashes. The lead checked and corrected the exact binary-versus-manifest relationship; see LEAD-REVIEW.md and provenance/known-conflicts.json.

## K04545
Source `D0029`, original line9, SHA256 `d500eeeefae5d70b8c165aa30c57dc4f88d618be3043d8734cb8ce6eeb8c5516`. Status: historical/unadjudicated narrative; English source prose.

Older optical handoffs and code-publication status are historical snapshots. Review predictions of large RX improvement or complete assert elimination are not measured outcomes.

## K04546
Source `D0030`, original line1, SHA256 `437579c1c8e55d3815aae8c9a67b1f41a8ba16ae2c23197fd9448dbdd1cd5640`. Status: historical/unadjudicated narrative; English source prose.

# Lead verification of helper findings

## K04547
Source `D0030`, original line3, SHA256 `437579c1c8e55d3815aae8c9a67b1f41a8ba16ae2c23197fd9448dbdd1cd5640`. Status: historical/unadjudicated narrative; English source prose.

Grok4.7medium ran through Cursor in read-only mode, without fast. Its raw answer is retained at raw/GROK-4.7-medium-original.txt. Relevant findings were checked against primary saved files before integration.

## K04548
Source `D0030`, original line5, SHA256 `437579c1c8e55d3815aae8c9a67b1f41a8ba16ae2c23197fd9448dbdd1cd5640`. Status: historical/unadjudicated narrative; English source prose.

Correction to Grok's two-ELF wording: both saved iteration24 binaries have SHAee5cc5d3…/capture code1936B, whereas adjacent deployment manifests describe SHA7572ee3e…/code1944B. The snapshot preserves a manifest-to-binary mismatch, not two verified binary variants. See provenance/known-conflicts.json.

## K04549
Source `D0030`, original line7, SHA256 `437579c1c8e55d3815aae8c9a67b1f41a8ba16ae2c23197fd9448dbdd1cd5640`. Status: historical/unadjudicated narrative; English source prose.

Build25's source was found at service-fast-poll24/roy_ram_station_v4_grid.c and matches the deployment source hash. The main source snapshot is different. Matching hashes establish provenance, not a rerun or hardware validation.

## K04550
Source `D0030`, original line9, SHA256 `437579c1c8e55d3815aae8c9a67b1f41a8ba16ae2c23197fd9448dbdd1cd5640`. Status: historical/unadjudicated narrative; English source prose.

Separate brain transcripts for the October7 grid3000 check and September28 PICO root request were retained. Unrelated chipset, game-install and model-settings conversations were excluded by their original subject.

## K04551
Source `D0030`, original line11, SHA256 `437579c1c8e55d3815aae8c9a67b1f41a8ba16ae2c23197fd9448dbdd1cd5640`. Status: historical/unadjudicated narrative; English source prose.

Gemini3.8FlashHigh subsequently passed an MCP connection probe and a bounded file-read request. Its extracted file count and bytes match the manifest. Its statement that the ELF is1976bytes was corrected:1976 is code_bytes; the ELF is8044bytes. This was a limited extraction check, not a comprehensive independent audit of every historical source.

## K04552
Source `D0164`, original line1, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# External breakdown of the link status report (Claude Opus 5.5)

## K04553
Source `D0164`, original line3, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-09. Read-only and offline analysis of saved artifacts: no RF, no
module loading, no OTA. Breakdown subject:
CONNECTION_AND_RESEARCH_STATE_FOR_EXTERNAL_REVIEW_2026-10-09.md (original reference: `CONNECTION_AND_RESEARCH_STATE_FOR_EXTERNAL_REVIEW_2026-10-09.md`).

## K04554
Source `D0164`, original line7, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Report verification

## K04555
Source `D0164`, original line9, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- All 60 files of `evidence-manifest.json` recomputed: SHA256 match. Archive passes `testzip`.
- Figures coexist56/64, phase67, and phase76 match `state.json` and raw records.
- Formulations are careful: denominators are separated, hypotheses are separated from observations.

## K04556
Source `D0164`, original line13, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Main finding: END time depends on reply length

## K04557
Source `D0164`, original line15, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Data taken from `radio-live-records.jsonl`, only records with valid CRC were counted.
`callback_observation_us` is the first 100-µs poll of TIMER0 after END, not the packet
arrival time.

## K04558
Source `D0164`, original line19, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Run | DMA length | n | Observation, µs (median) |
|---|---|---|---|
| service52 | 3 (empty `010300`) | 68 | 226 |
| service52 | 24 / 30 (IMU) | 1006 / 1008 | 335 / 336 |
| service52 | ≥35 (product info, long records) | 13 | 441–446 |
| mode53 | 24–56 | 220 | 418–447, sporadic 531/588 |
| coexist56/64, phase67 | only 3 | 5 | 265–267 |

## K04559
Source `D0164`, original line27, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the coexistence profile, reception ends at 400 µs (slot 600 − margin 200).
This means that every reply observed at ≥441 µs in service52 physically cannot
be captured. Among them:

## K04560
Source `D0164`, original line31, SHA256 `5b8b21f4da12df2265dcd5ae6037af5de25804b937b1d3c9a721ec7c0217c172`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- **product info** (command 1, payload 35 bytes, `0x82`). Without it, `PeerEngine` does
  not exit CONFIRMING: after 3 s it initiates disconnect and begins reconnecting.
- **schema2 mode reply** (`fe02`): this cannot be caught either, so the mode cannot be proven.
