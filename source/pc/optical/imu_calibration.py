# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Offline/measured IMU calibration; documented axes never authorize fusion.

Offset convention: physical sample time = supplied IMU timestamp + offset.
Sync timing assumes an acknowledged, independently calibrated exposure phase.
"""
from collections import deque
from dataclasses import dataclass
import struct
import numpy as np
from .ray_pose_solver import rotation_step,skew

GYRO_RADPS_PER_COUNT=0.002130528911948204
ACCEL_MPS2_PER_COUNT=0.009580078534781933
TICKS_PER_FRAME=533333
TICK_HZ=16e6
DOCUMENTED_AXES=np.array([[0,-1,0],[1,0,0],[0,0,1]],dtype=float)


@dataclass(frozen=True)
class ImuSample:
    gyro_raw: tuple[int,int,int]
    accel_raw: tuple[int,int,int]
    temperature_raw: int
    sync_ticks: int
    counter: int


def decode_imu_samples(body: bytes) -> list[ImuSample]:
    if len(body) not in (6,18,24,36,42):raise ValueError('Malformed command2 body')
    result=[]
    for offset in range(0,(len(body)//18)*18,18):
        values=struct.unpack_from('<7h',body,offset)
        ticks=int.from_bytes(body[offset+14:offset+17],'little')
        if ticks>=TICKS_PER_FRAME:raise ValueError('Invalid frame sync ticks')
        result.append(ImuSample(values[:3],values[3:6],values[6],ticks,body[offset+17]))
    return result


class SyncTimeResolver:
    def __init__(self,phase_at_exposure_ticks=None,*,acknowledged=False,min_samples=200,
                 window=400,typical_delay_ns=10_000_000):
        if min_samples<2 or window<min_samples:raise ValueError('Invalid verification window')
        self.min_samples=min_samples;self.window=window;self.typical_delay_ns=typical_delay_ns
        self.reset(phase_at_exposure_ticks,acknowledged=acknowledged)

    def reset(self,phase_at_exposure_ticks=None,*,acknowledged=False):
        if phase_at_exposure_ticks is not None and (type(phase_at_exposure_ticks) is not int
                or not 0<=phase_at_exposure_ticks<TICKS_PER_FRAME):raise ValueError('Invalid phase')
        self.phase=phase_at_exposure_ticks;self.acknowledged=acknowledged is True
        self.delays=deque(maxlen=self.window);self.verified=False;self.last_ns=None

    @property
    def stats(self):
        data=np.asarray(self.delays,dtype=float)
        median=float(np.median(data)) if len(data) else None
        return {'n_samples':len(data),'median_delay_ns':median,
                'mad_ns':float(np.median(abs(data-median))) if len(data) else None,
                'spread_ns':float(np.ptp(data)) if len(data) else None,'verified':self.verified}

    def resolve(self,sync_ticks: int,exposure_times_ns,coarse_ns: int) -> int | None:
        if type(sync_ticks) is not int or not 0<=sync_ticks<TICKS_PER_FRAME:raise ValueError('Invalid sync ticks')
        if not self.acknowledged or self.phase is None:return None
        exposures=np.asarray(exposure_times_ns,dtype=np.int64)
        if exposures.ndim!=1 or not len(exposures):return None
        if np.any(np.diff(exposures)<=0):raise ValueError('Exposure times must increase')
        delta=((sync_ticks-self.phase+TICKS_PER_FRAME//2)%TICKS_PER_FRAME)-TICKS_PER_FRAME//2
        candidates=exposures+round(delta*1e9/TICK_HZ)
        delays=coarse_ns-candidates
        good=(delays>=0)&(delays<=25_000_000)
        if not np.any(good):self.verified=False;return None
        typical=self.stats['median_delay_ns'] if self.delays else self.typical_delay_ns
        candidates=candidates[good];delays=delays[good]
        index=int(np.argmin(abs(delays-typical)));stamp=int(candidates[index]);delay=int(delays[index])
        if self.last_ns is not None and stamp<=self.last_ns:return None
        if len(self.delays)>=10:
            band=max(1_500_000,6*self.stats['mad_ns'])
            if abs(delay-typical)>band:self.verified=False;return None
        self.delays.append(delay);self.last_ns=stamp
        self.verified=len(self.delays)>=self.min_samples and self.stats['spread_ns']<3_000_000
        return stamp


def _log(matrix):
    """SO(3) logarithm, including rotations close to pi."""
    angle=np.arccos(np.clip((np.trace(matrix)-1)/2,-1,1))
    vector=np.array([matrix[2,1]-matrix[1,2],matrix[0,2]-matrix[2,0],matrix[1,0]-matrix[0,1]])
    if angle<1e-7:return vector/2
    if np.pi-angle<1e-5:
        values,vectors=np.linalg.eigh((matrix+matrix.T)/2)
        axis=vectors[:,np.argmax(values)]
        if axis@vector<0:axis=-axis
        return axis*angle
    return vector*(angle/(2*np.sin(angle)))


def _project(matrix):
    u,_,vt=np.linalg.svd(matrix)
    return u@np.diag([1,1,np.linalg.det(u@vt)])@vt


class GyroAxisCalibrator:
    def __init__(self,*,max_gap_ns=8_000_000):
        self.max_gap_ns=max_gap_ns;self.reset()

    def reset(self):self.optical=[];self.samples=[]

    def add_optical(self,stamp_ns: int,R_world_tracker):
        rotation=np.asarray(R_world_tracker,dtype=float)
        if (rotation.shape!=(3,3) or not np.isfinite(rotation).all()
                or not np.allclose(rotation.T@rotation,np.eye(3),atol=1e-5)
                or np.linalg.det(rotation)<.999):raise ValueError('Invalid optical orientation')
        if self.optical and stamp_ns<=self.optical[-1][0]:raise ValueError('Optical time must increase')
        self.optical.append((stamp_ns,rotation.copy()))

    def add_imu(self,stamp_ns: int,gyro_raw,accel_raw,counter: int):
        gyro=np.asarray(gyro_raw,dtype=float);accel=np.asarray(accel_raw,dtype=float)
        if (gyro.shape!=(3,) or accel.shape!=(3,) or not np.isfinite(gyro).all()
                or not np.isfinite(accel).all() or not 0<=counter<=255):raise ValueError('Invalid IMU sample')
        if self.samples and stamp_ns<=self.samples[-1][0]:raise ValueError('IMU time must increase')
        self.samples.append((stamp_ns,gyro.copy(),accel.copy(),counter))

    def _segments(self,offset):
        times=np.array([s[0] for s in self.samples],dtype=np.int64)+offset
        raw=np.array([s[1] for s in self.samples]);counters=np.array([s[3] for s in self.samples])
        segments=[];targets=[];durations=[]
        for (start,r0),(end,r1) in zip(self.optical,self.optical[1:]):
            if not 20_000_000<=end-start<=200_000_000 or start<times[0] or end>times[-1]:continue
            lo=max(0,np.searchsorted(times,start,side='right')-1)
            hi=min(len(times)-1,np.searchsorted(times,end))
            if (np.any(np.diff(times[lo:hi+1])>self.max_gap_ns)
                    or np.any((np.diff(counters[lo:hi+1])%256)!=1)):continue
            knots=np.r_[start,times[(times>start)&(times<end)],end]
            values=np.column_stack([np.interp(knots,times,raw[:,axis]) for axis in range(3)])
            segments.append(((values[:-1]+values[1:])/2,np.diff(knots)/1e9))
            targets.append(_log(r0.T@r1));durations.append((end-start)/1e9)
        if not segments:return None
        width=max(len(dt) for _,dt in segments);rates=np.zeros((len(segments),width,3));dt=np.zeros((len(segments),width))
        for i,(values,steps) in enumerate(segments):rates[i,:len(steps)]=values;dt[i,:len(steps)]=steps
        return rates,dt,np.array(targets),np.array(durations)

    @staticmethod
    def _integrate(rates,dt,bias):
        # Batch exact Rodrigues products in sensor coordinates; conjugate by A.
        rotations=np.broadcast_to(np.eye(3),(len(rates),3,3)).copy()
        for column in range(dt.shape[1]):
            vectors=(rates[:,column]-bias)*GYRO_RADPS_PER_COUNT*dt[:,column,None]
            angles=np.linalg.norm(vectors,axis=1);k=np.zeros_like(rotations)
            x,y,z=vectors.T;k[:,0,1]=-z;k[:,0,2]=y;k[:,1,0]=z;k[:,1,2]=-x;k[:,2,0]=-y;k[:,2,1]=x
            a=np.sinc(angles/np.pi);b=.5*np.sinc(angles/(2*np.pi))**2
            rotations=rotations@(np.eye(3)+a[:,None,None]*k+b[:,None,None]*(k@k))
        return np.array([_log(r) for r in rotations])

    def _fit(self,offset):
        data=self._segments(offset)
        if data is None:return None
        rates,dt,target,duration=data
        integral=np.sum(rates*dt[:,:,None],axis=1)*GYRO_RADPS_PER_COUNT
        fit=np.linalg.lstsq(np.column_stack((integral,duration)),target,rcond=None)[0]
        axes=_project(fit[:3].T);bias=-axes.T@fit[3]/GYRO_RADPS_PER_COUNT
        for _ in range(3):
            integrated=self._integrate(rates,dt,bias);predicted=integrated@axes.T
            jac_axes=np.array([-skew(p) for p in predicted])
            jac_bias=np.stack([(self._integrate(rates,dt,bias+np.eye(3)[i]*.1)-integrated)@axes.T/.1
                               for i in range(3)],axis=2)
            jac=np.concatenate((jac_axes,jac_bias),axis=2).reshape(-1,6)
            step=np.linalg.lstsq(jac,(target-predicted).ravel(),rcond=None)[0]
            axes=rotation_step(step[:3])@axes;bias+=step[3:]
        predicted=self._integrate(rates,dt,bias)@axes.T
        rms=float(np.sqrt(np.mean(np.sum((target-predicted)**2,axis=1))))
        return rms,axes,bias,target

    def _gravity(self,axes,bias,offset):
        times=np.array([s[0] for s in self.samples])+offset
        quiet=np.array([np.linalg.norm((s[1]-bias)*GYRO_RADPS_PER_COUNT)<.05 for s in self.samples])
        ratios=[];angles=[];start=None
        for i in range(len(times)+1):
            continuous=i<len(times) and quiet[i] and (start is None or
                (times[i]-times[i-1]<=self.max_gap_ns and (self.samples[i][3]-self.samples[i-1][3])%256==1))
            if continuous:
                if start is None:start=i
                continue
            if start is not None and times[i-1]-times[start]>=300_000_000:
                observations=[r for t,r in self.optical if times[start]<=t<=times[i-1]]
                if observations:
                    expected=np.mean([r.T@np.array([0,9.80665,0]) for r in observations],axis=0)
                    measured=axes@np.mean([s[2] for s in self.samples[start:i]],axis=0)*ACCEL_MPS2_PER_COUNT
                    norm=np.linalg.norm(measured);reference=np.linalg.norm(expected)
                    ratios.append(float(norm/reference))
                    angles.append(float(np.degrees(np.arccos(np.clip(measured@expected/max(norm*reference,1e-12),-1,1)))))
            start=i if i<len(times) and quiet[i] else None
        return ratios,angles

    def solve(self) -> dict:
        if len(self.optical)<3 or len(self.samples)<3:raise ValueError('Insufficient calibration samples')
        candidates=[]
        for offset in range(-25_000_000,25_000_001,1_000_000):
            fit=self._fit(offset)
            if fit is not None:candidates.append((fit[0],offset,fit))
        if not candidates:raise ValueError('No continuous optical/IMU intervals')
        _,offset,best=min(candidates,key=lambda v:v[0])
        for refined in range(max(-25_000_000,offset-900_000),min(25_000_000,offset+900_000)+1,100_000):
            fit=self._fit(refined)
            if fit is not None and fit[0]<best[0]:best=fit;offset=refined
        rms,axes,bias,target=best;singular=np.linalg.svd(target,compute_uv=False)
        ratios,angles=self._gravity(axes,bias,offset)
        verified=(len(target)>=60 and singular[1]>=.5 and singular[2]>=.15 and rms<=.01
                  and np.allclose(axes.T@axes,np.eye(3),atol=1e-5) and np.linalg.det(axes)>.999
                  and np.linalg.norm(bias)*GYRO_RADPS_PER_COUNT<.05 and len(ratios)>=2
                  and all(.97<=r<=1.03 for r in ratios) and all(a<=5 for a in angles))
        return {'axes':axes,'gyro_bias_counts':bias,'time_offset_ns':offset,'residual_rms_rad':rms,
                'n_pairs':len(target),'excitation':float(singular[-1]),'excitation_singular_values':singular,
                'angle_to_documented_prior_deg':float(np.degrees(np.linalg.norm(_log(DOCUMENTED_AXES.T@axes)))),
                'stationary_windows':len(ratios),'accel_scale_ratio':float(np.mean(ratios)) if ratios else None,
                'gravity_angle_deg':max(angles) if angles else None,'verified':bool(verified)}
