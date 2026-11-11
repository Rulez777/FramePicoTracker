#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
build=${1:-build-gcc}
"./$build/pc_bridge_receive" --bind 127.0.0.1:39691 --seconds 4 --expect-test > "$build/loopback-receiver.txt" 2>&1 &
receiver=$!
sleep 0.25
"./$build/pico_pc_bridge_send" --test --target 127.0.0.1:39691 --frames 120
wait "$receiver"
cat "$build/loopback-receiver.txt"
