# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Causal per-chip optical identity from command10's documented skipped pulse.

A scheduled slot alone is not identity evidence. Require two distinct commanded
dark frames bracketed by stable measured constellations in the same camera.
"""
from collections import deque
import numpy as np


class TemporalIdentity:
    def __init__(self):
        self.bindings={};self.observations={};self.events={};self.confirmations={}
        self.configuration=None

    def configure(self,chips):
        chips=tuple(chips)
        if chips==self.configuration:return
        self.configuration=chips;self.bindings.clear();self.observations.clear()
        self.events.clear();self.confirmations.clear()

    def sync(self,chip,count,remainder,dark_frame):
        if count!=len(self.configuration or ()) or chip not in (self.configuration or ()):return
        if not 0<=remainder<count:raise ValueError('Invalid temporal frame remainder')
        events=self.events.setdefault(chip,deque(maxlen=10))
        if not any(event[0]==dark_frame for event in events):events.append((dark_frame,count,remainder))

    def observe(self,chip,frame,camera,world_position):
        # None means a decoded exposure with no valid constellation. A missing
        # frame/decoder failure is never recorded as a deliberate dark pulse.
        key=(chip,camera);history=self.observations.setdefault(key,{})
        history[frame]=None if world_position is None else np.asarray(world_position,dtype=float).copy()
        for old in list(history):
            if old<frame-90:del history[old]
        for dark,count,remainder in self.events.get(chip,()):
            token=(chip,dark)
            if token in self.confirmations:continue
            indices=[dark-count*n for n in (3,2,1)]+[dark+count*n for n in (1,2,3)]
            if dark not in history or history[dark] is not None:continue
            if not all(index in history and history[index] is not None for index in indices):continue
            positions=np.array([history[index] for index in indices])
            if not np.isfinite(positions).all():continue
            # Same rigid body moving smoothly: constant-velocity fit through the
            # six lit frames, not a static box that ordinary body motion fails.
            offsets=np.array(indices,dtype=float)-dark
            design=np.column_stack((np.ones(len(offsets)),offsets))
            fit=np.linalg.lstsq(design,positions,rcond=None)[0]
            if (np.max(np.linalg.norm(positions-design@fit,axis=1))>.02
                    or np.linalg.norm(fit[1])*30>1.5):continue
            self.confirmations[token]={'camera':camera,'frame':dark,'count':count,'remainder':remainder}
            confirmations=[v for (identity,_),v in self.confirmations.items() if identity==chip]
            if len(confirmations)>=2 and all(v['count']==count and v['remainder']==remainder for v in confirmations):
                self.bindings[chip]={'count':count,'frame_remainder':remainder,
                                     'method':'Two command10 dark pulses with six stable lit bracketing own frames',
                                     'confirmations':confirmations}
        return chip in self.bindings

    def verified(self,chip,frame):
        binding=self.bindings.get(chip)
        return binding is not None and frame%binding['count']==binding['frame_remainder']
