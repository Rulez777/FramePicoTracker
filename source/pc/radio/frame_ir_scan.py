# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Bounded offline IR component scan; no hardware access or pose publication."""
import argparse
import json
import subprocess
import sys
from pathlib import Path
import numpy as np


def scan(source: Path, width: int, height: int, maximum: int = 300):
    if not 1 <= maximum <= 600 or not 1 <= width*height <= 4096*4096:
        raise ValueError('Invalid bounded scan dimensions')
    from blob_detector import BlobDetector
    detector = BlobDetector(threshold=120, min_area=2)
    process = subprocess.Popen(['ffmpeg','-hide_banner','-loglevel','fatal','-i',str(source),
                                '-frames:v',str(maximum),'-f','rawvideo','-pix_fmt','gray','pipe:1'],
                               stdout=subprocess.PIPE)
    rows = []
    try:
        while len(rows) < maximum:
            needed = width*height
            data = bytearray()
            while len(data) < needed:
                chunk = process.stdout.read(needed-len(data))
                if not chunk:
                    break
                data.extend(chunk)
            if not data:
                break
            if len(data) != needed:
                raise ValueError('Truncated raw frame or incorrect dimensions')
            image = np.frombuffer(data,dtype=np.uint8).reshape(height,width)
            blobs = detector.detect(image)
            rows.append({'frame':len(rows),'blobs':[{'x':b.x,'y':b.y,'area':b.area,'peak':b.brightness} for b in blobs]})
        code = process.wait(timeout=10)
        if code:
            raise RuntimeError(f'ffmpeg failed: {code}')
    finally:
        if process.poll() is None:
            process.terminate()
            process.wait(timeout=10)
        process.stdout.close()
    return {'frames':rows,'width':width,'height':height,'threshold':120,
            'identity_verified':False,'poses_produced':False}


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source',type=Path)
    parser.add_argument('--width',type=int,required=True)
    parser.add_argument('--height',type=int,required=True)
    parser.add_argument('--out',type=Path,required=True)
    args = parser.parse_args()
    result = scan(args.source,args.width,args.height)
    args.out.write_text(json.dumps(result))
    counts = [len(r['blobs']) for r in result['frames']]
    print(json.dumps({'frames':len(counts),'min_blobs':min(counts,default=0),'max_blobs':max(counts,default=0)}))
