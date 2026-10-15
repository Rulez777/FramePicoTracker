# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Read-only SWD load probe: does debugger traffic alone disturb Roy's controllers?

No module, no RAM writes, no RF, no halt. Holds the shared Roy ownership lock,
reads the same regions the ABI4 station reads at start (2KB checkpoints) and
then polls a 120-byte header plus DWT at the station loop pace.
  venv/bin/python swd_read_probe.py --seconds 20 --log state/swd-probe-openocd.log
Run controller_baseline.py concurrently to see controller pose freezes.
"""
import argparse
import json
import time
from pathlib import Path
try:
    from .roy_ram_loader import Debugger,DATA,CODE
    from .ownership import RoyOwnership
except ImportError:
    from roy_ram_loader import Debugger,DATA,CODE
    from ownership import RoyOwnership


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--seconds',type=float,default=20)
    parser.add_argument('--log',type=Path,required=True)
    parser.add_argument('--checkpoint-only',action='store_true')
    args=parser.parse_args()
    with RoyOwnership():
        debug=Debugger(args.log)
        try:
            marks=[('attached',time.monotonic())]
            debug.read(CODE,2044);debug.read(DATA,2048);marks.append(('checkpoint_read',time.monotonic()))
            reads=0;latency=[]
            until=time.monotonic()+(0 if args.checkpoint_only else args.seconds)
            while time.monotonic()<until:
                t=time.monotonic_ns();debug.read(DATA,120);debug.read(0xe0001004,4)
                latency.append((time.monotonic_ns()-t)/1e6);reads+=2;time.sleep(.005)
            marks.append(('done',time.monotonic()))
        finally:debug.close()
    latency.sort()
    print(json.dumps({'marks':[(name,round(t-marks[0][1],3)) for name,t in marks],'reads':reads,
                      'median_pair_ms':latency[len(latency)//2] if latency else None,
                      'max_pair_ms':latency[-1] if latency else None}))


if __name__=='__main__':main()
