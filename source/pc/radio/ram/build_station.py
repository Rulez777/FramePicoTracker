# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Reproducible offline ABI3 build, never opens SWD or starts a service.

This preserves the experimental RF engine; it does not certify coexistence,
camera synchronization or 6DoF. Run on Frame with clang and ld.lld installed.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
try:
    from .roy_ram_loader import elf_image, STOCK_SHA
except ImportError:
    from roy_ram_loader import elf_image, STOCK_SHA

DEFINES = ('ROY_NATIVE_CALL ROY_MPU_RESERVE ROY_RX_ONLY ROY_RX_POLL_TICK '
           'ROY_SWIFT_POLL ROY_SWIFT_POLL_NOOP ROY_SWIFT_POLL_IDENTITY '
           'ROY_SWIFT_POLL_BOOTSTRAP ROY_SWIFT_POLL_FAST_RX '
           'ROY_SWIFT_STATION_OFFER ROY_SWIFT_STATION_HANDOVER ROY_STATION_SERVICE').split()


def build(source: Path, output: Path, abi=3, service=False):
    if abi not in (3,4):raise ValueError('Unsupported station ABI')
    source_name='roy_ram_station_v4.c' if abi==4 else 'roy_ram_station_three.c'
    if abi==4 and service and (source/'roy_ram_station_service27.c').is_file():
        source_name='roy_ram_station_service27.c'
    output.mkdir(parents=True,exist_ok=True)
    elf=output/'roy_station.elf'
    # Service build (ROY_SERVICE_MODE): controllers off. Drops the BLE phase-lock
    # and re-requests the earliest slot on a block. Not for coexistence.
    defines=list(DEFINES)+(['ROY_SERVICE_MODE'] if service else [])
    command=['clang','--target=arm-none-eabi','-mcpu=cortex-m4','-mthumb','-Oz',
             *(['-fomit-frame-pointer'] if abi==4 else []),
             '-ffreestanding','-fno-builtin','-fno-unwind-tables',
             '-fno-asynchronous-unwind-tables','-ffunction-sections','-fdata-sections',
             '-nostdlib',*['-D'+d for d in defines],'-Wl,--gc-sections',
             '-Wl,-T,'+str(source/'roy_ram_rx.ld'),str(source/source_name),
             '-o',str(elf)]
    subprocess.run(command,check=True)
    entry,code=elf_image(elf)
    manifest={'config_version':1,'abi':abi,'stock_sha256':STOCK_SHA,
              'entry':hex(entry),'code_bytes':len(code),'code_limit':2045,
              'code_sha256':hashlib.sha256(code).hexdigest(),
              'elf_sha256':hashlib.sha256(elf.read_bytes()).hexdigest(),
              'source_sha256':{name:hashlib.sha256((source/name).read_bytes()).hexdigest()
                               for name in (source_name,'roy_ram_rx.ld')},
              'build_command':command,'compiler':subprocess.check_output(['clang','--version'],text=True),
              'hardware_validated':False,'production_ready':False}
    (output/'deployment.json').write_text(json.dumps(manifest,indent=2)+'\n')
    return manifest


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source',type=Path,default=Path(__file__).parent)
    parser.add_argument('--out',type=Path,required=True)
    parser.add_argument('--abi',type=int,choices=(3,4),default=3)
    parser.add_argument('--service',action='store_true',help='ROY_SERVICE_MODE: no BLE phase-lock, re-request on block (controllers off)')
    args=parser.parse_args()
    print(json.dumps(build(args.source,args.out,args.abi,args.service)))
