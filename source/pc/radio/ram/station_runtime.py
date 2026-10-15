# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Host-side live state. Only checksum-validated IMU proves recent reception."""
import json
import time
from pathlib import Path
try:
    from ..pico_swift_probe import decode_bootstrap_record, decode_identity_response
    from ..swift_records import decode_records
    from ..swift_telemetry import decode_sensor_payload
except ImportError:
    from radio.pico_swift_probe import decode_bootstrap_record, decode_identity_response
    from radio.swift_records import decode_records
    from radio.swift_telemetry import decode_sensor_payload


class StationRuntime:
    def __init__(self, path, peers, registry=None):
        self.path = Path(path)
        self.registry = Path(registry) if registry else None
        self.reload_requested = False
        self.peers = [dict(p, slot=i, last_imu_unix=None, samples=0,
                           battery_percent=None, battery_source='not decoded')
                      for i, p in enumerate(peers)]

    def record(self, record):
        log = self.path.with_name('radio-live-records.jsonl')
        if log.exists() and log.stat().st_size > 8*1024*1024:
            log.replace(log.with_suffix('.previous.jsonl'))
        with log.open('a', encoding='utf-8') as output:
            output.write(json.dumps(record)+'\n')
        # `crc` is the RXCRC register value, not a boolean CRCSTATUS.
        if record.get('partial') or record.get('hardware_crc_valid') is not True:
            return
        try:
            dma = bytes.fromhex(record['dma_hex'])
        except (KeyError, ValueError, TypeError):
            return
        if len(dma) < 2 or len(dma) < dma[0]+2:
            return
        payload = dma[2:2+dma[0]]
        if self.registry and record.get('channel') in (1,37,77):
            identity=None
            try:
                identity=(decode_bootstrap_record(payload) if len(payload)==12
                          else decode_identity_response(payload))['device_id_hex']
            except ValueError:
                pass
            if identity and int(identity,16):
                known=json.loads(self.registry.read_text())
                if identity not in known and len(known)<3:
                    known.append(identity)
                    temporary=self.registry.with_suffix('.tmp')
                    temporary.write_text(json.dumps(known),encoding='utf-8')
                    temporary.replace(self.registry)
                    self.reload_requested=True
        index = record.get('profile_index')
        if index not in range(len(self.peers)) or record.get('channel') != 2:
            return
        try:
            commands = decode_records(payload)
        except ValueError:
            return
        peer = self.peers[index]
        if (peer['last_imu_unix'] is not None
                and time.time()-peer['last_imu_unix'] > .94):
            peer.pop('sample_counter',None)
        for command in commands:
            if command.command != 2:
                continue
            try:
                samples = decode_sensor_payload(bytes([command.tag,2,len(command.body)])
                                                +command.body)['samples']
            except ValueError:
                continue
            for sample in samples:
                counter = sample['sample_counter']
                previous = peer.get('sample_counter')
                delta = 1 if previous is None else (counter-previous)&255
                if delta == 0 or delta > 127:
                    continue
                peer['sample_counter'] = counter
                peer['sample_gaps'] = peer.get('sample_gaps',0)+max(0,delta-1)
                peer['last_imu_unix'] = time.time()
                peer['samples'] += 1

    def publish(self, slots, running=True):
        now = time.time()
        peers = []
        for p in self.peers:
            age = None if p['last_imu_unix'] is None else now-p['last_imu_unix']
            peers.append(dict(p, imu_age_seconds=age,
                              connected=running and age is not None and age < 2))
        state = dict(updated_unix=now, running=running, slots=slots,
                     trackers=peers, poses_valid=False)
        temporary = self.path.with_suffix('.tmp')
        temporary.write_text(json.dumps(state), encoding='utf-8')
        temporary.replace(self.path)
