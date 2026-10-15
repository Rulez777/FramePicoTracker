# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Command builders from pico_tracker_packets_en.md, no automatic RF TX."""
import struct


def record(seq, command, body):
    if not 0 <= seq < 64 or not 0 < command < 30 or len(body)>255:
        raise ValueError('Invalid Swift record')
    return bytes([seq,command,len(body)])+body


def light_sync(seq, online_count, reason=0):
    if not 1 <= online_count <= 3 or not 0 <= reason <= 255:
        raise ValueError('Light divisor must be1..3')
    return record(seq,14,bytes([1,online_count,reason]))


def phase_sync(seq, ticks):
    if not 0 <= ticks < 533333:
        raise ValueError('Expected30Hz frame phase in16MHz ticks')
    return record(seq,10,struct.pack('<i',ticks))


def pack(records):
    body=b''.join(records)
    return body+bytes([sum(body)&255])
