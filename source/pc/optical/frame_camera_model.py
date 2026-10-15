# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Polynomial KB ray model for Frame's primary calibration.

Stream-to-camera mapping, crop and thermal corrections must be established by
the caller. This module does not identify cameras from their image dimensions.
"""
from dataclasses import dataclass
import numpy as np


@dataclass(frozen=True)
class FrameKBCamera:
    name: str
    width: int
    height: int
    fx: float
    fy: float
    cx: float
    cy: float
    coefficients: tuple[float, float, float, float]

    @classmethod
    def from_calibration(cls, calibration: dict, name: str):
        entries = [c for c in calibration['cameras'] if c['sourceCamera'] == name]
        if len(entries) != 1:
            raise ValueError('Camera name must identify one primary calibration')
        c = entries[0]
        if c['cameraModel'] != 'kb' or c.get('preRotateClockwiseByDeg', 0) != 0:
            raise ValueError('Unsupported camera model or image rotation')
        i = c['intrinsics']
        result = cls(name, c['width'], c['height'], i['fx'], i['fy'], i['cx'], i['cy'],
                     tuple(i[f'k{n}'] for n in range(1, 5)))
        if result.fx <= 0 or result.fy <= 0:
            raise ValueError('Invalid focal length')
        if not np.isfinite([result.fx, result.fy, result.cx, result.cy, *result.coefficients]).all():
            raise ValueError('Nonfinite calibration')
        return result

    def pixel_rays(self, pixels: np.ndarray) -> np.ndarray:
        """Unit rays in the camera frame, using stored calibration coordinates.

        The caller must supply coordinates matching the calibrated crop. Positive
        ray Z points forward; no Frame/OpenVR extrinsic transform is applied.
        """
        pixels = np.asarray(pixels, dtype=np.float64)
        if pixels.ndim != 2 or pixels.shape[1] != 2 or not np.isfinite(pixels).all():
            raise ValueError('Expected finite Nx2 pixels')
        xy = (pixels - [self.cx, self.cy]) / [self.fx, self.fy]
        radius = np.linalg.norm(xy, axis=1)
        theta = radius.copy()
        k1, k2, k3, k4 = self.coefficients
        for _ in range(30):
            t2 = theta * theta
            polynomial = 1 + t2*(k1+t2*(k2+t2*(k3+t2*k4)))
            derivative = 1 + t2*(3*k1+t2*(5*k2+t2*(7*k3+t2*9*k4)))
            if np.any(derivative <= 0) or not np.isfinite(derivative).all():
                raise ValueError('KB inversion outside monotonic domain')
            theta -= (theta * polynomial - radius) / derivative
        t2 = theta * theta
        residual = theta*(1+t2*(k1+t2*(k2+t2*(k3+t2*k4))))-radius
        if not np.isfinite(theta).all() or np.any(abs(residual)>1e-8) or np.any(theta<0) or np.any(theta>=np.pi):
            raise ValueError('KB inversion did not converge in ray domain')
        scale = np.divide(np.sin(theta), radius, out=np.ones_like(radius), where=radius>1e-12)
        return np.column_stack((xy*scale[:,None], np.cos(theta)))

    def project(self,points):
        points=np.asarray(points,dtype=float)
        if points.ndim!=2 or points.shape[1]!=3 or not np.isfinite(points).all():
            raise ValueError('Expected finite Nx3 camera points')
        radial=np.linalg.norm(points[:,:2],axis=1)
        if np.any(np.linalg.norm(points,axis=1)<1e-8):raise ValueError('Point at camera origin')
        theta=np.arctan2(radial,points[:,2]);t2=theta*theta
        k1,k2,k3,k4=self.coefficients
        distorted=theta*(1+t2*(k1+t2*(k2+t2*(k3+t2*k4))))
        scale=np.divide(distorted,radial,out=np.zeros_like(radial),where=radial>1e-12)
        return points[:,:2]*scale[:,None]*[self.fx,self.fy]+[self.cx,self.cy]
