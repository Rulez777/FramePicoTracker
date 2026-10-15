# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Explicit single-target OTA adapter for the existing guarded Roy ABI4 loader.

Never selected by a persistent service/config alone: requires --tracker-ota-job.
Boot/recovery/physical coexistence remain unverified until a hardware session.
"""
import hashlib
import json
import re
import struct
import time
from pathlib import Path
try:
    from .station_v4_runtime import StationV4Runtime, load_registry
    from .peer_engine import PeerState
    from ..tracker_ota import OtaSession
    from ..tracker_ota_readiness import require_hardware_ota_clearance, RISK_ACCEPTED_FORKS
    from ..tracker_grid_patch import IMAGE_BYTES, STOCK_SHA256, patch, reverse
    from ..swift_product_info import decode_product_info
    from ..tracker_dual_mode_patch import reverse as reverse_dual
except ImportError:
    from radio.ram.station_v4_runtime import StationV4Runtime, load_registry
    from radio.ram.peer_engine import PeerState
    from radio.tracker_ota import OtaSession
    from radio.tracker_ota_readiness import require_hardware_ota_clearance, RISK_ACCEPTED_FORKS
    from radio.tracker_grid_patch import IMAGE_BYTES, STOCK_SHA256, patch, reverse
    from radio.swift_product_info import decode_product_info
    from radio.tracker_dual_mode_patch import reverse as reverse_dual

DATA = 0x20010c18


def load_job(path, registry, *, preparation_only=False, allow_unverified_fork=False):
    path = Path(path); job = json.loads(path.read_text())
    if job.get('schema') != 1 or job.get('action') not in ('patch', 'stock', 'fork'):
        raise ValueError('Expected an explicit OTA job schema1')
    if job['action']=='fork' and not (preparation_only or allow_unverified_fork is True):
        raise ValueError('Expanded fork is preparation-only; hardware job whitelist unchanged')
    if job.get('source_grid_us') not in (4727.5, 3000):
        raise ValueError('Explicit current tracker cadence required; FW189 version cannot distinguish the patch')
    slot = job.get('slot'); identity = job.get('chip_id_le', '')
    if type(slot) is not int or slot not in range(3) or not re.fullmatch('[0-9a-f]{8}', identity) or identity == '00000000':
        raise ValueError('OTA requires one real LE32 chip identity and assigned slot')
    entries = load_registry(registry)['peers']
    if not any(p.get('slot') == slot and p['chip_id_le'] == identity for p in entries):
        raise ValueError('OTA target does not match the saved registry slot')
    image_path = (path.parent / job['image']).resolve(); image = image_path.read_bytes()
    expected_size=RISK_ACCEPTED_FORKS.get(job.get('sha256')) if job['action']=='fork' else IMAGE_BYTES
    if len(image) != expected_size or hashlib.sha256(image).hexdigest() != job.get('sha256'):
        raise ValueError('OTA image size/hash mismatch')
    if job['action'] == 'stock':
        if job['sha256'] != STOCK_SHA256:raise ValueError('Rollback must use the frozen stock image')
    elif job['action']=='fork':
        if job['sha256'] not in RISK_ACCEPTED_FORKS:
            raise ValueError('Unreviewed expanded fork')
        manifest=json.loads((path.parent/job['manifest']).read_text(encoding='utf-8'))
        reverse_dual(image,manifest)
    else:
        manifest = json.loads((path.parent / job['manifest']).read_text())
        original = reverse(image, manifest); expected, expected_manifest = patch(original)
        if expected != image or manifest != expected_manifest:raise ValueError('OTA candidate is not the reviewed minimal patch')
    result = (path.parent / job['result']).resolve()
    protected={image_path,Path(registry).resolve(),path.resolve()}
    if job.get('manifest'):protected.add((path.parent/job['manifest']).resolve())
    if result in protected:
        raise ValueError('OTA result must be a separate file')
    if result.exists():raise ValueError('OTA result already exists; do not replay a completed/interrupted job')
    return dict(job, image_bytes=image, result_path=result,preparation_only=preparation_only,
                unverified_fork_authorized=False)


def apply_cli_authorization(job, args):
    """Reflash authorization comes only from this invocation's CLI flag, never job JSON."""
    job['stock_reflash_authorized'] = getattr(args, 'authorize_stock_reflash', False) is True
    job['unverified_fork_authorized'] = getattr(args, 'authorize_unverified_fork', False) is True
    job['pause_before_ota_start'] = getattr(args, 'pause_before_ota_start', False) is True
    return job


def preflight(job, config, args):
    """Hardware entry point: fail before loader starts SWD/RF."""
    require_hardware_ota_clearance(job)
    return plan_preflight(job, config, args)


def plan_preflight(job, config, args):
    """Reject unguarded/optical/multi-peer or unknown cadence before SWD starts."""
    if not (getattr(args, 'station_v4', False) and getattr(args, 'station_service', False)):
        raise ValueError('OTA requires the reviewed guarded ABI4 loader')
    if config.get('tracker_dual_mode_ids'):
        raise ValueError('Dual-mode handshake is not integrated with the exclusive OTA adapter')
    if job.get('radio_profile') in ('service27','service36','service38'):
        validate_service27_plan(job,config,args)
        return
    coexist = config.get('coexistence', {})
    if coexist.get('controllers_off') is True:
        # Controllers switched off: no BLE grid to protect, so no phase lock and
        # a 1000us slot whose 800us RX end catches the longest (~530us) replies.
        if config.get('optics_enabled', True) or config.get('ble_phase_lock') or coexist.get('min_controllers') != 0:
            raise ValueError('Controllers-off OTA requires optics off, no BLE phase lock and min_controllers 0')
        if args.slot_us not in (600, 1000) or args.interval_us != 6000 or args.steady_interval_us != 6000 or args.mpsl_priority != 0:
            raise ValueError('Controllers-off OTA uses HIGH 600us or 1000us slots every 6000us')
    else:
        if (config.get('optics_enabled', True) or not config.get('ble_phase_lock')
                or coexist.get('min_controllers', 2) < 2
                or not 0 < coexist.get('freeze_limit_ms', 0) <= 150):
            raise ValueError('OTA requires optics off, BLE phase lock, two controllers and freeze guard <=150ms')
        if args.slot_us != 600 or args.interval_us != 6000 or args.steady_interval_us != 6000 or args.mpsl_priority != 0:
            raise ValueError('OTA retains the reviewed HIGH 600us/6000us RF allocation')
    if config.get('slot_end_margin_us', 200) < 200 or config.get('diagnostic_pause_all'):
        raise ValueError('OTA requires the full 200us end margin and a transmitting target')
    if config.get('slot_cycle', 7) != 7 or config.get('tracker_poll_decimation_n') != 9:
        raise ValueError('Initial OTA must use the saved stock-target cadence: cycle7, N9')
    # Make changes only to the in-memory configuration of this one invocation.
    config['staggered_enroll'] = False
    config['diagnostic_pause_peers'] = [s for s in range(3) if s != job['slot']]
    config['tracker_poll_decimation_n'] = 13 if job['source_grid_us'] == 3000 else 9
    config['tracker_poll_decimation_by_id'] = {}


def validate_service27_plan(job,config,args):
    """Offline profile validation; hardware clearance is still a separate gate."""
    expected=dict(service_poll=True,optics_enabled=False,ble_phase_lock=False,
                  slot_cycle=1,slot_end_margin_us=100,tracker_poll_decimation=True,
                  tracker_poll_decimation_n=0,tracker_poll_decimation_m=2,
                  staggered_enroll=False,diagnostic_pause_all=False,
                  interval_us=4727,steady_interval_us=4727,slot_us=900,mpsl_priority=0)
    if any(config.get(k)!=v for k,v in expected.items()):
        raise ValueError('Service27 preparation requires the exact measured profile')
    coexist=config.get('coexistence',{})
    if (coexist.get('controllers_off') is not True or coexist.get('min_controllers')!=0
            or coexist.get('freeze_limit_ms') is not None):
        raise ValueError('Service27 preparation requires controllers off')
    if (job['slot']!=0 or job['source_grid_us']!=4727.5
            or config.get('diagnostic_pause_peers')!=[1,2]
            or config.get('half_us_extra',False) or config.get('tracker_poll_decimation_by_id')):
        raise ValueError('Service27 measured only stock-cadence slot0, peers1/2 paused')
    if any(getattr(args,k,None)!=v for k,v in
           dict(slot_us=900,interval_us=4727,steady_interval_us=4727,mpsl_priority=0).items()):
        raise ValueError('Effective loader arguments differ from measured service27')
    module_sha=hashlib.sha256(Path(args.module).read_bytes()).hexdigest()
    expected_sha=('44f4e52b07bbba4c459256c910228d440e25ff0d288238e76047fd21000c4c7c'
                  if job.get('radio_profile')=='service38' else '05c3c84f20ba0d07867fd09b70e5d850e4f9d3e13a105e216ffa85b772992604'
                  if job.get('radio_profile')=='service36' else
                  '65d217e5e47e8028a6d58fe46c17923fcd20877dff27ffed64bc4dbf71f64a44')
    if module_sha!=expected_sha:
        raise ValueError('Service27 module differs from hardware-measured ELF')


class TrackerOtaRuntime(StationV4Runtime):
    def __init__(self, *args, ota_job, **kwargs):
        super().__init__(*args, **kwargs)
        self.ota_job = ota_job; self.ota = None; self.ota_slot = ota_job['slot']
        self.connection_deadline = time.monotonic() + 120
        self.target_search = None; self.finish_at = None
        self.prestart_error = None; self.last_status_bytes = None;self.prestart_paused=False
        self.prestart_identity=None
        self.last_ota_publication=None
        self.result = ota_job['result_path']
        # Exclusive result creation prevents replay of the same transfer.
        with self.result.open('x', encoding='utf-8') as stream:
            json.dump(dict(state='waiting_target', chip_id_le=ota_job['chip_id_le'], boot_verified=False), stream)

    def _enroll(self, identity):
        # An OTA job uses the saved target exclusively; never enroll another peer.
        return

    def _mailbox(self, slot, engine):
        if slot != self.ota_slot:return
        if self.ota is not None:
            publication=self.ota.mailbox()
            self.writer.publish(slot, *publication)
            if publication != self.last_ota_publication:
                generation,s1,payload=publication
                with self.result.with_suffix('.tx.jsonl').open('a',encoding='utf-8') as stream:
                    stream.write(json.dumps(dict(unix=time.time(),scope='Host mailbox publication; not RF TX proof',
                        state=self.ota.state,block=self.ota.index,command_sequence=self.ota.sequence,
                        tx_generation=generation,s1=s1,payload_hex=payload.hex()))+'\n')
                self.last_ota_publication=publication
        else:super()._mailbox(slot, engine)

    def _write_status(self):
        value = (self.ota.status() if self.ota is not None else
                 dict(state='failed' if self.prestart_error else 'paused_before_ota_start' if self.prestart_paused else 'waiting_target',
                      error=self.prestart_error, boot_verified=False, cadence_verified=False,
                      ota_start_sent=False,identity=self.prestart_identity))
        value = dict(value, chip_id_le=self.ota_job['chip_id_le'])
        encoded = (json.dumps(value, indent=2) + '\n').encode()
        if encoded == self.last_status_bytes:return
        temporary = self.result.with_suffix(self.result.suffix + '.tmp')
        temporary.write_bytes(encoded);temporary.replace(self.result)
        self.last_status_bytes = encoded

    def _prestart_fail(self, reason):
        self.prestart_error = str(reason);self._write_status()
        raise RuntimeError(self.prestart_error)

    def record(self, item):
        if self.ota is None:return super().record(item)
        if (item.get('hardware_crc_valid') is not True or item.get('partial')
                or item.get('channel') != 2 or item.get('profile_index') != self.ota_slot
                or item.get('source_chip_id') != int.from_bytes(bytes.fromhex(self.ota_job['chip_id_le']), 'little')):return
        try:dma = bytes.fromhex(item['dma_hex'])
        except (ValueError, KeyError):return
        if len(dma) < 3 or len(dma) != dma[0] + 2:return
        previous = (self.ota.state, self.ota.index)
        accepted = self.ota.receive(dma[2:], crc_valid=True, s1=dma[1], tx_generation=item.get('tx_generation', 0))
        # Keep target liveness for the UI without publishing invented poses.
        self.engines[self.ota_slot].last_reply = time.monotonic()
        if accepted or self.ota.state == 'failed':
            self._write_status();self._mailbox(self.ota_slot, self.engines[self.ota_slot])
        if previous != (self.ota.state, self.ota.index):
            with self.result.with_suffix('.events.jsonl').open('a') as stream:
                stream.write(json.dumps(dict(unix=time.time(), **self.ota.status())) + '\n')

    def poll(self):
        try:self._poll()
        except Exception as error:
            if self.ota is None:
                if not self.prestart_error:self.prestart_error = str(error)
            elif not self.ota.terminal:self.ota.abort(f'OTA runtime failure: {error}')
            self._write_status()
            raise

    def _poll(self):
        if self.ota is None:
            if time.monotonic() >= self.connection_deadline:
                self._prestart_fail('OTA target configuration deadline expired')
            super().poll()
            engine = self.engines[self.ota_slot]
            if engine is None or engine.state not in (PeerState.WAIT_SYNC, PeerState.STREAMING):return
            product = decode_product_info(engine.product_info_raw)
            if product.firmware != 189:self._prestart_fail('OTA target does not report native FW189')
            if self.ota_job.get('action')=='fork' and product.serial!='OWNER_SUPPLIED_PRIVATE_VALUE_REQUIRED':
                self._prestart_fail('Live OTA serial differs from explicitly authorized fork target')
            try:require_hardware_ota_clearance(self.ota_job)
            except RuntimeError as error:self._prestart_fail(error)
            if self.ota_job.get('pause_before_ota_start') is True:
                self.prestart_paused=True
                self.prestart_identity=dict(serial=product.serial,firmware=product.firmware,
                                            chip_id_le=self.ota_job['chip_id_le'])
                self._write_status();return
            self.ota = OtaSession(self.ota_job['image_bytes'], self.ota_job['sha256'],
                                  sequence=engine.sequences[1], initial_generation=engine.generation,
                                  # Live ordinary polling exposes empty queue
                                  # barriers sparsely (audit33); allow a bounded
                                  # 2h total while keeping each data/drain at 10s.
                                  overall_timeout_s=7200. if self.ota_job.get('radio_profile') in ('service27','service36','service38') else 1800.,
                                  response_poll=self.ota_job.get('radio_profile') in ('service27','service36','service38'),
                                  capture_ack=self.ota_job.get('radio_profile')=='service38')
            self.target_search = self.mcu_search[self.ota_slot]
            self.ota.begin();self._write_status()
            if self.ota.state == 'failed':raise RuntimeError(self.ota.error)
            self._mailbox(self.ota_slot, engine)
            return
        self.ota.tick()
        block = self.debug.read(DATA + 1504 + 32 * self.ota_slot, 32)
        _, _, _, stage, _, search, _, paused = struct.unpack('<8I', block)
        if stage != 3 or search != self.target_search or paused:
            self.ota.abort('OTA peer disconnected/re-discovered; automatic resume forbidden')
        self._write_status()
        if self.ota.state == 'failed':raise RuntimeError(self.ota.error)
        self._mailbox(self.ota_slot, self.engines[self.ota_slot])
        if self.ota.state == 'finish_verified':
            if self.finish_at is None:self.finish_at = time.monotonic()
            if time.monotonic() - self.finish_at >= 1:self.reload_requested = True

    def publish(self, slots, running=True):
        if not running and self.ota is None and not self.prestart_error and not self.prestart_paused:
            self.prestart_error = 'Loader stopped while waiting for OTA target; automatic resume forbidden'
        if not running and self.ota is not None and not self.ota.terminal:
            self.ota.abort('Loader stopped before finish/application ACK; automatic resume forbidden')
        super().publish(slots, running)
        self._write_status()
