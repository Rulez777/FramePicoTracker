# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Measure axes from actual sparse gyro observations and optical derivatives.

No gyro gap interpolation or missing counter reconstruction. This calibrator
does not change inertial propagation, age or covariance admission.
"""
import numpy as np
from .imu_calibration import GyroAxisCalibrator,_log,_project,GYRO_RADPS_PER_COUNT,ACCEL_MPS2_PER_COUNT


class SparseGyroAxisCalibrator(GyroAxisCalibrator):
    def _optical_rates(self):
        t=np.array([x[0] for x in self.optical],dtype=np.int64)
        R=np.array([x[1] for x in self.optical]);centers=[];rates=[]
        for i in range(2,len(t)-2):
            window=t[i-2:i+3]
            if not 80_000_000<=window[-1]-window[0]<=160_000_000 or np.max(np.diff(window))>40_000_000:continue
            x=(window-t[i])/1e9;design=np.column_stack((x,np.ones(5)))
            angles=np.array([_log(R[i].T@r) for r in R[i-2:i+3]])
            fit=np.linalg.lstsq(design,angles,rcond=None)[0]
            if np.sqrt(np.mean((angles-design@fit)**2))>.01:continue
            centers.append(t[i]);rates.append(fit[0])
        return np.array(centers,dtype=np.int64),np.array(rates)

    def _quiet_gravity(self,axes,bias,offset):
        t=np.array([s[0]+offset for s in self.samples],dtype=np.int64)
        gyro=(np.array([s[1] for s in self.samples])-bias)*GYRO_RADPS_PER_COUNT
        accel=np.array([s[2] for s in self.samples])*ACCEL_MPS2_PER_COUNT
        ot=np.array([o[0] for o in self.optical],dtype=np.int64)
        R=np.array([o[1] for o in self.optical]);windows=[]
        # Disjoint one-second observed windows, never continuous gyro integrals.
        for start in range(int(t[0]),int(t[-1])-1_000_000_000,1_000_000_000):
            sm=(t>=start)&(t<start+1_000_000_000);om=(ot>=start)&(ot<start+1_000_000_000)
            if sm.sum()<20 or om.sum()<15:continue
            ts=t[sm];ots=ot[om]
            if ts[-1]-ts[0]<800_000_000 or ots[-1]-ots[0]<800_000_000:continue
            if np.max(np.diff(ts))>50_000_000 or np.max(np.diff(ots))>70_000_000:continue
            if np.max(np.linalg.norm(gyro[sm],axis=1))>=.05:continue
            rotations=R[om];reference=rotations[0]
            if max(np.linalg.norm(_log(reference.T@r)) for r in rotations)>.02:continue
            measured=axes@accel[sm].mean(axis=0)
            expected=np.mean([r.T@np.array([0.,9.80665,0.]) for r in rotations],axis=0)
            ratio=float(np.linalg.norm(measured)/np.linalg.norm(expected))
            angle=float(np.degrees(np.arccos(np.clip(measured@expected/(np.linalg.norm(measured)*np.linalg.norm(expected)),-1,1))))
            windows.append((ratio,angle,expected/np.linalg.norm(expected)))
        return windows

    def solve(self):
        if len(self.samples)<300 or len(self.optical)<120:raise ValueError('Insufficient actual sparse calibration observations')
        centers,rates=self._optical_rates()
        if len(centers)<120:raise ValueError('Insufficient measured optical angular derivatives')
        t=np.array([s[0] for s in self.samples],dtype=np.int64)
        gyro=np.array([s[1] for s in self.samples]);candidates=[]
        for offset in range(-25_000_000,25_000_001,1_000_000):
            target=centers-offset;idx=np.clip(np.searchsorted(t,target),1,len(t)-1)
            idx=np.where(abs(t[idx]-target)<abs(t[idx-1]-target),idx,idx-1)
            mask=abs(t[idx]-target)<=2_500_000
            X=gyro[idx[mask]]*GYRO_RADPS_PER_COUNT;Y=rates[mask]
            if len(X)<240:continue
            # Temporal thirds provide a held-out block, not neighbors reused as
            # training targets. Both blocks must independently excite all axes.
            cut=int(len(X)*.7);train=np.arange(len(X))<cut;test=~train
            design=np.column_stack((X,np.ones(len(X))));keep=train.copy()
            for _ in range(4):
                fit=np.linalg.lstsq(design[keep],Y[keep],rcond=None)[0]
                error=np.linalg.norm(design@fit-Y,axis=1)
                keep=train&(error<=np.quantile(error[train],.85))
            linear=fit[:3].T;singular=np.linalg.svd(linear,compute_uv=False)
            axes=_project(linear)
            bias=-np.linalg.solve(linear,fit[3])/GYRO_RADPS_PER_COUNT
            proper_prediction=(X-bias*GYRO_RADPS_PER_COUNT)@axes.T
            residual=np.linalg.norm(proper_prediction[test]-Y[test],axis=1)
            rms=float(np.sqrt(np.mean(residual**2)))
            excitation=[np.linalg.svd(Y[part]-Y[part].mean(axis=0),compute_uv=False) for part in (train,test)]
            windows=self._quiet_gravity(axes,bias,offset)
            separation=max((float(np.degrees(np.arccos(np.clip(a[2]@b[2],-1,1)))) for a in windows for b in windows),default=0.)
            reasons=[]
            if np.linalg.det(linear)<=0 or np.any(abs(singular-1)>.03):reasons.append('Unconstrained axes fit is not a proper unit-scale rotation')
            if any(e[1]<3 or e[2]<1.5 for e in excitation):reasons.append('Insufficient independent three-axis excitation')
            if rms>.05 or np.quantile(residual,.95)>.1:reasons.append('Held-out proper rotation angular-rate residual exceeded')
            if np.linalg.norm(bias)*GYRO_RADPS_PER_COUNT>=.05:reasons.append('Measured gyro bias exceeded')
            if abs(offset)>=25_000_000:reasons.append('Time offset at search boundary')
            if len(windows)<2 or separation<15:reasons.append('Insufficient independently observed stationary gravity orientations')
            if any(not .97<=ratio<=1.03 or angle>5 for ratio,angle,_ in windows):reasons.append('Measured gravity scale or direction mismatch')
            result={'axes':axes,'gyro_bias_counts':bias,'time_offset_ns':offset,
                'residual_rms_rad':None,'heldout_angular_rate_rms_rad_s':rms,
                'heldout_angular_rate_p95_rad_s':float(np.quantile(residual,.95)),
                'n_pairs':len(X),'excitation':float(min(e[-1] for e in excitation)),
                'excitation_singular_values':np.array(excitation),
                'unconstrained_scale_singular_values':singular.tolist(),
                'stationary_windows':len(windows),'gravity_orientation_separation_deg':separation,
                'accel_scale_ratio':float(np.mean([v[0] for v in windows])) if windows else None,
                'gravity_angle_deg':max((v[1] for v in windows),default=None),
                'verified':not reasons,'rejection_reasons':reasons,
                'method':'Actual sparse gyro nearest<=2.5ms vs measured optical derivative; no missing-sample integration'}
            candidates.append(result)
        if not candidates:raise ValueError('Insufficient actual time-matched sparse pairs')
        valid=[r for r in candidates if r['verified']]
        return min(valid or candidates,key=lambda r:r['heldout_angular_rate_rms_rad_s'])
