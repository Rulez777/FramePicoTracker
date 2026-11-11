#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
build=build-gcc
asan=OFF
if [[ ${1:-} == --sanitize ]]; then build=build-gcc-asan; asan=ON; fi
mkdir -p "$build"
flags=(-std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror -I${OPENVR_INCLUDE:-/opt/steamvr/tools/hellovr_vulkan_linux/src/openvr/headers})
if [[ $asan == ON ]]; then flags+=(-fsanitize=address,undefined -fno-omit-frame-pointer); fi
cxx=${CXX:-g++}
"$cxx" "${flags[@]}" tests.cpp -o "$build/pc_bridge_tests"
"./$build/pc_bridge_tests"
"$cxx" "${flags[@]}" send.cpp -ldl -pthread -o "$build/pico_pc_bridge_send"
"$cxx" "${flags[@]}" receive.cpp -pthread -o "$build/pc_bridge_receive"
