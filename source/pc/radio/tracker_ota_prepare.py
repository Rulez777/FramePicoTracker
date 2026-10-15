# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Prepare a fresh local/Frame OTA job directory; never starts SSH, SWD or RF.

Use a private copy of the current station config and the existing patch bundle.
The target chip ID/slot and current cadence must be supplied explicitly.
"""
import argparse
import hashlib
import json
from pathlib import Path
from .tracker_grid_patch import patch, reverse, STOCK_SHA256


def prepare(bundle, config_path, out_dir, *, slot, identity, action, source_grid_us, controllers_off=False):
    bundle = Path(bundle);out_dir = Path(out_dir)
    if slot not in range(3) or len(identity) != 8 or any(c not in '0123456789abcdef' for c in identity) or identity == '00000000':
        raise ValueError('Explicit real target identity/slot required')
    if action not in ('patch', 'stock') or source_grid_us not in (4727.5, 3000):
        raise ValueError('Explicit OTA action and source cadence required')
    stock = (bundle/'fw189-stock.bin').read_bytes()
    expected, manifest = patch(stock)
    if (bundle/'fw189-valve-grid-3000.bin').read_bytes() != expected:
        raise ValueError('Candidate bundle differs from the reproducible patch')
    if json.loads((bundle/'patch-manifest.json').read_text()) != manifest:
        raise ValueError('Candidate bundle manifest differs from the reviewed patch')
    if reverse(expected, manifest) != stock:raise ValueError('Stock reverse validation failed')
    config = json.loads(Path(config_path).read_text())
    if config.get('version') != 1 or len(bytes.fromhex(config['seed_le'])) != 4:
        raise ValueError('Use the actual private station config/seed')
    # This is a separate one-shot config, never the persistent station config.
    config.update(optics_enabled=False, ble_phase_lock=True, interval_us=6000,
                  steady_interval_us=6000, mpsl_priority=0, slot_cycle=7,
                  slot_end_margin_us=200, tracker_poll_decimation=True,
                  tracker_poll_decimation_n=9, tracker_poll_decimation_by_id={}, staggered_enroll=False)
    config['coexistence'] = dict(config.get('coexistence', {}), freeze_limit_ms=150, min_controllers=2)
    if controllers_off:
        # Valve controllers switched off for the transfer; any reconnect stops RF.
        config['ble_phase_lock'] = False
        config['coexistence'] = dict(config['coexistence'], controllers_off=True, min_controllers=0, freeze_limit_ms=None)
    image = expected if action == 'patch' else stock
    job = dict(schema=1, action=action, source_grid_us=source_grid_us, slot=slot, chip_id_le=identity,
               image='image.bin', sha256=hashlib.sha256(image).hexdigest(),
               manifest='patch-manifest.json', result='ota-result.json')
    # Never overwrite/reuse a job directory, even after an interrupted transfer.
    out_dir.mkdir(parents=True, exist_ok=False)
    (out_dir/'image.bin').write_bytes(image)
    for name, value in (('job.json', job), ('station-config.json', config), ('patch-manifest.json', manifest)):
        (out_dir/name).write_bytes((json.dumps(value, indent=2) + '\n').encode())
    return job


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--bundle', type=Path, required=True)
    parser.add_argument('--station-config', type=Path, required=True)
    parser.add_argument('--out-dir', type=Path, required=True)
    parser.add_argument('--slot', type=int, choices=(0, 1, 2), required=True)
    parser.add_argument('--chip-id-le', required=True)
    parser.add_argument('--action', choices=('patch', 'stock'), required=True)
    parser.add_argument('--source-grid-us', type=float, choices=(4727.5, 3000), required=True)
    parser.add_argument('--controllers-off', action='store_true')
    args = parser.parse_args()
    print(json.dumps(prepare(args.bundle, args.station_config, args.out_dir, slot=args.slot,
                             identity=args.chip_id_le, action=args.action, source_grid_us=args.source_grid_us,
                             controllers_off=args.controllers_off), indent=2))
