# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Timestamped factory-model optical poses and strictly bounded inertial fusion.

Production identity comes from verified optical time division, never blob order.
Only a calibrated world pose is sent to PTP2. Historical acquisition can provide
a solver prior but cannot publish a pose. No default LED model or IMU axes.
"""
from collections import deque
from dataclasses import dataclass,field
import time
import numpy as np
from .blob_detector import BlobDetector
from .pico_factory_calibration import load_led_positions
from .ray_pose_solver import RayPoseSolver,rotation_step
from .frame_camera_rig import world_from_tracker
from .fusion_ekf import OpticalInertialEKF
try:
    from ..coords import rot_matrix_to_quat
    from ..driver.pose_frame import PoseFrame
except ImportError:
    from coords import rot_matrix_to_quat
    from driver.pose_frame import PoseFrame


# Publication freshness matches PoseFrame.MAX_AGE (250ms); decode latency on
# Frame made 150ms reject almost every frame. Phase-search candidates never
# publish, so they tolerate 1s.
POSE_AGE_NS=250_000_000
CANDIDATE_AGE_NS=1_000_000_000


def blob_clusters(pixels,link_px=60.):
    """Single-linkage groups of blob centres (one tracker spans ~30-60px)."""
    pixels=np.asarray(pixels,dtype=float);label=np.arange(len(pixels))
    def root(i):
        while label[i]!=i:label[i]=label[label[i]];i=label[i]
        return i
    close=np.linalg.norm(pixels[:,None]-pixels[None],axis=2)<link_px
    for i,j in zip(*np.nonzero(np.triu(close,1))):label[root(i)]=root(j)
    roots=np.array([root(i) for i in range(len(pixels))])
    return [pixels[roots==r] for r in dict.fromkeys(roots.tolist())]


@dataclass
class MeasuredPeer:
    chip_id: int
    serial: str
    model: np.ndarray
    sequence: int=0
    priors: dict=field(default_factory=dict)
    world: np.ndarray | None=None
    anchor_ns: int | None=None
    state_ns: int | None=None
    quality: float=0.
    velocity: np.ndarray=field(default_factory=lambda:np.zeros(3))
    omega_world: np.ndarray=field(default_factory=lambda:np.zeros(3))
    filter: object=None
    axes: np.ndarray | None=None
    sensor_scale: dict | None=None
    imu_time_verified: bool=False
    last_imu_ns: int | None=None
    optical_count: int=0
    rejected: int=0
    last_optical_world: np.ndarray | None=None
    invalid_sent: bool=False
    gyro_bias: np.ndarray=field(default_factory=lambda:np.zeros(3))
    imu_offset_ns: int=0
    imu_buffer: deque=field(default_factory=lambda:deque(maxlen=64))
    imu_diverged: bool=False


class MeasuredPosePipeline:
    def __init__(self,rig,camera_clock,head_history,factory_led_file,publisher,*,now_ns=time.monotonic_ns):
        self.rig=rig;self.clock=camera_clock;self.head_history=head_history
        self.factory_led_file=factory_led_file;self.publisher=publisher;self.now_ns=now_ns
        self.peers={};self.detector=BlobDetector();self.solvers={}

    def _associate(self,key,peer,bound,pixels):
        """Find this tracker among the blobs. Real Frame frames hold several
        objects (other trackers, controller LEDs) and one tracker side shows
        only ~5 of its 8 LEDs, so search spatial clusters of >=5 blobs, largest
        first (2026-10-07: 3% -> 67% of lit clusters associated offline)."""
        if key not in self.solvers:
            self.solvers[key]=RayPoseSolver(peer.model,bound.intrinsics,max_residual_px=2.5,min_inliers=5)
        solver=self.solvers[key];prior=peer.priors.get(bound.sensor_serial)
        if prior is not None:
            try:return solver.associate(pixels,prior)
            except (ValueError,np.linalg.LinAlgError):pass
        for group in sorted(blob_clusters(pixels),key=len,reverse=True):
            if len(group)<5:break
            try:return solver.associate(group,None)
            except (ValueError,np.linalg.LinAlgError):continue
        return solver.associate(pixels,None)  # Last resort: all blobs together.

    def enroll(self,chip_id,serial):
        if not 0<chip_id<2**32:raise ValueError('Invalid tracker identity')
        if chip_id in self.peers:
            if self.peers[chip_id].serial!=serial:raise ValueError('Tracker serial/identity changed')
            return
        if any(p.serial==serial for p in self.peers.values()):raise ValueError('Duplicate tracker serial')
        self.peers[chip_id]=MeasuredPeer(chip_id,serial,load_led_positions(self.factory_led_file,serial))

    def configure_imu(self,chip_id,axes,scale,*,axes_verified,time_alignment_verified,
                      gyro_bias_counts=(0,0,0),time_offset_ns=0):
        axes=np.asarray(axes,dtype=float);bias=np.asarray(gyro_bias_counts,dtype=float)
        if (axes_verified is not True or time_alignment_verified is not True or axes.shape!=(3,3)
                or not np.allclose(axes.T@axes,np.eye(3),atol=1e-4) or np.linalg.det(axes)<.999
                or not 0<scale.get('gyro_radps_per_count',0)<.01
                or not 0<scale.get('accel_mps2_per_count',0)<.1
                or bias.shape!=(3,) or not np.isfinite(bias).all()
                or np.linalg.norm(bias)*scale['gyro_radps_per_count']>=.05
                or type(time_offset_ns) is not int or abs(time_offset_ns)>25_000_000):
            raise ValueError('IMU fusion needs measured body axes, scale and optical sample-time alignment')
        peer=self.peers[chip_id];peer.axes=axes;peer.sensor_scale=scale;peer.imu_time_verified=True
        peer.gyro_bias=bias;peer.imu_offset_ns=time_offset_ns;peer.imu_buffer.clear()

    def set_imu_time(self,chip_id,verified):
        """Sample-time alignment is per radio session; reconnect revokes it."""
        peer=self.peers.get(chip_id)
        if peer is None:return
        peer.imu_time_verified=verified is True and peer.axes is not None
        if not peer.imu_time_verified:peer.imu_buffer.clear()

    def camera(self,chip_id,frame,*,identity_verified,dark_frame=False):
        peer=self.peers[chip_id]
        if identity_verified is not True or dark_frame or not frame.packet_timestamp_matched:
            peer.rejected+=1;return None
        try:
            stamp=self.clock.convert(frame.source_metadata,self.now_ns())
            bound=self.rig.bind(frame.source_metadata,frame.pixels.shape[1],frame.pixels.shape[0])
            head=self.head_history.head_pose(stamp)
            blobs=self.detector.detect(frame.pixels)
            pixels=np.array([[b.x,b.y] for b in blobs])+bound.pixel_offset
            key=(chip_id,bound.sensor_serial)
            solution=self._associate(key,peer,bound,pixels)
            peer.priors[bound.sensor_serial]=solution.camera_from_tracker
            world=world_from_tracker(head,bound.head_from_camera,solution.camera_from_tracker)
            if self.now_ns()-stamp>POSE_AGE_NS:raise ValueError('Optical computation exceeded age budget')
            if peer.anchor_ns is not None:
                if stamp<=peer.anchor_ns:return None
                dt=(stamp-peer.anchor_ns)/1e9
                distance=np.linalg.norm(world[:3,3]-peer.last_optical_world[:3,3])
                if dt<.25 and distance>max(.05,8*dt):raise ValueError('Optical identity jump exceeds speed gate')
                peer.velocity=(world[:3,3]-peer.last_optical_world[:3,3])/dt if .005<dt<.25 else np.zeros(3)
                if np.linalg.norm(peer.velocity)>8:peer.velocity=np.zeros(3)
            peer.world=world;peer.anchor_ns=peer.state_ns=stamp
            peer.last_optical_world=world.copy();peer.invalid_sent=False
            peer.last_imu_ns=None
            peer.quality=min(1.,solution.inliers/8)*np.exp(-solution.residual_px/2)
            # Reinitialize at the measured exposure to avoid feeding delayed
            # optical data into an already propagated future state.
            peer.filter=OpticalInertialEKF(initial_pos=world[:3,3])
            peer.filter.x[3:6]=peer.velocity
            peer.filter.P[:3,:3]=np.eye(3)*.003**2
            peer.filter.P[3:6,3:6]=np.eye(3)*.2**2
            peer.optical_count+=1;peer.imu_diverged=False
            # Samples newer than the exposure arrived before this delayed optical
            # result. Replay them from the new anchor; publish once at the end.
            for sample in [item for item in peer.imu_buffer if item[0]>stamp]:
                if not self._propagate(peer,*sample):break
            return self._send(peer,peer.state_ns)
        except (ValueError,np.linalg.LinAlgError):
            peer.rejected+=1;return None

    def imu(self,chip_id,sample_ns,gyro_raw,accel_raw):
        """Aligned sample time in; publishes a bounded prediction after an optical anchor."""
        peer=self.peers.get(chip_id)
        if peer is None or peer.axes is None or not peer.imu_time_verified:return None
        sample=(sample_ns+peer.imu_offset_ns,gyro_raw,accel_raw)
        peer.imu_buffer.append(sample)
        if not self._propagate(peer,*sample):return None
        return self._send(peer,peer.state_ns)

    def _propagate(self,peer,sample_ns,gyro_raw,accel_raw):
        now=self.now_ns()
        if (not peer.imu_time_verified or peer.anchor_ns is None or peer.axes is None
                or peer.invalid_sent or peer.imu_diverged
                or not peer.anchor_ns<sample_ns<=now or now-peer.anchor_ns>POSE_AGE_NS
                or sample_ns<=peer.state_ns):return False
        dt=(sample_ns-peer.state_ns)/1e9
        if not 0<dt<=.025:return False
        scale=peer.sensor_scale
        gyro=peer.axes@((np.asarray(gyro_raw,dtype=float)-peer.gyro_bias)*scale['gyro_radps_per_count'])
        accel=peer.axes@np.asarray(accel_raw,dtype=float)*scale['accel_mps2_per_count']
        if (gyro.shape!=(3,) or accel.shape!=(3,) or not np.isfinite(gyro).all()
                or not np.isfinite(accel).all() or np.linalg.norm(gyro)>35 or np.linalg.norm(accel)>320):return False
        rotation=peer.world[:3,:3]@rotation_step(gyro*dt)
        saved=peer.filter.x.copy(),peer.filter.P.copy()
        peer.filter.predict(dt,accel,rot_matrix_to_quat(rotation))
        if np.sqrt(np.max(np.linalg.eigvalsh(peer.filter.P[:3,:3])))>.03:
            # Commit nothing: a rejected step must not be applied twice later.
            peer.filter.x,peer.filter.P=saved;peer.imu_diverged=True;return False
        peer.world[:3,:3]=rotation;peer.world[:3,3]=peer.filter.position;peer.velocity=peer.filter.velocity
        peer.omega_world=rotation@gyro;peer.state_ns=sample_ns
        return True

    def _send(self,peer,stamp):
        peer.sequence+=1
        age=max(0,(stamp-peer.anchor_ns)/150_000_000)
        frame=PoseFrame(peer.chip_id,peer.sequence,stamp,tuple(peer.world[:3,3]),
                        tuple(rot_matrix_to_quat(peer.world[:3,:3])),tuple(peer.velocity),
                        tuple(peer.omega_world),True,True,float(peer.quality*(1-min(age,.99))))
        # PoseFrame's own age check is a final independent publication barrier.
        frame.encode(now_ns=self.now_ns())
        self.publisher.send(frame)
        return frame

    def status(self,chip_id):
        peer=self.peers[chip_id];age=None if peer.anchor_ns is None else (self.now_ns()-peer.anchor_ns)/1e9
        return {'pose_valid':age is not None and age<POSE_AGE_NS/1e9,'optical_age_seconds':age,
                'quality':peer.quality,'optical_measurements':peer.optical_count,
                'optical_rejections':peer.rejected,'imu_axes_verified':peer.axes is not None,
                'imu_time_verified':peer.imu_time_verified,'imu_diverged':peer.imu_diverged}

    def candidate(self,chip_id,frame):
        """Measure an unassigned optical candidate; never publishes to IPC."""
        peer=self.peers[chip_id]
        if not frame.packet_timestamp_matched:return None
        try:
            stamp=self.clock.convert(frame.source_metadata,self.now_ns(),max_age_ns=CANDIDATE_AGE_NS)
            bound=self.rig.bind(frame.source_metadata,frame.pixels.shape[1],frame.pixels.shape[0])
            head=self.head_history.head_pose(stamp)
            blobs=self.detector.detect(frame.pixels)
            pixels=np.array([[b.x,b.y] for b in blobs])+bound.pixel_offset
            key=(chip_id,bound.sensor_serial)
            try:fit=self._associate(key,peer,bound,pixels)
            except (ValueError,np.linalg.LinAlgError):
                if self.now_ns()-stamp>CANDIDATE_AGE_NS:return None
                return {'stamp_ns':stamp,'world':None,'camera':bound.sensor_serial,'inliers':0,'blobs':len(pixels),
                        'pixels':np.round(pixels,1).tolist()[:12],
                        'residual_px':None}
            peer.priors[bound.sensor_serial]=fit.camera_from_tracker
            world=world_from_tracker(head,bound.head_from_camera,fit.camera_from_tracker)
            if self.now_ns()-stamp>CANDIDATE_AGE_NS:return None
            return {'stamp_ns':stamp,'world':world,'camera':bound.sensor_serial,'inliers':fit.inliers,'blobs':len(pixels),
                    'residual_px':fit.residual_px}
        except (ValueError,np.linalg.LinAlgError):return None

    def expire(self,connected,allowed=None):
        now=self.now_ns()
        for peer in self.peers.values():
            if peer.world is None or peer.invalid_sent:continue
            is_connected=bool(connected.get(peer.chip_id,False))
            if is_connected and (allowed is None or peer.chip_id in allowed) and now-peer.anchor_ns<POSE_AGE_NS:continue
            peer.sequence+=1
            self.publisher.send(PoseFrame(peer.chip_id,peer.sequence,now,tuple(peer.world[:3,3]),
                tuple(rot_matrix_to_quat(peer.world[:3,:3])),connected=is_connected,pose_valid=False))
            peer.invalid_sent=True
