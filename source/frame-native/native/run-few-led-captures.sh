#!/bin/sh
# OFFLINE ONLY: no SSH, RF, service, profile or device operations.
set -eu
cd "$(dirname "$0")"
if [ "$#" -lt 2 ] || [ "$#" -gt 3 ]; then
  echo 'usage: run-few-led-captures.sh PRIVATE_LED_JSON FACTORY_CAMERA_JSON [IMU_CALIBRATION_TXT]' >&2
  exit 2
fi
leds=$1
cameras=$2
calibration=${3:-}
capture_root=${PICO_CAPTURE_ROOT:-/home/steamos/pico-ram-work/captures}
output=${PICO_FEW_OUTPUT:-build/recovery3-captures}
binary=${PICO_FEW_REPLAY:-build/few-plain/capture_replay}
# Explicit simulation only; unset means original receive timing. Never edits wire input.
set --
if [ -n "${PICO_REPLAY_IMU_LATENCY_MS:-}" ]; then
  set -- --imu-latency-ms "$PICO_REPLAY_IMU_LATENCY_MS"
fi
mkdir -p "$output"
export PICO_OPTICAL_LIBRARY_ROOT=${PICO_OPTICAL_LIBRARY_ROOT:-$(cd ../candidate/release/optical && pwd)}
for capture in hand1 hand2 hand3-ack; do
  # Run the exact old path independently, then the new path, with identical clocks/calibration.
  for enabled in 0 1; do
    if [ -n "$calibration" ]; then
      PICO_FEW_LED=$enabled PICO_TRACKING_RECOVERY=$enabled "$binary" "$capture_root/$capture" "$leds" "$cameras" "$output/$capture-$enabled.wire.bin" \
        --few-led "$enabled" --prior-calibration "$calibration" "$@" > "$output/$capture-$enabled.txt" 2>&1
    else
      PICO_FEW_LED=$enabled PICO_TRACKING_RECOVERY=$enabled "$binary" "$capture_root/$capture" "$leds" "$cameras" "$output/$capture-$enabled.wire.bin" \
        --few-led "$enabled" "$@" > "$output/$capture-$enabled.txt" 2>&1
    fi
    cat "$output/$capture-$enabled.txt"
  done
done
if [ -z "$calibration" ]; then
  echo 'No calibrated PTI1 gyro input: persistent recovery was NOT evaluated; inspect recovery_seed_pairs.'
fi
