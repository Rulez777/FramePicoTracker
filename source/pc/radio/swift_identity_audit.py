# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Audit claimed Swift identities against the current, conditional RF model.

Never sends commands. CRC disagreement is conditional on seed/order/framing,
and does not replace reading a primary identity source.
"""
from .pico_swift_probe import crc16_bits


def infer_identity_from_base(base_register: int, seed_le: bytes,
                             observed_low16: bytes) -> dict:
    """Invert the researched byte-sum/RBIT model; never authorize RF commands."""
    if (type(base_register) is not int or not 0 <= base_register <= 0xffffffff
            or len(seed_le) != 4 or len(observed_low16) != 2):
        raise ValueError('Expected uint32 BASE, four seed bytes and two ID bytes')
    summed = int(f'{base_register:032b}'[::-1], 2).to_bytes(4, 'little')
    candidate = bytes((value - seed) & 255 for value, seed in zip(summed, seed_le))
    return {'base_register': f'{base_register:08x}', 'seed_le': seed_le.hex(),
            'candidate_chip_id_le': candidate.hex(),
            'candidate_chip_id_u32': f'{int.from_bytes(candidate, "little"):08x}',
            'low16_matches': candidate[:2] == observed_low16,
            'identity_verified': False,
            'transmission_authorized_by_this_audit': False,
            'scope': 'Conditional inverse address model, not a direct FICR read'}


def audit_identity(chip_id_le: bytes, seed_le: bytes, slot: int,
                   observed_low16: bytes, observed_tail: bytes, crc_state: int) -> dict:
    if len(chip_id_le) != 4 or len(seed_le) != 4 or len(observed_low16) != 2 or len(observed_tail) != 3 or slot not in (0,1,2):
        raise ValueError('Invalid identity audit inputs')
    reversed_bytes = bytes(int(f'{(s+t)&255:08b}'[::-1],2) for s,t in zip(seed_le,chip_id_le))
    prefix = (0,0x80,0x40)[slot]
    address = reversed_bytes[::-1] + bytes([prefix])
    predicted_state = crc16_bits(''.join(f'{b:08b}' for b in address))
    return {'slot':slot,'claimed_chip_id_le':chip_id_le.hex(),
            'claimed_chip_id_u32':hex(int.from_bytes(chip_id_le,'little')),
            'low16_matches':chip_id_le[:2] == observed_low16,
            'conditional_address':address.hex(),
            'tail_matches':address[-3:] == observed_tail,
            'conditional_crc_state':hex(predicted_state), 'observed_crc_state':hex(crc_state),
            'crc_model_matches':predicted_state == crc_state,
            'identity_verified':False,'transmission_authorized_by_this_audit':False}
