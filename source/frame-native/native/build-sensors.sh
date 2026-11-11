#!/bin/sh
# Offline only; no hardware access.
set -eu
cd "$(dirname "$0")"
mkdir -p build ../outputs
git show 64609915:driver/pico_fusion.h | sed 's/namespace pico_fusion {/namespace pico_baseline {/' > ../outputs/sensors_baseline.h
CXX=${CXX:-c++}
FLAGS="-std=c++17 -O2 -Wall -Wextra -pthread"
if [ "${1:-}" = "--sanitize" ]; then FLAGS="$FLAGS -g -fsanitize=address,undefined -fno-omit-frame-pointer"; fi
$CXX $FLAGS -DSENSOR_BASELINE data_plane.cpp sensors_tests.cpp -o build/sensors_tests
./build/sensors_tests
$CXX $FLAGS -DPICO_MAG_FUSION=1 -DPICO_MAG_LEARN=1 -DPICO_TEMP_BIAS=1 -DPICO_SENSOR_SCALES=1 data_plane.cpp sensors_tests.cpp -o build/sensors_enabled_tests
./build/sensors_enabled_tests
$CXX $FLAGS -DCOUNTER_SPACING -DSENSOR_CLOCK data_plane.cpp ../driver/tools/smooth_replay.cpp -o build/smooth_replay
$CXX $FLAGS data_plane.cpp ../driver/tools/sensors_fixture.cpp -o build/sensors_fixture
