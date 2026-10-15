# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Strict product event 0x82 layout observed in CRC-filtered FW189 replies.

Identity is the RF chipID of the responding personal address, supplied by the
caller. Unexplained suffix/status bytes never become a fabricated battery.
"""
from dataclasses import dataclass
import re
import struct


@dataclass(frozen=True)
class SwiftProductInfo:
    firmware: int
    serial: str
    sensor_class_bits: int
    raw: bytes
    battery_percent: None = None


def decode_product_info(body: bytes) -> SwiftProductInfo:
    if len(body)!=35 or body[0]!=0x82:
        raise ValueError('Expected complete 35-byte product event0x82')
    firmware=struct.unpack_from('<H',body,1)[0]
    try:serial=body[8:25].decode('ascii')
    except UnicodeDecodeError:raise ValueError('Non-ASCII product serial') from None
    if not re.fullmatch(r'PC[A-Z0-9]{15}',serial) or not 1<=firmware<=9999:
        raise ValueError('Unrecognized Swift product format')
    return SwiftProductInfo(firmware,serial,body[25]>>6,body)


def validated_sensor_scale(info: SwiftProductInfo):
    """FW189 class0 matches the documented WHO_AM_I0x44 branch.

    Other product branches need their own verification. Sensor part name, body
    axes and bias are not inferred from this selection.
    """
    if info.firmware!=189 or info.sensor_class_bits!=0:
        raise ValueError('Sensor scale not validated for this firmware/product branch')
    return {'accel_mps2_per_count':0.009580078534781933,
            'gyro_radps_per_count':0.002130528911948204,
            'basis':'FW189 product class0 and external WHO_AM_I0x44 profile',
            'body_axes_validated':False}
