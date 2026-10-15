# Offline helper tools

These new standard-library utilities contain no private project implementation, vendor code or device interface. Python3.12+ is recommended. They do not download assets, inspect devices, start services, flash, pair or change a runtime.

1. Verify an independently obtained exact vendor container, without writing an image:
   `python tools/extract_stock.py <PRIVATE_FW189_CONTAINER>`
2. Extract its identical stock application to a NEW PRIVATE file outside this package:
   `python tools/extract_stock.py <PRIVATE_FW189_CONTAINER> --output <PRIVATE_STOCK_APPLICATION>`
3. Verify diagnostic10 reconstruction and reverse-to-stock in memory, without output:
   `python tools/firmware_diff.py --input <PRIVATE_STOCK_APPLICATION> --diff firmware-diffs/diagnostic10-installed.diff.json`
4. Only if independently authorized, reconstruct a NEW PRIVATE file outside this package by adding `--output <PRIVATE_RESULT>`. This still performs no installation. A reverse operation takes the exact patched file plus `--reverse`.
5. Audit package contents and hashes:
   `python tools/verify_package.py`
6. Retrieve only relevant metadata instead of loading a large catalog into an AI context:
   `python tools/query_catalog.py documents --phase 393`
   `python tools/query_catalog.py knowledge --contains optics --limit 20`
   `python tools/query_catalog.py assets --contains libAlgSwiftBodyPose`
   `python tools/query_catalog.py source --contains measurement_clock`

The query result reports truncation explicitly; use `--limit 0` to return all matches. It reads package metadata only, not private original assets.

Source overwrite, mismatched stock/version/hash and output inside the handoff are refused. Changing a diff invalidates the package manifest. Reconstruction is not approval for OTA or a claim that the loader/recovery path is safe.
