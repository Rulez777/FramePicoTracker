#!/bin/sh
# Before native/build.sh: point the test inputs at the files install.sh put on the Frame (no copies kept here).
cd "$(dirname "$0")"
W=/home/steamos/pico-ram-work
ln -sfn $W/native-port/candidate candidate
ln -sfn $W/native-port-r3-20261101/outputs outputs
echo "candidate -> $W/native-port/candidate, outputs -> $W/native-port-r3-20261101/outputs"
