#!/bin/sh
set -eu
cd "$(dirname "$0")"
export PICO_DEVICES="${PICO_DEVICES:-$(pwd)/fixtures/devices.conf}"  # synthetic test identities
OUT=${OUT:-../outputs/pairing}
mkdir -p "$OUT"
CXX=${CXX:-g++}
flags='-std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror -pthread'
if [ "${1:-}" = --sanitize ]; then flags="$flags -g -fsanitize=address,undefined -fno-omit-frame-pointer"; fi
common='phase5/sha256.cpp data_plane.cpp roy_owner.cpp tcl_transport.cpp peer_engine.cpp radio_loop.cpp image_admission.cpp'
$CXX $flags frame_pair_tests.cpp $common -o "$OUT/frame_pair_tests"
"$OUT/frame_pair_tests"
$CXX $flags frame_pair.cpp pose_publication.cpp live_policy.cpp phase5/orchestrator.cpp phase5/tracking_guard.cpp $common -ldl -o "$OUT/frame_pair"
mkdir -p build
$CXX $flags ota_prep_probe.cpp data_plane.cpp roy_owner.cpp tcl_transport.cpp -o build/ota_prep_probe
CLANG=${CLANG:-clang}; LD_LLD=${LD_LLD:-ld.lld}
for kind in single multi pairing; do
    source=$kind; extra=''
    if [ "$kind" = pairing ]; then source=single; extra=-DPICO_PAIRING; fi
    "$CLANG" --target=arm-none-eabi -mcpu=cortex-m4 -mthumb -Oz -flto -fomit-frame-pointer -ffreestanding -fno-builtin -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections $extra -c "station_alpha/$source.c" -o "$OUT/$kind.o"
    "$LD_LLD" --mllvm=-enable-machine-outliner=always --mllvm=-machine-outliner-reruns=2 --gc-sections -T station_alpha/roy_ram_rx.ld "$OUT/$kind.o" -o "$OUT/$kind.elf"
    "${SIZE:-llvm-size}" "$OUT/$kind.elf" 2>/dev/null || ls -l "$OUT/$kind.elf"
done
sha256sum "$OUT/"*.elf >"$OUT/modules.sha256"
