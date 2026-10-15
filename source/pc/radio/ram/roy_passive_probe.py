# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Run on Frame: one bounded RX experiment, with concurrent pose evidence."""
import argparse,json,subprocess,time
from pathlib import Path
p=Path('/home/steamos/pico-ram-work')
profile={'frequency':37,'mode':4,'pcnf0':'0x40008','pcnf1':'0x01040096',
         'base0':'0x552c6a1e','prefix0':'0xc0','crccnf':2,
         'crcpoly':'0x11021','crcinit':'0xffff','whiteiv':0}
parser=argparse.ArgumentParser()
parser.add_argument('--name',default='passive-rx24')
parser.add_argument('--slot',default=1500,type=int)
parser.add_argument('--channel',default=37,type=int)
parser.add_argument('--channels',default='')
parser.add_argument('--duration',default=.5,type=float)
parser.add_argument('--interval',default=10000,type=int)
parser.add_argument('--preamble',action='store_true')
parser.add_argument('--retry',action='store_true')
parser.add_argument('--supported-raw',action='store_true')
parser.add_argument('--ble-reference',action='store_true')
parser.add_argument('--ble-raw',action='store_true')
parser.add_argument('--ble-preamble',action='store_true')
parser.add_argument('--ble-no-white',action='store_true')
parser.add_argument('--ble-short-address',action='store_true')
parser.add_argument('--ble-short-lsb',action='store_true')
parser.add_argument('--ble-short-tail',action='store_true')
parser.add_argument('--swift-partial-tail',action='store_true')
parser.add_argument('--swift-paired-prefix',action='store_true')
parser.add_argument('--swift-recovered-full',action='store_true')
parser.add_argument('--swift-address-lsb',action='store_true')
parser.add_argument('--swift-address-order',type=int,choices=(0,1,2,3),default=0)
parser.add_argument('--swift-multi',action='store_true')
parser.add_argument('--swift-sweep',action='store_true')
parser.add_argument('--swift-tail-index',type=int,choices=(0,1,2),default=0)
parser.add_argument('--swift-phy',choices=('4p8','4p16','1p8'),default='4p8')
parser.add_argument('--prefix',type=int,choices=(85,170),default=85)
parser.add_argument('--raw-length',type=int,choices=(24,40),default=24)
args=parser.parse_args()
name=args.name
profile['frequency']=args.channel
if args.preamble:
    profile.update(pcnf0=0,pcnf1=('0x01011818' if args.raw_length==24 else '0x01012828'),base0=0,prefix0='0x55',crccnf=0)
if args.supported_raw:
    profile.update(pcnf0='0x01000000',pcnf1='0x01022828',base0='0x55555500',prefix0='0x55',crccnf=0)
if args.ble_reference:
    profile.update(mode=3,pcnf0='0x100108',pcnf1='0x02030040',base0='0x89bed600',prefix0='0x8e',crccnf='0x103',crcpoly='0x65b',crcinit='0x555555',whiteiv={2:37,26:38,80:39}[args.channel])
if args.ble_raw:
    profile.update(pcnf0=0,pcnf1='0x02032828',crccnf=0)
if args.ble_preamble:
    profile.update(mode=3,pcnf0=0,pcnf1='0x01012828',base0=0,prefix0=args.prefix,crccnf=0,whiteiv=0)
if args.ble_no_white:
    profile.update(pcnf0=0,pcnf1='0x00032828',crccnf=0,whiteiv=0)
if args.ble_short_address:
    profile.update(mode=3,pcnf0=0,pcnf1='0x00022828',base0='0x89be0000',prefix0='0xd6',crccnf=0,crcpoly='0x65b',crcinit='0x555555',whiteiv=0)
if args.ble_short_lsb:
    profile.update(base0='0xbed60000',prefix0='0x89')
if args.ble_short_tail:
    profile.update(base0='0x89be0000',prefix0='0x8e')
if args.swift_partial_tail:
    profile.update(mode=4,pcnf0=0,pcnf1='0x01022828',base0=('0x4d060000','0xec4c0000','0x84090000')[args.swift_tail_index],prefix0='0xc0',crccnf=0,crcpoly='0x11021',crcinit='0xffff',whiteiv=0)
if args.swift_partial_tail:
    profile.update(mode=(1 if args.swift_phy=='1p8' else 4),pcnf0=('0x01000000' if args.swift_phy=='4p16' else 0))
if args.swift_paired_prefix:
    if not args.swift_partial_tail:
        parser.error('--swift-paired-prefix requires --swift-partial-tail')
    profile['prefix0']=(0x00,0x80,0x40)[args.swift_tail_index]
if args.swift_recovered_full:
    profile.update(mode=4,pcnf0='0x40008',pcnf1='0x01040096',base0='0x4d0625b8',prefix0=0,crccnf=2,crcpoly='0x11021',crcinit='0xffff',whiteiv=0)
    if args.swift_address_lsb:
        profile['base0']='0x4d06b671'
    elif args.swift_address_order:
        profile['base0']=('0x4d0625b8','0x4d06b671','0x4d0698dd','0x4d06e942')[args.swift_address_order]
profilepath=p/(name+'.profile.json')
profilepath.write_text(json.dumps(profile))
with (p/(name+'.poses.csv')).open('w') as output:
    poses=subprocess.Popen(['stdbuf','-oL','/opt/steamvr/bin/linuxarm64/vrcmd','--pollposes'],stdout=output,stderr=subprocess.PIPE)
    try:
        time.sleep(.2)
        command=['python3',str(p/'roy_ram_loader.py'),'--stock',str(p/'stock.bin'),
            '--module',str(p/('roy_sweep.elf' if args.swift_sweep else 'roy_multi.elf' if args.swift_multi else 'roy_passive.elf')),'--out',str(p/(name+'.json')),
            '--log',str(p/(name+'.log')),'--rx-only','--native-shell','--reserve-mpu',
            '--profile',str(profilepath),'--slot-us',str(args.slot),
            '--channels',args.channels,'--duration',str(args.duration),'--interval-us',str(args.interval),'--run']
        if args.retry: command.append('--repeat-host')
        if args.swift_multi: command.append('--swift-multi')
        if args.swift_sweep: command.append('--swift-sweep')
        result=subprocess.run(command,stdout=subprocess.DEVNULL,stderr=subprocess.PIPE,text=True)
        time.sleep(.3)
    finally:
        poses.terminate()
        poses.wait(timeout=3)
counts={}
for line in (p/(name+'.poses.csv')).read_text().splitlines():
    columns=line.split(',')
    if len(columns)==7: counts[columns[0]]=counts.get(columns[0],0)+1
report={'loader_returncode':result.returncode,'loader_stderr':result.stderr,
    'pose_rows_per_device':counts,'pose_process_returncode':poses.returncode,
    'pose_stderr':poses.stderr.read().decode(errors='replace'),
    'scope':'Candidate RX configuration, not validated pairing or tracker firmware'}
(p/(name+'.summary.json')).write_text(json.dumps(report,indent=2))
print(json.dumps(report))






