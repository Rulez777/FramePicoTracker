#!/bin/sh
# 2026-11-02 owner: Claude cleans up after its own sessions. XRService writes a ~1 GB Deckard_*.dataset per session (filled the
# Frame disk: 131 sets, 105 GB). Keep the 3 newest (capture.sh copies calibration json from the newest), delete the rest.
D=/home/steamos/.config/openvr/config/cv/xrservice/datasets
[ -d "$D" ] || exit 0
ls -td "$D"/Deckard_*.dataset 2>/dev/null | tail -n +4 | while read -r d; do rm -rf -- "$d"; done
