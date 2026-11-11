# Sourced by launcher and offline format regression. Produces a proposal ONLY.
ota_approval_body() {
    issued=$1; image_sha=$2; shift 2
    chip=d4c3b2a1; serial=PCTESTTRACKER0001; key=
    for arg do
        case "$key" in --chip) chip=$arg;; --serial) serial=$arg;; esac
        key=$arg
    done
    printf 'fw189-ota-approval-v2\nunix=%s\nchip=%s\nserial=%s\nfirmware=189\nsha256=%s\n' "$issued" "$chip" "$serial" "$image_sha"
    for arg do
        printf 'argv='
        printf '%s' "$arg" | od -An -v -tx1 | tr -d ' \n'
        printf '\n'
    done
}
