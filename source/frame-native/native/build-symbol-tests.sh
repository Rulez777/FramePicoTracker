#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p build/symbol-regression
cat > build/symbol-regression/compiler <<'EOF'
#!/bin/sh
source=
while [ "$#" -gt 0 ]; do
    case "$1" in *.cpp) source=$1;; -o) shift; output=$1;; esac
    shift
done
cp "$source" "$output"
EOF
cat > build/symbol-regression/nm <<'EOF'
#!/bin/sh
shift 2
cat "$1"
EOF
chmod +x build/symbol-regression/compiler build/symbol-regression/nm
export CXX=./build/symbol-regression/compiler NM=./build/symbol-regression/nm
printf 'one T 0 1\nweak W 0 1\nlocal t 0 1\nunique u 0 1\n' > build/symbol-regression/one.cpp
printf 'two D 0 1\nweak W 0 1\nlocal t 0 1\nunique u 0 1\n' > build/symbol-regression/two.cpp
sh ./build-check-symbols.sh build/symbol-regression/audit build/symbol-regression/one.cpp build/symbol-regression/two.cpp
printf 'one T 0 1\n' >> build/symbol-regression/two.cpp
if sh ./build-check-symbols.sh build/symbol-regression/audit build/symbol-regression/one.cpp build/symbol-regression/two.cpp > build/symbol-regression/duplicate.log 2>&1; then
    echo 'ERROR: duplicate definition accepted' >&2; exit 1
fi
grep 'DUPLICATE STRONG DEFINITION: one' build/symbol-regression/duplicate.log
grep 'also in build/symbol-regression/two.cpp' build/symbol-regression/duplicate.log
# Tool errors must not silently produce a clean audit.
NM=false
export NM
if sh ./build-check-symbols.sh build/symbol-regression/audit build/symbol-regression/one.cpp; then exit 1; fi
printf 'PASS symbol audit regressions\n'
