#!/bin/sh
# Compile every supplied translation unit, including otherwise unextracted archive
# members. Reject repeated strong external definitions before linking any tests.
set -eu
directory=$1
shift
mkdir -p "$directory"
: > "$directory/symbols"
index=0
for source do
    index=$((index + 1))
    object="$directory/$index.o"
    # CXX and flags are intentionally word-split, like the native build driver.
    ${CXX:-g++} ${PICO_CHECK_FLAGS:--std=c++17 -O2 -pthread} -c "$source" -o "$object"
    ${NM:-nm} -P --defined-only "$object" > "$directory/nm"
    # Local, weak, GNU unique and undefined symbols are legal repetitions.
    # Do not demangle: symbol names must remain single fields and ABI-exact.
    awk -v source="$source" '$2 ~ /^[ABCDGRST]$/ { print $1, source }' \
        "$directory/nm" >> "$directory/symbols"
done
LC_ALL=C sort "$directory/symbols" > "$directory/sorted"
awk '
    $1 == previous {
        if (!reported) print "DUPLICATE STRONG DEFINITION: " previous " in " origin > "/dev/stderr"
        print "  also in " $2 > "/dev/stderr"
        reported=1; failed=1; next
    }
    { previous=$1; origin=$2; reported=0 }
    END { exit failed ? 1 : 0 }
' "$directory/sorted"
printf 'PASS strong-symbol audit: %s translation units\n' "$index"
