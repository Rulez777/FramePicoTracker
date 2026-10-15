# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
# Synthetic benchmark with frozen pre-optimization references; NumPy only.
"""Measured 6DoF from calibrated KB rays and explicit factory LED geometry.

No invented pinhole intrinsics, gyro quaternion or world position. Unknown
correspondences are searched with bounded geometric hypotheses; ambiguous fits
are rejected instead of assigning one tracker to another by convenience.
"""
from dataclasses import dataclass
from itertools import combinations
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


def _reference_dlt_pose(model,rays):
    if len(model)<6:raise ValueError('Initial metric pose requires at least six correspondences')
    centre=model.mean(axis=0)
    scale=np.sqrt(np.mean(np.sum((model-centre)**2,axis=1)))
    if scale<1e-6:raise ValueError('Degenerate LED geometry')
    normalized=(model-centre)/scale
    singular=np.linalg.svd(normalized,compute_uv=False)
    if singular[-1]<singular[0]*1e-4:raise ValueError('Initial LED geometry is coplanar/collinear')
    equations=np.concatenate([np.kron(skew(ray),np.r_[point,1][None,:])
                              for point,ray in zip(normalized,rays)],axis=0)
    _,_,v=np.linalg.svd(equations,full_matrices=False)
    matrix=v[-1].reshape(3,4)
    if np.linalg.det(matrix[:,:3])<0:matrix=-matrix
    u,s,vt=np.linalg.svd(matrix[:,:3]);rotation=u@vt
    if np.linalg.det(rotation)<0:raise ValueError('DLT reflected coordinate frame')
    multiplier=np.mean(s)/scale
    translation=matrix[:,3]/multiplier-rotation@centre
    result=np.eye(4);result[:3,:3]=rotation;result[:3,3]=translation
    return result


def _reference_refine(model,rays,pose,iterations=12):
    result=pose.copy();condition=float('inf')
    for _ in range(iterations):
        rotation=result[:3,:3];points=model@rotation.T+result[:3,3]
        lengths=np.linalg.norm(points,axis=1)
        if np.any(lengths<.04):raise ValueError('LED too close to camera')
        directions=points/lengths[:,None]
        residual=[];jacobian=[]
        for point,ray,direction,length in zip(model,rays,directions,lengths):
            residual.append(skew(ray)@direction)
            projection=(np.eye(3)-np.outer(direction,direction))/length
            jacobian.append(skew(ray)@projection@np.column_stack((-rotation@skew(point),np.eye(3))))
        residual=np.concatenate(residual);jacobian=np.concatenate(jacobian)
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


class _ReferenceRayPoseSolver:
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
        seeds=[]
        if prior is not None:seeds.append(np.asarray(prior,dtype=float))
        elif len(indices)>=6:
            for subset in combinations(range(len(indices)),6):
                try:seeds.append(_reference_dlt_pose(model[list(subset)],rays[list(subset)]))
                except (ValueError,np.linalg.LinAlgError):pass
        best=None
        for seed in seeds:
            try:
                estimate,conditioning=_reference_refine(model,rays,seed)
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
        if not np.all(good):pose,condition=_reference_refine(model[good],rays[good],pose)
        projected=self.camera.project(model[good]@pose[:3,:3].T+pose[:3,3])
        residual=float(np.sqrt(np.mean(np.sum((projected-pixels[good])**2,axis=1))))
        if residual>self.max_residual_px:raise ValueError('Optical residual exceeds threshold')
        return RayPose(pose,int(np.sum(good)),residual,tuple(indices[good]),tuple(np.flatnonzero(good)),condition)

    def associate(self,pixels,prior=None):
        pixels=np.asarray(pixels,dtype=float)
        if pixels.ndim!=2 or pixels.shape[1]!=2 or not np.isfinite(pixels).all() or not 6<=len(pixels)<=64:
            raise ValueError('Expected 6..64 finite LED candidates')
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
        hypotheses=0;fits=[];groups=set()
        for seed in range(min(len(pixels),24)):
            nearest=np.argsort(np.linalg.norm(pixels-pixels[seed],axis=1))[:8]
            for count in (8,7,6):
                if len(nearest)<count:continue
                group=tuple(sorted(int(i) for i in nearest[:count]))
                if group in groups:continue
                groups.add(group)
                observed=pixels[list(group)]
                order=np.argsort(np.arctan2(*(observed-observed.mean(axis=0)).T[::-1]))
                observations=np.array(group)[order]
                for subset in combinations(range(8),count):
                    shape=self.model[list(subset),:2]
                    model_order=np.argsort(np.arctan2(*(shape-shape.mean(axis=0)).T[::-1]))
                    sorted_leds=np.array(subset)[model_order]
                    for direction in (sorted_leds,sorted_leds[::-1]):
                        for shift in range(count):
                            hypotheses+=1
                            if hypotheses>self.max_hypotheses:break
                            leds=np.roll(direction,shift)
                            try:
                                result=self.solve_known(pixels[observations],leds)
                                fits.append(RayPose(result.camera_from_tracker,result.inliers,result.residual_px,result.led_indices,
                                                    tuple(observations[i] for i in result.observation_indices),result.condition))
                            except (ValueError,np.linalg.LinAlgError):pass
                        if hypotheses>self.max_hypotheses:break
                    if hypotheses>self.max_hypotheses:break
                if hypotheses>self.max_hypotheses:break
            if hypotheses>self.max_hypotheses:break
        if not fits:raise ValueError('No calibrated constellation found')
        fits.sort(key=lambda fit:(-fit.inliers,fit.residual_px));best=fits[0]
        for other in fits[1:]:
            if other.inliers<best.inliers or other.residual_px>best.residual_px+.3:continue
            distance=np.linalg.norm(other.camera_from_tracker[:3,3]-best.camera_from_tracker[:3,3])
            rotation=other.camera_from_tracker[:3,:3].T@best.camera_from_tracker[:3,:3]
            angle=np.arccos(np.clip((np.trace(rotation)-1)/2,-1,1))
            if distance>.02 or angle>.3:raise ValueError('Ambiguous constellation association')
        return best

"""Fast Subpixel IR Blob Detector for Steam Frame tracking cameras.

Implemented in pure NumPy (zero external CV dependencies, minimal latency).
Finds bright IR LED spots, extracts subpixel centroids via weighted intensity moments,
and filters out non-circular reflections/glare.
"""



from dataclasses import dataclass
import numpy as np


@dataclass(slots=True)
class Blob:
    """Detected 2D IR LED spot."""

    x: float  # Subpixel horizontal center (pixels)
    y: float  # Subpixel vertical center (pixels)
    radius: float  # Equivalent radius (pixels)
    brightness: float  # Peak intensity (0..255)
    area: int  # Pixel count


class _ReferenceBlobDetector:
    def __init__(
        self,
        threshold: int = 180,
        min_area: int = 2,
        max_area: int = 250,
        min_circularity: float = 0.5,
    ) -> None:
        self.threshold = threshold
        self.min_area = min_area
        self.max_area = max_area
        self.min_circularity = min_circularity

    def detect(self, image: np.ndarray) -> list[Blob]:
        """Detect IR LED spots in a grayscale 2D image (H, W).

        Args:
            image: 2D numpy array of uint8 or float.

        Returns:
            List of detected Blob objects sorted by descending brightness.
        """
        img = np.asarray(image)
        if img.ndim != 2:
            raise ValueError(f"Expected 2D grayscale image, got shape {img.shape}")

        mask = img >= self.threshold
        if not np.any(mask):
            return []

        h, w = mask.shape
        visited = np.zeros_like(mask, dtype=bool)
        blobs: list[Blob] = []

        # Find seed pixels above threshold
        ys, xs = np.nonzero(mask)

        for seed_y, seed_x in zip(ys, xs):
            if visited[seed_y, seed_x]:
                continue

            # Fast 8-connected BFS for this blob
            blob_pixels = []
            queue = [(seed_y, seed_x)]
            visited[seed_y, seed_x] = True

            min_y, max_y = seed_y, seed_y
            min_x, max_x = seed_x, seed_x

            while queue:
                cy, cx = queue.pop()
                blob_pixels.append((cy, cx))
                min_y = min(min_y, cy)
                max_y = max(max_y, cy)
                min_x = min(min_x, cx)
                max_x = max(max_x, cx)

                # 4-neighbor expansion
                for dy, dx in ((-1, 0), (1, 0), (0, -1), (0, 1)):
                    ny, nx = cy + dy, cx + dx
                    if 0 <= ny < h and 0 <= nx < w:
                        if mask[ny, nx] and not visited[ny, nx]:
                            visited[ny, nx] = True
                            queue.append((ny, nx))

            area = len(blob_pixels)
            if area < self.min_area or area > self.max_area:
                continue

            # Bounding box aspect ratio check (reject long streaks / lines)
            bw = max_x - min_x + 1
            bh = max_y - min_y + 1
            aspect = float(bw) / float(bh) if bh > 0 else 999.0
            if aspect > 3.0 or aspect < 0.33:
                continue

            # Calculate weighted intensity centroid (subpixel accuracy)
            pts = np.array(blob_pixels)
            intensities = img[pts[:, 0], pts[:, 1]].astype(np.float64)

            total_weight = np.sum(intensities)
            if total_weight <= 0:
                continue

            sub_y = np.sum(pts[:, 0] * intensities) / total_weight
            sub_x = np.sum(pts[:, 1] * intensities) / total_weight
            radius = np.sqrt(area / np.pi)
            peak_val = float(np.max(intensities))

            blobs.append(
                Blob(
                    x=float(sub_x),
                    y=float(sub_y),
                    radius=float(radius),
                    brightness=peak_val,
                    area=area,
                )
            )

        # Sort by brightness descending
        blobs.sort(key=lambda b: b.brightness, reverse=True)
        return blobs


def _reference_detect(detector,image):
    return _ReferenceBlobDetector(detector.threshold,detector.min_area,detector.max_area,detector.min_circularity).detect(image)


def _reference_associate(solver,pixels,prior=None):
    return _ReferenceRayPoseSolver(solver.model,solver.camera,solver.max_residual_px,solver.min_inliers,
                                  solver.max_hypotheses).associate(pixels,prior)


def _reference_solve_known(solver,pixels,led_indices,prior=None):
    return _ReferenceRayPoseSolver(solver.model,solver.camera,solver.max_residual_px,solver.min_inliers,
                                  solver.max_hypotheses).solve_known(pixels,led_indices,prior)


def synthetic(seed=0,noise=0.):
    from pc.optical.constellation import DEFAULT_LED_POSITIONS
    from pc.optical.frame_camera_model import FrameKBCamera
    rng=np.random.default_rng(seed)
    camera=FrameKBCamera('bench',1056,1024,329.,329.,533.,512.,(.035,-.009,-.002,-.0003))
    model=DEFAULT_LED_POSITIONS.astype(float)
    truth=np.eye(4);truth[:3,:3]=rotation_step(np.array([.25,-.3,.14])+rng.normal(0,.015,3))
    truth[:3,3]=[.4,.1,.6]
    pixels=camera.project(model@truth[:3,:3].T+truth[:3,3])+rng.normal(0,noise,(8,2))
    clutter=np.array([[50,50],[1000,50],[50,970],[1000,970]],dtype=float)
    candidates=np.concatenate((pixels[rng.permutation(8)],clutter))
    image=np.zeros((1024,1056),dtype=np.uint8)
    yy,xx=np.mgrid[-5:6,-5:6]
    for x,y in np.concatenate((pixels,clutter)):
        ix,iy=int(round(x)),int(round(y))
        spot=255*np.exp(-((xx+ix-x)**2+(yy+iy-y)**2)/8)
        image[iy-5:iy+6,ix-5:ix+6]=np.maximum(image[iy-5:iy+6,ix-5:ix+6],spot.astype(np.uint8))
    return model,camera,truth,pixels,candidates,image


def main():
    import argparse
    import platform
    from time import perf_counter
    from pc.optical.ray_pose_solver import RayPoseSolver,refine
    from pc.optical.blob_detector import BlobDetector
    parser=argparse.ArgumentParser(description='Synthetic optics benchmark; no files or hardware accessed')
    parser.add_argument('--repeat',type=int,default=3);args=parser.parse_args()
    if args.repeat<1:parser.error('--repeat must be positive')
    model,camera,truth,pixels,candidates,image=synthetic()
    solver=RayPoseSolver(model,camera);reference=_ReferenceRayPoseSolver(model,camera)
    detector=BlobDetector();rays=camera.pixel_rays(pixels)
    prior=truth.copy();prior[:3,3]+=[.002,-.001,.003]
    print(f'{platform.machine()} Python {platform.python_version()} NumPy {np.__version__}; median of {args.repeat}')
    cases=[('detect',lambda:_reference_detect(detector,image),lambda:detector.detect(image)),
           ('refine',lambda:_reference_refine(model,rays,prior),lambda:refine(model,rays,prior)),
           ('solve_known',lambda:reference.solve_known(pixels,range(8)),lambda:solver.solve_known(pixels,range(8))),
           ('associate/clutter',lambda:reference.associate(candidates),lambda:solver.associate(candidates)),
           ('associate/prior',lambda:reference.associate(candidates,prior),lambda:solver.associate(candidates,prior))]
    for name,old,new in cases:
        medians=[]
        for fn in (old,new):
            fn();times=[]
            for _ in range(args.repeat):
                start=perf_counter();fn();times.append((perf_counter()-start)*1000)
            medians.append(float(np.median(times)))
        before,after=medians
        print(f'{name:20s} before {before:9.3f} ms after {after:9.3f} ms speedup {before/after:7.2f}x')


if __name__=='__main__':
    # Also support python pc/optical/bench_optics.py from any working directory.
    import sys
    from pathlib import Path
    sys.path.insert(0,str(Path(__file__).resolve().parents[2]))
    sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
    main()
