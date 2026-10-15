# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Read-only OpenVR tracking monitor; stop custom RF on controller regression."""
from collections import deque
import json
import subprocess
import threading
import time
import os
from pathlib import Path
import numpy as np


class ControllerMetrics:
    """Per-controller pose freshness. A pose SteamVR keeps returning unchanged
    while `valid` is still true is how a hand "disappears" in a game; validity
    alone missed it on 2026-10-06. Window stats are logged once per second."""
    def __init__(self,log_path=None):
        self.log_path=None if log_path is None else Path(log_path)
        self.last={};self.window={};self.window_start=None;self.latest={}

    def observe(self,sample):
        stamp=sample['t_ns']
        if self.window_start is None:self.window_start=stamp
        for d in sample['devices']:
            if d['kind']!=2:continue
            index=d['index'];w=self.window.setdefault(index,{'samples':0,'changes':0,'invalid':0,
                'invalid_episodes':0,'max_freeze_ms':0.,'was_invalid':False})
            w['samples']+=1
            healthy=d['connected'] and d['valid'] and d['result']==200
            if not healthy:
                w['invalid']+=1
                if not w['was_invalid']:w['invalid_episodes']+=1
            w['was_invalid']=not healthy
            matrix=tuple(d['matrix']);previous=self.last.get(index)
            if previous is None or previous[1]!=matrix:self.last[index]=(stamp,matrix);w['changes']+=1
            else:w['max_freeze_ms']=max(w['max_freeze_ms'],(stamp-previous[0])/1e6)
        if stamp-self.window_start>=1_000_000_000:
            self.latest={index:{k:v for k,v in w.items() if k!='was_invalid'} for index,w in self.window.items()}
            if self.log_path is not None:
                try:
                    with self.log_path.open('a') as stream:
                        stream.write(json.dumps({'t_ns':stamp,'controllers':self.latest})+'\n')
                except OSError:pass
            for w in self.window.values():
                w.update(samples=0,changes=0,invalid=0,invalid_episodes=0,max_freeze_ms=0.)
            self.window_start=stamp

    def current_freeze_ms(self,index,now_ns):
        last=self.last.get(index)
        return None if last is None else (now_ns-last[0])/1e6


class ControllerGuard:
    def __init__(self,binary,*,metrics_log=None,freeze_limit_ms=None,min_controllers=2,forbid_controllers=False):
        command=[str(binary)]
        if hasattr(os,'geteuid') and os.geteuid()==0:command=['runuser','-u','steamos','--',*command]
        self.process=subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.DEVNULL,text=True)
        self.history=deque(maxlen=500)
        self.lock=threading.Lock()
        self.failure=None
        self.started=time.monotonic()
        self.invalid_since=None
        self.controller_indices=None
        # Bench runs (headset on a stand, controllers off or out of view) may
        # lower this; the controllers valid at RF start are still watched.
        self.min_controllers=min_controllers
        # Controllers-off runs (tracker OTA): any connected Valve controller
        # stops custom RF; HMD validity is not required on a resting headset.
        self.forbid_controllers=forbid_controllers
        # freeze_limit_ms=None only records freeze metrics; set it once a
        # station-off baseline shows how long a resting controller pose holds.
        self.metrics=ControllerMetrics(metrics_log);self.freeze_limit_ms=freeze_limit_ms
        self.reader=threading.Thread(target=self._read,daemon=True);self.reader.start()

    def _read(self):
        try:
            for line in self.process.stdout:
                if len(line)>65536:raise ValueError('Tracking monitor line exceeds limit')
                sample=json.loads(line)
                if type(sample.get('t_ns')) is not int:raise ValueError('No OpenVR sample clock')
                with self.lock:self.history.append(sample);self.metrics.observe(sample)
        except (ValueError,OSError) as error:self.failure=error

    def check(self,*,require_now=False):
        stamp=time.monotonic()
        with self.lock:sample=self.history[-1] if self.history else None
        if self.failure is not None or self.process.poll() is not None:
            raise RuntimeError('OpenVR watchdog terminated or failed') from self.failure
        fresh=sample is not None and 0<=time.monotonic_ns()-sample['t_ns']<250_000_000
        if getattr(self,'forbid_controllers',False):
            if fresh:
                connected=[d['index'] for d in sample['devices'] if d['kind']==2 and d['connected']]
                if connected:
                    raise RuntimeError(f'Valve controller connected during a controllers-off run: {connected}; '
                                       'stopping custom RF and restoring Roy')
                self.invalid_since=None;return True
            if require_now:raise RuntimeError('Fresh OpenVR sample required to confirm controllers are off')
            if self.invalid_since is None:self.invalid_since=stamp
            if stamp-self.invalid_since>.5:
                raise RuntimeError('OpenVR monitor stale; cannot confirm controllers are off; stopping custom RF')
            return False
        indices=set();hmd=False
        if fresh:
            for d in sample['devices']:
                healthy=d['connected'] and d['valid'] and d['result']==200
                if d['kind']==1 and healthy:hmd=True
                if d['kind']==2 and healthy:indices.add(d['index'])
        if hmd and len(indices)>=self.min_controllers:
            if self.controller_indices is None:self.controller_indices=indices
            if self.controller_indices<=indices:
                if self.freeze_limit_ms is not None:
                    with self.lock:frozen={i:self.metrics.current_freeze_ms(i,sample['t_ns']) for i in self.controller_indices}
                    worst=max((v for v in frozen.values() if v is not None),default=0)
                    if worst>self.freeze_limit_ms:
                        raise RuntimeError(f'Controller pose frozen {worst:.0f}ms (limit {self.freeze_limit_ms}ms): {frozen}; '
                                           'stopping custom RF and restoring Roy')
                self.invalid_since=None;return True
        if require_now:raise RuntimeError(f'Frame HMD and {self.min_controllers} Valve controllers must have valid tracking before RF starts')
        if self.invalid_since is None:self.invalid_since=stamp
        # Brief occlusion of a controller happens without our RF; 0.5s of
        # continuous loss is treated as coexistence failure. The message names
        # the device so the session log shows what was actually lost.
        if stamp-self.invalid_since>.5:
            age=None if sample is None else (time.monotonic_ns()-sample['t_ns'])/1e6
            devices=[] if sample is None else [(d['index'],d['kind'],d['connected'],d['valid'],d['result'])
                                               for d in sample['devices']]
            raise RuntimeError(f'HMD/controller tracking lost for {stamp-self.invalid_since:.2f}s; '
                               f'sample_age_ms={age}; devices={devices}; stopping custom RF and restoring Roy')
        return False

    def head_pose(self,source_time_ns,max_gap_ns=30_000_000):
        with self.lock:history=list(self.history)
        before=None;after=None
        for sample in history:
            hmd=next((d for d in sample['devices'] if d['kind']==1 and d['connected'] and d['valid'] and d['result']==200),None)
            if hmd is None:continue
            if sample['t_ns']<=source_time_ns:before=(sample['t_ns'],hmd)
            if sample['t_ns']>=source_time_ns:after=(sample['t_ns'],hmd);break
        if before is None or after is None or after[0]-before[0]>max_gap_ns:
            raise ValueError('No bracketing valid HMD poses for exposure time')
        a=np.eye(4);b=np.eye(4)
        a[:3]=np.array(before[1]['matrix']).reshape(3,4)
        b[:3]=np.array(after[1]['matrix']).reshape(3,4)
        fraction=0 if before[0]==after[0] else (source_time_ns-before[0])/(after[0]-before[0])
        result=np.eye(4)
        # Polar projection of interpolated rotations avoids nonrigid pose matrices.
        u,_,v=np.linalg.svd((1-fraction)*a[:3,:3]+fraction*b[:3,:3])
        result[:3,:3]=u@np.diag([1,1,np.linalg.det(u@v)])@v
        result[:3,3]=(1-fraction)*a[:3,3]+fraction*b[:3,3]
        return result

    def close(self):
        if self.process.poll() is None:
            self.process.terminate()
            try:self.process.wait(timeout=2)
            except subprocess.TimeoutExpired:self.process.kill();self.process.wait(timeout=2)
        self.reader.join(timeout=2);self.process.stdout.close()
