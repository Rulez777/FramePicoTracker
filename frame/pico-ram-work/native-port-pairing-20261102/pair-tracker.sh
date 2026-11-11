#!/bin/sh
# Pair one PICO tracker to this Steam Frame with no IDs to type.
# Put the tracker into pairing (discovery) mode, keep other trackers off, then run: sh pair-tracker.sh
# It listens for the tracker, pairs it on the first free slot and records it in devices.conf and the app's tracker list.
set -u
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
W=/home/steamos/pico-ram-work
CONF=${PICO_DEVICES:-$W/devices.conf}
PEERS=$W/frame-app-peers.txt
export PICO_PAIR_WATCH=${PICO_PAIR_WATCH:-$W/session-template/release/tracking_watch}
touch "$CONF" "$PEERS"

SLOT=""
for s in 0 1 2; do
    awk -v s=$s '$1=="tracker" && $3==s {f=1} END{exit !f}' "$CONF" || { SLOT=$s; break; }
done
[ -n "$SLOT" ] || { echo "All three slots are taken in $CONF. Remove a tracker line to pair another one."; exit 2; }

echo "Tracker in pairing mode, other trackers OFF, not on USB, headset on its tracked spot, controllers ON."
printf "Type GO (or да) to start: "; read -r GO
case "$GO" in GO|go|да) ;; *) echo "Not started."; exit 2;; esac

T=$(date +%H%M%S)
echo "1/2 Listening for the tracker (up to 30 s)..."
echo "$GO" | PICO_PAIR_MAX_S=30 sh "$HERE/frame-pair.sh" 0 "$SLOT" - /home/steamos/pair-scan-$T >/dev/null 2>&1
CHIP=$(sed -n 's/^scan_result chip=\([0-9A-F]\{8\}\)$/\1/p' /home/steamos/pair-scan-$T/pair.log | tail -1)
[ -n "$CHIP" ] || { echo "No tracker heard. Is it in pairing mode and close to the headset? Log: /home/steamos/pair-scan-$T/pair.log"; exit 3; }
echo "   heard tracker $CHIP"

echo "2/2 Pairing on slot $SLOT..."
echo "$GO" | sh "$HERE/frame-pair.sh" "$CHIP" "$SLOT" - /home/steamos/pair-$CHIP-$T >/dev/null 2>&1
LOG=/home/steamos/pair-$CHIP-$T/pair.log
grep -q "product_verified=1" "$LOG" || { echo "Pairing did not finish. Log: $LOG"; exit 4; }
SERIAL=$(sed -n 's/.* serial=\([^ ]*\).*/\1/p' "$LOG" | grep -v '^$' | tail -1)

grep -v "^tracker $CHIP " "$CONF" > "$CONF.new"; echo "tracker $CHIP $SLOT $SERIAL" >> "$CONF.new"; mv "$CONF.new" "$CONF"
grep -v "^$CHIP " "$PEERS" > "$PEERS.new"; echo "$CHIP $SLOT $SERIAL" >> "$PEERS.new"; mv "$PEERS.new" "$PEERS"
rm -rf /home/steamos/pair-scan-$T /home/steamos/pair-$CHIP-$T
echo "Paired: tracker $CHIP, slot $SLOT, serial $SERIAL (saved in $CONF)."
