#!/bin/sh
# Explicit pairing only; no OTA, camera/profile change or Frame flash write.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
CHIP=${1:?usage: sh frame-pair.sh CHIP_HEX SLOT SERIAL_OR_- /ABS/NEW_ATTEMPT [trackerless]}
SLOT=${2:?PICO slot 0..2}; SERIAL=${3:?Expected serial or -}; ATTEMPT=${4:?New absolute attempt}
TRACKERLESS=0
case "${5:-}" in '') ;; trackerless) TRACKERLESS=1;; *) exit 2;; esac
case "$ATTEMPT" in /*) ;; *) echo 'Absolute attempt required'; exit 2;; esac
mkdir "$ATTEMPT"
# Preserve stdin for the fresh human answer; log while showing the prompt.
exec 3>&1
exec >"$ATTEMPT/pair.log" 2>&1
export ROY_MODULE_KIND=pairing
. /home/steamos/pico-ram-work/roy-prepare.sh || exit 2
TOOL="$ROOT/outputs/pairing/frame_pair"; MODULE="$PICO_TEST_MODULE"
STOCK="$ROY_FLASH"
WATCH=${PICO_PAIR_WATCH:?Set PICO_PAIR_WATCH to the verified native tracking_watch ELF}
PORT=${PICO_PAIR_PORT:-6666}
CFG=/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg
PICO_OPENOCD=${PICO_OPENOCD:-/home/openocd-tlmm/bin/openocd}
export PICO_TLMM_BASE=0xf100000 PICO_TLMM_DELAY=${PICO_TLMM_DELAY:-0}
for f in "$TOOL" "$MODULE" "$STOCK" "$WATCH" "$CFG"; do [ -f "$f" ] || { echo "Missing $f"; exit 2; }; done
MODULE_SHA=$(sha256sum "$MODULE"); MODULE_SHA=${MODULE_SHA%% *}
WATCH_SHA=$(sha256sum "$WATCH"); WATCH_SHA=${WATCH_SHA%% *}
"$TOOL" --chip "$CHIP" --slot "$SLOT" --module "$MODULE" --module-sha "$MODULE_SHA" --inspect 1
sha256sum "$TOOL" "$MODULE" "$STOCK" "$WATCH" "$CFG" >"$ATTEMPT/inputs.sha256"
mkdir -p /home/steamos/.local/state/frame-pico
exec 9<>/home/steamos/.local/state/frame-pico/roy.lock
flock -n 9 || { echo 'Another Roy owner holds lock'; exit 3; }
export PICO_PAIR_LOCK_FD=9
if pgrep -f '[p]ico_producer|[r]oy_ram_loader|[o]penocd|[t]racker_ota_tool --ota|[/]frame_pair --chip' >"$ATTEMPT/other-owners.txt"; then echo 'Other Roy owner alive'; exit 3; fi
if systemctl --user is-active --quiet frame-pico-radio.service; then echo 'Radio service active'; exit 3; fi
PROBE="$ROOT/native/build/ota_prep_probe"
"$PROBE" "$PORT" --free
trap 'echo "Signal: retain debugger/lock until tool restores SRAM"' INT TERM HUP
"$PICO_OPENOCD" -f "$CFG" -c "bindto 127.0.0.1; gdb_port disabled; telnet_port disabled; tcl_port $PORT; init; poll; poll off" >"$ATTEMPT/openocd.log" 2>&1 9>&- &
OCD=$!
"$PROBE" "$PORT" "$OCD" || { echo 'No owner installed; debugger startup failed'; kill -TERM "$OCD" 2>/dev/null || :; wait "$OCD" || :; exit 3; }
echo "Follow $ATTEMPT/pair.log; tool will request fresh GO on stdin" >&3
"$TOOL" --chip "$CHIP" --slot "$SLOT" --serial "$SERIAL" --module "$MODULE" --module-sha "$MODULE_SHA" --stock "$STOCK" --port "$PORT" --attempt "$ATTEMPT" --watch "$WATCH" --watch-sha "$WATCH_SHA" --trackerless "$TRACKERLESS" <&0 &
CHILD=$!
while :; do wait "$CHILD" && RC=0 || RC=$?; kill -0 "$CHILD" 2>/dev/null || break; done
if ! grep -qx roy_sram_released "$ATTEMPT/roy-release.txt" 2>/dev/null; then
    echo "No release proof; retain OpenOCD PID=$OCD and lock for recovery" >&3
    while :; do sleep 30; done
fi
kill -TERM "$OCD" 2>/dev/null || :
wait "$OCD" || :
echo "Pair tool rc=$RC; SRAM released. See $ATTEMPT/pair.log" >&3
exit "$RC"
