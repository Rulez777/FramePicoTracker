#!/bin/sh
# Compile only; same target, LTO and outliner as station-ack/build.sh.
# Unset address variables keep today's defaults. Set any of:
#   ROY_DATA ROY_CODE ROY_REGIONS ROY_FN_* ROY_MPU_*
set -eu
cd "$(dirname "$0")"
CLANG=${CLANG:-clang}
LD_LLD=${LD_LLD:-ld.lld}
OUT=${OUT:-../../outputs/alpha-session}
mkdir -p "$OUT"
"$CLANG" --version

defs=""
add_def() {
  eval "val=\${$1-}"
  if [ -n "$val" ]; then
    defs="$defs -D$1=$val"
  fi
}
add_def ROY_DATA
add_def ROY_CODE
add_def ROY_REGIONS
while IFS= read -r line; do
  case $line in
    ROY_FN_*=*|ROY_MPU_*=*)
      name=${line%%=*}
      val=${line#*=}
      if [ -n "$val" ]; then
        defs="$defs -D$name=$val"
      fi
      ;;
  esac
done <<END
$(env)
END

code_origin=${ROY_CODE:-0x2001151c}
for kind in ${ROY_KINDS:-single multi}; do
  source=$kind; extra=""
  if [ "$kind" = pairing ]; then source=single; extra=-DPICO_PAIRING; fi
  # $defs is a list of -D flags, empty when no overrides are set.
  "$CLANG" --target=arm-none-eabi -mcpu=cortex-m4 -mthumb -Oz -flto -fomit-frame-pointer -ffreestanding -fno-builtin -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections $defs $extra -c "$source.c" -o "$OUT/$kind.o"
  "$LD_LLD" --mllvm=-enable-machine-outliner=always --mllvm=-machine-outliner-reruns=2 --gc-sections --defsym=ROY_CODE="$code_origin" -T roy_ram_rx.ld "$OUT/$kind.o" -o "$OUT/$kind.elf"
  "${OBJDUMP:-objdump}" -h "$OUT/$kind.elf"
  sha256sum "$OUT/$kind.elf"
done
