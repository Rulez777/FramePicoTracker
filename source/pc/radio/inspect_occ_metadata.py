# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Bounded read-only metadata audit of existing OCC, no recording or RF."""
import argparse
import json
import statistics
import time
from pathlib import Path
from .frame_occ_live import GrowingOCC,camera_metadata


def inspect(path,limit=300):
    cameras={}
    reader=GrowingOCC(path)
    for kind,data in reader.iter_records(max_records=limit,max_bytes=64*1024*1024):
        for item in camera_metadata(kind,data):
            key=str(item['stream_key'])
            entry=cameras.setdefault(key,{'samples':[]})
            if len(entry['samples'])<3:entry['samples'].append(item)
            if 'timestamp_raw' in item:entry.setdefault('times',[]).append(item['timestamp_raw'])
    for entry in cameras.values():
        times=entry.pop('times',[])
        steps=[b-a for a,b in zip(times,times[1:]) if b>a]
        entry['median_positive_step_raw']=statistics.median(steps) if steps else None
    return {'source':str(path),'bytes_read':reader.offset,'camera_streams':cameras,
            'frame_monotonic_seconds':time.monotonic(),'frame_unix_seconds':time.time()}


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('path',type=Path)
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    result=inspect(args.path)
    args.out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result))
