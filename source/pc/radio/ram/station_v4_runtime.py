# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Integrated ABI4 peer lifecycle, enrollment, mailboxes and honest live state."""
import json
import struct
import time
from pathlib import Path
try:
    from .peer_engine import PeerEngine,MailboxWriter,PeerState
    from ..pico_swift_probe import decode_bootstrap_record,decode_identity_response
    from ..swift_records import decode_records
    from ..swift_product_info import decode_product_info
    from ...optical.imu_calibration import decode_imu_samples
except ImportError:
    try:from peer_engine import PeerEngine,MailboxWriter,PeerState
    except ImportError:from radio.ram.peer_engine import PeerEngine,MailboxWriter,PeerState
    from radio.pico_swift_probe import decode_bootstrap_record,decode_identity_response
    from radio.swift_records import decode_records
    from radio.swift_product_info import decode_product_info
    from optical.imu_calibration import decode_imu_samples


def address(chip,seed):
    summed=bytes((a+b)&255 for a,b in zip(chip,seed))
    return int(f'{int.from_bytes(summed,"little"):032b}'[::-1],2)


TRACKER_PERIOD_US=4727.5
TRACKER_RX_REOPEN_US=3538


def validate_remote_mode_grid(profile,identities,decimations,poll_period_us):
    """Offline transition preflight; does not prove RF or controller coexistence."""
    if profile is None:return
    if profile not in ('stock','frame') or not identities:
        raise ValueError('Automatic mode requires explicit dual-mode identities')
    if not isinstance(decimations,dict) or set(decimations)!= {'stock','frame'}:
        raise ValueError('Automatic mode requires both profile decimations')
    if type(poll_period_us) not in (int,float):
        raise ValueError('Automatic switch requires a common station poll period')
    for name,period,delay in (('stock',4727.5,3934.5),('frame',3000.,2207.)):
        n=decimations[name]
        if type(n) is not int or not 0<=n<=40 or not n*period+delay<=poll_period_us<=(n+1)*period:
            raise ValueError('Station poll period must fit BOTH Stock and Frame receive windows')


def poll_decimation(per_tracker_us):
    """Command16 N so the tracker's RX reopens [N*4727.5+3538,(N+1)*4727.5]us
    after a poll and contains our per-tracker poll period (FW189 0x6100/0x6cac).
    The tracker then neither misses our polls nor transmits IMU on its own
    between them (autonomous TX at random times disturbed the Valve controllers)."""
    n=int((per_tracker_us-TRACKER_RX_REOPEN_US)//TRACKER_PERIOD_US)
    if not 0<=n<=40 or not n*TRACKER_PERIOD_US+TRACKER_RX_REOPEN_US<=per_tracker_us<=(n+1)*TRACKER_PERIOD_US:
        raise ValueError(f'Per-tracker poll period {per_tracker_us}us is outside every tracker RX window')
    return n


def load_registry(path):
    raw=json.loads(Path(path).read_text())
    if isinstance(raw,list):
        raw={'version':2,'peers':[{'chip_id_le':value,'slot':slot,'bond_state':'saved'}
                                 for slot,value in enumerate(raw)]}
    if raw.get('version')!=2 or not isinstance(raw.get('peers'),list):
        raise ValueError('Invalid ABI4 registry')
    seen=set();slots=set()
    for entry in raw['peers']:
        chip=bytes.fromhex(entry['chip_id_le'])
        if len(chip)!=4 or not any(chip) or chip in seen:raise ValueError('Invalid registry chipID')
        seen.add(chip)
        slot=entry.get('slot')
        if slot is not None:
            if slot not in range(3) or slot in slots:raise ValueError('Invalid registry slot assignment')
            slots.add(slot)
    return raw


def initialize_v4(state,registry,seed,interval_us,priority,steady_interval_us=5000,steady_slot_us=1000,half_us_extra=False,
                  pause_all=False,slot_cycle=7,pause_peers=(),end_margin_us=200,dense_frame=False):
    if len(state)!=2048 or len(seed)!=4 or priority not in (0,1):
        raise ValueError('Invalid ABI4 configuration')
    # The MCU gives the next slot to a just-offered peer, so the ~23ms confirm
    # window bounds the interval, not 3*interval. Keep each tracker's poll
    # period on the stock 4727.5us grid (e.g. 4727.5 or 9455us).
    rtc_diagnostic=(dense_frame and interval_us==steady_interval_us and interval_us in (2989,2600)
                    and steady_slot_us==900 and end_margin_us==100
                    and slot_cycle==1 and tuple(pause_peers)==(1,2) and priority==0)
    if not 3000<=interval_us<=10000 and not rtc_diagnostic:
        raise ValueError('ABI4 interval must be 3000..10000us')
    # Steady state (all three streaming) uses its own cadence. The former fixed
    # 700us/1576us took 44% of air time and starved the Valve controllers.
    if not 500<=steady_slot_us<=2000 or not steady_slot_us+1000<=steady_interval_us<=10000:
        raise ValueError('Invalid ABI4 steady cadence')
    peers=[{'slot':slot,'id_le':None,'base':0,'prefix':int(f'{slot:08b}'[::-1],2)} for slot in range(3)]
    for entry in registry['peers']:
        slot=entry.get('slot')
        if slot is None:continue
        chip=bytes.fromhex(entry['chip_id_le'])
        peers[slot].update(id_le=chip.hex(),base=address(chip,seed))
    for peer in peers:
        slot=peer['slot'];chip=bytes.fromhex(peer['id_le']) if peer['id_le'] else bytes(4)
        # pause_all (diagnostic): MPSL slots are granted but the MCU never keys
        # the radio, separating timeslot scheduling from RF transmission.
        struct.pack_into('<8I',state,1504+32*slot,int.from_bytes(chip,'little'),peer['base'],peer['prefix'],0,0,0,0,
                         int(bool(pause_all) or slot in pause_peers))
    # MCU reply timeout in 64MHz DWT cycles: 3s (sparse replies on onboard Roy).
    struct.pack_into('<III',state,1608,200,192_000_000,priority)
    state[1856:1860]=seed
    struct.pack_into('<II',state,1860,steady_interval_us,steady_slot_us)
    struct.pack_into('<I',state,1876,struct.unpack_from('<I',state,36)[0])
    # DATA+1880 is now the host's one-shot slot distance adjustment (phase lock);
    # half_us_extra is kept in the signature for config compatibility only.
    struct.pack_into('<I',state,1880,0)
    state[1884:1920]=bytes(36)
    # Slots cycle: slot k serves peer k for k<3, later slots of the cycle idle.
    if not 1<=slot_cycle<=16:raise ValueError('Invalid slot cycle')
    struct.pack_into('<I',state,1888,slot_cycle)
    if not 100<=end_margin_us<=400:raise ValueError('Invalid slot end margin')
    struct.pack_into('<I',state,1892,steady_slot_us-end_margin_us)  # RX/slot end, us from slot start
    return peers


class StationV4Runtime:
    def __init__(self,path,peers,registry,debug,seed,optics=None):
        self.path=Path(path);self.registry=Path(registry);self.seed=seed;self.debug=debug
        self.saved=load_registry(registry)
        self.peers=[dict(peer,last_imu_unix=None,samples=0,sample_gaps=0,pose_valid=False,
                         battery_percent=None,firmware=None,serial=None,last_counter=None) for peer in peers]
        self.engines=[PeerEngine(bytes.fromhex(p['id_le']),p['slot']) if p['id_le'] else None for p in peers]
        self.decimation=None
        self.writer=MailboxWriter(debug)
        self.reload_requested=False
        self.fault=None
        self.optics=optics
        self.mcu_stages=[0,0,0];self.mcu_search=[None,None,None];self.phase_lock=None;self.slot_cycle=7;self.mcu_acks=[0,0,0];self.mcu_rx=[0,0,0]

    def _save(self):
        tmp=self.registry.with_suffix('.tmp')
        tmp.write_text(json.dumps(self.saved,indent=2)+'\n');tmp.replace(self.registry)

    def _enroll(self,identity):
        if identity in [p['id_le'] for p in self.peers]:return
        slot=next((i for i,p in enumerate(self.peers) if p['id_le'] is None),None)
        if slot is None:
            # Preserve registry history but free a stale runtime slot only when
            # it has been silent for ten seconds; active peers are never replaced.
            slot=next((i for i,p in enumerate(self.peers) if p['last_imu_unix'] is not None
                       and time.time()-p['last_imu_unix']>10),None)
        if slot is None:
            self.fault='All three runtime slots occupied; enrollment waiting for a free/stale peer'
            return
        chip=bytes.fromhex(identity);base=address(chip,self.seed);prefix=int(f'{slot:08b}'[::-1],2)
        for entry in self.saved['peers']:
            if entry.get('slot')==slot:entry['slot']=None
        entry=next((p for p in self.saved['peers'] if p['chip_id_le']==identity),None)
        if entry is None:
            entry={'chip_id_le':identity,'bond_state':'discovered'};self.saved['peers'].append(entry)
        entry['slot']=slot
        # Pause this peer before replacing a stale slot. Commit ID last so an
        # empty discovery slot never offers a torn personal BASE/identity pair.
        location=0x20010c18+1504+slot*32
        old=self.engines[slot]
        if old is not None and self.optics:self.optics.reset_peer(int.from_bytes(old.chip_id_le,'little'))
        self.debug.write(location+28,struct.pack('<I',1))
        # SWD transfers do not halt Roy. Wait for any slot that already took
        # the old profile to end before changing its mutable peer fields.
        time.sleep(.006)
        self.debug.write(location+4,struct.pack('<6I',base,prefix,0,0,0,0))
        # The previous tracker's command/window must never reach the new one.
        self.writer.sent.pop(slot,None);self.writer.publish(slot,0,2,b'\0')
        self.debug.write(location,chip)
        self.peers[slot].update(id_le=identity,base=base,prefix=prefix,last_imu_unix=None,
                                samples=0,last_counter=None,serial=None,firmware=None)
        self.engines[slot]=PeerEngine(chip,slot);self._configure_engine(self.engines[slot])
        # Persist the assignment before the MCU may use it; a failed save leaves
        # the slot paused (fault reported) instead of diverging from the registry.
        try:self._save()
        except OSError as error:
            self.fault=f'Registry save failed; slot {slot} paused: {error}';return
        self.debug.write(location+28,bytes(4))

    def set_poll_period(self,per_tracker_us):
        """Send command16 [N,2] first in every tracker configuration."""
        self.decimation=poll_decimation(per_tracker_us)
        for engine in self.engines:
            if engine:self._configure_engine(engine)

    def enable_dual_mode(self,identities,decimations,*,desired_profile=None,poll_period_us=None,query_schema=None,dense_frame=False):
        if (not isinstance(identities,list) or any(not isinstance(x,str) or len(x)!=8 or x=='00000000'
                or any(c not in '0123456789abcdef' for c in x) for x in identities)
                or len(set(identities))!=len(identities)):
            raise ValueError('Invalid dual-mode tracker identities')
        if identities and (not isinstance(decimations,dict) or set(decimations)!= {'stock','frame'}
                or any(type(n) is not int or not 0<=n<=40 for n in decimations.values())):
            raise ValueError('Explicit stock/frame N values required for dual-mode trackers')
        if query_schema is not None and (type(query_schema) is not int or query_schema not in (1,2)):
            raise ValueError('Mode query schema must be 1 or 2')
        if desired_profile is not None and query_schema==1:
            raise ValueError('Remote mode changes require schema2')
        if dense_frame:
            if (desired_profile is not None or query_schema!=2 or identities!=['OWNER_PRIVATE_ID_REQUIRED']
                    or decimations!={'stock':0,'frame':0} or poll_period_us not in (3000,2989,2600)):
                raise ValueError('Invalid dense Frame capability diagnostic')
        else:
            validate_remote_mode_grid(desired_profile or ('frame' if query_schema==2 else None),identities,decimations,poll_period_us)
        self.dense_frame=dense_frame
        self.dual_mode_ids=set(identities);self.mode_decimations=dict(decimations or {})
        self.desired_mode=desired_profile
        self.mode_query_schema=query_schema
        for engine in self.engines:
            if engine:self._configure_engine(engine)

    def _configure_engine(self,engine):
        if engine.chip_id_le.hex() in getattr(self,'dual_mode_ids',set()):
            engine.mode_required=True
            engine.desired_mode=getattr(self,'desired_mode',None)
            engine.mode_query_schema=getattr(self,'mode_query_schema',None)
            engine.required_query_profile='frame' if getattr(self,'dense_frame',False) else None
            base=tuple(c for c in engine.configuration if c[0]!=16)
            if getattr(self,'imu_mode',None) is not None:
                base=tuple((17,bytes([self.imu_mode])) if c[0]==17 else c for c in base)
            engine.mode_configurations={profile:((16,bytes([n,getattr(self,'decimation_m',2)])),)+base
                                        for profile,n in self.mode_decimations.items()}
            return
        if self.decimation is not None:
            decimation=getattr(self,'decimation_by_id',{}).get(engine.chip_id_le.hex(),self.decimation)
            if type(decimation) is not int or not 0<=decimation<=40:
                raise ValueError('Invalid per-tracker command16 N')
            base=tuple(c for c in engine.configuration if c[0]!=16)
            if getattr(self,'imu_mode',None) is not None:
                # Stock PICO pairs N=3 only with low-power IMU mode 0x81 (logs).
                base=tuple((17,bytes([self.imu_mode])) if c[0]==17 else c for c in base)
            engine.configuration=((16,bytes([decimation,getattr(self,'decimation_m',2)])),)+base

    def _reset_peer(self,slot,engine):
        """One reconnect path: sequences, sample counter, sync timer and alignment."""
        engine.disconnect();engine.state=PeerState.ABSENT
        self.peers[slot]['last_counter']=None
        if self.optics:self.optics.reset_peer(int.from_bytes(engine.chip_id_le,'little'))

    def record(self,item):
        log=self.path.with_name('radio-live-records.jsonl')
        if log.exists() and log.stat().st_size>8*1024*1024:log.replace(log.with_suffix('.previous.jsonl'))
        with log.open('a') as stream:stream.write(json.dumps(item)+'\n')
        if item.get('hardware_crc_valid') is not True or item.get('partial'):return
        try:dma=bytes.fromhex(item['dma_hex'])
        except (KeyError,ValueError):return
        if len(dma)<2 or len(dma)<dma[0]+2:return
        payload=dma[2:2+dma[0]]
        if item['channel'] in (1,37,77,4,78):
            try:
                identity=(decode_bootstrap_record(payload) if len(payload)==12 else decode_identity_response(payload))['device_id_hex']
                if int(identity,16):self._enroll(identity)
            except ValueError:pass
            return
        slot=item.get('profile_index')
        if item['channel']!=2 or type(slot) is not int or slot not in range(3):return
        engine=self.engines[slot]
        if engine is None:return
        if item.get('source_chip_id')!=int.from_bytes(engine.chip_id_le,'little'):
            return
        try:commands=decode_records(payload)
        except ValueError:return
        # A repeated ACK tag is still a real reply to a later transmission.
        # Count retained, checksum-valid replies once per slot, including idle
        # polls. Tag transitions below are a separate transport statistic.
        answered_slot=item.get('slot_number_mod32768')
        peer=self.peers[slot]
        if type(answered_slot) is int and answered_slot!=peer.get('last_answered_slot'):
            peer['last_answered_slot']=answered_slot
            peer['replies']=peer.get('replies',0)+1
        # MCU re-discovery is detected in poll() by its search counter, not by
        # tag0 product-info: an unacknowledged product event repeats under tag0
        # and would otherwise reset a healthy connection in a loop.
        if engine.state in (PeerState.ABSENT,PeerState.RECONNECTING):engine.start()
        pending_command=engine.pending.command if engine.pending else None
        accepted=engine.receive(payload,s1=dma[1],tx_generation=item.get('tx_generation',0),crc_valid=True)
        if accepted and self.optics:
            self.optics.acknowledge(slot,pending_command,item.get('roy_cycles'),engine.acked_generation)
        peer=self.peers[slot]
        for command in commands:
            if command.command==1:
                try:product=decode_product_info(command.body)
                except ValueError:continue
                peer.update(serial=product.serial,firmware=product.firmware,sensor_class_bits=product.sensor_class_bits)
                if self.optics:self.optics.enroll(int.from_bytes(engine.chip_id_le,'little'),product.serial)
                saved=next(p for p in self.saved['peers'] if p['chip_id_le']==peer['id_le'])
                saved.update(product_sn=product.serial,firmware=product.firmware,bond_state='confirmed')
                self._save()
            elif command.command==2 and len(command.body) in (18,24,36,42):
                try:samples=decode_imu_samples(command.body)
                except ValueError:continue
                for sample in samples:
                    counter=sample.counter;previous=peer['last_counter']
                    delta=1 if previous is None else (counter-previous)&255
                    if not 1<=delta<=127:continue
                    peer['last_counter']=counter;peer['sample_gaps']+=delta-1;peer['samples']+=1
                    peer['last_imu_unix']=time.time()
                    if self.optics:self.optics.imu(int.from_bytes(engine.chip_id_le,'little'),sample,item['roy_cycles'])
        self._mailbox(slot,engine)

    def _mailbox(self,slot,engine):
        batch=getattr(self,'_mailbox_batch',None)
        if batch is not None:
            batch[slot]=engine
            return
        if self.optics:self.optics.mailbox(slot,engine,self.writer)
        else:self.writer.publish(slot,*engine.mailbox())

    def record_batch(self,items):
        """Consume a copied ring before one final publication per peer.

        Dense no-optics diagnostic only. ACK and command processing still uses
        actual transmitted tags; intermediate unpublished states cannot be ACKed.
        On failure discard the staged writes and let loader cleanup stop RF.
        """
        if not getattr(self,'dense_frame',False) or self.optics is not None:
            raise ValueError('Record batching requires dense no-optics diagnostic')
        if getattr(self,'_mailbox_batch',None) is not None:
            raise RuntimeError('Nested mailbox batch')
        self._mailbox_batch={}
        try:
            for item in items:self.record(item)
            staged=self._mailbox_batch
        finally:
            self._mailbox_batch=None
        for slot,engine in staged.items():self._mailbox(slot,engine)

    def poll(self):
        if self.optics:self.optics.poll(self.debug,self.engines,self.peers)
        # One SWD transaction (each costs ~6ms on Frame): peers+1504..1600 and
        # per-peer counters +1884..1920 in a single 416-byte read.
        if hasattr(self.debug,'read_many'):
            packed=self.debug.read_many(((0x20010c18+1504,96),(0x20010c18+1880,40)))
            block=packed[:96]+bytes(280)+packed[96:]
        else:
            block=self.debug.read(0x20010c18+1504,416)
        states=struct.unpack_from('<24I',block,0);counters=struct.unpack_from('<9I',block,380)
        if getattr(self,'service27_counters',False):self.radio_tx_end=counters[7]
        if self.phase_lock is not None and getattr(self,'radio_log',None):
            # Live overlap diagnostic: stock BLE activity vs our slot phase.
            try:
                from .ble_phase import sample as ble_sample
            except ImportError:
                from ble_phase import sample as ble_sample
            try:
                rows=[ble_sample(self.debug) for _ in range(getattr(self,'radio_log_samples',1))]
                target=self.phase_lock.target_tick()
                with open(self.radio_log,'a') as stream:
                    for tick,busy,base in rows:stream.write(json.dumps([tick,busy,base,target])+'\n')
            except (RuntimeError,ValueError,OSError):pass
        if self.phase_lock is not None:
            # The MCU locks slots to the BLE quiet window itself every slot;
            # the host only records the last correction it applied (us).
            correction=struct.unpack_from('<i',block,376)[0]
            self.phase_lock.errors=(self.phase_lock.errors+[-correction/1e6])[-20:]
        paused=getattr(self,'stagger_paused',None)
        if paused:
            # Staggered enrollment: release the next paused tracker only once the
            # earlier ones are configured (command16 in force), or after 20s, so
            # several trackers never handshake at once (startup controller freezes).
            slot=min(paused)
            ready=all(self.engines[s] is None or self.engines[s].state in (PeerState.WAIT_SYNC,PeerState.STREAMING)
                      for s in range(slot) if s not in paused)
            if ready or time.monotonic()-self.stagger_since>20:
                self.debug.write(0x20010c18+1504+slot*32+28,bytes(4))
                paused.discard(slot);self.stagger_since=time.monotonic()
        for slot,engine in enumerate(self.engines):
            # Retained replies are counted in record(); ACK tags do not count
            # repeated idle-poll responses and cannot measure radio delivery.
            self.peers[slot].setdefault('replies',0)
            if engine is None:continue
            mcu_stage=states[slot*8+3];search=states[slot*8+5]
            # `search` only advances in MCU discovery/offer slots, so any change
            # after a connection means the MCU went back to discovery, even when
            # the stage dropped and recovered between two host polls.
            rediscovered=self.mcu_search[slot] is not None and search!=self.mcu_search[slot]
            if (self.mcu_stages[slot]>=3 and mcu_stage<3) or (rediscovered and engine.state not in
                    (PeerState.ABSENT,PeerState.RECONNECTING,PeerState.CONFIRMING)):
                self._reset_peer(slot,engine)
            self.mcu_search[slot]=search
            self.mcu_stages[slot]=mcu_stage
            if engine.state==PeerState.ABSENT and mcu_stage>=2:engine.start()
            rx=states[slot*8+4]
            if rx!=self.mcu_rx[slot] and mcu_stage>=3:engine.last_reply=engine.now()
            self.mcu_rx[slot]=rx
            tag=states[slot*8+6]>>16
            if tag!=self.mcu_acks[slot]:
                self.mcu_acks[slot]=tag
                self.peers[slot]['ack_tag_changes']=self.peers[slot].get('ack_tag_changes',0)+1
                command=engine.pending.command if engine.pending else None
                if tag and mcu_stage>=3 and engine.ack_tag(tag) and self.optics:
                    self.optics.acknowledge(slot,command,states[slot*8+4],engine.acked_generation)
            engine.advance()
            if engine.state==PeerState.RECONNECTING:
                self.debug.write(0x20010c18+1504+slot*32+12,bytes(4))
                self._reset_peer(slot,engine)
            self._mailbox(slot,engine)

    def publish(self,slots,running=True):
        now=time.time();trackers=[]
        for p,engine in zip(self.peers,self.engines):
            # MCU 7-slot cycle: each peer polled once per 7 slots.
            polls=slots//self.slot_cycle if isinstance(slots,int) else None
            if getattr(self,'service27_counters',False):
                polls=getattr(self,'radio_tx_end',0) if p['slot']==0 else 0
            p.update(polls=polls,reply_ratio=p.get('replies',0)/polls if polls else None)
            p['reply_ratio_metric']='retained_unique_answered_slots_per_scheduled_slots'
            if getattr(self,'service27_counters',False):
                p['reply_ratio_metric']='retained_unique_answered_slots_per_completed_tx'
            if p['id_le'] is None:continue
            age=None if p['last_imu_unix'] is None else now-p['last_imu_unix']
            rf_age=None if engine.last_reply is None else time.monotonic()-engine.last_reply
            optical=self.optics.status(int.from_bytes(engine.chip_id_le,'little')) if self.optics else {'pose_valid':False}
            if engine.mode_required and (not engine.mode_verified() or
                    (engine.desired_mode is not None and not engine.phase_verified)):
                optical=dict(optical,pose_valid=False)
            trackers.append(dict(p,imu_age_seconds=age,rf_age_seconds=rf_age,
                                 connected=running and rf_age is not None and rf_age<3.0,
                                 radio_mode=engine.mode_profile,radio_mode_verified=engine.mode_verified(),
                                 radio_mode_desired=engine.desired_mode,radio_mode_switch_pending=engine.mode_set_target,
                                 radio_mode_required=engine.mode_required,radio_mode_error=engine.mode_error,
                                 radio_mode_query_failures=engine.mode_query_failures,connection_state=engine.state.value,command_retries=engine.retry_count,
                                 transport_acks=engine.transport_acks,optical=optical))
            # Only a fresh nonce/chip-bound mode response proves custom mode
            # support. Its absence cannot classify FW189 as stock firmware.
            trackers[-1]['custom_mode_supported']=True if engine.mode_verified() else None
            trackers[-1]['mode_capability_evidence']='fresh_nonce_chip_bound_mode_reply' if engine.mode_verified() else None
            trackers[-1]['pose_valid']=running and optical['pose_valid'] and (not engine.mode_required or engine.mode_verified())
        value={'updated_unix':now,'running':running,'slots':slots,'trackers':trackers,
               'poses_valid':running and any(p['pose_valid'] for p in trackers),
               'abi':4,'error':self.fault,
               'phase_lock':None if self.phase_lock is None else self.phase_lock.status()}
        if getattr(self,'service27_counters',False):
            tx=getattr(self,'radio_tx_end',0);rx=getattr(self,'radio_crc_rx',0)
            value['radio_measurement']={'tx_end':tx,'crc_valid_rx':rx,
                'rx_per_tx_end':rx/tx if tx else None,
                'host_ring_dropped':getattr(self,'radio_ring_dropped',0),
                'scope':'single active peer; cumulative counters read sequentially'}
        tmp=self.path.with_suffix('.tmp');tmp.write_text(json.dumps(value));tmp.replace(self.path)
