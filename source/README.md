# Project implementation source supplement

This folder contains the selected project-authored implementation, not vendor SDK code or a firmware distribution. Read [the exact file and exclusion manifest](../verification/SOURCE_SUPPLEMENT.json). Each file has its original path/hash, bundled hash and any sanitization changes. The historical [source index](../catalog/SOURCE_CODE_INDEX.json) remains broader than this supplement.

Included components: native dashboard and monitors, Python diagnostics and transport, camera/timing/geometry/fusion, pose output and native driver source, Roy SRAM game/service C modules, loader/runtime and recovery logic, offline patch-generation scripts, requirements and two publication build scripts. Their presence is not proof of working full-body tracking or authorization to run device-facing tools.

## Roy variants

- `historical-original210/roy_ram_station_v4.c` is the protected historical original210 source. Do not confuse the active-tree copy with this frozen source.
- `pc/radio/ram/roy_ram_station_v4.c` is the active-tree game variant; other station files are distinct research versions.
- `pc/radio/ram/roy_ram_station_service27.c`, `service36.c` and `service38.c` preserve the service variants. They do not share an interchangeable validation status.
- `pc/radio/ram/build_station.py` selects service27 for ABI4 `--service`; it does NOT automatically build service36 or service38. Review the chosen source and its recorded build recipe before selecting a variant.
- Absolute native-call addresses, register definitions, stock hashes and memory layouts are compatibility metadata. The matching stock Roy image is not included. See [Roy constraints](../docs/ROY_AND_COEXISTENCE.md) and [saved state](../docs/CURRENT_STATE.md).

## Deliberate export changes

`constellation.py` retains our classes/math but removes the embedded factory LED coordinates and normals. Constructing the tracker model without explicit recipient-supplied coordinates and normals raises an error. Never replace this with guessed geometry.

Literal private serials and LAN addresses are replaced by `OWNER_SUPPLIED_PRIVATE_VALUE_REQUIRED`, and known chip literals by `OWNER_PRIVATE_ID_REQUIRED`; see the manifest for exact files/counts. This is deliberately non-working configuration. Device comparisons remain present. Configure and review your own exact identity and endpoint before any authorized operation. Hashes of these exports differ from the private originals.

The spline camera model also has no bundled calibration defaults: supply explicit values. Three older identity-bound C candidates (`roy_ram_rx.c`, `roy_ram_station_link.c`, `roy_ram_station_three.c`) retain authored logic but have redacted identity arrays and an unconditional `#error`. They require a reviewed configuration refactor before compilation; do not simply remove the error and use zero identities. This does not affect the separate v4 game/service source files.

All files were normalized to UTF-8 and LF; compare original and bundled hashes through the manifest rather than assuming byte equality. Historical tests are held out where fixtures have not been reviewed individually. Captured packets, raw logs, factory calibration, native libraries, vendor headers/code and compiled outputs are absent. Exclusions are explicit, not lost research.

## Build and continuation

Copy this source folder into a separate development checkout before installing dependencies, fetching upstream code or building. Keep this verified handoff unchanged. Follow [quick setup](../QUICK_SETUP.md) and [build notes](../docs/SOFTWARE_AND_BUILD.md).

`pc/requirements.txt` records broad historical minimums, not a reproducible dependency lock. Optional packages and system libraries remain component-specific. Native UI additionally includes `json-c/json.h` and needs json-c development headers/link flags. The historical `scripts/build_frame_app.sh` lacks explicit json-c pkg-config flags; review and add those flags in your development checkout if required by your toolchain. It fetches the pinned overlay dependency, so do not execute it as an offline verification command. Third-party notices must accompany separately obtained dependencies.

No clean-machine full build has been demonstrated. Configuration files, profile approvals, clocks, private calibration and recorded datasets are obtained/prepared separately. Offline patch generators write images only in your private development area. Packaging them does not waive the firmware-only-as-diff rule for this handoff.

This export applies the user's authorization to include project-authored source. The publication licenses project authored material under GPL version 3 or later, with third party and artwork exceptions in NOTICE.md. Review scope and remaining provenance caveats are recorded in [source review](../verification/SOURCE_REVIEW.md).
