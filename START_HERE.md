
## Included source supplement — 2026-10-15

The user subsequently authorized inclusion of project-authored implementation source. Read [source supplement](source/README.md) and its exact [file/exclusion manifest](verification/SOURCE_SUPPLEMENT.json). This supersedes earlier blanket statements that no implementation is bundled. Some private inputs, captured fixtures and third-party dependencies remain excluded; no clean-machine full build is claimed. Work in a separate development checkout, not this integrity-checked folder.

# PICO Swift / Steam Frame: project-wide handoff

Prepared on2026-10-15 from the local research trees. The latest saved project reports are dated2026-10-11; newer saved optical probe artifacts are incorporated explicitly. No device was queried during packaging. This handoff does not establish the current live installation or authorize another hardware attempt.

## Read order

New contributors: follow [QUICK_SETUP.md](QUICK_SETUP.md) for required source/assets, environment preparation, offline checks and explicit readiness blockers.

1. AGENTS.md and docs/CURRENT_STATE.md.
2. docs/RESEARCH_TREE.md, docs/SUPERSESSION.md and docs/EVIDENCE_AND_SAFETY.md.
3. The relevant technical chapters: RADIO_PROTOCOL, ROY_AND_COEXISTENCE, FIRMWARE, OPTICS, IMU_AND_FUSION, BODY_AND_STEAMVR, INTERFACES, SOFTWARE_AND_BUILD and EXPERIMENT_RECIPES.
4. research/INDEX.md and catalog/RESEARCH_DOCUMENTS.json: all discovered Markdown versions, their source hashes, historical role and knowledge references.
5. docs/ACQUISITION.md and catalog/ASSET_REGISTER.json: excluded assets, exact provenance and procedures to obtain your own copy.
6. docs/CONTINUATION.md and docs/COVERAGE.md before planning implementation.

The target is standalone PICO Swift trackers on Steam Frame through its onboard Roy radio, eventually waist/left-leg/right-leg roles with both native controllers. An external receiver or a PICO headset in the live pose path does not meet this goal. Full-body tracking is not complete.

## Distribution boundary

This package contains original explanatory documents, sanitized research prose and metadata, and sparse stock-relative diffs. It contains no stock or patched firmware image, ELF, vendor library, SDK source, decompilation, disassembly, raw calibration, camera recording, RF log or private credential. The earlier2026-10-08 archive is PRIVATE and is not a distributable predecessor to zip or publish.

The24 firmware differences are version-bound descriptions of authored changes, not24 releases approved for hardware. The protected diagnostic10 difference reconstructs the exact known image only when the recipient supplies the exact stock application. Unchanged stock bytes are never embedded. Tools operate offline and reject an output path inside this package.

## Source access

Local source labels use `pico`, `publication`, `sdk-pc`, `archive-20261008`, `quick-bohr`, and `brain`. Their original file paths and hashes are catalog entries, not bundled files. A recipient without the private first-party workspace must obtain its rights-cleared source separately from the project owner; the public source license and exclusions are described in NOTICE.md. Third-party acquisition instructions are separate from first-party source transfer.

Check verification/PACKAGE_VERIFICATION.json for actual file coverage, allowed formats, link checking, identity scanning, translation coverage and firmware-diff verification. A manifest cannot adjudicate legal rights; it records the concrete exclusion controls applied to this package.

For efficient AI retrieval use `tools/query_catalog.py` and read the matched source cards/ledger entries, rather than inserting the entire large asset register into context. Canonical current-state chapters have priority over historical ledger instructions.
