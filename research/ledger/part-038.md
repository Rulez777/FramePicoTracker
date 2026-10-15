# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K02961
Source `D0394`, original line13, SHA256 `7ea10c9c7037595367991a1d38ba024125bd921a7d10355c0d075f05a4ac1842`. Status: historical/unadjudicated narrative; English source prose.

For the button interface, keep this tunnel running and open the local URL:

## K02962
Source `D0394`, original line19, SHA256 `7ea10c9c7037595367991a1d38ba024125bd921a7d10355c0d075f05a4ac1842`. Status: historical/unadjudicated narrative; English source prose.

Open `http://127.0.0.1:18741/`. “Record diagnostics” runs a bounded 10-second
passive slot0 full-address capture and records the Frame/controller poses.
It never transmits IR commands. Concurrent API captures are rejected.
Do not run other SWD tools while a capture is active.

## K02963
Source `D0394`, original line24, SHA256 `7ea10c9c7037595367991a1d38ba024125bd921a7d10355c0d075f05a4ac1842`. Status: historical/unadjudicated narrative; English source prose.

Endpoints: GET `/status`, POST `/capture`, GET `/capture/ID`,
GET `/capture/ID/bundle`, GET `/dataset/DATASET_NAME/FILE_NAME`.
Artifacts include raw DMA, hardware CRC diagnostics, host read timestamps,
Frame/controller pose CSV, loader logs, restoration checks and dataset inventory.
The restoration checkpoint is kept on Frame and excluded from the download bundle.

## K02964
Source `D0394`, original line30, SHA256 `7ea10c9c7037595367991a1d38ba024125bd921a7d10355c0d075f05a4ac1842`. Status: historical/unadjudicated narrative; English source prose.

Camera recording still starts through SteamVR Developer → Record Tracking Data.
The service inventories and downloads recorded files; the button does not start
the cameras. Dataset clocks, radio arrival clocks and Frame pose clocks are not
yet aligned. Packet timestamps currently mark host SWD reads, not RF arrival.
`usable_radio_capture=false` means no usable packet sequence was obtained.
No tracker world-position or 6DoF output is claimed.

## K02965
Source `D0394`, original line37, SHA256 `7ea10c9c7037595367991a1d38ba024125bd921a7d10355c0d075f05a4ac1842`. Status: historical/unadjudicated narrative; English source prose.

Camera recording update: capture now uses verified HMD start_tracking_recording 1 0 0 and stop_tracking_recording, automatically. New camera datasets are included in the bundle. This supersedes the manual-camera limitation above.

## K02966
Source `D0395`, original line1, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

# Multi-profile RX, raw telemetry and camera recording — 2026-10-06

## K02967
Source `D0395`, original line18, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

This supersedes the previous claim that controller coexistence and raw telemetry
decoding were wholly untested. The final standalone 6DoF goal is still unmet.

## K02968
Source `D0395`, original line21, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Radio evidence

## K02969
Source `D0395`, original line23, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

`ROY_SWIFT_MULTI` cycles all three shortened profiles within one temporary SRAM
session, alternating channels 2 and 80. The module is 1180 bytes. No flash writes,
TX, ACK, pairing reset, HID service stop or IRQ vector changes were performed.
Runs 97 and 98 closed the MPSL session and restored saved RAM/MPU state.

## K02970
Source `D0395`, original line28, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

- Run 97: 1022 slots, 25 frozen-state software CRC matches, profile counts 8/9/8.
- Run 98: 1023 slots, 14 software CRC matches, profile counts 5/5/4. SteamVR pose
  logs contained 1086 rows for each of HMD and both Valve controllers during this
  capture. This demonstrates coexistence during this bounded test, not guaranteed
  uninterrupted service or sufficient tracker sampling rate.
- Learned CRC states remain 7F24/1ABA/0DAB. They were frozen before these captures.
  Full hardware CRC address reception and full FICR identity remain unverified.
- Accepted telemetry is below one packet/second/profile in these tests, far below
  the rate needed for IMU tracking. A 200 Hz transmitter claim does not establish
  a 200 Hz receiver. Do not publish these samples as real tracker poses.

## K02971
Source `D0395`, original line39, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Verified raw sensor layout

## K02972
Source `D0395`, original line41, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

Swift AD08..ADD4 queues an 18-byte block: LE signed gyro xyz (offset 0), LE signed
accel xyz (6), signed temperature (12), LE 24-bit timestamp (14), counter (17).
Sensor command payload has a three-byte header, command byte 1 = 2 and byte 2 =
block length 6/18/24. Data begins at byte 3, not byte 4.

## K02973
Source `D0395`, original line46, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

The six-byte block is a magnetometer block, NOT a quaternion. Swift B3D8..B432
reads the six bytes and 9778 retrieves them. A 24-byte block appends it after IMU.
Temperature conversion is raw/132.48 + 25 C. Runtime gyro/accel scales and timer
units remain unverified; preserve raw values. Preserve trailing bytes without
assigning an unsupported battery or other meaning. The SLPI float quaternion
layout does not prove a quaternion exists in this radio payload.

## K02974
Source `D0395`, original line53, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

`radio.swift_capture_decode` combines frozen CRC validation with the sensor
parser and rejects partial/missing-profile/truncated records. It ran on Frame
against capture98: 13 parsed sensor packets, counts 5/4/4; one additional valid
CRC packet had an unsupported sensor format, and two partial records were rejected.
It explicitly emits no position/orientation. The full PC test suite passes 72 tests.

## K02975
Source `D0395`, original line59, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Camera observations

## K02976
Source `D0395`, original line61, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

Current OpenVR reports HasCamera=true but NumCameras=0 and GetCameraFrameSize
returns error100; no stream was acquired. `/dev/video99` supplied a black frame.
Neither result proves all camera routes unavailable.

## K02977
Source `D0395`, original line65, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

The legacy vrcmd lighthouse recording call returned success but created no dataset.
Its temporary camera-recording setting was restored. The user then recorded through
the native VR Settings Developer UI, producing:
`/home/steamos/.local/share/Steam/config/cv/xrservice/datasets/Deckard_2026-10-06_08-31-30.dataset`.

## K02978
Source `D0395`, original line77, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Remaining implementation

## K02979
Source `D0395`, original line79, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

Increase and measure useful RX rate while preserving both controllers; validate
runtime sensor scales and timing; decode real camera frames and LED observations;
establish a safe live camera path; implement standalone tracker heartbeat and
optical/inertial fusion plus the Frame SteamVR driver transport. PICO root is not
needed for the current passive route. PICO dependence has not yet been eliminated.

## K02980
Source `D0395`, original line85, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Camera extraction verified, 2026-10-06 (supersedes undecoded OCC status above)
OCC records use magic occipitl + LE uint32 kind and payload size. Observed kinds
32/38 contain repeated LE uint32 key/size TLVs. Camera selector key17 is one byte;
key14 contains SPS/PPS NALs, key13 contains big-endian length-prefixed H264 NALs.
Converting these to Annex B and decoding with ffmpeg produced a genuine grayscale
1024x1024 tracking-camera frame (observed selector8) and another selector10 stream.
Frame selector8 image was visually inspected: real room and hand-held device,
not rendered VR screen. Selector numbers are empirical, not proven camera names.
No PICO LED identification or live camera access has been verified yet.
Reproducible bounded offline extractor: pc/radio/frame_occ_extract.py.
Recording was still growing when inspected (approximately650MB); user was asked
to press STOP. Do not parse it as a complete immutable file until stopped.
Evidence: outputs/occ-cam-8.png. No hardware/service configuration was changed.
User confirmed STOP. The final OCC file is801910658 bytes; a read-only full record
scan reached exactly EOF without truncation. Last-record H264 contains interframes;
standalone ffmpeg decoding produced no image. Use a sequence beginning with an IDR
before drawing conclusions about the end-of-recording tracker visibility.

## K02981
Source `D0395`, original line103, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Review of Gemini SLPI V2 addition — 2026-10-06
The new decode_swift_telemetry_v2 function describes a researched SLPI consumer
layout; it is NOT evidence that the same layout is transmitted over Swift RF.
The former length>=46 automatic dispatcher was unsafe: packet length alone cannot
identify this transport. unpack_swift_telemetry now defaults to captured radio
sensor framing and requires explicit transport='slpi_v2' for that research path.
Flagged SLPI quaternions must additionally be finite and approximately unit norm;
this sanity check does not establish component order, coordinates or live origin.
Synthetic V2 tests prove code behavior, not a captured on-air quaternion.
Frame execution against unchanged capture98 still yields13 sensor packets (5/4/4),
zero available orientations and no positions. Full test suite:81 passed.
Do not connect MCU_Q fields to optical fusion until a real validated source and
quaternion conventions are proven. The optical engine expects xyzw and SI accel;
radio gyro/accel scales are still unverified. CameraIntrinsics currently assumes
pinhole, whereas recorded calibration includes kb_spline: distortion/crop/stream
mapping must be implemented before using factory calibration for metric PnP.
37us exposure and identification of PICO LED markers were not independently
verified in this review. Live camera access and sustained RF rate remain open.

## K02982
Source `D0395`, original line122, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Tail camera sequences and real IR detection, 2026-10-06
Added read-only frame_occ_tail.py: retains at most240 records per kind32/38,
separates repeated camera selectors by occurrence ordinal, and emits H264
sequences (not isolated interframes). Ran it directly on Frame on the completed
801910658-byte dataset. Outputs contain2 selector8 streams and4 selector10
streams; these ordinals are not yet mapped to factory camera names.
Decoded selector10 stream0 using ffmpeg: raw grayscale image is1056x1024
(1081344 bytes). Earlier displayed resized preview must not establish1024width.
Existing BlobDetector at threshold120/min_area2 found12 compact bright spots,
visually in two contours. This proves real IR blob detection, not PICO identity
or metric pose. User was asked where PICO trackers were located in this capture.
Evidence: outputs/occ-tail-ir.png, outputs/occ-tail-ir.gray,
outputs/occ-tail-ir.blobs.json. No camera/firmware/service configuration changed.
Latest full test suite86 passed; other contributors may be modifying tests.

## K02983
Source `D0395`, original line137, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Fixed80 RX and primary camera model, 2026-10-06
Multi SRAM receiver now accepts explicitly reviewed frequency80 to stay on80
while cycling all three profiles; frequency2 retains the old2/80 alternation.
Slot duration2000us and request interval10000us are unchanged. Run100:
loader0, 19 parsed IMU/mag packets counts9/6/4 during10s, plus6 CRC-valid
unsupported service packets and3 partial records. Both stock controllers logged
poses (HMD1075, controller1=1045, controller2=1075 rows). All session-close,
RAM/MPU/count restoration checks true; flash_written=false. This is a bounded
coexistence result, not proof of uninterrupted controller tracking or adequate
tracker rate. Radio reception remains too sparse for pose fusion.
New FrameKBCamera calculates camera-frame unit rays with polynomial KB distortion
inversion and rejects unsupported spline/rotation/nonfinite data. It does not
apply guessed stream mapping, crop, thermal model or OpenVR extrinsics.
Primary calibration actually has slam_right/left kb1056x1024, upper_left/right
kb640x480; kb_spline is an alternative calibration, not the primary four cameras.
The KB polynomial follows OpenCV fisheye documentation; the Valve-specific
coordinate and stream conventions still require verification.
User identified a visible ring on the table as a PICO CONTROLLER. Do not call
these observations Motion Tracker markers. Requested a short isolated recording
of an actual enabled Motion Tracker with the PICO controller removed from view.
Tail extractor, real blob detections and candidate KB rays are offline pipeline
components; live raw-camera transport and standalone optical poses remain open.

## K02984
Source `D0395`, original line160, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Orientation and spline helper review, 2026-10-06
Gemini imu_orientation.py contained invented fallback samples (zero gyro and
1g accel) when packets omitted IMU, and assumed5ms timing. Removed those fallbacks;
manager now requires real gyro/accel fields and explicit finite0<dt<=50ms. Sparse
current RF samples must not be integrated with an invented5ms interval. Added
regression for missing IMU/timing and sparse gaps. Madgwick local reference is
+Z-up, not OpenVR+Y-up; magnetic heading/world alignment is not implemented.
1024LSB/g is an observed provisional conversion, not verified sensor range/config.
The kb_spline helper uses assumed bilinear residual interpolation and guessed
crop convention; self round-trip tests do not validate Valve's spline basis.
Keep it research-only until compared with native projection or calibrated targets.
A new isolated recording08-47-20 was captured (38332520byte OCC). First and later
visible/IR frames still show a hand-held ringed controller. The suspected Motion
Tracker on the tabletop has no identifiable IR constellation in those inspected
frames. This does not prove its LEDs never activate: temporal/other-view checks
are still required. Evidence: outputs/tracker-camera101-ir-later.png and visible.
Standalone goal remains unachieved; never publish an artificial pose from these
research helpers merely because they return a normalized quaternion.

## K02985
Source `D0395`, original line179, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Local diagnostic agent installed, 2026-10-06
User requested a Linux debugging program on Frame. Installed and launched
pc/radio/frame_debug_agent.py in /home/steamos/pico-ram-work. HTTP endpoint binds
127.0.0.1:18741 only; access through SSH curl http://127.0.0.1:18741/status.
Returns completed capture reports and offline camera scan summary, never claims
cached reports are live device presence. No arbitrary command execution or radio
write API. No root, system service modification, flash or camera acquisition.
PID/log: frame-debug-agent.pid and frame-debug-agent.log in that directory.
Stop with kill of that saved process PID; not configured for startup at boot.
Read-only IR scanner frame_ir_scan.py processed226 frames of camera101 selector10
stream0:6..7 bright components per frame. Visible/IR images associate this contour
with a hand-held ringed controller. Motion Tracker constellation still unproven.

## K02986
Source `D0395`, original line192, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Channel mask independently checked, 2026-10-06
Disassembled extracted Swift image (base1000) VA7508..7568. It expands input
bytes1..5 into selected channels2*(bit_index+1), bits processedLSB first, across
40bits. This proves supported channel-mask encoding, NOT that the live mask is
FFFFFFFFFF, hop frequency is200Hz, or next-channel algorithm is linear.
VA74C8 permutes five input bytes through a table; VA7554..7562 passes a runtime
byte modulo24 as its selector. Exact hopping scheduler and live mask remain open.
Added pure swift_channel_mask.py and bit-order/endpoints regression test.
The channel80 conditional in roy_ram_rx.c was this chat's reviewed change for
run100 (19parsed sensor packets/10s, all restoration checks true), not an
unverified Gemini change. Current callback saves a bounded frame per slot;
100slots/s is a current capture-architecture limit, not a universal radio ceiling.
Do not widen duty cycle or add TX/light-sync until coexistence/protocol is checked.

## K02987
Source `D0395`, original line206, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Gemini hopping/light report independently checked, 2026-10-06
Swift6EBE..6EDA selects table[index] (context+5) then increments index modulo
context byte+2. Confirms cyclic selection on this executed path, not its cadence
or current live mask. Full40-channel use and one hop per transmitted packet still
require caller/timing verification. Do not infer them from matching capture counts.
Critical omitted condition: Swift8D40 reads mode from input[0], accepts1/2,
then compares FOUR bytes at input+2 with RAM20000EFE using memcmp14958.
8D68 literal points to that cached tracker FICR location. Only a successful
comparison calls78F4 and tail-calls7C44. Thus this handler is NOT a one-byte
broadcast command; enabling light through this path requires correct additional
identity bytes (and outer framing/opcode must be verified). No RF TX performed.
Therefore full FICR is optional for passive IMU filtering but potentially required
for active standalone light control. A PICO-assisted optical experiment may prove
camera visibility; it is not an acceptable final standalone dependency.
Absence of startup calls in a simple xref search does not prove all runtime
states stay dark without PICO. Boot/callback paths and timing must be checked.

## K02988
Source `D0395`, original line223, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Audit of new full-ID/root claims, 2026-10-06
User reports Pico root and IDs EEACED3E/EEADDFC3/EEAE1DAD. Primary root session,
calibration dump or MCU memory extraction containing these values was not found
in K:/AI/pico or H:/_other/AI/pico_tracker_research. ADB currently lists no devices;
Frame SSH[DEVICE_IP] times out. This is unavailable verification, not denial
that the other session obtained root. Requested USB reconnect/wake and dump path.
Definite inconsistency: claimed slot1 ID bytesC3 DF AD EE disagree with independently
loggedC3 BF and supplied table itself. Under current conditional seed/address
model it gives tail4A EC80, not captured4C EC80. Canonical predicted CRC states
EB84/E1B7/4D21 differ from frozen captured7F24/1ABA/0DAB. These comparisons assume
current byte/bit order and seed and are not absolute identity disproof.
Added swift_identity_audit.py; outputs/claimed-ficr-audit.json retains unverified
claims and mismatches. No TX commands formed/sent with these IDs.94tests pass.
Root metadata JSON has only slots/SNs/versions/bonds and cannot prove full FICR.
Seed bytes[PRIVATE_LINK_VALUE] have LE integer15547374; don't label that integer[PRIVATE_LINK_VALUE].
Swift8D88 comparison and failure branch independently checked again: wrong ID
branches to6588, success enters78F4/7C44. Outer RF dispatch/opcode still needs
verification. Dispatcher8D6C uses16byte entries at RAM20001868;8D88 Thumb pointer
occurs in ROM91EC, not a directly proved opcode1A record at9218. Locate table init
and caller framing before transmitting. Firmware flash remains unchanged.

## K02989
Source `D0395`, original line244, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Root verified and real factory calibration extracted, 2026-10-06
ADB PICO [HEADSET_ADB_SERIAL] and Frame SSH[DEVICE_IP] are reachable again.
Read /sdcard/hashsu.sh before use. Bounded read-only commands through it return
uid0 root, groups0/1003/1006, contextu:r:pvrtrackingservice:s0. Ordinary adbd shell
remainsuid2000; this is not unrestricted root/adbd-root. Several /sys and /proc
reads remain denied under this context. No SELinux change performed.
Exact metadata saved outputs/pico-tracker-info-root.json; slots1/2/3 map current
SNs110865G/110282G/111064G, versionsTR018903, bonds1. No full FICR field.
Exact /data/stationservice/calibration_data.json saved outputs/pico-calibration-root.json.
It contains FIVE base64 records of660bytes: two older SNs[TRACKER_SERIAL] and
[TRACKER_SERIAL] plus current three. Decoded binaries saved by SN under
outputs/pico-calibration-root/. Marker at204 isLE1/60,24LED floats at212.
Slot0 LED coordinates match existing DEFAULT_LED_POSITIONS. Added explicit
serial-based pico_factory_calibration.load_led_positions, verified8x3 for all
three currentSNs; no selection by JSON array index. Units follow current researched
meter convention; full parser/IMU calibration is not claimed.94tests pass.
User clarified EEAC/EEAD/EEAE upper ID words were speculative, not dumps.
Slot1 physical bytesC3 BF areLEBFC3, NOT DFC3. Root by itself does not expose
Nordic MCU address20008EC0 as host virtual memory. Arbitrary /dev/stationdev
memory-read command is not verified, do not send guessed SPI opcodes or consume
stock SPI stream. No pairing/flash/TX/state reset performed.

## K02990
Source `D0395`, original line266, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Bounded discovery145/147/148 and station read ABI, 2026-10-06
Rebuilt pico_identity_read.c with NDKclang aarch64-api29 -Wall -Wextra -Werror;
pushed pico_identity_read_verified. Bounded --discover calls SetDevDiscoverySwitch
(1,3) successfully, then(0,3) successfully after12s. No full-ID callbacks observed.
Saved147/148 transcripts. GetWhiteListNum returns-1/count-1, not zero trackers.
Root execution attempt146 failed inaccessible/not found under currentSELinux;
ordinary shell execution works. Root does not imply permission to execute every
shell-owned file. User held one tracker button during tests. Post148 status:
tracker1online0 bind1 cali2; tracker2/3online1 bind1. Don't claim all still online
or infer button action erased bonds. No pairing API invoked by helper.
Swift68F0..691A: requestlength9 path skips6844 full-ID reply when record+4==AAAA.
That gate can explain missing discovery for bonded devices; does not prove every
reconnection path omits full ID. Next inspect ordinary connection03 path.
Host MCUNRF52832::mcu_read at8BB78 is fd/buffer/length with optional select timeout;
8BC8C calls libc read (PLTDA3F0). initSpi8B7A0 callsopen (PLTD9410).
This is SPI message reading, NOT an arbitrary Nordic RAM address read operation.
No verified driver command for20008EC0/8ECA/8ED4 found. Do not confuse root access
to stationdev with a firmware debug-memory command or consume stock RX replies.

## K02991
Source `D0395`, original line285, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## User-authorized delete/re-pair of tracker1 and discovery149, 2026-10-06
User explicitly accepted removing only first tracker[TRACKER_SERIAL] and pairing
it again for raw full-ID discovery. Extended helper --discover [seconds] to a
strict1..180s bound (default12); rebuiltNDKclang warnings-as-errors. Started149
before UI action. Observed slot1online0/bind0/emptySN after delete, while other
two online1/bind1. User pressedPair and device automatically re-bound.
Stopped helper usingSIGTERM; transcript shows scanenable0 and scandisable0,
listener_finished. No discovery callbacks captured. This failed to obtain fullID
rather than proving ID transmission absent. Afterward all three exactSNs are
online1/bind1 again. Pairing was changed ONLY for tracker1 under explicituser
request and restored successfully; do not state pairing was untouched in this test.
Saved outputs/pico-repair-discovery149.txt. Logcat still onlyshows low16ID.
Don't repeat physical deletion until verifying host listener registration/event
routing relative to the UI's pairing flow or preparing raw startup capture.

## K02992
Source `D0395`, original line300, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Discovery listener ABI and candidate hardware check151, 2026-10-06
Local libtrackerclient/libstationopticsservice SHA256 match current PICO system
libraries exactly (23b0eae...87bc1 and2c9d4d9...77afe). Analysis is current binary.
CreateTrackerClient uses StationClient constructor; constructor starts Binder
ProcessState thread pool at35190/357C4 throughPLT59BC0. Missing Binder pool is
therefore NOT a demonstrated bug. SetTrackerDevDiscoveryCallback stores local
fn atclient118. onDeviceDiscoveryEventNotify36ADC copies19bytes from parcel3568,
only calls that fn when device_type==10. By-value19byte C ABI uses indirectx0;
current callback signature matches. Local registration0 alone does not prove a
server event was delivered. No UI/listener-displacement explanation confirmed.
Passive full-address candidate4D0625B8 onchannel80 run151: only1slot,0ADDRESS,
HMD170pose rows/no controllers; not a valid negative address test. Sessionclose,
RAM/MPU restorationtrue, flashfalse. Requested Frame presence/controllers active
while continuing offline work. All three PICO exactSNs online1/bind1 currently.
No additional pairing/IR TX or firmware writes performed.
Server onDeviceDiscoveryEventNotify6E410..6E530 independently inspected: it builds
parcel eventtype0x35, copies19bytes atparcel3568, lockslistener vector atservice90/98
and sends the event to EACH listener throughvtableC8. This routine contains no
last-listener-only/UI-selection or UID filter. Do not explain149 as the pairing
UI stealing the event without further evidence. Delivery before this routine
and registration into this vector remain to verify.

## K02993
Source `D0395`, original line322, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Awake Frame hardware address comparison152/154/155, 2026-10-06
User wore Frame. Passive channel80, slot2000us/interval10000us/duration10s.
152 BASE4D0625B8/PREFIX00:1022slots,0ADDRESS/END. Controller pose rows1056/1086.
153 invocation lacked required raw-profile switches; loader rejected before loading.
154 corrected short-tail multi control:1020slots,8ADDRESS/END; frozen software CRC
sensor decoder accepted4frames (profile0=2,profile1=2,profile2=0), rejected1CRC/length
and3unsupported sensor formats. Both controllers1077pose rows.
155 full alternate bit-order BASE[PRIVATE_LINK_VALUE]/PREFIX00, MODE4, PCNF040008,
[TRACKER_SERIAL], CRCCNF2/poly11021/initFFFF:1024slots,7ADDRESS/END,0CRC errors.
All7 records are captured only after NRF_RADIO CRCSTATUS passes, confirmed in
roy_ram_rx.c. This is hardware confirmation of this full RX address/framing on80,
NOT yet proof of a full FICR ID or autonomous station functionality.
DMA layout: byte0 length, byte1 S1, payload startsbyte2. Two10byte payloads decode
as researched6byte magnetometer command; other lengths0/1/7/11 are not IMU poses.
Do not parse stale DMA bytes beyond announcedlength; full records currently save258B.
155 controller rows1049/1085, HMD1085. 152/154/155 sessionclose/RAM/MPU restoration
all verifiedtrue, flash_writtenfalse. No TX/ACK/IR control/pairing changes.
Evidence outputs/full-crc152.json, short-control154.json and-decoded.json,
full-order155.json and-decoded.json. Next derive exact register-to-canonical-address
mapping and validate identity inversion against known low16 bytes before IR TX.
Standalone6DoF and continuous camera access remain unfinished.

## K02994
Source `D0395`, original line344, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Full address confirmation and conditional ID inversion156, 2026-10-06
Repeated passive BASE[PRIVATE_LINK_VALUE]/PREFIX00 hardwareCRC profile onchannel80:1022slots,
20ADDRESS/19END,0CRCerrors, controller/HMD1079pose rows each. Sessionclose/RAM/MPU
restoration verifiedtrue; noflash/TX/pairing changes. Saved full-confirm156.json.
Inverting researched RBIT32(bytewise(seed+ID)) with seedbytes[PRIVATE_LINK_VALUE] yields
sumbytesB2606D8E and candidate FICR bytes[TRACKER_ID], LEuint327919ED3E.
Known physical low16bytes3EED match. This is stronger evidence than arbitrary
software CRC fitting, but upper16 remain derived under the seed/address model,
not a direct FICR dump or confirmation of command targeting.
Added infer_identity_from_base with explicit identity_verified=false and no
transmission authorization. Saved full-base-identity156.json. Three focused
identity audit tests pass, including lowword mismatch and no authorization claim.
Next verify identity/address association independently and extend hardware profiles
to other trackers before implementing addressed IR commands/standalone station.

## K02995
Source `D0395`, original line359, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Standalone demo preparation, 2026-10-06
User requires ONLY standalone demo, explicitly rejects PICO pose bridge. No promise
of complete6DoF within hour; live optical source and active station role remain blockers.
Native Linux driver now consumes bounded localhost UDP39571 PTP1 slot valid
px py pz qx qy qz qw in calibrated OpenVR standing metres, validates finite/unit
quaternion, invalidates after250ms. Initial trackers disconnected/invalid, replacing
previous fake-valid origin. Compiled ARM64 shared object directly onFrame with
-Wall-Wextra-Werror; exportedHmdDriverFactory confirmed. NOT registered/loaded into
SteamVR yet, no real6DoF input; compile is not a tracking proof.
Re-pulled Swift/Phoenix update images tooutputs/firmware-standalone. Swift container
validated using existing anchors. Command-table init901C usesRAM20001868.
9042 literal91EC=8D89 stored by9090 attable+9C => opcode09 handler+0C.
9146 literal921C=8E19 goeselsewhere; 9138 literal9218=8D41 stored914E attable+168
=> opcode16 handler+08, whereas9186 table+19C is handler8E19 (not8D41).
IMPORTANT: initializer dataflow must be tracked exactly; do not infer0x19 from
an unrelated laterSTRD. 0x09 ->8D88 independently proven. Previous0x1A mapping
is not verified. Parser9500: commandbyte+1, lengthbyte+2, body+3, tablehandler+0C,
sequenceheader low6bits; callback8D88 readsmodebody0 and comparesIDbody2..5.
No IR TX performed. Command table annotated evidence swift-command-table.txt.

## K02996
Source `D0395`, original line379, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## First bounded active Nordic IR probe157, 2026-10-06
Diagnostic agent verifiedrunning; camera remains offline-only. Prepared separately
tagged ROYIRTXPROBE1 ELF (1261B), explicit --ir-tx-probe required. Passive flags
cannot select it. Fixed candidateID[TRACKER_ID], command09body[1,0,ID4], ESB DMA
[length9,S1zero,payloadheader1,09,06,body]. Sends at most40packets, one on each
channel2..80 at0dBm, then passiveRX. This is a laboratory unsolicited TX probe,
NOT a verified ESB ACK-payload delivery engine or complete standalone station.
157:125MPSLslots,40TXattempted/40ENDobserved,0RXresponses. Sessionclose/RAM/count/MPU
restoration alltrue, flashfalse. Postrun vrcmd shows HMD andbothcontrollers poses.
RF transmission completion does NOT prove tracker received or enabledIR. Requested
short Camera Data capture of firsttracker at30..50cm for observable validation.
Agent now exposes recent_active_probes with age and explicit deliveryfalse.
Added full hardwareDMA decoder respecting lengthbyte andS1byte; stale tailbytes
cannot become fabricated sensors. 23focused loader/IR/hardwaredecoder tests pass.
No6DoF poses produced, noSteamVR driver registered, no firmware/pairing changes.

## K02997
Source `D0395`, original line395, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Camera159 and command delivery investigation, 2026-10-06
New user recording11-26-39.dataset (47MiBOCC) extracted six H264 streams into
ir-proof159. Visual inspection firstand2s IRstream0 plus firstframesall4IRviews:
bright handheld controller constellation, no attributed Motion Tracker LEDs.
Visibleframe shows three small devicesontable; askedwhichisMotionTracker toavoid
misidentification. Scan216framesstream0 gives6..9blobs; blobcountaloneNOTidentity
or evidence of activation by Frame. No claim that LEDsneverlightfromthissample.
Swift5B40 callsradio commandparser94FC afterCRCSTATUS gate5A34/5A38 on nonzero
stateRAM20000E10 branch. It builds outgoingreplyandstarts RADIO START at5B2A
before parsingreceivedcommand. Otherstatebranch5B58 usesdifferent gates, payload
length/content processing. Saveannotated swift-rx-command-path.txt. This proves
a validreceivedframecommandpath, NOT that run157 reachedit. ACK-only hypothesis
notconfirmed. Swift6DC8setsderivedBASE1, rotatesprefixvia59A4, selectsTXpipe2;
channeladvance6EBEconfirmed. ExactactiveRXwindow/stateandTXdestinationstill
needcompletecontrol-flowtrace. Do not repeatunsynchronized40packetTXasguaranteed
commanddelivery. No additional Nordic mutation this investigation.

## K02998
Source `D0395`, original line423, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Isolated Motion Tracker camera161, 2026-10-06
User turnedoffothertrackers and recorded11-34-02. Extracted300tailrecords,
scanned215frames eachof4IRstreams. Counts0=7..11,1=7..7,2=1..2,3=11..19.
Visiblefirst/2sframe showsonewhiteMotionTracker strappedontable aroundx650,y750;
handheldcontroller remainsinframe. CorrespondingIRstream0 brightdetections across
215frames boundedx131..403,y278..513; broadtableROI x450..850/y600..900 haszero
threshold120/minarea2 blobs. Cameraalignment/time/exposure anddim/subpixelLEDs
notexcluded; thisisnegativeevidenceforvisibleconstellation, NOTproofnoemission.
AllfourfirstIRviewsandstream0laterchecked. Command157 LEDactivationstillunverified.
Evidence outputs/ir-proof161 inclvisible/IRfirstandlater,scan0..3.json.
NoadditionalTX/firmware/pairing change. Donotrequestrepeatidenticalrecordinguntil
newdelivery/synchronization test providesnewcondition.

## K02999
Source `D0395`, original line436, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Direct FramePicoTracker diagnostic API162/163, 2026-10-06
Extended frame_debug_agent fromread-onlystatus toloopback18741 boundedpassive
captureAPI andminimalbuttonpage. PasswordlessSSH clientframe_debug_client.py:
status,capturebundle,datasetfile. POSTcapture fixed10second slot0 hardwareCRC
profile only; concurrentAPIjobsrejected, no arbitraryshell/profile/TXinput.
Artifacts radioDMA,Frame/controllerCSV,logs,restorationchecks,manifest clocks,
datasetinventory. Camerarecording stillrequiresofficialVRDeveloperUI; nofake
oneclickvideostartorRFarrivaltimestamps. AddedhostSWDread monotonic/unixns,
explicitlynotRFtime. usable_radio_capture flag distinguishno-datafromsuccess.
LiveAPI162 id590002453d2e4269b179e25e82ed9803 complete, archive downloaded;
only1MPSLslot/0frames, restorationtrue, NOTusabletrackerdata. Dataset163 fetched
sensorRigCalibration viaAPI successfully.18focusedAPI/loader tests pass.
SSHloopbacktunnel localhost18741 activeexecsession71188;GETstatus/GETpageHTTP200.
Codexpanelopenqueued. AgentPIDfileupdated, noautoboot/system/rootserviceinstalled.
Docsframe-debug-api.md. NoadditionalTXorfirmwarechanges;6DoFunfinished.

## K03000
Source `D0395`, original line452, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Native FramePicoTracker dashboard installed, 2026-10-06
User correctlyrequestednativeheadsetUI likeFrameMic, notPCwebpage. Inspected
sasaken1102r/frame-mic-tuner MITsource, retainedLICENSE andadapted onlyOpenVR/
Vulkanoverlaytransport (vendorvr_overlay/vk_texture), uniquekeypico.frame-tracker
andnamePICO. No mic/autoupdater/stockaudiochanges. NativeCairo/libcurlapp source
pc/frame_app/main.cpp, ARM64compiledonFrame warnings-as-errors. Installedbinary
~/.local/bin/FramePicoTracker andapplications/FramePicoTracker.desktop.
LaunchedwithoutSteamVRrestart. OpenVRprobe confirms1200x700 overlayexists,
panelVisibleyes,dashboardopen; renderpreviewoutputs/frame-pico-ui-preview.png
visuallyinspectedshowsAPIconnectedidle andrecord/closebuttons. Controllerclick
pathnotyetuserverified. ButtoncallsboundedpassivecaptureAPI, notcameraautostart;
UIstatescamera/manualand6DoFunavailable. Re-launchsignalscurrentinstanceSIGUSR1.
Noautostartenabled. APIremoteCodexconnectionpreserved. ReferencecloneonFrame
pico-ram-work/frame-mic-reference; noinstall.sh executed. READMEinpc/frame_app.

## K03001
Source `D0395`, original line467, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Automatic camera recording implemented164, 2026-10-06
Staticcurrentvrwebhelper analysis resolvesVRSystem.StartTrackingRecording handler
56338 toHMDcommandformat start_tracking_recording %d %d %d atRO1AAB50;
flagsareallowTrackingCameraRecording,ScreenRecording,AudioRecording.
Stop56050 sendsstop_tracking_recording. vrcmd--cmd0exactcommandsverifiedok,
new11-53-43.dataset8.9MiBOCC (bounded2svalidation). Legacy--startlighthouse
recording stillreturns0withoutdataset; donotuseitforcameraAPI.
Agentcapture nowautomaticallystarts cameras1/screen0/audio0 beforeboundedradio
jobandstopsinfinally. ExistinggrowingOCCguardrefusesexternalrecordingownership.
NativeFramePicoTracker buttonupdated Record cameras+radio. Bundlesinclude new
OCC/calibration/poseCSVfiles, notonlyinventory. API164 id00a47f55206a43f69db4353ec4f801bf:
new11-54-39.dataset64278528byteOCC, start/stopok, archive downloadedlocal
outputs/camera-api164.tar.gz. ExtractedsixrealH264streams, framePNGdecodedand
visuallychecked outputs/camera-api164-frame.png. Radio1slot0frames unusable,
RAM/MPU/sessionrestorationtrue; nofictionaltrackingclaim. Nativeoverlayprobe
visibleyes1200x700 afterupdate.3APItests pass. Camera/radio exactclockalignment
andtracker6DoFremainunfinished. Noaudio/mic/firmwarechanges.

## K03002
Source `D0395`, original line485, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Awake moving tracker capture167, 2026-10-06
User clarified PICOasleepduring165. 1651021slots0records;cameradataset11-59-10
validbutnorealIMUtrace. Control166shortmultireceivedoneprofile0sensorpacket;
PICOstatusfirstSNonline1other2offline. UserthenwokePICOandrotatedagain.
167APIida1c370bae606493fbf67a10dd176864d, PICOAwake/displayON beforeandafter.
1021slots19hardwareCRCrecords; decoder16sensorframes(15gyro/accel+1magonly),
3otherframes. Valuesvaryconsistentwithmovement, NOTpreciselytimealignedvideo
or continuousorientation. CameraOCC64589824B dataset12-00-50 includedin
outputs/awake-moving167.tar.gz. Decodedoutputs/awake-moving167-decoded.json.
Extracted4IRstreams242frames each; threshold120/minarea2 counts0=0..5,
1=0..0,2=0..0,3=5..19. First1svisible/IRframe inspected: MotionTrackerinhand,
notattributedbrightLEDconstellation. Otherstream3brightcandidatesrequiremapping,
notclaimIRabsentuniversally. outputs/moving167 containsframes/scans.
Capturecomplete/restorationverifiedtrue;noadditionalTX,pairing,firmwarechange.
Next improveRFhopping/receivecadenceandactualIRcommanddelivery; no6DoFyet.

## K03003
Source `D0395`, original line501, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Diagnostic sensor visualization168, 2026-10-06
Addedframe_telemetry_trace.py: explicit hardwareCRC IMU CSV/JSON withhostreadns,
normalizedmeasuredacceldirectioninsensoraxes, noorientation/worldposevalidclaim.
15gyro/accel samplesfrom167 exportedonFrame. Nativepanel displayscompletedIR
previewandlastmeasuredacceldirectiononlabeledsensoraxes, nottracked3Daxes pinned
toMotionTracker. Realrender visuallycheckedoutputs/frame-pico-vectors168.png,
OpenVRpanelvisibleyes. AgentcaptureautomaticallyextractscompletedIRpreviewand
includesCSV/JSONinbundle; noframe/RFpreciseclockalignmentimplemented.
API168d919a6e6e3ca4cca87bec58c03c912d4 complete:camera-previewtrue,newdataset
12-06-47OCC51281920B,IMU0/1MPSLslot; clearvectoronemptycapture, restorationtrue.
5focusedtrace/APItests pass; ARM64UIrecompiled warnings-as-errors. NativeUI
maintainsstatedno6DoF/no-live-camera limitation. NoRFcadenceimprovementyet.
NextusercaptureFrame/PICOawake/controllersawake, firststationarythencontrolled
rotation; nocompetitionleadclaim. NoTX/flash/pairingchange.

## K03004
Source `D0395`, original line516, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Controlled motion capture169, 2026-10-06
API42b99a2ffc7c405e886c52201c4f9de1:1019slots30hardwareCRCframes27IMUsamples,
restoration_verifiedtrue,camera65736704B12-11-28.dataset. Bothcontrollerpose
filespresent. PICOAwake/displayONchecked during capture. Gyro peak1656raw;
acceldirection changedfrom[.011,.229,-.973]to[-.360,.905,-.227].
Hostread sample span7.779s,maxgap3.353s: no continuous orientation inference.
Localarchiveoutputs/motion-check169.tar.gz,extractedmotion169. Visiblefirstframe
showswhiteMotionTrackerheldbelowhand, part of tracker obstructed. MatchingIR
frame hasnoattributedbrightconstellation; fullstream0scan243frames0..4blobs,
identityunverified. Do notclaimallcamerasIRabsence. ffmpeg -ss1/5 onextracted
rawstream producednoimage; firstframe decodeworks, streamtimebase notassumed.
Nextunoccludedtargetviewneeded, improve RF timing/delivery before6DoFclaim.

## K03005
Source `D0395`, original line529, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Second controlled capture170, 2026-10-06
API7c0d4569104e4500b4c038ede7008a9c completed:1022slots30hardwareCRCframes,
18gyro/accel samples,maxhostreadgap3.249s,restorationtrue. PICOAwake/displayON.
Dataset12-15-56.occ64966656B; archiveoutputs/motion-check170.tar.gz.
AllfourIRstreams scanned242frames threshold120/minarea2: counts0=0..6,
1=0,2=0,3=6..10. Inspectedvisiblefirstframe whiteMotionTrackerfrontvisible,
IR0noattributedLEDconstellation; IR3mostlywindowbackground/trackeroutofframe.
Notproofnoflashinallframes/cameras. APIcamera_preview_createdfalse despitevalid
OCCandmanualdecode; nativepreviewmustnotbeinterpretedaslatestcapture.
NativeOpenVR posepoll onlydevice0HMD rows1076; controllerdatasetfilesexistbut
notproofvalidcontrollerposes/coexistence. Donotclaimcontrollersactivefromfiles.
NoTX,pairing,firmwarechange. NeedRFcadenceandcommanddelivery/IRphasework,
notrepetitionofidenticalcamerausercaptures.

## K03006
Source `D0395`, original line543, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Passive channel sweep171 and controls172-173, 2026-10-06
Added opt-in ROY_SWIFT_SWEEP tagged ROYSWIFTSWEEP1 module, explicit loader flag
and exact confirmed slot0 CRC profile guard. Sweeps2..80 every40 granted slots,
noTX,noBLEpriority/duty increase. Harness --interval optional, default10000us.
FrameclangARMfreestanding build1111B, 24 existing loader/probe tests pass.
171:1021slots2CRCframes: channel34 one valid6-byte magnetometer, channel28
zero-length frame. Not2IMU samples. 172fixed80:1019slots0frames. 173fixed34
interval10007us:1018slots7CRCframes,2magnetometer and5other frames, no gyro/accel.
Allthree session_closed/ram_restored/mpu_restored true, flash_writtenfalse.
Posepoll onlyHMD(device0); coexistence with awakecontrollers notvalidated here.
PICOAwake/displayON, firstSNonline1 other2offline checked. Raw/decoded evidence
outputs/sweep171.json,sweep171-decoded.json,fixed172.json,channel34-173*.json.
Channel sweep is diagnostic, NOT hopping-following or proven cadence improvement.
Firmware6E20..6E38 has configurable threshold200/(byte+1) and counter gate
before6EBE channel advance. Do notassert unconditional200Hz/hop everyIMU packet.
Annotated disassembly outputs/firmware-standalone/swift-hop-scheduling173.txt;
ends literalpool(6F80+) decodedasThumb data, not executable evidence.
IRcommand delivery remainsunproven. ExistingPICOlight_synclogs are historical
and do not establish present Frame-phase exposure alignment.

## K03007
Source `D0395`, original line563, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Cycle174 and bounded poll175, 2026-10-06
See docs/swift-station-cycle-2026-10-06.md. PC-literal-aware disassembly resolves
595C(2) as RXADDRESSES mask2(pipe1), NOT TX pipe2. 592C/51EC configprefix;
597C configfrequency. 5B0E copiesRXMATCHtoTXADDRESS,5B2A startsreplybefore5B40
parser. Responsepath doesnotproveanyemptyrequest accepted by currenttracker.
AddedseparatelytaggedROY_SWIFT_POLL max40zeroapplicationpayloadTX thenRX in
ownedslots; exactfullprofileguard andexplicitflagrequired. Passiveflagwithpoll
module rejectedbeforehardware. 24loader/probe tests pass, ARMbuildpassed.
175121slots40attempt40TXEND,0ADDRESS/END/frames, allrestorationtrueflashfalse.
NoIR/pairing/configopcodes. No response/standaloneconnectionclaim. PICOAwakeON
andnativeoverlay1200x700visible checkedafter. Twoawakecontrollersnotverified.
Nextstate/request/timinganalysis; norepeatidenticalTXprobe withoutnewhypothesis.

## K03008
Source `D0395`, original line576, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## PICO sleep comparison and bootstrap channels176-178, 2026-10-06
See docs/swift-station-cycle-2026-10-06.md. 176PICOAsleep/OFF empty40TX0RX;
wakecommand224 sent,butlaterAsleep meansnotconfirmedawake. Sleepisnotstationoff.
177taggedNOOPpoll complete00 00 00 header o[TRACKER_SERIAL]:40TX0RX.
176/177allrestoretrueflashfalse. NewROMtablefinding:15FDC[1,37,77]fallbackpipe0
prefixC0;15FE0[4,78,78]pairedinitialpipe1, copied6658..672A. BASE0RBIT785634AA
=552C6A1E. This isdistinctfromconfigured40evenchannelmask. 178passivebootstrap
1/37/77:1021slots0RXallrestoretrue. No trackerrestart/pairingchange; livebootstrap
state notproven. 24loader/probetests pass. Nextstate/transitionanalysis,
notblindTXrepetition. Currentstatus andbothresearchheaders synchronized178.

## K03009
Source `D0395`, original line587, SHA256 `5402f1797b1172a1a781e24d83246f9b4086a31410690339253b5476203a413c`. Status: historical/unadjudicated narrative; English source prose.

## Persistent tracking goal and mode investigation179-180, 2026-10-06
User explicitly requires continue until standalonetrackingfinished. create_goal
armedactivewithfull6DoF/Valvecoexistence/noflashscope, notnarrowedto diagnostics.
179fixedpairedinitial78:1020slots0framesallrestoretrue. PICOdum[TRACKER_SERIAL],
actualTrackerFrequency1, tracker1mode3freq129; enumvaluesnotHz.
180ELFAArch64methoddisassembler added pc/radio/elf_method_disasm.py.
ClientGetWorkMode3AC9C writes24bytes(3x8), Set3AD58 reads8bytes. ServiceGet7D10C
SPIcmd22 body[1,0]; Set7D368 cmd22 body[0,type,first3modebytes]. SetterNOTcalled.
Read-onlyNDKhelperpico_tracker_mode_read.c deployed/run, returns-1, stationlog
GetTrackerWorkMode:mcu send fail,ret0. Cannotinterpretaszeromodeorconfirmrollback.
Modehelperdoesnotrequestpairing/OTA. outputs/tracker-mode-{client,service}180.txt,
outputs/pico-state179.txt retained. NDKcmdwrapperfailswithspaces; useclang.exe
--target=aarch64-linux-android29 underUnity2022.3.22f1 AndroidPlayer/NDK.
RemovedEKFinvented5msfallbackforinvaliddt; nowraisebeforemutatingstate.
8optical/gaptests pass. No6DoFpublicationclaim. Goalremainsactive.

## K03010
Source `D0396`, original line1, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

# How full Swift hardware IDs were obtained

## K03011
Source `D0396`, original line3, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

Updated 2026-10-06. These are directly received identity bytes mapped to the Nordic FICR producer in the extracted Swift image. They are not the earlier speculative EEAC/EEAD/EEAE values and were not obtained by inverting CRC.

## K03012
Source `D0396`, original line7, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

| Historical PICO slot / serial | Raw FICR bytes, little-endian | Full DEVICEID[1] | Evidence |
|---|---|---|---|
| 0 / [TRACKER_SERIAL] | [TRACKER_ID] | [TRACKER_ID] | Own active bootstrap capture207, producer trace210 |
| 1 / [TRACKER_SERIAL] | Unknown full ID; historical low bytes C3 BF | Unknown | Do not substitute DFC3 or an invented high half |
| 2 / [TRACKER_SERIAL] | [TRACKER_ID] | [TRACKER_ID] | Later external219 capture, independently preserved220; two producer layouts agree |

## K03013
Source `D0396`, original line13, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

Slot/serial correspondence uses historical PICO station logs containing the lower16 bits. It does not identify which physical tracker the user currently holds.

## K03014
Source `D0396`, original line15, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

## First tracker: exact acquisition and proof

## K03015
Source `D0396`, original line17, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

1. With PICO powered off according to the user, Frame used a temporary Roy SRAM receiver/transmitter on bootstrap BASE552C6A1E,prefixC0,channels1/37/77. The fast RX variant uses hardware TX->RX handover. Its bounded query contained nine zero payload bytes; no station negotiation or IR command in207.
2. Capture outputs/fast-identity207.json contains a hardware-CRC-accepted record on channel1,slot10,callback observation359us. Application payload: `04 00 1F 00 00 00 0A 01 [TRACKER_ID]`. S1=2 is separate from the payload.
3. Swift63B4 constructs this exact12-byte layout: fixed04,00; variable context+30 at payload2; zero3..5;0A at6;01 at7.63EA loads RAM20000EF6+8=20000EFE and63EE copies it to payload8..11.
4. Startup66BE/66C0 calls7D18 with destination20000EFE.7D18 reads `[0x10000000+0x64]`, Nordic FICR DEVICEID[1], and stores its four bytes little-endian. Thus the received last4 bytes are hardware identity, not a guessed address suffix.
5. `int.from_bytes(bytes.fromhex('[TRACKER_ID]'),'little')` gives[TRACKER_ID]. Historical station lowED3E corroborates slot0. Bytewise sum with historical seed74735415, thenRBIT32, givesBASE[PRIVATE_LINK_VALUE], matching previously received working address. This address consistency is supporting evidence; the ID itself comes from the FICR packet producer.

## K03016
Source `D0396`, original line23, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

Files: outputs/first-tracker-id210.json; outputs/firmware-standalone/swift-id-prefix210.txt; swift-ficr-callers207.txt; docs/first-id210.md. Strict decoder: pc/radio/pico_swift_probe.py::decode_bootstrap_record.

## K03017
Source `D0396`, original line25, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

## Third tracker: second full ID and independent layout

## K03018
Source `D0396`, original line27, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

1. During a later read-only check, remote fast-identity219.json had been replaced by a later capture with five records. Our originally preserved219 capture had zero records. Do not attribute the later run to our original test or silently overwrite its evidence.
2. The later raw file was copied separately to outputs/external-fast-identity219-observed220.json. Frozen SHA256: `de27236ff0c8a45fa753f4a2e260fba54ca829df5b429fbd6c797bb5e78b594f`. Summary reports40TXEND/40hardwareRXhandovers; cleanupsession/RAM/MPUtrue,Frameflashfalse.
3. One bootstrap12-byte payload: `04 00 1F 00 00 00 0A 01 [TRACKER_ID]` onchannel77. The same FICR producer maps payload8..11 to DEVICEID[1].
4. Four identity13-byte payloads include `02 00 0A 00 00 [TRACKER_ID] 30 09 BD 00` (remaining metadata varies). In Swift6844 the source buffer begins atsp+8, while7D18 writes FICR atsp+0D: therefore the ID is payload5..8, not payload8..11. The radio record starts payload atrecord+5, so this ID is record+0A.
5. Both layouts yieldAD1DA201, little-endian[TRACKER_ID]. Historical low1DAD matches slot2. Historicalseed74735415 yieldsBASE[PRIVATE_LINK_VALUE],prefix40, consistent with earlier24-bit tail09 84 40.
6. Swift68F0 emits the13-byte identity reply for length9 only if the paired marker is notAAAA at that instant. This is a discovery-state observation, not proof of a current Frame pairing or active6DoF.

## K03019
Source `D0396`, original line34, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

Files: outputs/external-identity220-decoded.json; outputs/tracker-id221.json (source hash, observations and derived address); outputs/identity_evidence221.py; docs/additional-id220.md. Decoder: decode_identity_response, separate from decode_bootstrap_record.

## K03020
Source `D0396`, original line36, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

## Limits

## K03021
Source `D0396`, original line38, SHA256 `93a5b5970bae050bab44f3647b168e4a0f46abad8967a3c66fcd3e27396e9904`. Status: historical/unadjudicated narrative; English source prose.

Hardware CRC acceptance supports intact reception; CRC alone does not establish FICR semantics or authenticate a tracker. Semantic proof comes from producer-to-FICR tracing and consistent independent layouts. Callback time is a slot-relative software observation, not exact RF arrival time. ASCII1233 in Android status and tracker_info_data.json are not these hardware IDs. Ordinary tracker pairing-record writes are authorized by the user221; Frameflash and firmware images remain unchanged. Standalone connection, optical tracking and6DoF remain incomplete.

## K03022
Source `D0397`, original line1, SHA256 `ac1330234246806e850d0f635a3efb5c5427123ffe418fdcb5219fa5e668b828`. Status: historical/unadjudicated narrative; English source prose.

# Review of Gemini Swift specification, 2026-10-06

## K03023
Source `D0397`, original line3, SHA256 `ac1330234246806e850d0f635a3efb5c5427123ffe418fdcb5219fa5e668b828`. Status: historical/unadjudicated narrative; English source prose.

The supplied report is research data, not authorization to execute its pairing,
TX or firmware suggestions. No device changes were made for this review.

## K03024
Source `D0397`, original line6, SHA256 `ac1330234246806e850d0f635a3efb5c5427123ffe418fdcb5219fa5e668b828`. Status: historical/unadjudicated narrative; English source prose.

Confirmed independently in local binaries:

## K03025
Source `D0397`, original line8, SHA256 `ac1330234246806e850d0f635a3efb5c5427123ffe418fdcb5219fa5e668b828`. Status: historical/unadjudicated narrative; English source prose.

- Swift VA8ac4 loads a pointer from literal8bb8=16515, dereferences the
  four bytes ASCII1233 and stores them at SP+17 (VA8acc). This supports
  a constant registration/metadata field, not a unique tracker radio ID.
- libpvrtrackingservice.so exports trackerDataParserV2 at96e44,1004 bytes.
  It loads four single-precision values at IMU-base+1b/1f/23/27. Those
  offsets are real in the SLPI parser. IMU base is conditional: payload+3,
  +5 or+7 according to flag bits. This is NOT direct proof of the raw-air
  layout, quaternion order/convention, nor that the tracker (rather than
  some processing stage) produced the values. Trace both producer and
  station transport before claiming ready-to-use air quaternions.
- PhoenixSta VA13144 writes BASE0=76412900/PREFIX0 low byte71,
  CRCCNF103, and PCNF1=300ff. These writes are real, but association
  with Swift pairing is not demonstrated by the report.

## K03026
Source `D0397`, original line22, SHA256 `ac1330234246806e850d0f635a3efb5c5427123ffe418fdcb5219fa5e668b828`. Status: historical/unadjudicated narrative; English source prose.

Errors or missing evidence:

## K03027
Source `D0397`, original line24, SHA256 `ac1330234246806e850d0f635a3efb5c5427123ffe418fdcb5219fa5e668b828`. Status: historical/unadjudicated narrative; English source prose.

1. PCNF1=000300ff has WHITEEN bit25 CLEAR. It cannot be described as
   whitening enabled. MODE4 is named BLE2M PHY, MODE1 is Nordic2M.
   A custom protocol can use BLE2M PHY; the register name alone does not
   establish a BLE transport. Source: Nordic RADIO specification (original reference: `https://docs.nordicsemi.com/r/bundle/ps_nrf52833/page/radio.html`).
2. The actual researched Swift routine50e8 writes PCNF0=40008, not108;
   uses big-endian5-byte addressing and CRC16 initFFFF/poly11021. The
   proposed station profile cannot simply replace it as Swift framing.
3. 71764129 is a known BLE RF-test syncword, documented in
   Espressif RF test documentation (original reference: `https://docs.espressif.com/projects/esp-test-tools/en/latest/esp32h2/development_stage/rf_test_items/bt_ble_non_signaling_test.html`).
   PhoenixSta callers135ce and13a66 and the dynamic test-like mode/channel
   configuration strongly suggest an RF-test path. This is an inference,
   not yet a completed symbol/source identification. No call graph from
   StartTrackerPairing to13144 is provided. Do NOT label it established
   Swift pairing access address or send beacons/network configuration there.
4. Existing Swift length9/13/17/21 analysis and seed arithmetic are useful
   hypotheses, but they do not by themselves prove those packet contents
   occur on the station profile above. No CRC-valid attributed Swift frame
   or recovered current tracker FICR/channel mask is presented.
5. 200Hz, fixed64-byte air frames, quaternion[w,x,y,z], and eliminating
   host fusion are not established by SLPI consumer offsets alone. Even
   a valid orientation quaternion would not provide absolute6DoF position.

## K03028
Source `D0397`, original line46, SHA256 `ac1330234246806e850d0f635a3efb5c5427123ffe418fdcb5219fa5e668b828`. Status: historical/unadjudicated narrative; English source prose.

Outcome: new useful leads are ASCII1233 producer and SLPI four-float loads.
The claim that both main uncertainties are fully resolved is premature.
Standalone reception is still unimplemented. Next static work: trace actual
station tracker radio setup and pairing callers separately from RF tests,
then trace Swift telemetry producer through station/DSP to the SLPI parser.
Do not modify SRAM RX framing based on the unsupported proposed profile.

## K03029
Source `D0398`, original line1, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

CURRENT389 APP STILL UNFINISHED: UI/API installed388 (backup retained); actual Frame API active8830, radio inactive/MainPID0, both profiles unavailable, fresh application trackers empty. User launch therefore cannot discover. Isolated389 finite mode lease + applied N1 refresh repair:35offline testsPASS, NOTpackaged/deployed/admitted; CursorGrokreviewTIMEOUT/noverdict. Exact diagnostic10 control flow CORRECTS historical285: BOTH correction signs outside +/-8 converge7a32/760c and setskip20000f98; ACK stillnotapplied/pulseproof. Adaptive offline140/1771bothfourblobs/max20 NOTidentity or48/61 proof; low180pixel threshold NOTabsenceofphotons. No newRF/OTA; diagnostic10/210 preserved/fullbodyUNACHIEVED. Read docs/PHASE388_389_APP_STATUS_AND_MODE_LEASE_2026-10-11.md. Lower entries historical.

## K03030
Source `D0398`, original line3, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

USER APP PREFERENCE: no Stop Radio button. Native drawing/click removed; stop and MCU restoration remain internal to profile changes/errors. Continue active app integration.

## K03031
Source `D0398`, original line5, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

CURRENT387 APP OWNERS IMPLEMENTED/SOURCE ONLY: exclusive game/service workers, original210/service38 seals, config-mode binding, same-invocation4MCUcleanup + durable ownership; Stop routes newowners.18offline testsPASS + simulatedLinuxflock/prctlfixturePASS; CursorGrokreviewTIMEOUT/noverdict. Actual385fullOCC decoded1771pairs/100bothfourblobs/longest16; >1600darkimagesperview, synchronizationrootcauseUNKNOWN. CatalogsUNADMITTED/OTAownerpending/UI NOTinstalled/noRF/OTA. Read docs/PHASE387_PROFILE_OWNERS_AND_OPTICAL_BLOCKER_2026-10-11.md. diagnostic10/210unchanged/fullbodyUNACHIEVED. Lower entries historical.

## K03032
Source `D0398`, original line7, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

CURRENT386 APP PROFILE REQUEST ACTIVE/INCOMPLETE: local game/service controls + structured API tracker status + explicit stock-capability policy. NativeARM64 compilePASS/7offline policy+API testsPASS; staged binaryNOTinstalled/notlaunched. Profile starts/OTA stillREFUSE until owner integration; no newRF/OTA. Need resolve385 opticalfailure then sustained guardedowner/game mode1/service OTA mode0/freshcapability/progress/SteamVR realpose. Read docs/PHASE386_APPLICATION_PROFILE_INTEGRATION_2026-10-11.md. FullbodyUNACHIEVED. Lower entries historical.

## K03033
Source `D0398`, original line9, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

CURRENT385 ACTUAL PILOT FAILED CLOSED after fresh human readiness: single [TRACKER_SERIAL], noOTA, unchanged diagnostic10/original210. Finite fixed-phase optical verification exhausted; optical publications0/IMU0/native25 episodes0/identityFALSE/fullbodyUNACHIEVED. 25 actual stereo candidates, 15 candidate exposure-age rejects and6 camera-age rejects; verification window900..960 had0lit, later candidates972..981 do not repair consumed window. All4MCU cleanupTRUE; camera stopped/settings+power restored/native restarted; child terminated/no retry. Evidence outputs/phase385/actual-result-summary.json and actual-attempt. Continue offline timing/pixel analysis; candidate385 attempt CONSUMED, no unchanged repeat. Lower entries historical.

## K03034
Source `D0398`, original line11, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

CURRENT385 CONCRETE PILOT WAITING FRESH READINESS:173sealedfiles/stagedFramephase385/candidate; invocationSHA f675ec8c9b36bb8072875c9c70cd324880d28c8c283c1a9c1f0ec618456d5939. Query+combined nowboundadmission/freshhumanfile(candidatehash/currentboot/<=300s), fileABSENT. ActualARM64 verifierbothentryrefuseFreshphysicalreadinessabsentbeforedevices/Popen. Standalonerunner/profilemainrefusalretained/allliveguardsunchanged. AsyncuserreadinessquestionSENT/replypending; NEVERcreateconfirmationfilewithoutfreshhumananswer. Nextactualready->exclusivefile->ONEsupervised150scombinedpilot/noOTA/no retry; actualIMUsignalbeforemotion. SSH/SCP/chmod/verificationonly; NOcamera/power/native restart/RF/SRAM/OTA/newcapture;diagnostic10/210unchanged;goalBLOCKEDwaitingFreshHumanReadiness/fullbodyUNACHIEVED. Read outputs/phase385/NOTES.md then384/382/365. Lowerentrieshistorical.

## K03035
Source `D0398`, original line13, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

CURRENT384 BOUND INVOCATION: outputs/phase384/candidate173sealedfiles; invocation.json SHA c9adfd75dada65799243ac904c20f4ecf3bb99051868438b1a2966da611b720c. load_bound validatesanchor/exactroot+argv+singlepeer+source/external seals; combinedrequiresanchoredobject; querygatebeforeELF/install/runpy (mainrefusalstillFIRST). BindingverificationPASS173hashes/tamperrejection/ASTorder/all4mainrefusals. AnchorNOTsignature/deploymentenvelope independentlyverifyrequired. NOTstaged/admitted; next finalentry/freshphysicalreadiness/deploymentseal keepingallguards. NoRF/SRAM/OTA/SSH/newcapture/power/cameraactivation;diagnostic10/210unchanged;goalACTIVE/fullbodyUNACHIEVED. Read outputs/phase384/NOTES.md then383/382/365. Lowerentrieshistorical.

## K03036
Source `D0398`, original line15, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

CURRENT383 EXACT ADMISSION MODULE: outputs/phase383/pilot_admission.py pureJSON/ELF/argv checks; actual381canonical150scommand/original210/required diag10/singlepeerN1/controllerOFF/fixedphase/opticalIMUconfig/registry PASS; six tamper/activationcases REJECTED. Requiredimagehash NOTinstalledimageproof. ModuleNOTyetboundtoentry/query orstagedseal; mainHARDREFUSES; candidate381unchanged. Nextbindgate/invocationhash/exactchildargv/finalentryseal thenfreshphysicalreadiness, keepallruntimeguards. NoRF/SRAM/OTA/SSH/newcapture/power/cameraactivation;diagnostic10/210unchanged;goalACTIVE/fullbodyUNACHIEVED. Read outputs/phase383/NOTES.md then382/381/365. Lowerentrieshistorical.

## K03037
Source `D0398`, original line17, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

CURRENT382 FRAME STAGED VERIFICATION: candidate381 copiedfresh /home/steamos/pico-ram-work/phase381/candidate; actualARM64 verifierPASS172hashes/122parsedPython/9selectedimports/native+OpenVRhashes/executablepermissions/all4mainrefusals. Python3.12.3/numpy2.5.3/ffmpeg+OpenOCDpresent; noPillowneededselectedPNGpath/noinstall. Reportoutputs/phase382/remote-verification.json. SSH/SCP/chmod/importchecks performed; NOdevices/camera/power/RF/SRAM/OTA/newcapture. Draftunadmitted/allmainsHARDREFUSE; next finalentry/admission thenfreshphysicalreadiness. diagnostic10/210unchanged;goalACTIVE/fullbodyUNACHIEVED. Read outputs/phase382/NOTES.md then381/380/365. Lowerentrieshistorical.

## K03038
Source `D0398`, original line19, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

CURRENT381 HELPER PROVENANCE: FrameSSHavailable/currentXRService+OpenVRhashesread; local/remote232powersourceEXACT22e07375. NewARM64helper compiledGCC15.1.1/d79847cf, downloaded/hashchecked/bundledtools/power; differsoldbinaryb5bcf572/NOexactreproductionclaim/helperNOTexecuted. Header/source/commandhashprovenance retained. Candidate172files/current381paths/externalnative+OpenVRsealrequired. SSH/compile/SCP performed; NO RF/SRAM/OTA/trackerwrites/camera/native restart/powerchange/newcapture. AllmainsHARDREFUSE/notadmitted/notdeployed; next finalentry/admission/dependencies+permissions thenfreshphysicalreadiness. diagnostic10/210unchanged;goalACTIVE/fullbodyUNACHIEVED. Read outputs/phase381/NOTES.md then380/379/365. Lowerentrieshistorical.

## K03039
Source `D0398`, original line21, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

CURRENT380 MCU CLEANUP BEFORE PROFILE: candidate171files/current380paths; terminatedchild alone NOlongerallowsnativeprofile restore/restart. Actualradio-run fourliteraltrue MCUbarriersrequired; missing/malformed/failedreport retainsprofile+power/no retry. Fivebarrierrejection+fiverestorationfixturesPASS, NOTphysicalproof. CursorGrok379review timedout150s/noverdict/noquotaerror;terminal/rawresultsaved/no unchangedretry. Exactremote phase232_power binaryhash/provenance stillmissing; sourceinspected377. AllmainsHARDREFUSE/notadmitted/notdeployed; next completeactivation/externalhelper/dependencyproof/admission thenfreshreadiness. NoRF/SRAM/OTA/SSH/newcapture;diagnostic10/210unchanged;goalACTIVE/fullbodyUNACHIEVED. Read outputs/phase380/NOTES.md then379/378/365. Lowerentrieshistorical.

## K03040
Source `D0398`, original line23, SHA256 `3e2247e6de88d72ec6e33ac194427b7eb359cbc612e0caf7b89f795b74eda1d9`. Status: historical/unadjudicated narrative; English source prose.

CURRENT379 OWNERSHIP GAP CLOSED: outputs/phase379/candidate171hashcheckedfiles includes retainedfactorycamera/LEDJSON insideprivate/hashbound; current379configpaths. launch_begin REQUIRED beforechildPopen, then directon_childhandle; exceptionalunknownchild retainsprofile+power/no retry. Five ownerorderfixtures PASS andASTordering/refusalsverified, NOliveprocess/hardwareproof. AllmainsHARDREFUSE/notadmitted/notdeployed. Nextindependentreview/exactpowerbinary+dependencyproof/finaladmission/remoteseal+permissions/freshreadiness. NoRF/SRAM/OTA/SSH/newcapture;diagnostic10/210unchanged;goalACTIVE/fullbodyUNACHIEVED. Read outputs/phase379/NOTES.md then378/377/365. Lowerentrieshistorical.
