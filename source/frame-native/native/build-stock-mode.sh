#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p outputs/stock-tests
flags="-std=c++17 -O2 -Wall -Wextra"
case "${1:-}" in
  "") ;;
  --sanitize) flags="$flags -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer";;
  *) exit 2;;
esac
common="native/peer_engine.cpp native/data_plane.cpp native/tcl_transport.cpp native/roy_owner.cpp"
c++ $flags -o outputs/stock-tests/query native/query_peer_tests.cpp native/query_peer.cpp native/station_control.cpp $common
outputs/stock-tests/query
c++ $flags -o outputs/stock-tests/peer native/peer_tests.cpp $common
outputs/stock-tests/peer
