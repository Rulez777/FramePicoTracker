
## Included source supplement — 2026-10-15

The user subsequently authorized inclusion of project-authored implementation source. Read [source supplement](source/README.md) and its exact [file/exclusion manifest](verification/SOURCE_SUPPLEMENT.json). This supersedes earlier blanket statements that no implementation is bundled. Some private inputs, captured fixtures and third-party dependencies remain excluded; no clean-machine full build is claimed. Work in a separate development checkout, not this integrity-checked folder.

# Recipient quick setup

This guide takes a new contributor from this documentation folder to an explicit readiness decision. It is not a turnkey source release. No clean-machine end-to-end build or hardware continuation by an independent recipient has been demonstrated.

## 1. Read the current boundary

Read [AGENTS.md](AGENTS.md), [current state](docs/CURRENT_STATE.md), [research tree](docs/RESEARCH_TREE.md), [supersession rules](docs/SUPERSESSION.md), and [continuation plan](docs/CONTINUATION.md). Historical experiments and permissions are not current instructions to execute them. The saved state is not a fresh observation of the devices.

The goal is standalone PICO Swift tracking through Steam Frame's onboard Roy. Stable full-body tracking is unfinished. Preserve diagnostic10 and original210. Recent RF-only success does not establish IMU delivery or valid optical poses; historical sparse pilots remain relevant evidence.

## 2. Verify the folder without hardware

Install Python 3.12 or newer. On Windows, check that `python --version` resolves to the installed interpreter, not the Microsoft Store alias. If needed, use `py -3.12` or the full path to your installed interpreter in every command below. The bundled offline utilities use only its standard library. From this folder on PowerShell:

```powershell
python --version
python tools/verify_package.py
python tools/query_catalog.py documents --phase 393
python tools/query_catalog.py knowledge --contains optics --limit 20
python tools/query_catalog.py source --contains measurement_clock
```

On Linux, use `python3` in place of `python`. Require a successful verifier result and inspect [verification report](verification/PACKAGE_VERIFICATION.json). A successful audit establishes package integrity and recorded coverage, not research correctness, licensing clearance, or recipient readiness. No device or network connection is needed for these commands.

## 3. Obtain the missing implementation and inputs

| Required item | Obtain and validate | Readiness consequence if absent |
| --- | --- | --- |
| First-party implementation and build files | Start with the bundled `source/` supplement and its manifest. Request any missing rights-cleared files from the owner. Match relevant paths and hashes against [source index](catalog/SOURCE_CODE_INDEX.json). Source labels and original paths are provenance, not download links. Exported source may be sanitized; use its bundled hash. This publication supplies selected source under GPL version 3 or later; see NOTICE.md for scope and remaining omissions. | BLOCKED for building or modifying the existing implementation. Documentation study and a separately scoped reimplementation remain possible. |
| Matching PICO and Frame assets | Follow [acquisition instructions](docs/ACQUISITION.md) and [asset register](catalog/ASSET_REGISTER.json). Obtain permitted copies from official distributions or your own authorized devices; check exact versions and available hashes. | BLOCKED for any dependent reproduction. Do not substitute a similarly named version. |
| Tracker and Frame calibration | Obtain your own authorized exports for the exact devices. Follow the calibration format and camera/clock requirements in the acquisition and optics chapters. Calibration and identifiers are intentionally absent. | BLOCKED for calibrated pose reproduction. Another device's calibration is not a replacement. |
| Historical recordings or logs, where a claim needs replay | Request authorized access separately or collect a new bounded dataset under a separately approved procedure. The catalog records omitted primary assets. | Historical descriptions can be studied, but exact replay remains unverified. |
| Development host and target access | Arrange the actual host, Steam Frame, PICO tracker, recovery resources and permitted target access for the selected task. | Offline work only until target prerequisites and explicit operation approval exist. |

Keep acquired binaries, recordings, calibration and private source OUTSIDE this handoff. Do not distribute the older private archive as a shortcut. A hash entry does not grant redistribution rights. Record mismatches and unavailable inputs rather than silently upgrading them.

## 4. Prepare the development environment

Follow [software and build details](docs/SOFTWARE_AND_BUILD.md) for the component you received. Use its actual requirements, lockfiles and build instructions; the included `source/pc/requirements.txt` has broad minimums, not locked versions. No universal full installation has been validated.

For Python work, first change directory to the separate received development checkout; do not create the environment inside this handoff. Replace the example checkout path below with your actual path. Create an isolated environment:

```powershell
Set-Location 'C:\Work\PICO-development'
python -m venv .venv
.\.venv\Scripts\python.exe -m pip --version
$env:PYTHONUTF8 = '1'
$env:PYTHONPATH = Join-Path (Get-Location) 'pc'
```

Linux equivalent, after changing to the received checkout: `python3 -m venv .venv`, then `.venv/bin/python -m pip --version`; use `export PYTHONUTF8=1` and `export PYTHONPATH="$PWD/pc"` for the historical Python layout. Confirm the received tree actually has that layout. Install the received component's pinned dependencies into that environment. Depending on the component, the historical stack uses numpy, openvr, python-osc and pytest, with optional image, video, binary-analysis and compression libraries. These are component requirements, not a claim that installing their latest versions reproduces the project.

Native Frame components have a distinct Linux ARM64 build/runtime environment: C++17, the matching OpenVR headers/runtime, and component-specific pkg-config, Cairo, libcurl, json-c, EGL and desktop OpenGL dependencies. A Windows Python environment alone does not satisfy it. Use the project authored overlay wrapper and separately supplied official OpenVR headers as described in the build chapter. No frame-mic-tuner checkout is needed. Do not start services, install overlays or run device-facing scripts as part of initial setup.

Record the received source revision, file hashes, host/target architecture, compiler/interpreter and dependency versions. Run the received component's offline checks first; record checks that cannot run because inputs are missing.

Return to this handoff folder before using its `tools/` commands. Firmware acquisition from an authorized Android device additionally requires Android platform-tools; installing them is not permission to connect or pull files. No exact universal native build or vendor OTA unpacking recipe has been clean-machine validated here. Obtain the matching build scripts, dependency pins and unpacker instructions with the source transfer; missing ones are unresolved reproduction prerequisites.

Read [review corrections and limitations](verification/REVIEW_DISPOSITION.md) before interpreting historical firmware arithmetic or review claims.

## 5. Optional firmware research, entirely offline

Firmware is unnecessary for simply reading the optical research. For firmware continuation, obtain the exact authorized stock container described in [firmware provenance](docs/FIRMWARE.md). Check it without extraction:

```text
python tools/extract_stock.py <PRIVATE_FW189_CONTAINER>
```

The placeholders above and below must be replaced with real paths; they are not literal shell syntax. If extraction is needed, choose a new private output path outside this folder:

```text
python tools/extract_stock.py <PRIVATE_FW189_CONTAINER> --output <PRIVATE_STOCK_APPLICATION>
python tools/firmware_diff.py --input <PRIVATE_STOCK_APPLICATION> --diff firmware-diffs/diagnostic10-installed.diff.json
```

The final command validates reconstruction and reversal in memory, without writing a patched image. Stock mismatch is a stop condition. See [offline tool instructions](tools/README.md) and [diff lineage](firmware-diffs/LINEAGE.md). These checks do not authorize installation or demonstrate recovery safety.

## 6. Resume one evidence-backed task

Read [interfaces](docs/INTERFACES.md), [experiment recipes](docs/EXPERIMENT_RECIPES.md), and every relevant source card and ledger entry found through [research index](research/INDEX.md). Keep historical negative results and abandoned hypotheses visible. Source cards and English prose preserve discovery paths; unavailable primary measurements cannot be recreated by a summary.

The prepared longer-exposure optical probe is a candidate, not a completed result. Before any device operation, establish fresh device identity, installation, mode/profile, exact source/image hashes, a bounded procedure, recovery/restoration and direct human readiness. Preserve optical identity, age, covariance, IMU and controller guards. Do not convert transport replies, a compiled candidate or a model fit into a valid-pose claim.

## Recipient acceptance record

Create a new English research entry using [the template](templates/RESEARCH_ENTRY.md) if present, or follow [continuation guidance](docs/CONTINUATION.md). Record these statuses with evidence:

1. Package integrity: PASS or FAIL, with manifest hash and verifier output.
2. Required source: RECEIVED AND MATCHED or BLOCKED, listing exact missing/mismatched files.
3. Required vendor assets and private calibration: VERIFIED or BLOCKED for the chosen task.
4. Offline build/checks: PASS, FAIL or NOT RUN, with the actual commands and versions.
5. Device work: NOT AUTHORIZED until a separate concrete procedure is approved.

The handoff is sufficient to understand and plan continuation. Building the existing project additionally requires the excluded implementation and task-specific assets. Reproducing historical experiments additionally requires their primary inputs or a newly authorized capture. Do not mark those stages complete merely because the documentation verifier passes.
