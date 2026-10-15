# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Minimal MPEG-TS H264 mux for preserving OCC access-unit timestamps.

Only one video stream, PAT/PMT and PTS/PCR. No guessed arrival-time timestamps.
ffmpeg receives a real timestamped container rather than assigning times to a
raw H264 byte stream. The input's clock domain is still validated separately.
"""
import math


def mpeg_crc(data):
    value=0xffffffff
    for byte in data:
        value^=byte<<24
        for _ in range(8):value=((value<<1)^0x04c11db7 if value&0x80000000 else value<<1)&0xffffffff
    return value.to_bytes(4,'big')


class TimestampMux:
    def __init__(self):self.continuity={}

    def _packet(self,pid,payload,*,start=False,pcr=None):
        maximum=176 if pcr is not None else 184
        if len(payload)>maximum:raise ValueError('TS payload too large')
        cc=self.continuity.get(pid,0);self.continuity[pid]=(cc+1)&15
        header=bytes([0x47,(0x40 if start else 0)|(pid>>8),pid&255])
        if len(payload)==184:
            packet=header+bytes([0x10|cc])+payload
        else:
            length=183-len(payload)
            adaptation=bytes([length])
            if length:
                adaptation+=bytes([0x10 if pcr is not None else 0])
                if pcr is not None:
                    adaptation+=bytes([(pcr>>25)&255,(pcr>>17)&255,(pcr>>9)&255,
                                       (pcr>>1)&255,((pcr&1)<<7)|0x7e,0])
                adaptation+=b'\xff'*(length-len(adaptation)+1)
            packet=header+bytes([0x30|cc])+adaptation+payload
        if len(packet)!=188:raise ValueError('Invalid TS packet size')
        return packet

    def encode(self,annex_b,timestamp_seconds):
        if not math.isfinite(timestamp_seconds) or timestamp_seconds<0 or not annex_b:
            raise ValueError('Timestamped access unit required')
        pts=round(timestamp_seconds*90000)&((1<<33)-1)
        pat=bytes.fromhex('00b00d0001c100000001e100')
        pmt=bytes.fromhex('02b0120001c10000e101f0001be101f000')
        output=bytearray(self._packet(0,b'\0'+pat+mpeg_crc(pat),start=True))
        output.extend(self._packet(0x100,b'\0'+pmt+mpeg_crc(pmt),start=True))
        encoded_pts=bytes([0x21|((pts>>29)&14),(pts>>22)&255,1|((pts>>14)&254),
                           (pts>>7)&255,1|((pts<<1)&254)])
        pes=b'\x00\x00\x01\xe0\x00\x00\x84\x80\x05'+encoded_pts+annex_b
        offset=0
        while offset<len(pes):
            first=offset==0;size=176 if first else 184
            block=pes[offset:offset+size];offset+=len(block)
            output.extend(self._packet(0x101,block,start=first,pcr=pts if first else None))
        return pts,bytes(output)
