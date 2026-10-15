# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Stock Terminal0 RTT console, without halting Nordic or touching flash.

Only sends shell text supplied on CLI; defaults to the read-only `help` command.
"""
import argparse
import json
import struct
import time
from pathlib import Path
from roy_ram_loader import Debugger, RTT

def send(debug, command, wait=.15):
    up=struct.unpack("<6I",debug.read(RTT+24,24))
    down=struct.unpack("<6I",debug.read(RTT+96,24))
    if up[1:3]!=(0x20012060,4096) or down[1:3]!=(0x20012020,64):
        raise RuntimeError("Stock RTT layout differs")
    payload=command.encode()+b"\n"
    if len(payload)>60 or b"\0" in payload:
        raise ValueError("Console command must be at most 59 bytes")
    if (down[4]-down[3]-1)%64<len(payload):
        raise RuntimeError("Console input ring is full")
    # Consume prior output so the stock shell can print the new response.
    debug.command(f"mww {RTT+40:#x} {up[3]}")
    for offset,value in enumerate(payload):
        debug.command(f"mwb {down[1]+(down[3]+offset)%64:#x} {value}")
    debug.command(f"mww {RTT+108:#x} {(down[3]+len(payload))%64}")
    time.sleep(wait)
    up_after=struct.unpack("<6I",debug.read(RTT+24,24))
    down_after=struct.unpack("<6I",debug.read(RTT+96,24))
    buffer=debug.read(up[1],up[2])
    start,end=up[3],up_after[3]
    output=buffer[start:end] if end>=start else buffer[start:]+buffer[:end]
    debug.command(f"mww {RTT+40:#x} {end}")
    return {"command":command,"up_before":up,"up_after":up_after,
            "down_before":down,"down_after":down_after,
            "output":output.decode(errors="replace")}

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--command",default="help")
    p.add_argument("--out",type=Path,required=True)
    p.add_argument("--log",type=Path,required=True)
    args=p.parse_args()
    debug=Debugger(args.log)
    try:
        report=send(debug,args.command)
        args.out.write_text(json.dumps(report,indent=2)+"\n")
        print(json.dumps(report))
    finally:
        debug.close()

if __name__=="__main__":main()
