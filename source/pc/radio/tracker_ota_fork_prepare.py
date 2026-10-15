# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Prepare the exact iteration05 fork for offline service27 validation only.

No hardware APIs or executable flashing command. Hardware job loading remains
blocked; this package is not permission to send start/data/finish to a tracker.
"""
import argparse
import hashlib
import json
import shutil
from pathlib import Path
from types import SimpleNamespace

from .tracker_dual_mode_patch import reverse
from .ram.tracker_ota_runtime import load_job,plan_preflight,preflight


def prepare(bundle,config_path,module_path,measurement_path,out_dir):
    bundle=Path(bundle);out_dir=Path(out_dir)
    image=(bundle/'fw189-dual-mode.bin').read_bytes()
    if len(image)!=97320 or hashlib.sha256(image).hexdigest()!=\
            '395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f':
        raise ValueError('Preparation requires the exact final iteration05 fork')
    manifest=json.loads((bundle/'patch-manifest.json').read_text(encoding='utf-8'))
    original=reverse(image,manifest)
    if original!=(bundle/'fw189-stock.bin').read_bytes():
        raise ValueError('Bundle stock copy differs from reverse validation')
    config=json.loads(Path(config_path).read_text(encoding='utf-8'))
    measurement=json.loads(Path(measurement_path).read_text(encoding='utf-8'))
    peer=measurement['final']['trackers'][0]
    if (peer['slot']!=0 or peer['id_le']!='OWNER_PRIVATE_ID_REQUIRED' or peer['firmware']!=189
            or peer['serial']!='OWNER_SUPPLIED_PRIVATE_VALUE_REQUIRED'):
        raise ValueError('Saved measurement does not identify the measured slot0 tracker')
    job=dict(schema=1,action='fork',source_grid_us=4727.5,slot=0,
             chip_id_le=peer['id_le'],radio_profile='service27',image='image.bin',
             sha256=hashlib.sha256(image).hexdigest(),manifest='patch-manifest.json',
             result='ota-result.json')
    args=SimpleNamespace(station_v4=True,station_service=True,slot_us=900,
                         interval_us=4727,steady_interval_us=4727,mpsl_priority=0,
                         module=Path(module_path))
    plan_preflight(job,config,args)  # Validate everything before making files.
    out_dir.mkdir(parents=True,exist_ok=False)
    (out_dir/'image.bin').write_bytes(image)
    (out_dir/'stock-reference.bin').write_bytes(original)
    shutil.copy2(bundle/'patch-manifest.json',out_dir/'patch-manifest.json')
    shutil.copy2(module_path,out_dir/'service27.elf')
    registry=dict(version=2,peers=[dict(slot=0,chip_id_le=peer['id_le'],
                                       product_sn=peer['serial'],firmware=189,
                                       bond_state='historical-measurement')])
    for name,value in [('job.json',job),('station-config.json',config),('registry.json',registry)]:
        (out_dir/name).write_text(json.dumps(value,indent=2)+'\n',encoding='utf-8')
    loaded=load_job(out_dir/'job.json',out_dir/'registry.json',preparation_only=True)
    args.module=out_dir/'service27.elf';plan_preflight(loaded,config,args)
    failures={}
    try:load_job(out_dir/'job.json',out_dir/'registry.json')
    except ValueError as error:failures['hardware_job_loader']=str(error)
    try:preflight(loaded,config,args)
    except RuntimeError as error:failures['hardware_clearance']=str(error)
    if len(failures)!=2:raise AssertionError('Preparation unexpectedly became hardware executable')
    evidence=dict(schema=1,state='prepared_offline_hardware_blocked',image_sha256=job['sha256'],
                  source_measurement_sha256=hashlib.sha256(Path(measurement_path).read_bytes()).hexdigest(),
                  target_chip_id_le=peer['id_le'],target_serial=peer['serial'],
                  live_identity_checked=False,controllers_off_live_checked=False,
                  bootloader_verified=False,recovery_verified=False,clean_start_verified=False,
                  hardware_started=False,stop_before='first real tracker OTA start command',
                  rejection_proofs=failures)
    (out_dir/'PREPARATION.json').write_text(json.dumps(evidence,indent=2)+'\n',encoding='utf-8')
    files=[dict(name=p.name,bytes=p.stat().st_size,sha256=hashlib.sha256(p.read_bytes()).hexdigest())
           for p in sorted(out_dir.iterdir()) if p.is_file()]
    (out_dir/'manifest.json').write_text(json.dumps(files,indent=2)+'\n',encoding='utf-8')
    return evidence


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    for name in ('bundle','config','module','measurement','out'):
        parser.add_argument('--'+name,type=Path,required=True)
    args=parser.parse_args()
    print(json.dumps(prepare(args.bundle,args.config,args.module,args.measurement,args.out),indent=2))
