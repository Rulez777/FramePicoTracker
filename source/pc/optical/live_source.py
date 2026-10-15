# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Bounded live OCC access units, parameter-set priming and owned recording.

An existing growing recording is reused and never stopped by this owner. Only
SPS/PPS are read from history; historical pictures cannot become live poses.
"""
import queue
import re
import shutil
import subprocess
import threading
import time
import os
from pathlib import Path
from .live_decode import LiveGrayDecoder
try:
    from ..radio.frame_occ_live import GrowingOCC,timestamped_camera_packets
except ImportError:
    from radio.frame_occ_live import GrowingOCC,timestamped_camera_packets


def parameter_sets(access_unit):
    parts=re.split(b'\x00\x00\x00?\x01',access_unit)
    return b''.join(b'\0\0\0\1'+part for part in parts if part and part[0]&31 in (7,8))


class LiveCameraSource:
    def __init__(self,dataset_root,*,disk_budget=2*1024**3,vrcmd='/opt/steamvr/bin/linuxarm64/vrcmd'):
        self.root=Path(dataset_root);self.disk_budget=disk_budget;self.vrcmd=vrcmd
        self.owned=False;self.created=False;self.reader=None;self.decoders={};self.parameters={}
        self.metadata=queue.Queue(maxsize=32);self.frames=queue.Queue(maxsize=8)
        self.failure=None;self.stop=threading.Event();self.thread=None
        self.start_size=0;self.path=None;self.dropped=0

    def _command(self,command):
        argv=[self.vrcmd,'--cmd','0',command]
        if hasattr(os,'geteuid') and os.geteuid()==0:argv=['runuser','-u','steamos','--',*argv]
        result=subprocess.run(argv,capture_output=True,text=True,timeout=8)
        if result.returncode or 'response: ok' not in result.stdout:
            raise RuntimeError('Camera recording control failed: '+result.stdout+result.stderr)

    def start(self):
        if self.thread is not None:raise RuntimeError('Camera source already started')
        paths=list(self.root.glob('*.dataset/*.occ'))
        sizes={path:path.stat().st_size for path in paths}
        time.sleep(.2)
        growing=[path for path,size in sizes.items() if path.exists() and path.stat().st_size>size]
        if growing:self.path=max(growing,key=lambda path:path.stat().st_mtime_ns)
        else:
            if shutil.disk_usage(self.root).free<self.disk_budget+512*1024**2:
                raise RuntimeError('Insufficient space for bounded camera recording')
            self._command('start_tracking_recording 1 0 0');self.owned=True
            until=time.monotonic()+8
            while time.monotonic()<until:
                paths=list(self.root.glob('*.dataset/*.occ'))
                changed=[p for p in paths if p not in sizes or p.stat().st_size>sizes[p]]
                if changed:
                    self.path=max(changed,key=lambda path:path.stat().st_mtime_ns)
                    # Only a dataset that did not exist before our start command is ours to delete.
                    self.created=self.path not in sizes;break
                time.sleep(.05)
            if self.path is None:
                self.close();raise RuntimeError('XRService did not produce a growing recording')
        try:
            # Prime at most sixteen MiB. No history pixels are decoded.
            prime=GrowingOCC(self.path)
            for kind,record in prime.iter_records(max_records=512,max_bytes=16*1024**2):
                for metadata,au in timestamped_camera_packets(kind,record):
                    key=tuple(metadata['stream_key'])
                    if key[0]!=38:continue
                    params=parameter_sets(au)
                    if params:self.parameters[key]=params
                if len(self.parameters)>=4:break
            self.reader=GrowingOCC(self.path);self.reader.seek_complete_tail()
            self.start_size=self.path.stat().st_size
            self.thread=threading.Thread(target=self._follow,daemon=True);self.thread.start()
            return self.path.parent
        except BaseException:self.close();raise

    def _latest(self,target,value):
        try:target.put_nowait(value)
        except queue.Full:
            try:target.get_nowait()
            except queue.Empty:pass
            target.put_nowait(value);self.dropped+=1

    def _follow(self):
        last_growth=time.monotonic()
        try:
            while not self.stop.is_set():
                if self.path.stat().st_size-self.start_size>self.disk_budget:
                    raise RuntimeError('Live OCC growth exceeded recording budget')
                previous=self.reader.offset
                for kind,record in self.reader.iter_records(max_records=128,max_bytes=16*1024**2):
                    for metadata,au in timestamped_camera_packets(kind,record):
                        key=tuple(metadata['stream_key'])
                        if key[0]!=38:continue
                        self._latest(self.metadata,metadata)
                        if key not in self.decoders:self.decoders[key]=LiveGrayDecoder(key,timestamped=True)
                        params=parameter_sets(au)
                        if params:self.parameters[key]=params
                        self.decoders[key].feed(self.parameters.get(key,b'')+au,metadata)
                if self.reader.offset>previous:last_growth=time.monotonic()
                elif time.monotonic()-last_growth>2:raise RuntimeError('Live camera recording stopped growing')
                for decoder in self.decoders.values():
                    if decoder.failure is not None:raise RuntimeError('Live camera decoder failed') from decoder.failure
                    while not decoder.frames.empty():self._latest(self.frames,decoder.frames.get_nowait())
                self.stop.wait(.005)
        except (OSError,ValueError,RuntimeError) as error:self.failure=error

    def close(self):
        self.stop.set()
        try:
            if self.thread:self.thread.join(timeout=3)
            for decoder in self.decoders.values():decoder.close()
        finally:
            if self.owned:
                try:self._command('stop_tracking_recording')
                finally:self.owned=False;self._remove_created()

    def _remove_created(self):
        # Each owned recording grows ~2.4GB per restart cycle; on 2026-10-07
        # undeleted ones filled /home to 97%. Remove only our own dataset.
        dataset=None if self.path is None else self.path.parent
        if (self.created and dataset is not None and dataset.parent==self.root
                and re.fullmatch(r'Deckard_[0-9_-]+\.dataset',dataset.name)):
            # XRService writes snapshots/metrics into the dataset after the stop
            # command (seen 2026-10-07): delete again until it stays gone.
            for _ in range(4):
                shutil.rmtree(dataset,ignore_errors=True)
                time.sleep(.75)
                if not dataset.exists():break
        self.created=False
