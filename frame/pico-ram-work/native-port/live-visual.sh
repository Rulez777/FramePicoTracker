#!/bin/sh
# One bounded single-tracker session: locate the Roy firmware, prepare a sealed session root, run the producer.
export PICO_STATION_COUNTERS=${PICO_STATION_COUNTERS:-1}  # 2026-10-31: station counters in /tmp/pico-diag.log every 5 s (log only): panel radio column + hang detector
export PICO_PROFILE_PERSIST=${PICO_PROFILE_PERSIST:-1}  # 2026-10-31 owner: keep camera mode 2 permanently (no XRService restart per session; 0 restores after each session)
export PICO_STREAMING_MODE=${PICO_STREAMING_MODE:-2}  # 2026-10-31: XRService camera mode 2 (Retroreflector .25 ms): Valve controllers stay tracked (~98% valid) and the tracker locks; mode 3 lost the controllers
# 2026-10-31: fast SWD to Roy: our openocd 64fb747da build with the TLMM memory-mapped fast path (/home/openocd-tlmm, root-owned,
# cap_sys_rawio+cap_dac_override set by the owner). IMU age at the driver p50 88 -> 27 ms. PICO_OPENOCD=/usr/bin/openocd restores the stock path.
if [ -x /home/openocd-tlmm/bin/openocd ]; then export PICO_OPENOCD=${PICO_OPENOCD:-/home/openocd-tlmm/bin/openocd} PICO_TLMM_BASE=${PICO_TLMM_BASE:-0xf100000} PICO_TLMM_DELAY=${PICO_TLMM_DELAY:-0} PICO_PHASE_OFFSET=${PICO_PHASE_OFFSET:--26500}; fi  # phase re-tuned for the fast SWD (sweep3/sweep4: lit -29000..-24000 ticks)
export PICO_TEST_MODULE=/home/steamos/pico-ram-work/native-port-r3-20261101/outputs/alpha-session/single.elf PICO_TEST_IMU_ACK=42; unset PICO_PEERS; export PICO_MAG_FUSION=0 PICO_MAG_LEARN=0 PICO_TEMP_BIAS=0 PICO_SENSOR_SCALES=0; sha256sum $PICO_TEST_MODULE
export ROY_MODULE_KIND=single
. /home/steamos/pico-ram-work/roy-prepare.sh || exit 2
cd /home/steamos/pico-ram-work/native-port-mrec-20261103/native || exit 2
R=/home/steamos/pico-ram-work/native-port/native-live-$(date +%Y%m%d-%H%M%S)
export PICO_TX_LOG=${PICO_TX_LOG:-/tmp/pico-tx-$(basename $R).log}  # 2026-10-31 hang hunt: every mailbox the station sends (log only); copy into the capture after the run
B=$(cat /proc/sys/kernel/random/boot_id)
D=/home/steamos/pico-ram-work/native-port-mrec-20261103/owner-decision-$(date +%Y%m%d-%H%M%S).json
printf '{"owner_words":"го прогон","timestamp":"%s","boot_id":"%s"}\n' "$(date -u +%Y-%m-%dT%H:%M:%SZ)" "$B" > "$D"
H=$(cat /home/steamos/pico-ram-work/native-port/answer2.txt)
build/make_native_invocation ${PICO_OPENOCD:+--openocd $PICO_OPENOCD} --root "$R" --owner-admit-unproven "$D" --human-answer "$H" --hmd-tracked --controllers-off --tracker-supported --hmd-worn -- "$R/producer" --live --owner-admit-unproven "$R/attempt/owner-decision.json" "$R" "$R/invocation.json" "$R/invocation.sha256" --duration ${PICO_DURATION:-150} > /tmp/live-prep.out 2>&1 || { echo "prep failed"; tail -4 /tmp/live-prep.out; exit 3; }
echo "root=$R"
export DISPLAY=:0 XDG_RUNTIME_DIR=/run/user/1000
# Bench screen saving: keep the HMD awake (cameras need it) but show black for the run, restore after.
MG=/home/steamos/pico-ram-work/mirror-grab
(cd $MG && exec env BENCH_AWAKE=1 BENCH_HOLD=0 ./mirror_grab /tmp/bench-hold 0 0 > /tmp/bench-hold.log 2>&1) &
HOLD=$!
# 2026-10-31: controller preflight (log only). An asleep/off/flat Valve controller makes Roy run its BLE scan: ~146 of
# 166 slots/s blocked, IMU ~100 Hz instead of 200 (owner6dof-1: 95f9 had a flat battery).
for s in $(awk '$1=="controller"{print $2}' ${PICO_DEVICES:-/home/steamos/pico-ram-work/devices.conf} 2>/dev/null); do  # controllers listed in devices.conf (none listed = no preflight)
  v=$(cd $MG && POSE_LOG=$s POSE_ANY=1 timeout 4 ./mirror_grab.new /tmp/ctrl-pre-$s 15 100 2>/dev/null | awk '/^P/{n++; if($3==1)v++} END{print v+0 "/" n+0}')
  rm -rf /tmp/ctrl-pre-$s; echo "controller $s valid $v"
  case $v in 0/*) echo "WARNING: controller $s has no pose (off, asleep, flat, or out of the HMD camera view). If it is off or asleep, Roy BLE scan blocks most radio slots and the IMU runs at about half rate: wake or charge it.";; esac
done
export PICO_PRIOR_CALIBRATION=/home/steamos/pico-ram-work/driver-build-fusion/tools/replay-calibration.txt
cd "$R" || exit 4
"$R/producer" --live --owner-admit-unproven "$R/attempt/owner-decision.json" "$R" "$R/invocation.json" "$R/invocation.sha256" --duration ${PICO_DURATION:-150} > /tmp/live-run.out 2>&1
RC=$?
kill $HOLD 2>/dev/null; (cd $MG && BENCH_AWAKE=0 timeout 20 ./mirror_grab /tmp/bench-hold 0 0 >> /tmp/bench-hold.log 2>&1); rm -rf /tmp/bench-hold
grep -aq "Supervised runtime failed" /tmp/live-run.out && echo "SESSION_FAILED: $(grep -a "Supervised runtime failed" /tmp/live-run.out | tail -1 | cut -c1-160)"
/home/steamos/pico-ram-work/clean-session-litter.sh  # 2026-11-02: delete old XRService datasets
echo "live exit $RC"
