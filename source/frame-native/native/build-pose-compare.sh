#!/bin/sh
set -eu
cd "$(dirname "$0")"
. ./build_common.sh
run mkdir -p build
CXX=${CXX:-g++}
flags='-std=c++17 -O2 -Wall -Wextra -Wpedantic -fno-fast-math -ffp-contract=off'
link build/pose_compare "$CXX" $flags pose_compare.cpp
link build/pose_compare_tests "$CXX" $flags pose_compare_tests.cpp
run build/pose_compare_tests
link build/pose_compare_tests_sanitize "$CXX" -std=c++17 -O1 -g -Wall -Wextra -Wpedantic -fno-fast-math -ffp-contract=off -fsanitize=address,undefined -fno-omit-frame-pointer pose_compare_tests.cpp
run build/pose_compare_tests_sanitize
