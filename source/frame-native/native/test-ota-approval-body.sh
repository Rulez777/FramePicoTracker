#!/bin/sh
# Offline mock proposal; never writes owner-approval.txt.
set -eu
cd "$(dirname "$0")"
. ./ota_approval_body.sh
binary=${1:-./build/ota_approval_body_tests}
fixture=${2:-./build/approval-proposal-fixture.txt}
ota_approval_body 1700000000 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa \
    '/tmp/tool with space' --ota "quote' dollar\$ backslash\\" 'line
break' '' >"$fixture"
"$binary" "$fixture"
for target in e2c3b2a1 e1c3b2a1; do
    case "$target" in
        e2c3b2a1) serial=PCTESTTRACKER0003;;
        e1c3b2a1) serial=PCTESTTRACKER0002;;
    esac
    ota_approval_body 1700000000 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa \
        '/tmp/tool with space' --ota "quote' dollar\$ backslash\\" 'line
break' '' --chip "$target" --serial "$serial" --stock-approval mock.fixture >"$fixture"
    "$binary" "$fixture" "$target" "$serial"
done
