# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Read-only sampler of Roy's stock RADIO usage (no module, no writes, no halt).

Reads the RADIO configuration/state block and the RTC1 counter (32768Hz) as a
timebase, as fast as SWD allows, to learn what the stock firmware does on air
(mode, channel, address, busy state) and the period of its activity.
  venv/bin/python swd_radio_probe.py --seconds 30 --log state/radio-probe-openocd.log --out state/radio-probe.json
"""
import argparse
import collections
import json
import struct
import time
from pathlib import Path
try:
    from .roy_ram_loader import Debugger
    from .ownership import RoyOwnership
except ImportError:
    from roy_ram_loader import Debugger
    from ownership import RoyOwnership

RADIO=0x40001000;RTC1=0x40011000
STATES={0:'disabled',1:'rxru',2:'rxidle',3:'rx',4:'rxdisable',9:'txru',10:'txidle',11:'tx',12:'txdisable'}
MODES={0:'nrf1m',1:'nrf2m',3:'ble1m',4:'ble2m',5:'ble_lr125k',6:'ble_lr500k',15:'ieee802154'}


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--seconds',type=float,default=30)
    parser.add_argument('--log',type=Path,required=True)
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    samples=[]
    with RoyOwnership():
        debug=Debugger(args.log)
        try:
            until=time.monotonic()+args.seconds
            while time.monotonic()<until:
                rtc=struct.unpack('<I',debug.read(RTC1+0x504,4))[0]
                block=struct.unpack('<21I',debug.read(RADIO+0x504,84))
                rtc2=struct.unpack('<I',debug.read(RTC1+0x504,4))[0]
                freq,mode,pcnf0,pcnf1,base0,base1,prefix0,prefix1,txaddr,rxaddrs=block[1],block[3],block[4],block[5],block[6],block[7],block[8],block[9],block[10],block[11]
                state=block[19]
                samples.append({'rtc':rtc,'rtc2':rtc2,'freq':freq&0x7f,'map':(freq>>8)&1,'mode':mode,'pcnf0':pcnf0,'pcnf1':pcnf1,
                                'base0':base0,'prefix0':prefix0,'txaddr':txaddr,'rxaddrs':rxaddrs,'state':state})
        finally:debug.close()
    summary={'samples':len(samples),
             'state':collections.Counter(STATES.get(s['state'],s['state']) for s in samples).most_common(),
             'mode':collections.Counter(MODES.get(s['mode'],s['mode']) for s in samples).most_common(),
             'freq':collections.Counter(s['freq'] for s in samples).most_common(20),
             'address':collections.Counter((hex(s['base0']),hex(s['prefix0']&0xff),s['txaddr'],s['rxaddrs']) for s in samples).most_common(8),
             'pcnf':collections.Counter((hex(s['pcnf0']),hex(s['pcnf1'])) for s in samples).most_common(4)}
    # Period search: fold busy (non-disabled) samples on candidate periods.
    busy=[(s['rtc']+s['rtc2'])/2/32768 for s in samples if s['state'] not in (0,)]
    every=[(s['rtc']+s['rtc2'])/2/32768 for s in samples]
    best=[]
    if len(every)>200:
        for period_us in list(range(1000,20001,25))+list(range(20000,200001,250)):
            p=period_us/1e6;bins=[0]*20;tot=[0]*20
            for t in every:tot[int((t%p)/p*20)]+=1
            for t in busy:bins[int((t%p)/p*20)]+=1
            frac=[b/max(1,n) for b,n in zip(bins,tot)]
            mean=sum(frac)/20;var=sum((f-mean)**2 for f in frac)/20
            best.append((var,period_us,[round(f,2) for f in frac]))
        best.sort(reverse=True)
    summary['period_candidates']=[(round(v,4),p,f) for v,p,f in best[:8]]
    args.out.write_text(json.dumps({'summary':summary,'samples':samples})+'\n')
    print(json.dumps(summary))


if __name__=='__main__':main()
