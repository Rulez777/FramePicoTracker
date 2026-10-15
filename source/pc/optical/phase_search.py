# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Measure the tracker LED timer phase that lands pulses inside the short exposure.

The phase is not known from documentation. Scan it in steps; a step counts only
after every active peer acknowledged command10 with that phase and a settle
interval passed. Lock at the centre of the lit range. A locked phase that stops
producing lit own frames, or never yields temporal identity, resumes scanning.
Lit evidence is model-fit inliers on scheduled own frames, never blob count alone.
"""
import threading

TICKS_PER_FRAME=533333


class PhaseSearch:
    def __init__(self,start_ticks,*,step=6400,settle_frames=12,window_frames=24,lit_fraction=.5,
                 identity_timeout_frames=900,max_extend=TICKS_PER_FRAME//4):
        if type(start_ticks) is not int or not 0<=start_ticks<TICKS_PER_FRAME:raise ValueError('Invalid start phase')
        self.step=step;self.settle=settle_frames;self.window=window_frames;self.lit_fraction=lit_fraction
        self.identity_timeout=identity_timeout_frames;self.max_extend=max_extend
        self.require_all=True
        self.lock=threading.Lock();self.phase=start_ticks;self.state='verifying'
        self.first_lit=None;self.last_lit=None;self.scanned=0;self.locked_at=None;self.history=[]
        self._restart_window(set())

    def _restart_window(self,chips):
        self.required=set(chips);self.acked_chips=set();self.applied=None;self.seen=set();self.lit=set()

    def _move(self,phase,chips,state=None):
        self.phase=phase%TICKS_PER_FRAME
        if state:self.state=state
        self._restart_window(chips)

    def acked(self,chip,phase,frame_index,active_chips):
        """Command10 ACK for `chip` carrying `phase`; window starts after all peers took it."""
        with self.lock:
            if phase!=self.phase:return
            if not self.required:self.required=set(active_chips)
            self.acked_chips.add(chip)
            # Sparse onboard polling: waiting for every peer could stall the scan
            # forever while one reconnects; start once any peer took this phase
            # (all=True restores the strict rule).
            if self.applied is None and (self.required<=self.acked_chips or not self.require_all):
                self.applied=frame_index

    def observe(self,frame_index,lit,*,identity_verified=False,active_chips=()):
        """Returns True when the caller must resend command10 with the new phase."""
        with self.lock:
            if self.state=='locked':
                if identity_verified:self.locked_at=None
                elif self.locked_at is not None and frame_index-self.locked_at>self.identity_timeout:
                    self.history.append(('identity_timeout',self.phase))
                    self.scanned=0;self._move(self.phase+self.step,active_chips,'searching');return True
            if self.applied is None or frame_index<=self.applied+self.settle:return False
            self.seen.add(frame_index)
            if lit:self.lit.add(frame_index)
            if len(self.seen)<self.window:return False
            bright=len(self.lit)>=self.lit_fraction*len(self.seen)
            self.history.append((self.state,self.phase,len(self.lit),len(self.seen)))
            del self.history[:-64]
            if self.state=='locked':
                if bright:self._restart_window(active_chips);return False
                self.scanned=0;self._move(self.phase+self.step,active_chips,'searching');return True
            if self.state=='verifying':
                if bright:
                    self.state='locked';self.locked_at=frame_index;self._restart_window(active_chips);return False
                self._move(self.phase+self.step,active_chips,'searching');return True
            if self.state=='searching':
                if bright:
                    self.first_lit=self.last_lit=self.phase
                    self._move(self.phase+self.step,active_chips,'extending');return True
                self.scanned+=self.step;self._move(self.phase+self.step,active_chips);return True
            # extending: walk forward until the pulse leaves the exposure.
            if bright and (self.phase-self.first_lit)%TICKS_PER_FRAME<self.max_extend:
                self.last_lit=self.phase;self._move(self.phase+self.step,active_chips);return True
            span=(self.last_lit-self.first_lit)%TICKS_PER_FRAME
            self.locked_at=frame_index
            self._move(self.first_lit+span//2,active_chips,'locked');return True

    def status(self):
        with self.lock:
            return {'state':self.state,'phase_ticks':self.phase,'scanned_ticks':self.scanned,
                    'lit_range':None if self.first_lit is None else [self.first_lit,self.last_lit],
                    'window':[len(self.lit),len(self.seen)],'awaiting_ack':self.applied is None}
