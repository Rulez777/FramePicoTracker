# Firmware Image Builder

Open `Build-FwImage.cmd`. Select **Download and build** to download the source in the active manifest, or **Build from local file** to supply a copy. Choose where to save the output. The included manifest builds PICO Alpha 1.3. In the handoff, the default save location is the Frame OTA outputs folder. Build before running the Frame installer. On Steam Frame, Roy preparation runs separately when a session starts.

## Use your own manifest

Copy `manifest.json` and edit its fields. Select the new file with **Choose manifest**. A relative `patchFile` is resolved beside the manifest.

| Field | Value |
| --- | --- |
| `id` | Name shown in the launcher |
| `sourceUrl` | HTTPS URL of the source image or ZIP archive |
| `packageSha256` | SHA-256 of the file served by the URL |
| `packageEntry` | Exact ZIP member path. For raw images, use the source filename |
| `inputSha256` | SHA-256 of the raw image after extracting it, if applicable |
| `slice.start`, `slice.endInclusive` | Zero-based inclusive range copied into the result |
| `patchFile` | Path to the patch JSON |
| `outputSha256` | SHA-256 of the expected final image |

The patch has a final byte `length` and a `runs` array. Each run contains a zero-based output `offset` and base64-encoded `bytes`. The copied slice starts at output offset zero. Extra bytes are initialized to zero before applying the runs.

```json
{"length":4,"runs":[{"offset":2,"bytes":"AQI="}]}
```

For automation, run `Build-FwImage.cmd -Download -Manifest C:\path\manifest.json -Output C:\path\firmware.bin`. For a local source, replace `-Download` with `-InputPath C:\path\stock.bin`.
