#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p build/optics-tsan
# Separate from ASan/UBSan and cached build outputs. No hardware dependencies.
"${CXX:-g++}" -std=c++17 -O1 -g -Wall -Wextra -Wpedantic -Werror -pthread \
    -fsanitize=thread -fno-omit-frame-pointer optics_concurrency_tests.cpp \
    -o build/optics-tsan/optics_concurrency_tests
build/optics-tsan/optics_concurrency_tests
