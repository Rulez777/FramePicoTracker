# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Roy-clock scheduled command10 and own-frame command14 metadata.

The phase offset at the short exposure is an explicit calibration input.
Transport timing and temporal optical identity are separate verification gates.
Frame indices are counted incrementally between observed exposures and looked up
from nearby anchors, so averaged-period error never accumulates over a session.
"""
import math
import threading
from collections import deque


class ExposureSchedule:
    def __init__(self,camera_clock,roy_clock,phase_at_exposure_ticks):
        if type(phase_at_exposure_ticks) is not int or not 0<=phase_at_exposure_ticks<533333:
            raise ValueError('Explicit LED timer phase at short exposure required')
        self.camera_clock=camera_clock;self.roy_clock=roy_clock;self.phase=phase_at_exposure_ticks
        self.origin_raw=None;self.last_raw=None;self.last_index=None;self.period=1/30;self.intervals=deque(maxlen=60)
        self.anchors=deque(maxlen=300);self.exposures=deque(maxlen=12);self.lock=threading.Lock()
        self.last_sync={};self.lights={};self.pending_timing={};self.dark_frames={}

    def observe(self,metadata):
        raw=metadata['timestamp_raw']
        # Frame counting and period statistics use every frame in order; only a
        # fresh exposure may become the timing reference (convert's 250ms bound).
        try:stamp=self.camera_clock.convert(metadata)
        except ValueError:stamp=None
        with self.lock:
            if self.origin_raw is None:
                self.origin_raw=raw;index=0
            elif raw<=self.last_raw:return stamp,self._index(raw)
            else:
                delta=raw-self.last_raw
                if .032<delta<.035:
                    self.intervals.append(delta);self.period=sum(self.intervals)/len(self.intervals)
                index=self.last_index+max(1,round(delta/self.period))
            self.last_raw=raw;self.last_index=index;self.anchors.append((raw,index))
            if stamp is not None:
                self.exposures.append(stamp);self.last_metadata=metadata
        if stamp is None:raise ValueError('Camera exposure stale; counted for frame index only')
        return stamp,index

    def _index(self,raw):
        if not self.anchors:raise ValueError('Camera exposure origin not observed')
        anchor,index=min(self.anchors,key=lambda item:abs(item[0]-raw))
        return index+round((raw-anchor)/self.period)

    def frame_index(self,metadata):
        with self.lock:return self._index(metadata['timestamp_raw'])

    def exposure_times(self):
        with self.lock:return list(self.exposures)

    def prepare(self,position,count,now_ns,phase_adjust_ticks=0,*,key=None):
        """Plan timed command14/10 for optical `position`; caches use stable `key`."""
        key=position if key is None else key
        if self.roy_clock.fit is None or self.last_raw is None or len(self.intervals)<6:return None
        # Firmware timed mailbox divides the 64MHz DWT cycles by four. Do not
        # enable it merely because a generic host clock fit happened to succeed.
        if abs(self.roy_clock.fit[2]-15.625)>.003:raise ValueError('Roy DWT is not measured 64MHz')
        with self.lock:metadata=self.last_metadata;frame=self.last_index
        exposure=self.camera_clock.convert(metadata,now_ns)
        elapsed=(now_ns-exposure)/1e9
        ahead=max(1,math.ceil(elapsed/self.period))
        target_frame=frame+ahead
        while target_frame%count!=position:target_frame+=1
        target_ns=exposure+round((target_frame-frame)*self.period*1e9)
        # command14 belongs to the own camera-frame period. Keep retries out of
        # adjacent peers' periods using the MCU deadline, not host sleep timing.
        light_due=self.lights.get(key)!=count
        phase_due=now_ns-self.last_sync.get(key,0)>=800_000_000
        if not light_due and not phase_due:return None
        if target_ns-now_ns>200_000_000:return None
        reference=self.roy_clock.cycles_at(now_ns)
        ticks=(self.phase+phase_adjust_ticks+round((now_ns-exposure)*.016))%533333
        phase_target=target_ns+round(count*self.period*1e9) if light_due else target_ns
        phase_window=(self.roy_clock.cycles_at(phase_target-12_000_000),
                      self.roy_clock.cycles_at(phase_target-4_000_000))
        light_window=(self.roy_clock.cycles_at(target_ns+1_000_000),
                      self.roy_clock.cycles_at(target_ns+12_000_000))
        return {'ticks':ticks,'reference':(reference,ticks),'phase_window':phase_window,
                'light_window':light_window,'own_frame':target_frame,'count':count,'light_due':light_due,
                'light_target_ns':target_ns,'phase_target_ns':phase_target,'position':position,'key':key}

    def identity_slot(self,metadata,active_slots):
        frame=self.frame_index(metadata);count=len(active_slots)
        if not count:return None
        return active_slots[frame%count]
