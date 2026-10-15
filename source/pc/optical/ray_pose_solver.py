# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Measured 6DoF from calibrated KB rays and explicit factory LED geometry.

No invented pinhole intrinsics, gyro quaternion or world position. Unknown
correspondences are searched with bounded geometric hypotheses; ambiguous fits
are rejected instead of assigning one tracker to another by convenience.
"""
from dataclasses import dataclass
from itertools import combinations, permutations
import numpy as np


def skew(value):
    x,y,z=value
    return np.array([[0,-z,y],[z,0,-x],[-y,x,0]],dtype=float)


def rotation_step(vector):
    angle=np.linalg.norm(vector)
    matrix=skew(vector)
    if angle<1e-8:return np.eye(3)+matrix+.5*matrix@matrix
    return np.eye(3)+(np.sin(angle)/angle)*matrix+((1-np.cos(angle))/angle**2)*(matrix@matrix)


@dataclass(frozen=True)
class RayPose:
    camera_from_tracker: np.ndarray
    inliers: int
    residual_px: float
    led_indices: tuple
    observation_indices: tuple
    condition: float


def dlt_pose(model,rays):
    if len(model)<6:raise ValueError('Initial metric pose requires at least six correspondences')
    centre=model.mean(axis=0)
    scale=np.sqrt(np.mean(np.sum((model-centre)**2,axis=1)))
    if scale<1e-6:raise ValueError('Degenerate LED geometry')
    normalized=(model-centre)/scale
    singular=np.linalg.svd(normalized,compute_uv=False)
    if singular[-1]<singular[0]*1e-4:raise ValueError('Initial LED geometry is coplanar/collinear')
    homogeneous=np.column_stack((normalized,np.ones(len(model))))
    equations=(_skews(rays)[...,:,:,None]*homogeneous[:,None,None,:]).reshape(-1,12)
    _,_,v=np.linalg.svd(equations,full_matrices=False)
    matrix=v[-1].reshape(3,4)
    if np.linalg.det(matrix[:,:3])<0:matrix=-matrix
    u,s,vt=np.linalg.svd(matrix[:,:3]);rotation=u@vt
    if np.linalg.det(rotation)<0:raise ValueError('DLT reflected coordinate frame')
    multiplier=np.mean(s)/scale
    translation=matrix[:,3]/multiplier-rotation@centre
    result=np.eye(4);result[:3,:3]=rotation;result[:3,3]=translation
    return result


def _skews(values):
    values=np.asarray(values);result=np.zeros(values.shape[:-1]+(3,3))
    x,y,z=np.moveaxis(values,-1,0)
    result[...,0,1]=-z;result[...,0,2]=y;result[...,1,0]=z
    result[...,1,2]=-x;result[...,2,0]=-y;result[...,2,1]=x
    return result


def _dlt_batch(model,rays):
    centre=model.mean(axis=1);offset=model-centre[:,None,:]
    scale=np.sqrt(np.mean(np.sum(offset**2,axis=2),axis=1))
    valid=scale>=1e-6;normalized=offset/np.maximum(scale[:,None,None],1e-300)
    singular=np.linalg.svd(normalized,compute_uv=False)
    valid&=singular[:,-1]>=singular[:,0]*1e-4
    homogeneous=np.concatenate((normalized,np.ones(model.shape[:2]+(1,))),axis=2)
    equations=(_skews(rays)[...,:,:,None]*homogeneous[...,None,None,:])
    equations=equations.reshape(len(model),model.shape[1]*3,12)
    _,_,v=np.linalg.svd(equations,full_matrices=False)
    matrix=v[:,-1].reshape(-1,3,4)
    matrix*=np.where(np.linalg.det(matrix[:,:,:3])<0,-1.,1.)[:,None,None]
    u,s,vt=np.linalg.svd(matrix[:,:,:3]);rotation=u@vt
    valid&=np.linalg.det(rotation)>=0
    multiplier=np.mean(s,axis=1)/np.maximum(scale,1e-300)
    translation=matrix[:,:,3]/np.maximum(multiplier[:,None],1e-300)-(rotation@centre[:,:,None])[:,:,0]
    poses=np.broadcast_to(np.eye(4),(len(model),4,4)).copy()
    poses[:,:3,:3]=rotation;poses[:,:3,3]=translation
    return poses,valid


def _refine_batch(model,rays,poses,iterations=12):
    result=poses.copy();condition=np.full(len(poses),np.inf)
    valid=np.ones(len(poses),dtype=bool);active=valid.copy()
    ray_skews=_skews(rays);model_skews=_skews(model)
    for _ in range(iterations):
        ids=np.flatnonzero(active)
        if not len(ids):break
        rotation=result[ids,:3,:3];points=model[ids]@rotation.transpose(0,2,1)+result[ids,None,:3,3]
        lengths=np.linalg.norm(points,axis=2)
        good=np.all(lengths>=.04,axis=1)
        valid[ids[~good]]=False;active[ids[~good]]=False
        ids=ids[good];rotation=rotation[good];points=points[good];lengths=lengths[good]
        if not len(ids):continue
        directions=points/lengths[:,:,None]
        residual=(ray_skews[ids]@directions[...,None])[...,0]
        projection=(np.eye(3)-directions[..., :,None]*directions[...,None,:])/lengths[...,None,None]
        jacobian=np.concatenate((-rotation[:,None]@model_skews[ids],
                                np.broadcast_to(np.eye(3),model[ids].shape[:2]+(3,3))),axis=3)
        jacobian=(ray_skews[ids]@projection@jacobian).reshape(len(ids),-1,6)
        norm=np.linalg.norm(residual,axis=2)
        weights=np.repeat(np.minimum(1,.006/np.maximum(norm,1e-12)),3,axis=1)
        weighted=jacobian*np.sqrt(weights[:,:,None]);rhs=-residual.reshape(len(ids),-1)*np.sqrt(weights)
        u,s,vt=np.linalg.svd(weighted,full_matrices=False)
        cond=np.divide(s[:,0],s[:,-1],out=np.full(len(ids),np.inf),where=s[:,-1]>0)
        condition[ids]=cond;good=np.isfinite(cond)&(cond<=1e7)
        # lstsq's default cutoff; accepted systems are far from rank deficient.
        coefficients=np.sum(u*rhs[:,:,None],axis=1)/np.maximum(s,1e-300)
        step=(vt.transpose(0,2,1)@coefficients[:,:,None])[:,:,0]
        good&=(np.linalg.norm(step[:,3:],axis=1)<=.5)&(np.linalg.norm(step[:,:3],axis=1)<=1)
        valid[ids[~good]]=False;active[ids[~good]]=False
        ids=ids[good];step=step[good];rotation=rotation[good]
        vector=step[:,:3];angle=np.linalg.norm(vector,axis=1);matrix=_skews(vector)
        small=angle<1e-8
        a=np.divide(np.sin(angle),angle,out=np.ones_like(angle),where=~small)
        b=np.divide(1-np.cos(angle),angle**2,out=np.full_like(angle,.5),where=~small)
        result[ids,:3,:3]=rotation@(np.eye(3)+a[:,None,None]*matrix+b[:,None,None]*(matrix@matrix))
        result[ids,:3,3]+=step[:,3:]
        active[ids[np.linalg.norm(step,axis=1)<1e-8]]=False
    points=model@result[:,:3,:3].transpose(0,2,1)+result[:,None,:3,3]
    valid&=np.all(np.sum(points*rays,axis=2)>0,axis=1)
    return result,condition,valid


def refine(model,rays,pose,iterations=12):
    result=pose.copy();condition=float('inf');ray_skews=_skews(rays);model_skews=_skews(model)
    for _ in range(iterations):
        rotation=result[:3,:3];points=model@rotation.T+result[:3,3]
        lengths=np.linalg.norm(points,axis=1)
        if np.any(lengths<.04):raise ValueError('LED too close to camera')
        directions=points/lengths[:,None]
        residual=(ray_skews@directions[...,None])[...,0].reshape(-1)
        projection=(np.eye(3)-directions[:,:,None]*directions[:,None,:])/lengths[:,None,None]
        jacobian=np.concatenate((-rotation@model_skews,np.broadcast_to(np.eye(3),(len(model),3,3))),axis=2)
        jacobian=(ray_skews@projection@jacobian).reshape(-1,6)
        # Robust angular residual prevents one reflection from pulling the fit.
        norm=np.linalg.norm(residual.reshape(-1,3),axis=1)
        weights=np.repeat(np.minimum(1,.006/np.maximum(norm,1e-12)),3)
        weighted=jacobian*np.sqrt(weights[:,None])
        condition=np.linalg.cond(weighted)
        if not np.isfinite(condition) or condition>1e7:raise ValueError('Ill-conditioned optical pose')
        step=np.linalg.lstsq(weighted,-residual*np.sqrt(weights),rcond=None)[0]
        if np.linalg.norm(step[3:])>.5 or np.linalg.norm(step[:3])>1:raise ValueError('Optical step diverged')
        result[:3,:3]=rotation@rotation_step(step[:3]);result[:3,3]+=step[3:]
        if np.linalg.norm(step)<1e-8:break
    points=model@result[:3,:3].T+result[:3,3]
    if np.any(np.sum(points*rays,axis=1)<=0):raise ValueError('Pose behind observed rays')
    return result,condition


def _p3p(model_triplets, rays):
    if model_triplets.ndim == 2: model_triplets = model_triplets[None]
    if rays.ndim == 2: rays = rays[None]
    p1, p2, p3 = model_triplets[:, 0], model_triplets[:, 1], model_triplets[:, 2]
    c = np.linalg.norm(p1 - p2, axis=1)
    a = np.linalg.norm(p2 - p3, axis=1)
    b = np.linalg.norm(p3 - p1, axis=1)
    
    K1 = (a/b)**2
    K2 = (c/b)**2
    
    j1, j2, j3 = rays[:,0], rays[:,1], rays[:,2]
    ca = np.sum(j2 * j3, axis=1)
    cb = np.sum(j1 * j3, axis=1)
    cg = np.sum(j1 * j2, axis=1)
    
    A4 = K1**2 - 2*K1*K2 - 2*K1 + K2**2 - 4*K2*ca**2 + 2*K2 + 1
    A3 = -4*K1**2*cb + 8*K1*K2*cb + 4*K1*ca*cg + 4*K1*cb - 4*K2**2*cb + 8*K2*ca**2*cb + 4*K2*ca*cg - 4*K2*cb - 4*ca*cg
    A2 = 4*K1**2*cb**2 + 2*K1**2 - 8*K1*K2*cb**2 - 4*K1*K2 - 8*K1*ca*cb*cg - 4*K1*cg**2 + 4*K2**2*cb**2 + 2*K2**2 - 4*K2*ca**2 - 8*K2*ca*cb*cg + 4*ca**2 + 4*cg**2 - 2
    A1 = -4*K1**2*cb + 8*K1*K2*cb + 4*K1*ca*cg + 8*K1*cb*cg**2 - 4*K1*cb - 4*K2**2*cb + 4*K2*ca*cg + 4*K2*cb - 4*ca*cg
    A0 = K1**2 - 2*K1*K2 - 4*K1*cg**2 + 2*K1 + K2**2 - 2*K2 + 1
    
    valid_mask = np.abs(A4) > 1e-10
    N_valid = np.sum(valid_mask)
    if N_valid == 0: return np.zeros((0, 4, 4)), np.array([], dtype=int)
    
    C = np.zeros((N_valid, 4, 4))
    C[:, 1, 0] = 1; C[:, 2, 1] = 1; C[:, 3, 2] = 1
    C[:, 0, 3] = -A0[valid_mask] / A4[valid_mask]
    C[:, 1, 3] = -A1[valid_mask] / A4[valid_mask]
    C[:, 2, 3] = -A2[valid_mask] / A4[valid_mask]
    C[:, 3, 3] = -A3[valid_mask] / A4[valid_mask]
    
    roots = np.linalg.eigvals(C)
    orig_indices = np.flatnonzero(valid_mask)
    poses = []
    indices = []
    
    for i, orig_idx in enumerate(orig_indices):
        r = roots[i]
        real_roots = r[np.abs(r.imag) < 1e-5].real
        for v in real_roots:
            if v <= 0: continue
            N = 1 - K2[orig_idx] + K1[orig_idx] + v**2*(K1[orig_idx] - K2[orig_idx] - 1) + 2*v*cb[orig_idx]*(K2[orig_idx] - K1[orig_idx])
            D = 2*cg[orig_idx] - 2*v*ca[orig_idx]
            if np.abs(D) > 1e-5:
                u = N / D
                if u <= 0: continue
            else:
                disc = v**2 * ca[orig_idx]**2 - v**2 + K1[orig_idx]*(1 + v**2 - 2*v*cb[orig_idx])
                if disc < 0: continue
                u1 = v * ca[orig_idx] + np.sqrt(disc)
                u2 = v * ca[orig_idx] - np.sqrt(disc)
                u = u1 if u1 > 0 else u2
                if u <= 0: continue
                
            s1_sq = b[orig_idx]**2 / max(1e-10, 1 + v**2 - 2*v*cb[orig_idx])
            s1 = np.sqrt(s1_sq)
            s2 = u * s1
            s3 = v * s1
            
            if abs(s2**2 + s3**2 - 2*s2*s3*ca[orig_idx] - a[orig_idx]**2) > 1e-3: continue
            if abs(s1**2 + s2**2 - 2*s1*s2*cg[orig_idx] - c[orig_idx]**2) > 1e-3: continue
            
            cam_pts = np.array([s1 * j1[orig_idx], s2 * j2[orig_idx], s3 * j3[orig_idx]])
            mod_pts = model_triplets[orig_idx]
            
            c_mod = np.mean(mod_pts, axis=0)
            c_cam = np.mean(cam_pts, axis=0)
            H = (mod_pts - c_mod).T @ (cam_pts - c_cam)
            U, S, Vt = np.linalg.svd(H)
            R = Vt.T @ U.T
            if np.linalg.det(R) < 0:
                Vt[2, :] *= -1
                R = Vt.T @ U.T
            t = c_cam - R @ c_mod
            pose = np.eye(4)
            pose[:3, :3] = R
            pose[:3, 3] = t
            poses.append(pose)
            indices.append(orig_idx)
            
    return np.array(poses) if poses else np.zeros((0, 4, 4)), np.array(indices, dtype=int)


class RayPoseSolver:
    def __init__(self,model,camera,max_residual_px=2.,min_inliers=6,max_hypotheses=300):
        self.model=np.asarray(model,dtype=float)
        if self.model.shape!=(8,3) or not np.isfinite(self.model).all():raise ValueError('Eight explicit factory LEDs required')
        self.camera=camera;self.max_residual_px=max_residual_px
        self.min_inliers=min_inliers;self.max_hypotheses=max_hypotheses

    def solve_known(self,pixels,led_indices,prior=None):
        pixels=np.asarray(pixels,dtype=float);indices=np.asarray(led_indices,dtype=int)
        if (pixels.shape!=(len(indices),2) or len(indices)<4 or len(set(indices))!=len(indices)
                or np.any(indices<0) or np.any(indices>=8)):
            raise ValueError('Invalid LED observations')
        model=self.model[indices];rays=self.camera.pixel_rays(pixels)
        def seed_poses():
            if prior is not None:yield np.asarray(prior,dtype=float)
            elif len(indices)>=6:
                for subset in combinations(range(len(indices)),6):
                    try:yield dlt_pose(model[list(subset)],rays[list(subset)])
                    except (ValueError,np.linalg.LinAlgError):pass
        best=None
        for seed in seed_poses():
            try:
                estimate,conditioning=refine(model,rays,seed)
                projected=self.camera.project(model@estimate[:3,:3].T+estimate[:3,3])
                errors=np.linalg.norm(projected-pixels,axis=1)
                inliers=int(np.sum(errors<=self.max_residual_px))
                score=(inliers,-float(np.sum(np.minimum(errors,self.max_residual_px)**2)))
                if best is None or score>best[0]:best=(score,estimate,conditioning)
            except (ValueError,np.linalg.LinAlgError):continue
            if inliers==len(indices):break
        if best is None:raise ValueError('No robust metric pose hypothesis')
        _,pose,condition=best
        projected=self.camera.project(model@pose[:3,:3].T+pose[:3,3])
        errors=np.linalg.norm(projected-pixels,axis=1)
        good=errors<=self.max_residual_px
        if np.sum(good)<self.min_inliers:raise ValueError('Insufficient optical inliers')
        if not np.all(good):pose,condition=refine(model[good],rays[good],pose)
        projected=self.camera.project(model[good]@pose[:3,:3].T+pose[:3,3])
        residual=float(np.sqrt(np.mean(np.sum((projected-pixels[good])**2,axis=1))))
        if residual>self.max_residual_px:raise ValueError('Optical residual exceeds threshold')
        return RayPose(pose,int(np.sum(good)),residual,tuple(indices[good]),tuple(np.flatnonzero(good)),condition)

    def _known_batch(self,pixels,leds):
        models=self.model[leds];count=leds.shape[1];n=len(leds)
        rays=self.camera.pixel_rays(pixels.reshape(-1,2)).reshape(n,count,3)
        best=[None]*n;active=np.ones(n,dtype=bool)
        for subset in combinations(range(count),6):
            ids=np.flatnonzero(active)
            if not len(ids):break
            subset=list(subset)
            poses,valid=_dlt_batch(models[ids][:,subset],rays[ids][:,subset])
            ids=ids[valid];poses=poses[valid]
            if not len(ids):continue
            poses,conditions,valid=_refine_batch(models[ids],rays[ids],poses)
            ids=ids[valid];poses=poses[valid];conditions=conditions[valid]
            if not len(ids):continue
            projected=self.camera.project((models[ids]@poses[:,:3,:3].transpose(0,2,1)+poses[:,None,:3,3]).reshape(-1,3))
            errors=np.linalg.norm(projected.reshape(-1,count,2)-pixels[ids],axis=2)
            inliers=np.sum(errors<=self.max_residual_px,axis=1)
            losses=np.sum(np.minimum(errors,self.max_residual_px)**2,axis=1)
            for row,i in enumerate(ids):
                score=(int(inliers[row]),-float(losses[row]))
                if best[i] is None or score>best[i][0]:best[i]=(score,poses[row],conditions[row],errors[row])
                if inliers[row]==count:active[i]=False
        results=[]
        for i,entry in enumerate(best):
            if entry is None or entry[0][0]<self.min_inliers:results.append(None);continue
            _,pose,condition,errors=entry;good=errors<=self.max_residual_px
            try:
                if not np.all(good):pose,condition=refine(models[i,good],rays[i,good],pose)
                projected=self.camera.project(models[i,good]@pose[:3,:3].T+pose[:3,3])
                residual=float(np.sqrt(np.mean(np.sum((projected-pixels[i,good])**2,axis=1))))
                if residual>self.max_residual_px:results.append(None);continue
                results.append(RayPose(pose,int(np.sum(good)),residual,tuple(leds[i,good]),tuple(np.flatnonzero(good)),float(condition)))
            except (ValueError,np.linalg.LinAlgError):results.append(None)
        return results

    def associate(self,pixels,prior=None):
        pixels=np.asarray(pixels,dtype=float)
        if pixels.ndim!=2 or pixels.shape[1]!=2 or not np.isfinite(pixels).all() or not 5<=len(pixels)<=64:
            raise ValueError('Expected 5..64 finite LED candidates')
        if prior is not None:
            predicted=self.camera.project(self.model@prior[:3,:3].T+prior[:3,3])
            distances=np.linalg.norm(predicted[:,None,:]-pixels[None,:,:],axis=2)
            matches=[];used=set()
            for led in np.argsort(distances.min(axis=1)):
                candidates=np.argsort(distances[led])
                observation=next((int(i) for i in candidates if i not in used and distances[led,i]<8),None)
                if observation is not None:matches.append((int(led),observation));used.add(observation)
            if len(matches)>=self.min_inliers:
                leds,observations=zip(*matches)
                try:
                    result=self.solve_known(pixels[list(observations)],leds,prior)
                    return RayPose(result.camera_from_tracker,result.inliers,result.residual_px,result.led_indices,
                                   tuple(observations[i] for i in result.observation_indices),result.condition)
                except ValueError:pass
        rng=np.random.default_rng(1)
        n_pix=len(pixels)
        all_combos=list(combinations(range(n_pix),3))
        rng.shuffle(all_combos)
        # Cap pixel combos to ensure <=100ms execution (1 combo takes ~1.5ms)
        max_combos = min(len(all_combos), max(1, self.max_hypotheses // 5))
        pixel_combos=all_combos[:max_combos]
        
        model_perms=list(permutations(range(8),3))
        model_perm_idx=np.array(model_perms)
        model_triplets=self.model[model_perm_idx]
        
        candidates=[]
        threshold=self.max_residual_px+2.0
        max_possible_inliers=min(8,n_pix)
        best_inlier_count=0
        
        for combo in pixel_combos:
            combo_pixels=pixels[list(combo)]
            try:rays=self.camera.pixel_rays(combo_pixels)
            except ValueError:continue
            rays_tiled=np.tile(rays[None],(336,1,1))
            poses,_=_p3p(model_triplets,rays_tiled)
            
            if len(poses)==0:continue
            points=self.model[None]@poses[:,:3,:3].transpose(0,2,1)+poses[:,None,:3,3]
            try:projected=self.camera.project(points.reshape(-1,3)).reshape(-1,8,2)
            except ValueError:continue
            
            errors=np.linalg.norm(projected[:,:,None,:]-pixels[None,None,:,:],axis=3)
            min_errors=np.min(errors,axis=2)
            inliers=np.sum(min_errors<=threshold,axis=1)
            
            promising=np.flatnonzero(inliers>=self.min_inliers)
            for idx in promising:
                pose_errs=errors[idx].copy()
                matched_leds=[]
                matched_obs=[]
                for _ in range(8):
                    min_val=np.min(pose_errs)
                    if min_val>threshold:break
                    led,obs=np.unravel_index(np.argmin(pose_errs),pose_errs.shape)
                    matched_leds.append(int(led))
                    matched_obs.append(int(obs))
                    pose_errs[led,:]=np.inf
                    pose_errs[:,obs]=np.inf
                
                if len(matched_leds)>=self.min_inliers:
                    err_sum=sum(errors[idx,matched_leds[i],matched_obs[i]]**2 for i in range(len(matched_leds)))
                    score=(len(matched_leds),-err_sum)
                    candidates.append((score,tuple(matched_leds),tuple(matched_obs),poses[idx]))
                    if len(matched_leds)>best_inlier_count:
                        best_inlier_count=len(matched_leds)
            
            if best_inlier_count>=max_possible_inliers:
                break
                
        if not candidates:raise ValueError('No calibrated constellation found')
        candidates.sort(key=lambda x:x[0],reverse=True)
        
        best_fits=[]
        seen_matches=set()
        
        for score,matched_leds,matched_obs,pose in candidates:
            if best_fits and best_fits[0].inliers==min(8,n_pix) and score[0]<best_fits[0].inliers:
                break
            match_key=tuple(sorted(zip(matched_leds,matched_obs)))
            if match_key in seen_matches:continue
            seen_matches.add(match_key)
            if len(seen_matches)>30:break
            
            try:
                fit=self.solve_known(pixels[list(matched_obs)],matched_leds,prior=pose)
                real_obs=tuple(matched_obs[i] for i in fit.observation_indices)
                new_fit=RayPose(fit.camera_from_tracker,fit.inliers,fit.residual_px,
                                fit.led_indices,real_obs,fit.condition)
                best_fits.append(new_fit)
            except ValueError:pass
                
        if not best_fits:raise ValueError('No robust metric pose hypothesis')
        best_fits.sort(key=lambda fit:(-fit.inliers,fit.residual_px))
        best=best_fits[0]
        for other in best_fits[1:]:
            if other.inliers<best.inliers or other.residual_px>best.residual_px+.3:continue
            distance=np.linalg.norm(other.camera_from_tracker[:3,3]-best.camera_from_tracker[:3,3])
            rotation=other.camera_from_tracker[:3,:3].T@best.camera_from_tracker[:3,:3]
            angle=np.arccos(np.clip((np.trace(rotation)-1)/2,-1,1))
            if distance>.02 or angle>.3:
                raise ValueError('Ambiguous constellation association')
        return best
