#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p build
# Never touch an existing lock, including one belonging to a real build.
mkdir build/.build-lock
printf 'regression-owner\n' > build/.build-lock/pid
trap 'rm -f build/.build-lock/pid; rmdir build/.build-lock' EXIT
status=0
sh ./build.sh > build/lock-regression.log 2>&1 || status=$?
[ "$status" -eq 1 ]
grep 'Build already running' build/lock-regression.log
[ "$(cat build/.build-lock/pid)" = regression-owner ]
printf 'PASS concurrent build exclusion without lock theft\n'
