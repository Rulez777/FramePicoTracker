#!/bin/sh
# Prepare a sealed root from this tree (same convention as native-port/live-visual.sh) and run live-visual-multi-alpha.sh.
# Usage: sh run-multi.sh   (trackers from devices.conf; PICO_PEERS=CHIP:SLOT:SERIAL,... overrides)
[ -n "${PICO_PEERS:-}" ] || PICO_PEERS=$(awk '$1=="tracker"{printf "%s%s:%s:%s",(n++?",":""),$2,$3,$4}' "${PICO_DEVICES:-/home/steamos/pico-ram-work/devices.conf}" 2>/dev/null); export PICO_PEERS
T=/home/steamos/pico-ram-work/native-port-mrec-20261103
W=${1:-"го прогон"}
export ROY_MODULE_KIND=multi
. /home/steamos/pico-ram-work/roy-prepare.sh || exit 2
cd $T/native || exit 2
R=/home/steamos/pico-ram-work/native-port/native-live-$(date +%Y%m%d-%H%M%S)
B=$(cat /proc/sys/kernel/random/boot_id)
D=$T/owner-decision-$(date +%Y%m%d-%H%M%S).json
printf "{\"owner_words\":\"%s\",\"timestamp\":\"%s\",\"boot_id\":\"%s\"}\n" "$W" "$(date -u +%Y-%m-%dT%H:%M:%SZ)" "$B" > "$D"
OCD=/usr/bin/openocd; [ -x /home/openocd-tlmm/bin/openocd ] && OCD=/home/openocd-tlmm/bin/openocd
build/make_native_invocation --openocd $OCD --root "$R" --owner-admit-unproven "$D" --human-answer "$W" --hmd-tracked --controllers-off --tracker-supported --hmd-worn -- "$R/producer" --live --owner-admit-unproven "$R/attempt/owner-decision.json" "$R" "$R/invocation.json" "$R/invocation.sha256" --duration ${PICO_DURATION:-150} > /tmp/live-prep.out 2>&1 || { echo "prep failed"; tail -4 /tmp/live-prep.out; exit 3; }
exec sh $T/live-visual-multi-alpha.sh "$R"
