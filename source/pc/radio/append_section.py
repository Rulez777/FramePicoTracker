# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
# append_section.py
section = """
## Breakthrough: Simultaneous 3-Profile Reception on Frame and Telemetry Layout Decoded, 2026-10-06

1. Confirmed Over-the-Air Reception on Steam Frame:
   - Reception for all three tracker profiles is confirmed operational on Steam Frame via the newly discovered slot-prefix tails:
     - Slot 0: 06 4D 00 (prefix 0x00)
     - Slot 1: 4C EC 80 (prefix 0x80)
     - Slot 2: 09 84 40 (prefix 0x40)
   - Reception occurs concurrently without interference from, or dropping of, Steam Frame stock tracking controllers.
   - Full 32-bit hardware ID recovery is not currently required or blocking, as the 24-bit tail matching hardware filter reliably demultiplexes all three tracker telemetry streams.

2. Correction Regarding Station Seed:
   - Station seed is confirmed strictly as the raw byte sequence: [0x74, 0x73, 0x54, 0x15].
   - It must not be treated as scalar 0x74735415 due to integer endianness (in little-endian integer representation, byte array [0x74, 0x73, 0x54, 0x15] represents 0x15547374).

3. Exact Telemetry Payload Structure (trackerDataParserV2 at 0x96E44 in libpvrtrackingservice.so):
   - Confirmed by machine disassembly and format string (VA 0x663E9):
     tracker imu id %d frame_cnt %d imu.ts %lu ts_diff=%lu latency %d ir_led status %d mag calib (%f %f %f %f) station_index %d, Q_flag %d, start_index %d, MCU_Q_0 %f, MCU_Q_1 %f, MCU_Q_2 %f, MCU_Q_3 %f
   - Header Flag Byte (payload[2]):
     - If (flag & 0x80) == 0: IMU_base = payload + 3
     - If (flag & 0x80) != 0 and (flag & 0x40) != 0: IMU_base = payload + 5
     - If (flag & 0x80) != 0 and (flag & 0x40) == 0: IMU_base = payload + 7
   - Offsets from IMU_base:
     - +0x00..+0x05: 3 x int16 Accelerometer (X, Y, Z)
     - +0x06..+0x0B: 3 x int16 Gyroscope (X, Y, Z)
     - +0x0C..+0x11: 3 x uint16 Magnetometer / Aux
     - +0x14..+0x17: Status and frame count fields
     - +0x18: uint8 IR LED status (active illumination state)
     - +0x19: uint8 Q_flag
     - +0x1A: uint8 start_index
     - +0x1B..+0x2A: 4 x float32 (single precision) onboard orientation quaternion:
       - +0x1B: MCU_Q_0
       - +0x1F: MCU_Q_1
       - +0x23: MCU_Q_2
       - +0x27: MCU_Q_3
"""
with open(r'K:\AI\pico\pico_tracker_steam_frame_research.md', 'a', encoding='utf-8') as f:
    f.write(section)
