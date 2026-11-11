#!/bin/sh
# Source this file before the session. Read-only SWD, no halt/reset/write.
# Refusal drops the pending launch; an existing SRAM owner is never signalled.
roy_prepare() (
    set -eu
    kind=${ROY_MODULE_KIND:-single}
    case "$kind" in single|multi|pairing|service) ;; *) echo 'Roy layout refused: module kind'; exit 1;; esac
    SOURCE=${ROY_SOURCE:-/home/steamos/pico-ram-work/roy-source}
    mkdir -p /home/steamos/.local/state/frame-pico
    exec 8<>/home/steamos/.local/state/frame-pico/roy.lock
    flock -n 8 || { echo 'Roy layout deferred: another SRAM owner'; exit 1; }
    if pgrep -x 'openocd|pico_producer|producer|frame_pair|tracker_ota_too|roy_ram_loader' >/dev/null; then
        echo 'Roy layout deferred: Roy process alive'; exit 1
    fi
    work=$(mktemp -d /tmp/roy-layout.XXXXXXXX)
    trap 'rm -rf "$work"' EXIT
    tool="$SOURCE/roy_layout"
    [ -x "$tool" ] || { echo 'Roy layout refused: missing C++ locator'; exit 1; }
    ocd=${PICO_OPENOCD:-/home/openocd-tlmm/bin/openocd}
    cfg=/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg
    export PICO_TLMM_BASE=0xf100000 PICO_TLMM_DELAY=${PICO_TLMM_DELAY:-0}
    # nRF52840: full 1 MiB flash is read, including erased tail. No firmware is copied to the handoff.
    "$ocd" -f "$cfg" -c "gdb_port disabled; tcl_port disabled; telnet_port disabled; init; dump_image $work/flash.bin 0 0x100000; shutdown" >"$work/read.log" 2>&1 || { cat "$work/read.log"; exit 1; }
    rtt=$("$tool" "$work/flash.bin" --rtt-address) || exit 1
    "$ocd" -f "$cfg" -c "gdb_port disabled; tcl_port disabled; telnet_port disabled; init; dump_image $work/rtt.bin $rtt 168; shutdown" >>"$work/read.log" 2>&1 || { cat "$work/read.log"; exit 1; }
    "$tool" "$work/flash.bin" "$work/rtt.bin" >"$work/report" || { cat "$work/report"; exit 1; }
    # Consume only locator-generated numeric assignments, never arbitrary report text.
    sed -n '/^ROY_[A-Z0-9_]*=[0-9xabcdef]*$/p' "$work/report" >"$work/env"
    set -a; . "$work/env"; set +a
    alpha="$SOURCE/station_alpha"
    [ "$kind" != pairing ] || alpha="$SOURCE/pairing_alpha"
    # The module depends only on the located addresses. With clang + ld.lld it is built here; without them a
    # module prebuilt for exactly this set of addresses is used, if one was shipped.
    key=$(sha256sum "$work/env"); key=${key%% *}
    mkdir -p "$work/modules"
    if command -v "${CLANG:-clang}" >/dev/null 2>&1 && command -v "${LD_LLD:-ld.lld}" >/dev/null 2>&1; then
        OUT="$work/modules" ROY_KINDS="$kind" sh "$alpha/build.sh" >"$work/build.log" 2>&1 || { cat "$work/build.log"; exit 1; }
    elif [ -f "$SOURCE/prebuilt/$key/$kind.elf" ]; then
        cp "$SOURCE/prebuilt/$key/$kind.elf" "$work/modules/$kind.elf"
        echo "roy_module prebuilt $key" >>"$work/report"
    else
        echo "Roy layout refused: no clang/ld.lld on this headset and no prebuilt module for layout $key"; exit 1
    fi
    dest=${ROY_LAYOUT_DIR:-/tmp/pico-roy-layout-$kind}
    mkdir -p "$dest"
    cp "$work/flash.bin" "$dest/flash.bin"
    cp "$work/modules/$kind.elf" "$dest/module.elf"
    cp "$work/report" "$dest/report"
    sha=$(sha256sum "$dest/module.elf"); printf 'module_sha256 %s\n' "${sha%% *}" >>"$dest/report"
    sha=$(sha256sum "$dest/flash.bin"); printf 'flash_sha256 %s\n' "${sha%% *}" >>"$dest/report"
    cp "$work/env" "$dest/env"
    printf 'export ROY_LAYOUT_REPORT=%s/report\nexport PICO_TEST_MODULE=%s/module.elf\nexport ROY_FLASH=%s/flash.bin\n' "$dest" "$dest" "$dest" >>"$dest/env"
    cat "$dest/report"
)
if roy_prepare; then
    kind=${ROY_MODULE_KIND:-single}
    set -a; . "${ROY_LAYOUT_DIR:-/tmp/pico-roy-layout-$kind}/env"; set +a
else
    echo 'Roy layout unavailable: pending module load dropped' >&2
    return 1 2>/dev/null || exit 1
fi