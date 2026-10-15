# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Offline command body from Swift 0x9500 / 0x8D88; no radio transmission."""


def build_ir_enable_candidate(chip_id_le: bytes, mode: int = 1,
                              sequence: int = 0) -> bytes:
    """Build the researched 0x09 frame, without claiming delivery or identity.

    The role of body byte1 and outer sequence/ack state still needs a capture.
    This candidate is not a complete ESB station implementation.
    """
    if len(chip_id_le) != 4 or mode not in (1, 2) or not 0 <= sequence < 64:
        raise ValueError('Expected four ID bytes, mode1/2, six-bit sequence')
    return bytes((sequence, 0x09, 6, mode, 0)) + chip_id_le
