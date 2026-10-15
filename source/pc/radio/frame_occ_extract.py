# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Extract bounded first camera samples from an observed Valve OCC dataset.

Container and TLV interpretation are empirical, not an official ABI. Input must
be a completed recording. This tool never opens a hardware camera or service.
"""
from __future__ import annotations
import argparse
import struct
from pathlib import Path


def tlv_fields(data: bytes):
    offset = 0
    while offset < len(data):
        if offset + 8 > len(data):
            raise ValueError('Incomplete TLV header')
        key, size = struct.unpack_from('<II', data, offset)
        offset += 8
        if size > len(data) - offset:
            raise ValueError('Incomplete TLV value')
        yield key, data[offset:offset + size]
        offset += size


def annex_b_nals(value: bytes) -> bytes:
    output = bytearray()
    offset = 0
    while offset < len(value):
        if offset + 4 > len(value):
            raise ValueError('Incomplete NAL length')
        size = int.from_bytes(value[offset:offset + 4], 'big')
        offset += 4
        if not size or size > len(value) - offset:
            raise ValueError('Invalid NAL size')
        output.extend(b'\x00\x00\x00\x01' + value[offset:offset + size])
        offset += size
    return bytes(output)


def extract(source: Path, out: Path) -> list[str]:
    outputs = []
    found = set()
    out.mkdir(parents=True, exist_ok=True)
    with source.open('rb') as stream:
        while len(found) < 2:
            header = stream.read(16)
            if not header:
                break
            if len(header) != 16 or header[:8] != b'occipitl':
                raise ValueError('Invalid or unfinished OCC record')
            kind, size = struct.unpack_from('<II', header, 8)
            if size > 64 * 1024 * 1024:
                raise ValueError('Unexpected record size')
            if kind not in (32, 38) or kind in found:
                stream.seek(size, 1)
                continue
            data = stream.read(size)
            if len(data) != size:
                raise ValueError('Recording is incomplete or still growing')
            camera = None
            samples = {}
            for key, value in tlv_fields(data):
                if key == 17 and len(value) == 1:
                    camera = value[0]
                elif key == 14 and camera is not None:
                    samples.setdefault(camera, bytearray()).extend(b'\x00\x00\x00\x01' + value)
                elif key == 13 and camera is not None:
                    samples.setdefault(camera, bytearray()).extend(annex_b_nals(value))
            for camera, sample in samples.items():
                path = out / f'occ-type-{kind}-camera-{camera}.h264'
                path.write_bytes(sample)
                outputs.append(str(path))
            found.add(kind)
    return outputs


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    for path in extract(args.source, args.out):
        print(path)
