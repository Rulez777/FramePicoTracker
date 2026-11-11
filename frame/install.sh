#!/bin/sh
# Installs the PICO tracker stack onto a Steam Frame (SteamOS, user steamos).
# Run on the Frame from this folder: sh install.sh
# Existing files with the same names are kept as *.pre-handoff copies.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
W=/home/steamos/pico-ram-work

backup() { [ -e "$1" ] && cp -a "$1" "$1.pre-handoff" || :; }

echo "1/6 Work folder -> $W"
mkdir -p "$W"
cp -a "$HERE/pico-ram-work/." "$W/"
chmod +x "$W"/native-port*/native/build/* "$W"/native-port-*/outputs/pairing/frame_pair \
    "$W/steamvr-driver/bin/linuxarm64/driver_pico_tracker.so" "$W/xrtap/libxrtap.so" \
    "$W/mirror-grab/mirror_grab" "$W/mirror-grab/mirror_grab.new" "$W/frame_api/frame_debug_agent" \
    "$W/session-template/tools/power" "$W/session-template/release/tracking_watch" \
    "$W/pc-bridge-20261102/pc_bridge/build-gcc/pico_pc_bridge_send" "$W/clean-session-litter.sh" \
    "$W/roy-source/roy_layout" "$W/roy-prepare.sh"
command -v clang >/dev/null && command -v ld.lld >/dev/null || echo "WARNING: clang and ld.lld are needed on the Frame: the station module is built for the installed Roy firmware before each session"
mkdir -p /home/steamos/.local/state/frame-pico

echo "2/6 App FramePicoTracker"
mkdir -p ~/.local/bin ~/.local/share/applications
backup ~/.local/bin/FramePicoTracker
cp -a "$HERE/home-files/FramePicoTracker" ~/.local/bin/
cp -a "$HERE/home-files/FramePicoTracker.desktop" ~/.local/share/applications/

echo "3/6 Camera tap in SteamVR (xrtap) and the local API service"
mkdir -p ~/.config/systemd/user/steamvr.service.d
cp -a "$HERE/home-files/92-xrtap.conf" ~/.config/systemd/user/steamvr.service.d/
backup ~/.config/systemd/user/frame-pico-api.service
cp -a "$HERE/home-files/frame-pico-api.service" ~/.config/systemd/user/
systemctl --user daemon-reload
systemctl --user enable --now frame-pico-api.service

echo "4/6 SteamVR driver registration"
/opt/steamvr/bin/vrpathreg.sh adddriver "$W/steamvr-driver" || echo "vrpathreg failed: register $W/steamvr-driver by hand"

echo "5/6 Fast SWD openocd (needs sudo once)"
if [ ! -x /home/openocd-tlmm/bin/openocd ]; then
    sudo cp -a "$HERE/openocd-tlmm" /home/openocd-tlmm
    sudo rm -rf /home/openocd-tlmm/share/openocd
    sudo ln -s /usr/share/openocd /home/openocd-tlmm/share/openocd
    sudo chown -R root:root /home/openocd-tlmm
    sudo setcap cap_dac_override,cap_sys_rawio=ep /home/openocd-tlmm/bin/openocd
fi
getcap /home/openocd-tlmm/bin/openocd

echo "6/6 Restart SteamVR to load xrtap and the driver"
systemctl --user restart steamvr
echo "Done. Next: START_HERE.md, section 4."
