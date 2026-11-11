#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p build
cxx=${CXX:-c++}
flags='-std=c++17 -O2 -Wall -Wextra -Werror'
common='peer_engine.cpp data_plane.cpp tcl_transport.cpp roy_owner.cpp'
$cxx $flags query_peer_tests.cpp query_peer.cpp station_control.cpp $common -o build/frame_release_query_tests
build/frame_release_query_tests
$cxx $flags peer_tests.cpp $common -o build/frame_release_peer_tests
build/frame_release_peer_tests
$cxx $flags multi_peer_tests.cpp query_peer.cpp station_control.cpp radio_loop.cpp phase5/orchestrator.cpp phase5/sha256.cpp $common -o build/frame_release_multi_tests
build/frame_release_multi_tests
$cxx $flags tracker_ota_tests.cpp phase5/sha256.cpp $common -o build/tracker_ota_tests
build/tracker_ota_tests build/ota-test.allowlist
$cxx $flags tracker_ota_tool.cpp pose_publication.cpp live_policy.cpp phase5/orchestrator.cpp phase5/tracking_guard.cpp image_admission.cpp radio_loop.cpp phase5/sha256.cpp $common -o build/tracker_ota_tool

$cxx $flags tracker_ota_radio_tests.cpp image_admission.cpp radio_loop.cpp phase5/sha256.cpp $common -o build/tracker_ota_radio_tests
build/tracker_ota_radio_tests ../outputs/service38.elf ../outputs/frame-release-a.bin frame-release-ota.allowlist
