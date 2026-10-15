# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05281
Source `D0188`, original line33, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Search for embedded plain Cortex-M image

## K05282
Source `D0188`, original line35, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Five binaries station_upgrade,stationservice,libstationopticsservice,libcontroller_service,libpxrcontrollerservice were checked across 4B offsets: SP in 20000000..20020000, Thumb reset in 0..80000, and another 6 vector words zero/Thumb in this range, minimum 4 non-zero handlers. Candidates: 0. This is only a plain vector table heuristic, not decoding of compressed/encrypted/custom-offset firmware. Hashes and exact input sizes are saved in embedded-vector-scan.json.

## K05283
Source `D0188`, original line37, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Public search and boundaries

## K05284
Source `D0188`, original line39, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Searching for exact Swift2.0/bootloader/trackerbin names did not yield a usable primary artifact. New full multi-GB PICO OS archives **were not downloaded and were not unpacked** in this iteration. One cannot write that all vendor packages have been checked. Internet results for the Swift language and Raspberry Pi Pico were filtered out as a different product and are not used as technical sources.

## K05285
Source `D0188`, original line41, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Next justified path

## K05286
Source `D0188`, original line43, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A full PICO OS package inventory with trackerbin/production images is needed, or an exact server implementation of the new diagnostic path. The verified Bootrom/DirectCmd candidates do not provide readback. Static search across all other retained library modules and new OS packages is still possible; its result is currently unknown.

## K05287
Source `D0188`, original line45, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Neither this iteration nor host RSA/container parsing confirms the stock verification/erase order inside the loader. The hypothesis "upon failure, stock is guaranteed to remain" remains unverified. Until boot/recovery/clean-start evidence, the hardware OTA gate remains closed. Controlled RF identification probes were previously separately authorized; this is not permission to flash.

## K05288
Source `D0188`, original line49, SHA256 `1275522104382cb885133fae89d94ef4cdbd27cf501fd1c0cbba2aab1973571a`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

outputs/fw189-boot-search16/: server-candidates.asm,updater-parser.asm,plt.json,container-check.json,embedded-vector-scan.json, symbols/filtered strings/paths by sources. Bootloader emulation was not performed. Production pytest was not rerun: production code/image were not changed. Native disassembly and container assertions are Static evidence.

## K05289
Source `D0189`, original line1, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 17 — full PICO OS packages and the entire local pico4e set

## K05290
Source `D0189`, original line3, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-08. Evidence: Static / downloaded vendor artifacts. No hardware actions: SSH, ADB, RF, reset, erase, and OTA were not performed. Production code, fork image, whitelist, and readiness gate are unchanged.

## K05291
Source `D0189`, original line7, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the full system partition of the PICO OS package 5.13.8, the same FW189 that was already read from the headset was found. All three trackerbin files match outputs/pico4e/trackerbin byte-for-byte. A separate Swift2.0 bootloader was not found along the checked paths. This is not proof of the absence of a bootloader on the tracker, nor grounds to authorize OTA.

## K05292
Source `D0189`, original line9, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The entire available local set outputs/pico4e was also checked: 236 files, 402149538 bytes. This is a complete catalog of the available set, not a confirmation that the entire file system was previously read from the headset or that every executable path was decompiled and analyzed. The user correctly pointed out that this set should be taken into account first: earlier iterations analyzed its individual modules rather than exhaustively all contents.

## K05293
Source `D0189`, original line11, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Sources and downloading

## K05294
Source `D0189`, original line13, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Links were found in https://pico.crx.moe/docs/picoos-research/version-table/ . This is a secondary index used only for URL discovery. The primary data for this iteration is directly the ZIP bytes from the PICO CDN:

## K05295
Source `D0189`, original line19, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

All three archives have HTTP 206 with Accept-Ranges; the last 131072 bytes, Content-Range, EOCD, and full central-directory entries were saved. Each has 41 entries. Archive sizes are respectively 3723661327, 3663144256, 3619789593 bytes. There are no separate ZIP entries named tracker/Swift/bootloader. station_bin contains PhoenixSta app and station_upgrade; they must not be confused with the tracker bootloader.

## K05296
Source `D0189`, original line21, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The ZIP entry system.new.dat.br was completely downloaded **only from 5.13.8**, 3177140997 bytes. The size, exact Content-Range intervals, and CRC32 of the entry were verified. SHA256 of the compressed entry: b1c897dc610f78266364577cf8b3d198f6ba68189b56d5f1d6aa50090e3c0fdb. This is the SHA of an individual entry, not of the entire ZIP. The full ZIP hash/signature was not verified; ZIP CRC provides extraction verification, not cryptographic authenticity of the package.

## K05297
Source `D0189`, original line23, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

metadata, updater-script, and system.transfer.list were also extracted and verified by ZIP CRC. Metadata: pre-device=PICOA8110, oem-state=true, secure_boot_tag=SE, post-build contains 5.13.8. These data characterize the package; it was not installed on the headset for extraction.

## K05298
Source `D0189`, original line25, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Reconstructing system

## K05299
Source `D0189`, original line27, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The Brotli stream was fully decoded with an is_finished() check. transfer.list version4 contains only new/zero/erase; reconstruction created a normal local file, no erase commands were sent to the device. 5676187648 bytes of new data were written, resulting in a logical system.img size of 5704593408 bytes. All new ranges were exhausted exactly, without surplus or shortage of decompressed bytes.

## K05300
Source `D0189`, original line29, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

ext4 magic 0xef53, blocksize4096, inode size256, group descriptor size32 were verified. A custom reader traverses inodes/extents and the directory tree, saving 5991 entries. All required trackerbin files were read via inodes/extents. This is not fsck and not a full check of the contents of all 5991 files: path inventory and target file extraction were performed.

## K05301
Source `D0189`, original line31, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Byte-by-byte comparison of trackerbin

## K05302
Source `D0189`, original line33, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| File | Size | SHA256 | Comparison with local |
|---|---:|---|---|
| Swift2.0_sv1.89_20241213_b189_app.bin | 193385 | 41bd2d24dd9293e7d17c62ab18acd1001d6085da393123a67d226b28d2132e54 | identical=True |
| Swift_hm01_20230707_b97.bin | 140304 | 75e5df6d5f4dffd20f89678e2b65d8afd4bf705ea9d1254eee6dbc9d417b28e7 | identical=True |
| public.pem | 450 | ccc597828f1964bc5adf8664a7e219eb3a79c6799b723e59d45e98518c7edaaf | identical=True |

## K05303
Source `D0189`, original line39, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Thus, the new package did not yield a new tracker image: the FW189 container is still the same 193385-byte file. The previous parsing of the container into two identical app segment96532 remains applicable. Neither this fact nor the public key for host verification reveals the internal bootloader and the erase/validation order.

## K05304
Source `D0189`, original line41, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Searching system paths for trackerbin/Swift bin-hex-elf/nrf/dfu/bootloader yielded three trackerbin files and libbootloader_message.so. The latter library is not recognized as a Swift bootloader: the name in itself is not proof. Hidden/renamed/compressed payloads inside other system files are not ruled out by this search. product/vendor/odm were not unpacked. System 5.12.0 and 5.13.3 was not downloaded: only the ZIP catalog was checked for them.

## K05305
Source `D0189`, original line43, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Verification of saved outputs/pico4e

## K05306
Source `D0189`, original line45, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A full inventory of 236 files was saved. The names and printable strings of all files were inspected according to bootrom/bootloader/trackerbin/readflash/dumpflash/flashread/readmemory/memoryread/dfu/recover. Strings are candidates, not established API semantics. There are false matches in models and general infrastructure.

## K05307
Source `D0189`, original line47, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Three APKs were additionally unpacked logically, and each ZIP entry was inspected by names and selected printable strings:
- ControllerUpgrade.apk: 658 entries, no selected firmware/bootloader names.
- PvrSwift.apk: 1570 entries; classes.dex contains /system/etc/trackerbin; libunity.so contains the string BOOTLOADER. This is not a found Nordic loader, the string does not provide its code.
- OsTeaTracker.apk: 417 entries, no selected firmware/bootloader names.

## K05308
Source `D0189`, original line52, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The plain Cortex-M vector heuristic on all local files checks 4B offsets, SP 20000000..20020000, Thumb handlers < 80000. 4 candidates were found in PhoenixCtrl and PhoenixSta. These are controller/station, not new Swift files. The known FW189 container has payload offset 0x141, so a 4B alignment scan does not replace the already completed dedicated container parsing. Misses of non-standard tables/compression are possible. Nested APK entries were checked by strings/paths; a separate exhaustive vector scan of each decompressed entry was not performed.

## K05309
Source `D0189`, original line54, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## What is closed and what remains

## K05310
Source `D0189`, original line56, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The question of whether trackerbin in system5.13.8 differs from ours is closed: no, all three files are identical. The inventory gap for the available outputs/pico4e and nested APKs is closed. The unknown bootloader, acceptance of the 97320-byte fork, verification/erase order, recovery, and clean-start/stale-ACK hardware conditions remain open.

## K05311
Source `D0189`, original line58, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Next priority if continuing offline: analyze DEX calls around trackerbin file selection and service dependencies; then the contents of vendor/product or other factory/service packages if there is a specific indication of a loader. Reloading the same FW189 provides no benefit. Do not consider the absence of the bootloader name as exhaustive proof.

## K05312
Source `D0189`, original line60, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Reproducibility / artifacts

## K05313
Source `D0189`, original line64, SHA256 `2d1da5ccb80d49e6a48b25acc7105d7ea9b4209eab2c841b7ebcf158ba1b5ad4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

5.13.8/: compressed system, reconstructed system.img, metadata/updater-script/transfer-list, filesystem-inventory.json, firmware-path-candidates.json, tracker-files.json, and extracted trackerbin. Files are saved locally. This is a public OS package, not a private flash dump of the tracker. The fork image was not modified. Pytest was not rerun: the production implementation was not changed; CRC/sizes/transfer/ext4 assertions and byte-by-byte comparisons were performed.

## K05314
Source `D0190`, original line1, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Iteration 18 — vendor/product/odm, APKs, and the limits of offline verification of FW189

## K05315
Source `D0190`, original line3, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-08. Scope: exclusively offline analysis of saved and downloaded files. No SSH/ADB/RF, reset, halt, unlock, erase, or hardware OTA. The user's lack of SWD was taken into account: physical access was not requested. The fork image, production code, whitelist, and OTA readiness gate were not changed.

## K05316
Source `D0190`, original line5, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Main result

## K05317
Source `D0190`, original line7, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The checked vendor/product/odm of the PICO OS package 5.13.8 and the examined APKs did not yield a confirmed Swift2.0/FW189 bootloader, an arbitrary flash reader, or an independent recovery method.

## K05318
Source `D0190`, original line9, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A new, but unsuitable for proof path has been found: BLE OTA in BluetoothPeripheralService. Its Java API contains constants READ_DATA=6 and READ_MEM=8, however, they are not implemented in the inspected command builder and response handler. Compatibility of this BLE protocol with our Swift2.0 has not been established. The names of these constants cannot be passed off as a working capability to read Nordic flash.

## K05319
Source `D0190`, original line11, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Partition acquisition and verification

## K05320
Source `D0190`, original line13, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The same primary CDN archive was used as in17:
https://lf-stone-iot-my.dlpicovr.com/obj/stone-iot-my/5.13.8-202609021949-RELEASE-user-phoenix-b10102-5cc013c385.zip

## K05321
Source `D0190`, original line16, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

vendor.new.dat.br, product.new.dat.br, odm.new.dat.br and three corresponding transfer.list files were extracted from the ZIP. Each GET verifies HTTP206, exact Content-Range, and length; extracted entries were verified against CRC32 from the central directory. CRC is extraction verification, not signature verification of the entire OTA archive. The entire ZIP was not downloaded and its full signature was not verified.

## K05322
Source `D0190`, original line18, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Partition | Compressed entry, bytes | SHA256 compressed entry | ext4 size, bytes | Tree entries / regular files |
|---|---:|---|---:|---|
| vendor |341061425|88fe0c12303ae541572f60618f9d926008832e35ebd8a8bc12aa46375c0a8d99|931987456|2559 /2192|
| product |123622396|aedfd663ec3cf03af890d6d8ed88d8b7b95f06e630cacda267fe9c0c7c891f74|324222976|906 /796|
| odm |257784|5f15a4d8ce6f96f6b04d1083a07e358751549da5e4e96498317a053d093cade8|1032192|9 /6|

## K05323
Source `D0190`, original line24, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Brotli is_finished(), complete exhaustion of new ranges, and decompressed byte count were verified. Written new bytes: vendor927952896, product323051520, odm954368. transfer commands were interpreted only to create the local image; erase/zero in transfer.list are not commands to the tracker.

## K05324
Source `D0190`, original line26, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The reader checks ext4 magic, inode/extents, directory records. Block4096, inode256, descriptor32 in all three. This is an exploratory reader, not fsck. Regular files were read across all2994 paths; SHA256/sizes were recorded. The sum of sizes across paths may include repeated inodes/hard links, therefore it is not equal to the unique partition size.

## K05325
Source `D0190`, original line28, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Search inside partitions

## K05326
Source `D0190`, original line30, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Performed:
1. Path catalog and extraction of firmware/tracker/swift/nrf/bootloader/dfu/station/upgrade candidates.
2. Printable strings of all regular files according to bootrom/bootloader/trackerbin/Swift2.0/readflash/dumpflash/readmemory/flashread/nrf52/nordic/hex.
3. Plain Cortex-M vector heuristic: 4-byte offset, SP20000000..20020000, Thumb reset/handlers below80000, minimum4 non-zero handlers.
4. For ZIP/APK files, each entry and its contents were read; the same strings/vector search was performed. CRC verification of nested entries is provided by zipfile.read.

## K05327
Source `D0190`, original line36, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In vendor/product/odm, vector candidates are **0**. There are CAMERA_ICP/GPU/IPA firmwares and generic Qualcomm objecttracker libraries; their names do not make them Swift firmware. Strings hits on bootloader/readmemory relate, among other things, to Android permissions/SEPolicy/Keymaster, GPU/Chromium/libxml, and BLE test/sniffer infrastructure. Nordic in keyboard layout names is a false candidate. pxr.ndifirmware.update.sh runs w25q_write_bin and works with top_level_bitmap.bin; this is an updater and writing, not a Swift flash getter; the script was not executed.

## K05328
Source `D0190`, original line38, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The heuristic does not rule out renamed, encrypted, or non-standardly packed payloads. One cannot write "no bootloader exists in the entire package".

## K05329
Source `D0190`, original line40, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## APK: provenance and decompilation quality

## K05330
Source `D0190`, original line42, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Six APKs were additionally extracted from system17: BluetoothPeripheralService, ControllerTest_Phoenix_v2, PicoFactoryTest3, ControllerUpgrade, FirmwareUpdate, PvrSwift. Paths/inodes, sizes, and SHAs are saved in system-apk-provenance.json. ControllerUpgrade and PvrSwift match byte-for-byte with outputs/pico4e; the other four add sources to the previously saved set.

## K05331
Source `D0190`, original line44, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

jadx1.5.6 from https://github.com/skylot/jadx/releases/tag/v1.5.6 was used. ZIP SHA545ea2be9c242511bc145755cf4bda2485ade42966e096f8b4d3da2a230e8974 matched the GitHub release asset digest. Standard decompilation: PeripheralService and ControllerTest without errors; PvrSwift10, factory1, FirmwareUpdate1, ControllerUpgrade16 errors. Consequently, these Java trees cannot be considered a complete error-free recovery of all APKs.

## K05332
Source `D0190`, original line46, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

For key McuUpgradeManager and PeripheralOtaManager, a fallback DEX listing was obtained separately; both single-class operations completed done. Key hm01 conditions, command selection, and Binder call were verified against this listing. Errors in other methods are not concealed. The common UpdateManager contains an unrecovered coroutine method (h.i): its behavior is not declared fully proven.

## K05333
Source `D0190`, original line48, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## McuUpgradeManager: this is not our Nordic FW189 updater

## K05334
Source `D0190`, original line50, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Source: jadx-pvrswift/sources/com/pvr/swift/util/McuUpgradeManager.java and McuUpgradeManager.fallback.java.

## K05335
Source `D0190`, original line52, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

getUpgradeBinPath:
- reads fwVersion and isolates the number after SW;
- enumerates /system/etc/trackerbin;
- accepts a name with **exactly4** underscore fields and the second field **hm01**;
- selects a higher b-version;
- startMcuOtaOfFirstTracker calls PeripheralDevice.f4560e.m(device,path) for a device in state2.

## K05336
Source `D0190`, original line59, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Swift_hm01_20230707_b97.bin matches this pattern. Swift2.0_sv1.89_20241213_b189_app.bin has5fields and does not have hm01, so this specific chooser does not select it. This is a conclusion about this particular Java branch, not about all update paths of the headset. The native StationService radio updater was analyzed in16 separately.

## K05337
Source `D0190`, original line61, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

IPeripheral proxy m serializes the device identifier and file path and performs Binder transaction21. The real IPeripheral in BluetoothPeripheralService names21 startMcuOTA. PeripheralService.startMcuOTA checks permissions/OTA busy and delegates to BleCentralConnectManager.startMcuOTA(bluetoothDevice,path), then PeripheralOtaManager.

## K05338
Source `D0190`, original line63, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Neither this Binder call nor state property2 constitutes a memory read or confirmation of a clean reset.

## K05339
Source `D0190`, original line65, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## BLE OTA: why the found READ names do not provide a solution

## K05340
Source `D0190`, original line67, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

PeripheralOtaManager uses service UUID02f00000-0000-0000-0000-00000000fe00, write characteristicff01, notifyff02, and CCCD2902. This is a GATT path different from the known FW189 command1/sub25 radio OTA.

## K05341
Source `D0190`, original line69, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

READ_DATA6, READ_MEM8, WRITE_MEM7, WRITE_DATA5, GET_BASE_ADDR1, PAGE_ERASE3, CHIP_ERASE4, REBOOT9 are defined. But o[TRACKER_SERIAL] implements only1,3,5,7; for6/8 it returns an empty byte array. A separate reboot builder handles9. onOTACommandResponse handles1,3,5, does not return a flash dump.

## K05342
Source `D0190`, original line71, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Important for a potential future executor:** a successful GET_BASE_ADDR response1 with data[2]==4, data[3]==0 automatically enqueues message3 — eraseSwiftTrackerOTAPage. Consequently, one must not launch standard getBaseAddress/startMcuOTA as "just to learn the address": the state machine proceeds with erasure. Nothing of this was executed in this iteration.

## K05343
Source `D0190`, original line73, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Compatibility of service UUID or these opcodes with Swift2.0/FW189 is not proven. The ability to manually construct opcode6/8 for another BLE product is not a verified readback of our tracker. Old b97 does not turn into a Nordic bootloader due to the Swift match in its name.

## K05344
Source `D0190`, original line75, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## FirmwareUpdate and factory APK

## K05345
Source `D0190`, original line77, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

FirmwareUpdate.business.a maps category TRACKER to /system/etc/trackerbin,CONTROLLER with /system/etc/ctlbin. UpdateManager reads assets/firmware/config.json and works with BASE_FW_DIR_/UPDATED_FW_DIR_/FW_HASH_/com.picovr.firmwareupdate.config_json. The built-in config is exactly66B: version1.0, hash_algsha256, hash_info is empty. In the extracted firmware assets there is no new Swift image or loader. This is not proof of the absence of runtime downloaded files in /data; such a private runtime directory was not read during this iteration.

## K05346
Source `D0190`, original line79, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Contents of the selected six APKs were inspected along firmware/bin/hex/pem paths: besides config.json, DebugProbesKt.bin1719B was found — a Kotlin coroutine asset, not an MCU image. Factory/controller Java search yielded no new confirmed Swift flash-reader. The decompilation error counts above limit the negative conclusion; not all JNI/callback paths of factory applications have been exhaustively recovered.

## K05347
Source `D0190`, original line82, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Additional full content scan of system

## K05348
Source `D0190`, original line84, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Following the APK analysis, reading across all 5097 regular paths of system17 was completed, totaling 5972223447 bytes across paths. For each file, the size/SHA256 were saved; ZIP/APK entries were additionally unpacked in memory and checked with strings/vector heuristics. This covers the content of files, not only5991 of tree names. Archive CRCs did not raise exceptions during reading. However, not all APK/JNI paths were decompiled: a content scan is not equivalent to analyzing every function.

## K05349
Source `D0190`, original line86, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

8 vector heuristic matches were obtained. Four relate to the already known PhoenixCtrl/PhoenixSta. Another four are two matches counted once each in APK bytes and DEX entry: PicoFeedback.classes2.dex and FileManager.classes.dex. DEX magic/map were verified, both reside in map region type0x2001(code_item). Identical words begin SP20010228/reset0000000f and contain addresses1/3/17; these are matches in the Dalvik code area, not a confirmed Cortex-M loader. Context/offset/map are saved in dex-vector-classification.json. We do not pass off the number8 as eight found firmwares.

## K05350
Source `D0190`, original line88, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

System strings again show station IN618 Bootrom API and known NRF52832 updater classes. No new confirmed Swift readback was found. System plain vector search retains the restriction of 4B alignment: the known Swift container offset0x141 was analyzed separately in16/17, its app is not ruled out by negative heuristics.

## K05351
Source `D0190`, original line90, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Additional artifacts: scan_system_content.py, system-content-inventory.json, system-string-hits.json, system-vector-candidates.json, system-archive-inventory.json, dex-vector-classification.json.

## K05352
Source `D0190`, original line92, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Firmware state and decisions

## K05353
Source `D0190`, original line94, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Stock app96532 SHA477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89.
Fork97320 SHA395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f.
Both rechecked; fork is unchanged. Production pytest was not run because the implementation did not change. Verifications here: HTTP ranges, ZIP CRC, SHA, Brotli/transfer/ext4 assertions, reading files/archives, static DEX/native-format search; this is not a hardware boot/OTA/RF verification.

## K05354
Source `D0190`, original line98, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The remaining questions are the same: loader validation/erase order, acceptance of the enlarged 97320-byte app, independent recovery, and clean start of OTA/stale ACK. File verification narrowed down the candidate space, but did not remove these limitations. The presence of stock.bin still does not guarantee rollback after loss of the working application/radio.

## K05355
Source `D0190`, original line104, SHA256 `22cf91cca1ccf402190aed07016b390ea02d3139f20b77eb57d1e4395e349f2d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

5.13.8/: compressed partitions, transfer.list, reconstructed ext4 images, inventories, target extracts. All local. Contents of the public package are not equal to the tracker's private flash dump.

## K05356
Source `D0191`, original line1, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# FW189 — iteration 19: JNI, diagnostic packets and unanalyzed DEX branches

## K05357
Source `D0191`, original line3, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-08. Offline only. Author: Codex.

## K05358
Source `D0191`, original line7, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Two planned directions were verified: native/JNI of factory applications and selected decompilation errors of updaters. Among the analyzed functions, no supported method was found for reading flash/bootloader of Swift2.0/FW189 or recovering the tracker after loss of application.

## K05359
Source `D0191`, original line9, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This is a limited negative result for the functions listed below, not proof of the absence of any hidden command in the product. It does not lift the OTA prohibition. Not a single diagnostic function was called on the headset or tracker.

## K05360
Source `D0191`, original line11, SHA256 `595ea0134da60629b855a956b33b9b14d55213701fe437af6ff9d9b9a2a0cf3d`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

New specific results:
