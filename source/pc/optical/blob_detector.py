# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Fast Subpixel IR Blob Detector for Steam Frame tracking cameras.

Implemented in pure NumPy (zero external CV dependencies, minimal latency).
Finds bright IR LED spots, extracts subpixel centroids via weighted intensity moments,
and filters out non-circular reflections/glare.
"""

from __future__ import annotations

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


class BlobDetector:
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

        h,w=mask.shape;ys,xs=np.nonzero(mask);flat=ys*w+xs;n=len(flat)
        # Sparse 4-connected graph: right and down neighbors only, no row wrap.
        right=np.flatnonzero((np.diff(flat)==1)&(ys[1:]==ys[:-1]))
        below=np.searchsorted(flat,flat+w);exists=below<n
        exists[exists]&=flat[below[exists]]==flat[exists]+w
        down=np.flatnonzero(exists)
        left=np.concatenate((right,down));other=np.concatenate((right+1,below[down]))
        labels=np.arange(n)
        while len(left):
            a=labels[left];b=labels[other];different=a!=b
            if not np.any(different):break
            a=a[different];b=b[different]
            np.minimum.at(labels,np.maximum(a,b),np.minimum(a,b))
            while True:
                compressed=labels[labels]
                if np.array_equal(compressed,labels):break
                labels=compressed
        _,components=np.unique(labels,return_inverse=True);count=int(components.max())+1
        area=np.bincount(components);intensities=img[ys,xs].astype(np.float64)
        total=np.bincount(components,weights=intensities)
        sum_x=np.bincount(components,weights=xs*intensities)
        sum_y=np.bincount(components,weights=ys*intensities)
        peak=np.full(count,-np.inf);np.maximum.at(peak,components,intensities)
        min_x=np.full(count,w);max_x=np.zeros(count,dtype=int)
        min_y=np.full(count,h);max_y=np.zeros(count,dtype=int)
        np.minimum.at(min_x,components,xs);np.maximum.at(max_x,components,xs)
        np.minimum.at(min_y,components,ys);np.maximum.at(max_y,components,ys)
        aspect=(max_x-min_x+1)/(max_y-min_y+1)
        good=(area>=self.min_area)&(area<=self.max_area)&(aspect<=3)&(aspect>=.33)&(total>0)
        blobs=[Blob(float(sum_x[i]/total[i]),float(sum_y[i]/total[i]),float(np.sqrt(area[i]/np.pi)),
                    float(peak[i]),int(area[i])) for i in np.flatnonzero(good)]
        blobs.sort(key=lambda b:b.brightness,reverse=True)
        return blobs
