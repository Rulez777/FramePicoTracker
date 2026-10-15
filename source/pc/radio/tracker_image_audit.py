# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Offline FW189 container audit; never connects to hardware or changes images."""
import argparse
import hashlib
import json
import struct
from pathlib import Path


def crc8_maxim(data: bytes) -> int:
    crc = 0
    for value in data:
        crc ^= value
        for _ in range(8):
            crc = (crc >> 1) ^ (0x8C if crc & 1 else 0)
    return crc


def audit(container: bytes, reference: bytes) -> dict:
    header_size, image_size, base = 0x141, 0x17914, 0x1000
    expected_size = header_size + 2 * image_size
    if len(container) != expected_size or len(reference) != image_size:
        raise ValueError('Unexpected FW189 image/container length')
    first = container[header_size:header_size + image_size]
    second = container[header_size + image_size:]
    if first != second or first != reference:
        raise ValueError('Container copies and extracted reference differ')
    sp, reset = struct.unpack_from('<II', first)
    if not (0x20000000 <= sp <= 0x20006000 and sp % 8 == 0):
        raise ValueError('Unexpected initial stack pointer')
    if not (reset & 1 and base <= (reset & ~1) < base + image_size):
        raise ValueError('Invalid Thumb reset vector')
    table = first[0x16200 - base:0x16300 - base]
    generated = bytes(crc8_maxim(bytes([i])) for i in range(256))
    return {
        'container_sha256': hashlib.sha256(container).hexdigest(),
        'image_sha256': hashlib.sha256(first).hexdigest(),
        'container_bytes': len(container), 'header_bytes': header_size,
        'image_bytes': image_size, 'identical_copies': True,
        'initial_sp': hex(sp), 'reset_vector': hex(reset),
        'crc_table_va': '0x16200', 'crc8_maxim_table_matches': table == generated,
        'crc8_maxim_check_123456789': hex(crc8_maxim(b'123456789')),
        'chunks_of_24': (image_size + 23) // 24,
        'last_chunk_bytes': image_size % 24,
        'bootloader_verified': False, 'ota_ready': False,
    }


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('container', type=Path)
    parser.add_argument('reference', type=Path)
    parser.add_argument('--out', type=Path)
    args = parser.parse_args()
    result = json.dumps(audit(args.container.read_bytes(), args.reference.read_bytes()), indent=2) + '\n'
    if args.out:
        args.out.write_bytes(result.encode('utf-8'))
    print(result, end='')
