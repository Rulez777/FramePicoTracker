# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Station-off controller pose-freshness baseline. Read-only OpenVR, no SWD/RF.

Run on Frame with the HMD and both controllers tracked (on a bench is fine):
  venv/bin/python controller_baseline.py --watch release/tracking_watch --seconds 120 --out state/baseline.jsonl
Prints the worst per-second freeze and invalid episodes per controller; that
sets ControllerGuard.freeze_limit_ms for RF coexistence runs.
"""
import argparse
import json
import time
from pathlib import Path
try:from .controller_guard import ControllerGuard
except ImportError:from controller_guard import ControllerGuard


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--watch',type=Path,required=True)
    parser.add_argument('--seconds',type=float,default=120)
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    guard=ControllerGuard(args.watch,metrics_log=args.out)
    try:
        until=time.monotonic()+args.seconds
        while time.monotonic()<until:
            if guard.failure is not None:raise RuntimeError('tracking_watch failed') from guard.failure
            time.sleep(.5)
    finally:guard.close()
    worst={}
    for line in args.out.read_text().splitlines():
        for index,w in json.loads(line)['controllers'].items():
            agg=worst.setdefault(index,{'max_freeze_ms':0.,'invalid_episodes':0,'min_change_rate':None})
            agg['max_freeze_ms']=max(agg['max_freeze_ms'],w['max_freeze_ms'])
            agg['invalid_episodes']+=w['invalid_episodes']
            rate=w['changes']/max(1,w['samples'])
            agg['min_change_rate']=rate if agg['min_change_rate'] is None else min(agg['min_change_rate'],rate)
    print(json.dumps(worst))


if __name__=='__main__':main()
