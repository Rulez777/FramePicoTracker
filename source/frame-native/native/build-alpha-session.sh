#!/bin/sh
# Offline readiness tests. No radio, process control, installation or Python.
set -eu
cd "$(dirname "$0")"
mkdir -p build ../outputs/alpha-session
CXX=${CXX:-g++}
flags='-std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror -fno-fast-math -ffp-contract=off -pthread'
if [ "${1:-}" = --sanitize ]; then flags="$flags -g -fsanitize=address,undefined -fno-omit-frame-pointer"; fi
$CXX $flags mag_workmode_tests.cpp peer_engine.cpp data_plane.cpp tcl_transport.cpp roy_owner.cpp -o build/mag_workmode_tests
build/mag_workmode_tests
alpha=${PICO_ALPHA11_IMAGE:-../outputs/frame-alpha11.bin}
rollback=${PICO_ROLLBACK_IMAGE:-../outputs/diagnostic10.bin}
module=${PICO_OTA_MODULE:-../outputs/service38.elf}
for file in "$alpha" "$rollback" "$module" ../outputs/frame-release-a.bin ../outputs/frame-release-b.bin ../outputs/frame-release-c.bin; do
  if [ ! -f "$file" ]; then echo "NOT RUN: Alpha session readiness: missing $file"; exit 2; fi
done
common='peer_engine.cpp data_plane.cpp tcl_transport.cpp roy_owner.cpp'
$CXX $flags alpha_session_tests.cpp data_plane.cpp tcl_transport.cpp roy_owner.cpp radio_loop.cpp -o build/alpha_session_tests
if [ -n "${PICO_REV7_RADIO:-}" ]; then build/alpha_session_tests "$PICO_REV7_RADIO"; else echo 'NOT RUN: real rev7 replay'; exit 2; fi
$CXX $flags alpha_module_tests.cpp image_admission.cpp data_plane.cpp tcl_transport.cpp roy_owner.cpp -o build/alpha_module_tests
build/alpha_module_tests ../candidate/stock.bin ../outputs/alpha-session/single.elf ../outputs/alpha-session/multi.elf
$CXX $flags tracker_ota_tests.cpp phase5/sha256.cpp $common -o build/alpha_ota_tests
build/alpha_ota_tests build/alpha-test.allowlist "$alpha" "$rollback" frame-release-ota.allowlist
$CXX $flags tracker_ota_radio_tests.cpp image_admission.cpp radio_loop.cpp phase5/sha256.cpp $common -o build/alpha_ota_radio_tests
build/alpha_ota_radio_tests "$module" ../outputs/frame-release-a.bin frame-release-ota.allowlist "$alpha" "$rollback"
echo 'ALPHA_OFFLINE_CHECKS_PASSED; Frame tracker-less session still not run'
