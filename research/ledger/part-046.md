# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K03601
Source `D0483`, original line231, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

## Gemini whitelist/event/three-ID review and live ext cache72, 2026-10-06

## K03602
Source `D0483`, original line233, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Attachment65752e37 is research input. Verified station whitelist producer: Gemini station labelsC0E4/C100/EBC4 are FILE OFFSETS, not VAs. Actual VA D0E4..D140: D104 reads runtime slot byte+0, D106 reads halfword+4; comparesAAAA by subtraction/CLZ, stores (slot+1,is_bonded). Three pairs follow leading1 in seven-byte payload; D13A callsFBC4 for command30/role1. No ID+8 reads. This closes0x30 as BOTH full32 and low16 identity source, correcting older claim it supplied low16. Gemini normal connection addresses14460/14508/E980 similarly need+1000:15460/15508/F980.

## K03603
Source `D0483`, original line235, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Host event17 parser6B610 does load four bytes from packet+4 and stores ext record+5; that does not establish FICR identity. Swift status producer8ACC writes ASCII1233 constant fromVA16515 and station15508 forwards payload bytes; producer association with event17 still requires complete path tracing. Do not label four-byte width as proof of hardware ID.

## K03604
Source `D0483`, original line237, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Verified cached getter service7F320 copies3x32bytes from host service cache; client3CF00 copies relevant fields into3x32 output. Added read-only pc/radio/pico_ext_identity_read.c. NDK -Wall -Wextra -Werror passes. Live72 GetStationChipid0 returns[PRIVATE_LINK_VALUE] bytes again. GetExtDevStatus0 returns96bytes: no nonzero identity/serial fields; second record type2, all three address fields+5 zero. Empty cached event records do not prove underlying MCU IDs unavailable or lost. No radio/discovery/pairing setters called. Evidence outputs/pico-ext-identity72.txt.

## K03605
Source `D0483`, original line239, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Gemini claims current log low16 IDs3EED/C3BF/AD1D tied to three serials. This attachment contains no raw timestamped captures; independent logcat in our recent passes did not yield these entries. Treat two new ID pairs and current slot mapping as reported, unverified inputs. Seed bytes[PRIVATE_LINK_VALUE] imply LE integer15547374; reversed bytes15547374 in Gemini calculation are unsupported by live output. Table also mixes untransformed sums with RBIT results.

## K03606
Source `D0483`, original line241, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Correct CONDITIONAL arithmetic using seed74735415:
- ID3EED: byte sumsB260; BASE known high16=4D06, tail register bytes064DC0.
- IDC3BF: byte sums3732; BASE known high16=EC4C, tail4CECC0.
- IDAD1D: byte sums2190; BASE known high16=8409, tail0984C0.
MaskFFFF0000; leading two address bytes unknown. These are offline constraints, not confirmed live radio profiles or air captures. Existing partial helper computes them reproducibly. Do not deploy Gemini13D80000/71C20000 table.

## K03607
Source `D0483`, original line247, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Global claim 'no diagnostic command can read table' is not independently established by reviewing a few dispatchers; requires complete call/indirect-handler audit. Root still not required for these getter checks. No Frame SRAM/MPU/RADIO changes this step; no firmware or pairing changes. Standalone Swift reception remains unproven.

## K03608
Source `D0483`, original line250, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

## Remaining reported tails73/74 and independent ID log confirmation75, 2026-10-06

## K03609
Source `D0483`, original line252, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Loader whitelist extended only to reviewed conditional BASE high16 tuples4D06/EC4C/8409, all other mode/framing/CRC/noWH/channel restrictions retained. Harness --swift-tail-index selects exact tuple.60 pytest checks pass; source/harness uploadedFrame. No Roy firmware/C-module/IRQ edits.

## K03610
Source `D0483`, original line254, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

PICO Awake before73 and after74, allthree trackers afterwards online1 and bonded1, serials110865G/110282G/111064G in slots1..3.73 BASEEC4C0000:419MPSLslots,0ADDRESS/raw.74 BASE84090000:421slots,0ADDRESS/raw. Both1000us slots,4sec, evenchannels2..80. All session/RAM/MPU cleanuptrue and unloaded.73 pose file hadHMDonly;74 HMD/Right. Neither proves Left continuity or uninterrupted controller availability. No reboot, flash, TX or pairing reset.

## K03611
Source `D0483`, original line256, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

Independent targeted logcat now confirms station lines:
set light_sync id0 ... trackerchipid3E ED;
set light_sync id1 ... trackerchipidC3 BF;
set light_sync id2 ... trackerchipidAD1D;
and matching set trackerN sync light ok lines. Timestamped original40lines saved outputs/pico-id-logs75.txt. This supersedes previous statement that new two pairs were only Gemini-reported. They are log-backed station index mappings; alignment with host serial ordering remains a mapping inference, not full32 ID extraction. Host log timestamps belong to device clock; client date is2026-10-06. Low16 does not prove upper16 or complete live radio configuration.

## K03612
Source `D0483`, original line262, SHA256 `6478b278c4fdc1d18d27eb09a3b83c74a463e391388c461ce61253ee72f65cab`. Status: historical/unadjudicated narrative; English source prose.

All3 conditional partial tails have now been tried (71/73/74) with zeroADDRESS, despite live log confirmation. This is a sampled negative, not proof trackers use a different protocol: negotiated peer seed/prefix, actual PHY/address handling, channel schedule and suffix detection under Swift2M are still uncertain. Do not keep repeating the same scan. Next isolate PHY and framing with known-address2M calibration or trace live negotiated seed/address profile; retain evidence that BLE1M suffix RX works, not claim Swift2M is calibrated.

## K03613
Source `D0485`, original line516, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; English source prose.

## Verified full hardware-ID acquisition (2026-10-06)

## K03614
Source `D0485`, original line518, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; English source prose.

See full-id-acquisition.md (original reference: `full-id-acquisition.md`) for exact raw bytes, producer offsets, FICR initialization, separate capture provenance and frozen SHA256. Confirmed IDs: first[TRACKER_ID]; third01A21DAD. Second fullID remainsunknown. These are received FICR bytes, not CRC inversion or speculative upper16 reconstruction. Bootstrap12B IDoffset8; identity13B IDoffset5. User authorizedordinarytrackerpairrecords221; no Frameflash/firmwarechanges.

## K03615
Source `D0485`, original line520, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; English source prose.

## External protocol review228
Reference pico_tracker_packets_en.md and docs/external-protocol-review228.md. Corrected raw slot2->register40 andmaxcommand7A; oldoffer40/00 incorrectlyencoded. Normalrecordsum8 added, command9 isconfirm ratherthanseparateIRmode25. Externalverifiedhandshakeincludesphase10+ACK22 toenable18BIMU. NoTX228,newbuildpending. Sensor gyro2xfactor,36/42sampledecoder,camera-phasesync andRoycadence/coexistence remainunfinished.

## K03616
Source `D0486`, original line1, SHA256 `7f09144bfa5eec76107bce1c268f6ddb985ba2877ca83560d638bc58e370b4a5`. Status: historical/unadjudicated narrative; English source prose.

# Post-offer working-address observation224

## K03617
Source `D0486`, original line3, SHA256 `7f09144bfa5eec76107bce1c268f6ddb985ba2877ca83560d638bc58e370b4a5`. Status: historical/unadjudicated narrative; English source prose.

Derived confirmed thirdFICR[TRACKER_ID]/historicalseed74735415 addressBASE[PRIVATE_LINK_VALUE],prefix40. Loader passive_profile allows only this exact new address/framing, CRC16, whitelistchannels2/6/8/10/12. Polltick path permits it onlypassively (rx-only,notSwiftPoll). Existingactiveguardsretainfirst/bootstrap restriction.17loader tests pass; tests do not prove radio transition.

## K03618
Source `D0486`, original line5, SHA256 `7f09144bfa5eec76107bce1c268f6ddb985ba2877ca83560d638bc58e370b4a5`. Status: historical/unadjudicated narrative; English source prose.

Fresh passivepriority0/native/reservedMPU/100usmodule tested3s atchannel2 after prioroffer223.12slots,0records,12hostretries,onlyHMD69poses. Cleanupsession/RAM/MPUalltrue. NoTX224. Late separatecapture andsparsegrantscannotproveofferaccepted/rejected. Next implement bootstrapoffer andworkingRX insideoneboundedSRAMsession; coverconfiguredmaskandobservepositiveworkingresponse beforeclaimingconnection. Do notrepeatsinglechannelnegative asproof.

## K03619
Source `D0487`, original line1, SHA256 `c2f108aa4de0cb67da42d0957006bc7c310068b9296ac77e739bbe1902bbb5e2`. Status: historical/unadjudicated narrative; English source prose.

# Protocol integration263

## K03620
Source `D0487`, original line3, SHA256 `c2f108aa4de0cb67da42d0957006bc7c310068b9296ac77e739bbe1902bbb5e2`. Status: historical/unadjudicated narrative; English source prose.

Read pico_tracker_packets_en.md in full, treat hardware/static/inferred labels
as external evidence, not proof of Roy implementation.

## K03621
Source `D0487`, original line6, SHA256 `c2f108aa4de0cb67da42d0957006bc7c310068b9296ac77e739bbe1902bbb5e2`. Status: historical/unadjudicated narrative; English source prose.

Remaining actual gaps:
- Command14 light schedule and periodically timed command10 missing in live Roy.
- Current configuration uses phase0; cannot claim camera-exposure synchronization.
- External gyro profile uses4000dps/32768 vs current2000dps/32768. Select by
  validated sensor/product info, not a global unverified replacement.
- IMU ring8, at most2samples/reply: full200Hz needs over100successfulpolls/peer/s.
- Camera frame->camera calibration mapping and timestamp alignment required.
- Factory LED geometry and per-peer constellation association required.
- World transform must use timestamped Frame pose, not camera-frame translation.

## K03622
Source `D0487`, original line16, SHA256 `c2f108aa4de0cb67da42d0957006bc7c310068b9296ac77e739bbe1902bbb5e2`. Status: historical/unadjudicated narrative; English source prose.

Implemented swift_light_sync.py builders for14/10, checksum and zero-divisor
guard; tests pass. frame_live_camera.py demultiplexes complete growingOCC records,
retains stream keys without guessedgeometry. Deployed onFrame.
Camera transport attempt263: command returnedok but no newdataset within6s,
STOP executed finally and returnedok. Recording allowed settingtrue. Current
device_monitor: HMDinvalid101, bothcontrollersdisconnected. User activation
requested. No RF263, no fakepose sent. Integrated6DoF remains incomplete.

## K03623
Source `D0488`, original line1, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

# Audit of quick-bohr evidence (investigation186, 2026-10-06)

## K03624
Source `D0488`, original line3, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

The supplied directory contains original firmware, ELF symbol tables, shared libraries and diagnostic sources. It is not a full source-level decompilation. Keep vendor files outside the code-only GitHub publication.

## K03625
Source `D0488`, original line5, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

SHA-256 matches existing research inputs: station firmware 4f376c52335a1c9e784093d4a8a40fba53081d08e1c871ccada8889e76e0f996; station service 2c9d4d9d4253b75b026473f06879c84d4122bced09bcbb05e7704fb4e3677afe.

## K03626
Source `D0488`, original line7, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

Verified SetTrackerConnectStatus VA 9c73c: 9c8d4 loads packet+5 as u32, 9c8dc stores it at spi_event_connect_t+0x1f. This proves a four-byte field is preserved, not its FICR semantics. Trace the station's packet construction before calling it a hardware ID.

## K03627
Source `D0488`, original line9, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

GetTrackerStatus service VA 7afdc copies 48-byte cached internal records into StationDataParcelable; the public client converts to three 64-byte records. A live read still returns ASCII 1233. A raw Binder-record path may expose the internal +0x1f field before this conversion and is the next focused inspection.

## K03628
Source `D0488`, original line11, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

Supplied dump_pico_tracker_id.cpp assumes three 32-byte ext-device records and ID at +5, does not validate the getter result and uses a pointer callback for a by-value struct API. Do not run it as a verified ID extractor. Existing bounded diagnostic has verified API binding and buffers.

## K03629
Source `D0488`, original line13, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

PICO root was reconfirmed, but reading /proc/982/maps for stationservice is Permission denied even under the existing pvrtrackingservice SELinux context. Root does not grant unrestricted process-memory access; no security settings were changed.

## K03630
Source `D0488`, original line15, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

The supplied IR opcode 0x1a conflicts with earlier verified Swift dispatch evidence (opcode09 for handler8d88); do not transmit it on this report alone. Stable 3x200 Hz standalone reception and identified optical constellation remain unproven.

## K03631
Source `D0488`, original line17, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

Publication is complete at https://github.com/Rulez777/Frame-Pico-Tracker : all 103 files in the downloaded GitHub ZIP match the sanitized local snapshot byte for byte. Offline snapshot tests: 116 passed, 1 skipped. Vendor dumps and device data are excluded.

## K03632
Source `D0488`, original line19, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

## Live raw Binder cache validation (187)

## K03633
Source `D0488`, original line21, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

A new read-only helper pc/radio/pico_raw_status_read.c calls the verified public getter to populate the local StationDataParcelable cache, then reads three 48-byte raw records at researched client VA 0x66778+0x2164 with stride48. The helper checks the researched export's first instruction before using build-specific addresses; use only the matching verified library. No remote service memory, pairing or radio settings are changed.

## K03634
Source `D0488`, original line23, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

Live outputs/pico-raw-status187.jsonl: all three internal fields +0x1f contain 31323333, exactly matching public addr. Thus the internal record does NOT reveal full FICR ID: the placeholder exists before client conversion. This closes the proposed raw-status ID route.

## K03635
Source `D0488`, original line25, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

Swift connect routine VA6844 was rechecked using split_container (321-byte container header, duplicated 96532-byte application, loadbase1000). It calls7d18 with sp+0xd, then copies sp+8 into buffer+5; the FICR starts at buffer+0xa in this routine, not buffer+5. The supplied report conflates offsets across packet layers. Preserve the correct extraction boundary for all disassembly. Evidence outputs/swift-connect187.txt.

## K03636
Source `D0488`, original line27, SHA256 `f73538fee3404f71ec697b174ffe1a59fb42b11dc23b483bbb176a2d650403eb`. Status: historical/unadjudicated narrative; English source prose.

Frame SSH timed out in this turn, PICO USB/read-only APIs worked. Continue offline station packet construction and reconnect receiver work; no Roy SRAM test was attempted while Frame was unreachable. Goal remains active; no standalone tracking completion claim.

## K03637
Source `D0490`, original line1, SHA256 `f05987241d2a52bf39d4dfe730d3209e5f0162e4c3883838410b9f29cfad0550`. Status: historical/unadjudicated narrative; English source prose.

# Targeted reconnect candidate213

## K03638
Source `D0490`, original line3, SHA256 `f05987241d2a52bf39d4dfe730d3209e5f0162e4c3883838410b9f29cfad0550`. Status: historical/unadjudicated narrative; English source prose.

Added build_negotiation_candidate in pc/radio/pico_swift_probe.py. Pure offline builder, no transport or loader integration. Its 21-byte template preserves PhoenixBD20 reserved fields and 0x55 at offset6, carries five-byte channel mask at7, seed at12, prefix at16 and target FICR at17. All unresolved runtime fields are explicit arguments rather than invented defaults.

## K03639
Source `D0490`, original line5, SHA256 `f05987241d2a52bf39d4dfe730d3209e5f0162e4c3883838410b9f29cfad0550`. Status: historical/unadjudicated narrative; English source prose.

Decoder now exposes exact Swift68F0 arithmetic: rate parameter payload0, context+12 flag=payload2&1, context+31=((payload3&3)<<6)|(payload2>>2), optional target ID at17. Swift6930 skips seed/prefix replacement when existing paired marker is AAAA. This does not prove all subsequent paths avoid flash, nor that the physical tracker currently has that marker.

## K03640
Source `D0490`, original line7, SHA256 `f05987241d2a52bf39d4dfe730d3209e5f0162e4c3883838410b9f29cfad0550`. Status: historical/unadjudicated narrative; English source prose.

Gate6008 merely sets volatile E4D;6020 tests volatile F02. These are not flash-write routines. Earlier persistence chain212 remains relevant for an unpaired tracker. No hardware transmission213. Nine targeted builder/bootstrap tests pass; these establish byte layout and input rejection only. FirstID[TRACKER_ID] confirmed; standalone connection, optical pose and controller coexistence still incomplete.

## K03641
Source `D0490`, original line9, SHA256 `f05987241d2a52bf39d4dfe730d3209e5f0162e4c3883838410b9f29cfad0550`. Status: historical/unadjudicated narrative; English source prose.

Evidence: outputs/firmware-standalone/swift-reconnect213.txt. Next audit other storage writer callers and paired reconnect dispatch before deploying the candidate.

## K03642
Source `D0491`, original line1, SHA256 `1004686f1b52df2fe3b182ccc627a6f4b4e36142a8035d08c3f73cf834ec48f2`. Status: historical/unadjudicated narrative; English source prose.

# Paired gate draft217

## K03643
Source `D0491`, original line3, SHA256 `1004686f1b52df2fe3b182ccc627a6f4b4e36142a8035d08c3f73cf834ec48f2`. Status: historical/unadjudicated narrative; English source prose.

Added compile-only ROY_SWIFT_PAIRED_GATE_DRAFT/tagROYPAIREDGATEDRAFT1 in Roy SRAM source. It constructs a fixed21-byte candidate with channel mask3D00000000, saved seed74735415, prefix0, and intentionally mismatching ID3EED1978 (known first FICR[TRACKER_ID]). S1 assigned only PIDbits1..2, bit3clear. This is a proposed conditional protocol gate, not a validated connection packet. Runtime parameters remain candidates.

## K03644
Source `D0491`, original line5, SHA256 `1004686f1b52df2fe3b182ccc627a6f4b4e36142a8035d08c3f73cf834ec48f2`. Status: historical/unadjudicated narrative; English source prose.

ARM build with native shell, reserved MPU, priority0,100us polling and fast TX/RX handover produced1915bytes, fits2045byte region. No deployment or hardware TX. Loader unconditionally rejects the draft tag before profile reading, stock checks or device access. Explicit rejection verified with required CLI arguments. Nine negotiation/bootstrap tests pass; no hardware success implied.

## K03645
Source `D0491`, original line7, SHA256 `1004686f1b52df2fe3b182ccc627a6f4b4e36142a8035d08c3f73cf834ec48f2`. Status: historical/unadjudicated narrative; English source prose.

Downstream persistence and unintended state effects still need audit. The draft cannot be promoted by merely adding a CLI flag. Use full individual confirmed address and preserve no-flash scope when the audit is complete. Main standalone6DoF objective remains active.

## K03646
Source `D0492`, original line1, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

# Roy の RAM モジュールについて（大まかな流れ）

## K03647
Source `D0492`, original line3, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

詳しい資料を共有していただき、本当にありがとうございました。おかげで多くの時間を節約できました。お返しに、私が Frame 本体だけでトラッカーと通信している方法の概要を書きます。

## K03648
Source `D0492`, original line5, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

## 1. コードを RAM に載せる方法

## K03649
Source `D0492`, original line7, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

1. Roy の SWD は Frame の SoC の GPIO につながっていて、SteamVR の frame_hmd ドライバーのリソースに標準の OpenOCD 設定ファイルが入っています。私はこれをそのまま使っています。
2. CPU を止めずに RAM を読み書きできます。まずフラッシュの数か所を読み、ファームウェアのビルドが想定どおりか確認します。対応していないビルドでは何もしません。
3. 使う RAM 領域の中身を保存してから、約 2 KB の小さなモジュールを空いている SRAM 領域に書き込みます。その RAM から実行できるように、MPU の設定を一時的に広げます。
4. フラッシュには一切書き込みません。

## K03650
Source `D0492`, original line12, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

## 2. モジュールの起動と MPSL タイムスロット

## K03651
Source `D0492`, original line14, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

1. エントリーはデバッガーで呼ぶのではなく、ファームウェアに入っている RTT シェル経由で、Roy 自身のスレッドから呼び出します。
2. エントリーは、そのビルドのファームウェアにある MPSL の関数を使ってタイムスロットのセッションを開き、スロットを要求します。スロットの中で RADIO を自分で設定し、ポーリングと受信を行います。
3. Frame 側の Python プログラムは SWD で共有メモリを見ています。メールボックスでコマンドを渡し、受信パケットはリングバッファで受け取ります。

## K03652
Source `D0492`, original line18, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

## 3. コントローラーの邪魔をしない方法

## K03653
Source `D0492`, original line20, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

ここが一番苦労したところです。

## K03654
Source `D0492`, original line22, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

1. Roy は 2 本のコントローラーとの BLE 2M 接続を、ちょうど 3 ms 周期で動かしています。SWD で RADIO の状態を短い間隔で読み、その周期の中で電波が空いている時間帯を見つけます。
2. 私のスロット（600 µs、6 ms ごと、高優先度）はその空き時間に入るようにしています。モジュールが毎回 RTC1 を読んで、次のスロットの位置を自分で補正します。
3. RADIO の MODECNF0 は書き換えないでください。標準の値は 0x201 で、ファームウェアは起動時に一度しか設定しません。私が別の値を書いたときは、Roy を再起動するまでコントローラーが不安定になりました。
4. トラッカーはコマンド 19 のあと、4.7275 ms 以内にポーリングされないと自分で送信を始めます。それがコントローラーの通信に重なっていたので、コマンド 16 でポーリング間隔を伝えて止めています。
5. 念のため、コントローラーの姿勢の更新が止まったら自分の通信をすぐ止める監視も入れています。

## K03655
Source `D0492`, original line28, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

この組み合わせで、3 台のトラッカーをつないだまま長時間動かしても、コントローラーへの影響は見られなくなりました。

## K03656
Source `D0492`, original line30, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

## 4. SteamVR やヘッドセットを再起動したとき

## K03657
Source `D0492`, original line32, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

1. 全部 RAM 上なので、止めるときは MPSL セッションを閉じたことを確認し、保存しておいた RAM と MPU の設定を元に戻します。
2. SteamVR の再起動は Roy のファームウェアに影響しません。ヘッドセットを再起動すると Roy も起動し直して RAM は消えるので、次に開始するときにもう一度読み込むだけです。
3. フラッシュを書き換えていないので、いつでも標準の状態に戻ります。

## K03658
Source `D0492`, original line36, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

もっと詳しく知りたい部分があれば、気軽に聞いてください。

## K03659
Source `D0492`, original line40, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

# About the Roy RAM module (rough outline)

## K03660
Source `D0492`, original line42, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

Thank you so much for sharing your detailed notes. They saved me a lot of time. In return, here is an outline of how I talk to the trackers from the Frame alone.

## K03661
Source `D0492`, original line44, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

## 1. Loading the code into RAM

## K03662
Source `D0492`, original line46, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

1. Roy's SWD lines are wired to GPIOs of the Frame SoC, and the SteamVR frame_hmd driver resources already contain a stock OpenOCD config for it. I simply use that.
2. RAM can be read and written without halting the CPU. I first read a few flash locations to confirm the firmware build is the one I expect. On any other build I do nothing.
3. I save the RAM region I am going to use, then write a small module of about 2 KB into a free SRAM area. I temporarily widen the MPU setup so that this RAM may execute.
4. I never write to flash.

## K03663
Source `D0492`, original line51, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

## 2. Starting the module and MPSL timeslots

## K03664
Source `D0492`, original line53, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

1. The entry point is not called through the debugger. I call it through the RTT shell that is built into the firmware, so it runs on Roy's own thread.
2. The entry opens an MPSL timeslot session with the MPSL functions present in that firmware build and requests slots. Inside a slot the module configures RADIO itself, polls the tracker and receives the reply.
3. A Python program on the Frame watches a shared memory area over SWD. Commands go in through mailboxes and received packets come back through a ring buffer.

## K03665
Source `D0492`, original line57, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

## 3. Not disturbing the controllers

## K03666
Source `D0492`, original line59, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

This was the hardest part.

## K03667
Source `D0492`, original line61, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

1. Roy runs two BLE 2M links to the controllers on an exact 3 ms grid. I read the RADIO state over SWD at short intervals and find the part of that grid where the air is free.
2. My slots (600 µs, every 6 ms, high priority) are placed into that free window. Each slot, the module reads RTC1 and corrects the position of the next slot by itself.
3. Please do not change RADIO MODECNF0. The stock value is 0x201 and the firmware sets it only once at boot. When I wrote a different value, the controllers stayed unstable until Roy rebooted.
4. After command 19, a tracker starts transmitting on its own if it is not polled within 4.7275 ms. Those transmissions landed on top of the controller traffic, so I tell the tracker my poll interval with command 16 to stop that.
5. As a safety net, my own traffic stops right away if a controller pose stops updating.

## K03668
Source `D0492`, original line67, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

With this combination I can keep three trackers connected for long runs without any visible effect on the controllers.

## K03669
Source `D0492`, original line69, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

## 4. When SteamVR or the headset restarts

## K03670
Source `D0492`, original line71, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

1. Everything lives in RAM. To stop, I confirm the MPSL session is closed and then restore the saved RAM and MPU setup.
2. A SteamVR restart does not touch the Roy firmware. A headset restart reboots Roy and clears the RAM, so I simply load the module again on the next start.
3. Since flash is never modified, the device always returns to its stock state.

## K03671
Source `D0492`, original line75, SHA256 `e880fa03023c52ea3103051c421dc02e4013cf8d9dffe9dace361907cd62f0f4`. Status: historical/unadjudicated narrative; English source prose.

If you would like more detail on any part, just ask.

## K03672
Source `D0496`, original line1, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

# Running a RAM-only receiver on the Steam Frame Roy MCU

## K03673
Source `D0496`, original line3, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Thanks again for your protocol notes. They helped us correct the negotiation fields, the raw slot versus bit-reversed prefix, and the command/checksum framing.

## K03674
Source `D0496`, original line5, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

We now have a hardware-verified **bounded direct IMU session through the onboard Roy**, with the PICO headset powered off. We have not yet turned this into a reliable continuously connected service or standalone 6DoF tracking. The description below separates those results from the implementation outline.

## K03675
Source `D0496`, original line7, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

## 1. Access path and firmware scope

## K03676
Source `D0496`, original line9, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Our path is:

## K03677
Source `D0496`, original line20, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

There is no external radio dongle or external SWD probe in this path. SSH only controls the Linux-side loader and reads diagnostics. The radio code runs on Roy itself. A tracker was connected to the PC for USB power during the successful experiment; we did not use that USB connection as an IMU/pose transport.

## K03678
Source `D0496`, original line22, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

On our unit Roy is an **nRF52833**, running a stock Zephyr/MPSL firmware build identified in our investigation as `1788551713` (Zephyr reports `3.3.99`). Our captured stock image SHA-256 is:

## K03679
Source `D0496`, original line28, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

The addresses below are reverse-engineered for this exact image. They are not a stable Valve API and must be checked against your own image. We validate the stock image locally and compare five live flash anchors before a session. We do not distribute the stock image.

## K03680
Source `D0496`, original line30, SHA256 `f00b752c52fa9c35e90c739901afa2268a1cb6cb7f93093803e7f3be0a19dd31`. Status: historical/unadjudicated narrative; English source prose.

Valve's OpenOCD configuration on this unit is:
