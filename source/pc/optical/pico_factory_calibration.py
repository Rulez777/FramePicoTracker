# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Read extracted 660-byte factory blobs by exact tracker serial number."""
import base64
import json
import struct
from pathlib import Path
import numpy as np


def load_led_positions(path: Path, serial: str) -> np.ndarray:
    records=json.loads(path.read_text())['calibration_data']
    matches=[r for r in records if r['sn']==serial]
    if len(matches)!=1:
        raise ValueError('Serial must identify exactly one factory record')
    data=base64.b64decode(matches[0]['calibration'],validate=True)
    if len(data)!=660 or data[204:212]!=struct.pack('<II',1,60):
        raise ValueError('Unrecognized researched factory blob layout')
    positions=np.asarray(struct.unpack_from('<24f',data,212)).reshape(8,3)
    if not np.isfinite(positions).all() or np.any(abs(positions)>0.1):
        raise ValueError('Invalid LED positions in meter convention')
    return positions
