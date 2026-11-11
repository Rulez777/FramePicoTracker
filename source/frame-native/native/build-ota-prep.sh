#!/bin/sh
# Build offline only; does not start any hardware process.
set -eu
cd "$(dirname "$0")"
mkdir -p build
CXX=${CXX:-g++}
flags='-std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror -pthread'
if [ "${1:-}" = --sanitize ]; then flags="$flags -g -fsanitize=address,undefined -fno-omit-frame-pointer"; shift; fi
state_common='data_plane.cpp roy_owner.cpp tcl_transport.cpp'
$CXX $flags make_ota_service_state.cpp $state_common -o build/make_ota_service_state
$CXX $flags ota_service_state_tests.cpp $state_common -o build/ota_service_state_tests
$CXX $flags ota_approval_body_tests.cpp -o build/ota_approval_body_tests
sh ./test-ota-approval-body.sh
$CXX $flags ota_prep_probe.cpp $state_common -o build/ota_prep_probe
$CXX $flags tracker_ota_tool.cpp pose_publication.cpp live_policy.cpp phase5/orchestrator.cpp phase5/tracking_guard.cpp phase5/sha256.cpp peer_engine.cpp radio_loop.cpp image_admission.cpp $state_common -ldl -o build/tracker_ota_tool
if [ "$#" -gt 0 ]; then
    state_test="build/ota-service-test-$$.bin"
    build/make_ota_service_state "$state_test"
    build/ota_service_state_tests "$1" "$state_test"
else
    echo 'Historical test NOT RUN: pass /REF/outputs/phase189 as argument'; exit 2
fi
build/tracker_ota_tool --dry-run ../outputs/frame-alpha11.bin frame-release-ota.allowlist
build/tracker_ota_tool --dry-run ../outputs/diagnostic10.bin frame-release-ota.allowlist
sh -n ../ota-alpha11.sh
