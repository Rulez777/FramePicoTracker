# Historical research narrative ledger

Historical original prose/translation, not current readiness or authorization. Interpret through ../../docs/CURRENT_STATE.md and ../../docs/SUPERSESSION.md. Native code fences/listings and private values were excluded or redacted; exact source paths/hashes are catalog metadata.

## K05521
Source `D0202`, original line142, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

UI native Cairo/Vulkan OpenVR overlay, hover and click queue are added. Old video, XYZ and agent-status are removed. Restart service executes fixed local systemctl for API independently of HTTP.

## K05522
Source `D0202`, original line144, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

UI does not show static three profiles as online: requires fresh connected +poses_valid. Runtime currently always has poses_valid=false, so list is empty even in presence of IMU. This is an honest barrier, not confirmation of RF absence.

## K05523
Source `D0202`, original line146, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Driver is installed/registered with vrpathreg. UDP39571 initialization, lazy GenericTracker registration on valid PTP1. Native build passed. Driver loading by current SteamVR and receiving real poses are not confirmed. A normal SteamVR restart may be needed after finishing integration.

## K05524
Source `D0202`, original line148, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 6. What is NOT ready and what defects remain

## K05525
Source `D0202`, original line150, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Permanent station for three devices. Systemd radio unit references old name `roy_station_service250.elf`, timing10ms and duration10seconds. New builds258/262 are not unified into a single production installation.
2. Previously, continuous high-priority mode affected right Valve controller according to user report; after stopping, controller returned. Low-priority mode received0slots. Real scheduler budget and watchdog coexistence are needed.
3. Per-peer command state machine is not completed. A repeated batch of seq0settings is sent, there is no complete ACK/sequence/retry engine.
4. New IDs: host registry is partially done, but empty slots are filled with zeros; there is no full safe enrollment state machine. Three occupied entries block adding a new device. DefaultIDs and seed remain in loader source.
5. Command14 and periodic10 exist as host builders, **are not connected to radio transmission**. Zero phase in init is not camera synchronization.
6. Product info, charging and battery replies are not parsed in a confirmed manner.
7. Significant bootstrap discriminator error: `len==12 && payload[0]==4` is not a strict enough matcher for ID autosaving. Use a verified full producer decoder.
8. Live OCC reader returns a list of all available recordings per poll, may load a huge old part into RAM. Streaming iterator, backlog limitation, disk budget and latency metrics are needed.
9. H264 demux writes files, does not output grayscale frames with metadata to PnP. There is no working continuous decoder +frame queue.
10. Stream-to-camera mapping, crop, calibration variant, time base and world transform are not established.
11. Visibility/recognition of Motion Tracker LEDs in correct phase and distinction of three constellations are not confirmed.
12. `imu_orientation.py` previous gyro scale differs by half from external profile. Do not select sensor type based on confident phrasing of old Gemini.
13. Optical modules contain synthetic and unmeasured performance claims. They are not hardware accuracy evidence.
14. `SteamFrameOpticalTracker.process_imu` can send VMT pose even without optical anchor. This must be forbidden in the final pipeline.
15. Native driver PTP1 uses slot, not persistent deviceID, serial is currently `FramePicoSlotN`. Identity based on real chipID/productInfo and correct reconnect lifecycle are needed.
16. UI/API capture invokes a separate passive loader. With permanent radio station, running a second OpenOCD/MPSL owner is not allowed; existing stream must be recorded.

## K05526
Source `D0202`, original line167, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 7. External developer hint: what to implement

## K05527
Source `D0202`, original line169, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

External author used a dongle. His hardware labels do not prove operation on onboard Roy.

## K05528
Source `D0202`, original line171, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Sequence: discovery ->offer ->command9 ->17/25/19 ->phase10 ->ACK notification22 ->IMU. Account for firmware178 vs189.

## K05529
Source `D0202`, original line173, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Radio frame: `[length][S1][payload]`, MODE4,5byteaddress, CRC16initFFFFpoly11021, without whitening. Working records `[seq][cmd][len][body]... [sum8]`. For a number of commands expected seq is incremented, reset on disconnect.

## K05530
Source `D0202`, original line175, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Polling of stock station is approximately4.7275ms/peer, RX window is not continuous. Before confirmation of command9, connection can drop in23ms, after in0.94seconds. Consequently uniform20ms slots with3peers are not equivalent to stock handshake; critical offer->confirm transition must fit within corresponding window.

## K05531
Source `D0202`, original line177, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

IR: command25mode, command14`[1,N,reason]`, Nnot0, transmission on own optical frame; periodic10 before short exposure. The author resynchronizes approximately0.8seconds, station50ms. Sync skips one LED pulse, take into account half camera period rule from the document.

## K05532
Source `D0202`, original line179, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Command0x1A(26) is not IR enable, it is mag calibration reply. Reject old opposite claims.

## K05533
Source `D0202`, original line181, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Sample2 body6/18/24/36/42, timer24 is not monotonic: period533333ticks,16MHz. Counter8 unwrap and skips. Do not infer200Hz delivery from nominal sensor rate. IMU does not provide position, and ankle/body inferred pose PICO network is not ported.

## K05534
Source `D0202`, original line183, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 8. Development procedure with completion criteria

## K05535
Source `D0202`, original line185, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage A. Make one reproducible build

## K05536
Source `D0202`, original line187, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Inventory local/remote source hashes, module ABI, deployed ELF and units. Do not execute arbitrary old wrappers.
2. Introduce deployment manifest: stock hash, code hash, module size, ABI, config version.
3. Remove duplicate filenames250/258/262 from production path. Save evidence builds separately.
4. Protect single ownership of OpenOCD/MPSL with process lock. On crash, do not overwrite active callback.
5. SIGTERM close ->verify session inactive ->restore checkpoint ->verify MPU/RAM ->terminate OpenOCD. KillMode=process already introduced, failure handling needs to be finished.

## K05537
Source `D0202`, original line193, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Readiness: a new contributor can build/install exactly the same version, UI/API/radio do not conflict.

## K05538
Source `D0202`, original line195, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage B. Real three-channel station

## K05539
Source `D0202`, original line197, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Registry of chipID,productSN,firmware,slot,bondstate. Not defaultIDs, separate station seed config.
2. Discovery unpaired and reconnect paired-address startup group4/78/78 with confirmed derivedBASE. Separate these states.
3. Per-peer states: absent,discovering,offer_sent,confirming,configuring,streaming,stale,reconnecting.
4. ACK/S1, per-command seq, timeouts, command9,17,19,25,4/notification22. Do not repeat entire seq0batch indefinitely.
5. TX queue from host to SRAM or MCU state machine with time in callback. Design within2045Blimit or justify another safe RAM reserve. Cannot simply expand into unknown RAM.
6. Schedule handshake with short offer->confirm interval, steady roundrobin of all3 without starvation.
7. MPSL request budget, deadline, priority, runtime denied metrics. Controller watchdog on validity violation terminates custom RF and restores state.
8. Keepalive, loss recovery, sample counter dedup, timestamp estimator from external doc.
9. Enrollment saves pairrecord using stock commands, not firmware. Re-enabling does not require long hold.

## K05540
Source `D0202`, original line207, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Readiness: all3provide checksum-valid IMU continuously, normal OFF/ON returns each, Valvecontrollers do not lose tracking. Verified in a general end-to-end session, not separate infinite probes.

## K05541
Source `D0202`, original line209, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage C. Optics and synchronization

## K05542
Source `D0202`, original line211, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Obtain active dataset by XRService status/log/mtime, not just new folder. Do not attribute someone else's manual recording to own process.
2. GrowingOCC streaming iterator, bounded queues, handling of partial EOF and recording rotation.
3. Persistent ffmpeg/libav decode of each IR stream, timestamped raw grayscale frames. Do not consider old backlog as live.
4. Confirm field3time/4exposure by metadata and framecounter; obtain cameraID->sourceCamera mapping from device config/explicit identifier, not by array order or size.
5. Select active factory calibration variant, crop/thermal correction, extrinsics. Remove defaults like arbitrary fx from production path.
6. Bind camera timestamp, Frame pose time and Roy timer. Estimate transport latency, do not use SWD read timestamp as RF arrival.
7. Send command14 on corresponding phase of each trackerframe; periodic10 before exposure. Develop retry/ACK and accounting for skipped pulse.
8. Factory8LED coordinates of each SN, correct units and axes. Verify association on single tracker, then on3 with temporal slot identification.
9. Blob detection + calibrated KB rays + multi-view/PnP/RANSAC, residual/inliers/rejection. Do not confuse PICO/Valvecontroller ring with MotionTracker.

## K05543
Source `D0202`, original line221, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Readiness: real LED observations are linked to a specific peer, valid optical positions/orientations and residual metrics are obtained. No identity swap, no false poses in absence of trackers.

## K05544
Source `D0202`, original line223, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage D. Fusion and real 6DoF

## K05545
Source `D0202`, original line225, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. Select sensor scale based on validated productInfo/firmwarebranch and measurement, store raw.
2. Bias, gravity, sample-counter timebase, gyro/accel axes. Optical orientation corrects yaw drift.
3. PnP cameraFromTracker ->headFromTracker ->worldFromTracker using timestamped HMD pose and calibrated extrinsics.
4. EKF only after optical initialization. Limit prediction by time/uncertainty, disable pose when anchor is lost.
5. Introduce unified PoseFrame: chipID,serial,sourceTime,position,quaternion,velocities,connected,poseValid,quality,batteryKnown.
6. Do not use bodyIK/LSTM as unannounced replacement for measurement6DoF. Occlusion limitation especially for legs with HMDcam must be measured.

## K05546
Source `D0202`, original line232, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Readiness: position changes in meters according to real movement, orientation is correct, worldpose is stable during head movement, trackinglost is honest.

## K05547
Source `D0202`, original line234, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage E. SteamVR

## K05548
Source `D0202`, original line236, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. UDP/Unix IPC PoseFrame, strict validation, identity instead of only slot. Loopback-only.
2. GenericTracker serial=chipID/productSN, roles optional, correct properties/icons.
3. Register only real connected device, poses_valid only by fusion. After disconnect device remains registered according to OpenVR lifecycle, but connected/valid false.
4. Timestamp offset,250ms stale policy align with production cadence. Battery property verified percent only.
5. Restart SteamVR once to load registered driver after finishing implementation, check driver log and devices.
6. Remove parallel VMT output if native driver is chosen. Not two independent publishers of identical devices.

## K05549
Source `D0202`, original line243, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Readiness: three GenericTrackers are visible in SteamVR and move according to real valid6DoF poses, consuming application receives them.

## K05550
Source `D0202`, original line245, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

### Stage F. Service and UI

## K05551
Source `D0202`, original line247, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. One persistent radio/fusion service, API separately. Autostart after verified lifecycle.
2. New peer enrollment and ordinary reconnect are indeed tied to buttons/auto-mode, not cosmetic text.
3. UI receives actual device list from runtime/driver. Distinguish connected-but-not-tracked and online according to user preference, do not show savedprofiles as devices.
4. Charge, charging, errors and pairing progress; "unknown" in the absence of a confirmed field.
5. Restartservice button must explicitly indicate which service it restarts. Currently restarts only API, not radio.
6. ClosingUI doesn't stop station. Diagnostics recording serves the already running radio, without a second SRAMsession.

## K05552
Source `D0202`, original line254, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Readiness: after boot/SteamVRrestart/APIrestart/userUIclose, trackers continue to work or automatically return correctly, Frameflash is unchanged.

## K05553
Source `D0202`, original line256, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 9. One final end-to-end check

## K05554
Source `D0202`, original line258, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Prepare implementation completely, then:
1. PICO is powered off, external dongle is absent, Frameactive, bothValvecontrollersactive.
2. Launch station+camera+fusion+driver of a single version.
3. All 3 trackerpairing, get IDs/productInfo, switch to streaming.
4. Record 10 minutes of common metrics: RF/ACK/sampleloss, cameraphase/inliers, worldposes, controller validstate, latency, CPU/disk/memory.
5. Check OFF/ON of each tracker, UI closure, restartAPI, then normal restartSteamVR.
6. Tracker/head motion, stationary tracker, absence of LED, occlusion, 3 deviceassociation.
7. Stopservice: sessionclosed/RAM/MPUrestored, Valvecontrollersremainvalid. Do not write Frameflash.
8. Record errors as precise defects; retry only after fix.

## K05555
Source `D0202`, original line268, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Do not declare readiness based on the number of unit tests or the fact that three icons appeared. User criterion: real MotionTracker 6DoF in SteamVR without PICO/dongle and without losing stock controllers.

## K05556
Source `D0202`, original line270, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 10. Minimum starter request for the next implementer

## K05557
Source `D0202`, original line272, SHA256 `6c76190df271721be1a0ee0be16e78fed055681a343c81622571bb50a58a4fd6`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

> Read AGENTS.md in K:\AI\pico and docs\HANDOFF_FULL_DEVELOPMENT_PLAN.md. Continue the integration of three PICO Swift Motion Trackers via onboard Roy Steam Frame, without firmwareflash/dongle/PICO runtime. Do not repeat short RF trials. Audit currentdeployment and execute stages A-F with concrete verifiable results, then one end-to-end session. Do not consider past assistant messages as proof of readiness and do not publish fictitious 6DoFposes. Main blockers: persistent3peerstateengine, command14/periodic10exposuresync, livecamera decoding/calibration/worldtransform and PoseFrame->nativeSteamVRdriver.

## K05558
Source `D0203`, original line1, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Handoff: optical 6DoF tracking of trackers using Steam Frame cameras

## K05559
Source `D0203`, original line3, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Date: 2026-10-07. Project: `K:\AI\pico`. Before starting work, read `AGENTS.md` in the root and follow it. General log: `docs/EXECUTION_STATUS_2026-10-06.md` (newest on top).

## K05560
Source `D0203`, original line5, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

This task is purely about computer vision: find the tracker in the Frame IR camera frames, calculate its pose in world coordinates, and deliver it to SteamVR. Radio communication with trackers and firmware are not part of this task and are not touched here.

## K05561
Source `D0203`, original line7, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 1. What we are doing and where the testbed is

## K05562
Source `D0203`, original line9, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The PICO Swift tracker carries 8 IR LEDs with known factory coordinates (model from `outputs/pico-calibration-root.json`, by serial number). Four Frame IR cameras (1056x1024) see these LEDs as bright spots. Based on the spots and the model, the tracker pose relative to the camera is solved (P3P), then transformed into world space via head pose and camera calibration, and published to the SteamVR driver as GenericTracker.

## K05563
Source `D0203`, original line11, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Testbed:
- Frame: `ssh steamos@[DEVICE_IP]` (key configured, `BatchMode=yes`). Working folder `/home/steamos/pico-ram-work`, code `release/` → `release-20261006b/pc`.
- Source files on PC: `K:\AI\pico\pc\optical\`. Deploy to Frame: copy modified file to `release/optical/<name>.py`, then `python3 -m py_compile`, then `systemctl --user restart frame-pico-radio.service`.
- Start/stop station (it also runs optics): `systemctl --user start|stop|restart frame-pico-radio.service`. Status: `state/fstat.py` (on Frame; commands `dev|cfg|live|opt`), optics details `state/optdump.py`.
- Terminal: sent from PC via ssh. In PowerShell, quotes for python get broken, so we put scripts into a file and run them (`cat > ... && python3 ...`).
- Semi-darkness in the room is not a problem for IR, even better (less parasitic flare).

## K05564
Source `D0203`, original line18, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Service mode without controllers (approved by user for testbed vision debugging): `state/service_mode.py on` sets `coexistence.min_controllers=0` and `freeze_limit_ms=null`, `off` restores from `station-config.json.before-vision`. In this mode, the station starts without controllers and does not stop due to freezing — fine for pure vision debugging.

## K05565
Source `D0203`, original line20, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 2. Vision pipeline (files and data flow)

## K05566
Source `D0203`, original line22, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`pc/optical/station_optics.py` — orchestrator. In a loop: takes a frame from `LiveCameraSource`, determines its index and which tracker is "its own" for this frame (time division), then:
- if temporal identity of the tracker is not yet verified → `pipeline.candidate(chip,frame)` (computes candidate pose, but does NOT publish);
- if verified → `pipeline.camera(chip,frame,identity_verified=True)` (computes and publishes).
Logs every processed frame to `state/optical-live.jsonl`.

## K05567
Source `D0203`, original line27, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`pc/optical/measured_pipeline.py` — core (`MeasuredPosePipeline`):
- `detect` spots (`blob_detector.py`), adds camera `pixel_offset` in the overall rig image.
- `_associate(key,peer,bound,pixels)` — finds tracker among spots. A real frame contains several objects (other trackers, reflections), and one side of the tracker shows only ~5 of 8 LEDs. Therefore: first we try previous prior; then spot clusters (`blob_clusters`, single-linkage 60px), from large to small, for each ≥5 spots we run `RayPoseSolver.associate`; the last resort is all spots at once. This yielded offline 3%→67% successful clusters (`outputs/assoc-eval`).
- `RayPoseSolver` (`ray_pose_solver.py`): robust P3P search, supports 5 inliers, `max_residual_px=2.5`, `min_inliers=5`. Gemini version (Codex attempt did not work, shelved in `outputs/assoc-eval/`).
- `world_from_tracker(head, head_from_camera, camera_from_tracker)` (`frame_camera_rig.py`) — transforms pose from camera coordinate system to world via head pose (`head_history.head_pose(stamp)`) and rig calibration.
- `camera(...)` on success: checks age (POSE_AGE_NS=250ms), speed-gate for pose jump, sets `peer.world`, reinitializes EKF (`fusion_ekf.py`) on measured pose, and `_send()` publishes `PoseFrame`.
- `_send/_propagate` — EKF between optical frames tracks IMU (if axis calibration and time alignment exist); without IMU, pure optics is published.
- `expire(...)` — marks pose as invalid if tracker is lost/stale, and sends one invalid frame.

## K05568
Source `D0203`, original line36, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`pc/driver/pose_frame.py` — `PoseFrame.encode()` packs into `PTP2` format, `NativePosePublisher.send()` sends UDP to `127.0.0.1:39571`. Native SteamVR driver (`pc/driver/steamvr_driver/`, `openvr_tracker.py`) receives and exposes GenericTracker.

## K05569
Source `D0203`, original line38, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Cameras and time:
- `measurement_clock.py`: `CameraClock` (frame from CLOCK_MONOTONIC_RAW domain, requires offset measured for this boot; station measures per-boot), `RoyClock`, `SampleClock`.
- `exposure_schedule.py`: frame indexing (33.33ms, 533333 ticks per frame), "dark frames".
- Factory camera calibrations in `outputs/frame-factory-20261006.json`, LED models in `outputs/pico-calibration-root.json` (by serial number, 8x3). On Frame privately in `private/` (chmod 600). Do not copy to repository.

## K05570
Source `D0203`, original line43, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 3. Temporal identity (key gate)

## K05571
Source `D0203`, original line45, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`pc/optical/temporal_identity.py` + `phase_search.py`. The models of the three trackers are nearly identical, so "which tracker this is" is determined by time: the tracker turns off the LED in its frame (every N-th camera frame, N = number of online trackers), synchronously with the 10 command. `TemporalIdentity` waits for TWO "dark flashes", each framed by six stable bright frames (constant-velocity fit, scatter ≤2cm), and only then sets `bindings[chip]`. `verified(chip,frame)` then passes the frame to `pipeline.camera(...)` (with publication).

## K05572
Source `D0203`, original line47, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

**This is the very reason why world is not currently being published.** `camera()` in `measured_pipeline.py:125` rejects a frame if `identity_verified is not True`. While identity is not verified, only `candidate()` operates, which calculates candidate pose (including world, field `world_position_candidate` in the log), but does not send to IPC. Identity verification requires dark flashes → 10 command → working radio communication and confirmed LED phase. Radio currently gives few responses, phase (`phase_search`) does not reach stable confirmation, and `identity_verified` remains False.

## K05573
Source `D0203`, original line49, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 4. What actually works (measured 2026-10-07 evening)

## K05574
Source `D0203`, original line51, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

One tracker (serial [TRACKER_SUFFIX], chip [TRACKER_ID]), lying on the side, service mode, station running, tracker in radio state `confirming` (IMU is not flowing):
- Out of 1500 recent frames: camera 039E — 378 frames, 24 with ≥5 spots, **16 solved** (6 inliers, residual ~1.46px); camera 05KV — 375 frames, 21 with ≥5 spots, but only 2 solved (gets confused in association, since it sees 10 spots). Cameras 0488 and 0BM9 do not see tracker (it is on the other side).
- That is: when the tracker falls into the camera's field of view, the pose is solved reliably and with good residual. `world_position_candidate` in `candidate()` is computed. But `identity_verified=False`, therefore there is no `camera()`/publishing.
- Offline on 221 real frame: 67% of bright clusters associate, depth ~0.32m stably.
- 295 tests pass (`cd /k/AI/pico && .venv/Scripts/python -m pytest -q pc/tests`).

## K05575
Source `D0203`, original line57, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 5. What to do next (by priority)

## K05576
Source `D0203`, original line59, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

1. **Lift identity gate for single-tracker testbed.** If exactly one known tracker is online, there is no need to figure out "which of the three" — it can be considered verified immediately, and then `camera()` will start publishing world. Proposed route: in `station_optics.py` introduce config flag (e.g. `optics.assume_single_identity`), under which for a single active chip `verified` returns True without dark flashes. This will open the end-to-end path camera→world→SteamVR and allow seeing the tracker in the headset. Caution: with multiple trackers this cannot be done (it will confuse them).
2. **Verify that world pose is correct and stable.** Enable publishing, observe `world_position_candidate` over time: tracker is stationary → position stays in place (jitter ≤ a couple of cm), when moved by hand — moves smoothly. Check axis/signs relative to head (ensure it is not mirrored). Verify depth against real distance.
3. **Improve association on "noisy" camera (05KV, 10 spots, solves 2/21).** Understand why clustering selects the wrong group: reflections probably form a large false cluster. Options: prioritize cluster closer to prior; filter out spots by brightness/size in `blob_detector`; require depth agreement between cameras.
4. **Bring to SteamVR and test in VRChat.** Ensure that native driver (`pc/driver/steamvr_driver/`) is loaded, accepts PTP2 on :39571 and displays GenericTracker. Assign role (pelvis/foot). For the first version, even pure optics without IMU at 20–30 poses/s with interpolation can yield a visible tracker.
5. **Multi-tracker (later).** For three trackers, working temporal identity is needed, i.e. stable confirmation of phase and dark flashes, which is tied to radio (outside this task).

## K05577
Source `D0203`, original line65, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 6. Useful facts and constants

## K05578
Source `D0203`, original line67, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- Camera frame 33.33ms, 533333 ticks (16MHz). Tracker LED pulse is around tick 268727; phase for command 10 was tuned ~270000, live phase search drifts (currently `state/led-phase.json`).
- POSE_AGE_NS=250ms, CANDIDATE_AGE_NS=1s.
- `RayPoseSolver`: `max_residual_px=2.5`, `min_inliers=5`. `blob_clusters` single-linkage 60px.
- Head pose: `controller_guard.head_pose(source_time_ns)` interpolates between valid HMD poses (polar rotation projection).
- Publication: UDP PTP2 to `127.0.0.1:39571`.
- Serial numbers and chip: [TRACKER_SUFFIX]=TRACKER_ID (original reference: `[TRACKER_ID]`), [TRACKER_SUFFIX]=TRACKER_ID (original reference: `[TRACKER_ID]`), [TRACKER_SUFFIX]=TRACKER_ID (original reference: `[TRACKER_ID]`).
- Do not copy to repository: raw calibrations, camera recordings, IDs. Station now deletes camera recordings automatically after stop (`live_source.py`).

## K05579
Source `D0203`, original line75, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## 7. Current state of Frame

## K05580
Source `D0203`, original line77, SHA256 `9c11c17f90b5c59bf785d41db858614a9cd6a75b2686d225487c4f4555406ef4`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Station is running in service mode (min_controllers=0, freeze off), one tracker in `confirming`, optics logs to `state/optical-live.jsonl`. Return production config: `python3 /home/steamos/pico-ram-work/state/service_mode.py off`. Backup of production config: `station-config.json.before-vision`. Free disk space ~47GB.

## K05581
Source `D0209`, original line3, SHA256 `342cded76c228e8bc1e89bce840cfb09b22a00f593be392d5921e371d0cd3e65`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. User explicitly confirmed immediate OTA start ("let's go") after
the pre-start pause (original reference: `OTA45_REVISION07_PRESTART_2026-10-08.md`).
One run45 attempt only; no retry or resume was launched.

## K05582
Source `D0213`, original line3, SHA256 `99a03bf74a2b68fa570ac84e1d32bae26ab959457cd63d8efddf707d620ddb93`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

2026-10-08. User explicitly confirmed immediate start ("start") after
fresh phone-off preparation (original reference: `OTA46_PHONE_OFF_PRESTART_2026-10-08.md`).
One new attempt, no resume/replay of failed45. Exact target
[TRACKER_SERIAL]/chip[TRACKER_ID], both controllers OFF, phone OFF per user.

## K05583
Source `D0250`, original line3, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Image `23beacdf…ab2451` (97976 bytes) — state read-only. No blocker of ABI, stack, length, and interrupt mask. One firmware is sufficient to select the branch of IMU disappearance. This is not permission for OTA and not a tracking repair.

## K05584
Source `D0250`, original line5, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Code

## K05585
Source `D0250`, original line7, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

In `profile_command`, scheme 3 is accepted only with length 18 and opcode `0x7E`. Response `FE 03` + nonce16 + chip4 + 10 bytes occupies exactly the same 32 bytes of stack (`sub sp,#32`), offsets 0..31 are initialized. `r8` holds `PRIMASK`, `cpsid i` masks only ten reads, restoration stands before `blx 0x9369`. Scheme 1 remains 24 bytes, scheme 2 — 26 bytes with revision 7 and explicit jump to `.Lreply`. Setter still requires `0x7D`, length 24 and scheme 2.

## K05586
Source `D0250`, original line9, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`0x9368` passes length 32 to `0x92A8`; length fits into one byte of node `+0x0F`. For the same 18-byte record body, stream length is 21, response budget `min(54, 65−L)` is equal to 44. Recording 35 bytes (32+3) fits into this budget. While there is no IMU, `0x9458` does not occupy a slot with an earlier record of opcode 22.

## K05587
Source `D0250`, original line11, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Installed 07 does not return these addresses. Subcommand `0x02` (`0x89D0`) reads only fixed windows `0x20001BA8..0x20001D43` (indices 0..12). `0x20001D45` is two bytes past the window of index 12. Subcommand `0x29` reads mailbox `0x20005BDE` and clears some of its branches.

## K05588
Source `D0250`, original line13, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Offline transfer of this file has already been recorded: 4083/4083, `finish_verified`, one erase, same SHA256. `native_ota_idle_verified` is false. Bootloader and recovery are unverified.

## K05589
Source `D0250`, original line15, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Meaning of fields

## K05590
Source `D0250`, original line17, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

| Offset | Address | What Swift does |
|---:|---|---|
| 22 | `0x20000442` | Sensor mode byte. `0xB118` writes it, `0xB1A4` sets `0x80`. |
| 23 | `0x20001FA0` | Identity cache. `0xAB46` accepts `0x47`, `0xAC88` accepts `0x44`; otherwise word `0x20001FB4` remains 0. |
| 24 | `0x20001FB4` | Enable word. `0xAD14` exits `0xAD08` immediately if it is 0. In the response, any non-zero value becomes 1. |
| 25 | `0x20000FE1` | Autonomous transmission enable. `0xAE08` on 0 skips queuing opcode 22. |
| 26 | `0x20001FB1` | `0xAD08` counter: when word `0xADC2` is enabled, increments it, then `0xADD4` invokes ring. |
| 27 | `0x20001AFC` | Byte that `0x97D8` writes after copying 18 bytes (`0x97AE`, step `0x12`). |
| 28 | `0x20001D45` | UART flag. `0x883E` and `0xD41C` write to it. |
| 29 | `0x2000201C` | Gate `0xD53C`: on 0, message of type 2 is not queued. `0xD560` sets 1, `0xD5D0` sets 0. This is not queue occupancy. |
| 30–31 | `0x2000066C` / `0x2000067C` | `0x15D4` scheduler indices. Full queue returns 4 and does not move the index. `0x9586` still proceeds to `0x9566` and advances command counter. |

## K05591
Source `D0250`, original line29, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

Ring `0x97A8` and radio record of opcode 22 are sequential stages. `0xAE40` calls `0x92A8` with `r2=0x16`, `r3=0x17` only if `0x20001FF6==0`, `0xE6B0` returned 4 and `0x20000FE1!=0`. Message of type 2 from `0xD53C` reaches `0xAD08` via worker `0xD482`. The only call to `0xD53C` is `0xA952`, and non-zero `0x20000441`, `0x20000FA7`, `0x20001FB0` are required before it.

## K05592
Source `D0250`, original line31, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`cpsid i` is not a producer completion barrier. If the worker has already passed `0xADCA` and has not yet reached `0x97DA`, a single snapshot will show new `0x20001FB1` and old `0x20001AFC`. A retry with an unequal interval separates this window from a dead counter: one byte wraps around after 256.

## K05593
Source `D0250`, original line33, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Sufficiency

## K05594
Source `D0250`, original line35, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

One response `FE 03` selects the branch:

## K05595
Source `D0250`, original line37, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

- no response — failure `0x92A8` (`0x0F` when `0x20000E10==0`, `0x13` on empty allocator, `0x06` at index check);
- byte 24 is equal to 0 — `0xAD08` produces no samples; check mode and identity;
- byte 24 is equal to 1, byte 29 is equal to 0, counters are stationary — `0xD53C` does not queue producer call;
- both counters run together and byte 25 is equal to 0 — ring is being written, opcode 22 is cut off at `0xAE08`;
- both counters run and byte 25 is equal to 1 — primary 18-byte ring is alive, break is below `0x97A8`.

## K05596
Source `D0250`, original line43, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

If power is on, gate `0x2000201C` is open, and both counters are stopped, this snapshot does not distinguish the three preconditions `0xA952`. If the ring is alive and transmission permission is present, the snapshot does not distinguish `0x20001FF6`, code `0xE6B0`, and failure `0x92A8`. A second diagnostic image before the first snapshot is not needed: these cells are read only if the first branch lands in them.

## K05597
Source `D0250`, original line45, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

## Unknown on hardware and OTA

## K05598
Source `D0250`, original line47, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

The live values of all ten bytes are unknown. The current IMU is equal to 0 at 99.15416% CRC in phase 151; phases 154 and 156 also gave IMU 0. Phase 132 has already yielded 5461 IMU on controller health failure, so the firmware is capable of producing samples. Loading, recovery, radio cadence, and physical response `FE 03` are unproven.

## K05599
Source `D0250`, original line49, SHA256 `57cd25ecb6f4b36d12052d665193e07f8a1866e2bcb30e34d8921e00e41ff61f`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

`approved_for_ota` remains false. Whitelist has not changed. The stop before real OTA is preserved. This image does not fix full tracking.

## K05600
Source `D0340`, original line1, SHA256 `6f60974fb7f6a9d199906300e8fddc86cc90d9dcdc90a6d7b64b642ebfbeb382`. Status: historical/unadjudicated narrative; Gemini English; exact placeholder/block coverage checked.

# Phase78: control runs without controllers
