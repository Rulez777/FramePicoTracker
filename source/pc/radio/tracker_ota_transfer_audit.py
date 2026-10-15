# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Offline audit of a fork and saved service measurements, never an OTA job."""
import argparse
import hashlib
import json
from pathlib import Path

from .tracker_dual_mode_patch import reverse
from .tracker_grid_patch import STOCK_SHA256
from .tracker_image_audit import crc8_maxim
from .tracker_ota import OtaSession
from .tracker_ota_readiness import BLOCKERS


def audit(image_path,manifest_path,measurement_path):
    image=Path(image_path).read_bytes()
    manifest=json.loads(Path(manifest_path).read_text(encoding='utf-8'))
    original=reverse(image,manifest)
    if hashlib.sha256(original).hexdigest()!=STOCK_SHA256:
        raise ValueError('Fork does not reverse to the frozen FW189 image')
    measurement=json.loads(Path(measurement_path).read_text(encoding='utf-8'))
    if not (measurement['stable_seconds']>0 and measurement['stable_tx']>0
            and 0<=measurement['stable_retained']<=measurement['stable_tx']):
        raise ValueError('Invalid saved service measurement')
    image_hash=hashlib.sha256(image).hexdigest()
    sender=OtaSession(image,image_hash)
    # Each application operation is followed by an ACK-only drain. No boot.
    exchanges=2*sender.blocks+5
    poll_rate=measurement['stable_tx']/measurement['stable_seconds']
    return dict(schema=1,scope='Offline fork and historical service evidence; not an executable OTA job',
                image_bytes=len(image),image_sha256=image_hash,crc8=hex(crc8_maxim(image)),
                reverse_to_stock_verified=True,blocks=sender.blocks,
                data_blocks=len(image)//24,last_block_bytes=len(image)%24,
                minimum_successful_poll_exchanges=exchanges,
                measured_service_poll_rate_hz=poll_rate,
                optimistic_seconds_at_measured_poll_rate=exchanges/poll_rate,
                estimate_limits='Excludes erase/program latency, queue delay, retries and startup; not a measured OTA rate',
                final_integrity='Stock receiver uses CRC8; exact SHA256 staging comparison is offline only',
                historical_measurement_sha256=hashlib.sha256(Path(measurement_path).read_bytes()).hexdigest(),
                hardware_ota_approved=False,blockers=list(BLOCKERS)+[
                    'Expanded dual-mode image is not supported by the current hardware job whitelist',
                    'Exclusive OTA adapter still selects cycle7/N9/6000us rather than measured service27 cadence',
                    'Measured ordinary-poll response rate does not establish firmware block application ACK throughput'])


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--image',type=Path,required=True)
    parser.add_argument('--manifest',type=Path,required=True)
    parser.add_argument('--measurement',type=Path,required=True)
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    value=audit(args.image,args.manifest,args.measurement)
    with args.out.open('x',encoding='utf-8') as stream:
        json.dump(value,stream,indent=2);stream.write('\n')
    print(json.dumps(value,indent=2))
