# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Read-only extraction of a bounded tail sequence from a completed OCC file."""
import argparse
from collections import deque
from pathlib import Path
import struct
from frame_occ_extract import tlv_fields, annex_b_nals


def extract_tail(source: Path, out: Path, records: int = 240):
    if not 1 <= records <= 300:
        raise ValueError('Tail must contain 1..300 records per kind')
    tails = {32: deque(maxlen=records), 38: deque(maxlen=records)}
    with source.open('rb') as stream:
        while True:
            header = stream.read(16)
            if not header:
                break
            if len(header) != 16 or header[:8] != b'occipitl':
                raise ValueError('Invalid or unfinished OCC record')
            kind, size = struct.unpack_from('<II', header, 8)
            if size > 64 * 1024 * 1024:
                raise ValueError('Unexpected record size')
            if kind in tails:
                data = stream.read(size)
                if len(data) != size:
                    raise ValueError('Truncated recording')
                tails[kind].append(data)
            else:
                stream.seek(size, 1)
    out.mkdir(parents=True, exist_ok=True)
    paths = []
    for kind, samples in tails.items():
        streams = {}
        for sample in samples:
            camera = None
            occurrence = {}
            key = None
            for field, value in tlv_fields(sample):
                if field == 17 and len(value) == 1:
                    camera = value[0]
                    ordinal = occurrence.get(camera, 0)
                    occurrence[camera] = ordinal + 1
                    key = (camera, ordinal)
                elif field == 14 and key is not None:
                    streams.setdefault(key, bytearray()).extend(b'\x00\x00\x00\x01' + value)
                elif field == 13 and key is not None:
                    streams.setdefault(key, bytearray()).extend(annex_b_nals(value))
        for (camera, ordinal), data in streams.items():
            path = out / f'occ-tail-{kind}-selector-{camera}-stream-{ordinal}.h264'
            path.write_bytes(data)
            paths.append(path)
    return paths


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--records', type=int, default=240)
    args = parser.parse_args()
    for path in extract_tail(args.source, args.out, args.records):
        print(path)
