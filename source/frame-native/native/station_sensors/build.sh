#!/bin/sh
# Compile only. Does not connect, install, select, or admit a module.
set -eu
cd "$(dirname "$0")"
CLANG=${CLANG:-clang}
LD_LLD=${LD_LLD:-ld.lld}
OBJDUMP=${OBJDUMP:-llvm-objdump}
# Same Thumb/LTO/size/outliner settings as known-good station-ack/build.sh.
# Separate stages also work with the Android clang bare-metal driver.
"$CLANG" --target=arm-none-eabi -mcpu=cortex-m4 -mthumb -Oz -flto -fomit-frame-pointer -ffreestanding -fno-builtin -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -c roy_station_sensors.c -o sensors.o
"$LD_LLD" --mllvm=-enable-machine-outliner=always --mllvm=-machine-outliner-reruns=2 --gc-sections -T roy_ram_rx.ld sensors.o -o sensors.elf
"$OBJDUMP" -h sensors.elf
sha256sum sensors.elf
