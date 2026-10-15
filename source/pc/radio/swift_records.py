# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Strict working-record decoder. Radio CRC is validated by the caller."""
from dataclasses import dataclass


@dataclass(frozen=True)
class SwiftRecord:
    tag: int
    command: int
    body: bytes

    @property
    def sequence(self):
        return self.tag & 63


def decode_records(payload: bytes) -> list[SwiftRecord]:
    if not payload or sum(payload[:-1]) & 255 != payload[-1]:
        raise ValueError('Invalid working checksum')
    result = []
    end = len(payload)-1
    offset = 0
    while offset < end:
        if end-offset < 3:
            raise ValueError('Truncated working record header')
        tag, command, size = payload[offset:offset+3]
        offset += 3
        if command == 0:
            if size != 0 or any(payload[offset:end]):
                raise ValueError('Invalid terminator')
            break
        if not 1 <= command <= 29 or offset+size > end:
            raise ValueError('Invalid working command or length')
        result.append(SwiftRecord(tag, command, payload[offset:offset+size]))
        offset += size
    return result
