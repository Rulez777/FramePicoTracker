# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Per-peer Swift command/ACK engine for ABI4 SRAM mailboxes.

Transport ACKs do not prove application acceptance. Streaming is only declared
after a checksum-validated real IMU sample. Product data stays raw until its
firmware-specific field layout is independently validated.
"""
from dataclasses import dataclass, field
from enum import Enum
import struct
import time
import secrets
try:
    from ..swift_records import decode_records
    from ..swift_light_sync import record, pack
except ImportError:
    from radio.swift_records import decode_records
    from radio.swift_light_sync import record, pack


REPLY_TIMEOUT_S=3.0


class PeerState(str, Enum):
    ABSENT='absent'
    DISCOVERING='discovering'
    OFFER_SENT='offer_sent'
    CONFIRMING='confirming'
    VERIFYING_MODE='verifying_mode'
    CONFIGURING='configuring'
    WAIT_SYNC='waiting_camera_sync'
    STREAMING='streaming'
    STALE='stale'
    RECONNECTING='reconnecting'


@dataclass
class Pending:
    command: int
    body: bytes
    generation: int
    sequence: int
    first_sent: float
    last_sent: float
    retries: int=0
    s1: int=2
    attempts: list=field(default_factory=list)
    retimes: int=0


@dataclass
class PeerEngine:
    chip_id_le: bytes
    slot: int
    now: object=time.monotonic
    state: PeerState=PeerState.ABSENT
    sequences: list[int]=field(default_factory=lambda:[0]*30)
    pending: Pending | None=None
    generation: int=0
    configure_index: int=0
    last_reply: float | None=None
    last_sample: float | None=None
    ack_notification: bool=False
    product_info_raw: bytes | None=None
    transport_acks: int=0
    retry_count: int=0
    rejected: int=0
    phase_verified: bool=False
    # Command25 [02]: FW189 LED gate 0xc5d4 permits camera-synchronised IR
    # pulses only in mode 2; mode 1 suppresses them (no LEDs were seen with 01).
    configuration: tuple=((17,b'\x07'),(25,b'\x02'),(19,b'\x00'))
    queued: list=field(default_factory=list)
    acked_generation: int | None=None
    confirm_started: float | None=None
    published_s1: int | None=None
    ack_tags: dict=field(default_factory=dict)

    mode_required: bool=False
    mode_nonce: bytes | None=None
    mode_profile: str | None=None
    mode_confirmed_at: float | None=None
    mode_configurations: dict=field(default_factory=dict)
    mode_resume: tuple | None=None
    mode_refresh_s: float=10.0
    mode_lease_s: float=30.0
    mode_query_timeout_s: float=8.0
    mode_retry_backoff_s: float=2.0
    mode_retry_after: float=0.0
    mode_error: str | None=None
    mode_query_failures: int=0
    # Opt-in revision07 remote protocol. Installed05 remains read-only.
    desired_mode: str | None=None
    mode_query_schema: int | None=None
    required_query_profile: str | None=None
    mode_set_target: str | None=None
    mode_switch_started: float | None=None
    mode_switch_timeout_s: float=20.0
    mode_preparing: bool=False
    # Opt-in finite telemetry experiment; never extends the normal proof lease.
    short_exchange_until: float | None=None
    mode_actual_n: int | None=None
    mode_native_pending: int | None=None
    short_probe_started: float | None=None
    short_probe_until: float | None=None

    def enter_short_exchange(self, duration_s, *, expected_n):
        self._enter_short_exchange(duration_s,expected_n=expected_n,resume_probe=False)

    def enter_short_resume_probe(self, duration_s, *, expected_n):
        """Finite closed-session handoff; no liveness timestamp is renewed.

        The owning driver must verify closed MPSL ownership and preserved peer
        state. A checksum-valid working reply must arrive within one second.
        """
        self._enter_short_exchange(duration_s,expected_n=expected_n,resume_probe=True)

    def _enter_short_exchange(self,duration_s,*,expected_n,resume_probe):
        stamp=self.now()
        if (self.short_exchange_until is not None or type(duration_s) not in (int,float)
                or not 0<duration_s<=8 or type(expected_n) is not int or not 0<=expected_n<=40
                or self._query_schema()!=2
                or not self.mode_required or not self.mode_verified() or self.mode_profile!='frame'
                or self.mode_error is not None or not 0<=stamp-self.mode_confirmed_at
                or self.mode_actual_n!=expected_n or self.mode_native_pending!=0
                or self.state not in (PeerState.WAIT_SYNC,PeerState.STREAMING)
                or self.pending is not None or self.queued or self.mode_nonce is not None
                or self.mode_switch_started is not None or self.mode_set_target is not None
                or self.last_reply is None or stamp<self.last_reply
                or (not resume_probe and stamp-self.last_reply>=REPLY_TIMEOUT_S)
                or stamp+duration_s>=self.mode_confirmed_at+self.mode_lease_s):
            raise ValueError('Short exchange requires fresh settled schema2 Frame proof and idle live peer')
        self.short_exchange_until=stamp+duration_s
        if resume_probe:
            self.short_probe_started=stamp;self.short_probe_until=stamp+min(1.,duration_s)

    def _check_short_exchange(self):
        if self.short_exchange_until is None:return
        stamp=self.now()
        probing=(self.short_probe_until is not None and stamp<self.short_probe_until
                 and self.last_reply is not None and self.last_reply<=self.short_probe_started)
        if (stamp>=self.short_exchange_until or not self.mode_verified()
                or self.mode_profile!='frame' or self.pending is not None or self.queued
                or self.mode_nonce is not None or self.mode_switch_started is not None
                or self.state not in (PeerState.WAIT_SYNC,PeerState.STREAMING)
                or self.last_reply is None or stamp<self.last_reply
                or (not probing and (stamp-self.last_reply>=REPLY_TIMEOUT_S
                    or (self.short_probe_started is not None and self.last_reply<=self.short_probe_started)))
                or stamp>=self.mode_confirmed_at+self.mode_lease_s):
            # Raise to the owning loader: do not silently restart a handshake
            # under the short deadline or keep publishing stale mode proof.
            self.mode_error='Bounded short exchange stopped; long response window required'
            self._clear_mode()
            raise RuntimeError(self.mode_error)

    def request_mode(self,profile):
        """Set policy; apply after capability query and application proof.

        Caller must prepare a poll grid supported by BOTH profiles first.
        No transport ACK can complete this request. No implicit OTA.
        """
        if profile not in ('stock','frame') or not self.mode_required or profile not in self.mode_configurations:
            raise ValueError('Remote mode requires explicit dual-mode configuration')
        if self.pending is not None or self.mode_set_target is not None:
            raise ValueError('Command pipeline busy; do not replace an in-flight request')
        self.desired_mode=profile;self.mode_switch_started=self.now()
        self._query_mode()

    def _set_mode(self):
        self.phase_verified=False;self.mode_confirmed_at=None
        self.state=PeerState.VERIFYING_MODE;self.queued.clear()
        if self.mode_switch_started is None:self.mode_switch_started=self.now()
        self.mode_set_target=self.desired_mode;self.mode_nonce=secrets.token_bytes(16)
        target_n=self.mode_configurations[self.desired_mode][0][1][0]
        self._begin(1,b'\x7d\x02'+self.mode_nonce+self.chip_id_le+bytes([int(self.desired_mode=='frame'),target_n]))

    def _accept_mode_set(self,commands):
        if self.mode_set_target is None or self.mode_nonce is None or self.pending is None:return False
        if self.now()-self.pending.first_sent>=self.mode_query_timeout_s:return False
        for c in commands:
            b=c.body
            if (c.command==1 and len(b)==26 and b[:2]==b'\xfd\x02'
                    and b[2:18]==self.mode_nonce and b[18:22]==self.chip_id_le
                    and b[22] in (0,1) and b[23]==int(self.mode_set_target=='frame')
                    and b[24] in (0,1,2) and b[25]==7):
                self.sequences[1]=(self.pending.sequence+1)&63
                self.pending=None;self.mode_set_target=None;self.mode_nonce=None
                if b[24]==2:
                    self.mode_error='Remote mode rejected: tracker OTA busy'
                    self.disconnect();return True
                # Staging ACK is NEVER applied-mode proof. Query a fresh nonce.
                self._query_mode();return True
        return False

    def _clear_mode(self):
        self.mode_actual_n=None;self.mode_native_pending=None
        self.mode_nonce=None;self.mode_profile=None;self.mode_confirmed_at=None;self.mode_resume=None;self.mode_retry_after=0.
        self.mode_set_target=None;self.mode_switch_started=None
        self.mode_preparing=False

    def mode_verified(self):
        return (self.mode_profile in ('stock','frame') and self.mode_confirmed_at is not None
                and self.now()-self.mode_confirmed_at < self.mode_lease_s)

    def _query_mode(self):
        resume=(self.mode_profile,self.state,self.phase_verified)
        if not self.mode_verified():
            # Preserve the overall switch budget across capability/set/query.
            started=self.mode_switch_started
            self._clear_mode();self.mode_switch_started=started
            self.phase_verified=False;self.state=PeerState.VERIFYING_MODE
        self.mode_resume=resume;self.queued.clear()
        self.mode_error=None
        self.mode_nonce=secrets.token_bytes(16)
        self._begin(1,b'\x7e'+bytes([self._query_schema()])+self.mode_nonce)

    def _query_schema(self):
        # Revision07 capability reads must not require requesting a mode change.
        # Existing revision05 read-only callers retain schema1 by default.
        return 2 if self.desired_mode is not None else (self.mode_query_schema or 1)

    def _accept_mode(self,commands):
        if self.mode_set_target is not None:return False
        if self.mode_nonce is None or self.pending is None or self.pending.command!=1:return False
        if self.now()-self.pending.first_sent>=self.mode_query_timeout_s:return False
        previous_mode_fresh=self.mode_verified()
        for c in commands:
            b=c.body
            schema=self._query_schema()
            if (c.command==1 and len(b)==(26 if schema==2 else 24) and b[:2]==bytes([0xfe,schema])
                    and b[2:18]==self.mode_nonce and b[18:22]==self.chip_id_le
                    and b[22] in (0,1) and b[23]==(7 if schema==2 else 1)):
                profile=('stock','frame')[b[22]]
                if self.required_query_profile is not None and profile!=self.required_query_profile:
                    raise ValueError('Dense Frame diagnostic received a non-Frame mode proof; stop RF')
                configuration=self.mode_configurations.get(profile)
                if configuration is None:return False
                self.mode_profile=profile;self.mode_confirmed_at=self.pending.first_sent;self.mode_nonce=None;self.mode_error=None
                self.mode_actual_n=b[24] if schema==2 else None
                self.mode_native_pending=b[25] if schema==2 else None
                self.sequences[1]=(self.pending.sequence+1)&63
                self.pending=None
                if schema==2 and b[25]!=0:
                    # Native command16 needs three scheduler passes for M2.
                    # Resending here would reset its countdown to2 forever.
                    self.phase_verified=False;self.state=PeerState.VERIFYING_MODE
                    if self.mode_switch_started is None:self.mode_switch_started=self.now()
                    self._query_mode();return True
                if schema==2 and b[24]!=configuration[0][1][0]:
                    # Establish CURRENT profile's common-grid N first, then
                    # query actual N and native pending flag; ACK is insufficient.
                    self.phase_verified=False;self.state=PeerState.VERIFYING_MODE
                    self.mode_preparing=True
                    if self.mode_switch_started is None:self.mode_switch_started=self.now()
                    self._begin(*configuration[0]);return True
                if self.desired_mode is not None and profile!=self.desired_mode:
                    self._set_mode();return True
                self.mode_switch_started=None
                resume=self.mode_resume;self.mode_resume=None
                if previous_mode_fresh and resume and resume[0]==profile and resume[1] in (PeerState.WAIT_SYNC,PeerState.STREAMING):
                    self.state=resume[1];self.phase_verified=resume[2]
                else:
                    self.phase_verified=False
                    # Keep post-switch command16 even when actual N already matches:
                    # the remote commit skips native f25 first-commit report 8a71.
                    # Reconfiguration through command16 supplies that native report.
                    self.configuration=configuration;self.configure_index=0
                    self.state=PeerState.CONFIGURING;self._next_config()
                return True
        return False

    def __post_init__(self):
        if len(self.chip_id_le)!=4 or not any(self.chip_id_le) or self.slot not in range(3):
            raise ValueError('Expected a real LE32 identity and slot0..2')

    def disconnect(self):
        self._clear_mode()
        self.state=PeerState.RECONNECTING
        self.sequences=[0]*30
        self.pending=None
        self.configure_index=0
        self.last_reply=self.last_sample=None
        self.ack_notification=False
        self.phase_verified=False
        self.product_info_raw=None
        self.queued.clear();self.ack_tags.clear()

    def start(self):
        # MCU may confirm and enqueue product info before the first host state
        # poll. receive() retains it while ABSENT; do not erase that fresh proof
        # and then ACK its reliable record without using it.
        early_product=(self.product_info_raw if self.state==PeerState.ABSENT
                       and self.last_reply is not None
                       and 0<=self.now()-self.last_reply<=.5 else None)
        self._clear_mode();self.phase_verified=False
        self.product_info_raw=None
        self.state=PeerState.CONFIRMING
        self.pending=None
        self.configure_index=0
        self.sequences=[0]*30
        # The MCU already sent command9 (seq0) on the next own slot after the
        # offer. The host waits for its proof, the product-info event, and never
        # repeats command9: the tracker ignores a duplicate seq0. A fresh nonzero
        # tag keeps tag0 reserved for MCU-originated packets.
        self._next_generation();self.confirm_started=self.now()
        self.published_s1=3 if self.ack_notification else 2
        if early_product is not None:
            self.product_info_raw=early_product
            self._confirmed()
        return self.mailbox()

    def _confirmed(self):
        self.confirm_started=None;self.sequences[9]=1;self.state=PeerState.CONFIGURING;self.pending=None
        if self.mode_required:self._query_mode()
        else:self._next_config()

    def _next_config(self):
        if self.configure_index<len(self.configuration):
            command,body=self.configuration[self.configure_index]
            self.configure_index+=1;self._begin(command,body)
        else:self.state=PeerState.WAIT_SYNC

    def _next_generation(self):
        self.generation=(self.generation+2)&0xffffffff
        if self.generation&0xffff==0:self.generation=(self.generation+2)&0xffffffff
        return self.generation

    def _begin(self, command, body):
        if self.short_exchange_until is not None:
            raise RuntimeError('Commands forbidden during bounded short exchange')
        generation=self._next_generation()
        stamp=self.now()
        self.pending=Pending(command,body,generation,self.sequences[command],stamp,stamp,
                             s1=3 if self.ack_notification else 2,attempts=[generation])
        self.published_s1=self.pending.s1
        return self.mailbox()

    def retime(self):
        """New publication attempt of the same command sequence (new RF tag).

        An ACK carrying any earlier attempt tag still completes the command, and
        `acked_generation` tells the caller which attempt the tracker accepted.
        Retiming refreshes the delivery budget at most three times.
        """
        p=self.pending
        if p is None:return None
        p.generation=self._next_generation();p.attempts=(p.attempts+[p.generation])[-8:]
        if p.retimes<3:p.first_sent=self.now()
        p.retimes+=1
        return p.generation

    def mailbox(self):
        self._check_short_exchange()
        # S1 bit1 is always set; bit0 acknowledges the tracker's previous reply
        # records. Evaluated at every publication, never frozen per command.
        s1=3 if self.ack_notification else 2
        if s1!=self.published_s1:
            # Changed content always gets a fresh seqlock tag: an odd->even rewrite
            # under the same generation could be missed by the MCU (ABA tear).
            self.published_s1=s1;self._next_generation()
            if self.pending is not None:
                self.pending.generation=self.generation
                self.pending.attempts=(self.pending.attempts+[self.generation])[-8:]
        self.ack_tags[self.generation&0xffff]=s1
        if len(self.ack_tags)>32:self.ack_tags.pop(next(iter(self.ack_tags)))
        if self.pending is None:
            return self.generation,s1,b'\0'
        p=self.pending;p.s1=s1
        return p.generation,s1,pack([record(p.sequence,p.command,p.body)])

    def receive(self, payload, *, s1, tx_generation, crc_valid, source_slot=None):
        if crc_valid is not True or s1 not in (2,3):
            self.rejected+=1;return False
        try: commands=decode_records(payload)
        except ValueError:
            self.rejected+=1;return False
        self.last_reply=self.now()
        # Hardware 2026-10-06: replies carry S1=3 whatever we sent, so S1 is not
        # an ACK echo. A reply in the slot that transmitted our tag is the ACK.
        # Records other than IMU/phase/frame-report need S1 bit0 next poll.
        if self.ack_tags.get(tx_generation,0)&1:self.ack_notification=False
        if any(c.command not in (2,10,11) for c in commands):self.ack_notification=True
        mode_accepted=self._accept_mode_set(commands) or self._accept_mode(commands)
        for command in commands:
            if command.command==1 and len(command.body)==35 and command.body[0]==0x82:
                self.product_info_raw=command.body
            if command.command==2 and len(command.body) in (18,24,36,42):
                self.last_sample=self.now()
                if self.phase_verified and (not self.mode_required or self.mode_verified()):self.state=PeerState.STREAMING
        if self.state==PeerState.CONFIRMING and self.product_info_raw is not None:
            # Product info is the application's proof that MCU command9 landed.
            self._confirmed();self.advance();return False
        if mode_accepted:return False
        accepted=self.ack_tag(tx_generation)
        if any(c.command not in (2,10,11) for c in commands):self.ack_notification=True
        return accepted

    def ack_tag(self, tag):
        self.last_reply=self.now()
        if self.ack_tags.get(tag,0)&1:self.ack_notification=False
        p=self.pending
        if self.mode_required and (self.mode_nonce is not None or not self.mode_verified()):return False
        if p is None or tag not in {g&0xffff for g in p.attempts}:
            return False
        self.transport_acks+=1;self.acked_generation=tag
        self.sequences[p.command]=(p.sequence+1)&63
        if p.command==10:self.phase_verified=True
        self.pending=None
        if self.mode_preparing:
            self.mode_preparing=False;self._query_mode()
        elif self.state==PeerState.CONFIGURING:self._next_config()
        self.advance()
        return True

    def advance(self):
        self._check_short_exchange()
        stamp=self.now()
        if self.mode_switch_started is not None and stamp-self.mode_switch_started>=self.mode_switch_timeout_s:
            self.mode_error='Remote switch timed out; applied profile unknown, phase invalid'
            self.disconnect();return
        if self.mode_required and self.mode_confirmed_at is not None:
            if stamp-self.mode_confirmed_at>=self.mode_lease_s:
                self.mode_error=f'Mode proof expired after {self.mode_lease_s:g}s; cadence must be confirmed again'
                self.disconnect();return
            if (self.short_exchange_until is None and self.pending is None
                    and stamp-self.mode_confirmed_at>=self.mode_refresh_s and stamp>=self.mode_retry_after):
                self._query_mode();return
        # Onboard Roy catches only ~10% of replies (BLE-safe 600us slots), so
        # host-side liveness/ACK budgets are 3s; the tracker itself hears polls.
        if self.last_reply is not None and stamp-self.last_reply>=REPLY_TIMEOUT_S:
            self.disconnect();return
        if (self.state==PeerState.CONFIRMING and self.confirm_started is not None
                and stamp-self.confirm_started>=REPLY_TIMEOUT_S):
            # Replies without product info must not hold a peer in CONFIRMING.
            self.disconnect();return
        if self.pending is not None:
            p=self.pending
            if self.mode_nonce is not None and p.command==1:
                if stamp-p.first_sent>=self.mode_query_timeout_s:
                    self.mode_query_failures+=1
                    self.mode_error='Mode 7e reply missing: unsupported firmware, wrong dual-mode ID configuration, or radio loss'
                    if self.mode_verified():
                        self.sequences[1]=(p.sequence+1)&63
                        self.pending=None;self.mode_nonce=None;self.mode_resume=None
                        self.mode_retry_after=stamp+self.mode_retry_backoff_s
                    else:self.disconnect()
                    return
            elif stamp-p.first_sent>=REPLY_TIMEOUT_S:
                self.disconnect();return
            if stamp-p.last_sent>=.05:
                p.last_sent=stamp;p.retries+=1;self.retry_count+=1
            return
        if self.queued:
            command,body=self.queued.pop(0);self._begin(command,body)

    def queue_exposure_sync(self, ticks, online_count, *, timing_verified, own_frame, reason=0):
        if self.mode_required and not self.mode_verified():
            raise ValueError('Mode must be confirmed before exposure sync')
        if timing_verified is not True or not 0<=ticks<533333 or not 1<=online_count<=3:
            raise ValueError('Exposure sync requires validated phase and a nonzero peer count')
        if not 0<=reason<=255:raise ValueError('Invalid light-sync reason')
        # Bound queued work; replace obsolete phase entries instead of accumulating
        # stale exposures during an outage. Command14 only on this peer's frame.
        self.queued=[item for item in self.queued if item[0] not in (10,14)]
        if own_frame:self.queued.append((14,bytes([1,online_count,reason])))
        self.queued.append((10,struct.pack('<i',ticks)))
        self.advance()


class MailboxWriter:
    """Host publication is odd -> full data -> even, never a torn TX packet."""
    def __init__(self,debug,base=0x20010c18):
        self.debug=debug;self.base=base
        self.sent={}

    def publish(self,slot,generation,s1,payload,*,phase_reference=None,cycle_window=None):
        if slot not in range(3) or generation&1 or s1 not in (2,3) or not 1<=len(payload)<=56:
            raise ValueError('Invalid ABI4 mailbox')
        meta=len(payload)|(s1<<8)
        data=bytearray(payload.ljust(56,b'\0'))
        if phase_reference is not None:
            if len(payload)!=8 or payload[1:3]!=b'\x0a\x04':raise ValueError('Timed phase requires exactly command10')
            cycles,ticks=phase_reference
            if not 0<=cycles<2**32 or not 0<=ticks<533333:raise ValueError('Invalid timed phase reference')
            struct.pack_into('<II',data,48,cycles,ticks);meta|=1<<16
        if cycle_window is not None:
            if len(payload)>40:raise ValueError('Timed command exceeds mailbox timing reservation')
            start,end=cycle_window
            if not 0<=start<2**32 or not 0<=end<2**32 or not 0<((end-start)&0xffffffff)<2**31:
                raise ValueError('Invalid command cycle window')
            struct.pack_into('<II',data,40,start,end);meta|=1<<17
        value=(generation,meta,bytes(data))
        if self.sent.get(slot)==value:return
        address=self.base+1664+64*slot
        writes=[(address,struct.pack('<I',generation|1)),
                (address+4,struct.pack('<I',meta)+data),
                (address,struct.pack('<I',generation))]
        batch=getattr(self.debug,'write_many',None)
        if batch is not None:batch(writes)
        else:
            for target,body in writes:self.debug.write(target,body)
        self.sent[slot]=value
