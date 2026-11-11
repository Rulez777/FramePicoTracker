#!/bin/sh
set -eu
cd "$(dirname "$0")"
CXX=${CXX:-g++}
mkdir -p build
flags='-std=c++17 -O2 -Wall -Wextra -Werror -pthread'
"$CXX" $flags frame_debug_agent.cpp ../native/phase5/sha256.cpp -o frame_debug_agent
"$CXX" $flags ../tools/make_rendermodel.cpp -o ../tools/make_rendermodel
"$CXX" $flags frame_debug_agent_tests.cpp ../native/phase5/sha256.cpp -o build/frame_debug_agent_tests
"$CXX" $flags ../tools/make_rendermodel_tests.cpp -o build/make_rendermodel_tests
build/frame_debug_agent_tests
build/make_rendermodel_tests
