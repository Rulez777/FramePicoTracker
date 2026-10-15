# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K03521
Source `D0480`, original line835, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Evidence: radio-reference46.json; ble-reference47..49.*, swift-address50.*,
ble-raw51.*; outputs/roy-ble-reference-evidence.json and
roy-reference-test-summary.json. Scripts: work/read_roy_radio.py,
verify_ble_reference.py. Project RAM source updated, SDK untouched.

## K03522
Source `D0480`, original line843, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Read-only native helper pc/radio/pico_channel_read.c loads researched
libstationclient.pxr.so, creates a StationClient and starts normal Binder
binding. No SetBLEChannelConfig, pairing, OTA or reset calls are made.
Disassembly VA386d8 verifies GetBLEChannelConfig(handle,out) copies56 bytes
on success (three16-byte chunks and8 bytes), unlike guessed layouts.
GetStationChipid VA35a98 copies exactly4 bytes on success.

## K03523
Source `D0480`, original line850, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Attempt52 while PICO Asleep: channel getter returned-1. ADB keyevent224
woke the OS; attempt53 with Awake again returned-1. Control54 in the SAME
client process returned GetStationChipid=0, bytes[PRIVATE_LINK_VALUE], followed by
GetBLEChannelConfig=-1. Binder binding therefore works for the control;
channel data were not exposed by this request. This does not prove a
permissions failure or that all possible diagnostic paths are unavailable.
No channel output is decoded on failure; zeroed buffers are not radio config.

## K03524
Source `D0480`, original line858, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Recent Swift app logs still report three bound/connected records, addr='1233'.
That field is not a verified radio address or FICR. Stationchipid remains only
a candidate peer seed in the derived Swift address formula; local tracker
FICR[1] is still unknown. Do not manufacture it from serials or cached status.

## K03525
Source `D0480`, original line863, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

tracker_test --help did not display help: it ran its default diagnostic path
and printed GetTrackerBatteryStatus/no pairing. No further speculative
invocation of this utility; rely on inspected explicit getter helpers.
Targeted station/Swift private-path listings produced no readable pairing
file. No root escalation, private-data access bypass or firmware changes.

## K03526
Source `D0480`, original line869, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Known-address BLE reference result remains valid; no additional Roy RF test
in this follow-up. Next address-discovery evidence must come from a captured
Swift identity/negotiation exchange or a verified readable pairing record.
Current getters have not established tracker FICR, paired BASE1/prefix or
actual channel mask. Standalone tracker poses remain unimplemented.

## K03527
Source `D0480`, original line875, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Evidence: work/frame-stock/pico-channel52.txt,
pico-channel-awake53.txt, pico-channel-control54.txt; native helper source.

## K03528
Source `D0480`, original line881, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

No tracker restart or pairing reset in this series. Tested whether the
experimental BALEN1 pseudoaddress method captures a known BLE reference.
MODE3/frequency26,PCNF0=0,PCNF1=01012828,BASE0=0,PREFIX55 orAA,
CRC disabled,no whitening. This remains outside documented address range.

## K03529
Source `D0480`, original line886, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Test55 PREFIX55 granted only1 slot and then BLOCKED,0 records: inconclusive.
Test56 PREFIXAA granted320 slots,4 raw records,3 completed;884 offline
CRC24 hypotheses after software dewhitening yielded0 matches.
Control57 also granted only1 slot: it cannot establish absent RF traffic.
The user then authorized proceeding after the request to wake controllers.

## K03530
Source `D0480`, original line892, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Controlled sequence58/59/60 with recurring slots:

## K03531
Source `D0480`, original line894, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

|Test|Receiver|Slots|Raw/saved records|END|Hardware-CRC-valid frames|
|---|---|---:|---:|---:|---:|
|58|known advertising address|320|2|4|2|
|59|pseudoaddress AA|319|12|7|not applicable,CRC off|
|60|known advertising address|323|3|3|3|

## K03532
Source `D0480`, original line900, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Test59 offline search tried both per-byte bit orders, all viable offsets,
BLE whitening channel38 and CRC24.3243 eligible CRC comparisons:0 matches.
Before/after positive controls show BLE traffic was available in this
sequence. This does not prove pseudoaddress capture can never work; it
does mean it did not establish a valid BLE frame in the reviewed buffers.
Do not use its raw records to recover Swift pairing or claim promiscuous RX.

## K03533
Source `D0480`, original line907, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Independent decoder validation61: known address but hardware whitening OFF,
PCNF0=0,PCNF1=00032828,CRC off,MODE3/frequency26,1000us/4s.
11 raw records. Software LFSR initialized(channel38|40hex), feedback44hex,
LSB-first byte order, offset0 reconstructed TWO CRC24-valid PDUs. One PDU
matches the exact advertising data from hardware-validated reference48.
Thus software dewhitening/CRC is validated for this known-address BLE
capture, independently of pseudopreamble reception. Search result includes
3483 hypotheses; exact reference equality supports attribution to the
reference advertiser rather than a lone coincidental CRC match.

## K03534
Source `D0480`, original line917, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

This is BLE1M control, not proof of Swift MODE4 capture. Swift's no-whitening,
address/header and CRC16 still require a captured attributed golden frame.
The BLE result explains why repeated arbitrary raw buffers cannot simply
be reinterpreted as tracker telemetry. Next work must establish an actual
Swift address or validate a different unknown-address acquisition method.

## K03535
Source `D0480`, original line923, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

All tests55..61 report session/RAM/MPU restoration. No OpenOCD process after
cleanup. Concurrent pose rows primarily HMD/Right; Left continuity remains
unproven. No firmware,IRQ,TX or pairing changes.57 offline tests pass.
Loader explicitly whitelists these bounded BLE reference profiles; no
unrestricted radio configuration is exposed. SDK unchanged.

## K03536
Source `D0480`, original line929, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Evidence: ble-preamble55/56/59.*,ble-control57/58/60.*,ble-whitened61.*;
outputs/ble-preamble59-decoded.json,ble-whitened61-decoded.json,
roy-preamble-control-evidence.json. Script work/decode_ble_preamble.py.

## K03537
Source `D0480`, original line936, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

User supplied corrected ESB offsets; inspected actual station firmware offline.
SHA2564f376c52335a1c9e784093d4a8a40fba53081d08e1c871ccada8889e76e0f996.
PhoenixSta is a raw image loaded at1000, not0000. VectorSP20020000,
reset1375 maps to file374, where a valid reset trampoline branches to1370.
Absolute data pointers also confirm the mapping. Earlier station disassembly
labels (including our Gemini review) used file offsets as addresses; add1000
to those code labels. PC-relative instruction relationships remain valid,
but absolute pointer dereferences must subtract1000. Swift labels already
used its known LOAD_BASE1000. Keep file offsets and VAs explicit.

## K03538
Source `D0480`, original line946, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Station ESB packet setup file924c /VAa24c writes PCNF0 literal40008 from
file9270, and PCNF1.ENDIAN bit24 plus address-length-minus1. Default ESB
addresses at file9e8c /VAae8c are real initialization values, not established
active tracker addresses. Base setters filea250/a284 (VAb250/b284) read
four API bytes, cache them and RBIT into RADIO.BASE0/BASE1. Prefix setter
filea31c /VAb31c RBIT+REVs prefix words before hardware register writes.

## K03539
Source `D0480`, original line953, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Actual application override: call filed51e /VAe51e invokes BASE0 setter.
Its input is ROM VA1bfd4, whose bytesaa345678 yield API word785634aa,
RBIT->RADIO.BASE0=552c6a1e. This MATCHES the previously researched Swift
application override, and supersedes an assumption that defaultE7 base
alone is enough to receive current trackers. A different application
initialization path filee33e /VAf33e passes ROM VA1bfcc, bytes12345678
(API78563412); these are distinct modes/paths, not one universal address.

## K03540
Source `D0480`, original line961, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Following the filed51e call, filed528 invokes prefix setter with API byte
from RAM20004942 and pipe index0. That dynamic byte is not a known current
prefix. Other paths select BASE1 and per-pipe prefixes. Trace initialization
and state transitions of that RAM byte before assuming a live prefix.
Current negotiated channel mask and local tracker FICR still unknown.

## K03541
Source `D0480`, original line967, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

DTM strings user cited are file offsets1a960/1a99c, VAs1b960/1b99c;
bytes match DTM op test!/DTM recv test!. RF-test setup file13144 /VA14144
has the known test syncword. It is not substituted into the Swift RX profile.

## K03542
Source `D0480`, original line971, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

This step strengthens the ESB association, shared framing and application
base evidence, without receiving an attributed Swift frame. It does not
prove standard unmodified ESB wire format;8-bit LENGTH/4-bit S1 are the
researched custom framing. CRC/header facts already verified for Swift
remain the candidate profile. Generic base defaults are not live pairing IDs.

## K03543
Source `D0480`, original line977, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

The four float loads in the SLPI parser remain confirmed consumer offsets;
producer/transport trace is still needed before calling them tracker-emitted
air quaternions or omitting orientation fusion. Even orientation alone is
not absolute6DoF position. No new RF/TX/pairing/firmware changes this step.

## K03544
Source `D0480`, original line982, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Next: follow station RAM20004942 and BASE1 selection to pairing state and
the negotiated-prefix producer, then compare both directions with Swift.
Evidence script work/inspect_phoenix_esb.py produces
outputs/phoenix-esb-evidence.json with verified vector/literal anchors.

## K03545
Source `D0480`, original line990, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Continued offline trace; no hardware writes, TX or pairing actions.
Phoenix VAs below use corrected LOAD_BASE1000.

## K03546
Source `D0480`, original line993, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

RAM20004942 has two directly found byte writers:
- VAe226 (filed226): controller-side alternation produces API1/2.
- VAe422 (filed422): explicit movs r2,#3; strb r2,[r3]. Literal atVAe5b0
  points to20004942. The surrounding path selects records starting at
  index2, distinct from the controller-side selection. This supports the
  Swift/tracker role, not an arbitrary prefix guess.

## K03547
Source `D0480`, original line1000, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

VAe51e subsequently sets BASE0 using API785634aa from ROM1bfd4;
VAe524 loads the prefix byte through20004942 and VAe528 passes it with
pipe index0 to VAb31c. Setter RBIT+REV transforms API3 to registerC0.
Swift VA7fa4 is literally movs r0,#3; bx lr and its researched initialization
uses the same prefix/base. Therefore the common startup profile is now
confirmed bilaterally at application level: BASE0=552c6a1e/PREFIX0=C0,
PCNF0=40008, big endian,5-byte address,CRC16FFFF/poly11021,no whitening.
This is the SAME candidate profile already used in RX tests, not a newly
discovered address that makes old unsuccessful captures successful.

## K03548
Source `D0480`, original line1010, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Paired path VAe5c8 sets logical pipe1 prefix from the current selected role.
VAe5d4 fetches a per-device record through a pointer table. VAe5da..e5f0
reads four bytes from record+8, adds four bytes from the station-seed source,
and STRB stores each sum. This is modulo256 per byte without cross-byte
carry. VAe5f4 passes resulting buffer to VAb284 BASE1 setter (RBIT).
This independently matches Swift's previously researched BASE1 formula.
It does not expose actual selected record bytes through Android APIs.

## K03549
Source `D0480`, original line1018, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

The lookup/register pipeline is thus understood better, but actual current
tracker ID/FICR, negotiated seed/prefix and channel mask are still unknown.
Do not infer them from registered serial or the1233 marker. Startup role3
does not imply paired traffic remains on BASE0/C0. That distinction explains
why merely repeating the same startup RX scan is not a useful next test.

## K03550
Source `D0480`, original line1024, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Candidate-write search is conservative local literal propagation, not a
complete CFG proof. Two reported writes were manually checked in disassembly;
unreported indirect/bulk writes may exist. Script:
work/find_phoenix_prefix_writes.py. Machine evidence updated in
outputs/phoenix-esb-evidence.json. No project runtime code changed this step.

## K03551
Source `D0480`, original line1030, SHA256 `ac99c13f645d21942873f777fc1767d4da19e7ff5657c4be1e99f12c01513eb8`. Status: historical/unadjudicated narrative; English source prose.

Next: trace source of record+8 back through identity response and saved
pairing table, and inspect how the station exposes that data to AP commands.
Any diagnostic read must be verified by ABI; no pairing reset or guessed TX.
Standalone attributed Swift packets and tracker poses remain absent.

## K03552
Source `D0481`, original line1, SHA256 `f1efab8b2358e5b7d5f8812f0921fff736ccdb5adf4145a5a106514c55768b87`. Status: historical/unadjudicated narrative; English source prose.

# IR completion and paired-only gate216

## K03553
Source `D0481`, original line3, SHA256 `f1efab8b2358e5b7d5f8812f0921fff736ccdb5adf4145a5a106514c55768b87`. Status: historical/unadjudicated narrative; English source prose.

Read-only Swift8D88 trace: correct targeted IR mode calls78F4/7C44, but then examines volatile contextE10+38. If set by prior negotiation6996, it clears+38/+37 and registers callback605C through15D4; error fallback also branches605C. Thus IR enable is not unconditionally isolated from pairing persistence. Earlier claim that only timers/GPIO matter would be incomplete.

## K03554
Source `D0481`, original line5, SHA256 `f1efab8b2358e5b7d5f8812f0921fff736ccdb5adf4145a5a106514c55768b87`. Status: historical/unadjudicated narrative; English source prose.

Potential paired-only negotiation gate: 5C1C checks markerAAAA first. Unpaired path always checks payload+17 FICR on length21 before entering5B8E; mismatch exits/requeues before68F0. Paired path with runtime E10[0]!=0 jumps5B8E without the ID check; when E10[0]==0 it may still require FICR match and therefore reject mismatch. A deliberately mismatching final ID can distinguish an already-active paired receiver, but silence is inconclusive. This is NOT the targeted candidate213 and must not be confused with it.

## K03555
Source `D0481`, original line7, SHA256 `f1efab8b2358e5b7d5f8812f0921fff736ccdb5adf4145a5a106514c55768b87`. Status: historical/unadjudicated narrative; English source prose.

Conditions before deployment: S1 bit3 must remain clear (bit3 set bypasses this guard); use confirmed individual working full address rather than shared bootstrap; confirm mismatch against known receiver FICR; trace downstream callback/persistence conditions and other receivers sharing the address. No such packet transmitted216. Normal targeted21-byte packet remains unsafe for an unpaired tracker under no-flash scope. Evidence swift-ir-runtime216.txt and swift-s1gate206.txt. Goal incomplete.

## K03556
Source `D0482`, original line1, SHA256 `9f181d52f838799f7962c8ff77b7a434dc3a696741fba77225fc9e947d25f1ee`. Status: historical/unadjudicated narrative; English source prose.

# Pairing authorization and command framing221

## K03557
Source `D0482`, original line3, SHA256 `9f181d52f838799f7962c8ff77b7a434dc3a696741fba77225fc9e947d25f1ee`. Status: historical/unadjudicated narrative; English source prose.

User explicitly authorized ordinary tracker flash pairing records. Frame flash and both firmware images remain unchanged. Existing draft217 stays blocked until a distinct reviewed station exchange replaces it.

## K03558
Source `D0482`, original line5, SHA256 `9f181d52f838799f7962c8ff77b7a434dc3a696741fba77225fc9e947d25f1ee`. Status: historical/unadjudicated narrative; English source prose.

Confirmed additional FICR[TRACKER_ID] from two producers and froze source SHA256 in outputs/tracker-id221.json. Historical seed74735415 givesBASE[PRIVATE_LINK_VALUE] and slot2 prefix40; this is not a present pairing claim.

## K03559
Source `D0482`, original line7, SHA256 `9f181d52f838799f7962c8ff77b7a434dc3a696741fba77225fc9e947d25f1ee`. Status: historical/unadjudicated narrative; English source prose.

Corrected a protocol-number ambiguity: command parser9500 selects table index from header+1, length from+2 and body at+3. Table901C writes8D89 to context+9C, so index9 calls8D88. Its body is mode1/2,reserved byte,4-byteFICR. Index0x15 instead binds8D40 at+15C. Prior0x1A IR assertion must not be used for this stream. Added explicit-sequence build_ir_enable_command; six-bit counter acceptance and radio framing remain to verify before TX.13focused IR/negotiation tests pass. No new radio transmission221.

## K03560
Source `D0483`, original line103, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

## Stored Tracker ID Record Architecture and Diagnostic Readability, 2026-10-06

## K03561
Source `D0483`, original line105, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Offline disassembly of PhoenixSta (LOAD_BASE 0x1000) and live headset diagnostic execution on [HEADSET_ADB_SERIAL]:

## K03562
Source `D0483`, original line119, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

2. In-Memory Active Pairing Table (RAM 0x20008EC0):
- Table of 5 tracker slots, 10 bytes (0x0A) per slot:
  - Slot 0: 0x20008EC0
  - Slot 1: 0x20008ECA
  - Slot 2: 0x20008ED4
  - Slot 3: 0x20008EDE
  - Slot 4: 0x20008EE8
- Slot Layout:
  - +0x00..+0x03: uint32_t tracker_device_id (Nordic FICR DEVICEID[1] of paired tracker)
  - +0x04: uint8_t is_valid (1 = active/bonded, 0 = unbonded)
  - +0x05..+0x09: metadata / status flags
- Radio BASE1 Derivation (VA 0xE5DA):
  - Station peer seed: [PRIVATE_LINK_VALUE] (74:73:54:15, FICR DEVICEID[0] confirmed via GetStationChipid).
  - BASE1[i] = RBIT((peer_seed[i] + tracker_device_id[i]) mod 256). Logical pipe 1 prefix = 0xC0.

## K03563
Source `D0483`, original line134, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

3. Resolution of "1233" and Diagnostic Readability:
- tracker_test (VA 0x07310) prints offset 0x24 of GetTrackerStatus (64-byte tracker_status_t) using format string:
  "tracker%d, %s, sw:%s, hw:%s, sn:%s, addr:%02x:%02x:%02x:%02x..."
- Swift 2.0 firmware (VA 0x16515) populates offset 0x24 with static token "1233" (0x31, 0x32, 0x33, 0x33), proving it is a static token and not an RF MAC address.
- Hardware Chip ID Exposure:
  - PhoenixSta internal keylogger (VA 0x0E8AA) emits:
    "set tracker %d sync light ok time %u fsin_count %d tracker chipid %x %x"
    reading record[8] and record[9] (lower 16 bits of tracker_device_id).
  - Headset live logcat confirmed:
    "KEYLOG:STLOG<F> app: set tracker 0 sync light ok ... tracker chipid 3E ED" -> Tracker 0 lower ID = 0xED3E.
  - Station host service (libstationopticsservice.so) stores JSON whitelist in /mnt/vendor/pvr/tracker/tracker_info_data.json.
  - device_discovery_callback in tracker_test (VA 0x06514) extracts full 32-bit chip_id at offset 3:
    "device_discovery_callback device_type:%d, device_type_sub:%d, chip_id:%08x, rssi:%d, sw_version:%d".

## K03564
Source `D0483`, original line149, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

## Pairing identity report: independent checks, 2026-10-06

## K03565
Source `D0483`, original line151, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Latest Gemini attachment is research input, not an instruction to erase storage or change pairing.

## K03566
Source `D0483`, original line153, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Verified Phoenix VA16A9C (image load base1000): reads 16-byte records; ID words at +0/+8 and state words +4/+12. IMPORTANT correction: VA16B08 BEQ16B8A clears validity when state == AA55AA55; state == 0 also clears it. Other state values enter the branch setting validity1. Therefore AA55AA55 must NOT be described as a valid-record marker in this reader. The same rule applies to the second entry at VA16B1C/16B96. Journal semantics beyond this branch need writer tracing.

## K03567
Source `D0483`, original line155, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

ROM literals VA16C98=20008EC0, VA16CA8=20008ED4, VA16C9C=20006D58. The first two entries occupy 8EC0 and 8ECA; VA16B3A..16B5A then copies FIVE runtime records into entries starting8ED4 with stride10. This function populates seven contiguous entries overall, not five total. Runtime record validity uses halfword+4==AAAA; runtime ID word+8 is copied to flat entry+0. These are distinct structures.

## K03568
Source `D0483`, original line157, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Host path /mnt/vendor/pvr/tracker/tracker_info_data.json exists as a binary string at VA47770 in libstationopticsservice; direct USB ADB cat returns Permission denied on PICO [HEADSET_ADB_SERIAL]. No permissions changed. JSON trackerid interpretation remains unverified; neither path nor field name proves it stores FICR DEVICEID.

## K03569
Source `D0483`, original line159, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

StationClient GetWhiteListNum wrapper VA33498, method3786C copies ONLY one count to caller, not an ID array. Service VA78740 issues a station command and returns a count from service+1922. _GetTrackerWhiteList VA66F54 issues command30h; it does not directly read the JSON at this location. The JSON reference belongs to other functions around VA9CB5C. Avoid conflating these paths.

## K03570
Source `D0483`, original line161, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Added pc/radio/pico_identity_read.c: verified local listener stores callback at client+118 (VA3D804), delivery passes a 19-byte struct indirectly under AAPCS64 (VA36B58..36B74). chip_id u32+3, type u16+0, sub u8+2, RSSI i8+7, version u16+9 verified from tracker_test. Listener registration is process-local; helper does NOT call SetDevDiscoverySwitch or pairing setters. Built with NDK clang -Wall -Wextra -Werror. Live run62: GetWhiteListNum result-1/count-1; listener registration0; 20sec no discovery events; listener removed and process exited normally. Failed count is not zero paired trackers. Logcat last6000 lines gave no chip-ID/whitelist matches. Evidence: outputs/pico-identity-read62.txt.

## K03571
Source `D0483`, original line163, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

GetStationChipid bytes[PRIVATE_LINK_VALUE] mean little-endian integer15547374, not[PRIVATE_LINK_VALUE]. They are not yet proven to equal the live peer_seed field. Historical keylog3E ED reveals two bytes only, not a full address and not a current device mapping.

## K03572
Source `D0483`, original line165, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

No Roy/RADIO/SRAM/flash changes during this check. Full live tracker IDs and channel mask still unknown; no CRC-valid attributed Swift reception or standalone poses. Next implementation step: trace the station whitelist response handler and discovery-switch lifecycle to recover full identity safely; then use confirmed identity/mask for bounded Roy RX, before any TX/ACK work.

## K03573
Source `D0483`, original line168, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

## JSON metadata path closed by writer verification, 2026-10-06

## K03574
Source `D0483`, original line170, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Independent libstationopticsservice inspection confirms VA9CA40 masks logical tracker index to8bits, VA9CA48 converts it to a string and VA9CA84 passes it to TrackerInfoDataJson::write (VA9D440). Writer keeps this string in x21 and stores it under trackerid at VA9D9AC..9D9C8. This path does not use Nordic FICR as trackerid. VA9D9D4 loads event+37 and VA9D9D8 extracts bit6 for bond_status. Keys sn, soft_ver, trackerid and bond_status verified at VA4402C,498E6,4779F,45EDB. These findings close this JSON writer as a source of full radio identity.

## K03575
Source `D0483`, original line172, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

The three-record JSON supplied by Gemini is a reconstruction from schema and previously observed host statuses, NOT an exact file dump. Current saved record count/order, stale entries, serialization and current bond_status values remain unobserved. Do not label it exact on-disk content. Previously observed serials/version can support example values only. Permission denied is verified; attribution to particular DAC/SELinux rules and mount configuration has not been independently checked this turn. adb root production refusal concerns adbd build policy, not evidence that the kernel itself blocks that command.

## K03576
Source `D0483`, original line174, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

RF identity remains in station/runtime protocol paths. Retain prior corrections: AA55AA55 clears validity in the verified reader; two plus five entries are populated; seed74735415 bytes have LE word15547374 and live seed equivalence is not established. No new RF test, pairing or firmware modification performed. Next useful work remains full whitelist-response/discovery identity decoding rather than attempts to read this metadata JSON.

## K03577
Source `D0483`, original line177, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

## Bounded active discovery run63 and command lifecycle, 2026-10-06

## K03578
Source `D0483`, original line179, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Gemini proposed a dongle or flashing Roy. Both conflict with the user requirements and are not the selected implementation. Onboard Roy remains stock BLE firmware plus reversible SRAM/MPSL receiver; no SPI disable, flash, erase or firmware replacement.

## K03579
Source `D0483`, original line181, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Host ABI independently verified: StationService VA7BDAC/B0 serializes mode and tracker count into two bytes; command2C at7BDBC and mcuSend at7BDE4. TrackerClient C SetDevDiscoverySwitch forwards (handle,mode,count). Phoenix handler VA109A8 distinguishes modeFF readback at109BE; ordinary mode calls VA DDC4. DDC4 only updates RAM20000274 low two mode bits (swapped bit0/1) and count bits2..4 in the next byte; count outside1..5 falls back2. No flash/erase calls in this direct setter path. This verifies reversibility of the direct command, not every future scheduler side effect.

## K03580
Source `D0483`, original line183, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

pc/radio/pico_identity_read.c now accepts --discover; default remains passive. Active mode installs the verified19-byte callback before scan, requests mode1/count3, waits at most12sec if accepted, then sends mode0/count3 (up to3 retries), removes listener and exits. SIGINT/SIGTERM request early cleanup. It does not pair/unpair/reset/upgrade. NDK clang -Wall -Wextra -Werror build passed.

## K03581
Source `D0483`, original line185, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Live USB run63 on [HEADSET_ADB_SERIAL]: PICO Awake before and after. GetWhiteListNum result-1/count-1; local listener0; enable mode1/count3 result0; no discovery events12sec; disable mode0/count3 result0; normal exit. Logcat confirms DEVICE_DISCOVERY_SWITCH reply and station ap_spi_discovery_mode 00 03 00 after disable. dumpsys afterwards shows all3 known serials online1/bond_status1. Thus last passive run62 silence is not explained solely by not enabling discovery; enabling it also yielded no events while these trackers were already paired. Whether paired Swift devices emit beacons in their current state remains to be traced. No full ID recovered; do not generate a purported confirmed BASE1 from partial IDs or seed assumptions.

## K03582
Source `D0483`, original line187, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Next work: trace RAM20000274 flag consumers and Swift beacon/registration conditions, then obtain an attributable full ID and channel mask before target-address Roy RX. Whitelist0x30 two bytes per device is corroborated by host consumer, but its interpretation as low16 FICR still needs station response producer trace. No attributed CRC-valid Swift radio frame or standalone pose yet.

## K03583
Source `D0483`, original line190, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

## Swift full-ID response gate and root scope, 2026-10-06

## K03584
Source `D0483`, original line192, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Root on PICO is not currently required for the selected next step. Public host client binds, bounded discovery commands and offline firmware analysis work under ordinary ADB shell. Root could broaden diagnostics but would not by itself prove access to the station MCU's private RAM/flash; no rooting or bootloader changes requested or performed. The closed metadata JSON is not a reason to root.

## K03585
Source `D0483`, original line194, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Swift VA68F0 checks received length9; VA6904 reads state halfword+4, VA6906 loadsAAAA and VA690C branches directly to return when equal. Only the non-AAAA branch calls full-ID response builder6844 at690E. Builder VA6892/689E calls local FICR getter7D18 with output SP+0D, and VA68A4 sets response length13. Because response data is copied from SP+8, hardware ID occupies response payload+5..8, consistent with prior producer trace. This verifies a suppression gate for this request path, not universal absence of all advertising by paired trackers.

## K03586
Source `D0483`, original line196, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Station template builder VA BD20: flat record valid+4 at BD5C controls branch BD6C/70. Existing valid entries take length21 path BD74..BD9A; invalid entries enter BE06. Only that branch consults discovery mode bit1 at BE08/BE0A and conditionally sets template byte+2 bit1 at BE40..BE46 when flat record+5 is zero. Thus discovery mode does not simply override all valid paired records in this builder. This corroborates why mode1 scan63 may yield no IDs, but complete scheduler/air mapping remains under analysis.

## K03587
Source `D0483`, original line198, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Keep pairing intact. Next trace: other callers of6844, ordinary connected identity forwarding, and station runtime record+8 consumers; use any recovered full ID only with independently checked seed and channel mask. No hardware writes this turn; root not a prerequisite, full standalone reception remains unproven.

## K03588
Source `D0483`, original line201, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

## Continued identity trace and validated shortened-address reference, 2026-10-06

## K03589
Source `D0483`, original line203, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Linear Thumb call-index inspection found Swift full-ID builder6844 called at690E, FICR getter7D18 called at66C0 (cached RAM20000EFE) and689E (identity response). This is a linear-disassembly result, not proof that indirect calls cannot exist. Station CFA4 ordinary connection handler compares all four incoming ID bytes against runtime+8..11 and VA D01A/D01E stores incoming u32 into runtime+8. The following D062/1658C queues a station/tracker command, not proven host SPI identity exposure. Added event-builder caller index outputs/phoenix-spi-event-callers.json; full discovery CE58->F980 still distinguished from ordinary connection/status.

## K03590
Source `D0483`, original line205, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Station host GetTrackerInfoForAlg VA7D5F4..7D694 writes three 12-byte scalar sets at parcel offsets21F0/22BC/2388 (per-tracker stride204); client3C250..3C260 copies612 bytes regardless. No FICR transfer in this service method. Unwritten/pointer-looking tail from earlier buffers is NOT radio identity evidence.

## K03591
Source `D0483`, original line207, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Alternative RX calibration: nRF52833 PCNF1 BALEN2 is documented, address register base truncates from least significant byte, ADDRESS bit order alwaysLSB (independent of payload ENDIAN). Source https://docs.nordicsemi.com/r/bundle/ps_nrf52833/page/radio.html. We tested shortened address against known BLE advertiser access address, with CRC off and whitening off, using TIMER fallback and unchanged Roy code/IRQ.

## K03592
Source `D0483`, original line209, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

64 hypothesis BASE89BE0000/PREFIXD6, mode3/PCNF0=0/PCNF1=00022828/channel26/slot1000us/4sec:420slots,0ADDRESS. Control65 only1slot then scheduling blockage, so inconclusive RF negative control.
66 corrected byte-order hypothesis BASEBED60000/PREFIX89, same timing:422slots,18raw records,12hardwareEND. Each first DMA byte is remaining original address byte8E in checked records. Offline software dewhitening/CRC24 scan4106hypotheses finds9matches at LSB order/offset8; eight are repeated valid advertising PDUs, including exact golden48 PDU6022c6fd...543730305f3534 (CRC3551368). Ninth sequence17 header47 is a tentative short candidate, not counted as established advertising identity. Thus matching the initial THREE address bytes and receiving the leftover fourth address byte into DMA is demonstrated for this known-address BLE reference. This does not prove PICO reception or arbitrary suffix matching.
67 tail-address hypothesis BASE89BE0000/PREFIX8E:8slots then blocked,0records, inconclusive. No claim suffix matching works. Only two known ID bytes may map to address tail, so do not assume66 immediately recovers a paired Swift full ID.

## K03593
Source `D0483`, original line213, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

All64..67 cleanup verifies session close, RAM/MPU restoration and unloadedtrue; noflashwrites. HMD/Right pose rows only; Left continuity remains unproven. Loader permits only three exact BLE short probe tuples, hardware CRCoff/whiteningoff/mode3/channel2/26/80; negative tests prevent widening this whitelist accidentally. Capture kind distinguishes unvalidated shortened/preamble probes from ordinary BLE references.58 pytest checks pass. Source/harness changes copied to Frame; no Roy firmware/C module changes.

## K03594
Source `D0483`, original line215, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Next: determine which Swift address bits the historical16-bit ID constrains, obtain current ID mapping and peer seed evidence, and validate tail-match calibration with recurring MPSL slots before trying partial-identity Swift reception. Root still not a prerequisite; no pairing reset.

## K03595
Source `D0483`, original line218, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

## Tail-address validation and conditional Swift scan69..71, 2026-10-06

## K03596
Source `D0483`, original line220, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Phoenix initializer F342 calls94C0 into station context+6; F348 calls94C0 into RAM2000492E (literalF3D8/F3E4).94C0 reads FICR10000064 and stores all4bytes little-endian. Runtime E5D2/E5E4 reads seed2000492E, E5D8/E5EC writes sum into separate20004944, then E5F4 sets BASE1 from that sum. Seed is not overwritten by this sum. GetStationChipid historical[PRIVATE_LINK_VALUE] remains a presumed matching host representation until its response producer is fully checked.

## K03597
Source `D0483`, original line222, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Known low ID bytes3E ED plus candidate seed74 73 54 15 imply per-byte sums B2 60 and BASE register high16=4D06, maskFFFF0000. ADDRESS register byte order means these constrain the last2base bytes on air (06 4D), followed by prefixC0, while the leading2bytes remain unknown. Added pure partial_paired_address() to pc/radio/pico_swift_probe.py: clearly conditional, not live pairing/RX profile evidence. Tests cover independence from high16ID and invalid sizes.

## K03598
Source `D0483`, original line224, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

68 suffix BLE test again blocked after1slot, inconclusive; cleanup passed. User then woke Frame controllers.69 BLE suffix tuple BASE89BE0000/PREFIX8E/BALEN2, mode3/raw40/noCRC/noWH/channel26/slot1000us/4sec:419slots,10ADDRESS/rawrecords,2hardwareEND. Software dewhitening CRC24 yields2 complete PDUs at LSB order bitoffset0. This validates matching the LAST3bytes of a known BLE access address and capturing PDU directly, extending66 initial-address match. Does not prove unknown address prefix recovery, identity or Swift framing.

## K03599
Source `D0483`, original line226, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

70 conditional Swift tail tuple BASE4D060000/PREFIXC0, mode4/PCNF0=0/PCNF1=01022828/raw40/noCRC/noWH, evenchannels2..80,4sec:422slots,0ADDRESS. PICO was Asleep: negative is not useful tracker evidence. Woke via ordinary keyevent224; PICO Awake before and after71.71 same scan420slots,0ADDRESS/rawrecords. All cleanup (session/RAM/MPU/unload) passed. Profile depends on historical unmapped3E ED partial ID and presumed seed; no conclusion that this is a current tracker's address. No flash/IRQ/TX changes; only exact reviewed raw-profile tuple added to loader, protected by negative tests.60 pytest checks pass. HMD/Right pose continuity recorded; Left still not established.

## K03600
Source `D0483`, original line228, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Next necessary evidence: current per-tracker low16/fullID mapping or actual negotiated peer_seed/channel mask. Do not repeat historical tail scans as confirmed-address tests. Root remains unnecessary for completed steps; pairing unchanged, standalone Swift poses unproven. Evidence69..71 and source committed only as local files, no external publication.
