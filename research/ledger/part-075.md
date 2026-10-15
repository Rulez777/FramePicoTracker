# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05921
Source `D0484`, original line228, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

> [!IMPORTANT]
> **Critical Discovery:** The Pico Swift 2.0 tracker transmits a **ready orientation quaternion (4 $\times$ float32)** in each telemetry packet (offset `0x1B`). The host does not need to implement its own heavyweight Mahony/Madgwick algorithm or EKF fusion of gyroscope and accelerometer to obtain tracker orientation.

## K05922
Source `D0484`, original line233, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 7. Practical Guide for Porting to Steam Frame

## K05923
Source `D0484`, original line235, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

To receive Pico Motion Trackers on Steam Frame without reflashing the Roy nRF52 chip and without losing stock Valve controllers:

## K05924
Source `D0484`, original line237, SHA256 `6b7230e9041aba76f87f37ff243896a4c9237d2ab6118bb73f465927a98a1e9f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Receiver radio profile:**
   - Modulation: Nordic Proprietary, 2 Mbps (`MODE=4`).
   - Preamble: 1 byte (`0x00000108`), Base address: 3 bytes, Prefix: 1 byte (`0x000300FF`).
   - CRC: 24 bits (`0x00000103`), polynomial `0x00065B`, initialization `0x00555555`, `SKIPADDR=1`.
2. **Pairing capture:**
   - Listen for or transmit pairing beacons at address `0x71764129` on channels `1, 37, 77`.
   - Capture the 13-byte tracker response and extract 4 bytes of `DEVICEID[1]` (offset 5..8).
   - Generate a 17-byte response with its own `peer_seed` and channel mask.
   - Calculate the working address: `BASE1 = RBIT((peer_seed + DEVICEID[1]) mod 256)`.
3. **Telemetry parsing:**
   - On negotiated channels, receive 64-byte frames at address `BASE1`.
   - Read 4 `float` numbers starting from the 27-th byte of the payload (`0x1B`) — this is the ready orientation quaternion $[w, x, y, z]$.
   - Stream the tracker pose to SteamVR via the VMT / OpenVR driver.

## K05925
Source `D0485`, original line18, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Date:** 2026-10-06  
**Status:** Verified based on the results of static disassembly (Capstone) of the firmware `PhoenixSta_sv2.96_PR_20250805_b296_app.bin` (headset station), `Swift2.0_sv1.89_20241213_b189_app.bin` (tracker), and system libraries `libpvrtrackingservice.so`.

## K05926
Source `D0485`, original line21, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

> [!IMPORTANT]
> **Separation of test and operational protocols:**  
> The values previously discovered in `PhoenixSta` at address `0x13144`:
> - Address `0x71764129`, `PCNF0 = 0x108`, `PCNF1 = 0x300FF`, `CRCPOLY = 0x65B`, `CRCINIT = 0x555555`, `MODE = 4` (`Ble_2Mbit`)
> pertain to **Nordic Bluetooth Direct Test Mode (DTM / RF certification)** (`DTM op test!`, `DTM recv test!`), where address `0x71764129` is the standard sync word of the Bluetooth Core Specification (Direct Test Mode PRBS9), and bit 25 `WHITEEN` in `0x300FF` is equal to 0 (whitening disabled).
> 
> **The actual operational protocol of Pico Swift 2.0** is implemented in `PhoenixSta` at addresses `0x924c..0x9266`, `0x9830..0x9848`, `0x9e8c..0x9ebc` and in `Swift2.0` at addresses `0x50ec`, `0x5480..0x54a6`, `0x7560..0x7600`. This is a modified **Nordic Enhanced ShockBurst (ESB)** stack with a 16-bit CRC16-CCITT checksum, addresses of the `0xE7E7E7E7` / `0x43434343` family, Big-Endian framing, and disabled hardware whitening.

## K05927
Source `D0485`, original line63, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### A. Comparison of DTM test mode and Pico Swift ESB operational mode

## K05928
Source `D0485`, original line65, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Parameter / Register | DTM test mode (`PhoenixSta 0x13144`) | Swift ESB operational protocol (`PhoenixSta 0x924c`, `Swift2.0 0x50ec`) | Note |
|---|---|---|---|
| **Purpose** | RF certification (FCC/CE, Direct Test Mode) | **Real telemetry and pairing transmission** | |
| **Stack family** | Bluetooth Core Spec DTM Test Tool | **Nordic Enhanced ShockBurst (ESB)** | Pure nRF RADIO |
| `PCNF0` | `0x00000108` ($S_0=1$, $LEN=8$, $PLEN=8$) | **`0x00040008`** ($LFLEN=8$, $S_0=0$, $S_1=4$, $PLEN=8$) | 4 bits of $S_1$ for ESB PID/NO_ACK |
| `PCNF1` | `0x000300FF` | **`BALEN`=3/4, `ENDIAN=1`, `WHITEEN=0`** | Big-Endian on transmission, whitening disabled |
| `CRCCNF` | `0x00000103` (24-bit CRC, $SKIPADDR=1$) | **`0x00000002`** (16-bit CRC, $SKIPADDR=0$) | Address is included in CRC calculation |
| `CRCPOLY` | `0x0000065B` (BLE CRC24) | **`0x00011021`** (CRC16-CCITT) | $x^{16} + x^{12} + x^5 + 1$ |
| `CRCINIT` | `0x00555555` | **`0x0000FFFF`** | Canonical ESB CRC init |
| `BASE0` | `0x76412900` | **`0xE7E7E7E7`** | Canonical Nordic ESB base address |
| `BASE1` | — | **`0x43434343`** (static) / dynamic | Common base address of logical channels 1..7 |
| `PREFIX0` | `0x71` | **`0x23C343E7`** (Pipes 0..3: `E7, 43, C3, 23`) | Canonical Nordic ESB prefixes |
| `PREFIX1` | — | **`0x13E363A3`** (Pipes 4..7: `A3, 63, E3, 13`) | |

## K05929
Source `D0485`, original line79, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

> [!NOTE]
> In nRF52, register `PCNF1`:
> - Bit 24: `ENDIAN` (0 = Little-Endian, 1 = Big-Endian). In Swift ESB, it is set to 1 (`0x01000000`).
> - Bit 25: `WHITEEN` (0 = Disabled, 1 = Enabled). In both modes, it is equal to 0 (hardware whitening is disabled).

## K05930
Source `D0485`, original line86, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. Addressing and unraveling address `31:32:33:33`

## K05931
Source `D0485`, original line96, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Disassembly of the tracker firmware `Swift 2.0` (VA `0x8ac4..0x8acc`) confirmed:
- At address VA `0x16515`, the string constant `\"1233\\0\"` (`0x31, 0x32, 0x33, 0x33, 0x00`) is located.
- The telemetry packet assembly function reads this word:

## K05932
Source `D0485`, original line107, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### B. Default base addresses (Nordic ESB)
- **Pipe 0 (service channel / discovery):**
  - Prefix: `PREFIX0[0] = 0xE7`.
  - Base address: `BASE0 = 0xE7E7E7E7`.
  - Full 5-byte on-air address of Pipe 0: `0xE7, 0xE7, 0xE7, 0xE7, 0xE7`.
- **Pipe 1..7 (communication channels):**
  - Base address: `BASE1 = 0x43434343`.
  - Prefixes: `0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13`.

## K05933
Source `D0485`, original line116, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### C. Dynamic over-the-air tracking address (BASE1)
The operating address `BASE1` and prefix `PREFIX1` are generated during the pairing stage uniquely for each "Station — Tracker" pair:
1. The station transmits its 4-byte identifier (`peer_seed`, on the headset equal to `[PRIVATE_LINK_VALUE]` / `74:73:54:15`).
2. The tracker reads its unique chip hardware identifier from Nordic FICR `DEVICEID[1]` (at address `0x10000064` via VA function `0x7d18`).
3. Both sides perform byte-by-byte addition modulo 256:
   $$\text{combined}[i] = (\text{peer\_seed}[i] + \text{device\_id}[i]) \pmod{256} \quad \text{for } i \in \{0, 1, 2, 3\}$$
4. The resulting 32-bit word is bit-inverted by the ARM `RBIT` instruction (VA function `0x58f8`).
5. The result is written to register `BASE1` on the station and tracker, and the agreed `peer_prefix` into `PREFIX1`.

## K05934
Source `D0485`, original line155, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Pairing Beacon / Request from the station (9 bytes):**
   - Length: `0x09`.
   - Header and pairing request markers.
2. **Tracker Identity Response from the tracker (13 bytes, VA `0x6844`):**
   - `Byte 0`: Length = `0x0D` (13 bytes).
   - `Bytes 1..2`: Type field `0x0002`.
   - `Bytes 3..4`: Protocol version field `0x000A`.
   - `Bytes 5..8`: **Tracker hardware chip ID (Nordic FICR `DEVICEID[1]`)**.
   - `Bytes 9..12`: Battery status, bracket status, and hardware revision.
3. **Network Configuration from the station (17 or 21 bytes, VA `0x68f0`):**
   - `Byte 0`: Length (`0x11` = 17 or `0x15` = 21).
   - `Bytes 7..11`: **40-bit operating frequency mask** (even channels 2..80).
   - `Bytes 12..15`: **Station `peer_seed`** (stored in tracker RAM `0x20000fd8`).
   - `Byte 16`: **`peer_prefix`** (stored in tracker RAM `0x20000fd0`).

## K05935
Source `D0485`, original line176, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### A. AP SPI interface (`/dev/stationdev`)
- **Linux bus:** `/dev/stationdev` node (Qualcomm QUP SPI master to nRF52 SPIS slave).
- **Interrupt:** GPIO 84 (`station`).
- **Handlers in MCU firmware:** `ap_spis_event_handler`, `ap_spi_recv_analysis`, `ap_spi_reply`, `ap_spi_report`.
- **Purpose:** Asynchronous control commands: pairing, calibration, unbinding (`unbond`), power status, OTA firmware update (`tracker_ota_process`).

## K05936
Source `D0485`, original line182, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### B. BP SPI interface (Baseband Processor / Qualcomm Hexagon SLPI DSP)
- **Handlers in MCU firmware:** `bp_spis_event_handler`, `bp_spi_send_with_timestamp`, `tracker_imu_info`.
- **Purpose:** High-speed synchronous delivery channel of IMU data with a frequency of 200 Hz.
- **Implementation features:**
  - Each packet received over the radio is tagged with a hardware timestamp from the station timer `time_ticks0` and transmitted via SPI directly to the Qualcomm Hexagon DSP sensor processor (SLPI).
  - The path is completely isolated from the Linux / Android kernel scheduler, eliminating OS latency when transferring motion.
  - Synchronized with the tracker's infrared LED strobes via the camera frame synchronization signal (`fsin_count`).

## K05937
Source `D0485`, original line194, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

From the Hexagon DSP, packets are transferred via the FastRPC interface (`mSlpiDataReader` / `SnsLowLatBridge`) to `libpvrtrackingservice.so`.  
The main parsing is performed by the function `pvr::SlpiDataParser::trackerDataParserV2(slpi_payload_data_t*, unsigned long)` (virtual address `0x96e44`, size 1004 bytes).

## K05938
Source `D0485`, original line197, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Byte-by-byte IMU payload layout

## K05939
Source `D0485`, original line222, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

> [!IMPORTANT]
> **Critical finding:** The Pico Swift 2.0 tracker transmits a **ready orientation quaternion (4 $\times$ float32)** in each telemetry packet (offset `0x1B`). The host does not need to perform resource-intensive EKF fusion of gyroscope and accelerometer to obtain tracker orientation.

## K05940
Source `D0485`, original line227, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 7. Practical guide for reception on Steam Frame

## K05941
Source `D0485`, original line229, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

To receive Pico Motion Trackers on the onboard Nordic Roy chip of the Steam Frame headset (without reflashing Roy flash and without losing Valve controllers):

## K05942
Source `D0485`, original line231, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Radio receiver configuration (ESB RX Profile):**
   - Framing: `PCNF0 = 0x00040008` (Length=8, S1=4, S0=0, Preamble=8).
   - Bit order and whitening: `PCNF1` with `ENDIAN = 1` (Big-Endian), `WHITEEN = 0` (whitening disabled).
   - Checksum: `CRCCNF = 0x00000002` (16-bit CRC, $SKIPADDR=0$), polynomial `0x00011021`, init `0x0000FFFF`.
2. **Traffic capture:**
   - For searching: listen on startup channels `1, 37, 77` with Pipe 0 address `0xE7, 0xE7, 0xE7, 0xE7, 0xE7`.
   - For paired tracker: the address is calculated via `BASE1 = RBIT(peer_seed + DEVICEID[1] mod 256)`.
3. **Telemetry parsing:**
   - Upon receiving a packet, read 4 `float` numbers from offset `0x1B` (orientation quaternion $[w, x, y, z]$).
   - Transmit tracker pose to SteamVR via VMT / OpenVR driver.

## K05943
Source `D0485`, original line245, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 8. Tracker ID storage architecture in Station Flash/RAM and diagnostic interfaces

## K05944
Source `D0485`, original line247, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 8.1. Storage of paired records in Flash (pages 116–118: `0x00074000`–`0x00076FFF`)

## K05945
Source `D0485`, original line249, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the `PhoenixSta` station firmware, tracker pairing is serviced by descriptor 2 of the `nrf_fstorage` subsystem (located in RAM at address `0x20000028`, flash range `0x74000..0x76FFF`, ring buffer of size 12 KB / 3 pages).

## K05946
Source `D0485`, original line251, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The flash initialization and scanning function (VA `0x16A9C`, file offset `0x15A9C`) performs sequential reading of fixed-size records of **16 bytes (`0x10`)**:

## K05947
Source `D0485`, original line262, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Marker check:** When reading each 16-byte record, the condition `record->magic == 0xAA55AA55` is checked (instruction at VA address `0x16B08`: `cmp r3, r2`, where `r2 = 0xAA55AA55`).
2. **Next record index:** The current number of written flash blocks is saved at RAM address `0x20008F06` (`record_count <= 0x2BC` = 700 records). When the ring is full, rotation and page erasure occur via VA function `0x171FC` (`nrf_fstorage_erase`).

## K05948
Source `D0485`, original line267, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 8.2. Table of active pairings in station RAM (RAM `0x20008EC0`)

## K05949
Source `D0485`, original line269, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Upon station startup and upon completion of the binding procedure, valid hardware tracker IDs are transferred to an array of active slots in RAM located at address **`0x20008EC0`**.

## K05950
Source `D0485`, original line271, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The array is designed for **5 slots** with a step of **10 bytes (`0x0A`)**:

## K05951
Source `D0485`, original line273, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Slot | Address in station RAM | Structure fields (10 bytes) |
|---|---|---|
| **Slot 0** | `0x20008EC0` | `+0x00..+0x03`: `uint32_t tracker_device_id` (FICR `DEVICEID[1]`)<br>`+0x04`: `uint8_t is_valid` (1 = paired/active, 0 = empty)<br>`+0x05..+0x09`: status flags and metadata |
| **Slot 1** | `0x20008ECA` | `+0x00..+0x03`: `uint32_t tracker_device_id`<br>`+0x04`: `uint8_t is_valid` |
| **Slot 2** | `0x20008ED4` | `+0x00..+0x03`: `uint32_t tracker_device_id`<br>`+0x04`: `uint8_t is_valid` |
| **Slot 3** | `0x20008EDE` | `+0x00..+0x03`: `uint32_t tracker_device_id`<br>`+0x04`: `uint8_t is_valid` |
| **Slot 4** | `0x20008EE8` | `+0x00..+0x03`: `uint32_t tracker_device_id`<br>`+0x04`: `uint8_t is_valid` |

## K05952
Source `D0485`, original line283, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 8.3. Calculation of the station operating radio address BASE1 (VA `0x0000E5DA`)

## K05953
Source `D0485`, original line285, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

To receive data from a paired tracker, the station selects logical pipe 1 (`RADIO.PREFIX0` byte for pipe 1 = `0xC0`):
1. The station reads 4 bytes of `tracker_device_id` from the RAM table `record + 8`.
2. Reads 4 bytes of its hardware seed `peer_seed` from `0x2000492E` (value `[PRIVATE_LINK_VALUE]` / `74:73:54:15`).
3. Adds them byte-by-byte modulo 256:
   $$\text{sum}[i] = (\text{peer\_seed}[i] + \text{tracker\_device\_id}[i]) \pmod{256}$$
4. Writes the result to `RADIO.BASE1` via the bit reversal function (`RBIT`).

## K05954
Source `D0485`, original line292, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This **completely matches** the formula found on the tracker firmware `Swift2.0` side.

## K05955
Source `D0485`, original line296, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### 8.4. Diagnostic channels and reading tracker IDs via host

## K05956
Source `D0485`, original line298, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### 1. Deconstruction of artifact "1233"
The utility `tracker_test` (VA `0x07310`) reads the 64-byte status `tracker_status_t` and outputs offset `0x24` via string:

## K05957
Source `D0485`, original line303, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In the production firmware `Swift2.0` (VA `0x16515`), offset 3 of the status payload is filled with the constant ASCII string `"1233"` (`0x31, 0x32, 0x33, 0x33`). Pico developers filled this field with a static protocol marker, causing the utility to display `"addr: 31:32:33:33"`. This confirms that the value `1233` is not a physical radio address.

## K05958
Source `D0485`, original line305, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### 2. ID output via station hardware Keylog
In the `PhoenixSta` optical synchronization subsystem (VA `0x0E8AA`), logging is invoked:

## K05959
Source `D0485`, original line310, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

where bytes `record[8]` and `record[9]` (lower 16 bits of `tracker_device_id`) are passed as arguments.
In the actual headset system log (`logcat`), the following line was recorded:

## K05960
Source `D0485`, original line315, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

which unambiguously reveals the lower bytes of tracker ID 0: `0x3E, 0xED` (i.e. `0xED3E`).

## K05961
Source `D0485`, original line317, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### 3. Whitelist storage in the Android system
The `stationservice` service and the `libstationopticsservice.so` library store the whitelist of paired trackers in a JSON file:

## K05962
Source `D0485`, original line322, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The `_GetTrackerWhiteList` method reads serials, versions, and paired IDs.

## K05963
Source `D0485`, original line324, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

#### 4. Discovery of full 32-bit Chip ID in the Discovery procedure
The callback function `device_discovery_callback` in `tracker_test` (VA `0x06514`) accepts the `device_discovery_data_t` structure and extracts the full 4-byte `chip_id`:

## K05964
Source `D0485`, original line334, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Format string:

## K05965
Source `D0485`, original line338, SHA256 `3988fa0a609602a4bad5bbb89a1d57d42f851c0cc90c58e27a8ba33d71e78347`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This provides a direct software method for obtaining the 32-bit `chip_id` of any tracker while in pairing mode.

## K05966
Source `D0506`, original line1, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Swift cycle: verified sections and polling probe

## K05967
Source `D0506`, original line3, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-06, studies 174–175. The static image Swift-image.bin has
load base 0x1000. Addresses below are VA. The utility `pc/radio/swift_fw_disasm.py`
resolves PC-relative literals and does not automatically determine data boundaries.
In conclusions, do not use the literal pool as executable instructions.

## K05968
Source `D0506`, original line8, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Radio setup

## K05969
Source `D0506`, original line10, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- `6DC8..6DD2`: address is calculated from seed in `20000FD8` and FICR cache
  `20000EFE`, then `58F8` writes RBIT32 of the result to RADIO BASE1 (+520).
- `6DD6..6DDE`: `59A4` receives byte value `20000FD0` and pipe index 1.
  `592C` saves prefix in config `20000080+8+pipe`; `51EC` does
  RBIT32+REV for two prefix words and writes PREFIX0/PREFIX1.
- `6DE2..6DE4`: `595C(2)` saves **RXADDRESSES mask 2**, not TXADDRESS 2.
  Proof: `595C→config+12`; `577C..577E→RADIO+530`.
  This means RX pipe1 is enabled. Previous conclusion "TX pipe2" has been corrected.
- `597C` saves FREQUENCY in config+13, only when radio-state allows
  change; `5782..5784` transfers it to RADIO+508 on RX start.

## K05970
Source `D0506`, original line21, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Table and frequency

## K05971
Source `D0506`, original line23, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- `6E74..6E92`: after update flag from `20000E4C`, the table is copied
  from `20000E10+19` to `+5`, length from `+3` to `+2`; index `+36` is reset.
- `6EBE..6EDA`: table[index] is selected, `597C` is called, index
  is incremented modulo count. This is the confirmed table traversal order.
- Before this `6E20..6E38` checks counter `20000EF0` against
  `200/(byte[20000F21]+1)` and additional mode byte `20000F6A`.
  Therefore, an unconditional hop at a frequency of200Hz cannot be assigned to each IMU packet.
- The actual values of these MCU RAM fields have not been read. Live mask and
  absolute switching time have not been recovered.

## K05972
Source `D0506`, original line33, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Response and command

## K05973
Source `D0506`, original line35, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Callback `5A08` splits paths by byte `20000E10` and checks CRCSTATUS.
- In the nonzero path after CRC, there is response preparation via `970C`, `95C4`.
  `5B0E..5B12` copies RXMATCH to TXADDRESS. The response is addressed to the pipe
  on which the packet was received.
- `5B1A` sets PACKETPTR of the response, `5B2A` launches TASKS_START;
  **only then** `5B40` calls command parser `94FC`.
- `512C..515A` after the response restores PACKETPTR of the input buffer and callback
  `5A09`. This confirms the RX→response→RX cycle, but does not prove that any
  empty Frame packet triggers this path in the tracker's current state.
- Tracker state, expected request structure, S1, and time window must
  be confirmed by a separate capture. "This is just unsolicited TX" and
  "any empty poll is sufficient" are not yet established protocol.

## K05974
Source `D0506`, original line48, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Active limited probe 175

## K05975
Source `D0506`, original line50, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

A separate `ROY_SWIFT_POLL`/tag `ROYSWIFTPOLL1` was added; loader requires explicit
`--swift-poll`, prohibits mixing passive/IR/multi/sweep flags, and verifies
the confirmed full slot0 CRC profile. It cannot be loaded as passive RX:
flag rejection check is performed before hardware operations.

## K05976
Source `D0506`, original line55, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Transmitted no more than40 packets: application payload length0, S1 PID round-robin,
NO_ACK clear, channels2..80, power0dBm. No setup/IR/pairing commands.
After each transmission, RX in the same dedicated MPSL slot. Flash is not modified.

## K05977
Source `D0506`, original line59, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Result `outputs/poll175.json`:121slots,40attempted,40TXEND,0ADDRESS,
0RXEND,0CRCframes. session_closed_verified,ram_restored_verified,mpu_restored
all true; flash_written false. Presence of tracker response/poll **not confirmed**.
PICO after test Awake/ON; FramePicoTracker overlay1200×700 visible verified.
Coexistence with two active controllers was not verified in this run.

## K05978
Source `D0506`, original line65, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

One cannot infer from a zero result the invalidity of FICR, address, or standalone
hardware capability. It is necessary to determine where the state, request,
or timing mismatched. Do not repeat the same 40-packet probe without altering the hypothesis.

## K05979
Source `D0506`, original line69, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Evidence: `outputs/firmware-standalone/swift-{cycle,timing,radio,pipes,
response,queue}174.txt`, source image, and `outputs/poll175.json`.

## K05980
Source `D0506`, original line72, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Verification of PICO influence and bootstrap, 176–178

## K05981
Source `D0506`, original line74, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

176: PICO temporarily put to sleep via keyevent223, mWakefulnessAsleep before and after
polling, displayOFF after; wake command224 sent. 40emptyTXEND,
zero RX. This does not prove complete shutdown of the Nordic station during Android sleep.
Late status remained Asleep, so one cannot write "PICO woken up successfully".

## K05982
Source `D0506`, original line79, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

177: separate tag ROYSWIFTPOLLNOOP1 and explicit --poll-noop. Application packet
`00 00 00`: full three-byte header, opcode0. In parser94FC..9528 opcode0
exits9512 without calling handler.121slots40TXEND0RX, recovery true.
Incomplete one-byte header is not used: parser reads +1/+2.

## K05983
Source `D0506`, original line84, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Initialization6658: ROM15FDC contains01 25 4D = [1,37,77], ROM15FE0
04 4E 4E = [4,78,78]. 6710..672A copies them to RAM+30 and+2D.
75CC..7604 selects one of three channels+2D, sets prefix3 on pipe0
(7FA4return3;51ECRBIT+REV→C0), RXADDRESSES1. 7574 paired path selects+30
and RXADDRESSES2. BASE0init66AC calls58C4 for785634AA, RBIT552C6A1E.
ROMinit16074 byte+8=4, corresponds to MODE4; do not invent other fields/timing.

## K05984
Source `D0506`, original line91, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

178 passively listened for bootstrap on1/37/77,1021slots0ADDRESS/END, all restore true.
Tracker was not restarted, not rebound; absence of a packet does not mean
that it entered bootstrap. Passive sweep of even operating channels did not check these
odd bootstrap channels. Documents updated, 24 loader/probe tests pass.

## K05985
Source `D0506`, original line96, SHA256 `b126b60b16bceb2ec12fd88de09b6017ce291d50130b5ae4790c2cb3e19d8bb1`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Evidence: poll176-pico-sleep.json,poll177-noop.json,bootstrap178.json;
firmware-standalone/swift-{poll-format176,fallback177,bootstrap177,
bootstrap-tables177,base0-177}.txt. Radio probes were not loaded as GUI/API
autostart. Next step: determine live mode and exact station transition,
do not repeat blind TX and do not reset pairing without prepared capture.

## K05986
Source `D0761`, original line326, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Historical research log

## K05987
Source `D0761`, original line328, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Historical review date: 2026-10-05. This document supersedes earlier claims of
completed reverse engineering and a "100% ready" standalone bridge with verified data.

## K05988
Source `D0761`, original line331, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**Result as of the old review date:** Roy's SRAM code works via the stock shell;
autonomous MPSL callbacks around 100 Hz and TIMER0 were verified with two active
controllers, with MPU enabled. Wi-Fi/SSH access allows disconnecting USB.
PICO RF, a full standalone bridge, and true 6DoF poses have not yet been achieved.
The latest MPU cleanup fix passed a full MPSL retry: 222 START,
337 lines each of HMD and both controller poses; recovery is confirmed.

## K05989
Source `D0761`, original line338, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## User requirements and the role of documents

## K05990
Source `D0761`, original line340, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- The final solution runs entirely on the Steam Frame, without an external dongle and
  without the PICO headset as a permanent receiver/compute device.
- Do not reflash the Frame, Roy, or trackers; do not alter flash/images of stock firmware.
- The user specifically permitted a temporary module in Nordic SRAM via SWD,
  which disappears after a reboot. Temporary MPU/CPU changes were used in the test.
- Preserve connectivity of stock Valve controllers. Their presence after recovery
  does not equate to proof of concurrent radio exchange during the experiment.
- Both connected headsets are available for research and comparative measurements.
- Initial plans and hardware summaries are research materials. Their claims
  regarding readiness and protocols do not replace actual code, dumps, and hardware tests.
- Project rules and wire protocol: `K:\AI\pico\AGENTS.md`. The original SDK
  `K:\AI\pico\pico-sdk` is not edited.

## K05991
Source `D0761`, original line353, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Both connected headsets: verified in this update

## K05992
Source `D0761`, original line355, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Device | USB ADB serial | Live data | Access |
|---|---|---|---|
| PICO 4 Enterprise | `[HEADSET_ADB_SERIAL]` | model A8110, Android 10, build/display 5.13.7 | regular ADB shell uid=2000; root later confirmed via hashsu.sh — see the current summary |
| Steam Frame dev kit | `frame` | SteamOS VR 0.4.3, BUILD_ID 20260930.6234839; kernel 6.18.0-ge66bc2ca6f8c, aarch64 | ADB shell already uid=0 |

## K05993
Source `D0761`, original line360, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

When multiple devices are present, always specify `-s`:

## K05994
Source `D0761`, original line368, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

According to the SSH details provided by the user: `ssh steamos@[DEVICE_IP]`,
without password; Devkit and steamos-devmode are enabled. These SSH settings
were not re-verified in this update. TCP ADB is absent; ADB is used
via USB, and SSH is intended for Wi-Fi. Access setup is described in
`steam-frame-adb-dev-setup.md` (reference to a user artifact,
its content here does not serve as independent confirmation).

## K05995
Source `D0761`, original line375, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

PICO is currently needed to read supplied files, analyze the SDK, and obtain
reference data. Its connection does not waive the standalone criterion of the final solution.

## K05996
Source `D0761`, original line378, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Detailed audit and hardware results

## K05997
Source `D0761`, original line380, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Objective and actual result

## K05998
Source `D0761`, original line403, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Where the layer should be located

## K05999
Source `D0761`, original line427, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Prepared tool

## K06000
Source `D0761`, original line462, SHA256 `19092c81851da405af25ab88b783543cf26b5efd8c55a05c2edb18a7850d777e`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Verified Frame data and SRAM experiment
