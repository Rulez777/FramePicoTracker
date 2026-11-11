#!/bin/sh
# Build FramePicoTracker (body panel 2026-10-31) on the Frame. Output: ./FramePicoTracker
cd "$(dirname "$0")" || exit 2
cp -n /opt/steamvr/tools/hellovr_vulkan_linux/src/openvr/headers/openvr.h vendor/openvr.h  # OpenVR header shipped with SteamVR on the Frame
g++ -std=c++17 -O2 -Wall -Wextra -Wno-unused-parameter -Ivendor $(pkg-config --cflags cairo vulkan) \
    main.cpp vendor/vr_overlay.cpp vendor/vk_texture.cpp -o FramePicoTracker \
    $(pkg-config --libs cairo vulkan) -L/opt/steamvr/bin/linuxarm64 -lopenvr_api -Wl,-rpath,/opt/steamvr/bin/linuxarm64 -pthread
