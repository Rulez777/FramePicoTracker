# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Follow growing OCC and demultiplex camera H264 while recording runs.

Recorded stream keys are retained verbatim. No guessed camera geometry or pose.
"""
import argparse
import json
import time
from pathlib import Path
from .frame_occ_live import GrowingOCC, camera_chunks, camera_metadata


def follow(source, output, seconds, *, disk_budget=256*1024*1024, live=True, decode=False):
    output.mkdir(parents=True,exist_ok=True)
    reader=GrowingOCC(source)
    skipped = reader.seek_complete_tail() if live else 0
    written = 0
    streams={}
    decoders={}
    decoded_counts={}
    counts={}
    metadata=(output/'metadata.jsonl').open('w',encoding='utf-8')
    started=time.monotonic()
    try:
        while time.monotonic()-started<seconds:
            for kind,value in reader.iter_records():
                for item in camera_metadata(kind,value):
                    line = json.dumps(item)+'\n'
                    written += len(line.encode('utf-8'))
                    if written > disk_budget:
                        raise RuntimeError('Camera recording disk budget exceeded')
                    metadata.write(line)
                metadata.flush()
                for key,data in camera_chunks(kind,value):
                    written += len(data)
                    if written > disk_budget:
                        raise RuntimeError('Camera recording disk budget exceeded')
                    name='-'.join(map(str,key))
                    if name not in streams:
                        streams[name]=(output/(name+'.h264')).open('wb')
                        counts[name]=0
                    streams[name].write(data)
                    streams[name].flush()
                    counts[name]+=1
                    if decode:
                        if key not in decoders:
                            from optical.live_decode import LiveGrayDecoder
                            decoders[key]=LiveGrayDecoder(key)
                            decoded_counts[name]=0
                        decoder=decoders[key]
                        decoder.feed(data)
                        while not decoder.frames.empty():
                            frame=decoder.frames.get_nowait()
                            decoded_counts[name]+=1
            time.sleep(.02)
    finally:
        metadata.close()
        for stream in streams.values():stream.close()
        for decoder in decoders.values():decoder.close()
    report={'streams':counts,'bytes_consumed':reader.offset-skipped,
            'history_bytes_skipped':skipped, 'disk_bytes_written':written,
            'decoded_frames':decoded_counts, 'source_time_verified':False,
            'geometry_verified':False,'poses_valid':False}
    (output/'transport.json').write_text(json.dumps(report),encoding='utf-8')
    return report


if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('source',type=Path)
    parser.add_argument('--out',type=Path,required=True)
    parser.add_argument('--seconds',type=float,default=10)
    parser.add_argument('--decode',action='store_true')
    args=parser.parse_args()
    print(json.dumps(follow(args.source,args.out,args.seconds,decode=args.decode)))
