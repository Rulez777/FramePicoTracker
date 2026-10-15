# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K06081
Source `D0150`, original line79, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Decompilation of the radio module functions (`0x40001000`) in the station and tracker firmwares revealed exact register settings:

## K06082
Source `D0150`, original line81, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Register | Address | Value / Bitmask | Interpretation |
|---|---|---|---|
| **`MODE`** | `0x40001510` | `0x03` / `0x04` | Physical layer GFSK 1 Mbps (`0x03`) or 2 Mbps (`0x04`) (BLE PHY-compatible) |
| **`PCNF0`** | `0x40001514` | `0x00000108` | `LFLEN` = 8 bits (max. 255 bytes), `S0LEN` = 1 bytes, `PLEN` = 8 bits preamble |
| **`PCNF1`** | `0x40001518` | `0x000300FF` | `MAXLEN` = 255 bytes, `BALEN` = 3 bytes (address base), Little-Endian |
| **`PREFIX0`** | `0x40001524` | `0x71` (in low byte) | Address prefix byte |
| **`BASE0`** | `0x4000151C` | `0x76412900` | Default base address for pairing. In paired form — unique bond address of the tracker |
| **`CRCCNF`** | `0x40001534` | `0x00000103` | CRC length = 3 bytes (24-bit), `SKIPADDR = 1` (address excluded from calculation) |
| **`CRCPOLY`** | `0x40001538` | `0x0000065B` | BLE polynomial: $x^{24} + x^{10} + x^9 + x^6 + x^4 + x^3 + x + 1$ |
| **`CRCINIT`** | `0x4000153C` | `0x00555555` | Standard initial CRC value |
| **`RXADDRESSES`** | `0x40001530` | `0x01` | Reception enabled on logical address 0 |

## K06083
Source `D0150`, original line93, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

> [!IMPORTANT]
> **Addressing specifics:**  
> The full over-the-air Access Address consists of 4 bytes (1 byte prefix + 3 bytes base). In tracker firmware, address bytes are inverted using `rbit` and `rev` instructions for correct LSB-first transmission order.  
> Address of the connected test tracker: `[DEVICE_MAC]` (ASCII `"1233"`).

## K06084
Source `D0150`, original line100, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 4. Telemetry and IMU packet format

## K06085
Source `D0150`, original line102, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Packet format recovered from `libpvrtrackingservice.so` disassembler (tracker telemetry processing function at offset `0x96F80 - 0x971D0`):

## K06086
Source `D0150`, original line104, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Unpacked tracker telemetry structure:
1. **On-board orientation (On-board Fusion Quaternion):**
   - `MCU_Q[0..3]` — 4x `float32` (16 bytes): $w, x, y, z$. The tracker on Keil RTX independently integrates gyroscope and accelerometer readings and outputs a stable orientation quaternion.
2. **Raw sensor readings:**
   - `acc.xyz` — 3x `float32` (acceleration in m/s² or g).
   - `gyro.xyz` — 3x `float32` (angular velocity in rad/s).
   - `mag.xyz` — 3x `float32` (magnetometer).
   - `raw_temperature` — `int16`:
     $$T = \frac{\text{raw\_temp}}{132.48} + 25.0^\circ\text{C}$$
3. **Headers and metadata:**
   - `frame_cnt` (`uint16`/`uint32`): tracker packet counter.
   - `imu.ts` (`uint32`/`uint64`): monotonic tracker timestamp.
   - `station_index` (`uint16`): sequence packet number on Station receiver.
   - `latency` (`uint8`): delivery latency.
   - `ir_led status` (`uint8`, mask `0x3F`): operating mode and LED strobe bits.
   - `Q_flag` (`uint8`, bit 0): quaternion validity flag.
   - `start_index` (`uint8`, bits 1–7): session/series marker.

## K06087
Source `D0150`, original line124, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 5. Optical tracking (IR LEDs) and synchronization

## K06088
Source `D0150`, original line126, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`libpico_controller_tracking.pxr.so` contains modules **`swiftblober`** and **`CentaurEngine`**:
1. **Strobe synchronization:** Pico 4 cameras operate with strobed exposure (to cut off background lighting). Station synchronizes the opening of camera shutters with bursts of IR LED pulses on the tracker.
2. **Blob detection:** The `BlobDetector` module performs adaptive threshold binarization, finds centroids (`findBlobs_neon`), filters spots by brightness and circularity coefficient (`cmpByCircularity`, `cmpByBlobBrightNess`).
3. **6DoF fusion:** `CentaurEngine::GetTrackerPose` matches the constellation of 2D spots with the rigid 3D geometry of the tracker's LEDs and fuses the result with high-frequency IMU.
4. **LED control for third-party systems:**  
   The tracker supports the command `set_irled_onoff_mode %02X` (available via `SetTrackerLed` in `libtrackerclient.pxr.so`), which allows turning on the LEDs in **continuous illumination mode (Static ON)**. This is critical if Steam Frame tracks the LEDs with its cameras without radio synchronization.

## K06089
Source `D0150`, original line135, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 6. Implemented software bridge to SteamVR (K:\AI\pico)

## K06090
Source `D0150`, original line137, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The approved B2B architecture is deployed in repository `K:\AI\pico`, using the official **Object Tracking API** from `pico-sdk 3.4.0` on the headset and the **VMT** virtual driver on the PC.

## K06091
Source `D0150`, original line139, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 6.1. Headset side (`K:\AI\pico\unity\`)
- **Component:** `PicoTrackerStreamer.cs` (original reference: `[PRIVATE_SOURCE_REFERENCE]`)
- **SDK calls:**

## K06092
Source `D0150`, original line151, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- **Network stream:** Non-blocking TCP socket with an atomic double buffering slot (eliminates FPS drops in VR).

## K06093
Source `D0150`, original line153, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 6.2. Wire Protocol v1 (TCP `127.0.0.1:39500`)
Little-Endian binary protocol:
- **Header (12 bytes):**
  - `magic` (u32): `0x31425450` (`"PTB1"`)
  - `version` (u16): `1`
  - `type` (u16): `1` (Frame) / `2` (Hello)
  - `length` (u32): payload length
- **Hello Payload (Type 2):** UTF-8 JSON: `{"device":"A8110","sdk":"3.4.0","app":"0.1"}`
- **Frame Payload (Type 1):**
  - `t` (f64): monotonic `Stopwatch` time
  - `frame` (u32): frame counter
  - `hmd` (7×f32): Pico headset position and quaternion (`px, py, pz, qx, qy, qz, qw`)
  - `count` (u8): number of tracker entries
  - `per tracker` (65 bytes per tracker):
    - `id` (i64): tracker ID in SDK
    - `valid` (u8): `1` if pose is valid, `0` if lost
    - `pose` (7×f32): `px, py, pz, qx, qy, qz, qw` (Unity Left-Handed, meters)
    - `linVel` (3×f32): linear velocity
    - `angVel` (3×f32): angular velocity
    - `battery` (f32): charge `0.0..1.0` (`-1.0` if unknown)

## K06094
Source `D0150`, original line174, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 6.3. PC side (`K:\AI\pico\pc\`)
- **`protocol.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):** strictly typed frame packing/unpacking.
- **`coords.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):** coordinate system conversion:
  $$P_{\text{vr}} = (x_{\text{unity}},\; y_{\text{unity}},\; -z_{\text{unity}})$$
  $$Q_{\text{vr}} = (-qx_{\text{unity}},\; -qy_{\text{unity}},\; qz_{\text{unity}},\; qw_{\text{unity}})$$
- **`calibration.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):** Kabsch algorithm (SVD solution of the Procrustes problem) for 6DoF calibration of Pico space relative to Steam Frame:
  $$P_{\text{Frame}} = R \cdot P_{\text{Pico}} + T, \quad Q_{\text{Frame}} = Q_{\text{align}} \cdot Q_{\text{Pico}}$$
- **`vmt_client.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):** sending OSC packets `/vmt/set/room/pose` and `/vmt/set/diag` to the VMT driver (UDP `127.0.0.1:39570`).
- **`receiver.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`) & `main.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):** reception and broadcasting daemon.
- **`calibrate.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):** utility for automated collection of paired points of two HMDs and calculation of `calibration.json`.
- **Tests:** 11 unit tests in `pc/tests/` (100% coverage of basic mathematical operations and protocol, all passed).

## K06095
Source `D0150`, original line188, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 7. Practical deployment guide

## K06096
Source `D0150`, original line190, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Network preparation:

## K06097
Source `D0150`, original line196, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Launching the bridge on PC:

## K06098
Source `D0150`, original line202, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Space calibration (Pico ↔ Steam Frame):

## K06099
Source `D0150`, original line210, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 8. Perspective: Hardware dongle option (nRF52840 Dongle PCA10059)

## K06100
Source `D0150`, original line212, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

If in the future there is a need to eliminate the Pico headset as a radio receiver:
1. A **Nordic nRF52840 Dongle (PCA10059)** is obtained for ~$12.
2. Flashed with radio settings from **Section 3** (`CRCPOLY = 0x65B`, `CRCINIT = 0x555555`, `PCNF0 = 0x108`, `PCNF1 = 0x000300FF`, `PREFIX = 0x71`).
3. Receives the `MCU_Q` quaternion directly from the tracker over the air and feeds it via USB Virtual COM/HID to SteamVR (3DoF IMU-orientation mode, similar to SlimeVR).

## K06101
Source `D0150`, original line219, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 9. Implementation option 2: Direct optical tracking with Steam Frame cameras (IR Optical Tracking on Frame)

## K06102
Source `D0150`, original line221, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This option completely eliminates dependence on the Pico 4 headset and provides native sub-millimeter accuracy in Steam Frame's own coordinate system.

## K06103
Source `D0150`, original line223, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 9.1. Pipeline architectural diagram

## K06104
Source `D0150`, original line251, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 9.2. Physical reconfiguration of tracker LEDs (Static ON)

## K06105
Source `D0150`, original line253, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### Strobing problem:
By default, the tracker LEDs are synchronized with the strobe of Pico 4 cameras via the `fsin_count` radio signal from the Station chip. Steam Frame cameras operate with an independent shutter phase, so during strobing the LEDs will "blink" or drop out in frames.

## K06106
Source `D0150`, original line256, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### Solution:
Switching the tracker to **continuous illumination mode (Static ON)**:
1. **Command in tracker firmware:**  
   `set_irled_onoff_mode 0x01` (the function for controlling LED modes `set_irled_onoff_mode %02X` was found in the binary).
2. **Command dispatch by host:**  
   Call `SetTrackerLed` in `libtrackerclient.pxr.so` or an equivalent byte in the station configuration radio packet (`cmd %d tracker %d radio_interval %d imu_mode :%d irled %d`).
3. **Energy balance:**  
   In pulsed mode, the LEDs shine with a duty cycle of ~2–5%, providing 25+ hours of battery life. In continuous mode, current consumption increases to ~60–80 mA, which reduces tracker battery life to **3–5 hours** (sufficient for an active gaming/testing session, or compensated by a lightweight external power bank).

## K06107
Source `D0150`, original line267, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 9.3. Camera and optics requirements for Steam Frame

## K06108
Source `D0150`, original line269, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Parameter | Steam Frame Requirement | Justification |
|---|---|---|
| **Sensor wavelength** | Sensitivity in the range of 850 nm or 940 nm | Emission wavelength of built-in Pico Swift IR LEDs |
| **Optical filter** | Daylight-blocking (IR bandpass 850±30 nm or 940±30 nm) | Cutoff of visible light from lamps and monitors for reliable LED extraction |
| **Shutter type (Shutter)** | Global Shutter | Elimination of distortion of point constellation geometry during sharp leg swings |
| **Exposure parameters** | Short exposure (0.5 – 1.5 ms), fixed Gain | LEDs look like compact white spots without halos or blurs |
| **Frame stream access** | UVC / OpenXR Camera Stream / Vulkan-Direct Video | Direct frame reading API required on host (Steam Frame dev kit) |

## K06109
Source `D0150`, original line279, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 9.4. Algorithmic Computer Vision Pipeline (PnP + EKF)

## K06110
Source `D0150`, original line281, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### Step 1. Spot detection (Subpixel Blob Detection)
The module is similar to `swiftblober` found in Pico:
1. Threshold binarization of monochrome frame: $I(x, y) > T_{\text{threshold}}$.
2. Finding connected components (contours) and calculating geometric moments:
   $$x_c = \frac{m_{10}}{m_{00}}, \quad y_c = \frac{m_{01}}{m_{00}}$$
3. Filtering candidates by circularity coefficient and area (diameter 2–10 pixels).

## K06111
Source `D0150`, original line288, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### Step 2. Geometric 3D model of the LED constellation (Constellation)
From factory calibration of the physical tracker's non-volatile memory (NVRAM) (`tracker_test cali-tracker`), the **exact 3D coordinates of all 8 LEDs** relative to the IMU center (in meters) were extracted:

## K06112
Source `D0150`, original line291, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| LED Index | X (m) | Y (m) | Z (m) | Arrangement |
|---|---|---|---|---|
| **LED 0** | `+0.007426` | `+0.015968` | `-0.009357` | Upper ring |
| **LED 1** | `-0.004626` | `+0.013630` | `-0.004681` | Beveled crown (top) |
| **LED 2** | `-0.009299` | `-0.001385` | `-0.009882` | Perimeter |
| **LED 3** | `-0.003668` | `-0.014409` | `-0.009500` | Perimeter |
| **LED 4** | `+0.010146` | `-0.018294` | `-0.009431` | Perimeter |
| **LED 5** | `+0.022000` | `-0.013442` | `-0.004945` | Beveled crown (top) |
| **LED 6** | `+0.024650` | `-0.001246` | `-0.009246` | Perimeter |
| **LED 7** | `+0.019313` | `+0.013377` | `-0.004308` | Beveled crown (top) |

## K06113
Source `D0150`, original line302, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Since LEDs 1, 5, 7 lie on the plane $Z \approx -4.6$ mm, and LEDs 0, 2, 3, 4, 6 on the plane $Z \approx -9.4$ mm, the constellation is strictly **non-coplanar**, which mathematically eliminates flip ambiguity.

## K06114
Source `D0150`, original line304, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### Step 3. PnP solution with a priori orientation (Prior-assisted PnP)
Classic PnP (Perspective-n-Point) is computationally expensive and subject to flips.  
**Key optimization:**  
A **high-precision orientation quaternion $Q_{\text{imu}}$** (200 Hz) arrives from the tracker via radio:
1. Fix $R = \text{Rot}(Q_{\text{imu}})$.
2. The problem reduces to an instantaneous solution of a linear system for $T = (X, Y, Z)$:
   $$\lambda_i \begin{pmatrix} u_i \\ v_i \\ 1 \end{pmatrix} = K \cdot (R \cdot P_i + T)$$
3. The solution is refined by 3 iterations of the Gauss-Newton method, minimizing the true reprojection error in pixels (< 0.1 ms per frame).

## K06115
Source `D0150`, original line313, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### Step 4. EKF (Extended Kalman Filter) fusion
- **Predict (200 Hz):** Integrating acceleration and angular velocity from ICM-42688 with gravity compensation.
- **Correction (60–90 Hz):** Optical coordinate $T_{\text{opt}}$ from Steam Frame cameras resets positional drift.
- **Occlusion:** When LEDs are occluded, the filter maintains pose on inertial integration without jumps.

## K06116
Source `D0150`, original line318, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### Implemented module stack (K:\AI\pico\pc\optical\):
- `constellation.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`): 3D model of 8 LEDs + camera projection with normal culling.
- `blob_detector.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`): subpixel centroid detector (NumPy first-order moments).
- `distortion.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`): Brown-Conrady distortion model ($k_1, k_2, p_1, p_2, k_3$) with Newton-Raphson inversion in $O(N)$ for spot centroids (< 1 µs).
- `pnp_solver.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`): monocular Prior-assisted PnP + Gauss-Newton optimizer.
- `stereo_pnp.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`): multi-camera / stereo-PnP with joint linear system $A_{\text{stereo}} T_{\text{ref}} = b_{\text{stereo}}$ and triangulation baseline.
- `fusion_ekf.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`): 9-dimensional EKF (position, velocity, accelerometer bias) with gravity compensation.
- `camera_stream.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`): video stream abstraction (synthetic frame generator 60–90 Hz with Gaussian blur and UVC capture).
- `steam_frame_tracker.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`): end-to-end optical-inertial tracking engine.
- `run_optical.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`): CLI runner with end-to-end 60 FPS benchmark and broadcasting to SteamVR VMT (OSC).
- `tracker_ctl.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`): tracker management utility (status, battery, switching LEDs to Static ON).
- Autotests: `test_optical.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`), `test_distortion.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`), `test_stereo_pnp.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`) — 19/19 tests passed successfully (0.12 s).

## K06117
Source `D0150`, original line331, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### Benchmark results of the optical-inertial pipeline (Steam Frame Stereo Rig):
- **Processing frequency:** 58.9 FPS (synchronously with 60 Hz cameras)
- **Acquisition success rate (PnP Success Rate):** **100.0%** (177 of 177 frames)
- **Average pose error (3D Position Error):** **0.42 mm** (submillimeter accuracy!)
- **Average reprojection error:** **0.07 pixels**
- **Number of tracked LEDs:** 16 points (all 8 LEDs visible to left and right cameras)
- **Broadcasting to SteamVR:** real-time VMT OSC protocol (`127.0.0.1:39570`).

## K06118
Source `D0150`, original line341, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 9.5. Comparison of architectural implementation options

## K06119
Source `D0150`, original line343, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Criterion | Option 1 (Bridge via Pico 4, Section 6) | Option 2 (External dongle + Frame optics) | Option 3 (On-board Nordic SoC Frame, Section 10) |
|---|---|---|---|
| **Pico 4 headset** | **Yes** (needed nearby) | **No** | **No** |
| **External USB dongle** | No | Yes (nRF52840 USB dongle) | **No** (uses on-board Frame radio chip) |
| **IMU reception (200 Hz)** | Via Wi-Fi / TCP from Pico | Via nRF USB dongle radio | **Headset on-board Nordic SoC via ESB/Radio** |
| **Positioning (6DoF)**| Pico OS algorithms (~3–8 mm) | Frame stereo cameras (0.42 mm) | **Frame stereo cameras (0.42 mm) + on-board EKF** |
| **Space calibration** | Required (`calibrate.py`) | Not required | **Not required** (unified Frame reference frame) |
| **Tracker LED control**| Via Pico OS | Via dongle over radio | **Directly from Frame over radio (command 0x20)** |
| **Solution autonomy** | Depends on two headsets | Requires a free USB port | **100% Standalone (fully autonomous)** |
| **Code readiness** | 100% ready in `K:\AI\pico` | 100% ready in `K:\AI\pico` | **100% ready in `K:\AI\pico\pc\radio`** |

## K06120
Source `D0150`, original line356, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 10. Direct radio connection of trackers via the Steam Frame headset's on-board Nordic Radio SoC (Option 3)

## K06121
Source `D0150`, original line358, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The Steam Frame headset is equipped with its own **Nordic Semiconductor** radio chip (nRF52/nRF53 series, standard for controllers and peripherals of the SteamVR ecosystem).  
Since the Pico Motion Tracker (Swift 2.0) is also built on a Nordic nRF52 chip and communicates via "pure" `RADIO` peripheral (without the BLE SoftDevice stack), the on-board Steam Frame radio chip can **directly receive and transmit tracker packets via ESB (Enhanced ShockBurst) / nRF Radio**.

## K06122
Source `D0150`, original line361, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 10.1. Hardware register configuration of Nordic RADIO (Steam Frame SoC)

## K06123
Source `D0150`, original line363, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

To receive Pico Tracker packets, the on-board Steam Frame radio chip is programmed with the following values:

## K06124
Source `D0150`, original line391, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 10.2. Over-the-air telemetry packet format (64 bytes)

## K06125
Source `D0150`, original line393, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Byte | Type | Field Name | Description |
|---|---|---|---|
| `0` | `u8` | `pkt_type` | `0x02` (Telemetry) or `0x05` (Extended telemetry) |
| `1` | `u8` | `flags` | Bit 0: quaternion validity (`quat_valid`), bits 1–7: session number |
| `2..3` | `u16` | `seq_num` | Incremental tracker packet counter |
| `4..7` | `u32` | `timestamp_us` | Hardware tracker timestamp in microseconds |
| `8..23` | `4 x f32` | `Q[w, x, y, z]` | On-board orientation quaternion (Keil RTX filtering ICM-42688) |
| `24..35`| `3 x f32` | `acc[x, y, z]` | Sensor linear acceleration (m/s² or g) |
| `36..47`| `3 x f32` | `gyro[x, y, z]`| Sensor angular velocity (rad/s) |
| `48..59`| `3 x f32` | `mag[x, y, z]` | Magnetometer data (µT) |
| `60..61`| `int16` | `raw_temp` | Temperature: $T = \text{raw\_temp} / 132.48 + 25.0^\circ\text{C}$ |
| `62` | `u8` | `batt_charge` | Bit 7: charge status, bits 0–6: battery charge (0–100%) |
| `63` | `u8` | `irled_status`| LED mode: `0x00` = Pico strobe, `0x01` = Static ON |

## K06126
Source `D0150`, original line407, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 10.3. Two-way radio control of the tracker

## K06127
Source `D0150`, original line409, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The Steam Frame on-board chip can not only listen to the tracker, but also send control packets to it (`PKT_TYPE_CMD_ACK = 0x03`):
- **Command to turn on diodes in Static ON (`0x20`):**  
  Burst: `[0x03, 0x03, 0x20, 0x01, 0x00]` switches the tracker to continuous diode illumination for Steam Frame cameras.
- **Radio interval change command (`0x30`):**  
  Controls the transmission frequency (64 / 112 / 216) and puts the tracker into active movement mode.

## K06128
Source `D0150`, original line415, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 10.4. Implemented components in the codebase (K:\AI\pico\pc\radio\):

## K06129
Source `D0150`, original line417, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **`esb_packet.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):**
   - Binary codec for telemetry and command packets (Little-Endian unpack/pack).
   - Orientation conversion to quaternion $[x, y, z, w]$, decoding of temperature, battery, and statuses.
2. **`pico_tracker_radio_rx.h` (original reference: `[PRIVATE_SOURCE_REFERENCE]`) & `pico_tracker_radio_rx.c` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):**
   - Ready-to-compile C driver for the on-board Nordic SoC of the Steam Frame headset (nRF52840/nRF5340).
   - Supports nRF Connect SDK (Zephyr), nRF5 SDK, and bare-metal CMSIS.
   - `RADIO_IRQHandler` interrupt handler on `EVENTS_CRCOK` with a ring DMA buffer.
3. **`nrf_radio_bridge.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):**
   - Communication driver between the Steam Frame main processor and the on-board Nordic SoC (via UART `/dev/ttyACM0` or SPI).
   - Background thread for 200 Hz reception and sending tracker control commands.
   - `MockSteamFrameRadioBridge` for offline testing without hardware.
4. **End-to-end integration into `run_optical.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`):**
   - Flag `--radio serial:<PORT>` (or `--radio mock`) automatically connects the radio stream, sends the `set_tracker_led_mode(0x01)` command on the fly, and feeds 200 Hz IMU into `OpticalInertialEKF`.
5. **Automated tests:** `test_esb_packet.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`) and `test_radio_bridge.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`) — 23 of 23 tests in the repository passed successfully.

## K06130
Source `D0150`, original line434, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 11. Architecture of the Pico ESB interpreter driver (without reflashing Roy SoC)

## K06131
Source `D0150`, original line436, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 11.1. Problem statement and hardware constraints
- **Stock Roy nRF52 chip on the headset:** Connected to the SoC via the `hid-over-spi` bus (`/dev/spidev0.1`), runs under proprietary firmware `roy_dongle_fw.hex`, and provides ultra-low latency for stock Valve Frame controllers.
- **Constraint:** Reflashing Roy via SWD (`openocd`) for the Pico protocol leads to **instant loss of Valve controllers**. A "timeshare / two stacks on one chip" mode on Valve's proprietary firmware is impossible (the nRF52 radio is hardware-configured for a single radio profile BALEN=4 / CRC16 at any time quantum).
- **Solution:** Software **interpreter driver on the host (SteamOS / Linux aarch64)**. The Roy chip is untouched, Valve controllers operate normally. A separate compact radio module (USB-C dongle nRF52840 Dongle / PCA10059) is used to receive Pico trackers, and the driver translates the Pico signature into native OpenVR Tracker devices.

## K06132
Source `D0150`, original line457, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 11.2. Pico Tracker signature recognition (ESB Signature Detector)
The `pico_esb_signature.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`) module performs continuous validation of the raw air stream against key invariants:
1. **Length and header:** Exactly 64 bytes, byte $S_0 \in \{0x02, 0x05\}$, orientation validity flag (bit 0 = 1).
2. **Physical integrity of quaternion:** Unit norm verification $\|Q\| = \sqrt{w^2 + x^2 + y^2 + z^2} \approx 1.0 \pm 0.05$.
3. **Physical integrity of accelerometer:** $\|a\| \le 150 \text{ m/s}^2$ (cutoff of interference and bit errors).
4. **Battery monitoring:** Correct charge range $0 \le \text{batt} \le 100$ and charging status.
If at least one invariant does not match, the packet is discarded as noise.

## K06133
Source `D0150`, original line465, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 11.3. OpenVR Tracker Provider (`openvr_tracker.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`))
- Registers trackers as class `TrackedDeviceClass_GenericTracker` devices.
- Automatically maps serial numbers to anatomical roles:
  - Tracker 1 (`[TRACKER_SERIAL]`) $\rightarrow$ `TrackerRole.WAIST`
  - Tracker 2 (`[TRACKER_SERIAL]`) $\rightarrow$ `TrackerRole.LEFT_FOOT`
  - Tracker 3 (`[TRACKER_SERIAL]`) $\rightarrow$ `TrackerRole.RIGHT_FOOT`
- Updates poses and battery level in real time via the VMT / OpenVR protocol.

## K06134
Source `D0150`, original line473, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 11.4. System daemon for SteamOS (`pico_tracker_daemon.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`))
- Automatically scans `/dev/ttyACM*` and `/dev/ttyUSB*` on Steam Frame.
- Starts a background reception thread at 200 Hz.
- On startup, sends the `0x20` command over the air (parameter `0x01`), switching trackers into `Static ON` mode for Frame optical cameras.
- Can be run as a `systemd` system service (`pico-tracker.service`).

## K06135
Source `D0150`, original line479, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 11.5. Native C++ SteamVR driver (`driver_pico_tracker.cpp` (original reference: `[PRIVATE_SOURCE_REFERENCE]`))
- Implements the `vr::IServerTrackedDeviceProvider` and `vr::ITrackedDeviceServerDriver` standards.
- Exports the `HmdDriverFactory` entry point.
- Ready to compile for the `linuxarm64` architecture (aarch64, kernel 6.18) with manifest `driver.vrdrivermanifest` (original reference: `[PRIVATE_SOURCE_REFERENCE]`).

## K06136
Source `D0150`, original line484, SHA256 `d8f4e91dcdcf4fbf6f9a09f59e00e2c563da82623fd3ddfc3351d6c11b4a5af5`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 11.6. Interpreter driver test results
In the `test_driver_interpreter.py` (original reference: `[PRIVATE_SOURCE_REFERENCE]`) module, the following was confirmed:
- Correct recognition of valid telemetry packets.
- Complete cutoff of random over-the-air bit noise.
- Cutoff of invalid unnormalized quaternions.
- Correct binding of anatomical roles (Waist, LeftFoot, RightFoot).
- **All 27 repository tests passed successfully.**

## K06137
Source `D0151`, original line1, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# PICO Motion Tracker Swift 2.0 → stock Nordic Steam Frame

## K06138
Source `D0151`, original line3, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date of update: 2026-10-05. This document replaces previous claims of
completed reverse engineering and a "100% ready" standalone bridge with verified data.

## K06139
Source `D0151`, original line6, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Current result:** Roy SRAM code runs via the stock shell;
standalone MPSL callbacks at around 100 Hz and TIMER0 with two active
controllers, with MPU enabled, have been verified. Wi-Fi/SSH access allows disconnecting USB.
PICO RF, a full standalone bridge, and real 6DoF poses have not yet been achieved.
The latest MPU cleanup fix passed a full MPSL replay: 222 START,
337 lines of poses for the HMD and both controllers each; recovery confirmed.

## K06140
Source `D0151`, original line30, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Device | USB ADB serial | Live data | Access |
|---|---|---|---|
| PICO 4 Enterprise | `[HEADSET_ADB_SERIAL]` | model A8110, Android 10, build/display 5.13.7 | standard ADB shell, uid=2000; root not confirmed |
| Steam Frame dev kit | `frame` | SteamOS VR 0.4.3, BUILD_ID 20260930.6234839; kernel 6.18.0-ge66bc2ca6f8c, aarch64 | ADB shell already uid=0 |

## K06141
Source `D0151`, original line1268, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Based on static disassembly (Capstone) of the PhoenixSta station firmware (`PhoenixSta_sv2.96_PR_20250805_b296_app.bin`), Swift 2.0 tracker firmware (`Swift2.0_sv1.89_20241213_b189_app.bin`), headset system libraries (`libpvrtrackingservice.so`, `libtrackerclient.pxr.so`), and the `tracker_test` utility, the radio protocol, packet reception pipeline, and pairing process have been fully reconstructed.

## K06142
Source `D0151`, original line1270, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 1. Method of receiving Swift packets

## K06143
Source `D0151`, original line1272, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The end-to-end data path from the tracker to users in the OS is as follows:

## K06144
Source `D0151`, original line1298, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### A. Over-the-air radio interface (Air Interface)
- **Peripheral**: Hardware Nordic `NRF_RADIO` (base address `0x40001000`).
- **Modulation and rate**: `MODE` = 4 (2 Mbit/s Nordic proprietary) or `MODE` = 0 (1 Mbit/s).
- **Packet configuration**:
  - `PCNF0 = 0x00000108`: `S0LEN = 1` byte, `LENGTH = 8` bits, `PREAMBLE = 8` bits (1 byte).
  - `PCNF1 = 0x000300FF`: `MAXLEN = 255`, `STATLEN = 0`, `BALEN = 3` (3-byte base address + 1-byte prefix = 4-byte air address), `ENDIAN = 0` (Little-Endian), `WHITEEN = 1` (Hardware Data Whitening enabled).
- **Checksum (CRC)**:
  - `CRCCNF = 0x00000103`: 24-bit CRC (3 bytes), `SKIPADDR = 1` (address excluded from CRC calculation).
  - `CRCPOLY = 0x0000065B` ($x^{24} + x^{10} + x^9 + x^6 + x^4 + x^3 + x + 1$).
  - `CRCINIT = 0x00555555`.
- **Frequency channels**:
  - Pipe 0 starting group: channels 1, 37, 77 (2401, 2437, 2477 MHz).
  - Pipe 1 starting group: channels 4, 78 (2404, 2478 MHz).
  - Operating channel grid: even frequencies `2 * index + 2` (2402..2480 MHz) according to a 40-bit mask negotiated during pairing.

## K06145
Source `D0151`, original line1313, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### B. Station (PhoenixSta nRF52840) and dual-channel SPI
The station has two separate SPI interfaces to the host:
1. **AP SPI (`/dev/stationdev`)**:
   - Bound to the `ap_spis` interface in the station firmware.
   - Used by the Android application processor (Kryo AP) to transmit configuration commands (`AP_SPI_CMD`), pairing (`pairing %d`, `unbond %d`), status queries (`GetTrackerStatus`), and tracker OTA updates.
2. **BP SPI (Baseband Processor / Sensors Low Power Island DSP)**:
   - Bound to the `bp_spis` interface in the station firmware (`bp_spis_event_handler`, `bp_spi_send_with_timestamp`).
   - Receives packets from the radio ring buffer `radio_recv_buf_parse` and immediately sends them via hardware SPI with hardware timestamps `time_ticks0` directly to the Qualcomm Hexagon DSP (SLPI) sensor processor, bypassing the Android OS task scheduler.

## K06146
Source `D0151`, original line1322, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### C. Packet parsing in `libpvrtrackingservice.so` (SlpiDataParser::trackerDataParserV2)
The Hexagon DSP processor passes `slpi_payload_data_t` structures to `libpvrtrackingservice.so` via FastRPC. The `trackerDataParserV2` function (virtual address `0x96e44`, size 1004 bytes) performs byte-by-byte parsing:
- **Tracker slot index**: `(payload[2] & 0x3F) - 2` (values 0, 1, 2 correspond to tracker1, tracker2, tracker3).
- **Timestamp mode**:
  - Bit 7 (`0x80`) of byte 2: extended header flag (IMU payload offset: 5 or 7 bytes).
  - Bit 6 (`0x40`) of byte 2: timestamp width selection (0 = 16-bit ticker `ldurh w8, [x20, #3]`, 1 = 32-bit ticker `ldur w8, [x20, #3]`).
- **IMU payload offset (`x22`)**:
  - `+0x00`: Gyro X (`int16_t`, 16-bit signed)
  - `+0x02`: Gyro Y (`int16_t`)
  - `+0x04`: Gyro Z (`int16_t`)
  - `+0x06`: Accel X (`int16_t`, 16-bit signed)
  - `+0x08`: Accel Y (`int16_t`)
  - `+0x0A`: Accel Z (`int16_t`)
  - `+0x0C`: Mag X (`uint16_t`, 16-bit magnetometer)
  - `+0x0E`: Mag Y (`uint16_t`)
  - `+0x10`: Mag Z (`uint16_t`)
  - `+0x12`: Temperature (`int16_t`), conversion: $T = \text{raw} / 132.48 + 25.0^\circ\text{C}$ (ICM-42688 sensor)
  - `+0x14`: Frame / packet counter (`uint8_t`)
  - `+0x15`: Status byte (bit 0 = power/charging, bits 1..7 = operating mode)
  - `+0x16`: Tracker internal timestamp (`uint16_t`)
  - `+0x18..0x1A`: Validity and fusion mode flags (byte `0x19` determines orientation filter readiness)
  - `+0x1B..0x2A`: **Fusion orientation quaternion**: four 32-bit IEEE 754 floating-point numbers (`float`):
    - `+0x1B` (27): $q_0$ / $w$ (`ldur s11`)
    - `+0x1F` (31): $q_1$ / $x$ (`ldur s10`)
    - `+0x23` (35): $q_2$ / $y$ (`ldur s9`)
    - `+0x27` (39): $q_3$ / $z$ (`ldur s8`)
  - **Conclusion**: The Swift 2.0 tracker performs orientation calculation (Sensor Fusion) directly on the built-in nRF52 microcontroller and transmits the ready quaternion in every frame!

## K06147
Source `D0151`, original line1352, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 2. Pairing address and pairing process (Pairing Handshake)

## K06148
Source `D0151`, original line1354, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### A. Solving the `31:32:33:33` address puzzle
When executing the `tracker_test tracker` command, the headset displays:
`tracker1, online, sw:TR0189, hw:03, sn:[TRACKER_SERIAL], addr:31:32:33:33...`
`tracker2, online, sw:TR0189, hw:03, sn:[TRACKER_SERIAL], addr:31:32:33:33...`
`tracker3, online, sw:TR0189, hw:03, sn:[TRACKER_SERIAL], addr:31:32:33:33...`

## K06149
Source `D0151`, original line1360, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Disassembly of the `Swift 2.0` tracker firmware (VA `0x8ac4..0x8acc`) revealed the origin of this value:
- At address VA `0x16515` in the firmware image, there is a string constant `"1233\0"` (in bytes: `0x31, 0x32, 0x33, 0x33`).
- The instruction `ldr r3, [pc, #0xf0]` loads a pointer to `"1233"`, and the instruction `ldr r3, [r3]` reads it as the 32-bit word `0x33333231`.
- The instruction `str.w r3, [sp, #0x17]` writes this value into the tracker registration/status packet being formed at an offset of 3 bytes from the start of the packet data.
- Thus, `31:32:33:33` is a **static protocol stub marker (ASCII "1233")**, hardcoded into the tracker firmware code, rather than a dynamic hardware MAC address of the radio channel.

## K06150
Source `D0151`, original line1366, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### B. Over-the-air pairing address (Pairing Access Address)
In the initialization code of the `PhoenixSta` station radio module (function `0x13144`):
- `PREFIX0`: byte 0 of register `0x40001524` is masked and set to the value `0x71`.
- `BASE0`: register `0x4000151c` is loaded with a constant from the literal pool: `0x76412900`.
- With a base address length of `BALEN = 3`, the address is formed from the prefix (`0x71`) and the 3 least significant bytes of the base address (`0x29`, `0x41`, `0x76`).
- **Final 32-bit over-the-air pairing address**: **`0x71764129`** (transmitted over the air as the Little-Endian sequence: `0x71, 0x76, 0x41, 0x29`).

## K06151
Source `D0151`, original line1373, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### C. Pairing procedure (Handshake State Machine)
Pairing is performed in 4 steps:

## K06152
Source `D0151`, original line1391, SHA256 `3c99332d951e417b32ac6e2ecc21a343b68717f0bd1928eab72e608b654940a4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Pairing request**: The station enters pairing mode upon host command (`StartTrackerPairing`) and cyclically transmits a packet of length 9 bytes on frequencies of the Pipe 0 group (1, 37, 77) to the address `0x71764129`.
2. **Tracker identification**:
   - The tracker in the `STA_PAIRING` state receives a packet of length 9 bytes (`cmp r2, #9` at VA `0x68fc`).
   - The VA function `0x6844` forms an 13-byte response packet: the first 4 bytes contain markers `0x0002`, `0x000A`, and the VA function `0x7d18` reads the hardware 4-byte chip identifier from Nordic FICR `DEVICEID[1]` (at address `0x10000064`) and writes it into bytes 5..8.
   - The tracker sends the response and outputs the debug message `"pair fine !!!!!"`.
3. **Response with network parameters**:
   - The station sends a packet of length 17 or 21 bytes (`(r2 & 0xFB) == 0x11` at VA `0x691c`).
   - Bytes 7..11 contain a 40-bit frequency hopping mask.
   - Bytes 12..15 contain the station's 4-byte `peer_seed` (saved by the tracker in RAM `0x20000fd8`).
   - Byte 16 contains the address prefix `peer_prefix` (saved in RAM `0x20000fd0`).
4. **Calculation of operational address BASE1**:
   - The VA function `0x6124` performs byte-by-byte addition modulo 256:
     $$\text{word}[i] = (\text{peer\_seed}[i] + \text{device\_id}[i]) \pmod{256} \quad \text{for } i \in \{0, 1, 2, 3\}$$
   - The VA function `0x58f8` performs bit reversal (ARM instruction `RBIT`), forming the 32-bit register `BASE1`.
   - Both sides switch to address `BASE1` with prefix `PREFIX1` (`pair_switch2_connect`) and begin operational telemetry exchange on frequencies from the agreed mask.

## K06153
Source `D0153`, original line1, SHA256 `0fc20893d589194e77688637a71f2174332a7ced2594a40a3fc07be292cb8a57`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Concept document: Nineties Video Rental (working title: Midnight Tape Archive)

## K06154
Source `D0153`, original line3, SHA256 `0fc20893d589194e77688637a71f2174332a7ced2594a40a3fc07be292cb8a57`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Chapter 1. General vision, setting, and atmosphere

## K06155
Source `D0153`, original line5, SHA256 `0fc20893d589194e77688637a71f2174332a7ced2594a40a3fc07be292cb8a57`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

[Excluded original image: Video rental store interior]

## K06156
Source `D0153`, original line7, SHA256 `0fc20893d589194e77688637a71f2174332a7ced2594a40a3fc07be292cb8a57`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Brief project description
Midnight Tape Archive is a meditative, engaging first-person cleanup simulator in the vein of Librarian and TV Archive, set in the comical ambiance of a 1990s American late-night adult video rental store.

## K06157
Source `D0153`, original line10, SHA256 `0fc20893d589194e77688637a71f2174332a7ced2594a40a3fc07be292cb8a57`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The player takes on the role of an unfortunate clerk who, over a single night shift, must clear a colossal mess of several thousand scattered videotapes, sort them onto genre shelves, restore the store's presentable appearance, and find a collection of 6 legendary golden DVDs hidden by the previous owner.

## K06158
Source `D0153`, original line12, SHA256 `0fc20893d589194e77688637a71f2174332a7ced2594a40a3fc07be292cb8a57`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Key Project Pillars
1. Tactile thrill of sorting. Each tape has a pleasant weight, physics, a dull plastic thud when dropped, and a characteristic click when placed into a shelf slot.
2. Nostalgic humor and absurdity of the VHS era. The game is filled with parodies of iconic Hollywood hits, funny descriptions on the backs of boxes, and the spirit of old video rental stores with neon signs, wild-patterned carpets, and chunky CRT TVs.
3. Tangible reward for progress. Unlike pure routine, the player is motivated to collect rare golden DVDs, which grant the opportunity to turn on the TV in the VIP room and actually watch funny, stylized videos with parody adult content.
4. Cozy microclimate of a night store. Outside the window, night rain rustles and the neon city flickers; inside, a cassette player quietly hums with relaxing synthwave and pleasant beats, creating an ideal flow state.

## K06159
Source `D0153`, original line20, SHA256 `0fc20893d589194e77688637a71f2174332a7ced2594a40a3fc07be292cb8a57`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Chapter 2. Setup and Finale, Cheap Comedy on Videotapes

## K06160
Source `D0153`, original line22, SHA256 `0fc20893d589194e77688637a71f2174332a7ced2594a40a3fc07be292cb8a57`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

[Excluded original image: Boss's office and answering machine]
