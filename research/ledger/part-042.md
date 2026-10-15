# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K03281
Source `D0398`, original line1555, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

- Both controllers reappeared without restart; 500us RX retest gave73 START,
  zero packets, full restoration, both controllers still listed after.
- Concurrent and independent --pollposes reads empty: no continuity proof.
- Pico power state Asleep; await user wake/confirmed tracker connection before
  reference RF capture. No TX/re-pairing. 52 offline tests pass; RX profile
  validates reviewed framing/CRC/PHY before any debugger access.

## K03282
Source `D0398`, original line1563, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Awake reference and station ABI, 2026-10-06

## K03283
Source `D0398`, original line1565, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

- Pico awake and live IMU IDs3/5 confirmed in logs. Roy initial channels1/37/77
  scan29/33 gave323/1023 START, zero ADDRESS/END/CRC events, full cleanup.
- Station C exports require handle first; corrected native GetTrackerStatus
  returned0, three cached registration serials TR0189. No verified FICR exposed.
- Tracker local FICR[1]→RAM20000efe; peer seed20000fd8; paired base is per-byte
  modulo256 sum then RBIT. Identity length13 payload5..8 has FICR bytes.
- User authorized tracker restart; guarded SetTrackerReset(handle,1) returned-1,
  no confirmed restart. Await physical off/capture cue; never unbond/OTA.
- 54 offline tests pass. See docs/roy-awake-reference-progress.md.

## K03284
Source `D0398`, original line1576, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Raw capture follow-up, 2026-10-06

## K03285
Source `D0398`, original line1578, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

- User wore PICO and woke Frame controllers. Frame SSH/Wi-Fi; PICO USB ADB.
- Capture34 missed physical power-on; no initial handshake captured.
- Raw39/40/41/42 restored recurring MPSL slots after controllers woke.
  Timer-only38 also BLOCKED while asleep; cause remains a hypothesis.
- Raw40/41/42:219/821/119 slots,7/19/7 partial DMA fragments, zero END and
  zero software CRC candidates. No validated PICO RF reception or 6DoF.
- Preamble BALEN1 recipe is experimental nRF51-derived, outside documented
  nRF52833 range. Establish supported capture/alignment before active polling.
- Both controllers listed after42, but Left pose continuity unproven.
- All restoration checks passed. See docs/roy-raw-capture-progress.md.

## K03286
Source `D0398`, original line1590, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Correction: END observation versus RADIO callback, 2026-10-06

## K03287
Source `D0398`, original line1592, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

- Hardware END occurred without RADIO callback in44 (7 vs0). Earlier zero
  END callback counts do not prove absent hardware completion.
- TIMER teardown now observes END:45 yielded218 slots/13 raw records,
  9 completed DMA/4 partial, zero Swift CRC candidates. All cleanup passed.
- Supported-field preamble comparison43 did not validate PICO reception.
- Stock IRQ routing unchanged; inspect forwarding read-only before modifying.
- 56 tests pass. No standalone tracker poses yet. See latest correction in
  docs/roy-raw-capture-progress.md and original research artifact.

## K03288
Source `D0398`, original line1602, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Offline alignment search and Swift register confirmation, 2026-10-06

## K03289
Source `D0398`, original line1604, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

- 55 raw records,14902 broader offline CRC hypotheses,0 matches. Framing
  alternatives/order/inversion do not establish PICO reception.
- Swift VA50ec writes PCNF0=40008 from literal5108, PLEN0 even with MODE4.
  CRC path54d6 INITFFFF/poly11021/CRCCNF2 includes address (no SKIPADDR).
- Roy RADIO vector44=33023 calls10644 radio-owner dispatch internal code4;
  public MPSL signal mapping not established by that internal argument.
- Stock IRQ handlers untouched; TIMER polling is current bounded fallback.
- See latest section in docs/roy-raw-capture-progress.md. No standalone poses.

## K03290
Source `D0398`, original line1614, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Positive known-address BLE reference, 2026-10-06

## K03291
Source `D0398`, original line1616, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

- BLE48/49 each accepted1 hardware-CRC-valid advertising PDU; independent
  CRC24 calculation matches both RXCRC values. Raw51 recovered two matching
  PDU+CRC buffers with independently valid CRC. Known-address RX/DMA proven.
- This does not validate BALEN1 promiscuous capture or any Swift packet.
- TIMER fallback now checks CRCSTATUS for CRC-enabled frames as well.
- PICO Awake confirmed for Swift50:322 slots,0 ADDRESS; pairing unknown.
- Live radio snapshot46 is non-atomic, may mix settings; do not use as profile.
- All restoration passed;57 tests. No standalone poses. See reference section
  in docs/roy-raw-capture-progress.md and original research artifact.

## K03292
Source `D0398`, original line1627, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Direct channel getter control, 2026-10-06

## K03293
Source `D0398`, original line1629, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

- Read-only StationClient channel getter ABI verified56-byte output. Returns
  -1 Asleep and Awake. Same-client GetStationChipid returns0/[PRIVATE_LINK_VALUE].
- Channel config unavailable through this getter; do not call zero buffer data.
- Tracker FICR/paired address/current mask still unknown; addr1233 unverified.
- tracker_test --help runs default diagnostic, not help. Avoid unknown commands.
- New helper pc/radio/pico_channel_read.c only getters, no setters or pairing.

## K03294
Source `D0398`, original line1637, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Controlled preamble comparison and software dewhitening, 2026-10-06

## K03295
Source `D0398`, original line1639, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

- BLE controls58/60 gave2/3 hardware-CRC-valid frames; pseudoaddress59
  gave12 raw records with0 valid software CRC. Do not call promiscuous RX proven.
- Known-address/no-hardware-whitening61 gave2 software-dewhitened CRC-valid
  PDUs, one matches reference48 exactly. Software decoder validated for BLE.
- Tests55/57 only1 slot then BLOCKED, inconclusive. All cleanup passed.
- 57 tests; no firmware/IRQ/TX changes. Swift pairing/standalone poses unknown.

## K03296
Source `D0398`, original line1647, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Independent review of Gemini specification, 2026-10-06

## K03297
Source `D0398`, original line1649, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

- Gemini spec is a partially verified draft, not a proven RX/pairing profile.
- Swift ASCII1233 producer confirmed. SLPI four-float loads confirmed;
  air-layout/quaternion order/on-tracker producer require independent trace.
- Phoenix13144 profile differs from Swift50e8; address71764129 known RF-test
  syncword, pairing association unproven. PCNF1=300ff whitening OFF.
- No TX/pairing changes. See docs/gemini-swift-spec-review.md.

## K03298
Source `D0398`, original line1657, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Phoenix ESB application paths and load base, 2026-10-06

## K03299
Source `D0398`, original line1659, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

- Phoenix image LOAD_BASE1000 confirmed by vectors/data pointers. Station
  labels in earlier reviews were file offsets, add1000 for actual VAs.
- Application filed51e /VAe51e BASE0 API785634aa→RBIT552c6a1e matches Swift.
- ESB defaultsE7/43 are init values; application overrides active bases.
- Prefix from RAM20004942 dynamic; live pairing address/channel still unknown.
- Station DTM strings confirmed with corrected VAs. No new RF/device changes.
- See docs/phoenix-esb-progress.md and outputs/phoenix-esb-evidence.json.

## K03300
Source `D0398`, original line1668, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Tracker prefix writer and bilateral BASE1 arithmetic, 2026-10-06

## K03301
Source `D0398`, original line1670, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

- Station VAe422 explicitly writes tracker role prefix3 toRAM20004942;
  VAe528 sets pipe0 prefix throughRBIT+REV ->C0, matching Swift7fa4.
- StartupBASE0552c6a1e/C0 profile is bilateral evidence, same as prior RX.
- Station VAe5da..e5f4 adds per-record+8 bytes and station seed modulo256,
  then BASE1 RBIT, matching Swift formula. Live per-device bytes unknown.
- Do not repeat empty startup scans as though a new active pairing address
  was recovered. Trace pairing record exposure. No hardware changes this step.

## K03302
Source `D0398`, original line1679, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Pairing report correction and passive identity helper, 2026-10-06
- Phoenix16B08: AA55AA55 or zero state clears validity, not valid marker.
- Reader populates two flat entries plus five runtime entries (seven overall);
  runtime ID+8 differs from flat ID+0. Do not blend these structures.
- JSON path direct ADB read denied; trackerid field meaning unverified.
- GetWhiteListNum only returns count; live62 result-1 is not count0.
- Local19B discovery listener observed no events20sec; no discovery switch.
- Helper pc/radio/pico_identity_read.c compiled warning-free; no RF writes.
- Station bytes[PRIVATE_LINK_VALUE] LE word15547374; live peer_seed equivalence unproven.
- See independent-check section in docs/phoenix-esb-progress.md. No Swift poses.

## K03303
Source `D0398`, original line1691, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## JSON writer verification, 2026-10-06
- trackerid is converted logical8bit slot index: VA9CA40/48/84 ->9D440.
- Writer stores index string at9D9AC..9D9C8; bond flag event+37 bit6.
- This metadata JSON is not a full RF identity source. Gemini sample JSON
  is reconstructed, not an exact file dump; current disk values unobserved.
- Preserve previous journal/entry/seed corrections. No device mutations.

## K03304
Source `D0398`, original line1699, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Active discovery63, 2026-10-06
- Verified command2C setter DDC4 updates RAM20000274 mode/count bits only;
  handler109A8 supports FF readback. Direct setter has no flash calls.
- --discover helper bounded12sec: mode1/count3 ret0, noevents, mode0 ret0.
- PICO Awake and all3 trackers online/bonded afterwards; no full IDs yet.
- Passive silence not explained solely by disabled discovery. Trace beacon
  conditions and flags; do not repeat same scan as new identity evidence.
- No dongle, disable_spi_hid or reflashing; retain stock Roy plus SRAM/MPSL.

## K03305
Source `D0398`, original line1709, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Swift identity response gate, 2026-10-06
- Swift length9 request path68F0: state halfword+4==AAAA skips6844 full-ID
  response. Non-AAAA calls6844; FICR producer689E, payload ID+5..8.
- Station BD20 existing valid records use length21; discovery bit consulted
  on invalid-entry branch BE06. No claim all paired beacons are impossible.
- Root on PICO not presently required; no pairing reset or rooting performed.

## K03306
Source `D0398`, original line1717, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Shortened-address reference66, 2026-10-06
- BLE BASEBED60000/PREFIX89/BALEN2 captures initial3 address bytes;
  DMA starts remaining8E then whitenedPDU.66:422slots/18raw/12END,
  eight established CRC24 advertising matches incl golden48 at bitoffset8.
- Does not validate Swift/suffix RX.64 zeroADDR;65/67 blocked/inconclusive.
- All cleanup true, HMD/Right poses only. Loader exact profile whitelist,
  capture labels corrected;58tests. No Roy firmware/IRQ/C-module changes.
- Ordinary station CFA4 stores incoming fullID at runtime+8, host exposure
  still unproven. GetTrackerInfoForAlg writes scalars only, tail not IDs.

## K03307
Source `D0398`, original line1728, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Suffix capture69 and conditional Swift71, 2026-10-06
-69:419slots,2CRC24-valid BLEPDUs atoffset0 proves suffix3byte RX onRoy.
- HistoricalID3EED +presumedseed74735415 ->BASEhigh16 4D06/tail064DC0;
  low16ID constrains address tail, not leadingbytes. Pure helper/tests added.
- Phoenix94C0 FICR64 seedsRAM492E; sum writes separate4944. Host seed
  representation linkage still needs complete response-producer verification.
-70 PICOAsleep;71Awake420slots0ADDR. No currentID mapping, not final rejection.
-68blocked1slot; all cleanuptrue.60tests; no flash/TX/IRQ changes or pairing.

## K03308
Source `D0398`, original line1738, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Whitelist producer and Gemini table correction72, 2026-10-06
-0x30 actualVA D0E4..D140 returns(slot+1,bonded) pairs, no IDbits at all.
- Gemini Phoenixlabels againfileoffsets; add1000. Event17 u32 is not FICRproof.
- GetExtDevStatus live72 ret0/cache96B address fieldszero; no fullID recovered.
- LiveStationChipid bytes[PRIVATE_LINK_VALUE]. ConditionalID3EED/C3BF/AD1D highBASE
  4D06/EC4C/8409, tails064DC0/4CECC0/0984C0. Newpairs/mapping unverifiedlogs.
- Do not use Gemini reversedseed/address table. Helper read-only; no RFwrites.

## K03309
Source `D0398`, original line1747, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Live low16 confirmation75 and scans73/74, 2026-10-06
- Targeted timestamped logcat independently confirms3EED/C3BF/AD1D at
  station indices0/1/2; supersedes Gemini-only status. Full32 still unknown.
-73EC4C419slots/74=8409 421slots,zeroADDR; PICO Awake,3trackersonline.
- Allcleanuptrue;73HMDonly/74HMDRight poses, Left continuityunproven.
- All3conditionaltails tried; do not blindlyrepeat. Check2M PHY calibration
  and negotiated profile.60tests; noflash/TX/pairing/IRQ changes.

## K03310
Source `D0398`, original line1756, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## PHY candidates prepared; Frame offline, 2026-10-06
- Swift521A directlywritesMODE;50EC PCNF040008. nRF52833MODE4 spec
  requiresPLEN16; runtimeSwiftcombination/order needs capture, not assumption.
- Narrow comparisons4p8/4p16/1p8 ready locally;61tests. No2M calibration yet.
- SSH[DEVICE_IP] timedout,USBFrameabsent. Attempt76didNOTexecute;
  uploadfailed, localfilespreserved. AwaitFrameIP/USB then reuploadboundedSSH.

## K03311
Source `D0398`, original line1764, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Paired-prefix correction and live shortened captures, 2026-10-06

## K03312
Source `D0398`, original line1766, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

This section supersedes the earlier blanket C0 paired-prefix assumption and
incorrect attribution of the second FICR getter at F348 to RAM492E.
Phoenix VA E3FA..E416 cycles RAM20004D85 modulo FIVE (literal E5A4=CCCCCCCD),
not three. E5C8 passes this index to B31C; E5CE/E5D4 uses the same index for
runtime records. For observed indices0/1/2 the RADIO prefixes are00/80/40.
Role3 at RAM20004942 produces C0 on the initial path only. Final live pairing
mapping still requires stronger confirmation; no new pairing was performed.
Seed source: F342 calls94C0 with r5+6, r5=20004928, thus RAM2000492E.
F348 calls94C0 with RAM20004D7C (a second copy), NOT492E.
F354/F358 mirrors the seed from492E to RAM20005C90+0F.

## K03313
Source `D0398`, original line1777, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Frame SSH[DEVICE_IP] is restored; USB PICO [HEADSET_ADB_SERIAL] remains non-root.
All new experiments are onboard Roy SRAM/MPSL RX-only, no TX/ACK/flash/erase,
no stock HID shutdown. PICO was Awake during checked captures. No standalone
SteamVR tracker poses or complete six-degree-of-freedom tracking yet.

## K03314
Source `D0398`, original line1782, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Corrected shortened raw profiles: BALEN2, ENDIAN1, no whitening/CRC hardware,
MODE4, PCNF0=0. Slot indices0/1/2 use BASE4D060000/EC4C0000/84090000 and
PREFIX00/80/40. Old C0 profiles remain explicitly historical candidates.
-83:424slots,2complete frames, channels2/80, lengths22/28.
-84:262slots,1complete frame, channel80, length10.
-90:622slots,5address events,3complete and2partial frames, channel2.
  Complete lengths22/28/22. Partial captures are excluded from CRC fitting.
-91:84slots,0address;93:6slots,0address, broad scan not covered.
  Third tracker therefore not captured/ruled out.

## K03315
Source `D0398`, original line1792, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

New offline decoder radio/swift_partial_crc.py fits missing16address bits under
explicit MSB CRC assumptions.83/84 agree on candidate B825064D00 (BASE4D0625B8);
90 agrees on BB764CEC80 (BASEEC4C76BB). Payloads consistently start0002 followed
by a body length equal to total minus4. This is strong cross-frame evidence,
NOT hardware CRC validation or recovered verified FICR identity. One arbitrary
frame always fits a missing16bit prefix; independent consistency is essential.
Raw framing with four control bits agrees across frames; three bits does not.
CRC-address bit/byte order remains unresolved; first profile variants have not
matched a full hardware address. Do not convert candidates into certified IDs.

## K03316
Source `D0398`, original line1802, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Hardware trials85/88/89 granted1/2/1slots: inconclusive.86 granted621slots and
87=821 with no ADDRESS events (MSB candidate and alternate bit order).92 had
68slots,0ADDRESS. Compare with raw successes, not a claim PICO radio is absent.
PHY trials76a/77 were421/422slots0ADDR under oldC0;79/80/82 incomplete scheduling;
BLE control78 had420slots and10hardware-CRC-valid records, but proves BLE1M only.
Full2M reference calibration and stable MPSL service remain unfinished.

## K03317
Source `D0398`, original line1809, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

All completed83..93 reports show session closure, RAM/MPU/static-count restoration
true, flash_written false. Pose logs mostly HMD only;86 also device2. Both Valve
controllers' continuous coexistence is NOT established by these runs.
Evidence: outputs/paired-prefix-live-report.json and per-run raw/log/profile/
recovery/pose files.65tests passed including live golden-frame consistency and
corruption/truncation checks. Harness preserved at pc/radio/ram/roy_passive_probe.py.
Next: resolve full-address CRC order with sufficient slots; confirm hardwareCRC,
recover third profile, then decode telemetry, establish permanent scheduling,
and implement optical/fusion + SteamVR driver. PICO root is not needed for this
current passive route. Receiving IMU alone is insufficient for standalone6DoF.

## K03318
Source `D0398`, original line1821, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## All three shortened profiles captured; frozen-state decoder deployed (2026-10-06)

## K03319
Source `D0398`, original line1823, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Supersedes the previous third-profile-not-captured status.96 was run after ADB
wake224 with PICO Awake before and after:820slots,4END/ADDRESS. Two usable frames
(length22 and10, channel80) independently agree on post-address CRC state0DAB.
One length65 exceeds saved40byte DMA despite partial=false; one length32 has a
wrong CRC state and is rejected. Do not equate END or partial=false with valid.
Software-fit candidate3B24098440 (BASE8409243B) is NOT a verified RF address/FICR.

## K03320
Source `D0398`, original line1830, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Frozen post-address CRC states: slot0=7F24, slot1=1ABA, slot2=0DAB. Training on
one frame per profile then validating other frames yields five holdout CRC16
matches total (two/two/one). This avoids needing a full32bit identity for raw
filtering. CRC remains software checked under stable address/framing assumptions;
hardware-full-address reception and mapping to physical FICR are still unverified.
No authentication/encryption claim. Payload decoding into physical IMU/fusion
units is not implemented; experimental64byte codec is explicitly marked MOCK,
with its obsolete DTM/CRC24 description corrected. Do not use it for live poses.

## K03321
Source `D0398`, original line1839, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

radio.swift_partial_crc CLI uploaded and executed directly on Frame:
cd /home/steamos/pico-ram-work/python && python3 -m radio.swift_partial_crc
 --capture ../swift-paired-prefix96.json --crc-state 0x0dab
 --out ../swift-paired-prefix96.filtered.json
Result:2accepted/2rejected. Training frame included in this count; holdout proof
is the separate independent frame, not all accepted rows. Evidence copied to
outputs/swift-paired-prefix96.filtered.json and learned-radio-profiles.json.

## K03322
Source `D0398`, original line1847, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

94:822slots,0ADDRESS on full-address byte+bit reversed variant, PICO power not
bounded at both ends.95:1020slots,0ADDRESS; PICO was Asleep afterwards, so no
negative protocol conclusion. Earlier full variants remain unconfirmed; they
are experimental whitelist candidates only. Loader default full-address path
now rejects arbitrary unreviewed BASE/PREFIX pairs;68tests pass.96 cleanup all
true; no flash/TX/HID stop. Pose logs96 HMD only: controller coexistence and
sustained radio service remain unproven. Final goal is NOT achieved: no real
standalone6DoF/SteamVR tracker poses. Next work is sustained scheduled RX,
verified telemetry parser, standalone optical/fusion and SteamVR integration.

## K03323
Source `D0398`, original line1858, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Slot counter 0x20004D85 and full-ID exposure, 2026-10-06
- RAM20004D85 has ONE writer, VA E416: on dispatcher event 0x144 (DF62->E3FA)
  idx=(idx+1)%5; E41A stores idx+2 to RAM200002A8 (global device index; flat
  entries 0/1 = hands). Not filled by pairing; zero-init BSS. Other refs only read.
- B31C(idx,pipe) writes idx as prefix byte, RBIT+REV -> air prefix rev8(idx):
  00/80/40/C0/20. Role3 "C0" equals rev8(3): the startup/pairing slot, not a tracker
  slot. E468: idx<=2 normal tracker path; 3/4 separate path. Record=[20006D58+4*idx].
- Back-computed from CRC-fit BASEs (rev8(BASE byte) minus seed BYTES 74 73 54 15,
  per byte; not the u32 [PRIVATE_LINK_VALUE]), UNVERIFIED by hardware address match:
  bit-order A: slot0 3E ED 50 08, slot1 C3 BF 1A C8, slot2 AD 1D D0 C7;
  bit-order B: [TRACKER_ID], [TRACKER_ID], [TRACKER_ID] (record+8 byte order).
  BASE A/B: 4D0625B8/[PRIVATE_LINK_VALUE], EC4C76BB/EC4C66FB, 8409243B/[PRIVATE_LINK_VALUE]. Bytes [8],[9]
  match live logcat; [10],[11] are only a software fit. BALEN2/3 give no solution for
  the learned CRC states, so the 5-byte address model stands. Hardware full-address
  was tried only for slot0 A/B on ch2 (86/87, 0 ADDRESS); slots 1/2 and ch80 untested.
- Station VA CFA4: packet[0]==3, slot=packet[1]-2, packet[4..7]=full ID. For bonded
  records (+4==AAAA) all 4 bytes are compared; D01E stores to record+8. So a
  reconnecting bonded tracker sends its full ID in clear. Capturing on the whitelisted
  startup address 552C6A1E/C0 (hardware CRC, ch 1/37/77) during a tracker power-cycle
  would give a CRC-checked ID without re-pairing. The Swift builder of this packet is
  not located yet.
- handshake-id100..104 (startup 552C6A1E/C0, hw CRC, ch 1/37/77, ~4x10 s): 0 ADDRESS.
  Pico logcat shows three reconnects INSIDE windows 101-103 (111064G after user
  power-cycle, plus user power-cycles of 110865G/110282G, 2.7 s restore). So bonded
  reconnects do not use this address/channels (or PHY): the 0x03 ID packet likely
  travels on the paired per-slot address. All restoration true, flash untouched.
- Host exposure: logcat prints only record[8],[9]. Full u32 is logged only by
  discovery CE58 "find rssi:%d batt:%d %08X" (needs discovery bit + unbonded
  tracker identity reply). 18D08 "write bond id" is the CONTROLLER path.
  /sdcard/AlgSwift logs contain SNs only; dumpsys PicoStation-new empty.

## K03324
Source `D0398`, original line2035, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Orientation and spline helper review, 2026-10-06
Gemini imu_orientation.py contained invented fallback samples (zero gyro and
1g accel) when packets omitted IMU, and assumed5ms timing. Removed those fallbacks;
manager now requires real gyro/accel fields and explicit finite0<dt<=50ms. Sparse
current RF samples must not be integrated with an invented5ms interval. Added
regression for missing IMU/timing and sparse gaps. Madgwick local reference is
+Z-up, not OpenVR+Y-up; magnetic heading/world alignment is not implemented.
1024LSB/g is an observed provisional conversion, not verified sensor range/config.
The kb_spline helper uses assumed bilinear residual interpolation and guessed
crop convention; self round-trip tests do not validate Valve's spline basis.
Keep it research-only until compared with native projection or calibrated targets.
A new isolated recording08-47-20 was captured (38332520byte OCC). First and later
visible/IR frames still show a hand-held ringed controller. The suspected Motion
Tracker on the tabletop has no identifiable IR constellation in those inspected
frames. This does not prove its LEDs never activate: temporal/other-view checks
are still required. Evidence: outputs/tracker-camera101-ir-later.png and visible.
Standalone goal remains unachieved; never publish an artificial pose from these
research helpers merely because they return a normalized quaternion.
- Follow-up 131-144 (Frame worn, controllers awake; BLOCKED after 1 slot happens when
  Frame idles, e.g. 111-114/120-122): reconnects (incl. after long-press blink mode,
  bonds kept, pair_num 3) fell inside startup-address windows (start132) and a
  paired-slot multi window (141); no 0x03 frame. Coverage is only ~2.5% per
  address+channel (1500us/10ms, 6-way rotation; 7-9 frames per 10 s of ~200 Hz), so
  negatives do NOT refute the reconnect-ID path. Seen non-telemetry short frames
  "xx 0B 01 yy" and "xx 0A 04 yy yy 03 00" (likely station control/ACK, no ID bytes).
  Next only if needed: locate Swift 0x03 sender/channel offline, then single-profile capture.

## K03325
Source `D0398`, original line2089, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Gemini hopping/light report independently checked, 2026-10-06
Swift6EBE..6EDA selects table[index] (context+5) then increments index modulo
context byte+2. Confirms cyclic selection on this executed path, not its cadence
or current live mask. Full40-channel use and one hop per transmitted packet still
require caller/timing verification. Do not infer them from matching capture counts.
Critical omitted condition: Swift8D40 reads mode from input[0], accepts1/2,
then compares FOUR bytes at input+2 with RAM20000EFE using memcmp14958.
8D68 literal points to that cached tracker FICR location. Only a successful
comparison calls78F4 and tail-calls7C44. Thus this handler is NOT a one-byte
broadcast command; enabling light through this path requires correct additional
identity bytes (and outer framing/opcode must be verified). No RF TX performed.
Therefore full FICR is optional for passive IMU filtering but potentially required
for active standalone light control. A PICO-assisted optical experiment may prove
camera visibility; it is not an acceptable final standalone dependency.
Absence of startup calls in a simple xref search does not prove all runtime
states stay dark without PICO. Boot/callback paths and timing must be checked.
- REJECTED (2026-10-06): Gemini "full IDs" 3E ED AC EE / C3 BF AD EE / AD 1D AE EE.
  tracker_info_data.json (now read with PICO root) has only sn/bond/soft_ver/trackerid,
  no chip IDs. Those IDs are back-derived from an assumed air address "C0 <prefix> ..":
  bytes 2/3 = rev8(prefix)-0x54 and rev8(C0)-0x15 (AC/AD/AE, EE). With either bit order
  they do NOT reproduce learned CRC states 7F24/1ABA/0DAB. Never use them as IDs.
- Swift IR enable handlers 8D40/8D88 run 78F4 (30 Hz 0x82355 / 25 Hz 0x9C400) and 7C44
  only if memcmp(cmd+2, own FICR @20000EFE, 4)==0: standalone IR needs the full ID.
  Hop loop 6EBE..6EDA: idx=(idx+1)%num_channels over table @20000E10+5.

## K03326
Source `D0398`, original line2494, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## quick-bohr and publication audit (186)

## K03327
Source `D0398`, original line2496, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

Read docs/quick-bohr-audit186.md. Four-byte packet+5 field is preserved at internal record+0x1f; FICR semantics are not yet established. Public getter still returns 1233. Root cannot read stationservice maps in the current SELinux context. Code-only GitHub snapshot was fully verified (103 matching files).

## K03328
Source `D0398`, original line2498, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Raw Binder field validation187
Live raw 48-byte records all contain ASCII1233 at+0x1f before public API conversion. Full FICR is not in that field. New read-only pico_raw_status_read.c is tied to the verified client build; see docs/quick-bohr-audit186.md and outputs/pico-raw-status187.jsonl. Swift6844 FICR is buffer+0xa after sp+8 copy, not+5; decode the split container before Thumb disassembly. Frame SSH currently timed out.

## K03329
Source `D0398`, original line2501, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Dual receiver live188–189
Both10s passive dual-address captures restored RAM/MPU, noflash. 189:1019slots11frames,9decoded sensor frames, allRXMATCH0; working address onodd1/77 aswell80. Even-only livechannelassumption is disproven. Guarded tracker reset APIreturned0, physicalreboot/handshake unproven; timestampsdifferentclockdomains. Decoder preserves pipe metadata (3tests pass). See docs/dual-reconnect188-189.md. Goalactive; no6DoF or currentbothcontrollerclaim.

## K03330
Source `D0398`, original line2504, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Timing preparation190
RecordABI280unchanged; software callback TIMER0CC1 observation added inpadding+278/tagROYTIMEDRX1, notRFedge. Tagged ELF1340Bpassesloaderguard, hardwarevalidationpending. Nondualnullpipecompatibilityfixed;20focusedtests pass. ctlbin119600Bappimage isnotfullphysicalflashdump;CRCdoesnotindependentlyauthenticateFICR.

## K03331
Source `D0398`, original line2507, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Timing/mask194
Timedbuild needsROY_COUNTER_HIGH (priority0); omittedflag caused191/192blocked, notsleep. Corrected194:320slots5CRCrecords workingpipe0/channel1, callbacktime1807us equalsfallbackend, notRFtimestamp;cleanupverified. Capturedopcode08mask[2,6,8,10,12] withdelay18;activationunknown. Swift8ACC injectsASCII1233 into status+3. See docs/timing-mask191-194.md.

## K03332
Source `D0398`, original line2511, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Reception195-197
195:1016slots/17CRCrecords,4IMU onchannel1; opcode0A sync seen, direction/unitsunknown.196/197 hostmaskscan only1slot each, notvalidchannelcomparison;allcleanupverified. Loaderodd1/37/77 limitedtofullBASE[PRIVATE_LINK_VALUE];18focusedtests pass. See docs/reception195-197.md. No6DoF/currentcontrollercoexistence claim.

## K03333
Source `D0398`, original line2515, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Poll-tick receiver201
ROY_RX_POLL_TICK/ROYRXPOLLTICK1: TIMER0 END observation100us, originalteardown1800us; taggedloader restricts passivefullslot0/2000us/native/reserve.201:1025slots43CRC23IMU,allmaskchannels2/6/8/10/12;nodrops/blocked;cleanuptrue.OnlyHMDposeobserved,notcontrollercoexistence.18tests pass. See docs/timer-poll198-201.md. Hostretry198/199recoversblocked requests;noautonomoushopfollowingor6DoFclaim.

## K03334
Source `D0398`, original line2519, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## PICO Wi-Fi ADB (2026-10-06)
User requested charging PICO separately. Enabled existing adbd TCP5555 via USB; adb connect[DEVICE_IP]:5555 verified shell and existing hashsu root over network. Battery6percent at switch. Use explicit networkserial[DEVICE_IP]:5555 for further PICO commands; USB may be disconnected. This connection does not establish wakefulness or tracker state; IP/mode may change after reboot. Frame remainsSSH[DEVICE_IP].

## K03335
Source `D0398`, original line2523, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Slot capture202
ROYRXSLOT1 usesreserved15bits forslotordinalmod32768 onlynondualpolltick;bit0partial preserved.202:1021slots13CRC4IMU, cleanuptrue, onlyHMDposes.19tests pass. See docs/slot-capture202.md. OrdinalnotRFclock;ratevariablevs201.

## K03336
Source `D0398`, original line2527, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Sensor correction203
Size6 = magnetometeronly,notgyro/accelIMU.195/202 both0gyroframes;20122gyro+1mag.2031020slots44CRC23uniquegyroframes;cleanuptrue;onlyHMDposes. Countermod5notuniquechannelrule. Decoderseparatesstats,20tests pass. See docs/sensor-channel203.md.

## K03337
Source `D0398`, original line2531, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Active direct probe204
UserconfirmedPICOoff. --poll-active-mask explicittag;40opcode0polls on2/6/8/10/12;40TXEND,0RX,323slots,cleanuptrue. ActiveTX timer100usarmedafterTX andclamped1800 toavoidpastcompare;abortifremaining<300us. Module1775B. OnlyHMDpose,notcontrollercoexistence. Nextbootstrap/fallbackexchange;no directconnectionclaim. See docs/direct-probe204.md.

## K03338
Source `D0398`, original line2535, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Bootstrap active205
--poll-bootstrap taggedROYSWIFTPOLLBOOT1 restrictedBASE552C6A1E/prefixC0 andno-op; channels1/37/77.205322slots0RX,cleanuptrue,onlyHMDposes. Trackerpower/LEDconfirmationpendingafterPICOoff;no unchangedrepeat. Nextactualnegotiationframeconstruction. See docs/bootstrap205.md.

## K03339
Source `D0398`, original line2539, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Identity gate206
Userfirstpairmode/secondsearch. Taggedlength9probe206318slots0RXcleanuptrue. S1bit3gate5B84 decidesupstreamwhitelist/FICRfilter;currentpollbit3clear. Length9notguaranteed68F0delivery. Identitypayload+5 correct(record+0xA);nofakeoffsetfix. NextS1producerandturnaroundcheck. See docs/identity-gate206.md.

## K03340
Source `D0398`, original line2543, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Fast turnaround207
ROYSWIFTPOLLFAST1 explicitidentityonly; hardwareTX->RXSHORTS11/fastRU1,confirmed14handover207. Onebootstra[TRACKER_SERIAL],S1=2,slot10/359us. SuffixmatchescandidatefirstID butsemanticFICR/causalitynotyetproven.208passive14slots0RX,209active12slots0RX;exposurelimitedBLOCKED,allcleanuptrue. See docs/turnaround207-209.md. No completedconnection/6DoFclaim.

## K03341
Source `D0398`, original line2547, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Full firstID210
Radio207suffix[TRACKER_ID] confirmedFICR bySwift63B4RAMef6+8=efe,initialized66C0->7D18. FirstLEID[TRACKER_ID];seed74735415->BASE[PRIVATE_LINK_VALUE] matcheslive. OtherIDsunknown. Strict12bytebootstrapdecoder,22tests pass. See docs/first-id210.md. Nextstationresponse withnoNVwrites,notcompleteconnection.

## K03342
Source `D0398`, original line2550, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Negotiation persistence audit212
Read docs/negotiation-persistence212.md. Unpaired connection605C sets RAM E49; scheduler6CF6 dispatches context1050+20=9DDC, which writes AAAA/slot/peer seed record via writer9D7C and storage1EFE. Concrete backend pending, but negotiation is not proven RAM-only. No hardware mutation212. Existing paired marker skips this flag; audit that reconnection path before sending17/21byte reply. FirstID[TRACKER_ID] confirmed; no standalone6DoF claim.

## K03343
Source `D0398`, original line2553, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Reconnect candidate213
See docs/reconnect-candidate213.md. Offline targeted21-byte builder validates seed/ID/mask/byte inputs; fields exact68F0 arithmetic. AAAA branch skips seed/prefix replacement, not proof of globally flash-free execution.6008 setsE4D;6020 testsF02.9focusedtests pass,nohardwareTX213. Audit writer callers before deployment.

## K03344
Source `D0398`, original line2556, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Storage reachability214
See docs/storage-reachability214.md. FrameSSH/agentlive,noactivecapture/loader. Newexternaltest_slot0 report copied, not launchedhere.9E24 loads8-bytejournal;9E64 rollsstoragewhen cursor>=boundary, canresavepairedrecord. KnowncallerBC8Cstartup, notproofeachreconnectwrites. Candidatehalfwordrefsnotexhaustivecontrolflowproof. NohardwareTX214. Auditcurrentbondstate before negotiation.

## K03345
Source `D0398`, original line2559, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Bond observability215
Read docs/bond-observability215.md. Internal63B4record+1 equals paired?1:0 andqueueusespipeindex;payloadexcludesit. Reply5D8E/5F28TXADDRESS=RXMATCH, S1separate. Bootstrap207doesnotestablishbondstate. NoTX215. Needpaired-onlygate before21Bnegotiation,nobondclaimfromS1/1F.

## K03346
Source `D0398`, original line2562, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Paired-only gate216
See docs/paired-only-gate216.md. IR8D88caninvoke605CifnegotiationE10+38set, notunconditionallytimersonly. Candidate21BwithdeliberatemismatchingID+S1bit3clear rejectsunpairedat5C2E; activepairedE10[0]!=0bypassesIDcheck. Useindividualworkingaddress;silenceinconclusive. NoTX216, gate/downstreampersistenceauditpending.

## K03347
Source `D0398`, original line2565, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Paired gate draft217
Compile-onlyROYPAIREDGATEDRAFT1 generates21Bmismatch3EED1978,S1bit3clear.1915Bfits2045. Loaderrejectsbeforehardware regardlessflags;rejectionverified.9focusedtests pass,noTX217. See docs/reconnect-module217.md; auditpending,notconnectionclaim.

## K03348
Source `D0398`, original line2568, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Fast working218
See docs/fast-working218.md.1822Bmodule,322slots40TX40fasthandover0RX onfullworkingaddress/channel1. Allcleanuptrueflashfalse;onlyHMD72poses. LoaderfastRXallowsreviewedworkingno-op;draft217stillblocked. UserLED/controllerstatequestionpending,no unchangedrepeat. No standaloneconnectionclaim.

## K03349
Source `D0398`, original line2571, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## User LED bootstrap219
Userfirstbluefaston/off confirmsindicatoractive,notbondenum. Fastbootstrap1/37/77identity219:320slots0records,cleanuptrue,onlyHMD73poses. No unchangedrepeat. See docs/bootstrap-led219.md. Bothcontrollersnotconfirmed.

## K03350
Source `D0398`, original line2574, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Additional ID220
Laterexternalremote219snapshot differsfromownlocal219run.5CRCframes,bootstrap+4identitybothFICR[TRACKER_ID]/LE01A21DAD,lowmatcheshistoricalthirdSN064G.13Bresponsepathrequiresunpairedmarkerat68F0. NonewTX220,noshutdowncausalityclaim. Rawpreservedseparately,see docs/additional-id220.md. First[TRACKER_ID],third01A21DAD,secondunknown.

## K03351
Source `D0398`, original line2577, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Tracker pairing persistence authorized221
User explicitly permits ordinary pairing-record writes in PICO tracker flash (answer to requestSZ0426VEQ0Q6M7T7hmupCVTy). Firmware images on tracker and Frame remain unchanged; no Frame flash writes. Prior212-217all-flash interpretation superseded only for standard tracker pairing records. Need validate protocol framing and exact targeted identity before deployment. ThirdFICR[TRACKER_ID] confirmedtwo producerlayouts, derivedhistoricalBASE[PRIVATE_LINK_VALUE]/prefix40; not currentbondproof. First[TRACKER_ID], secondunknown.

## K03352
Source `D0398`, original line2580, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Pairing authorization and IR framing221
Userallowsordinarytrackerpairrecordwrites,notfirmware/Frameflash. Decoderconfirmedthird01A21DAD andhistoricalBASE[PRIVATE_LINK_VALUE]. NewcommandtableauditcorrectsIRopcode: Swift8D88 is entry9 (901C+9C pointer); header[seq,9,6],body[mode,0,FICR4]. It is not command0x1A in this stream. Entry0x15 binds8D40. PreviousGemini0x1AclaimsareunverifiedandmustnotdriveTX. build_ir_enable_command addedexplicitseq; delivery/countervalidationpending.

## K03353
Source `D0398`, original line2583, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Targeted station offer223
Userpairrecordwritesauthorized.2009Bmodule21Btarget[TRACKER_ID]/bootstrap,max40,explicittagflagrequired.13sampledslots0RX13hostretries,allcleanuptrue,onlyHMD134poses. NoIR/workinghandoveryet,no confirmedconnection.25loader+13framingtestspass. See docs/station-offer223.md.

## K03354
Source `D0398`, original line2586, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Post-offer224
ConfirmedderivedthirdBASE[PRIVATE_LINK_VALUE]/prefix40 exactpassiveguardaddedchannels2/6/8/10/12. Channel2passive3s12slots0RX12retries,cleanuptrue,onlyHMD69poses,noTX.17loadertestspass. Needonesessionbootstrap->workinghandover,notseparatesparsecapture. See docs/post-offer224.md.

## K03355
Source `D0398`, original line2589, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Handover draft225
First12bootstrapoffer->passiveworkingmask implementedoneSRAMsession. Initial2044Bfitsby1byte; finaltagbuildrecheckpending. ROYSTATIONNEXT1 explicit--station-handover+offer lifecycle, redundanttagselided. Nohardware225; diagnosticsphasemaxpending. See docs/station-handover225.md.

## K03356
Source `D0398`, original line2592, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Full-ID evidence document
Read docs/full-id-acquisition.md before claimingIDretrieval. Exact207first and laterexternal219third, hashes/offsets/producer proof preserved. Secondunknown;notCRC-derivedIDs; do not confuse13Boffset5with12Boffset8.

## K03357
Source `D0398`, original line2595, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Handover226
2043Bfinalmodule;25tests/explicitguardrejectionpass. Correctedwrapperhardware:12TX12handover,323slots0RX,nohostretry,allcleanuptrue,onlyHMD74poses. See docs/station-handover226.md. NoIR/no connectionclaim. Full-IDacquisition document synchronizedbothresearchartifacts.

## K03358
Source `D0398`, original line2598, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## External protocol228 corrections
Read docs/external-protocol-review228.md/userpico_tracker_packets_en.md. Appliedoffermaxcmd7A,wire slot2(not40),singlechannelmask01. Swift189prefixchain51EC RBIT+REV proves correction. Command9confirmrename,normalrecordsum8builderadded20testspass. New compiledmodulepending,noTX228. Needcommands17/19/25/10 andACK22, customS1/cadence. Gyroscale/36,42samples/timerwrap/bodyocclusionremainunverified. ExternalauthorHardware!=ourRoyproof.

## K03359
Source `D0398`, original line2601, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

## Direct standalone connection240
Read docs/direct-connection240.md. USB-powered physical tracker answered FIRST ID[TRACKER_ID], contrary to requested third-slot identification. Targetedfirst probe240 produced90working sum8-valid replies,86gyro+accel atBASE[PRIVATE_LINK_VALUE]/prefix0/frequency2 withPICOreportedoff. Allcleanuptrue. Boundedone-sessionconnectionproven; continuousreconnect/all3/coexistence/6DoFunfinished. Saved-address238 executedAFTER240 got0replies.45focusedtestspass. NeverinferconnectionfromLEDorhistoricalphysicalslot; use receivedID.

## K03360
Source `D0398`, original line2635, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

### Registry update after service250
Service now reads radio-peers.json rather than hardcoded IDs. A verified-format
bootstrap identity can fill a free registry slot, then requests a clean module
close/reload through Restart=on-success. Existing three entries are never silently
replaced. This generic enrollment path has four host unit tests but has NOT been
verified end-to-end on a new physical tracker. The current Frame registry contains
our three measured IDs. Ordinary power-on reconnect is still awaiting a live test.
Frame UI overlay registration1200x700 was verified after installation. Battery
remains null/unknown, no unverified old ESB battery layout is used.
