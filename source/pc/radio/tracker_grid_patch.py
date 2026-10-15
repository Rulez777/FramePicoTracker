# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Reproducible, offline-only FW189 radio cadence patch. Outputs raw OTA images.

No vendor container/signature is forged. Stock bytes are kept separately.
The patch is experimental: matching the nominal grid does not prove RF phase
alignment, reply duration, oscillator accuracy, or safe bootloader activation.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

BASE = 0x1000
STOCK_SHA256 = '477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89'
IMAGE_BYTES = 96532


def movw(rd: int, value: int) -> bytes:
    """Thumb-2 MOVW T3, retaining register and instruction width."""
    if not 0 <= rd < 15 or not 0 <= value <= 0xffff:
        raise ValueError('Invalid MOVW operand')
    return struct.pack('<HH', 0xf240 | ((value >> 12) & 15) | ((value >> 1) & 0x400),
                       ((value & 0x700) << 4) | (rd << 8) | (value & 255))


def edits() -> list[tuple[int, bytes, bytes, str]]:
    result = []
    for va, rd, old, new in ((0x6214, 4, 18910, 12000), (0x6218, 0, 4727, 3000),
                             (0x630a, 0, 18910, 12000), (0x630e, 1, 4727, 3000),
                             (0x6cde, 0, 3751, 2024), (0x6ce4, 0, 3355, 1628)):
        result.append((va, movw(rd, old), movw(rd, new), f'MOVW r{rd}: {old} -> {new} us'))
    # Keep the original RX aperture width. Subtract 1727.5us from reopening
    # offsets, rather than scaling RF airtime, interrupt latency or reply guards.
    for va, old, new in ((0x6d40, 4727.5, 3000.0), (0x6d48, 3538.0, 1810.5),
                         (0x6d50, 3934.5, 2207.0), (0x6f80, 4727.5, 3000.0)):
        result.append((va, struct.pack('<d', old), struct.pack('<d', new),
                       f'RX scheduler double: {old} -> {new} us'))
    # 0x6f6c/0x13754 SUBTRACTS 25us from the next RX deadline. Retain this
    # original service-latency compensation; it is not an added period drift.
    return sorted(result)


def patch(stock: bytes) -> tuple[bytes, dict]:
    if len(stock) != IMAGE_BYTES or hashlib.sha256(stock).hexdigest() != STOCK_SHA256:
        raise ValueError('Only the independently frozen FW189 raw image is supported')
    result = bytearray(stock)
    manifest_edits = []
    for va, before, after, reason in edits():
        offset = va - BASE
        if result[offset:offset + len(before)] != before:
            raise ValueError(f'Unexpected original bytes at {va:#x}')
        result[offset:offset + len(before)] = after
        manifest_edits.append(dict(va=hex(va), offset=offset, before=before.hex(), after=after.hex(), reason=reason))
    patched = bytes(result)
    return patched, dict(schema=1, name='fw189-valve-grid-3000-experimental', image_base=BASE,
                         image_bytes=len(stock), source_sha256=STOCK_SHA256,
                         patched_sha256=hashlib.sha256(patched).hexdigest(), edits=manifest_edits,
                         changed_bytes=sum(a != b for a, b in zip(stock, patched)),
                         hardware_verified=False, bootloader_verified=False,
                         version_field_unchanged=True,
                         notes=['Integer hunt offsets round upward by 0.5us.',
                                'Native -25us RX service compensation retained.',
                                'IMU, LED, camera timer, commands, pairing and boot vectors unchanged.',
                                'Stock availability alone does not prove recovery after a failed boot.'])


def reverse(patched: bytes, manifest: dict) -> bytes:
    if hashlib.sha256(patched).hexdigest() != manifest['patched_sha256']:
        raise ValueError('Patched image hash mismatch')
    result = bytearray(patched)
    for entry in manifest['edits']:
        offset = entry['offset']; after = bytes.fromhex(entry['after']); before = bytes.fromhex(entry['before'])
        if result[offset:offset + len(after)] != after:
            raise ValueError('Patched edit mismatch')
        result[offset:offset + len(after)] = before
    if hashlib.sha256(result).hexdigest() != STOCK_SHA256:
        raise ValueError('Reversed image is not the frozen stock')
    return bytes(result)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('stock', type=Path)
    parser.add_argument('--out-dir', type=Path, required=True)
    args = parser.parse_args()
    stock = args.stock.read_bytes(); patched, manifest = patch(stock)
    if reverse(patched, manifest) != stock:
        raise ValueError('Patch failed exact reverse verification')
    # A fresh directory prevents overwriting a user's prior candidate or source.
    args.out_dir.mkdir(parents=True, exist_ok=False)
    (args.out_dir / 'fw189-stock.bin').write_bytes(stock)
    (args.out_dir / 'fw189-valve-grid-3000.bin').write_bytes(patched)
    (args.out_dir / 'patch-manifest.json').write_bytes((json.dumps(manifest, indent=2) + '\n').encode())
    print(json.dumps(manifest, indent=2))
