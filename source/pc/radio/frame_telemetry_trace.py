# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Hardware-CRC capture diagnostics; sensor vectors are not tracker world poses."""
from __future__ import annotations
import csv
import json
import math
from pathlib import Path
from .swift_capture_decode import decode_hardware_crc_capture


def write_trace(report: dict, prefix: Path) -> dict:
    decoded = decode_hardware_crc_capture(report)
    records = {r['sequence']: r for r in report.get('raw_records', [])}
    samples = []
    for item in decoded['accepted']:
        sensors = item['sensors']
        if 'gyro_raw' not in sensors or 'accel_raw' not in sensors:
            continue
        record = records[item['sequence']]
        for sensor_sample in sensors.get('samples',[sensors]):
            acceleration=sensor_sample['accel_raw']
            magnitude=math.sqrt(sum(v*v for v in acceleration))
            samples.append({'sequence':item['sequence'],
                'receiver_profile_index':item.get('profile_index'),
                'host_read_monotonic_ns':record.get('host_read_monotonic_ns'),
                'gyro_raw':sensor_sample['gyro_raw'],'accel_raw':acceleration,
                'accel_direction_sensor':[v/magnitude for v in acceleration] if magnitude else None,
                'sample_counter':sensor_sample.get('sample_counter')})
    gaps = [(b['host_read_monotonic_ns']-a['host_read_monotonic_ns'])/1e9
            for a,b in zip(samples,samples[1:])
            if a['host_read_monotonic_ns'] is not None and b['host_read_monotonic_ns'] is not None]
    counts={}
    for sample in samples:
        key=str(sample['receiver_profile_index'])
        counts[key]=counts.get(key,0)+1
    result = {'samples_per_receiver_profile':counts,'samples':samples,'sample_count':len(samples),
              'largest_host_read_gap_seconds':max(gaps,default=None),
              'world_pose_valid':False,'orientation_valid':False,
              'scope':'Raw sensor axes; host read time, not RF arrival or camera alignment'}
    prefix.with_suffix('.telemetry.json').write_text(json.dumps(result,indent=2))
    with prefix.with_suffix('.telemetry.csv').open('w',newline='') as stream:
        writer=csv.writer(stream)
        writer.writerow(['sequence','host_read_monotonic_ns','gx','gy','gz','ax','ay','az','counter','receiver_profile'])
        for sample in samples:
            writer.writerow([sample['sequence'],sample['host_read_monotonic_ns'],
                             *sample['gyro_raw'],*sample['accel_raw'],sample['sample_counter'],sample['receiver_profile_index']])
    return result
