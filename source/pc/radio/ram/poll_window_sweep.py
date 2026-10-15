# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Empirical sweep of tracker poll period x command16 N on Frame.

The FW189 RX window formula (poll_decimation) did not hold on hardware: with
N=9 at 42ms only ~20% of polls get a reply and peers drop after 3s silences.
For each (slot_cycle, N) this restarts the radio service, waits for settle and
counts working-channel replies per tracker against the polls the MCU sent.
The station config is restored afterwards. Controller guard stays active.
"""
import argparse
import itertools
import json
import shutil
import subprocess
import time
from pathlib import Path

WORK=Path('/home/steamos/pico-ram-work')
CONFIG=WORK/'station-config.json'
RECORDS=WORK/'radio-live-records.jsonl'
METRICS=WORK/'state/controller-metrics.jsonl'


def service(action):
    subprocess.run(['systemctl','--user',action,'frame-pico-radio.service'],check=False)


def tail_lines(path,since_size):
    try:
        with path.open('rb') as stream:
            stream.seek(since_size if path.stat().st_size>=since_size else 0)
            return [json.loads(line) for line in stream.read().splitlines() if line.strip()]
    except (OSError,ValueError):return []


def run(cycle,n,settle,measure,extra=None):
    config=json.loads(CONFIG.read_text())
    config.update(extra or {});interval_us=int(config.get('interval_us',6000))
    config['slot_cycle']=cycle;config['tracker_poll_decimation_n']=n
    CONFIG.write_text(json.dumps(config,indent=2))
    service('restart');time.sleep(settle)
    start_records=RECORDS.stat().st_size if RECORDS.exists() else 0
    start_metrics=METRICS.stat().st_size if METRICS.exists() else 0
    start=time.monotonic_ns();time.sleep(measure);stop=time.monotonic_ns()
    active=subprocess.run(['systemctl','--user','is-active','frame-pico-radio.service'],
                          capture_output=True,text=True).stdout.strip()
    rows=[r for r in tail_lines(RECORDS,start_records) if start<=r.get('host_read_monotonic_ns',0)<=stop]
    polls=(stop-start)/1000/(interval_us*cycle)
    replies={s:sum(1 for r in rows if r.get('channel')==2 and r.get('profile_index')==s and r.get('hardware_crc_valid'))
             for s in range(3)}
    offers={s:sum(1 for r in rows if r.get('channel')!=2 and r.get('profile_index')==s) for s in range(3)}
    freeze=0.;invalid=0
    for m in tail_lines(METRICS,start_metrics):
        for c in m.get('controllers',{}).values():
            freeze=max(freeze,c.get('max_freeze_ms',0));invalid+=c.get('invalid_episodes',0)
    live={}
    try:live=json.loads((WORK/'radio-live.json').read_text())
    except (OSError,ValueError):pass
    return {'cycle':cycle,'period_us':interval_us*cycle,'n':n,'service':active,
            'reply_ratio':{s:round(v/polls,3) for s,v in replies.items()},'startup_records':offers,
            'states':[t.get('connection_state') for t in live.get('trackers',[])],
            'controller_max_freeze_ms':round(freeze),'controller_invalid_episodes':invalid,'error':live.get('error')}


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--cycles',default='7,6,8')
    parser.add_argument('--ns',default='6,7,8,9,10,11')
    parser.add_argument('--settle',type=float,default=40)
    parser.add_argument('--measure',type=float,default=45)
    parser.add_argument('--extra',type=json.loads,default={})
    parser.add_argument('--out',type=Path,default=WORK/'state/poll-sweep.jsonl')
    args=parser.parse_args()
    backup=CONFIG.with_suffix('.json.before-sweep');shutil.copy2(CONFIG,backup)
    try:
        for cycle,n in itertools.product([int(v) for v in args.cycles.split(',')],[int(v) for v in args.ns.split(',')]):
            result=run(cycle,n,args.settle,args.measure,args.extra)
            with args.out.open('a') as stream:stream.write(json.dumps(result)+'\n')
            print(json.dumps(result),flush=True)
    finally:
        service('stop');shutil.copy2(backup,CONFIG)
        print('restored config; radio service stopped',flush=True)


if __name__=='__main__':main()
