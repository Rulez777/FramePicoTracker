#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
set -euo pipefail
cd "$(dirname "$0")/.."
python3 scripts/prepare_overlay_dependency.py
mkdir -p build
g++ -std=c++17 -O2 -Wall -Wextra -Werror \
  -I.deps/frame-mic-tuner/third_party/openvr \
  pc/frame_app/main.cpp pc/frame_app/vendor/vr_overlay.cpp pc/frame_app/vendor/vk_texture.cpp \
  $(pkg-config --cflags --libs cairo libcurl vulkan) -pthread \
  -L/opt/steamvr/bin/linuxarm64 -lopenvr_api \
  -Wl,-rpath,/opt/steamvr/bin/linuxarm64 -o build/FramePicoTracker
