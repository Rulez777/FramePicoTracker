# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""One camera/optics owner for ABI4. Heavy optical work never blocks SWD polling.

Clock, radio connection and optical identity have independent evidence gates.
No recorded-history pose, estimated battery or default inertial axes is used.
IMU samples reach fusion only after per-session sync-timer alignment and an
optical body-axes/bias calibration have both been verified for that tracker.
"""
import json
import queue
import struct
import threading
import time
import math
from pathlib import Path
import numpy as np
from .live_source import LiveCameraSource
from .measurement_clock import CameraClock,RoyClock,SampleClock
from .frame_camera_rig import FrameCameraRig
from .measured_pipeline import MeasuredPosePipeline
from .exposure_schedule import ExposureSchedule
from .temporal_identity import TemporalIdentity
from .phase_search import PhaseSearch
from .imu_calibration import (SyncTimeResolver,GyroAxisCalibrator,GYRO_RADPS_PER_COUNT,
                              ACCEL_MPS2_PER_COUNT)
try:
    from ..driver.pose_frame import NativePosePublisher
except ImportError:
    from driver.pose_frame import NativePosePublisher

IMU_SCALE={'gyro_radps_per_count':GYRO_RADPS_PER_COUNT,'accel_mps2_per_count':ACCEL_MPS2_PER_COUNT}


class StationOptics:
    def __init__(self,config,guard):
        required=('camera_clock_file','factory_camera_file','factory_led_file','dataset_root','phase_at_exposure_ticks')
        if not all(key in config for key in required):raise ValueError('Incomplete explicit optics configuration')
        self.config=config;self.guard=guard
        self.boot=Path('/proc/sys/kernel/random/boot_id').read_text().strip()
        # Clock evidence is per boot. A missing/old file is re-measured in start()
        # from the live recording instead of blocking the station after reboot.
        self.clock=None
        try:self.clock=CameraClock(json.loads(Path(config['camera_clock_file']).read_text()),self.boot)
        except (OSError,ValueError,KeyError):pass
        self.roy_clock=RoyClock();self.sample_clocks={};self.resolvers={}
        # Phase is measured in-session ('search') or resumed from an earlier lock;
        # either way it must re-prove lit own frames before it is trusted.
        self.phase_file=Path(config['phase_file']) if config.get('phase_file') else None
        start=config['phase_at_exposure_ticks']
        if self.phase_file and self.phase_file.exists():
            start=json.loads(self.phase_file.read_text())['phase_at_exposure_ticks']
        if start=='search':start=0
        # Sparse onboard sync: ~30s per 0.4ms step was too slow; scan in 1ms
        # steps (LED pulse vs <1ms exposure) with shorter settle/evaluation.
        self.phase_search=PhaseSearch(start,step=int(config.get('phase_step_ticks',16000)),
                                      settle_frames=int(config.get('phase_settle_frames',6)),
                                      window_frames=int(config.get('phase_window_frames',12)))
        self.persisted_phase=None
        self.phase_search.require_all=bool(config.get('phase_require_all_peers',False))
        self.imu_timebase=config.get('imu_timebase','envelope')
        if self.imu_timebase not in ('envelope','sync'):raise ValueError('imu_timebase must be envelope or sync')
        self.schedule=ExposureSchedule(self.clock,self.roy_clock,start)
        self.source=LiveCameraSource(config['dataset_root'])
        self.pipeline=None;self.publisher=None;self.thread=None;self.stop=threading.Event()
        self.lock=threading.Lock();self.statuses={};self.connections={};self.enrollment=queue.Queue(maxsize=3)
        self.failure=None;self.last_clock_poll=0;self.plans={};self.attempt_plans={};self.verified_slots={}
        self.identity=TemporalIdentity();self.identity_lock=threading.Lock();self.active_chips=()
        self.phase_attempts={};self.observation_log=Path(config.get('optical_log','optical-live.jsonl'))
        self.imu_queue=queue.Queue(maxsize=4096);self.resets=queue.Queue()
        self.calibrators={};self.calibration={};self.solving={};self.last_solve={}
        self.stage_timing={'candidate_ms':0.0,'camera_ms':0.0}

    def start(self):
        self.guard.check(require_now=True)
        try:
            dataset=self.source.start()
            if self.clock is None:
                self.clock=CameraClock(self._measure_clock(dataset),self.boot)
                self.schedule.camera_clock=self.clock
            # XRService writes calibration/config files shortly after a new
            # recording starts; wait briefly instead of failing the station.
            until=time.monotonic()+10
            while time.monotonic()<until and not ((dataset/'sensorRigCalibration.json').exists()
                                                  and list(dataset.glob('*_device_config.json'))):
                time.sleep(.1)
            time.sleep(.2)
            active=json.loads((dataset/'sensorRigCalibration.json').read_text())
            device_files=list(dataset.glob('*_device_config.json'))
            if len(device_files)!=1:raise ValueError('Dataset device_config missing or ambiguous')
            device=json.loads(device_files[0].read_text())
            factory=json.loads(Path(self.config['factory_camera_file']).read_text())
            rig=FrameCameraRig(factory,active,device)
            self.publisher=NativePosePublisher()
            self.pipeline=MeasuredPosePipeline(rig,self.clock,self.guard,Path(self.config['factory_led_file']),self.publisher)
            self.thread=threading.Thread(target=self._work,daemon=True);self.thread.start()
        except BaseException:self.close();raise

    def _restart_camera(self):
        old=self.source
        try:old.close()
        except Exception:pass
        while not self.stop.is_set():
            source=LiveCameraSource(self.config['dataset_root'])
            try:
                source.start();self.source=source;break
            except Exception as error:
                self._log({'camera_restart_failed':repr(error)})
                try:source.close()
                except Exception:pass
                self.stop.wait(5)
        self.camera_restarting=False

    def _measure_clock(self,dataset,seconds=3.):
        """Same causal rule as tests/audit_live_camera_clock.py, for this boot.

        Source time must be causal (0..250ms old at read) in exactly one domain:
        RAW. If MONOTONIC or BOOTTIME also fit, the domain is ambiguous: refuse.
        """
        samples=[];settle=time.monotonic()+1.;until=settle+seconds
        while time.monotonic()<until:
            if self.source.failure:raise RuntimeError('Live camera source failed') from self.source.failure
            try:metadata=self.source.metadata.get(timeout=.05)
            except queue.Empty:continue
            stamp=round(metadata['timestamp_raw']*1e9)
            # The first second after a (re)started recording replays a backlog.
            if time.monotonic()<settle:continue
            samples.append({'raw':time.clock_gettime_ns(time.CLOCK_MONOTONIC_RAW)-stamp,
                            'mono':time.monotonic_ns()-stamp,
                            'boottime':time.clock_gettime_ns(time.CLOCK_BOOTTIME)-stamp})
        if len(samples)<40:raise RuntimeError('Too few live camera timestamps to verify the clock domain')
        # Fraction of reads that are causal (0..250ms old) per domain. RAW must
        # dominate; the other domains must clearly fail, otherwise refuse.
        causal={key:sum(0<=s[key]<=250_000_000 for s in samples)/len(samples) for key in ('raw','mono','boottime')}
        # Shortly after boot MONOTONIC/BOOTTIME still coincide with RAW, so they
        # can look causal too. XRService's RAW domain was proven uniquely on
        # boot a7b75755 (2026-10-06); that is a software property. Here RAW
        # must still hold; the ambiguity is recorded, not treated as failure.
        if causal['raw']<.9:
            raise RuntimeError(f'Camera clock domain not uniquely RAW this boot: {causal}')
        evidence={'version':1,'boot_id':self.boot,'verified':True,'source_domain':'CLOCK_MONOTONIC_RAW',
                  'offset_ns':0,'uncertainty_ns':500_000,
                  'method':'Station start: RAW uniquely causal over live recording; RAW-to-MONO bridged per conversion',
                  'dataset':str(dataset),'samples':len(samples),'causal_fraction':causal,
                  'domain_basis':'RAW causal now; uniquely proven for XRService on boot a7b75755',
                  'difference_bounds_ns':{key:[min(s[key] for s in samples),max(s[key] for s in samples)]
                                          for key in ('raw','mono','boottime')}}
        try:Path(self.config['camera_clock_file']).write_text(json.dumps(evidence,indent=2)+'\n')
        except OSError:pass
        return evidence

    def enroll(self,chip_id,serial):
        if chip_id not in self.sample_clocks:
            self.sample_clocks[chip_id]=SampleClock();self.resolvers[chip_id]=SyncTimeResolver()
            self.enrollment.put_nowait((chip_id,serial))

    def reset_peer(self,chip_id):
        """Radio session ended: counters, sync-timer phase and time alignment restart."""
        clock=self.sample_clocks.get(chip_id)
        if clock:clock.reset()
        resolver=self.resolvers.get(chip_id)
        if resolver:resolver.reset()
        for key in [key for key,plan in self.plans.items() if plan.get('chip')==chip_id]:
            self.plans.pop(key,None);self.schedule.lights.pop(key,None);self.schedule.last_sync.pop(key,None)
        self.resets.put(chip_id)

    def _log(self,row):
        try:
            if self.observation_log.exists() and self.observation_log.stat().st_size>8*1024**2:
                self.observation_log.replace(self.observation_log.with_suffix('.previous.jsonl'))
            with self.observation_log.open('a') as stream:stream.write(json.dumps(row)+'\n')
        except OSError:pass  # Diagnostics must never stop tracking.

    def _work(self):
        try:
            while not self.stop.is_set():
                while not self.enrollment.empty():
                    chip,serial=self.enrollment.get_nowait();self.pipeline.enroll(chip,serial)
                    self.calibrators.setdefault(chip,GyroAxisCalibrator())
                while not self.resets.empty():
                    chip=self.resets.get_nowait()
                    if chip in self.calibrators:self.calibrators[chip].reset()
                    self.pipeline.set_imu_time(chip,False)
                self._drain_imu()
                try:frame=self.source.frames.get(timeout=.01)
                except queue.Empty:frame=None
                if frame is not None:self.stage_timing['frames_in']=self.stage_timing.get('frames_in',0)+1
                with self.lock:connections=dict(self.connections);active_chips=self.active_chips
                if frame is not None and self.schedule.origin_raw is not None:
                    index=self.schedule.frame_index(frame.source_metadata)
                    chip=active_chips[index%len(active_chips)] if active_chips else None
                    if chip in self.pipeline.peers:
                        with self.identity_lock:verified=self.identity.verified(chip,index)
                        if not verified:
                            t0 = time.monotonic()
                            candidate=self.pipeline.candidate(chip,frame)
                            self.stage_timing['candidate_ms'] = (time.monotonic() - t0) * 1000.0
                            if candidate is not None:
                                position=None if candidate['world'] is None else candidate['world'][:3,3]
                                with self.identity_lock:
                                    self.identity.observe(chip,index,candidate['camera'],position)
                                    self.verified_slots=dict(self.identity.bindings)
                                if index not in self.schedule.dark_frames.values():
                                    self.phase_search.observe(index,candidate['inliers']>=6,
                                        identity_verified=bool(self.verified_slots),active_chips=active_chips)
                                self._log({'scheduled_chip_id':chip,'frame_index':index,'camera':candidate['camera'],
                                           'source_time_ns':candidate['stamp_ns'],'inliers':candidate['inliers'],
                                           'residual_px':candidate['residual_px'],'blobs':candidate.get('blobs'),'pixels':candidate.get('pixels'),
                                           'phase':self.schedule.phase,'identity_verified':False,
                                           'world_position_candidate':None if position is None else position.tolist()})
                        else:
                            t0 = time.monotonic()
                            dark=index in self.schedule.dark_frames.values()
                            measured=self.pipeline.camera(chip,frame,identity_verified=True,dark_frame=dark)
                            self.stage_timing['camera_ms'] = (time.monotonic() - t0) * 1000.0
                            if not dark:
                                self.phase_search.observe(index,measured is not None,identity_verified=True,
                                                          active_chips=active_chips)
                            if measured is not None:
                                peer=self.pipeline.peers[chip]
                                try:self.calibrators[chip].add_optical(peer.anchor_ns,peer.last_optical_world[:3,:3])
                                except (KeyError,ValueError):pass
                self._calibrate()
                self.pipeline.expire(connections,allowed=self.verified_slots)
                with self.lock:
                    self.statuses={chip:dict(self.pipeline.status(chip),temporal_identity_verified=chip in self.verified_slots,
                                             imu_calibration=self.calibration.get(chip,{}).get('summary'),
                                             imu_sync=self.resolvers[chip].stats if chip in self.resolvers else None,
                                             phase_search=self.phase_search.status(),
                                             sync={'lights':dict(self.schedule.lights),'phase_acks':dict(self.phase_attempts),
                                                   'plans':len(self.plans),'why':dict(getattr(self,'prepare_reasons',{}))},
                                             worker_timing=dict(self.stage_timing,
                                                 source_dropped=getattr(self.source,'dropped',None),
                                                 uptime_s=round(time.monotonic()-self.stage_timing.setdefault('t_start',time.monotonic()),1)),
                                             timing={'roy_fit':self.roy_clock.fit is not None,
                                                     'roy_slope':None if self.roy_clock.fit is None else self.roy_clock.fit[2],
                                                     'roy_uncertainty_ns':getattr(self.roy_clock,'uncertainty',None),
                                                     'swd_read_ns':getattr(self.roy_clock,'last_read_ns',None),
                                                     'swd_rejected':getattr(self.roy_clock,'rejected',0),
                                                     'camera_intervals':len(self.schedule.intervals),
                                                     'camera_period_s':self.schedule.period})
                                   for chip in self.pipeline.peers}
                    for chip,status in self.statuses.items():
                        status['pose_valid']=status['pose_valid'] and chip in self.verified_slots
        except Exception as error:self.failure=error

    def _drain_imu(self):
        for _ in range(512):
            try:chip,stamp,gyro,accel,counter,time_verified=self.imu_queue.get_nowait()
            except queue.Empty:return
            calibrator=self.calibrators.get(chip)
            if calibrator is not None and time_verified:
                try:calibrator.add_imu(stamp,gyro,accel,counter)
                except ValueError:pass
                calibrator.samples=calibrator.samples[-12000:];calibrator.optical=calibrator.optical[-1800:]
            if chip in self.pipeline.peers:
                self.pipeline.set_imu_time(chip,time_verified and chip in self.calibration
                                           and self.calibration[chip].get('applied',False))
                if time_verified:self.pipeline.imu(chip,stamp,gyro,accel)

    def _calibrate(self):
        now=time.monotonic()
        for chip,calibrator in self.calibrators.items():
            done=self.calibration.get(chip,{})
            if done.get('applied'):continue
            result=done.get('result')
            if result is not None:
                done['summary']={k:v for k,v in result.items() if k not in ('axes','gyro_bias_counts','excitation_singular_values')}
                if result['verified']:
                    try:
                        self.pipeline.configure_imu(chip,result['axes'],IMU_SCALE,axes_verified=True,
                            time_alignment_verified=True,gyro_bias_counts=result['gyro_bias_counts'],
                            time_offset_ns=int(result['time_offset_ns']))
                        done['applied']=True
                    except ValueError as error:done['summary']['rejected']=str(error)
                self._log({'imu_calibration_chip_id':chip,**done['summary']});done['result']=None
            if self.solving.get(chip) or len(calibrator.optical)<120 or now-self.last_solve.get(chip,0)<15:continue
            self.last_solve[chip]=now
            snapshot=GyroAxisCalibrator();snapshot.optical=list(calibrator.optical);snapshot.samples=list(calibrator.samples)
            self.solving[chip]=True
            threading.Thread(target=self._solve,args=(chip,snapshot),daemon=True).start()

    def _solve(self,chip,snapshot):
        # Seconds of numpy on Frame; never inside the optical frame loop.
        try:result=snapshot.solve()
        except (ValueError,np.linalg.LinAlgError):result=None
        finally:self.solving[chip]=False
        if result is not None:self.calibration.setdefault(chip,{})['result']=result

    def acknowledge(self,slot,command,roy_cycles=None,generation=None):
        plan=self.attempt_plans.get((slot,generation)) if generation is not None else None
        if plan is None:return
        key=plan['key']
        if command==14:self.schedule.lights[key]=plan['count']
        if command==10:
            stamp=time.monotonic_ns()
            self.schedule.last_sync[key]=stamp
            self.phase_attempts[key]=self.phase_attempts.get(key,0)+1
            resolver=self.resolvers.get(plan['chip'])
            if resolver is not None and not resolver.acknowledged and plan['phase']==self.schedule.phase:
                resolver.reset(self.schedule.phase,acknowledged=True)
            if roy_cycles is None:return
            try:stamp,_=self.roy_clock.convert(roy_cycles)
            except ValueError:return
            metadata=self.schedule.last_metadata
            try:exposure=self.clock.convert(metadata);index=self.schedule.frame_index(metadata)
            except ValueError:return
            self.phase_search.acked(plan['chip'],plan['phase'],index,self.active_chips)
            first=index+max(0,math.ceil((stamp+16_500_000-exposure)/(self.schedule.period*1e9)))
            remainder=plan['position']
            while first%plan['count']!=remainder:first+=1
            self.schedule.dark_frames[key]=first
            # Identity binds to the chip the plan was made for; TemporalIdentity
            # ignores it if peer membership changed since planning.
            with self.identity_lock:self.identity.sync(plan['chip'],plan['count'],remainder,first)

    def mailbox(self,slot,engine,writer):
        kwargs={};pending=engine.pending;plan=self.plans.get(slot)
        if pending is not None and plan and pending.command in (10,14):
            target=plan['phase_target_ns'] if pending.command==10 else plan['light_target_ns']
            if getattr(self,'config',{}).get('timed_windows',True) and time.monotonic_ns()>target+12_000_000:
                # A denied MPSL slot must not leave an obsolete timed mailbox
                # permanently queued. Retry the same command sequence in its next
                # own frame under a new RF attempt tag mapped to the new plan.
                try:newer=self.schedule.prepare(plan['position'],plan['count'],time.monotonic_ns(),
                                                self._phase_adjust(slot,engine),key=slot)
                except ValueError:newer=None  # Clock momentarily unusable: skip, never crash.
                # None keeps the expired window: the MCU then sends an empty poll
                # and the engine's bounded delivery timeout handles the peer.
                if newer is not None:
                    newer['chip']=plan['chip'];newer['phase']=self.schedule.phase;plan=newer;self.plans[slot]=plan;engine.retime()
        # engine.mailbox() may allocate a fresh tag (S1 change); map the plan to
        # the tag actually published, or its ACK would find no plan (2026-10-07:
        # command14 was never credited and trackers timed out every ~25s).
        published=engine.mailbox()
        if pending is not None and plan and pending.command in (10,14):
            for tag in {published[0]&0xffff,pending.generation&0xffff}:
                self.attempt_plans.setdefault((slot,tag),plan)
            while len(self.attempt_plans)>48:self.attempt_plans.pop(next(iter(self.attempt_plans)))
        if pending is not None and plan:
            # With sparse BLE-safe polling (each tracker every ~42ms) an 11ms TX
            # window is almost never hit. The MCU re-derives command10's tick at TX
            # from phase_reference, so the window is optional (timed_windows).
            windows=getattr(self,'config',{}).get('timed_windows',True)
            if pending.command==10:
                kwargs={'phase_reference':plan['reference']}
                if windows:kwargs['cycle_window']=plan['phase_window']
            elif pending.command==14 and windows:kwargs={'cycle_window':plan['light_window']}
        writer.publish(slot,*published,**kwargs)

    def poll(self,debug,engines,peers):
        if self.failure:raise RuntimeError('Optical worker failed') from self.failure
        if self.thread is not None and not self.thread.is_alive() and not self.stop.is_set():
            raise RuntimeError('Optical worker exited unexpectedly')
        if self.source.failure and not getattr(self,'camera_restarting',False):
            # XRService can stop its recording (HMD idle etc.). Losing cameras
            # must not take the radio down: restart the source in the background.
            self.camera_restarting=True;self.camera_failures=getattr(self,'camera_failures',0)+1
            self._log({'camera_source_failure':repr(self.source.failure),'count':self.camera_failures})
            threading.Thread(target=self._restart_camera,daemon=True).start()
        now=time.monotonic_ns()
        if now-self.last_clock_poll>50_000_000:
            before=time.monotonic_ns();cycles=struct.unpack('<I',debug.read(0xe0001004,4))[0]
            after=time.monotonic_ns();self.roy_clock.observe(cycles,before,after);self.last_clock_poll=now
        # Observe every queued exposure in order: consecutive frames feed the
        # period statistics even when the host loop is slower than 30Hz.
        while not self.source.metadata.empty():
            metadata=self.source.metadata.get_nowait()
            if metadata['stream_key'][2]!=0:continue
            try:self.schedule.observe(metadata)
            except ValueError:pass  # Stale exposure: counted, not a timing reference.
        if self.phase_search.phase!=self.schedule.phase:
            # New scan step: every peer must take command10 with it promptly;
            # sample-timer alignment from the old phase is void.
            self.schedule.phase=self.phase_search.phase;self.schedule.last_sync.clear()
            for resolver in self.resolvers.values():resolver.reset()
        status=self.phase_search.status()
        # Persist every step, not only a lock: a scan takes ~20 min with sparse
        # onboard sync, and a restart must resume instead of starting from 0.
        if self.phase_file and self.persisted_phase!=status['phase_ticks']:
            try:
                self.phase_file.write_text(json.dumps({'phase_at_exposure_ticks':status['phase_ticks'],
                    'state':status['state'],'lit_range':status['lit_range'],
                    'method':'In-session scan; lit own frames by LED-model fit'})+'\n')
                self.persisted_phase=status['phase_ticks']
            except OSError:pass
        active=[i for i,e in enumerate(engines) if e and e.state.value in ('waiting_camera_sync','streaming')
                and (not e.mode_required or e.mode_verified())]
        active_chips=tuple(int.from_bytes(engines[slot].chip_id_le,'little') for slot in active)
        if active_chips!=self.active_chips:
            # Membership changed: queued timed work carries the old divisor and
            # frame assignment. Drop it and make every peer resend command14.
            for engine in engines:
                if engine:engine.queued=[item for item in engine.queued if item[0] not in (10,14)]
            self.schedule.lights.clear()
        with self.identity_lock:
            self.identity.configure(active_chips);self.verified_slots=dict(self.identity.bindings)
        with self.lock:
            self.active_chips=active_chips
            self.connections={int.from_bytes(e.chip_id_le,'little'):
                e.last_reply is not None and time.monotonic()-e.last_reply<3.0
                and (not e.mode_required or e.mode_verified()) for e in engines if e}
        for position,slot in enumerate(active):
            engine=engines[slot]
            reasons=self.__dict__.setdefault('prepare_reasons',{})
            if engine.pending is not None or engine.queued:
                key=f'busy:{engine.pending.command if engine.pending else "queued"}'
                reasons[key]=reasons.get(key,0)+1;continue
            # A transient clock gap (stale Roy fit, old exposure) skips this cycle;
            # the next poll plans again. It must not take the station down.
            try:plan=self.schedule.prepare(position,len(active),now,self._phase_adjust(slot,engine),key=slot)
            except ValueError as error:
                reasons[str(error)[:40]]=reasons.get(str(error)[:40],0)+1;continue
            if plan is None:reasons['none']=reasons.get('none',0)+1;continue
            plan['chip']=active_chips[position];plan['phase']=self.schedule.phase;self.plans[slot]=plan
            engine.queue_exposure_sync(plan['ticks'],len(active),timing_verified=True,own_frame=plan['light_due'])

    def _phase_adjust(self,slot,engine):
        chip=int.from_bytes(engine.chip_id_le,'little')
        if chip in self.verified_slots:return 0
        # A <=32us deliberate phase change makes the documented skipped pulse
        # observable even when the natural drift is below the ±8-tick deadband.
        # This is calibration within the integrated session, not pose synthesis.
        return 256 if self.phase_attempts.get(slot,0)%2==0 else -256

    def imu(self,chip_id,sample,roy_cycles):
        """Radio thread: align one decoded sample and hand it to the optical worker."""
        clock=self.sample_clocks.get(chip_id);resolver=self.resolvers.get(chip_id)
        if clock is None or resolver is None:return None
        try:
            receive,_=self.roy_clock.convert(roy_cycles)
            coarse=clock.observe(sample.counter,receive)
        except ValueError:return None
        if coarse is None:return None
        exposures=self.schedule.exposure_times()
        # The sync-timer resolver always runs as evidence; 'envelope' (the documented
        # counter/lower-envelope method) is the default fusion timebase. Either way
        # the calibrator measures the remaining offset against optical rotation.
        try:synced=resolver.resolve(sample.sync_ticks,exposures,coarse[0]) if exposures else None
        except ValueError:synced=None
        if self.imu_timebase=='sync':
            if synced is None or not resolver.verified:return None
            stamp=synced
        else:stamp=coarse[0]
        item=(chip_id,stamp,sample.gyro_raw,sample.accel_raw,sample.counter,True)
        try:self.imu_queue.put_nowait(item)
        except queue.Full:
            try:self.imu_queue.get_nowait()
            except queue.Empty:pass
            self.imu_queue.put_nowait(item)
        return stamp

    def status(self,chip_id):
        with self.lock:return dict(self.statuses.get(chip_id,{'pose_valid':False}))

    def close(self):
        self.stop.set()
        if self.thread:self.thread.join(timeout=3)
        alive=self.thread is not None and self.thread.is_alive()
        try:self.source.close()
        finally:
            if alive:raise RuntimeError('Optical worker did not stop; retaining its publisher until worker exits')
            try:
                if self.pipeline:self.pipeline.expire({})
            finally:
                if self.publisher:self.publisher.close()
