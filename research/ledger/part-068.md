# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05361
Source `D0191`, original line13, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- `nativeReadHardwareBuffer` reads Android AHardwareBuffer, not MCU memory.
- `getFwEncryptionState` returns five bytes of station/two controllers status; the interface does not accept flash address and size.
- Six MucTestSpi builders in offline emulation formed fixed queries of 131 bytes for version/MAC/serial numbers. These are not Swift dump queries.
- A JADX error in `UpdateManager.extractFirmware$2` concealed the copying of Android assets into local files with hash verification.
- ControllerUpgrade "repair" switches the controller/station protocol using local bin files. Its backup is the `/system/etc/ctlbin_bak` directory, not a read Swift bootloader.

## K05362
Source `D0191`, original line19, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Source materials and reproducibility

## K05363
Source `D0191`, original line21, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The already extracted PICO 5.13.8 image `outputs/fw189-package-search17/5.13.8/system.img`, APK, and JADX source files from `outputs/fw189-offline18/` were used. New files are located in `outputs/fw189-jni-audit19/`.

## K05364
Source `D0191`, original line23, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Artifact | Purpose |
| --- | --- |
| `native-java-inventory.json` | Native declarations and library loading; regex inventory, not a strict method count |
| `jadx-error-methods.json` | Found JADX ERROR stubs |
| `apk-native-libraries.json`, `system-library-provenance.json` | Origin of extracted ELFs |
| `library-symlinks.json` | Factory APK symlinks to `/system/lib` |
| `elf-symbols.json` | Exports, addresses, function sizes, dependencies |
| `*.asm`, `*.plt.json`, `*.strings.json` | Disassembly, imports, and strings |
| `plt-validation.json` | Verification of ARM PLT against actual GOT addresses |
| `resolved-pic-strings.json` | Resolved LDR/add PC references to strings |
| `disassemble_jni.py` | Generation of native listings |
| `emulate_node_wire.py`, `node-offline-wire.json` | Limited emulation of packet builders |
| `UpdateManager.*.java`, `C2S1UpgradeDelegate.*.java`, `CV3ServiceProxy.*.java` | Re-decompilation in simple/fallback modes |
| `jadx-*.log` | Re-decompilation logs |

## K05365
Source `D0191`, original line38, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

JADX 1.5.6 was used, which was already downloaded and verified in iteration 18. Emulation reproduction command from the project root:

## K05366
Source `D0191`, original line44, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The last run finished with exit code 0. The script checks require completion of six selected builders and a length of each captured packet of 131. They do not require successful completion of the seventh function `checkStationLinkStatus`, whose limitation is explicitly recorded below.

## K05367
Source `D0191`, original line46, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Native libraries

## K05368
Source `D0191`, original line48, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In PicoFactoryTest3.apk there are no built-in `.so` files. Its `lib/arm` directory contains symlinks to system libraries. Extracted from the image:

## K05369
Source `D0191`, original line50, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Library ARM32 | Size, bytes |
| --- | ---: |
| libpico_factorytest3.so | 19904 |
| libpico_factorytest_jni.so | 45732 |
| libpico_factorytest_pxrhmd.so | 24200 |
| libpico_factorytest_sensor.so | 19708 |
| libpico_node_connection.so | 28248 |
| libpxrcontrollerclient.pxr.so | 155540 |
| libpxrinit.so | 211352 |
| libpxrstreamingserviceclient.so | 59628 |
| libstationclient.pxr.so | 266144 |

## K05370
Source `D0191`, original line62, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Additionally, AArch64 `libtestlib.so` (5896 bytes) and `libpxr_api.so` (362888 bytes) from ControllerTest were analyzed.

## K05371
Source `D0191`, original line64, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For all nine ARM32 ELFs, 1454 PLT entries were verified: the GOT address computed from instructions matched the relocation `r_offset`. When decoding ARM immediate, rotation was taken into account, not just the printable operand value. This verification is not claimed for AArch64 PLT.

## K05372
Source `D0191`, original line66, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The listings cover exported STT_FUNC ranges and selected called functions. Literal pools may be displayed by the disassembler as instructions; string addresses were verified separately. This is not a complete graph of all indirect calls.

## K05373
Source `D0191`, original line68, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Factory JNI: what is actually read

## K05374
Source `D0191`, original line70, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The addresses below are virtual addresses in the corresponding ELF, not flash addresses of the tracker.

## K05375
Source `D0191`, original line74, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`libpico_factorytest_jni.so`, `nativeReadHardwareBuffer` @ `0x543c`, 248 bytes: check of Java array and offsets → `AHardwareBuffer_fromHardwareBuffer` → `AHardwareBuffer_lock` → `SetByteArrayRegion` → `AHardwareBuffer_unlock`.

## K05376
Source `D0191`, original line76, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Source is an Android buffer. The name read does not mean MCU access.

## K05377
Source `D0191`, original line78, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Encryption state and station version

## K05378
Source `D0191`, original line80, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`getFwEncryptionState` @ `0x59a0`, 172 bytes, calls `onControllerFwEncryptionState` @ `0x7d68`, 204 bytes. The latter loads `libstationclient.pxr.so`, resolves `CreateStationClient` and `GetFwEncryptionState`.

## K05379
Source `D0191`, original line82, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The result is five bytes unpacked by JNI into five Java ints. Log strings designate: station code, left controller code, left controller uart, right controller code, right controller uart. In StationClient, method @ `0x23608` and Binder proxy @ `0x321e8`, transact `0x61`, fixed word+byte response were traced. There is no arbitrary flash address and length in this interface.

## K05380
Source `D0191`, original line84, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`JniUtil.getStationVersion` @ `0x52f8` → `getStationVersion` @ `0x64a0` → `CreateStationClient`/`GetStationVer`: fixed version of seven bytes.

## K05381
Source `D0191`, original line86, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is a classification of client contracts. It does not prove the internal behavior of all server handlers and does not automatically authorize invoking them on hardware.

## K05382
Source `D0191`, original line88, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Generic command and USB DP

## K05383
Source `D0191`, original line90, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`JniUtil.doCmd` @ `0x5534` → `docmd` @ `0x5c94` → import of `cCmd` from `libpxrinit.so`. `cCmd` @ `0x1bd90` obtains `IPxrInitInterface` and passes the command via IPC. In Java `doCommand(String)`, the result is ignored and an empty string is returned.

## K05384
Source `D0191`, original line92, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

It is not established that an arbitrary string here is a shell command or a safe read. The server-side implementation of this generic IPC has not been completely analyzed. No proven Swift flash readback contract was found.

## K05385
Source `D0191`, original line94, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`usbDPWriteData` @ `0x5b68`, 300 bytes, loads streaming client, obtains module, calls callback/startService/getServiceMode/setServiceMode, then `writeHIDData(inputString, strlen)`. This is data transmission with service mode alteration. The name USB does not prove a tracker diagnostic port.

## K05386
Source `D0191`, original line96, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

RF/setter/reset tests were not run. Sensor/display branches use Android sensor/looper and associated callbacks; in the reviewed interfaces, arbitrary reading of Swift flash was not found.

## K05387
Source `D0191`, original line98, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## MucTestSpi: offline packet verification

## K05388
Source `D0191`, original line100, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`initNodeConnection` @ `0x2070` receives a path string and calls `__open_2(path, 2)`, that is, opens an O_RDWR file descriptor. From the SPI name it cannot be inferred that this is SWD or a connected USB tracker.

## K05389
Source `D0191`, original line102, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

JNI contains getter functions for station version, MAC, controllers version/SN, as well as reset/test functions. Controller id in Java is restricted to 1/2. The arguments of the selected builders do not specify a flash address or dump size.

## K05390
Source `D0191`, original line104, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Call | Arguments | First 8 bytes of request | Length | Return in model |
| --- | --- | --- | ---: | --- |
| getVersion | 1, 0 | bb0100040000000c | 131 | yes |
| getMac | 1, 0 | bb01000a01000000 | 131 | yes |
| getControllerVersion | 1, 1 | bb01010401000000 | 131 | yes |
| getControllerVersion | 2, 1 | bb01020401000020 | 131 | yes |
| getControllerSN | 1, 1 | bb01010501000020 | 131 | yes |
| getControllerSN | 2, 1 | bb01020501000020 | 131 | yes |

## K05391
Source `D0191`, original line113, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Unicorn ARM emulation loads the ELF and necessary relocations. JNI, `write`, FD waiting, and standard functions are stubbed. `write` only saves the buffer to JSON, the JNI array is synthetic, select/readiness check do not model a real response. The descriptor in the capture is the synthetic initial value `0xffffffff`, there is no device behind it. The remaining external calls have simplified returns; this is acceptable for a narrow check of packet formation, but not for proving a full-fledged session.

## K05392
Source `D0191`, original line115, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The limit is 200000 instructions per call. `checkStationLinkStatus(1,0)` did not return within this limit; there were no write entries. The reason has not been established; this call is not declared a verified getter. The fields of byte 7 and other parts of the packets do not acquire additional semantics solely from these six captures.

## K05393
Source `D0191`, original line117, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Result: specific builders of fixed diagnostic requests are confirmed. Device responses, the target handler, over-the-air, bootloader reading, and recovery were not verified.

## K05394
Source `D0191`, original line121, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`libtestlib.so`: `NativeLib.SetControllerSwap` @ `0x6ec`, 316 bytes, resolves `SetControllerSweep`, creates ControllerClient, calls setter for id 1 and 2. This is not a reader.

## K05395
Source `D0191`, original line123, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In `libpxr_api.so` and the corresponding JNI declarations, reset/config/serial/plugin/graphic/platform methods were reviewed. No arbitrary memory-reader was found in the reviewed interfaces. The full IL2CPP/managed graph was not reconstructed.

## K05396
Source `D0191`, original line125, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## JADX errors and reconstructed branches

## K05397
Source `D0191`, original line127, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Re-decompilation was performed in simple and fallback modes using real DEX names:

## K05398
Source `D0191`, original line133, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

All six corresponding runs completed with code 0. Simple contains goto and type warnings; this is a listing for analysis, not ready compilable Java. Fallback preserves low-level structure for mapping.

## K05399
Source `D0191`, original line137, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The error in `UpdateManager.h.i(Object)`, metadata `extractFirmware$2`, was closed by reviewing simple/fallback and the invoked copy-helper.

## K05400
Source `D0191`, original line139, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Chain: `assets.list(\"firmware\")` → skipping config.json → creation of local directories → `assets.open(assetpath)` → `FileOutputStream` → `FileUtils.copy` → DigestUtil → comparison of hash from configuration → local JSON recording of results. Upon mismatch, the local file is deleted and an exception is thrown.

## K05401
Source `D0191`, original line141, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In this branch and the reviewed helper, there is no native/Bluetooth/MCU update call. In this APK, the configuration is 66 bytes, version 1.0, sha256, `hash_info={}`; no other embedded firmware assets were found in iteration 18. This is not an assertion about all future packages or runtime configuration.

## K05402
Source `D0191`, original line145, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`protocolDowngradeRepairProcess`: protocol and dual-controller connection check → `CVServiceProxy.I()` → `LocalCtlBinStore.findBackupBinFile` → conversion of bin to command → upgrade call → new protocol check. There is an explicit error \"no station backup bin found locally\".

## K05403
Source `D0191`, original line147, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`CVServiceProxy.I()` copies `Q()` to `m()`. AppConfig sets `Q()` as `/system/etc/ctlbin_bak`, and `m()` as the application's external directory `ctlbin_bak`. The main directory is `/system/etc/ctlbin`, its local copy is `mybin`. Therefore, the word backup here means the original local bin directory, not a dump from the connected MCU.

## K05404
Source `D0191`, original line149, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`protocolUpgradeRepairProcess` loads the bin via LocalCtlBinStore, sends the upgrade, and checks the protocol. In the reconstructed `CV3ServiceProxy.a0`, a loop over C2S1UpgradeCommand, checks of controller0/controller1, and sequential upgrade calls are visible.

## K05405
Source `D0191`, original line151, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

These branches modify the firmware/protocol of the station and controllers; running them to verify reading is prohibited. No contract for Swift readback or FW189 recovery was found in them. The absence of Swift strings is not in itself proof; the conclusion also relies on the C2S1 model and the traced local bin source.

## K05406
Source `D0191`, original line153, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The remaining JADX errors found relate to generic coroutine/flow/UI/media- classes. They are not passed off as fully reconstructed code of all APKs. In the factory APK, the ERROR stub found relates to MediaSession2Stub, not to factory JNI.

## K05407
Source `D0191`, original line155, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What remained unknown

## K05408
Source `D0191`, original line157, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- The full server of common `cCmd`, some indirect native calls, legacy HummingBird/decode-jni dependencies, and the full IL2CPP graph are not covered by this report.
- A dump of the real Swift bootloader was not obtained; its checks, erase/copy order, admissibility of an enlarged image, and recovery path are unknown.
- A clean start of the first OTA on stock and the old ACK race are not resolved in hardware.
- Read protection, real diagnostic port responses, and hardware recovery were not tested.

## K05409
Source `D0191`, original line162, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

One cannot conclude \"if the image is rejected, the tracker will necessarily remain intact\". This requires the bootloader's check and write order, which is not present here. The presence of a saved stock file does not guarantee the ability to deliver it after losing a working radio application.

## K05410
Source `D0191`, original line164, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Immutability and summary

## K05411
Source `D0191`, original line166, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Hashes were re-verified:

## K05412
Source `D0191`, original line168, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Image | Size | SHA256 |
| --- | ---: | --- |
| `outputs/swift189-image.bin` | 96532 | `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89` |
| `outputs/fw189-dual-mode-iter05-final-20261007/fw189-dual-mode.bin` | 97320 | `395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f` |

## K05413
Source `D0191`, original line173, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Working firmware/station code, whitelist, and unconditional hardware OTA gate were not edited. Production tests were not rerun: only analytical artifacts and documents were modified. Narrow native emulation was verified, not hardware readiness.

## K05414
Source `D0191`, original line175, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The planned offline verification of JNI and selected DEX errors is completed. It yielded no new path for safe reading or proven recovery and did not alter the OTA risk assessment.

## K05415
Source `D0193`, original line1, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# FW189: offline refinement after independent reviews

## K05416
Source `D0193`, original line3, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-07. The user's hardware pause remains in effect. SSH, SWD, RF, OTA, and services were not launched. Old images and reports are preserved.

## K05417
Source `D0193`, original line7, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Confirmed defects of OTA host code have been fixed, execution of real ARM timer paths has been expanded, and the activation boundary has been investigated. 104 tests passed. No new binary patch was created: additional replacements cannot yet be justified without assumptions. The old candidate remains experimental and not ready for installation.

## K05418
Source `D0193`, original line9, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Particularly important: fixing the tx_generation check does NOT completely close an old ACK from the tracker queue. A reproducible remaining counterexample is given below. This limitation is explicitly kept in the package, not declared fixed.

## K05419
Source `D0193`, original line11, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What has been changed in the sender

## K05420
Source `D0193`, original line15, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- In start/data/finish and drain, tags of the current operation that were issued via mailbox are checked. A generation without publication, of a past operation, or of the regular peer engine does not advance the state machine.
- OTA continues the generation of the regular peer rather than starting from 2. Full generation is used for publishing and low16 for the ABI4 response.
- Crossing low16=0xffff with tag reuse is prohibited. Before start, it is verified that space will suffice for at least a transmission without retries: 2×blocks+4 operations, each requiring a tag. Retries may exhaust the margin; then transmission completes with an error without automatic wrap/resume.
- A delayed response from one of the published attempts of the current operation is permitted. A command retry preserves the body and command sequence.
- Drain does not complete with a response containing an OTA record: a repeated application ACK does not prove queue flushing. ACK-only polls remain.

## K05421
Source `D0193`, original line21, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Mailbox publication means issuing a packet to the adapter; physical transmission is confirmed only by a correlated MCU record. Offline tests are not over-the-air proof.

## K05422
Source `D0193`, original line25, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Timeout before OTA and unsuitable firmware preserve state=failed and error.
- Stopping during waiting_target preserves the reason, instead of perpetual waiting_target.
- Poll errors are preserved prior to re-throwing the exception. Cleanup does not erase the original reason.
- Identical statuses are not rewritten on every poll. This eliminates redundant writes without state changes, but does not prove bounding of file I/O latency: real transitions and the event log are still written synchronously.
- Result remains single-use: automatic replay after an error was not added.

## K05423
Source `D0193`, original line31, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Remaining ACK counterexample

## K05424
Source `D0193`, original line33, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`tx_generation` in ABI4 designates the poll after which Roy accepted the response. It is not a nonce written by the tracker inside the application ACK upon creation.

## K05425
Source `D0193`, original line35, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

If an old queued start ACK `25 01 01` is returned in response to a fresh poll of the current operation, it carries a fresh tx_generation. Its wire-body does not differ from a new start ACK. A generated offline input with such an ACK still transitions the sender from start to drain. This is recorded in evidence JSON as `queued_old_start_ACK_returned_under_current_poll_tag: accepted=true`.

## K05426
Source `D0193`, original line37, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is not a reproduction of real queue delay. This is proof that a single host tag check does not establish the freshness of an application ACK. Similarly, the presence of a matching command sequence cannot be demanded without proof of echo semantics: the tracker forms its own outbound queue via 0x9368 → 0x92a8.

## K05427
Source `D0193`, original line39, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Before installation, a native queue model and boundaries of a new session are required: 0x9458/0x96f8/0x95c4, application of S1 ACK, response packing, and asynchronous erase completion. Until a proven flush/sync procedure exists, the sender cannot be declared ready. In the existing model, these queues were bypassed by direct call to dispatcher and 0xf974; it does not close this issue.

## K05428
Source `D0193`, original line41, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Timer audit result

## K05429
Source `D0193`, original line43, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Real inits

## K05430
Source `D0193`, original line45, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

TIMER0: executed 0x8492 → 0x6184. MODE=0, BITMODE=3, PRESCALER=4. Tick is 1 µs. Values of 3000/12000 specify 3/12 ms on the verified path. 3 at +0x508 pertains to BITMODE.

## K05431
Source `D0193`, original line47, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

TIMER2: executed 0x6600 with real SDK init 0x3868. MODE=0, BITMODE=3, PRESCALER=4. Real driver clear 0x3ab4 and compare writer 0x3ae0 were also executed, without substitution in conversion tests.

## K05432
Source `D0193`, original line49, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Both branches of 0x641c

## K05433
Source `D0193`, original line51, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

When `0x20000f02=0`: `(argument×16) >> TIMER2.PRESCALER`, then TIMER2 CC. Prescaler 3,4,9 checked for all N=0…255; under the actually observed init=4, the integer argument of microseconds is transferred accurately.

## K05434
Source `D0193`, original line53, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

When `0x20000f02≠0`: double division by 30.5, conversion to integer, RTC clear and CC. Checked normal/alternate/miss for all N=0…255 on stock and candidate. The physical time below assumes RTC 32768 Hz, PRESCALER=0; this is a calculation, not a frequency measurement.

## K05435
Source `D0193`, original line55, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Candidate N=13 | Integer argument | RTC CC | Conditional physical time, µs |
| --- | ---: | ---: | ---: |
| normal | 40810 | 1338 | 40832.51953125 |
| alternate | 41207 | 1351 | 41229.248046875 |
| miss | 41975 | 1376 | 41992.1875 |

## K05436
Source `D0193`, original line61, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

These values are delays from a specific clear, not proven absolute RX boundaries relative to station poll. IRQ latency, RADIO transitions, clear points, and station phase cannot be omitted. I do not pass off this table as proof of poll hitting RX.

## K05437
Source `D0193`, original line63, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Decision on controversial constants

## K05438
Source `D0193`, original line65, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- 9922 at five locations is preserved. Branches 0x6538/0x65a2/0x67de/0x6c42/0x6c5c are associated with clearing/starting/state transitions, rather than directly with the N×period formula. In 0x6c5c, HFCLK is even started and STARTED is awaited before the delay. This does not prove that the constant is independent of cadence, but rules out replacement on the basis of numerical equality alone.
- 10980 at 0x6eac is preserved. The branch is selected by byte 0x20000f69; the zero branch goes to the formula (N+1)×period−25. 10980 indeed corresponds to 360 quanta of divider30.5; the purpose of the mode and all flag writers have not been established. Proof of reachability of this mode during normal streaming is insufficient.
- Period2989 was not applied. Coincidence with 98×30.5 does not mean physical 3000 µs.
- The common divider30.5 was not replaced with an exact RTC quantum: it also participates in timekeeping transitions between modes (for example 0x6adc…0x6b04). A local replacement without auditing these transitions could create a new incompatibility.
- Native −25 is preserved. On the RTC path, this does not guarantee physical compensation of exactly 25 µs after rounding.

## K05439
Source `D0193`, original line71, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The OTA gate was also verified: when 0x20002240=2, initial arming via 0x62e4 does not write a new CC and does not start TIMER0. But re-arming 0x61f4 does not check this gate and sets 12000. This is yet another reason not to infer over-the-air behavior solely from nominal period.

## K05440
Source `D0193`, original line73, SHA256 `89c5e975fc73648e99bde980b0383084b82b33cb81924289cc65b01b2f692a7b`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The byte-by-byte patch/reverse remains exact; candidate SHA256 remains `6a82a89206a609fdce92a3de764882ce53f79f0360b925bebe1ce7853cde6a0d`. This is preservation of the evidentiary base, not confirmation of readiness.
