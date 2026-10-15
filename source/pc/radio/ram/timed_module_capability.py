# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Exact-artifact admission for the Phase210 SRAM timing implementation."""
import hashlib
import struct
from pathlib import Path

from .peer_engine import MailboxWriter

CODE_SHA = '63d319d4b424bc5a291558e41d9a740962b20eeb33dba77ba288d0c1d0480575'
ELF_SHA = 'd8328717a5f6b757fd002ad6622e4fa5f07c45a1de935f6631267792deb286a3'
ORIGIN, LIMIT = 0x2001151c, 2045


def admit_module(path):
    """Validate ELF and every allocated section; a caller assertion is insufficient."""
    blob = Path(path).read_bytes()
    if hashlib.sha256(blob).hexdigest() != ELF_SHA:
        raise ValueError('Unadmitted timed-module ELF')
    if blob[:7] != b'\x7fELF\x01\x01\x01':
        raise ValueError('Expected ELF32 little endian')
    if struct.unpack_from('<I', blob, 24)[0] != ORIGIN | 1:
        raise ValueError('Unexpected timed-module entry')
    offset = struct.unpack_from('<I', blob, 32)[0]
    stride, count = struct.unpack_from('<HH', blob, 46)
    code = bytearray()
    for index in range(count):
        section = struct.unpack_from('<10I', blob, offset + stride * index)
        kind, flags, address, position, size = section[1:6]
        if not flags & 2:
            continue
        if kind != 1 or address != ORIGIN or size != 2043 or code:
            raise ValueError('Unexpected allocated timed-module section')
        if address + size > ORIGIN + LIMIT:
            raise ValueError('Timed module exceeds reserved SRAM')
        code.extend(blob[position:position + size])
    if len(code) != 2043 or hashlib.sha256(code).hexdigest() != CODE_SHA:
        raise ValueError('Unadmitted timed-module code')
    return bytes(code)


class TimedMailboxWriter(MailboxWriter):
    """Single-peer phase_reference only; reject unsupported flags before SWD writes."""
    def __init__(self, debug, module_path, base=0x20010c18):
        admit_module(module_path)
        super().__init__(debug, base)

    def publish(self, slot, generation, s1, payload, *, phase_reference=None,
                cycle_window=None):
        if slot != 0:
            raise ValueError('Phase210 supports target peer0 only')
        if cycle_window is not None:
            raise ValueError('Phase210 does not implement cycle_window')
        return super().publish(slot, generation, s1, payload,
                               phase_reference=phase_reference)


def bind_runtime(runtime, module_path):
    """Bind before any mailbox publication; never change an active writer."""
    if runtime.writer.sent:
        raise ValueError('Cannot replace an already-published mailbox writer')
    optics = runtime.optics
    if optics is not None and getattr(optics, 'config', {}).get('timed_windows', True):
        raise ValueError('Phase210 requires timed_windows=false')
    writer = TimedMailboxWriter(runtime.debug, module_path)
    loaded = runtime.debug.read(ORIGIN, 2044)
    if len(loaded) != 2044 or hashlib.sha256(loaded[:2043]).hexdigest() != CODE_SHA:
        raise ValueError('Loaded SRAM code does not match timing capability')
    runtime.writer = writer
    runtime.timed_module_code_sha256 = CODE_SHA
