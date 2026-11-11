#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p build
${CXX:-c++} -std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror -pthread \
    camera_transport.cpp live_camera.cpp decoder_process.cpp decoder_stress_tests.cpp \
    -o build/decoder_stress_tests
if command -v ffmpeg >/dev/null 2>&1; then
    ffmpeg_bin=$(command -v ffmpeg)
    case "$ffmpeg_bin" in /*) ;; *) echo "ffmpeg must resolve to an absolute path" >&2; exit 1;; esac
    "$ffmpeg_bin" -hide_banner -loglevel error -nostdin -y \
        -f lavfi -i color=c=gray:s=1056x1024:r=30 -frames:v 1 \
        -c:v libx264 -threads 1 -preset ultrafast -tune zerolatency \
        -pix_fmt yuv420p -x264-params keyint=1 -f h264 build/stress-idr.h264
    build/decoder_stress_tests "$ffmpeg_bin" build/stress-idr.h264
else
    build/decoder_stress_tests
fi
