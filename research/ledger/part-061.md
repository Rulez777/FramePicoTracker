# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K04801
Source `D0170`, original line374, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Build an event-level scheduler for all native RX/TX branches, separately hardware ADDRESS/END, delayed handler, autonomous TX, hopping, and command16 transition. Run 3 peers with 0/6/12 ms slots and controller reservation. Find a counterexample where integer-multiple cadence still causes collision, missed RX, or starvation. Answer first whether the claimed root effect can follow from these 30 bytes at all.

## K04802
Source `D0170`, original line376, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Profile C: OTA/boot/recovery

## K04803
Source `D0170`, original line378, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Independently derive start/data/finish fields and ACK forms from native code; verify retry/dedup/queue/S1 semantics, sentinel, SPI page layout, whole-image CRC, metadata, and magic. Specify the minimal bootloader dump/region/trigger without which bootability and recoverability cannot be concluded. Do not declare RSA bypass and bank safety based on host strings alone.

## K04804
Source `D0170`, original line380, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Profile D: host implementation/reliability

## K04805
Source `D0170`, original line382, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Check OtaSession + adapter + loader lifecycle for stale ACK, generation namespace/wrap, partial RX, ring ordering, failure before begin, guard failure, interrupted cleanup, file I/O errors, and state durability. Propose a deterministic offline regression test for each defect found. Do not test hardware and do not modify persistent service.

## K04806
Source `D0170`, original line384, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Profile E: tests and proof independence

## K04807
Source `D0170`, original line386, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Separate real ARM assertions from assumptions encoded in Python stubs. Find circular tests, missing path coverage, and assertions that pass under an incorrect phase model. Verify that fixed entry fragments do not hide incorrect origins and event ordering. Propose a minimal extended model instead of yet another dozen tests of identical arithmetic.

## K04808
Source `D0170`, original line388, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Required response format

## K04809
Source `D0170`, original line408, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 13. Criteria for further decision

## K04810
Source `D0170`, original line410, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Offline review is sufficient to approve byte/format correctness only within the verified scope. For hardware testing, the following are still required: verified activation/recovery path, absence of unresolved sender blockers, a single selected target, fresh controllers/Frame baseline, clear stop/cleanup plan, and a separate user signal.

## K04811
Source `D0170`, original line412, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Hardware success must confirm: acceptance of whole-image CRC; boot candidate; measured3000/12000cadence; correct RX phase; absence of autonomous collisions; absence of controller validity/freeze regression; reconnect; native IMU/LED/camera behavior; return to stock via working path. Do not promise all of this in advance.

## K04812
Source `D0170`, original line414, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 14. Appendices: frozen evidence and sources

## K04813
Source `D0170`, original line416, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The following sections are added directly from the current project files. Hash tables help distinguish this revision from subsequent fixes. Real registry IDs, private config/seed, credentials, calibration, and raw RF captures are not included.

## K04814
Source `D0170`, original line418, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

<!-- GENERATED_VALIDATION_APPENDICES -->
### Appendix A. Result of offline re-verification
This run was performed during MD preparation; no device was used.

## K04815
Source `D0170`, original line424, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Python: `3.14.6`. OS: `win32`.
Libraries: pytest=9.1.1, unicorn=2.1.4, capstone=5.0.9, numpy=2.5.3.
Hexadecimal bytes and hash re-verified; firmware files have not changed.

## K04816
Source `D0170`, original line428, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Appendix B. File snapshot hashes

## K04817
Source `D0170`, original line430, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| File | SHA256 |
|---|---|
| `pc/radio/tracker_grid_patch.py` | `973491875c989fb712b8d047f21b010132a469ae7408ca4cc342ec66a28d8dea` |
| `pc/radio/tracker_image_audit.py` | `5368002f65a13cdfe7f07a7719fec841aad12896dbe086d444ddfa7204b47a1b` |
| `pc/radio/tracker_ota.py` | `cdf625b7e9baec023430d1275bc13d941974f5706d03c4e440323b7896347661` |
| `pc/radio/tracker_ota_prepare.py` | `c9a1bcebbca8ade2d0f8510112a8a5220ef1dbfbf1b2698c40855c5456a0fe47` |
| `pc/radio/ram/tracker_ota_runtime.py` | `1aa40fa9c82e6a9e7c01429da9d0864731de4c22f0e1c13392d4ac1249fa5a4f` |
| `pc/radio/swift_light_sync.py` | `009161b2a10263d64f9e6fb5ef46bbfba816280be8432438b8b243b8bc2942d5` |
| `pc/radio/swift_records.py` | `f2067681039c8a9a0f0a6ffe484e3ee7ed393dc8756238bc97b1327b4211686e` |
| `pc/radio/ram/peer_engine.py` | `e82f9398b31a342a26f0ef1904def7db1863c78444c2f1645a17a441c5d3e305` |
| `pc/radio/ram/roy_ram_station_v4.c` | `43b9af2d1333a252e243614e3e50434650f7190c418dae890e5b4fa694072950` |
| `pc/radio/ram/ble_phase.py` | `b0b415ff0f052228ae81bbc470b23e6084d517482cf0e7155768ef72a3ddde9f` |
| `pc/radio/ram/controller_guard.py` | `177b08343eae38aa7f6d587345e8f2d32079d9944341d75586a7ab62bec5bfa6` |
| `pc/tests/fw189_emulator.py` | `e67e64b4f6644502cf64b911095385511d410fe90390a29e6528de514d18a802` |
| `pc/tests/test_tracker_grid_patch.py` | `c1ce61eae3a7244a7963250fe1261a6a9a2b168c67aaed1865dba3b5846a6d26` |
| `pc/tests/test_tracker_ota.py` | `10bec88ce83103f5a2b3f8995b15d96ab1d8d63793e1af5260b85018fb89952f` |
| `pc/radio/ram/roy_ram_loader.py` | `ba6444be0cbcb74e1c93b462dd510503d98e06ef8238d9613a31fd78adc9992e` |
| `pc/radio/ram/station_v4_runtime.py` | `78e4d465d4ec9c537c37aef6f4de33a7ae3f64a9a799d740298588457d7f901b` |
| `pc/tests/test_tracker_ota_runtime.py` | `0703dc85ab6f00606c82ccd86036eb54836fa43fefec7ef523d9554dfc6828b3` |
| `pc/tests/test_peer_engine.py` | `7bb7c9ab0a624043bbc8bf217526c37eb0b9dc55e1e056f6350401982c1a0672` |
| `pc/tests/test_integration_abi4.py` | `1c9b1c989761b825c0e1dd3f454248f99ec2beaa90eb7e17fd65dad3ba1fae59` |
| `pc/tests/test_controller_guard.py` | `57290b8c35fd39d647fbce834c740a96a6357470d0e9afcb29f43eaa1e5b4f61` |
| `outputs/swift189-image.bin` | `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89` |
| `outputs/tracker-grid3000-20261007/fw189-stock.bin` | `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89` |
| `outputs/tracker-grid3000-20261007/fw189-valve-grid-3000.bin` | `6a82a89206a609fdce92a3de764882ce53f79f0360b925bebe1ce7853cde6a0d` |
| `outputs/pico4e/trackerbin/Swift2.0_sv1.89_20241213_b189_app.bin` | `41bd2d24dd9293e7d17c62ab18acd1001d6085da393123a67d226b28d2132e54` |

## K04818
Source `D0170`, original line457, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Appendix C. `patch-manifest.json`

## K04819
Source `D0170`, original line554, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Appendix C. `ota-wire-example.json`

## K04820
Source `D0170`, original line571, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Appendix D. Full source snapshots

## K04821
Source `D0170`, original line573, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Code block line numbers match files starting from line 1; for comments, specify file/function/line. Snapshots are byte-normalized into MD text only by line endings. The hash above refers to the original file, prior to normalization.

## K04822
Source `D0170`, original line2173, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Appendix E. Integration fragments

## K04823
Source `D0170`, original line2175, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The rest of loader contains the original version-gated SRAM lifecycle. The given range numbers refer to the hash of this snapshot. For a complete recovery audit, request the complete loader separately.

## K04824
Source `D0170`, original line2673, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Appendix F. Stock disassembly of key regions

## K04825
Source `D0170`, original line2675, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Source: `outputs/swift189-full.asm`, SHA256 `4ed45fa85967e4b004181b4927ee0493fe0a92c0d6c3116e3ca276c0d103c553`. This is a saved linear disassembly: literal pool/data are sometimes shown as instructions. Use resolved literals and raw bytes, do not consider the entire text as executable code. Base0x1000. The snapshot does not include the bootloader.

## K04826
Source `D0170`, original line3735, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Every8bytes: code/type word and Thumb callback pointer; confirm independently.

## K04827
Source `D0170`, original line3753, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Appendix G. What is not attached

## K04828
Source `D0170`, original line3755, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Full tracker bootloader and its flash dump.
- Full binary/linear disassembly of vendor application (provide separately for full analysis).
- Private registry IDs, seed/config, keys, calibration, raw camera/RF captures.
- Fresh hardware measurements of the new patch: they do not exist yet.
- Full runtime test fixtures with real product serials (intentionally not included).

## K04829
Source `D0170`, original line3761, SHA256 `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Do not supplement these gaps with guesswork. Request minimal specific evidence.

## K04830
Source `D0171`, original line1, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Investigation: independent validation of `FW189_GRID3000_INDEPENDENT_VALIDATION.md`

## K04831
Source `D0171`, original line3, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-07. Verification author: separate offline analysis based on artifacts, source code, and test re-run.

## K04832
Source `D0171`, original line5, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Subject: `docs/FW189_GRID3000_INDEPENDENT_VALIDATION.md` and bundle `outputs/tracker-grid3000-20261007/`.

## K04833
Source `D0171`, original line7, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**The source document was not modified.** Binaries, manifest, and wire examples were not modified. SSH, SWD, RF, OTA, services, and device firmwares were not launched.

## K04834
Source `D0171`, original line16, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The document itself does not claim that 30 bytes resolve coexistence with Steam Frame controllers or that the bootloader is safe. The verdicts below pertain to the verifiable assertions of the document and to the completeness of the candidate patch itself.

## K04835
Source `D0171`, original line20, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 1. Scope of work

## K04836
Source `D0171`, original line22, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Permitted: reading source code, calculations, Capstone, hash reproduction, offline Unicorn/pytest.

## K04837
Source `D0171`, original line24, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Prohibited by this verification and not performed: flashing to Frame, OpenOCD, radio, OTA, restart of services, modification of `FW189_GRID3000_INDEPENDENT_VALIDATION.md`.

## K04838
Source `D0171`, original line26, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Three subjects were not conflated:

## K04839
Source `D0171`, original line28, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Correctness of the ten claimed edits (encoding, completeness of dependent constants, absence of extra diffs).
2. Correctness of OTA (wire, ACK, SPI staging, CRC, host sender).
3. Effect on radio (phase, autonomous TX, response length, coexistence).

## K04840
Source `D0171`, original line32, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The correct 30 bytes do not prove safe OTA and do not prove a compatibility fix.

## K04841
Source `D0171`, original line36, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2. Snapshot identity

## K04842
Source `D0171`, original line38, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

SHA256 of the verified MD matched `outputs/tracker-grid3000-20261007/independent-validation-artifact.json`:

## K04843
Source `D0171`, original line40, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Field | Value |
|---|---|
| Document SHA256 | `976d766758edb6c21a668b518b4510b0219a1e7b4b3199b18f6c63155e3cc0e7` |
| Size / lines | 184698 bytes / 3761 |
| Python / OS during packaging | 3.14.6 / win32 |
| Re-run of this verification | 79 passed in 2.80s, pytest 9.1.1, unicorn 2.1.4, capstone 5.0.9 |

## K04844
Source `D0171`, original line47, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

All hashes in Appendix B of the document matched the files in the tree at the time of analysis. There is no source code drift relative to the snapshot.

## K04845
Source `D0171`, original line49, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Object | Size | SHA256 |
|---|---:|---|
| Vendor container `Swift2.0_sv1.89_20241213_b189_app.bin` | 193385 | `41bd2d24dd9293e7d17c62ab18acd1001d6085da393123a67d226b28d2132e54` |
| Raw stock / `swift189-image.bin` / `fw189-stock.bin` | 96532 | `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89` |
| Candidate `fw189-valve-grid-3000.bin` | 96532 | `6a82a89206a609fdce92a3de764882ce53f79f0360b925bebe1ce7853cde6a0d` |
| `patch-manifest.json` | 2392 | `3ac2a6b141903c8c60b4505de0934dbaf15aca2e00b4607fde95a03f27d5f694` |
| `ota-wire-example.json` | 466 | `12f3c5f6ca1d80a2491b218b5f083d6f8fe98b4cf29456f5d011a991571164ac` |
| `outputs/swift189-full.asm` | — | `4ed45fa85967e4b004181b4927ee0493fe0a92c0d6c3116e3ca276c0d103c553` |

## K04846
Source `D0171`, original line58, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Container: header of 321 bytes (`0x141`) + two identical copies of raw image. Both are equal to frozen stock. Base VA `0x1000`, `file_offset = VA - 0x1000`, SP `0x20006000`, reset `0x13a5`, image end exclusive `0x18914`. The signed container was not rewritten.

## K04847
Source `D0171`, original line60, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Re-run suite command, as in the document:

## K04848
Source `D0171`, original line70, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Result of this session: 79 passed, 0 failures / errors / skips.

## K04849
Source `D0171`, original line74, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. Binary patch verdict (scope of ten sites)

## K04850
Source `D0171`, original line76, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 3.1. The claimed table is encoded correctly

## K04851
Source `D0171`, original line78, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Between stock and candidate there are **exactly 30 differing bytes**. The image length does not change. The modified offsets lie strictly within the 56 bytes of the claimed ranges (`6×4 + 4×8`). The first 8 bytes (SP/reset) match. Reversing via the manifest restores the stock SHA.

## K04852
Source `D0171`, original line80, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Independent Thumb-2 MOVW T3 and IEEE754 LE double, plus Capstone on candidate:

## K04853
Source `D0171`, original line95, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

After the patch, **no** double 4727.5 / 3538.0 / 3934.5 **remain** in the image and **no** MOVW 4727 / 18910 **remain**. The literal pool was not shifted.

## K04854
Source `D0171`, original line97, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

RX-offset policy: `Δ = 4727.5 − 3000 = 1727.5`.

## K04855
Source `D0171`, original line107, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For N=13:

## K04856
Source `D0171`, original line115, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`4 × 4727.5 = 18910` and `4 × 3000 = 12000` — arithmetic consistency of TX, not proof of the necessity of a fourfold autonomous period.

## K04857
Source `D0171`, original line117, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 3.2. Sign −25 confirmed by instructions, not only by test

## K04858
Source `D0171`, original line119, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Miss branch `0x6f5e`: `(N+1)*period`, then `0x13754`. Bytes `0x13754`:

## K04859
Source `D0171`, original line126, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is a sign inversion of high-word double 25.0 (`0x40390000` @ `0x6fd4`, low word 0) and addition = **subtraction of 25**. The purpose 'compensation of software latency of exactly 25 µs' remains an interpretation.

## K04860
Source `D0171`, original line128, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Separate double 25.0 @ `0xe89c` is the IMU/temperature path (`raw/132.48 + 25 C`), not RX. The document correctly did not patch it.

## K04861
Source `D0171`, original line130, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 3.3. What the emulator actually verifies

## K04862
Source `D0171`, original line132, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

TX inputs `0x61f4` / `0x62e4` (N=0,1,9): CC0 stock 4727/18910, candidate 3000/12000.

## K04863
Source `D0171`, original line134, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

RX N ∈ {0,1,3,9,13,40,255}: stop **before** `0x641c`, r0 is read. No hardware sleep, RADIO IRQ, TIMER origin.

## K04864
Source `D0171`, original line136, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This confirms the arithmetic of the selected branches, not the on-air phase.

## K04865
Source `D0171`, original line140, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 4. Finding A-1: five unpatched delays `9922 = 2×period + 467`

## K04866
Source `D0171`, original line142, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Severity: **major** for radio cadence completeness. Not an encoding error of the ten sites.
- Exact VA: `0x6538`, `0x65a2`, `0x67de`, `0x6c42`, `0x6c5c`.
- Instruction: `movw r0, #0x26c2` (9922). Encoding `42f6c26c`.
- All five lead to the same `0x641c` as hunt `0x6cde`/`0x6ce4` and computed RX delays. `0x641c` immediately does `mov r4, r0` — timer argument in the same units.

## K04867
Source `D0171`, original line147, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Calculation:

## K04868
Source `D0171`, original line155, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

467 matches the ADDRESS alignment constant of command 10 in the project log (`V+467`). Here this is an **assumption about units** (microseconds vs ticks of TIMER1). The non-hypothetical part: multiplier `2 × 4727.5` enters the immediate exactly, and the patch does not change it.

## K04869
Source `D0171`, original line157, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

False positives: raw `c226` @ `0x13aca` — inside `orr.w r6, r6, r2, lsl #11`, not constant 9922.

## K04870
Source `D0171`, original line159, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Minimal reproducer:

## K04871
Source `D0171`, original line166, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Minimal fix, if policy is 'all arguments 0x641c, linearly related to period': replace five MOVW with `6467` with the same rd/width, add to manifest, Unicorn test 'r0 at input of 0x641c'. Do not do this without a trace if 467 turns out to be ticks of another timer.

## K04872
Source `D0171`, original line168, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The document in profile A explicitly asked to search for missed period-derived constants. In the 'full patch table' they are absent.

## K04873
Source `D0171`, original line170, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Neighboring MOVWs to the same timer, deliberately not in the table

## K04874
Source `D0171`, original line172, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| VA | Immediate | Relationship | Assessment |
|---|---:|---|---|
| `0x698c`, `0x6a08` | 457 (`#0x1c9`) | RX-close ~457.5−4L µs | Consistent with policy 'do not scale RF delay'. Keep. |
| `0x6eac` | 10980 (`#0x2ae4`) | `24 × 457.5 = 10980` exactly; also `2×4727.5+1525` | Rather a timeout from close-delay, not from period. Too early to patch. |
| `0x6b7c` | literal `#0x49327` = 299815 | ~300 ms, not integer ×4727.5 | Not period-derived. |

## K04875
Source `D0171`, original line180, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 5. OTA sender verdict (offline model)

## K04876
Source `D0171`, original line182, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 5.1. Wire, CRC, SPI — matched

## K04877
Source `D0171`, original line184, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Application record: `[seq u8][cmd=1][len][body]`, working checksum = sum of preceding bytes, **not** CRC8/MAXIM. CRC8/MAXIM is for image. Sequence modulo 64.

## K04878
Source `D0171`, original line186, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Table `0x16558`: `25000000 cd880000` → opcode `0x25`, Thumb `0x88cd` = handler `0x88cc` → `0xfbcc`.

## K04879
Source `D0171`, original line188, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Start body 9 bytes: `25 01` + BE32 length `00017914` + CRC `e6` + BE16 189 `00bd`.

## K04880
Source `D0171`, original line190, SHA256 `7f22f6afcf2287f6e49239abd8d680064458f6dcf101e78bb9b1a930eeedfca4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Sequence 0 packets from the bundle, independently assembled by the builder, are byte-for-byte equal to `ota-wire-example.json`:
