# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""One finite optical scan session; never restart or treat controller blobs as PICO."""
import time

MODULUS = 533333


class BoundedPhaseScan:
    def __init__(self, phases, chip_id, *, timeout_s=90, ack_timeout_s=3,
                 settle_frames=2, window_frames=4, now=time.monotonic):
        phases = tuple(phases)
        if (not phases or len(phases) > 128 or len(set(phases)) != len(phases)
                or any(type(p) is not int or not 0 <= p < MODULUS for p in phases)):
            raise ValueError('Expected 1..128 unique explicit phases')
        if not 0 < ack_timeout_s <= timeout_s <= 120:
            raise ValueError('Invalid one-session time budget')
        if settle_frames < 1 or window_frames < 3:
            raise ValueError('Insufficient optical observation window')
        self.phases, self.chip_id = phases, chip_id
        self.now, self.started = now, now()
        self.deadline = self.started + timeout_s
        self.ack_timeout = ack_timeout_s
        self.settle, self.window = settle_frames, window_frames
        self.index = 0
        self.state = 'ready'
        self.history = []
        self.frames = {}
        self.generation = self.sent_at = self.applied_frame = None

    @property
    def phase(self):
        return self.phases[self.index]

    def check(self, *, controllers_healthy=True):
        if self.state in ('locked', 'exhausted', 'stopped'):
            return False
        if not controllers_healthy:
            self.stop('controller health failed')
        elif self.now() >= self.deadline:
            self.stop('whole-session deadline')
        elif self.state == 'awaiting_ack' and self.now() - self.sent_at >= self.ack_timeout:
            self.stop('phase command acknowledgement deadline')
        return self.state not in ('locked', 'exhausted', 'stopped')

    def published(self, generation):
        if not self.check() or self.state != 'ready' or not 0 < generation < 65536 or generation & 1:
            raise ValueError('Scan phase cannot be published')
        self.generation, self.sent_at = generation, self.now()
        self.state = 'awaiting_ack'

    def acknowledge(self, chip_id, generation, phase, frame_index):
        if not self.check() or self.state != 'awaiting_ack':
            return False
        if (chip_id, generation, phase) != (self.chip_id, self.generation, self.phase):
            return False
        self.applied_frame = frame_index
        self.state = 'observing'
        return True

    def observe(self, frame_index, *, fresh, timing_verified, model_fit,
                target_identity_verified):
        if not self.check() or self.state != 'observing':
            return False
        if not fresh or not timing_verified or frame_index <= self.applied_frame + self.settle:
            return False
        if frame_index in self.frames:
            return False
        self.frames[frame_index] = bool(model_fit and target_identity_verified)
        if len(self.frames) < self.window:
            return False
        lit = sum(self.frames.values())
        self.history.append(dict(phase=self.phase, generation=self.generation,
                                 verified_lit_frames=lit, frames=len(self.frames)))
        if lit * 2 >= len(self.frames):
            self.state = 'locked'
        elif self.index + 1 == len(self.phases):
            self.state = 'exhausted'
        else:
            self.index += 1
            self.state = 'ready'
            self.frames = {}
        return True

    def stop(self, reason):
        self.state = 'stopped'
        self.reason = reason

    def status(self):
        return dict(state=self.state, phase=self.phase, completed=len(self.history),
                    maximum=len(self.phases), remaining_s=max(0, self.deadline-self.now()),
                    reason=getattr(self, 'reason', None), history=list(self.history))
