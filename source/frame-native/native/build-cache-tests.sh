#!/bin/sh
set -eu
cd "$(dirname "$0")"
. ./build_common.sh
mkdir -p build/cache-regression
# No real compiler or hardware: record each invocation and emit synthetic outputs.
cat > build/cache-regression/compiler <<'EOF'
#!/bin/sh
if [ "${1:-}" = --version ]; then echo fixture-compiler-v1; exit 0; fi
printf 'call\n' >> build/cache-regression/calls
output=
while [ "$#" -gt 0 ]; do
    if [ "$1" = -o ]; then shift; output=$1; fi
    shift
done
printf '#!/bin/sh\nexit 0\n' > "$output"
chmod +x "$output"
EOF
chmod +x build/cache-regression/compiler
export PICO_REAL_CXX=./build/cache-regression/compiler
printf '// fixture source\n' > build/cache-regression/input.cpp
: > build/cache-regression/calls
# Use an explicit unique define to isolate the test from earlier retained caches.
stamp=$(date +%s)-$$
PICO_SANITIZE=0 bash ./build_cached_cxx.sh -DTEST="$stamp" build/cache-regression/input.cpp -o build/cache-regression/one
[ "$(wc -l < build/cache-regression/calls)" -eq 2 ]
PICO_SANITIZE=1 bash ./build_cached_cxx.sh -DTEST="$stamp" build/cache-regression/input.cpp -o build/cache-regression/two
[ "$(wc -l < build/cache-regression/calls)" -eq 2 ]
printf '// changed source\n' >> build/cache-regression/input.cpp
PICO_SANITIZE=1 bash ./build_cached_cxx.sh -DTEST="$stamp" build/cache-regression/input.cpp -o build/cache-regression/two
[ "$(wc -l < build/cache-regression/calls)" -eq 4 ]
bash ./build_cached_cxx.sh -DTEST="$stamp" -O1 build/cache-regression/input.cpp -o build/cache-regression/two
[ "$(wc -l < build/cache-regression/calls)" -eq 6 ]
# Header changes must invalidate even when the source is unchanged.
printf '// header\n' > build/cache-regression/header.hpp
# build/ is intentionally excluded; use a bounded fixture header in this directory.
printf '// header\n' > cache-regression-header.hpp
trap 'rm -f cache-regression-header.hpp' EXIT
bash ./build_cached_cxx.sh -DTEST="$stamp" -O1 build/cache-regression/input.cpp -o build/cache-regression/two
[ "$(wc -l < build/cache-regression/calls)" -eq 8 ]
printf '// changed\n' >> cache-regression-header.hpp
bash ./build_cached_cxx.sh -DTEST="$stamp" -O1 build/cache-regression/input.cpp -o build/cache-regression/two
[ "$(wc -l < build/cache-regression/calls)" -eq 10 ]
# Explicit -c reuses the same cached object, without invoking the compiler.
bash ./build_cached_cxx.sh -DTEST="$stamp" -O1 -c build/cache-regression/input.cpp -o build/cache-regression/explicit.o
[ "$(wc -l < build/cache-regression/calls)" -eq 10 ]
[ -s build/cache-regression/explicit.o ]
printf 'archive v1\n' > build/cache-regression/input.a
bash ./build_cached_cxx.sh -DTEST="$stamp" -O1 build/cache-regression/input.cpp build/cache-regression/input.a -o build/cache-regression/two
[ "$(wc -l < build/cache-regression/calls)" -eq 11 ]
printf 'archive v2\n' > build/cache-regression/input.a
bash ./build_cached_cxx.sh -DTEST="$stamp" -O1 build/cache-regression/input.cpp build/cache-regression/input.a -o build/cache-regression/two
[ "$(wc -l < build/cache-regression/calls)" -eq 12 ]
printf 'PASS compiler cache: warm reuse, explicit objects, source/flags/header/archive invalidation\n'
