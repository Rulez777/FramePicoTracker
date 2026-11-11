#!/bin/sh
set -eu
cd "$(dirname "$0")"
. ../build_common.sh
. ../build_sources.sh
# LiveSessionOwner in tests owns the complete control graph. Use the same
# dependency list as the native control archives, including the ACK bridge.
control_sources=$(pico_control_sources ../)
run mkdir -p build
CXX=${CXX:-g++}
FLAGS='-std=c++17 -Wall -Wextra -Wpedantic -Werror -fno-fast-math -ffp-contract=off'
run env CXX="$CXX" PICO_CHECK_FLAGS="$FLAGS -O2 -pthread" sh ../build-check-symbols.sh build/odr orchestrator.cpp sha256.cpp tests.cpp ../data_plane.cpp $control_sources
link build/phase5_tests "$CXX" $FLAGS -O2 -pthread orchestrator.cpp sha256.cpp tests.cpp ../data_plane.cpp $control_sources -ldl
run ./build/phase5_tests ../../candidate/build/roy_station.elf
link build/phase5_tests_san "$CXX" $FLAGS -O1 -g -pthread -fsanitize=address,undefined -fno-omit-frame-pointer orchestrator.cpp sha256.cpp tests.cpp ../data_plane.cpp $control_sources -ldl
run ./build/phase5_tests_san ../../candidate/build/roy_station.elf
run "$CXX" $FLAGS -O2 -c orchestrator.cpp -o build/orchestrator.o
run "$CXX" $FLAGS -O2 -c sha256.cpp -o build/sha256.o
run ar rcs build/libpico_phase5.a build/orchestrator.o build/sha256.o

link build/tracking_guard_tests "$CXX" $FLAGS -O2 tracking_guard.cpp tracking_guard_tests.cpp orchestrator.cpp sha256.cpp
run ./build/tracking_guard_tests
link build/tracking_guard_tests_san "$CXX" $FLAGS -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer tracking_guard.cpp tracking_guard_tests.cpp orchestrator.cpp sha256.cpp
run ./build/tracking_guard_tests_san
run "$CXX" $FLAGS -O2 -c tracking_guard.cpp -o build/tracking_guard.o
run ar rcs build/libpico_phase5.a build/tracking_guard.o

# Linux-only real syscalls, restricted to a fresh directory below this build.
link build/os_adapters_tests "$CXX" $FLAGS -O2 -pthread os_adapters.cpp os_adapters_tests.cpp orchestrator.cpp sha256.cpp
run ./build/os_adapters_tests "build/os-fixture-$$"
