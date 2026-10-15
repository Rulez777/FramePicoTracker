
## Included source supplement — 2026-10-15

The user subsequently authorized inclusion of project-authored implementation source. Read [source supplement](../source/README.md) and its exact [file/exclusion manifest](../verification/SOURCE_SUPPLEMENT.json). This supersedes earlier blanket statements that no implementation is bundled. Some private inputs, captured fixtures and third-party dependencies remain excluded; no clean-machine full build is claimed. Work in a separate development checkout, not this integrity-checked folder.

# Independently obtaining excluded materials

This package does not supply vendor data or grant access to someone else's device. Use your own device, a vendor-authorized backup/export, or a lawfully obtained official package. If a file is inaccessible under your authorized access, stop and obtain the appropriate export/permission; this handoff does not provide an access-control bypass. Never erase/unlock/reset hardware merely to obtain a reference.

## A. Exact Swift FW189 reference

Preferred source paths recorded on the owned PICO system are `/system/etc/trackerbin/Swift2.0_sv1.89_20241213_b189_app.bin` and the matching official OS package's system partition. With an already authorized, explicitly selected ADB serial and readable file, an owner can copy that file using `adb -s <OWN_DEVICE_SERIAL> pull <SOURCE_PATH> <PRIVATE_OUTPUT>`. This is a documented future recipe, not an action performed during packaging. Do not use an automatic root/recovery/flash script.

Exact container precondition:193385bytes,SHA256`41bd2d24dd9293e7d17c62ab18acd1001d6085da393123a67d226b28d2132e54`. The321byte header is followed by two identical96532byte applications. Extract bytes321..96852 and compare to bytes96853..193384, using zero-based end-exclusive slices321:96853 and96853:193385. Both must match SHA256`477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89`. Load the resulting application locally atVA0x1000. Header/container length alone is not enough.

An alternate historically observed official PICO CDN URL was:

`https://lf-stone-iot-my.dlpicovr.com/obj/stone-iot-my/5.13.8-202609021949-RELEASE-user-phoenix-b10102-5cc013c385.zip`

This is an October8 recorded acquisition URL, not a claim that availability, signatures or user access were reverified on October15. Historical ZIP total3723661327bytes; compressed system.new.dat.br entry3177140997bytes,SHA256`b1c897dc610f78266364577cf8b3d198f6ba68189b56d5f1d6aa50090e3c0fdb`. This SHA identifies that entry, NOT the whole ZIP. HTTP Range/ZIP CRC validation was performed historically; full ZIP cryptographic authenticity was not established by CRC.

To reproduce offline: obtain system.new.dat.br and system.transfer.list plus metadata from your own complete package; check ZIP CRC/length and the known entry SHA; Brotli-decompress fully; apply version4 transfer-list new/zero/erase ranges to a LOCAL regular file using4096byte blocks; verify every decompressed byte/range consumed, then read ext4 privately and extract trackerbin. The local reconstruction's erase token is not a device erase command. Historical logical system.img size5704593408bytes. Keep reconstructed partitions and extracted binaries outside this handoff.

Older recorded package versions5.13.3 and5.12.0 were initially central-directory inventories; do not claim their whole systems were decoded. Later vendor/product/odm and APK audits are separate evidence. Asset/source records identify their exact coverage.

## B. Roy stock firmware and Valve runtime

Obtain your own exact Roy stock reference using a vendor-supported developer export, authorized existing backup or matching official update/development tooling. Required audited hash is`0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50`,237920bytes. A different hash is a different firmware-specific audit and invalidates SRAM addresses, hooks and restoration assumptions.

No exact public product-version/download mapping for this stock hash is established in the inspected metadata. Do not substitute a similarly named Nordic image or infer compatibility from SoC family. A recipient unable to obtain the matching reference must re-audit their own build; copying old addresses is not continuation.

Use the locally installed SteamVR/OpenVR/XRService runtime supplied with your Frame/SteamOS setup, recording exact binary hash/build and executable identity. Do not distribute XRService, Vulkan/OpenVR runtime libraries or private calibration through this package. Historical runtime paths include `/opt/steamvr/drivers/cv/bin/linuxarm64/XRService`; validate your actual installation rather than assuming the path/build.

## C. PICO station, native optics and body libraries

Acquire from your own matching PICO OS installation/package. Recorded station FW includes PhoenixSta2.96; exact native library hashes and source aliases appear in the asset register or research records. Relevant basenames include libstationopticsservice.so, libpvrtrackingservice.so, libpico_controller_tracking.pxr.so, libAlgSwiftTracking and libAlgSwiftBodyPose.so. Multiple ARM/AArch64 variants exist: check ELF machine/class, build identity and symbols before comparing addresses.

Recorded optical library hash:`749b655313ac161ec0f8a7b081ec33bef8f3d5d04c271e9f11b066c87d30bf60`; body library5311128bytes,hash`ef824ded874f2aa8ab2bced03bbaf4faec76ec3444ddee8662381c87d4c92c99`. Treat these as version identifiers, not runtime portability proof. Obtain dependency libraries/models through their authorized distributions. Android/SNPE/bytenn/EGL/GLES availability and ABI are independent blockers on Frame.

For local analysis use Ghidra/Capstone/pyelftools against your acquired file. Raw Swift applications use base0x1000 and Cortex-M Thumb/M-class; ELF libraries use ELF load mappings. Preserve literal pools/data boundaries, symbols, called targets and the exact binary SHA. Keep all decompilation/disassembly local. This package contains findings and procedure only; it does not contain native listings or claim that a call-graph scan proves all indirect paths.

## D. Calibration and real input data

Export your own tracker factory LED geometry keyed to the actual tracker identity and your own Frame factory/active calibration and head/device configuration. Preserve source schema/units/serial bindings privately. Do not reuse another unit's calibration or IDs. Active flattened camera extrinsics may differ from raw factory parent-relative transforms. Record per-camera dimensions, KB intrinsics, transforms and same-exposure HEAD pose without publishing the raw tables.

The exact historical PICO calibration source was `/data/stationservice/calibration_data.json`, captured privately as outputs/pico-calibration-root.json. It contains a calibration_data array; select exactly one record by its sn value, never JSON position or presumed physical slot. Strictly base64-decode its calibration field. The investigated660byte blob has little-endian marker values1and60 at bytes204..211 and24little-endian float32LED coordinates at offset212, interpreted as8x3 under the researched metre convention. Validate finite values and plausible magnitude≤0.1m; this is the researched LED layout, not a complete parser or certified IMU calibration. Five historical records included older units, so record count is not proof of current presence. No base64 content, decoded blob, serial or coordinate is included here.

Ordinary adbd access did not have the same privileges as the earlier authorized service context. If your own file is not normally readable, obtain a vendor-authorized calibration export/service backup. This package does not include the historical privilege script or instruct a recipient to bypass permissions. A successful Android root shell would not itself expose Nordic MCU RAM at its address or authorize guessed SPI/memory commands.

For Frame obtain both the factory camera description and the exact recording/profile's active calibration plus device head/CAD configuration through the owned developer export. The receiver expects active cameras entries with unique sourceCamera, cameraModelkb, dimensions, intrinsicfx/fy/cx/cy/k1..k4 and extrinsic unitsmm. Factory serial binding and same-exposure head transforms are separate requirements. An exact publicly downloadable calibration URL is neither established nor appropriate: these are per-device inputs.

Create a fresh bounded native tracking recording through the documented owned-device developer interface after the exact probe and human readiness are approved. Save H264/raw packet metadata, exact decoded timestamps, exposure, current-boot RAW/MONO/BOOTTIME samples and bracketing HMD poses. Historical API paths include localhost18741 status/capture/dataset endpoints; inspect the current rights-cleared implementation before invoking POST/capture because it can own radio/camera resources. A GET-like status check is not equivalent to a recording operation.

The native tracking-recording start/stop interface replaced an earlier manual-recording limitation; do not infer the current command from the first API note alone. Never reuse someone else's recording as current device presence. Camera-only probes must explicitly exclude Roy/RF/LED/OTA/IMU and still perform actual camera/watcher cleanup.

## E. First-party development source

The full private workspace remains excluded. This publication includes 135 selected project authored source exports under GPL version 3 or later, with scope and exceptions in NOTICE.md. Start with `source/README.md` and `verification/SOURCE_SUPPLEMENT.json`; compare bundled hashes for the exported files and original hashes only for separately received originals. The older separate code publication snapshot is historical and must not override this repository's current status or license. Request missing rights cleared files separately. The asset catalog lists exact relative locations and known hashes so missing inputs are explicit.

If access to the first-party source cannot be granted, reimplement the documented interfaces/algorithms with independently acquired inputs and new tests. Mark that implementation as a new port, not byte-identical continuation. Private primary recordings not distributed here must be reacquired to reproduce measured results independently.
