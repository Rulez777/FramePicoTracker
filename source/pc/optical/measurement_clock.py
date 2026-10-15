# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Explicit camera clock evidence and bounded Roy/sample clock estimation.

An old recording or decoder arrival time cannot certify a camera clock domain.
Camera conversion requires a session-specific independently measured mapping.
"""
from collections import deque
import math
import time
import numpy as np


class CameraClock:
    def __init__(self,evidence,boot_id):
        if (evidence.get('version')!=1 or evidence.get('boot_id')!=boot_id
                or evidence.get('verified') is not True
                or evidence.get('source_domain') not in ('CLOCK_MONOTONIC','CLOCK_MONOTONIC_RAW')
                or not evidence.get('method')
                or not 0<=evidence.get('uncertainty_ns',float('inf'))<=500_000):
            raise ValueError('Missing current-boot measured camera clock evidence')
        self.offset=evidence['offset_ns']
        self.source_domain=evidence['source_domain']
        if type(self.offset) is not int:raise ValueError('Invalid camera clock offset')
        self.uncertainty_ns=evidence['uncertainty_ns']

    def convert(self,metadata,now_ns=None,max_age_ns=250_000_000):
        stamp=metadata.get('timestamp_raw')
        exposure=metadata.get('exposure_raw')
        if (not isinstance(stamp,(int,float)) or not math.isfinite(stamp)
                or not isinstance(exposure,(int,float)) or not 0<exposure<.001):
            raise ValueError('Expected timestamp and short IR exposure in seconds')
        # Dataset timestamp convention must be explicit in the evidence;
        # offset includes any validated exposure-start to midpoint correction.
        result=round(stamp*1e9)+self.offset
        if self.source_domain=='CLOCK_MONOTONIC_RAW':
            # MONOTONIC slews relative to RAW on Frame. Measure this offset
            # every conversion; a startup constant drifts during long sessions.
            before=time.clock_gettime_ns(time.CLOCK_MONOTONIC_RAW)
            mono=time.monotonic_ns()
            after=time.clock_gettime_ns(time.CLOCK_MONOTONIC_RAW)
            if after-before>self.uncertainty_ns*2:
                raise ValueError('Camera clock bridge sampling uncertainty exceeded')
            result=round(stamp*1e9)+mono-(before+after)//2+self.offset
        now_ns=time.monotonic_ns() if now_ns is None else now_ns
        if not 0<=now_ns-result<=max_age_ns:raise ValueError('Camera exposure stale or in future')
        return result


class RoyClock:
    def __init__(self):
        self.points=deque(maxlen=40)
        self.last_raw=None;self.unwrapped=0;self.fit=None

    def observe(self,cycles,before_ns,after_ns):
        # GPIO-bitbanged SWD on Frame takes several ms per read; keep reads up
        # to 10ms and let the weighted fit, not each read, carry the precision.
        self.last_read_ns=after_ns-before_ns
        if not 0<=cycles<2**32 or not 0<=after_ns-before_ns<=10_000_000:
            self.rejected=getattr(self,'rejected',0)+1;return False
        if self.last_raw is not None:
            delta=(cycles-self.last_raw)&0xffffffff
            if delta>=2**31:raise ValueError('Roy cycle clock restarted or sampling gap exceeded half wrap')
            self.unwrapped+=delta
        else:self.unwrapped=cycles
        self.last_raw=cycles
        self.points.append((self.unwrapped,(before_ns+after_ns)/2,(after_ns-before_ns)/2))
        if len(self.points)<6 or self.points[-1][1]-self.points[0][1]<300_000_000:return False
        data=np.array(self.points);x=data[:,0]-data[-1,0];y=data[:,1]-data[-1,1]
        weights=1/np.maximum(data[:,2],10_000)**2
        # Roy is an nRF52833 with a 64MHz crystal (+-50ppm): the slope is known.
        # Fitting it from multi-ms SWD reads only adds noise, so fit the offset.
        slope=15.625
        intercept=float(np.sum(weights*(y-slope*x))/np.sum(weights))
        residual=y-(slope*x+intercept)
        # Read latency is mostly symmetric jitter. With the slope fixed only the
        # offset is estimated, so its error is the residual RMS / sqrt(n) (not the
        # per-read RMS, which reached 1.5ms with 8ms SWD reads under optics load)
        # plus the best single read half-width.
        uncertainty=float(np.sqrt(np.mean(residual**2)/len(residual))+np.min(data[:,2]))
        self.uncertainty=uncertainty
        if not 14<slope<18 or uncertainty>1_000_000:self.fit=None;return False
        self.fit=(self.unwrapped,data[-1,1]+intercept,slope,uncertainty)
        return True

    def convert(self,cycles,now_ns=None):
        if self.fit is None:raise ValueError('Roy clock not measured yet')
        reference,stamp,slope,error=self.fit
        delta=((cycles-(reference&0xffffffff)+2**31)&0xffffffff)-2**31
        result=round(stamp+slope*delta)
        now_ns=time.monotonic_ns() if now_ns is None else now_ns
        if not 0<=now_ns-result<=250_000_000:raise ValueError('Roy observation stale/future')
        return result,error

    def cycles_at(self,stamp_ns):
        if self.fit is None:raise ValueError('Roy clock not measured yet')
        reference,stamp,slope,error=self.fit
        if abs(stamp_ns-stamp)>250_000_000:raise ValueError('Roy clock projection exceeds horizon')
        return (reference+round((stamp_ns-stamp)/slope))&0xffffffff


class SampleClock:
    """Counter unwrapping plus per-second lower envelope; no timer24 wrap guess."""
    def __init__(self):self.reset()

    def reset(self):
        self.counter=None;self.index=0;self.bins={};self.period_ns=4_976_480.;self.gaps=0
        self.offset_ns=None

    def observe(self,counter,receive_ns):
        if not 0<=counter<=255:raise ValueError('Invalid IMU sample counter')
        delta=0 if self.counter is None else (counter-self.counter)&255
        if self.counter is not None and not 1<=delta<=127:return None
        self.index+=delta;self.gaps+=max(0,delta-1);self.counter=counter
        residual=receive_ns-self.index*self.period_ns;bucket=receive_ns//1_000_000_000
        point=(self.index,residual)
        if bucket not in self.bins or residual<self.bins[bucket][1]:self.bins[bucket]=point
        self.bins={key:value for key,value in self.bins.items() if bucket-key<30}
        entries=np.array(list(self.bins.values()),dtype=float)
        if len(entries)<3:return None
        x=entries[:,0]-self.index
        drift,offset=np.linalg.lstsq(np.column_stack((x,np.ones(len(x)))),entries[:,1],rcond=None)[0]
        if abs(drift)>self.period_ns*.002:raise ValueError('IMU sample clock drift outside fit limit')
        # This time still contains minimum transport delay. An optical alignment
        # estimate is required before using it as physical sample time in fusion.
        return round(self.index*self.period_ns+offset),float(drift)
