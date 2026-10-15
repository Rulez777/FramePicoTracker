# Exact-version firmware changes without distributing images

## Stock reference

PICO Swift2.0 FW189 file name: `Swift2.0_sv1.89_20241213_b189_app.bin`. The vendor container is193385bytes, SHA256`41bd2d24dd9293e7d17c62ab18acd1001d6085da393123a67d226b28d2132e54`. A321byte(0x141) header precedes two identical96532byte application segments. The application SHA256 is`477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89`, load baseVA0x1000.96532 decimal is0x17914; the end-exclusive VA is0x18914. Do not disassemble the vendor container as if offset0 were application code.

No stock container/application, patched application, assembly listing, linker output, shared library or decompiled file is included. Obtain the exact stock privately as described in ACQUISITION.md.

## Installed diagnostic10

The exact recorded installed image is98868bytes, SHA256`040b304d33e200785bb04ad4b2c0d8730f75d97e4798691530ac2d79b247376d`. Appended section beginsVA0x18918 after four alignment bytes;2332 authored payload bytes plus4padding bytes produce2336bytes total expansion. The sparse comparator independently finds90 changed stock-position bytes,22 contiguous changed runs. The historical patch manifest describes24 logical edit sites: these counts measure different things and must not be conflated.

Changes include conditional stock/Frame radio timing selectors, startup mode handling, a mode-query/switch protocol, deferred mode commit after native receive completion, heap/state reservation, and event observers.32bytes are reserved at0x20005150..0x2000516f, reducing the native heap512→480. Active mode and pending mode are distinct. Event sequence and five ordered timestamps are stronger evidence than isolated8bit summary counters.

The product version stays189. Mode query is identity/nonce/schema-bound, not a version-string test. Default stock behavior and early boot-release gestures are documented, but physical button timing and power-cycle behavior must be proved for the admitted image. Nominal startup release100..149counts is not a universal1second hardware guarantee.

Native code paths outside the hooks remain stock by design; exact equality must be checked for the path under investigation. ACK is not native mode commit. Commit requires native pending/N/RADIO-idle conditions; host must observe a fresh reply with actual applied mode/N/pending state. Service mode0 is used for separately authorized update operations; normal Game mode1 needs continuously refreshed finite proof.

## Historical branches and hazards

The catalog includes grid-only and dual-mode generations, rejected fallthrough/unaligned/u8-counter candidates, revisions07–10 and the uninstalled candidate11 short-producer branch. Every distinct reconstructed image is tied to its hash and source aliases. Except for the saved installed baseline, candidates remain historical/offline and must not be treated as approved hardware releases.

Stale pending OTA workers can ACK after a fresh queue barrier. Empty queue, new nonce and reconnect alone did not prove clean application state. Prepare ACK, block/tag generation, zero-length sentinel,24byte blocks, CRC8/MAXIM and transfer/activation are distinct stages. The reflected CRC8 polynomial is8C; check string123456789 givesA1. A transfer can finish while boot/recovery remains unproved.

Revision07 initially failed during transfer, later service-specific transfer succeeded; revision09 and diagnostic10 introduced additional admission/observer work.189 completed diagnostic10 transfer;191 supplied later activation evidence. Older manifests intentionally preserve offline flags such as approved_for_otafalse; do not rewrite them as current blanket permission or repeat OTA because of historical flags.

Recovery searches in PICO OS/APKs/system/vendor/product/odm and native commands had limited coverage. Station/controller boot/download paths are not a verified Swift recovery loader. b97 is not assumed compatible. GET_BASE_ADDR in the investigated BLE updater queues PAGE_ERASE; log getters can write/advance cursors. Do not call them read-only dump APIs. Read-only SWD requires audited no-unlock/no-erase/no-reset/no-halt hooks and checked power-request release; an OpenOCD reset_config alone is insufficient.

## Difference files and offline use

`firmware-diffs/catalog.json` lists24 unique sparse diffs. Each records exact stock and result lengths/hashes, changed positions only, appended authored difference only, and source aliases. No unchanged stock image data is included. Both forward reconstruction and reverse-to-stock were executed against local originals during packaging.

Use `tools/firmware_diff.py` only with a lawfully obtained exact stock or exact patched input, and put any reconstructed image outside this package. The utility refuses mismatched hashes and package-contained output. Its verify operation reconstructs in memory and writes no image. This is offline reproducibility, not OTA tooling or device readiness.

## Reimplementing from source

Obtain the rights-cleared first-party builder and authored source from the owner, verify the catalog hashes, compile Cortex-M4 Thumb assembly with the recorded Clang/LLD toolchain, extract the appended section, apply the exact stock-relative hooks and compare the final SHA. The diagnostic10 recorded compiler was LLVM18.1.6 from the Zig bootstrap distribution. A different compiler may produce a different image; only exact hash establishes identity. This handoff does not assign MIT/Apache rights to unknown code or claim binary provenance alone proves ownership.
