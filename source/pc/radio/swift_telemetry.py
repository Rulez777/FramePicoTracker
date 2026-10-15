# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Firmware-derived raw sensor blocks and SLPI V2 full fusion telemetry.

Decodes both:
1. Legacy/short sensor blocks (Swift 9458/9858/9778, AD08..ADD4, B3D8..B432).
2. Full V2 optical-inertial telemetry (SlpiDataParser::trackerDataParserV2, VA 0x96E44
   in libpvrtrackingservice.so) containing raw IMU axes, IR LED constellation status,
   temperature, and onboard computed orientation quaternion (MCU_Q_0..3).
"""
from __future__ import annotations
import struct
import math
from typing import Any


def decode_sensor_payload(payload: bytes) -> dict[str, Any]:
    """Swift9458/9858/9778, AD08..ADD4 and B3D8..B432.

    Observed payloads contain a three-byte command header then 6/18/24/36/42-byte
    blocks and additional uninterpreted bytes. Call only after CRC validation.
    Scale globals are runtime configurable; raw axes are deliberately retained.
    """
    if len(payload) < 3 or payload[1] != 2 or payload[2] not in (6, 18, 24, 36, 42):
        raise ValueError('Not the researched sensor command')
    size = payload[2]
    if len(payload) < 3 + size:
        raise ValueError('Truncated sensor block')
    body = payload[3:3 + size]
    result: dict[str, Any] = {
        'command_tag': payload[0],
        'sensor_bytes': size,
        'unparsed_suffix_hex': payload[3 + size:].hex(),
        'orientation_available': False,
        'position_available': False,
    }
    samples=[]
    count=2 if size in (36,42) else 1 if size in (18,24) else 0
    for index in range(count):
        offset=index*18
        axes=struct.unpack_from('<7h',body,offset)
        samples.append(dict(gyro_raw=list(axes[:3]),accel_raw=list(axes[3:6]),
            temperature_raw=axes[6],temperature_c=25.0+axes[6]/132.48,
            timestamp_raw24=int.from_bytes(body[offset+14:offset+17],'little'),
            sample_counter=body[offset+17]))
    result['samples']=samples
    if samples: result.update(samples[0])  # Existing consumers retain first-sample fields.
    if size in (6,24,42):
        mag=struct.unpack_from('<3H',body,count*18)
        result.update(magnetometer_raw_unsigned=list(mag),
            magnetometer_raw_centered=[value-32768 for value in mag])
    return result


def decode_swift_telemetry_v2(payload: bytes) -> dict[str, Any]:
    """Decode SlpiDataParser::trackerDataParserV2 (VA 0x96E44 in libpvrtrackingservice.so).

    Extracts:
    - Tracker identity: tracker_id, slot_index (0..2)
    - Dynamic IMU base offset (payload+3, payload+5, or payload+7)
    - High-rate IMU: Gyroscope XYZ (int16), Accelerometer XYZ (int16), Magnetometer XYZ (uint16)
    - Temperature (int16, T = 25.0 + raw / 132.48 deg C)
    - Timing & State: frame_cnt, status_flags, ir_led_status
    - Onboard Fusion: Q_flag, station_index, start_index, MCU_Q_0..3 (float32 quaternion)
    """
    if len(payload) < 3:
        raise ValueError('Payload too short for V2 telemetry header')

    flag = payload[2]
    tracker_id = flag & 0x3F
    slot_index = max(0, tracker_id - 2) if tracker_id >= 2 else tracker_id

    # Dynamic IMU_base based on flag bits (VA 0x96E8C..0x96EA4)
    # (flag & 0x80) == 0 -> payload + 3
    # (flag & 0x80) != 0 and (flag & 0x40) != 0 -> payload + 5 (2-byte latency)
    # (flag & 0x80) != 0 and (flag & 0x40) == 0 -> payload + 7 (4-byte timestamp)
    has_prefix = bool(flag & 0x80)
    has_short_prefix = bool(flag & 0x40)

    if not has_prefix:
        imu_base = 3
        latency_ticks = None
        timestamp_ticks = None
    elif has_short_prefix:
        imu_base = 5
        if len(payload) < 5:
            raise ValueError('Truncated V2 latency prefix')
        latency_ticks = struct.unpack_from('<H', payload, 3)[0]
        timestamp_ticks = None
    else:
        imu_base = 7
        if len(payload) < 7:
            raise ValueError('Truncated V2 timestamp prefix')
        timestamp_ticks = struct.unpack_from('<I', payload, 3)[0]
        latency_ticks = None

    min_required_len = imu_base + 43
    if len(payload) < min_required_len:
        raise ValueError(
            f'Truncated V2 telemetry payload (got {len(payload)} bytes, need {min_required_len})'
        )

    # 1. IMU sensor axes: Gyro (3x int16), Accel (3x int16), Mag (3x uint16)
    gyro_raw = list(struct.unpack_from('<3h', payload, imu_base))
    accel_raw = list(struct.unpack_from('<3h', payload, imu_base + 6))
    mag_raw = list(struct.unpack_from('<3H', payload, imu_base + 12))

    # 2. Temperature (int16, T = 25.0 + raw / 132.48)
    temp_raw = struct.unpack_from('<h', payload, imu_base + 18)[0]
    temp_c = 25.0 + temp_raw / 132.48

    # 3. Counters and status
    frame_cnt = payload[imu_base + 20]
    status_flags = payload[imu_base + 21]
    ir_led_status = struct.unpack_from('<H', payload, imu_base + 22)[0]
    station_index = payload[imu_base + 24]
    q_flag = payload[imu_base + 25]
    start_index = payload[imu_base + 26]

    # 4. Onboard orientation quaternion (4x float32)
    # Log string: MCU_Q_0 %f, MCU_Q_1 %f, MCU_Q_2 %f, MCU_Q_3 %f
    # Offsets: +0x1B, +0x1F, +0x23, +0x27 from IMU_base
    mcu_q = list(struct.unpack_from('<4f', payload, imu_base + 27))
    quaternion_valid = bool(q_flag != 0 and all(math.isfinite(v) for v in mcu_q)
                            and 0.9 <= sum(v*v for v in mcu_q) <= 1.1)

    return {
        'version': 2,
        'transport': 'slpi_v2',
        'layout_status': 'Research layout; not verified as an over-air Swift payload',
        'command_tag': payload[0],
        'sub_command': payload[1],
        'tracker_id': tracker_id,
        'slot_index': slot_index,
        'has_prefix': has_prefix,
        'latency_ticks': latency_ticks,
        'timestamp_ticks': timestamp_ticks,
        'gyro_raw': gyro_raw,
        'accel_raw': accel_raw,
        'magnetometer_raw_unsigned': mag_raw,
        'magnetometer_raw_centered': [val - 32768 for val in mag_raw],
        'temperature_raw': temp_raw,
        'temperature_c': temp_c,
        'frame_cnt': frame_cnt,
        'status_flags': status_flags,
        'ir_led_status': ir_led_status,
        'station_index': station_index,
        'q_flag': q_flag,
        'start_index': start_index,
        'mcu_quaternion': mcu_q if quaternion_valid else None,
        'mcu_quaternion_raw': mcu_q,
        'orientation_available': quaternion_valid,
        'position_available': False,
    }


def unpack_swift_telemetry(payload: bytes, *, transport: str = 'radio') -> dict[str, Any]:
    """Require an explicit source; packet length cannot identify SLPI framing.

    The default is the independently captured radio sensor layout. SLPI V2
    must be selected by a caller that obtained an actual SLPI buffer.
    """
    if transport == 'radio':
        return decode_sensor_payload(payload)
    if transport == 'slpi_v2':
        return decode_swift_telemetry_v2(payload)
    raise ValueError('Unknown telemetry transport')
