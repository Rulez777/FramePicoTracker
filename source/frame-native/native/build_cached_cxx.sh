#!/usr/bin/env bash
# Content-addressed objects; sequential, with private compiler temporary files.
set -euo pipefail
compiler=${PICO_REAL_CXX:-g++}
mkdir -p build/cache
identity=$("$compiler" --version)
compiler_path=$(command -v "$compiler")
compiler_digest=$(sha256sum "$compiler_path" "$0")
environment=$(env | sed '/^TMPDIR=/d; /^TMP=/d; /^TEMP=/d; /^PICO_SANITIZE=/d; /^SHLVL=/d; /^_=/d' | LC_ALL=C sort)
headers=$(find . -path ./build -prune -o -type f \( -name '*.hpp' -o -name '*.h' \) -exec sha256sum {} \; | LC_ALL=C sort)
args=("$@")
flags=()
compile_only=false
for ((i=0;i<${#args[@]};i++)); do
    arg=${args[i]}
    case "$arg" in
        -o) ((i+=1));;
        -c) compile_only=true;;
        *.cpp|*.o|*.a|-l*|-Wl,*) ;;
        *) flags+=("$arg");;
    esac
done
# Explicit object compilation is already cheap on a warm build through this cache.
output=
for ((i=0;i<${#args[@]};i++)); do
    if [[ ${args[i]} == -o ]]; then output=${args[i+1]}; fi
done
for ((i=0;i<${#args[@]};i++)); do
    source=${args[i]}
    [[ $source == *.cpp ]] || continue
    key=$({ printf '%s\n' "$compiler" "$identity" "$compiler_digest" "$environment" "$headers" "${flags[@]}"; sha256sum "$source"; } | sha256sum | cut -d' ' -f1)
    object=build/cache/$key.o
    if [[ ! -s $object ]]; then
        stage=$(mktemp -d build/cache/compile.XXXXXX)
        if "$compiler" "${flags[@]}" -c "$source" -o "$stage/object.o"; then
            [[ -s $stage/object.o ]] || { echo "Compiler succeeded without object: $source" >&2; exit 1; }
            mv "$stage/object.o" "$object"
            rmdir "$stage"
        else
            status=$?
            echo "COMPILE FAILED exit=$status source=$source retained=$stage" >&2
            exit "$status"
        fi
    fi
    args[i]=$object
done
if $compile_only; then
    for arg in "${args[@]}"; do
        case "$arg" in build/cache/*.o) cmp -s "$arg" "$output" || cp "$arg" "$output"; exit 0;; esac
    done
fi
# Link identity excludes the random destination, but includes all object/archive bytes.
normalized=("${args[@]}")
for ((i=0;i<${#normalized[@]};i++)); do
    if [[ ${normalized[i]} == -o ]]; then normalized[i+1]='<output>'; fi
done
key=$({ printf '%s\n' "$compiler" "$identity" "$compiler_digest" "$environment" "${normalized[@]}"; for arg in "${args[@]}"; do case "$arg" in *.o|*.a|*.so) sha256sum "$arg";; esac; done; } | sha256sum | cut -d' ' -f1)
artifact=build/cache/link-$key
if [[ -s $artifact && -x $artifact ]]; then
    printf 'CACHE LINK HIT %s\n' "$artifact" >&2
    cp "$artifact" "$output"
    exit 0
fi
printf 'compiler-link:' >&2
printf ' <%s>' "$compiler" "${args[@]}" >&2
printf '\n' >&2
status=0
"$compiler" "${args[@]}" || status=$?
if [[ $status != 0 ]]; then exit "$status"; fi
[[ -s $output && -x $output ]] || { echo 'Linker succeeded without executable' >&2; exit 1; }
cp "$output" "$artifact.tmp.$$"
mv "$artifact.tmp.$$" "$artifact"