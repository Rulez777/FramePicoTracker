# Reader only. Never manufactures a READY answer, approval or readiness file.
ota_ready_feed() {
    attempt=$1
    remaining=${PICO_OTA_READY_WAIT_SECONDS:-120}
    while [ "$remaining" -gt 0 ]; do
        # Tool flushes this line before starting its independent 120s deadline.
        if grep -q 'Type: READY ' "$attempt/ota.log"; then break; fi
        sleep 1
        remaining=$((remaining - 1))
    done
    while [ "$remaining" -gt 0 ]; do
        if [ -f "$attempt/ready.txt" ]; then
            cat "$attempt/ready.txt"
            return
        fi
        sleep 1
        remaining=$((remaining - 1))
    done
    # EOF makes the tool reject readiness, before any SRAM installation.
    echo 'READY file deadline expired; no answer supplied' >&2
}
