# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Incremental OCC reader for a growing local tracking recording.

Only complete records are emitted. This is camera transport, not a pose solver.
"""
import struct
from pathlib import Path
from .frame_occ_extract import tlv_fields, annex_b_nals


class GrowingOCC:
    def __init__(self, path, *, max_record_bytes=8*1024*1024):
        self.path = Path(path)
        self.offset = 0
        self.max_record_bytes = max_record_bytes
        self.records_emitted = 0
        self.bytes_emitted = 0
        self.file_identity = None

    def iter_records(self, *, max_records=128, max_bytes=16*1024*1024):
        if max_records <= 0 or max_bytes < self.max_record_bytes:
            raise ValueError('Invalid OCC polling budget')
        with self.path.open('rb') as stream:
            stat = self.path.stat()
            identity = (stat.st_dev, stat.st_ino)
            if self.file_identity is not None and identity != self.file_identity:
                raise ValueError('Recording was replaced; decoder reset required')
            self.file_identity = identity
            if stat.st_size < self.offset:
                raise ValueError('Recording was truncated')
            stream.seek(self.offset)
            consumed = 0
            for _ in range(max_records):
                header = stream.read(16)
                if len(header) < 16:
                    break
                if header[:8] != b'occipitl':
                    raise ValueError('Invalid OCC header')
                kind, size = struct.unpack_from('<II', header, 8)
                if size > self.max_record_bytes:
                    raise ValueError('Record exceeds limit')
                if consumed+16+size > max_bytes:
                    break
                value = stream.read(size)
                if len(value) != size:
                    break
                self.offset += 16+size
                consumed += 16+size
                self.records_emitted += 1
                self.bytes_emitted += 16+size
                yield kind, value

    def poll(self, **budgets):
        """Compatibility adapter, bounded to the same streaming budget."""
        return list(self.iter_records(**budgets))

    def seek_complete_tail(self):
        """Skip history without allocating payloads; retain partial EOF header.

        A live H264 decoder must wait for SPS/PPS and IDR after this boundary.
        Existing history is never labelled as new camera measurements.
        """
        with self.path.open('rb') as stream:
            end = self.path.stat().st_size
            while self.offset+16 <= end:
                stream.seek(self.offset)
                header = stream.read(16)
                if header[:8] != b'occipitl':
                    raise ValueError('Invalid OCC header')
                size = struct.unpack_from('<I', header, 12)[0]
                if size > self.max_record_bytes:
                    raise ValueError('Record exceeds limit')
                if self.offset+16+size > end:
                    break
                self.offset += 16+size
        return self.offset


def camera_chunks(kind, record):
    if kind not in (32, 38):
        return []
    selector = None
    ordinal = {}
    chunks = []
    key = None
    for field, value in tlv_fields(record):
        if field == 17 and len(value) == 1:
            selector = value[0]
            index = ordinal.get(selector, 0)
            ordinal[selector] = index+1
            key = (kind, selector, index)
        elif key is not None and field in (13,14):
            chunk = annex_b_nals(value) if field == 13 else b'\x00\x00\x00\x01'+value
            chunks.append((key, chunk))
    return chunks


def camera_metadata(kind, record):
    """Empirical OCC fields, retain IDs rather than guess sourceCamera names."""
    if kind not in (32,38):
        return []
    cameras=[]
    current=None
    ordinal={}
    for field,value in tlv_fields(record):
        if field==17 and len(value)==1:
            selector=value[0]
            index=ordinal.get(selector,0)
            ordinal[selector]=index+1
            current={'stream_key':[kind,selector,index]}
            cameras.append(current)
        elif current is not None:
            if field==15 and len(value)==4:
                current['camera_id']=struct.unpack('<I',value)[0]
            elif field==3 and len(value)==8:
                current['timestamp_raw']=struct.unpack('<d',value)[0]
            elif field==4 and len(value)==8:
                current['exposure_raw']=struct.unpack('<d',value)[0]
            elif field==55:
                current['sensor_identifier']=value.split(b'\0',1)[0].decode('ascii',errors='replace')
    return cameras


def timestamped_camera_packets(kind,record):
    """Group NALs for each actual OCC camera access unit with its metadata."""
    metadata={tuple(item['stream_key']):item for item in camera_metadata(kind,record)}
    chunks={}
    for key,chunk in camera_chunks(kind,record):chunks.setdefault(key,bytearray()).extend(chunk)
    for key,chunk in chunks.items():
        item=metadata.get(key)
        if item is None or 'timestamp_raw' not in item:
            raise ValueError('Camera access unit has no source timestamp')
        yield item,bytes(chunk)
