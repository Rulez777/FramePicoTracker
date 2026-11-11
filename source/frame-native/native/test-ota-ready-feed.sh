#!/bin/sh
# Synthetic file-bridge test only. Never invoke a hardware tool.
set -eu
cd "$(dirname "$0")"
. ./ota_ready_feed.sh
fixture="${1:-build/ready-feed-fixture-$$}"
mkdir "$fixture"
: >"$fixture/ota.log"
# A preexisting synthetic answer must not be read before the challenge.
printf 'synthetic answer\n' >"$fixture/ready.txt"
PICO_OTA_READY_WAIT_SECONDS=1 ota_ready_feed "$fixture" >"$fixture/no-challenge.out" 2>"$fixture/timeout.log"
[ ! -s "$fixture/no-challenge.out" ]
printf 'Type: READY synthetic-challenge\n' >"$fixture/ota.log"
PICO_OTA_READY_WAIT_SECONDS=2 ota_ready_feed "$fixture" >"$fixture/answer.out"
cmp "$fixture/ready.txt" "$fixture/answer.out"
echo 'PASS synthetic READY bridge: no challenge gives EOF; supplied file is byte-exact'
