# Shared POSIX build diagnostics. Link runs in a subshell: no variable leakage.
show_command() {
    printf 'command:' >&2
    for arg do printf ' <%s>' "$arg" >&2; done
    printf '\n' >&2
}
build_enabled() {
    [ "${PICO_SANITIZE+x}" = x ] || return 0
    case " $* " in *sanitize*) [ "$PICO_SANITIZE" = 1 ];; *) return 0;; esac
}
run() {
    build_enabled "$@" || return 0
    show_command "$@"
    if "$@"; then return 0; else
        status=$?
        printf 'FAILED (exit %s) in %s\n' "$status" "$PWD" >&2
        if [ "$status" -eq 126 ] || [ "$status" -eq 127 ]; then
            # ENOENT can mean a missing ELF interpreter, even if the file exists.
            ls -ld "$1" "${1%/*}" >&2 2>/dev/null || :
            if command -v file >/dev/null 2>&1; then file "$1" >&2 || :; fi
            if command -v readelf >/dev/null 2>&1; then readelf -l "$1" >&2 2>/dev/null || :; fi
        fi
        show_command "$@"
        exit "$status"
    fi
}
link_memory_evidence() {
        if [ -r "$1/memory.before" ]; then printf 'cgroup memory.events BEFORE link\n' >&2; cat "$1/memory.before" >&2; fi
        # Best-effort evidence only; restricted kernel logs are not proof of no OOM.
        if [ -r /proc/meminfo ]; then cat /proc/meminfo >&2; fi
        for f in /sys/fs/cgroup/memory.events /sys/fs/cgroup/memory.peak /proc/pressure/memory; do
            if [ -r "$f" ]; then printf '%s\n' "$f" >&2; cat "$f" >&2; fi
        done
}
link() (
    build_enabled "$@" || exit 0
    output=$1
    shift
    stage=$(mktemp -d "${output}.link.XXXXXX") || exit 1
    mkdir "$stage/tmp" || exit 1
    TMPDIR="$PWD/$stage/tmp"
    export TMPDIR
    if [ -r /sys/fs/cgroup/memory.events ]; then cat /sys/fs/cgroup/memory.events > "$stage/memory.before"; fi
    show_command "$@" -o "$stage/program"
    status=0
    "$@" -o "$stage/program" >"$stage/stdout" 2>"$stage/stderr" || status=$?
    printf '%s\n' "$status" > "$stage/status"
    printf 'LINK exit=%s output=%s diagnostics=%s\n' "$status" "$output" "$stage" >&2
    cat "$stage/stdout"
    cat "$stage/stderr" >&2
    if [ "$status" -ne 0 ]; then
        printf 'LINK FAILED (exit %s), output %s\n' "$status" "$output" >&2
        link_memory_evidence "$stage"
        exit "$status"
    fi
    if [ ! -s "$stage/program" ] || [ ! -x "$stage/program" ]; then
        printf 'LINK REPORTED SUCCESS BUT OUTPUT MISSING/EMPTY/NOT EXECUTABLE: %s\n' "$stage/program" >&2
        link_memory_evidence "$stage"
        exit 1
    fi
    # Keep stderr and status even after a successful atomic installation.
    mv -f "$stage/stderr" "$output.link.stderr"
    printf '%s\n' "$status" > "$output.link.status"
    mv -f "$stage/program" "$output" || exit 1
    rm -f "$stage/stdout"
    rm -f "$stage/status" "$stage/memory.before"
    rmdir "$stage/tmp" "$stage"
)
