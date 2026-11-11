# Sourced by launcher and offline format regression. Produces a proposal ONLY.
ota_approval_body() {
    issued=$1; image_sha=$2; shift 2
    chip=; serial=; key=  # always passed by the launcher (--chip, --serial)
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
