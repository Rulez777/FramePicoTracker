#!/bin/sh
# One supervised Service-mode OTA. Never creates owner-approval.txt.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd -P)
TARGET=${1:?usage: sh ota-stock.sh SLOT /ABS/NEW_ATTEMPT   (SLOT of a tracker in devices.conf, see pair-tracker.sh)}
LINE=$(awk -v s="$TARGET" '$1=="tracker" && $3==s {print $2, $4}' "${PICO_DEVICES:-/home/steamos/pico-ram-work/devices.conf}" | tail -1)
[ -n "$LINE" ] || { echo "No tracker on slot $TARGET in devices.conf"; exit 2; }
SERIAL=${LINE#* }
CHIP=$(echo "${LINE%% *}" | tr A-F a-f | sed 's/\(..\)\(..\)\(..\)\(..\)/\4\3\2\1/')  # on-air byte order
ATTEMPT=${2:?Absolute new attempt required}
case "$ATTEMPT" in /*) ;; *) echo 'ATTEMPT must be absolute'; exit 2;; esac
IMAGE="$ROOT/outputs/frame-alpha13.bin"
case "$IMAGE" in /*) ;; *) IMAGE="$ROOT/$IMAGE";; esac
PORT=${PICO_OTA_PORT:-6666}
case "$PORT" in ''|*[!0-9]*) echo 'Invalid Tcl port'; exit 2;; esac
[ "$PORT" -gt 0 ] && [ "$PORT" -le 65535 ] || exit 2
mkdir "$ATTEMPT" # never reuse an attempt, checkpoint or approval
# Run from a terminal: the person at the keyboard approves and confirms readiness there (fd 3 = screen, fd 4 = keyboard).
# Without a terminal the approval and READY files must be created from fresh owner words, as before.
INTERACTIVE=0; if [ -t 0 ] && [ -t 1 ]; then INTERACTIVE=1; exec 3>&1 4<&0; fi
trap 'rc=$?; if [ "$INTERACTIVE" = 1 ]; then if grep -q finish_verified "$ATTEMPT/ota.log" 2>/dev/null && [ $rc = 0 ]; then echo "Done: firmware transferred. Restart the tracker with a long press." >&3; else echo "ota-stock.sh ended with exit $rc, firmware NOT confirmed. Last lines of $ATTEMPT/ota.log:" >&3; tail -5 "$ATTEMPT/ota.log" >&3; fi; fi' EXIT
exec >"$ATTEMPT/ota.log" 2>&1
echo "ATTEMPT=$ATTEMPT; log=$ATTEMPT/ota.log"
TOOL="$ROOT/native/build/tracker_ota_tool"
WATCH=${PICO_OTA_WATCH:-/home/steamos/pico-ram-work/session-template/release/tracking_watch}
# Roy firmware is located by signatures and the service module is built for it (read-only step).
export ROY_MODULE_KIND=service
. /home/steamos/pico-ram-work/roy-prepare.sh || exit 2
MODULE="$PICO_TEST_MODULE"
STOCK="$ROY_FLASH"
ALLOW="$ROOT/native/frame-release-ota.allowlist"
CFG=/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg
if [ -x /home/openocd-tlmm/bin/openocd ]; then
    PICO_OPENOCD=${PICO_OPENOCD:-/home/openocd-tlmm/bin/openocd}
else
    PICO_OPENOCD=${PICO_OPENOCD:-/usr/bin/openocd}
fi
export PICO_OPENOCD PICO_TLMM_BASE=0xf100000 PICO_TLMM_DELAY=${PICO_TLMM_DELAY:-0}
# Cadence qualification needs a running 64MHz DWT; owner checkpoints/restores
# both controls and logs enable/readback. Do not rely on a prior session.
export PICO_HOST_DWT=1
for f in "$TOOL" "$WATCH" "$MODULE" "$STOCK" "$ALLOW" "$CFG" "$IMAGE"; do
    [ -f "$f" ] || { echo "Missing $f"; exit 2; }
done
"$ROOT/native/build/make_ota_service_state" "$ATTEMPT/service-state.bin" "$CHIP"
"$TOOL" --dry-run "$IMAGE" "$ALLOW"
WATCH_SHA=$(sha256sum "$WATCH"); WATCH_SHA=${WATCH_SHA%% *}
IMAGE_SHA=$(sha256sum "$IMAGE"); IMAGE_SHA=${IMAGE_SHA%% *}
case "$IMAGE_SHA" in
    bad0a210b00ddd80ba6898c7b10cdbb2e4b7e7345c18aca0ac02d699ac4ca915) ;;
    *) echo 'Stock launcher supports only Alpha 1.3'; exit 2;;
esac
MODULE_SHA=$(sha256sum "$MODULE"); MODULE_SHA=${MODULE_SHA%% *}
grep -qx "module_sha256 $MODULE_SHA" "$ROY_LAYOUT_REPORT" || { echo "Service module differs from the locator report"; exit 2; }
sha256sum "$TOOL" "$WATCH" "$MODULE" "$STOCK" "$CFG" "$IMAGE" >"$ATTEMPT/inputs.sha256"
mkdir -p /home/steamos/.local/state/frame-pico
exec 9<>/home/steamos/.local/state/frame-pico/roy.lock
flock -n 9 || { echo 'Roy lock held; no launch'; exit 3; }
# OtaNativeHistory requires a prepared file. Preserve all existing negative
# evidence; its constructor also knows the primary tracker from devices.conf.
: >>/home/steamos/.local/state/frame-pico/ota-native-chips
if pgrep -f '[p]ico_producer|[r]oy_ram_loader|[o]penocd|[/]producer --live|[t]racker_ota_tool --ota' >"$ATTEMPT/other-owners.txt"; then
    echo 'Another Roy producer/debugger is alive; no launch'; exit 3
fi
if systemctl --user is-active --quiet frame-pico-radio.service; then
    echo 'Radio service active; no launch'; exit 3
fi
# Same sealed watcher bytes as make_native_invocation (session-template).
cp "$WATCH" "$ATTEMPT/tracking_watch"
chmod 700 "$ATTEMPT/tracking_watch"
WATCH="$ATTEMPT/tracking_watch"
set -- "$TOOL" --ota "$IMAGE" --chip "$CHIP" --serial "$SERIAL" \
    --allowlist "$ALLOW" --module "$MODULE" --stock "$STOCK" \
    --state "$ATTEMPT/service-state.bin" --port "$PORT" --checkpoint "$ATTEMPT/checkpoint" \
    --watch "$WATCH" --watch-sha "$WATCH_SHA" --proof "$ATTEMPT/observer-proof.json" \
    --stock-approval "$ATTEMPT/owner-approval.txt"
export PICO_OTA_TRACE="$ATTEMPT/ota-trace.log"
export PICO_OTA_LOCK_FD=9 PICO_OTA_RELEASE_PROOF="$ATTEMPT/roy-release.txt"
# Do not terminate a child on INT/TERM: tool owns restoration. wait may be
# interrupted, so continue waiting until the actual child exits.
trap 'echo "Signal received; retaining Roy debugger/lock; waiting for tool cleanup"' INT TERM HUP
"$ROOT/native/build/ota_prep_probe" "$PORT" --free
"$PICO_OPENOCD" -f "$CFG" -c "bindto 127.0.0.1; gdb_port disabled; telnet_port disabled; tcl_port $PORT; init; poll; poll off" \
    >"$ATTEMPT/openocd.log" 2>&1 9>&- &
OCD=$!
# Check the real local Tcl endpoint without issuing any SRAM command.
"$ROOT/native/build/ota_prep_probe" "$PORT" "$OCD" || {
    echo 'Debugger not ready; no SRAM owner installed'; kill -TERM "$OCD" 2>/dev/null || :; wait "$OCD" || :; exit 3;
}
. "$ROOT/native/ota_approval_body.sh"
ota_approval_body "$(date +%s)" "$IMAGE_SHA" "$@" >"$ATTEMPT/approval-body.txt"
cat "$ATTEMPT/approval-body.txt"
echo 'WAITING: Claude must create owner-approval.txt from fresh owner words after good link test.'
echo 'Target alone, no USB/charger; fresh normal Service0 boot without gesture; HMD tracked.'
echo 'Approval expires after 120s. No mode switch, automatic retry or approval renewal.'
echo 'After challenge appears here, Claude must create ready.txt from fresh owner words: READY <challenge>.'
echo 'Controllers connected to suppress BLE scan; HMD tracked; all other trackers OFF.'
if [ "$INTERACTIVE" = 1 ]; then
    {
        echo "Flash firmware Alpha 1.3 to tracker $SERIAL (slot $TARGET)?"
        echo "Tracker: alone (other trackers OFF), not on USB/charger, freshly switched on without the Frame-mode gesture."
        echo "Headset tracked, both controllers connected."
        printf "Type GO to approve (anything else cancels): "
    } >&3
    read -r GO <&4 || GO=
    case "$GO" in
        GO|go) ota_approval_body "$(date +%s)" "$IMAGE_SHA" "$@" >"$ATTEMPT/owner-approval.txt";;
        *) echo "Not approved; nothing was loaded." >&3; kill -TERM "$OCD" 2>/dev/null || :; wait "$OCD" || :; exit 2;;
    esac
fi
while [ ! -f "$ATTEMPT/owner-approval.txt" ]; do
    if ! kill -0 "$OCD" 2>/dev/null; then echo 'Debugger exited before OTA'; exit 3; fi
    sleep 1
done
[ ! -e "$ATTEMPT/ready.txt" ] || { echo 'READY file predates challenge'; exit 2; }
mkfifo "$ATTEMPT/ready.fifo"
. "$ROOT/native/ota_ready_feed.sh"
ota_ready_feed "$ATTEMPT" >"$ATTEMPT/ready.fifo" 9>&- &
FEED=$!
"$@" <"$ATTEMPT/ready.fifo" &
OTA=$!
echo "OTA tool PID=$OTA; OpenOCD PID=$OCD; wait for restoration before stopping either"
if [ "$INTERACTIVE" = 1 ]; then
    # Show the tool's readiness question and pass the typed answer on. Nothing is answered automatically.
    n=0; while [ $n -lt 120 ] && kill -0 "$OTA" 2>/dev/null && ! grep -q "Type: READY " "$ATTEMPT/ota.log"; do sleep 1; n=$((n+1)); done
    if grep -q "Type: READY " "$ATTEMPT/ota.log"; then
        grep "Type: READY " "$ATTEMPT/ota.log" | tail -1 >&3
        printf "> " >&3
        if read -r ANSWER <&4; then printf "%s
" "$ANSWER" >"$ATTEMPT/ready.txt"; fi
        echo "Transfer runs about a minute. Do not interrupt; progress: $ATTEMPT/ota.log" >&3
    fi
fi
RC=0
while :; do
    wait "$OTA" && RC=0 || RC=$?
    kill -0 "$OTA" 2>/dev/null || break
done
if ! grep -qx roy_sram_released "$PICO_OTA_RELEASE_PROOF" 2>/dev/null; then
    echo "Tool exited rc=$RC WITHOUT release proof. Keep debugger PID=$OCD and lock; manual recovery required."
    [ "$INTERACTIVE" != 1 ] || echo "Tool exited rc=$RC without proof that Roy memory was released. Do not start sessions; reboot the headset to restore the radio. Log: $ATTEMPT/ota.log" >&3
    # Includes validation failures before mutation: deliberately retain evidence
    # until an operator reviews the tool exit. Never guess cleanup after a crash.
    while :; do sleep 30; done
fi
kill -TERM "$FEED" 2>/dev/null || :
wait "$FEED" || :
echo "Roy released; tool rc=$RC. Stopping only our debugger PID=$OCD"
kill -TERM "$OCD" 2>/dev/null || :
wait "$OCD" || :
exit "$RC"
