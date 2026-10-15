# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Bind OCC sensors by factory serial and named active calibration, not ordinal.

Exported dataset calibration is already flattened into its reference camera
frame. Factory upper-camera extrinsics can be parent-relative; never substitute
them for the flattened runtime calibration.
"""
from dataclasses import dataclass
import numpy as np
from .frame_camera_model import FrameKBCamera


def transform(entry,position_scale):
    x=np.asarray(entry['plus_x'],dtype=float)
    z=np.asarray(entry['plus_z'],dtype=float)
    y=np.cross(z,x)
    rotation=np.column_stack((x,y,z))
    if (not np.isfinite(rotation).all() or not np.allclose(rotation.T@rotation,np.eye(3),atol=1e-4)
            or np.linalg.det(rotation)<.999):
        raise ValueError('Invalid calibrated axis basis')
    result=np.eye(4)
    result[:3,:3]=rotation
    result[:3,3]=np.asarray(entry['position'],dtype=float)*position_scale
    if not np.isfinite(result).all():raise ValueError('Nonfinite calibrated transform')
    return result


@dataclass(frozen=True)
class BoundCamera:
    sensor_serial: str
    source_camera: str
    intrinsics: FrameKBCamera
    head_from_camera: np.ndarray
    pixel_offset: float


class FrameCameraRig:
    def __init__(self,factory,active,device_config):
        self.factory=factory
        self.active=active
        self.device_config=device_config
        self.by_serial={}
        for camera in factory['cameras']:
            serial=camera['serial']
            if serial in self.by_serial:raise ValueError('Ambiguous factory camera serial')
            self.by_serial[serial]=camera
        # Driver's CAD-from-calibration transform uses metres. Camera calibration
        # explicitly declares millimetres. Head basis comes from device_config.
        self.cad_from_reference=transform(device_config['cv']['cad_from_cal'],1.)
        self.head_from_cad=np.linalg.inv(transform(device_config['head'],1.))

    def bind(self,metadata,width,height):
        serial=metadata.get('sensor_identifier')
        if serial not in self.by_serial:raise ValueError('OCC camera has no factory serial match')
        factory=self.by_serial[serial]
        name=factory['sourceCamera']
        entries=[entry for entry in self.active['cameras'] if entry['sourceCamera']==name]
        if len(entries)!=1:raise ValueError('Active camera calibration is ambiguous')
        entry=entries[0]
        camera=FrameKBCamera.from_calibration(self.active,name)
        if (camera.width,camera.height)!=(width,height):
            raise ValueError('Decoded dimensions do not match serial-bound calibration')
        extrinsic=entry['extrinsics']
        if extrinsic.get('__units')!='mm':raise ValueError('Unknown camera extrinsic units')
        head_from_camera=self.head_from_cad@self.cad_from_reference@transform(extrinsic,.001)
        choices=[i for i in factory['intrinsics'] if i['cameraModel']=='kb']
        if len(choices)!=1:raise ValueError('No unambiguous factory KB profile')
        origin=choices[0]
        # Exported principal points use a half-pixel convention in observed Frame
        # recordings. Explicitly validate this against this unit's factory model.
        offsets=np.array([camera.cx-origin['cx'],camera.cy-origin['cy']])
        if not (np.allclose(offsets,0,atol=1e-4) or np.allclose(offsets,.5,atol=1e-4)):
            raise ValueError('Factory/runtime pixel origin mismatch; calibration selection required')
        return BoundCamera(serial,name,camera,head_from_camera,float(offsets[0]))


def world_from_tracker(world_from_head,head_from_camera,camera_from_tracker):
    matrices=[np.asarray(m,dtype=float) for m in (world_from_head,head_from_camera,camera_from_tracker)]
    for matrix in matrices:
        if (matrix.shape!=(4,4) or not np.isfinite(matrix).all()
                or not np.allclose(matrix[3],[0,0,0,1],atol=1e-6)):
            raise ValueError('Invalid rigid world transform')
        rotation=matrix[:3,:3]
        if not np.allclose(rotation.T@rotation,np.eye(3),atol=1e-4) or np.linalg.det(rotation)<.999:
            raise ValueError('Nonrigid world transform')
    return matrices[0]@matrices[1]@matrices[2]
