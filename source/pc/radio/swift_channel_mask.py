# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Swift VA7508 mask expansion; not a hopping schedule or live channel discovery."""


def channels_from_mask(mask: bytes) -> tuple[int, ...]:
    if len(mask) != 5:
        raise ValueError('Swift channel mask must contain exactly five bytes')
    return tuple(2*(8*byte_index+bit+1)
                 for byte_index, value in enumerate(mask)
                 for bit in range(8) if value & (1 << bit))
