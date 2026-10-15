# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Mode transition transaction: accepted requests never count as applied proof.

The 8.8ms bridge reuses the Phase192 stock/frame RX-window intersection.
Native radio runtime supplies fresh nonce/chip-bound proof and request ACKs;
profile owner supplies actual restoration before the new operating cadence.
"""
from dataclasses import dataclass

BRIDGE_US=8800
BRIDGE_N={'stock':1,'frame':2}
FINAL_N={'stock':0,'frame':1}
FINAL_US={'stock':4727,'frame':6000}

def validate_bridge():
    for mode,period,delay in (('stock',4727.5,3934.5),('frame',3000,2207)):
        n=BRIDGE_N[mode]
        if not n*period+delay<=BRIDGE_US<=(n+1)*period:raise ValueError('Bridge outside native receive window')

@dataclass
class ModeHandover:
    chip: str
    desired: str
    deadline_ns: int
    state: str='awaiting_bridge_proof'
    request_token: str | None=None
    final_token: str | None=None
    applied_mode_verified: bool=False
    error: str | None=None

    def __post_init__(self):
        validate_bridge()
        if self.desired not in FINAL_N or self.chip!='OWNER_PRIVATE_ID_REQUIRED':raise ValueError('Current admitted hardware target is single first tracker only')

    def fail(self,reason):
        self.state='failed';self.error=reason;self.applied_mode_verified=False
        raise RuntimeError(reason)

    def proof(self,proof,now_ns,*,session):
        if now_ns>=self.deadline_ns:self.fail('Mode transition deadline expired')
        if (proof.get('chip_id_le')!=self.chip or proof.get('session')!=session
            or proof.get('nonce_bound') is not True or proof.get('pending')!=0
            or type(proof.get('stamp_ns')) is not int or not 0<=now_ns-proof['stamp_ns']<=2_000_000_000):self.fail('Fresh applied-mode proof required')
        mode=proof.get('mode');n=proof.get('n')
        if self.state=='awaiting_bridge_proof':
            if mode not in BRIDGE_N or n!=BRIDGE_N[mode]:self.fail('Current mode does not use the measured bridge grid')
            self.state='request_bridge_mode';return {'mode':self.desired,'n':BRIDGE_N[self.desired]}
        if self.state=='awaiting_bridge_target':
            if mode!=self.desired or n!=BRIDGE_N[self.desired]:self.fail('Requested bridge mode is not actually applied')
            self.state='request_final_grid';return {'mode':self.desired,'n':FINAL_N[self.desired]}
        if self.state=='awaiting_operating_proof':
            if mode!=self.desired or n!=FINAL_N[self.desired]:self.fail('Final mode/cadence not actually applied')
            self.applied_mode_verified=True;self.state='complete';return None
        self.fail('Unexpected mode proof order')

    def requested(self,token):
        if not isinstance(token,str) or not token:self.fail('Native request token missing')
        if self.state=='request_bridge_mode':self.request_token=token;self.state='awaiting_bridge_ack'
        elif self.state=='request_final_grid':self.final_token=token;self.state='awaiting_final_ack'
        else:self.fail('Unexpected native request order')

    def acknowledged(self,token):
        if self.state=='awaiting_bridge_ack' and token==self.request_token:self.state='awaiting_bridge_target';return None
        if self.state=='awaiting_final_ack' and token==self.final_token:
            # ACK means intent accepted. Original owner must restore, and the
            # new runtime must independently prove the final mode/N before19.
            self.state='awaiting_owner_cleanup'
            return {'next_profile':'game' if self.desired=='frame' else 'service','interval_us':FINAL_US[self.desired],'applied_mode_verified':False}
        self.fail('Unmatched native request ACK')

    def cleaned(self,report):
        try:from .frame_profile_owner import BARRIERS
        except ImportError:from frame_profile_owner import BARRIERS
        if self.state!='awaiting_owner_cleanup' or not all(report.get(k) is True for k in BARRIERS):self.fail('Verified previous radio restoration required')
        self.state='awaiting_operating_proof'
