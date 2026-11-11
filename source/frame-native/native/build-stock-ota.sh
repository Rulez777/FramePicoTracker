#!/bin/sh
# Offline only. No debugger, radio, camera, owner approval or readiness.
set -eu
cd "$(dirname "$0")"
export PICO_DEVICES="${PICO_DEVICES:-$(pwd)/fixtures/devices.conf}"  # synthetic test identities
mkdir -p build
CXX=${CXX:-g++}
flags='-std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror -pthread'
if [ "${1:-}" = --sanitize ]; then
    flags="$flags -g -fsanitize=address,undefined -fno-omit-frame-pointer"
fi
common='phase5/sha256.cpp data_plane.cpp roy_owner.cpp tcl_transport.cpp peer_engine.cpp radio_loop.cpp image_admission.cpp'
$CXX $flags make_ota_service_state.cpp data_plane.cpp roy_owner.cpp tcl_transport.cpp -o build/make_ota_service_state
$CXX $flags ota_prep_probe.cpp data_plane.cpp roy_owner.cpp tcl_transport.cpp -o build/ota_prep_probe
$CXX $flags tracker_ota_tool.cpp pose_publication.cpp live_policy.cpp phase5/orchestrator.cpp phase5/tracking_guard.cpp $common -ldl -o build/tracker_ota_tool
$CXX $flags stock_ota_tests.cpp $common -o build/stock_ota_tests
$CXX $flags tracker_ota_tests.cpp $common -o build/tracker_ota_tests
$CXX $flags tracker_ota_radio_tests.cpp $common -o build/tracker_ota_radio_tests
$CXX $flags ota_approval_body_tests.cpp -o build/ota_approval_body_tests
sh test-ota-approval-body.sh
sh test-ota-ready-feed.sh
build/stock_ota_tests ../outputs/frame-alpha13.bin frame-release-ota.allowlist ../outputs/service38.elf build/stock-fixtures
build/tracker_ota_tests build/ota-test.allowlist
build/tracker_ota_radio_tests ../outputs/service38.elf ../outputs/frame-release-a.bin frame-release-ota.allowlist
[ ! -f ../ota-stock.sh ] || sh -n ../ota-stock.sh  # the launcher lives in the installed r3 folder
