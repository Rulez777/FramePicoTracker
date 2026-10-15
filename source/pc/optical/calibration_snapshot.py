# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Preserve the active recording calibration before its owned dataset is closed."""
import hashlib
import json
from pathlib import Path


def preserve_calibration(dataset, destination):
    dataset, destination = Path(dataset).resolve(), Path(destination).resolve()
    if destination == dataset or dataset in destination.parents:
        raise ValueError('Snapshot must survive dataset cleanup')
    files = [dataset / 'sensorRigCalibration.json']
    devices = list(dataset.glob('*_device_config.json'))
    if len(devices) != 1:
        raise ValueError('Active device configuration missing or ambiguous')
    files.extend(devices)
    blobs = {}
    for source in files:
        if source.is_symlink() or source.resolve().parent != dataset:
            raise ValueError('Unexpected calibration source')
        if not source.is_file() or source.stat().st_size > 8 * 1024**2:
            raise ValueError('Calibration file missing or too large')
        body = source.read_bytes()
        json.loads(body)
        blobs[source.name] = body
    destination.mkdir(parents=True, exist_ok=False)
    manifest = dict(source_dataset=str(dataset), files={})
    for name, body in blobs.items():
        with (destination / name).open('xb') as stream:
            stream.write(body)
        manifest['files'][name] = dict(bytes=len(body), sha256=hashlib.sha256(body).hexdigest())
    (destination / 'manifest.json').write_text(json.dumps(manifest, indent=2))
    return manifest
