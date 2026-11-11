#!/bin/sh
set -eu
cd "$(dirname "$0")"
case "${1:-}" in
  '') dest=build/few-plain; extra='-O2';;
  --sanitize) dest=build/few-asan; extra='-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer';;
  *) echo 'usage: build-few-led.sh [--sanitize]' >&2; exit 2;;
esac
mkdir -p "$dest"
. ./build_sources.sh
flags="-std=c++17 $extra -Wall -Wextra -Wpedantic -Werror -Wno-error=maybe-uninitialized -Wno-error=dangling-reference -fno-fast-math -ffp-contract=off -pthread -I."
cxx=${CXX:-g++}
sources="data_plane.cpp $(pico_control_sources '') optical_timing.cpp pose_publication.cpp optical_worker.cpp"
objects=''
for source in $sources; do
  object="$dest/$(basename "$source" .cpp).o"
  $cxx $flags -c "$source" -o "$object"
  objects="$objects $object"
done
ar rcs "$dest/control.a" $objects
export PICO_IDENTITY_EPISODES=1
for suite in multi_optics_tests few_led_tests recovery_tests recovery2_tests recovery3_tests optical_prior_tests native_identity_tests optical_worker_tests mono_solver_tests; do
  $cxx $flags "$suite.cpp" "$dest/control.a" -ldl -o "$dest/$suite"
  "$dest/$suite"
done
$cxx $flags tools/capture_replay.cpp replay_calibration.cpp "$dest/control.a" -ldl -o "$dest/capture_replay"
# capture_replay's legacy root lookup assumes native/build/<binary>.
# Its new nested output directory needs an explicit runtime library root (see source).
