#!/bin/sh
# One bounded session from a freshly prepared/sealed invocation. No saved answer2,
# automatic readiness, OTA or installation. Use this entry for tracker-less validation too.
set -u
BASE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
ROOT=${1:?usage: PICO_PEERS=... sh live-visual-multi-alpha.sh /ABS/native-port/native-live-NEW}
case "$ROOT" in /home/steamos/pico-ram-work/native-port/native-live-*) ;; *) echo 'Use a prepared native-port/native-live-* root'; exit 2;; esac
case "${PICO_PEERS:-}" in *,*,*,*|'') echo 'PICO_PEERS requires 2 or 3 Alpha peers'; exit 2;; *,*) ;; *) echo 'PICO_PEERS requires 2 or 3 Alpha peers'; exit 2;; esac
export PICO_PEERS PICO_MULTI_OPTICS=${PICO_MULTI_OPTICS:-1}
export PICO_TEST_MODULE="${PICO_TEST_MODULE:-$BASE/outputs/alpha-session/multi.elf}" PICO_TEST_IMU_ACK=42
export PICO_STREAMING_MODE=2 PICO_PROFILE_PERSIST=1 PICO_STATION_COUNTERS=1
export PICO_TX_LOG=${PICO_TX_LOG:-"$ROOT/tx.log"} PICO_RADIO_DUMP="$ROOT/radio.bin"
if [ -x /home/openocd-tlmm/bin/openocd ]; then
    export PICO_OPENOCD=${PICO_OPENOCD:-/home/openocd-tlmm/bin/openocd}
    export PICO_TLMM_BASE=0xf100000 PICO_TLMM_DELAY=${PICO_TLMM_DELAY:-0}
    export PICO_PHASE_OFFSET=${PICO_PHASE_OFFSET:--26500}
fi
echo "root=$ROOT peers=$PICO_PEERS"
sha256sum "$PICO_TEST_MODULE"
export DISPLAY=:0 XDG_RUNTIME_DIR=/run/user/1000
MG=/home/steamos/pico-ram-work/mirror-grab
(cd "$MG" && exec env BENCH_AWAKE=1 BENCH_HOLD=0 ./mirror_grab /tmp/bench-hold 0 0 > /tmp/bench-hold.log 2>&1) &
HOLD=$!
# Only this script's mirror helper is signalled; never signal the producer/debugger.
cleanup_screen() {
    kill "$HOLD" 2>/dev/null || :
    (cd "$MG" && BENCH_AWAKE=0 timeout 20 ./mirror_grab /tmp/bench-hold 0 0 >> /tmp/bench-hold.log 2>&1) || :
}
trap cleanup_screen EXIT
for serial in $(awk '$1=="controller"{print $2}' ${PICO_DEVICES:-/home/steamos/pico-ram-work/devices.conf} 2>/dev/null); do  # controllers listed in devices.conf (none listed = no preflight)
    valid=$(cd "$MG" && POSE_LOG="$serial" POSE_ANY=1 timeout 4 ./mirror_grab.new "/tmp/ctrl-pre-$serial" 15 100 2>/dev/null | awk '/^P/{n++; if($3==1)v++} END{print v+0 "/" n+0}')
    echo "controller $serial valid $valid"
    case "$valid" in 0/*) echo 'Controller has no pose: check wake/charge; BLE scanning can reduce radio throughput';; esac
done
export PICO_PRIOR_CALIBRATION=/home/steamos/pico-ram-work/driver-build-fusion/tools/replay-calibration.txt
LINE=$(wc -l < /tmp/pico-diag.log 2>/dev/null || echo 0)
cd "$ROOT" || exit 2
"$ROOT/producer" --live --owner-admit-unproven "$ROOT/attempt/owner-decision.json" "$ROOT" "$ROOT/invocation.json" "$ROOT/invocation.sha256" --duration ${PICO_DURATION:-150} > /tmp/live-run.out 2>&1
RC=$?
cp /tmp/live-run.out "$ROOT/live-run.out"
tail -n +$((LINE+1)) /tmp/pico-diag.log | grep -aE 'multi_peer|multi_led|multi_optics|host_dwt|n_readback|station_counters|sensor_|mag_|optical_diag|unenrolled peer' > "$ROOT/multi-observables.txt" || :
echo "multi observables: $ROOT/multi-observables.txt"
/home/steamos/pico-ram-work/clean-session-litter.sh  # 2026-11-02: delete old XRService datasets
echo "live exit $RC (read result and restoration lines)"
exit "$RC"
