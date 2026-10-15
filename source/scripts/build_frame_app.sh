#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
set -euo pipefail
cd "$(dirname "$0")/.."
# Official OpenVR headers and runtime are supplied by the builder, never fetched here.
: "${OPENVR_INCLUDE_DIR:?Set OPENVR_INCLUDE_DIR to the directory containing official openvr.h}"
OPENVR_LIB_DIR="${OPENVR_LIB_DIR:-/opt/steamvr/bin/linuxarm64}"
test -f "$OPENVR_INCLUDE_DIR/openvr.h"
pkg-config --exists cairo libcurl json-c egl gl
mkdir -p build
g++ -std=c++17 -O2 -Wall -Wextra -Werror \
  -I"$OPENVR_INCLUDE_DIR" \
  pc/frame_app/main.cpp pc/frame_app/frame_overlay.cpp \
  $(pkg-config --cflags --libs cairo libcurl json-c egl gl) -pthread \
  -L"$OPENVR_LIB_DIR" -lopenvr_api \
  -Wl,-rpath,"$OPENVR_LIB_DIR" -o build/FramePicoTracker
