# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Restore a RAM experiment checkpoint only when MPSL has no live session."""
import argparse
import json
from pathlib import Path
from roy_ram_loader import Debugger, CODE, DATA, RTT, MPSL, SHELL_HOOK, MPU_STATIC_WORD, restore_permissions, session_is_active

def main():
    from ownership import RoyOwnership
    with RoyOwnership():
        recover()


def recover():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("checkpoint",type=Path)
    parser.add_argument("--log",type=Path,required=True)
    args=parser.parse_args()
    checkpoint=json.loads(args.checkpoint.read_text())
    debug=Debugger(args.log)
    try:
        session=debug.read(MPSL,48)
        if session_is_active(session):
            raise RuntimeError("MPSL session is live; cannot remove callback code")
        if debug.read(SHELL_HOOK,4)!=b"\0"*4:
            raise RuntimeError("Shell bypass remains active; cannot remove callback code")
        for address,value in (checkpoint.get('trace_controls') or {}).items():
            expected=bytes.fromhex(value)
            debug.write(int(address,16),expected)
            if debug.read(int(address,16),len(expected))!=expected:
                raise RuntimeError('Trace control restoration failed')
        if "mpu_static_word" in checkpoint:
            debug.write(MPU_STATIC_WORD,bytes.fromhex(checkpoint["mpu_static_word"]))
            import time
            time.sleep(.03)
            debug.command("mww 0xe000ed94 5")
        debug.write(CODE,bytes.fromhex(checkpoint["code"]))
        debug.write(DATA,bytes.fromhex(checkpoint["data"]))
        debug.write(RTT+16,bytes.fromhex(checkpoint["counts"]))
        restore_permissions(debug,checkpoint.get("mpu_permissions",[]))
        for address, key in ((CODE, 'code'), (DATA, 'data'), (RTT+16, 'counts'),
                             (MPU_STATIC_WORD, 'mpu_static_word')):
            if key in checkpoint:
                expected = bytes.fromhex(checkpoint[key])
                if debug.read(address,len(expected)) != expected:
                    raise RuntimeError('Checkpoint restoration failed: '+key)
        print("RAM and RTT restored; no live MPSL callback; flash was not written")
    finally:
        debug.close()

if __name__=="__main__": main()
