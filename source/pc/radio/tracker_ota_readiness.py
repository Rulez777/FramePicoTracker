# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Offline inventory and explicit current hardware-OTA blockers.

No hardware access. The temporary hard gate has no JSON/config bypass: replace
it only after actual bootloader/recovery and clean-start evidence is reviewed.
Only explicit CLI-authorized stock reflash or the exact user-accepted risky fork
may pass; underlying unknowns are not cleared.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path
from .tracker_image_audit import audit,crc8_maxim
from .tracker_grid_patch import STOCK_SHA256

BLOCKERS=(
    'FW189 bootloader not available as a verified dump; enlarged-image acceptance unknown',
    'Recovery after a nonbooting application has not been demonstrated',
    'Clean native startup before OTA is not established; pending-worker ACK race remains',
)

# Exact user-selected risky images only; this is not hardware approval.
RISK_ACCEPTED_FORKS={
    '395ce84b3b807af0156845ce51cd8f956daee48959d35956ef2cc70d014fd14f':97320,
    '40941b346f12db9d1385433b45a635500bb553487769d1819a87e3844db356d5':97832,
}

def require_hardware_ota_clearance(job=None):
    # Sole exception (user decision 2026-10-08): reflash the frozen stock image
    # to exercise transfer/CRC/activation with known-good firmware, accepting the
    # unproven recovery risk. Authorization comes only from the loader CLI flag
    # (stored as stock_reflash_authorized after load_job), never from job JSON.
    if (job is not None and job.get('stock_reflash_authorized') is True and job.get('action')=='stock'
            and job.get('sha256')==STOCK_SHA256 and isinstance(job.get('image_bytes'),bytes)
            and hashlib.sha256(job['image_bytes']).hexdigest()==STOCK_SHA256):
        return
    # User2026-10-08 explicitly accepts unavailable SWD/recovery and says start.
    # Narrow exception for this exact image/target/profile, not a claim of safety.
    if (job is not None and job.get('unverified_fork_authorized') is True
            and job.get('preparation_only') is False and job.get('action')=='fork'
            and job.get('radio_profile') in ('service27','service36','service38') and job.get('slot')==0
            and job.get('chip_id_le')=='OWNER_PRIVATE_ID_REQUIRED' and job.get('target_serial')=='OWNER_SUPPLIED_PRIVATE_VALUE_REQUIRED'
            and job.get('source_grid_us')==4727.5
            and job.get('sha256') in RISK_ACCEPTED_FORKS
            and isinstance(job.get('image_bytes'),bytes)
            and len(job['image_bytes'])==RISK_ACCEPTED_FORKS[job['sha256']]
            and hashlib.sha256(job['image_bytes']).hexdigest()==job['sha256']):
        return
    raise RuntimeError('Hardware OTA blocked: '+'; '.join(BLOCKERS))


def inventory(root):
    root=Path(root)
    reference=(root/'outputs/swift189-image.bin').read_bytes()
    if hashlib.sha256(reference).hexdigest()!=STOCK_SHA256:raise ValueError('Wrong FW189 reference')
    container=root/'outputs/pico4e/trackerbin/Swift2.0_sv1.89_20241213_b189_app.bin'
    container_audit=audit(container.read_bytes(),reference)
    paths=[container,root/'outputs/pico4e/trackerbin/Swift_hm01_20230707_b97.bin',
           root/'outputs/pico4e/ctlbin/PhoenixSta_sv2.96_PR_20250805_b296_app.bin',
           root/'outputs/pico4e/ctlbin/PhoenixCtrl_sv3.26_PR_tv01_20240621_b326_app.bin',
           root/'outputs/swift189-image.bin',
           root/'outputs/fw189-dual-mode-iter05-final-20261007/fw189-dual-mode.bin']
    items=[]
    for path in paths:
        data=path.read_bytes();vectors=[]
        # Heuristic candidates only: absence does not prove absence of loader
        # in an unknown compressed/encoded container such as b97.
        for offset in range(0,len(data)-32,4):
            words=struct.unpack_from('<8I',data,offset)
            if (0x20000000<=words[0]<=0x20040000 and words[0]%8==0
                    and words[1]&1 and words[1]<0x40000
                    and sum(v==0 or (v&1 and v<0x40000) for v in words[1:])>=6):
                vectors.append(dict(file_offset=hex(offset),sp=hex(words[0]),reset=hex(words[1])))
        items.append(dict(path=str(path.relative_to(root)).replace('\\','/'),bytes=len(data),
                          sha256=hashlib.sha256(data).hexdigest(),vector_candidates=vectors))
    candidate=paths[-1].read_bytes()
    return dict(schema=1,scope='Offline; no hardware or bootloader execution',
                container_audit=container_audit,inventory=items,
                candidate=dict(bytes=len(candidate),crc8=hex(crc8_maxim(candidate)),
                               blocks=len(candidate)//24+1,last_block_bytes=len(candidate)%24),
                unknown_b97_container=True,verified_fw189_bootloader_available=False,
                recovery_verified=False,clean_start_hardware_verified=False,
                approved_for_ota=False,blockers=list(BLOCKERS))

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root',type=Path,default=Path(__file__).parents[2])
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args();value=inventory(args.root)
    with args.out.open('x',encoding='utf-8') as stream:json.dump(value,stream,indent=2);stream.write('\n')
    print(json.dumps(dict(approved_for_ota=False,files=len(value['inventory']),blockers=value['blockers'])))
