# Frozen handoff — 2026-10-15

The documentation and selected source export are complete for this handoff revision. Start at [quick setup](QUICK_SETUP.md), then [source instructions](source/README.md). The [package report](verification/PACKAGE_VERIFICATION.json) contains the final manifest hash; independently verify with `python tools/verify_package.py` before use.

Scope: 768 historical Markdown versions, 1680 translated narrative blocks, 24 exact stock-relative firmware differences, and 135 selected source files. The source export includes game and service Roy SRAM modules with distinct version/status information. No vendor firmware image, native library, decompilation, SDK, private calibration or raw recording is included.

Review: Grok independently inspected Roy, native UI and optics source provenance; findings were integrated and export changes are documented in [source review](verification/SOURCE_REVIEW.md) and [file manifest](verification/SOURCE_SUPPLEMENT.json). Earlier component reviews and corrections remain in [review disposition](verification/REVIEW_DISPOSITION.md).

Validation: package hashes, local links, allowed contents, 99 Python source syntax checks, and isolated fail-closed calibration-default checks. See [source checks](verification/SOURCE_EXPORT_CHECKS.json). Full dependency imports, a clean-machine full build and device continuation have not been demonstrated. Source exclusions and unresolved external inputs are explicit prerequisites, not completed tests.

Preserve this folder as a reference. Make development changes in a separate checkout. New findings should create a new dated handoff revision and retain this revision's manifest. No upload, device operation or firmware installation was performed to create this revision.
