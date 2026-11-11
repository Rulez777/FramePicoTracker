# Steam Frame tracker bridge for Windows SteamVR

The Frame sender reads tracker poses from local SteamVR and sends them over UDP. The Windows driver exposes the poses as trackers in PC SteamVR. See [Start Here](../../START_HERE.md) for installation context.

## Build and test

- Steam Frame: run `bash build.sh` and then `bash loopback.sh`.
- Windows x64 Native Tools Command Prompt: run `build-msvc.cmd` and then `powershell -File loopback.ps1`. The driver package is in `build-msvc/frametrackers/`. Use its `install.cmd` or `uninstall.cmd`.
- On Windows, allow UDP 39590 on the private network from the Frame IP, install the driver and restart PC SteamVR. On Frame, run `/home/steamos/pico-ram-work/pc-bridge-20261102/pc_bridge/build-gcc/pico_pc_bridge_send --target PC_IP:39590` during the session.

Sender options: `--hz 90..120`, `--frames N`, `--roles FILE`, and `--openvr LIB`. The roles file defaults to `/tmp/pico-body-status.txt` and is reloaded once per second. The default OpenVR library is `/opt/steamvr/bin/linuxarm64/libopenvr_api.so`. Set `FRAME_TRACKERS_BIND=IP:PORT` in the Windows vrserver environment to override the receiver bind address.

## Wire format

FTB1 is little-endian. The header contains magic, version (1), and tracker count (up to 4) as u32, followed by session, sequence, and timestamp in nanoseconds as u64. It contains one HMD pose and one record per tracker. Total size is `92 + 64*N` bytes.

- Pose: u32 flags (valid and connected), xyz position, wxyz quaternion, linear velocity and angular velocity (13 float32 values).
- Tracker record: u32 chip, u32 role, pose. Roles are 0 unknown, 1 waist, 2 left foot, 3 right foot, 4 chest.

Poses use TrackingUniverseRawAndUncalibrated, meters, Y up and -Z forward. Head alignment uses `R = Q_pc * inverse(Q_frame)` and `t = P_pc - R * P_frame`, with 0.25 s smoothing. Packet age is estimated from the minimum observed clock offset plus 5 ms. Poses become invalid after 0.5 s without packets.

Loopback passed. Manage Trackers visibility, role acceptance and VRChat calibration still need a live test.
