# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Bounded application command inspection after independently verified RF CRC.

No transmission and no tracker poses. Partial suffixes remain opaque. Sender
direction is unknown for a passive capture on the shared pipe address.
"""
from __future__ import annotations


def inspect_commands(payload: bytes) -> dict:
    commands = []
    offset = 0
    while len(payload)-offset >= 3:
        tag, opcode, size = payload[offset:offset+3]
        if opcode == 0 or opcode > 0x1e or size > len(payload)-offset-3:
            break
        body = payload[offset+3:offset+3+size]
        command = {'offset':offset, 'tag':tag, 'sequence':tag&0x3f,
                   'opcode':opcode, 'body_hex':body.hex()}
        if opcode == 8 and size == 6:
            command['table_update_delay'] = body[0]
            command['even_channels'] = [2*(i+1) for i in range(40)
                                       if body[1+i//8] & (1 << (i%8))]
            command['scope'] = 'Channel-mask handler8F48; not proof of current applied mask'
        elif opcode == 10 and size == 4:
            command['sync_value_le'] = int.from_bytes(body,'little')
            command['scope'] = 'Value consumed by8DF0→79B8; clock units not established here'
        elif opcode == 11 and size == 1:
            command['queue_ack_counter'] = body[0]
            command['scope'] = '9458→97F4 queue acknowledgement; not an IR phase label'
        commands.append(command)
        offset += 3+size
    return {'commands':commands, 'opaque_suffix_hex':payload[offset:].hex(),
            'sender_direction_verified':False, 'poses_produced':False}
