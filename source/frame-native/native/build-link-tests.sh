#!/bin/sh
set -eu
cd "$(dirname "$0")"
. ./build_common.sh
mkdir -p build/link-regression
# Zero-exit compiler that does not create output must never admit a stale binary.
printf '#!/bin/sh\nexit 0\n' > build/link-regression/no-output
chmod +x build/link-regression/no-output
printf 'stale\n' > build/link-regression/program
if (link build/link-regression/program ./build/link-regression/no-output) > build/link-regression/missing.log 2>&1; then exit 1; fi
if ! grep 'OUTPUT MISSING' build/link-regression/missing.log; then exit 1; fi
if ! grep 'command:' build/link-regression/missing.log; then exit 1; fi
printf '#!/bin/sh\nexit 23\n' > build/link-regression/fail
chmod +x build/link-regression/fail
status=0
(link build/link-regression/program ./build/link-regression/fail) > build/link-regression/fail.log 2>&1 || status=$?
[ "$status" -eq 23 ]
grep 'LINK FAILED (exit 23)' build/link-regression/fail.log
printf 'PASS link regressions\n'
# Signal-like compiler termination and its stderr must survive the wrapper.
printf '#!/bin/sh\necho linker-memory-fixture >&2\nexit 137\n' > build/link-regression/killed
chmod +x build/link-regression/killed
status=0
(link build/link-regression/program ./build/link-regression/killed) > build/link-regression/killed.log 2>&1 || status=$?
[ "$status" -eq 137 ]
grep 'LINK exit=137' build/link-regression/killed.log
grep 'linker-memory-fixture' build/link-regression/killed.log
# Optional sanitizer stage cannot run an old binary in a default build.
PICO_SANITIZE=0
run build/definitely_missing_sanitize
PICO_SANITIZE=1
status=0
(run build/definitely_missing_sanitize) > build/link-regression/run.log 2>&1 || status=$?
[ "$status" -eq 127 ]
printf 'PASS signal/stderr and sanitizer-stage regressions\n'