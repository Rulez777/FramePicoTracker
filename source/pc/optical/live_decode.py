# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Persistent ffmpeg grayscale decoder with bounded queues.

OCC metadata is retained separately. Decode order is not a validated camera
clock mapping; the frames cannot enable poses until that mapping is measured.
"""
from dataclasses import dataclass
import queue
import subprocess
import threading
import time
import re
from collections import deque
import numpy as np


@dataclass(frozen=True)
class GrayFrame:
    pixels: np.ndarray
    decoded_monotonic_ns: int
    stream_key: tuple
    decode_index: int
    source_time_verified: bool = False
    source_metadata: dict | None = None
    packet_timestamp_matched: bool = False


class LiveGrayDecoder:
    def __init__(self, stream_key, *, max_frame_bytes=2*1024*1024, timestamped=False):
        self.key=tuple(stream_key)
        self.max_frame_bytes=max_frame_bytes
        self.frames=queue.Queue(maxsize=2)
        self.chunks=queue.Queue(maxsize=32)
        self.failure=None
        self.stopped=threading.Event()
        self.dropped_frames=0
        self.timestamped=timestamped
        self.metadata={}
        self.log_tail=deque(maxlen=32)
        self.metadata_lock=threading.Lock()
        self.decoded_times=queue.Queue(maxsize=32)
        if timestamped:
            from .h264_timestamp_mux import TimestampMux
            self.mux=TimestampMux()
        command=['ffmpeg','-v','info' if timestamped else 'error','-threads','1',
            '-fflags','nobuffer','-probesize','32768','-analyzeduration','0',
            '-f','mpegts' if timestamped else 'h264','-i','pipe:0','-an']
        if timestamped:command.extend(['-copyts','-vf','showinfo','-fps_mode','passthrough'])
        command.extend(['-f','image2pipe','-vcodec','pgm','pipe:1'])
        self.process=subprocess.Popen(command,stdin=subprocess.PIPE,stdout=subprocess.PIPE,
                                      stderr=subprocess.PIPE if timestamped else subprocess.DEVNULL)
        self.writer=threading.Thread(target=self._write,daemon=True)
        self.reader=threading.Thread(target=self._read,daemon=True)
        self.writer.start();self.reader.start()
        self.log_reader=None
        if timestamped:
            self.log_reader=threading.Thread(target=self._read_times,daemon=True)
            self.log_reader.start()

    def feed(self, chunk,metadata=None):
        if self.failure is not None or self.stopped.is_set():
            raise RuntimeError('Camera decoder failed or stopped') from self.failure
        if len(chunk)>self.max_frame_bytes:
            raise ValueError('Compressed camera chunk exceeds budget')
        if self.timestamped:
            if metadata is None or 'timestamp_raw' not in metadata:
                raise ValueError('Timestamped decoder requires OCC camera metadata')
            pts,chunk=self.mux.encode(chunk,metadata['timestamp_raw'])
            with self.metadata_lock:
                self.metadata[pts]=dict(metadata)
                while len(self.metadata)>256:self.metadata.pop(next(iter(self.metadata)))
        try:
            self.chunks.put_nowait(bytes(chunk))
        except queue.Full:
            # Dropping a reference picture silently corrupts subsequent frames.
            self.close()
            raise RuntimeError('Compressed backlog exceeded; decoder reset and new IDR required') from None

    def _read_times(self):
        try:
            for line in iter(self.process.stderr.readline,b''):
                self.log_tail.append(line.decode('utf-8',errors='replace').strip())
                if self.stopped.is_set():break
                match=re.search(rb'\bn:\s*(\d+)\s+pts:\s*(-?\d+)\s+pts_time:',line)
                if match:
                    self.decoded_times.put((int(match[1]),int(match[2])&((1<<33)-1)),timeout=1)
        except (OSError,queue.Full) as error:
            if not self.stopped.is_set():self.failure=error

    def _write(self):
        try:
            while not self.stopped.is_set():
                try: chunk=self.chunks.get(timeout=.1)
                except queue.Empty: continue
                self.process.stdin.write(chunk)
                self.process.stdin.flush()
        except (OSError,ValueError) as error:
            if not self.stopped.is_set(): self.failure=error

    def _read(self):
        try:
            index=0
            source=self.process.stdout
            while not self.stopped.is_set():
                magic=source.readline(64)
                if not magic: break
                if magic!=b'P5\n': raise ValueError('Invalid decoded PGM header')
                dims=source.readline(64).split()
                if len(dims)!=2: raise ValueError('Invalid camera dimensions')
                width,height=map(int,dims)
                if not 0<width*height<=self.max_frame_bytes or width<=0 or height<=0:
                    raise ValueError('Decoded image exceeds budget')
                if source.readline(16)!=b'255\n': raise ValueError('Expected 8-bit grayscale')
                size=width*height
                pixels=bytearray()
                while len(pixels)<size:
                    block=source.read(size-len(pixels))
                    if not block: raise ValueError('Truncated decoded camera image')
                    pixels.extend(block)
                array=np.frombuffer(bytes(pixels),dtype=np.uint8).reshape(height,width)
                metadata=None;matched=False
                if self.timestamped:
                    frame_index,pts=self.decoded_times.get(timeout=2)
                    if frame_index!=index:raise ValueError('Decoded timestamp/frame-index mismatch')
                    with self.metadata_lock:metadata=self.metadata.pop(pts,None)
                    if metadata is None:raise ValueError('Decoded PTS has no matching OCC access-unit metadata')
                    matched=True
                frame=GrayFrame(array,time.monotonic_ns(),self.key,index,
                                source_metadata=metadata,packet_timestamp_matched=matched)
                index+=1
                try: self.frames.put_nowait(frame)
                except queue.Full:
                    try: self.frames.get_nowait()
                    except queue.Empty: pass
                    self.dropped_frames+=1
                    self.frames.put_nowait(frame)
        except (ValueError,OSError,queue.Empty) as error:
            if not self.stopped.is_set(): self.failure=error
        finally:
            if not self.stopped.is_set() and self.failure is None:
                self.failure=RuntimeError('Camera decoder output ended')

    def close(self):
        self.stopped.set()
        if self.process.poll() is None:
            self.process.terminate()
            try: self.process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                self.process.kill();self.process.wait(timeout=2)
        self.writer.join(timeout=2);self.reader.join(timeout=2)
        self.process.stdin.close();self.process.stdout.close()
        if self.log_reader:
            self.log_reader.join(timeout=2);self.process.stderr.close()
