# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05121
Source `D0177`, original line33, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- resetreason=0,count<150 → standard delay/retry0xc48a;
- resetreason=0,count>=150 → movs r0,#1, native event tail0xc498;
- resetreason!=0 → cmp r2,#3 and return0xc448: original bls/cbnz and check600 remain operational.

## K05122
Source `D0177`, original line37, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

0xc4aa allows Frame only when r4/resetreason=0, count100..149. Runtime0xc28c and counter0x20001ff4 were not changed. Mode RAM0x20005150 remains8bytes of expanded BSS, heap starts0x20005158/end0x20005350. Full startup and analyzed full-off reset mode; pseudo-power-off may preserve it. Heap worst case and the physical duration of the gesture are unproven.

## K05123
Source `D0177`, original line39, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## New wire protocol

## K05124
Source `D0177`, original line41, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In FW189, command1 dispatcher0x8c10 scans exactly13native entries0x16528..0x16588; subcommands02/12/29/0c/13/14/25/30/31/2f/50/51/52. **0x7e is absent from this table.** The table and all its native handlers are not replaced; the read-only extension intercepts native command1 wrapper0x8d16. Other firmware versions have not been investigated; the protocol is intended for this exact FW189 fork.

## K05125
Source `D0177`, original line43, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Request — command1, body exactly18bytes:

## K05126
Source `D0177`, original line49, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Response — native reliable command1 queue0x9368, body exactly24bytes:

## K05127
Source `D0177`, original line55, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

mode=0 — stock constants, mode=1 — Frame; revision=1. Identity is read from native RAM0x20000efe. Native0x7d18 populates it from FICR0x10000064 during radio init. The test sets synthetic chip ID directly in RAM; real initialization of this field on the device was not verified in this iteration.

## K05128
Source `D0177`, original line57, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

An unknown mode-flag returns0, just like existing selectors. The query checks full length, subcommand, and schema. There is no flash write, mode change, or OTA action. The response is constructed on a 40-byte temporary stack and copied by the native queue; return restores callee-saved registers. Malformed/other commands fall through to the original wrapper: push{r4,r5,lr},sub sp,#0x2c,PC0x8d1a. The fallback comparison verifies GPR/APSR/SP/LR and the native saved-register stack. Additional temporary writes in the uninitialized part of the future stack-frame are not guaranteed to be byte-for-byte identical; the stock wrapper then performs its standard zeroing/copying.

## K05129
Source `D0177`, original line59, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Host station

## K05130
Source `D0177`, original line61, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Modified pc/radio/ram/peer_engine.py, station_v4_runtime.py, roy_ram_loader.py, tracker_ota_runtime.py, and pc/optical/station_optics.py.

## K05131
Source `D0177`, original line63, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Enabling is **explicit**, via a separate configuration for real chip IDs:

## K05132
Source `D0177`, original line65, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- tracker_dual_mode_ids — a list of non-empty LE32 IDs in lowercase hex; zero ID is forbidden. By default the list is empty: existing stock/fixed-grid peers operate via the old path.
- tracker_mode_decimation — mandatory map of strictly stock/frame with explicit integer N0..40. One cannot substitute a single common N or interpret version189 as mode. Tests use stock9/frame13; these N values are not declared measuredly compatible over the air.

## K05133
Source `D0177`, original line68, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

These settings are not written to the active config and are not deployed to Frame. For the dual-mode engine, manual tracker_poll_decimation_by_id does not override N selected by the confirmed mode. For other peers, old behavior is preserved.

## K05134
Source `D0177`, original line70, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

After native product-info connection confirmation, the engine transitions to verifying_mode, generates secrets.token_bytes(16), and sends request. Reception passes existing hardware CRC check, working checksum, and source_chip_id/slot in StationV4Runtime.record. Then reply subtype/schema/length, nonce, chip ID, mode, and revision are verified. An ACK in receive or an MCU ack_tag without a body does not advance query to command16.

## K05135
Source `D0177`, original line72, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Disconnect/start clear mode/nonce. A previous connection query with a different nonce, a different chip ID, unknown mode/revision, a CRC-invalid packet, and an empty transport response do not authorize configuration. A random nonce correlates response with request; this is not cryptographic radio authentication.

## K05136
Source `D0177`, original line74, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

On an idle command pipeline, 2 seconds after the last query, background refresh begins. If the previous mode is still valid, state/phase are not reset solely for the query; optics skips sending new commands while query is busy. If the mode is the same, phase is preserved. If it changed, phase_verified is reset and configuration begins with the corresponding command16. Unsent obsolete exposure commands are removed.

## K05137
Source `D0177`, original line76, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Lease is **3 seconds from query creation time (Pending.first_sent, prior to SWD publication)**, rather than from delayed response receipt. A response to a query of age >=3 seconds is rejected. A busy pipeline does not extend the lease: upon expiration, the engine disconnects, mode becomes unknown. An unconfirmed peer is excluded from optical active/connections; live top-level and nested optical pose_valid=false.

## K05138
Source `D0177`, original line78, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is not an instant reboot detector. If the tracker reboots unnoticed by the MCU between checks, the previous profile may be used until the expiration of the current 3-second lease or until a new mode is received. RF bootstrap/phase, packet loss, and real delays can interfere with the request itself. In this case, the mode is not guessed, and the connection does not advance. The Availability/latency/overhead of this protocol on the device have not yet been measured.

## K05139
Source `D0177`, original line80, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Exclusive OTA adapter explicitly rejects the configuration with tracker_dual_mode_ids: background mode query must not be mixed with the previous OTA pipeline. Whitelist expanded-image is **not allowed**.

## K05140
Source `D0177`, original line82, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Checks

## K05141
Source `D0177`, original line84, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Final JUnit: outputs/fw189-dual-mode-iter05-tests.xml. Additional static/repro audit: outputs/fw189-dual-mode-iter05-audit.json. **161 passed**, 10.50s.

## K05142
Source `D0177`, original line86, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The new test group executes:

## K05143
Source `D0177`, original line88, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- real start button with a GPIO model for boundaries 4/99/100/120/149/150/599/600/800; resetreason 0 does not set startup flag20001ff0 and emits event 1, counter is limited to 150;
- comparison of resetreason 4 with stock at seven boundaries, including standard event 3;
- native record dispatcher, wrapper, and reliable reply queue: query, retry/dedup, ACK retirement, malformed lengths/schema, modes 0/1/2/255;
- end-to-end native reply → PeerEngine → N9/N13; invalid nonce/identity/revision/mode/CRC/ACK are rejected;
- Runtime source identity guard, ignoring stale manualN for dual-mode, live unknown/pose gating, reconnect and background refresh, profile change and expiry/delayed reply;
- GPR/APSR/SP/LR after 11 selectors for 4 mode flags and fallback wrapper;
- TX/RX/TIMER2/RTC real functions for all N0..255 in both profiles;
- full-size 97320 native OTA/SPI reconstruction and sentinel 4056; reverse and startup heap/full-off.

## K05144
Source `D0177`, original line97, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

SPI, GPIO, delays/peripheral setup, queue allocation, and IRQ helpers are modeled. In the new fixture, the fake allocator excludes the reserved mode interval. This is not proof of the sufficiency of 504 bytes of native heap. IRQ races, physical radio, bootloader, and recovery are not covered.

## K05145
Source `D0177`, original line99, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The final regression run also includes the previous peer-engine, ABI 4/optics integration, OTA sender/runtime, and Roy loader tests. The previous stage before the background-refresh refinement separately passed 228 tests, including the frozen old fork; the final JUnit corresponds to the current source code and the specified suite.

## K05146
Source `D0177`, original line101, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What remains until release

## K05147
Source `D0177`, original line103, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Bootloader acceptance of 97320/activation/recovery, heap worst case, RTC quantization, RX phase/window 400 µs, controller interoperability, and physical UX remain open. The previous OTA race of cached/pending of the old worker is not fixed by this query. Deep effects of native pairing for other reset reasons have not been rewritten and have not been fully audited.

## K05148
Source `D0177`, original line105, SHA256 `3ce568a2285c643f8227b631c0b1a2dc0dcf0136ff6fdcca5639c7cbfbf38f15`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The stated objective of this iteration—to remove the pairing branch added to normal startup and implement mode confirmation by the station—has been completed in offline scope. Hardware verification and flashing require a separate user command; they were not started here.

## K05149
Source `D0178`, original line1, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 06 — bootloader/recovery and clean start of OTA

## K05150
Source `D0178`, original line3, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Report date: 2026-10-08; work started on October 07. No SSH/SWD/RF/OTA/service restarts or device actions. The user requires continuing preparation, but the previous hardware pause has not been cancelled. Firmware iter05 was not changed.

## K05151
Source `D0178`, original line5, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Result

## K05152
Source `D0178`, original line7, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**68 focused tests passed, 7.35s.** Application activation of the new size has been additionally verified. It has been established that the analyzed button branch of full power-off is not itself a C runtime reset and does not clear pending OTA state. A verified FW189 bootloader/recovery could not be obtained from the saved local files. The old OTA ACK race is not fixed; an explicit prohibition on hardware start has been introduced until it is closed and boot/recovery evidence is obtained. This is a readiness limitation, not a resolution of the protocol race.

## K05153
Source `D0178`, original line9, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Image inventory

## K05154
Source `D0178`, original line11, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

pc/radio/tracker_ota_readiness.py generates a machine-readable report outputs/fw189-iteration06-readiness-20261007.json with lengths, SHA 256, heuristic vector candidates, and blockers. Reconciled:

## K05155
Source `D0178`, original line13, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- FW189 container outputs/pico4e/trackerbin/Swift2.0_sv1.89_20241213_b189_app.bin: 193385 bytes, header 0x141 + two identical copies of 96532 bytes. Each copy equals frozen outputs/swift189-image.bin. Vectors refer to application base 0x1000; reset 0x13a5. This is not a dump of region 0..0xfff.
- PhoenixSta119600 and PhoenixCtrl122008: separate controller/station application images; not proof of tracker bootloader.
- Swift_hm01_20230707_b97.bin: 140304 bytes, SHA75e5df6d5f4dffd20f89678e2b65d8afd4bf705ea9d1254eee6dbc9d417b28e7. Format differs: start 0000000000800200..., standard vector table search yields no result. **The b97 format is not parsed**, the absence of a vector candidate does not prove the absence of a loader in this unknown format. It is not recognized as a FW189 bootloader and is not used for recovery.
- Additional search for Swift/bootloader/dump bin/hex/elf names in K:/AI and H:/_other/AI revealed duplicates of these files. H:/_other/AI/pico_tracker_research/fw/trackerbin/b97 has the same SHA. outputs/firmware-standalone/Swift.bin is the same 193385-byte FW189 container; Swift-image.bin is the exact frozen app SHA 477000fc... . No separate verified tracker flash dump was found.

## K05156
Source `D0178`, original line18, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Firmware final iter05 remains 97320 bytes, SHA395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f, crc1f. There is no required bootloader in its vectors and source image.

## K05157
Source `D0178`, original line20, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Application activation for the new image

## K05158
Source `D0178`, original line22, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

New tests execute the real 0xfa98 on iter05 ROM with mocked NVMC write/delay. Standard verified flags RAM 0x2000224f=1 and metadata state 0x20002260=4, packed size/CRC 0x20002250=(97320<<8)|0x1f are set.

## K05159
Source `D0178`, original line24, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Both branches have been verified:

## K05160
Source `D0178`, original line26, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- argument 1 → metadata +4=97320, +8=0x1f, +c=0xaaaaaaaa, +10=0;
- argument 0 → same length/CRC, magic 0xbbbbbbbb;
- native metadata of 24 bytes is CRC-compared with the written bytes 0x2f000, after which AIRCR contains SYSRESETREQ.

## K05161
Source `D0178`, original line30, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is **not** bootloader execution, real NVMC, or proof of bootability. Magic is only named after values: the copy/rollback meaning of these two magics is not established by the loader code. The image below 0x1f000/other native storage pages does not prove the absence of additional bootloader restrictions.

## K05162
Source `D0178`, original line32, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

From native callers: 0x8548 calls 0x852c/0xa688/0xb69c, with a certain branch tail 0xfa98(argument 1); 0xa638 switches subsystems and calls 0xfa98(argument 0). Therefore, successful finish and subsequent teardown/disconnect cannot be considered a guaranteed harmless stage-only experiment. In this iteration, hardware finish was not performed.

## K05163
Source `D0178`, original line34, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Why the button does not prove a clean session

## K05164
Source `D0178`, original line36, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

0xbcd0..0xbcf6 call native teardown0x7000/0x7bd4/HFCLKSTOP0x5fe0, LED 0xcd8c, and 0x8564, then return. 0x8564 configures GPIO via 0x7fe6; there is no C startup or SYSRESETREQ in the shown chain. This is not a claim about all possible external power-management effects: they are not modeled.

## K05165
Source `D0178`, original line38, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The test for stock and dual executes this chain with mocked teardown/LED, real GPIO helpers, and a synthetic pending operation in 0x20002240.. . After returning, pending bytes remain. Then the real C startup 0x1200 indeed zeroes them out. Consequently, available evidence is insufficient to consider the "turn off/turn on" gesture a clean boot. Dual clearing mode in 0xbce6 also does not mean clearing OTA worker state.

## K05166
Source `D0178`, original line40, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The previously reproduced race continues to exist: the old start was accepted by the native dispatcher; the worker was not executed; the response queue is empty. A new sender sees an empty barrier, issues start. The old worker completes erase and issues an old unmarked ACK, which arrives under fresh RF tx_generation. Clearing the visible queue and reconnect do not distinguish this origin.

## K05167
Source `D0178`, original line42, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The nonce of the new mode-query does not fix stock OTA: native OTA ACK does not contain nonce, image identity, or session marker. Manual JSON fresh_start_verified=true is not considered proof.

## K05168
Source `D0178`, original line44, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Loader changes

## K05169
Source `D0178`, original line46, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Added pc/radio/tracker_ota_readiness.py:

## K05170
Source `D0178`, original line48, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- inventory(root) — only reading local files;
- require_hardware_ota_clearance() — reports three specific current blockers and rejects hardware start. JSON/config bypass is absent. For future removal of the prohibition, it is required to replace it with a verified mechanism after obtaining evidence; this is not a permanent flashing prohibition.

## K05171
Source `D0178`, original line51, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In roy_ram_loader.run, verification is performed during tracker_ota_job **before ownership and hardware runner**. In tracker_ota_runtime.preflight, it is performed before setting up configuration. In _poll — repeated verification immediately before creating OtaSession; error is stored in result as failed. Thus even a direct call to runtime does not start erase/write.

## K05172
Source `D0178`, original line53, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The previous configuration check has been separated into plan_preflight(job,config,args): it is available for offline preparation and tests, and is not an authorization of hardware execution. load_job and whitelist have not been expanded; low-level OtaSession remains a pure offline state machine without hardware I/O, its tests reproduce transmission and race.

## K05173
Source `D0178`, original line55, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Tests forbid bypass via job/config boolean claims, verify rejection before _run_owned, retention of prestart failure, and absence of OtaSession. Old planner tests have been migrated to plan_preflight; runtime test now expects rejection instead of issuing start. This is a deliberate elimination of the previously accessible unsafe hardware path, rather than masking a failing sender test.

## K05174
Source `D0178`, original line57, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What is needed to actually close blockers

## K05175
Source `D0178`, original line61, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

An exact-device flash dump with proven base/address/length and tracker hardware identity is needed. Minimally for map 0..0xfff; for a full analysis, full flash plus UICR, metadata 0x2f000, and native configuration pages are desirable while preserving privacy. **Reading was not performed, access method/SWD protection is not established.** Do not apply recover/mass-erase/unlock: they may destroy the original loader/settings. Do not issue a new SWD command before determining access and obtaining separate authorization for the hardware stage.

## K05176
Source `D0178`, original line63, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

From the dump, it is necessary to reconstruct reset-vector path, SPI staging reader/page mapping, length/CRC/signature/version checks, copy ordering, protected pages, and power-loss handling. Recovery must work without launching corrupted application. Normal reverse OTA is possible only if application and radio are alive; presence of stock.bin is insufficient evidence.

## K05177
Source `D0178`, original line65, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Clean start of the first OTA on stock

## K05178
Source `D0178`, original line67, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The first stock FW189 does not have our new nonce protocol; changing its ACK before installation is impossible. A proven real reset/startup, clearing relevant BSS, and absence of another OTA writer are needed. Physical reset/power removal must be confirmed in hardware, rather than only by product version 189 or UI reconnect. If direct debug access is available, read-only evidence of specific RAM flags/queues and startup can be prepared; current methods of access to the tracker are not proven. An idle snapshot of a single moment without checking scheduling/concurrent writers does not resolve the race automatically.

## K05179
Source `D0178`, original line69, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For subsequent modified versions, nonce-correlated OTA idle/session state and ACK can be designed, but this is a separate protocol patch; it does not resolve the bootstrap of the first flashing of the stock device.

## K05180
Source `D0178`, original line71, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Continuation limitation

## K05181
Source `D0178`, original line73, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Analysis of the available application and local inventory was continued without hardware access. Missing bootloader bytes and real recovery cannot be obtained through additional emulator tests or declared proven. The next hardware read-only stage is a separate action; the previous user pause remains for now. A report of each step is preserved; firmware release status is still approved_for_ota=false.

## K05182
Source `D0178`, original line75, SHA256 `01ba4d6a6cbe40ee0a22b0fae39b3fa4880cbe7b2f86853a2cfd959f478b3d8c`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Evidence: outputs/fw189-iteration06-readiness-20261007.json; outputs/fw189-iteration06-tests.xml; pc/tests/test_fw189_readiness.py. 68 tests include native pending-worker race reproducer, full stock OTA, new metadata, retained OTA state, hardware gate, and loader regressions. SPI/NVMC/GPIO side effects/IRQ/allocator limitations are stated above.

## K05183
Source `D0179`, original line1, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 07 — mode verification under loss and native pool

## K05184
Source `D0179`, original line3, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. Firmware iter05 was NOT changed: SHA395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f, 97320 bytes. Whitelist/hardware gate remain closed. No SSH/SWD/RF/OTA/service actions.

## K05185
Source `D0179`, original line5, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Reviewer remarks

## K05186
Source `D0179`, original line7, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Accepted: previous refresh 2s/lease 3s gave less than 1s for the background query and could provoke disconnections. The reviewer's numerical estimate of 8%/25s is a model of independent losses, not a measurement; in this iteration it is not reproduced as a hardware fact. Not accepted as a proven fact: the station will necessarily detect any silent reboot immediately. A limited confirmation period is retained, now substantially longer.

## K05187
Source `D0179`, original line9, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Station fixes

## K05188
Source `D0179`, original line11, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

PeerEngine defaults: refresh 10s, query timeout 10s, lease 30s. Lease is still counted from first_sent of the confirmed request, rather than the time of a late response. A query response of age >= 10s is not accepted. Initial connection without confirmation does not receive N/camera-sync.

## K05189
Source `D0179`, original line13, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

After 10s when the pipeline is free, a new nonce is requested. If the background query is not confirmed within 10s, but the previous proof is still valid, pending is released, command 1 sequence is incremented, nonce is deleted, and retry is postponed for another 10s. Mode/phase/connection state remain active. Sequence increment is necessary: native dedup could have already accepted the old request, while the previous response was lost; the next nonce must go with a new record sequence.

## K05190
Source `D0179`, original line15, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A single late refresh does not break the connection. At the same time, the standard RF-liveness check of 3s without responses remains unchanged. Lease 30s is a finite limit: if the mode could not be confirmed during it, the station clears mode and reconnects instead of using the old N indefinitely. After a very late first refresh, margin may run out; policy does not promise infinite availability under losses.

## K05191
Source `D0179`, original line17, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

On missing initial 7e, radio_mode_error is reported: unsupported firmware, wrong dual-mode ID configuration, or radio loss. This is **not** a proven classification of stock firmware: silence is indistinguishable from radio loss. Live also shows radio_mode_query_failures. With proven reconnect/re-discovery/start, confirmation is cleared as before. The expiration error contains the actual configured lease.

## K05192
Source `D0179`, original line19, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In pc/tests/test_fw189_dual_mode_v2.py, old temporal tests explicitly specify 2/3/3 seconds as a short-budget fixture to preserve the verification of the expiry boundary; production defaults 10/30/10 are tested by the new file pc/tests/test_fw189_mode_review07.py.

## K05193
Source `D0179`, original line21, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Correction of the heap assumption

## K05194
Source `D0179`, original line23, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Native response 0x9368 calls 0x92a8, which allocates a node via 0x8c3c from descriptor 0x20000158. After real C startup, descriptor bytes are 14 65 00 00 83 10 00 20: **20 slots, stride 101, pool 0x20001083**. The allocator scans the occupancy byte of each block; free 0x8c74 zeroes the byte before the pointer. This is a fixed pool 0x20001083..0x20001867, not heap 0x20005158..0x20005350.

## K05195
Source `D0179`, original line25, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Consequently, the formulation "each mode response takes 40 bytes from the reduced 504-byte heap" is not confirmed by the native allocation path. The overall shortage of 504 bytes of dynamic heap is still a separate unknown risk; a complete allocator audit of the application has not been performed.

## K05196
Source `D0179`, original line27, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The new test excludes the previous mocks0x8c3c/0x8c74/0x8c7c, loads real C startup .data/BSS, and executes native pool allocator, record dedup/queues/ACK retirement. IRQ lock/unlock helpers are still mocks; this is not an IRQ concurrency proof.

## K05197
Source `D0179`, original line29, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Results:

## K05198
Source `D0179`, original line31, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- native queue init takes 3 pool nodes; 17 remain;
- one mode query takes another 1;
- 50 retries of the same query/sequence do not create new nodes;
- different query sequences without ACK fill all 20 nodes; further responses are lost upon exhaustion, without guessed mode;
- 17 response records are unloaded with correct native ACK retirement; used returns to 3;
- the next new query allocates a node again and responds.

## K05199
Source `D0179`, original line38, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This confirms bounded exhaustion/recovery, rather than guaranteeing queue availability with IMU/other reliable notifications and real radio. New host refresh reduces the frequency of mode queries; hardware impact has not been measured.

## K05200
Source `D0179`, original line40, SHA256 `6e464584176b0358505f112737b57e28ca562218dd478e57c8dddc23765ade64`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Checks and remaining limitations
