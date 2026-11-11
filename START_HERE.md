# PICO Motion Trackers on Steam Frame

## Quick Start

You need a Steam Frame with SSH access as `steamos`, both Frame controllers connected, and one PICO Swift tracker. Stop any running tracker session or game before testing. Start with a tracker already running Alpha 1.3 and paired to the Frame if available.

1. **Prepare the tracker image if OTA is needed.** On Windows, open [Build-FwImage.cmd](launcher/Build-FwImage.cmd), select **Download and build**, and keep the default save location inside `frame/pico-ram-work/native-port-r3-20261101/outputs/`. The expected Alpha 1.3 SHA-256 is `bad0a210b00ddd80ba6898c7b10cdbb2e4b7e7345c18aca0ac02d699ac4ca915`.
2. **Install on the Frame.** From the handoff folder, replace `FRAME_IP` with the headset IP. Use a fresh destination for the upload to avoid nesting the local `frame` folder inside an existing directory.

   ```sh
   scp -r frame steamos@FRAME_IP:/home/steamos/handoff-frame
   ssh steamos@FRAME_IP
   sh /home/steamos/handoff-frame/install.sh
   ```

   The installer may ask for sudo to set up OpenOCD. It installs the app and drivers, registers the camera tap, and restarts SteamVR.
3. **Pair or update only as needed.** Follow [Pairing and OTA](#pairing-and-ota) below. If the tracker is already paired and on Alpha 1.3, continue directly to the session.
4. **Wake the tracker and start a 30-second session on the Frame.**

   ```sh
   PICO_DURATION=30 sh /home/steamos/pico-ram-work/native-port/live-visual.sh
   ```
5. **Check the result.** Look for `live exit 0` and four successful cleanup barriers in the session journal. In `/tmp/pico-diag.log`, a rising `s16` in `station_counters` confirms radio replies. A rising IMU packet count on the driver's `pico_fusion` lines in `~/.local/share/Steam/logs/vrserver.txt` confirms delivery to SteamVR. On the 2026-11-06 SteamVR update, `fits=0` is expected with Alpha 1.3. See the [optics blocker](ROADMAP.md).

## Pairing and OTA

For pairing, turn off the other trackers, put the target in discovery mode, and run the following on the Frame. Follow the prompt and note the assigned slot. A tracker paired to PICO again must be paired back to the Frame.

```sh
sh /home/steamos/pico-ram-work/native-port-pairing-20261102/pair-tracker.sh
```

For OTA, use an interactive SSH terminal. Keep the target tracker freshly powered on in normal mode, off USB and the charger, with other trackers off. Keep both controllers connected and the headset tracked. Replace `SLOT` with the assigned slot from `devices.conf`.

```sh
cd /home/steamos/pico-ram-work/native-port-r3-20261101
sh hardcoded-tested/ota-stock.sh SLOT "/home/steamos/ota-$(date +%Y%m%d-%H%M%S)"
```

For a different image supported by the OTA tool, use `sh ota-stock.sh SLOT /absolute/new-attempt /path/to/image.bin`. The tool validates the image against `native/frame-release-ota.allowlist`. Both routes require typing `GO` and then the exact `READY <challenge>` displayed. Wait for `finish_verified` in `ota.log` and the successful completion message, then restart the tracker with a long press. Long-press restart returns Alpha 1.3 to stock-compatible mode without replacing the installed firmware. Pairing discovery and OTA on the updated Roy firmware still need live validation.

## Running and troubleshooting

- In the headset, open **SteamVR Dashboard → + → FramePicoTracker**, select 1, 2, or 3 trackers, and start a session. For body calibration select **Auto 2** or **Auto 3**, stand facing forward, then look down at the trackers and hold still.
- For a manual 2–3 tracker run use `sh /home/steamos/pico-ram-work/native-port-mrec-20261103/run-multi.sh`. Both session scripts accept `PICO_DURATION` from 5 to 150 seconds, defaulting to 150.
- Pairing generates `/home/steamos/pico-ram-work/devices.conf`. Optional owner-local camera and LED calibrations live in its sibling `private/` directory. Keep device data local. Default calibration is available.
- `prep failed`: read `/tmp/live-prep.out`. Session output is in `/tmp/live-run.out`; session directories are under `native-port/native-live-*`.
- `Roy layout refused`: inspect the reason. Confirm `roy-source/roy_layout` is executable and `clang` and `ld.lld` are available on the Frame. The supplied prebuilt module covers the known layout only.
- `Roy layout deferred` or `Other Roy owner alive`: let the current session finish and release Roy before retrying.
- Few radio replies: wake the tracker and both controllers, check pairing, and disconnect charging or USB.

## Development

Build on the Frame with `g++`, `clang`, and `ld.lld` unless otherwise noted. Preserve the installed folder names and back up installed binaries before replacing them. The deployment root is `/home/steamos/pico-ram-work`.

| Component | Build and deployment |
| --- | --- |
| Producer and session tools | From a copy of `source/frame-native` on the Frame, run `sh link-installed.sh`, then `sh native/build.sh`. The producer and invocation helper are built in `native/build/`. Check the active session scripts for their binary paths before replacing them. |
| OTA tools | From `source/frame-native`, run `sh link-installed.sh` and then `sh native/build-stock-ota.sh`. It uses the synthetic devices fixture and reads Alpha 1.3 from the linked `outputs/` directory. Deploy the tools to `native-port-r3-20261101/native/build/`. |
| Roy layout and modules | Deploy `native/build/roy_layout` to `roy-source/`. Keep that directory's locator sources and station/pairing module sources in sync. `roy-prepare.sh` builds the module against the detected layout. |
| Pairing | Copy `source/pairing` to the Frame and copy `source/frame-native/driver` into it as `driver/`. Run `sh native/build-frame-pair.sh` from the pairing root. Output: `outputs/pairing/frame_pair` and `pairing.elf`. |
| SteamVR driver | From `source/frame-native/driver`, run `g++ -std=c++17 -O2 -fPIC -shared -I/opt/steamvr/tools/hellovr_vulkan_linux/src/openvr/headers driver_pico_tracker.cpp -o driver_pico_tracker.so -ldl`. |
| Frame app | From `source/frame-app`, run `sh build_frame.sh`. |
| Camera tap | From `source/frame-native/xrtap`, run `gcc -shared -fPIC -O2 xrtap.c -o libxrtap.so -ldl`. |
| OpenOCD | `source/openocd-tlmm/linuxgpiod-tlmm.patch` targets the `mhei/openocd` repository at `64fb747da`. |
| PC bridge | See the [PC bridge README](source/pc-bridge/README.md). |
| Tracker firmware | Use the [launcher](launcher/README.md) for exact reconstruction. Source changes use `source/firmware-alpha13/tools/fwpatch/build-alpha13.ps1` with Zig and the original development inputs. See [limitations](ROADMAP.md). |

Some legacy build tests require the matching local Roy fixture at `candidate/stock.bin`, which is absent from this package. Live sessions read the installed Roy firmware. The known `lower_camera_tests` failure predates this handoff.
