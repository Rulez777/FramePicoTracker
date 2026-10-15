# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Offline Swift 2.0 image evidence. Never opens a device or produces an RX profile."""
from __future__ import annotations
import argparse
import hashlib
import json
import struct
from pathlib import Path

HEADER_SIZE=0x141
IMAGE_SIZE=96532
LOAD_BASE=0x1000

def split_container(data: bytes) -> bytes:
    if len(data)!=HEADER_SIZE+2*IMAGE_SIZE:
        raise ValueError('Unexpected Swift container size')
    image=data[HEADER_SIZE:HEADER_SIZE+IMAGE_SIZE]
    if image!=data[HEADER_SIZE+IMAGE_SIZE:]:
        raise ValueError('Swift images differ')
    stack,reset=struct.unpack_from('<2I',image)
    if stack!=0x20006000 or reset!=0x13a5 or image[0x35c:0x35e]!=b'\xfe\xe7':
        raise ValueError('Unexpected image vectors/load-base anchors')
    return image

def flash_bytes(image: bytes, address: int, size: int) -> bytes:
    offset=address-LOAD_BASE
    if offset<0 or size<0 or offset+size>len(image):
        raise ValueError('Address outside extracted application')
    return image[offset:offset+size]

def decode_channel_mask(mask: bytes) -> list[int]:
    """VA 0x7508 consumes five mask bytes after its skipped first byte.

    This is firmware arithmetic, not a claim that a frame was received.
    """
    if len(mask)!=5:
        raise ValueError('Swift channel mask must contain five bytes')
    return [2*(index+1) for index in range(40)
            if mask[index//8] & (1 << (index%8))]

def decode_negotiation_payload(payload: bytes) -> dict:
    """Decode offsets consumed by VA 0x68f0, excluding ESB metadata.

    The first byte tested by that routine is the ESB payload LENGTH, not
    a message type. Meaning of remaining fields needs a real radio capture.
    """
    if len(payload) not in (17,21):
        raise ValueError('Negotiation path accepts payload lengths 17 or 21')
    return {'payload_length':len(payload),
            'runtime_rate_parameter':payload[0],
            'runtime_flag12':payload[2] & 1,
            'runtime_metadata31':((payload[3] & 3) << 6) | (payload[2] >> 2),
            'target_device_id_hex':payload[17:21].hex() if len(payload)==21 else None,
            'channel_mask_hex':payload[7:12].hex(),
            'channels':decode_channel_mask(payload[7:12]),
            'peer_seed_hex':payload[12:16].hex(),
            'peer_prefix':payload[16],
            'peer_prefix_register':int(f'{payload[16]:08b}'[::-1],2),
            'scope':'Offline firmware-derived offsets; frame identity and live reception unverified'}

def build_negotiation_candidate(*, peer_seed: bytes, prefix: int,
                                channel_mask: bytes, device_id: bytes,
                                rate_parameter: int, flags: int,
                                metadata_high: int) -> bytes:
    """Build a targeted 21-byte research candidate; DOES NOT transmit.

    Offsets: Swift68F0 receiver and PhoenixBD20 template. Parameter meanings
    are not fully resolved. On an unpaired tracker this can trigger storage
    writes (605C -> 6CF6 -> 9DDC); no RAM-only guarantee is implied.
    """
    if len(peer_seed) != 4 or len(device_id) != 4:
        raise ValueError('Seed and target ID must each contain four bytes')
    if not decode_channel_mask(channel_mask):
        raise ValueError('Negotiation requires a nonempty channel mask')
    if any(type(v) is not int or not 0 <= v <= 255
           for v in (prefix, rate_parameter, flags, metadata_high)):
        raise ValueError('Parameters must be byte-valued integers')
    return (bytes([rate_parameter, 0, flags, metadata_high, 0, 0, 0x55])
            + channel_mask + peer_seed + bytes([prefix]) + device_id)

def build_connection_confirm_command(*, device_id: bytes, mode: int, sequence: int) -> bytes:
    """Connection-confirm record for handler8D88, table901C entry9.

    Includes the three-byte command header consumed by9500; excludes ESB
    length/S1. Sequence acceptance and RF delivery require live validation.
    May finalize ordinary tracker pairing after negotiation (user-authorized).
    """
    if len(device_id) != 4 or type(mode) is not int or mode not in (1, 2):
        raise ValueError('Expected four-byte device ID and mode1 or2')
    if type(sequence) is not int or not 0 <= sequence <= 63:
        raise ValueError('Command sequence must be a six-bit integer')
    return bytes([sequence, 9, 6, mode, 0]) + device_id

# Compatibility for the earlier research name; command9 is connection confirm,
# not the separate IR-mode command25.
build_ir_enable_command = build_connection_confirm_command

def encode_record_run(records: list[bytes], *, protocol_version: int) -> bytes:
    """Frame complete command records, excluding radio length/S1/CRC16.

    Additive checksum for protocol>2 is separate from hardware radio CRC.
    Discovery and negotiation payloads are NOT record runs.
    """
    if type(protocol_version) is not int or not 0 <= protocol_version <= 255:
        raise ValueError('Protocol version must be a byte')
    for record in records:
        if len(record)<3 or len(record)!=3+record[2]:
            raise ValueError('Incomplete command record')
        if record[0]>63 or not 1<=record[1]<=29:
            raise ValueError('Invalid command sequence or dispatch index')
    payload=b''.join(records)
    return payload+bytes([sum(payload)&255]) if protocol_version>2 else payload

def paired_base_register(peer_seed: bytes, device_id: bytes) -> int:
    """VA0x6124 byte-wise sum, then VA0x58f8 RBIT (no carry between bytes)."""
    if len(peer_seed)!=4 or len(device_id)!=4:
        raise ValueError('Both address seeds must be four bytes')
    word=int.from_bytes(bytes((a+b)&255 for a,b in zip(peer_seed,device_id)),'little')
    return int(f'{word:032b}'[::-1],2)

def operational_tracker_prefix(slot: int, bit_reversed: bool = True) -> int:
    """Operational Pipe 1 prefix derived from station RAM 0x20004D85.

    PhoenixSta.bin executes at VA 0x0E5C8 / 0x0E626:
        r0 = [0x20004D85]  (tracker slot index 0..2)
        bl #0xb31c         (writes rev(rbit32(word)) to NRF_RADIO->PREFIX0.AP1)
    """
    if not 0 <= slot <= 4:
        raise ValueError('Tracker slot index must be 0..4')
    if bit_reversed:
        return int(f'{slot:08b}'[::-1], 2)
    return slot

def partial_paired_address(peer_seed: bytes, id_low: bytes, prefix: int = 0xc0) -> dict:
    """Offline constraint only: low16 ID fixes the address tail, not its start.

    Register ADDRESS byte order corroborated by shortened-address BLE run66.
    This is not a usable receiver profile or a verified live seed/ID mapping.
    """
    if len(peer_seed)!=4 or len(id_low)!=2 or not 0<=prefix<=255:
        raise ValueError('Expected four seed bytes, two low ID bytes and byte prefix')
    reverse=lambda value:int(f'{value:08b}'[::-1],2)
    known=bytes(reverse((a+b)&255) for a,b in zip(peer_seed[:2],id_low))
    return {'base_known_mask':'ffff0000',
            'base_known_value':f'{int.from_bytes(known,"big")<<16:08x}',
            'on_air_tail_register_bytes':bytes([known[1],known[0],prefix]).hex(),
            'unknown_leading_address_bytes':2,
            'scope':'Conditional offline constraint; not verified live pairing'}

def decode_identity_response(payload: bytes) -> dict:
    """VA0x6844 response; FICR DEVICEID[1] supplied by VA0x7d18."""
    if len(payload)!=13 or payload[:4]!=bytes([2,0,10,0]):
        raise ValueError('Not the researched Swift identity response layout')
    return {'device_id_hex':payload[5:9].hex(),
            'scope':'Firmware-derived response layout; actual reception unverified'}

def decode_bootstrap_record(payload: bytes) -> dict:
    """Swift63B4 copies RAMef6[12]; RAMefe holds FICR from66C0/7D18.

    Require independently validated RF CRC before calling. This is the twelve-
    byte bootstrap record, distinct from the thirteen-byte identity response.
    """
    if len(payload)!=12 or payload[:2]!=b'\x04\x00' or payload[3:8]!=b'\x00\x00\x00\x0a\x01':
        raise ValueError('Not the firmware-mapped Swift bootstrap record')
    chip=payload[8:12]
    return {'device_id_hex':chip.hex(),'device_id_le':int.from_bytes(chip,'little'),
            'configuration_byte30':payload[2],
            'scope':'Firmware-mapped FICR payload; RF CRC must be verified independently'}


def decode_station_status(data: bytes) -> list[dict]:
    """Fixed researched library: three 64-byte records, cached registration table.

    Preserve status as a raw integer; its enum and live connection semantics
    have not been established. This table does not expose verified FICR IDs.
    """
    if len(data)!=192:
        raise ValueError('Expected three fixed-size station status records')
    records=[]
    for offset in range(0,192,64):
        record=data[offset:offset+64]
        status,kind=struct.unpack_from('<2I',record)
        text=lambda start,end:record[start:end].split(b'\0',1)[0].decode('ascii',errors='strict')
        records.append({'type':kind,'status_raw':status,
            'version_string':text(8,15),'serial':text(18,36),
            'uninterpreted_bytes36_41':record[36:42].hex()})
    return records

def crc16_bits(bits: str) -> int:
    """CCITT/MSB-first, polynomial0x1021, initialFFFF, no final XOR."""
    crc=0xffff
    for bit in bits:
        if bit not in '01': raise ValueError('Expected a bit string')
        feedback=(crc>>15)^int(bit)
        crc=(crc<<1)&0xffff
        if feedback: crc^=0x1021
    return crc

def decode_raw_swift_candidates(raw: bytes) -> list[dict]:
    """Search captured bits for 5-byte address + 8/4-bit PCF + CRC16.

    CRC matches alone are candidates, not attribution to Pico. No raw RF
    capture has validated alignment/bit ordering for this hardware yet.
    """
    bits=''.join(f'{byte:08b}' for byte in raw)
    results=[]
    for offset in range(min(17,max(0,len(bits)-68))):
        length=int(bits[offset+40:offset+48],2)
        if length>150: continue
        crc_start=offset+52+8*length
        if crc_start+16>len(bits): continue
        expected=int(bits[crc_start:crc_start+16],2)
        if crc16_bits(bits[offset:crc_start])!=expected: continue
        payload=bytes(int(bits[bit:bit+8],2) for bit in range(offset+52,crc_start,8))
        address=bytes(int(bits[bit:bit+8],2) for bit in range(offset,offset+40,8))
        results.append({'bit_offset':offset,'address_hex':address.hex(),
            'payload_hex':payload.hex(),'payload_length':length,
            'control_nibble':int(bits[offset+48:offset+52],2),'crc':expected,
            'attribution':'Unverified; software CRC candidate only'})
    return results

def inspect(data: bytes) -> dict:
    image=split_container(data)
    default=flash_bytes(image,0x16074,24)
    addresses=flash_bytes(image,0x1832c,24)
    if default.hex()!='010000000000000004020000550003000001020000280000':
        raise ValueError('Swift default configuration differs from researched build')
    if addresses[:20].hex()!='e7e7e7e7c2c2c2c2e7c2c3c4c5c6c7c80805ff02':
        raise ValueError('Swift address initializer differs')
    return {
        'container_sha256':hashlib.sha256(data).hexdigest(),
        'image_sha256':hashlib.sha256(image).hexdigest(),
        'image_bytes':len(image),'load_base':hex(LOAD_BASE),
        'reset_address':'0x13a5','reset_file_offset':'0x3a4',
        'default_configuration_va':'0x16074','default_configuration_hex':default.hex(),
        'address_initializer_va':'0x1832c','address_initializer_hex':addresses.hex(),
        'address_initializer_ram':'0x20000080',
        'startup_copy':{'source':'0x18318','destination':'0x2000006c','end':'0x20000668'},
        'register_setup':{'pcnf0':'0x40008','pcnf1_big_endian':True,
                          'default_address_bytes':5,'default_rf_channel':2},
        'application_overrides':{'mode_byte':1,'mode_offset':1,
            'bitrate_byte':4,'crc_byte':2,'base0_api_word':'0x785634aa',
            'base0_register_after_rbit':'0x552c6a1e','initial_prefix_api_value':3},
        'startup_channel_groups':{'pipe0':list(flash_bytes(image,0x15fdc,3)),
            'pipe1':list(flash_bytes(image,0x15fe0,3)),
            'initializer':'0x6658 -> state+0x2d / state+0x30',
            'selection':'0x75cc / 0x7574 select an index modulo 3'},
        'negotiation':{'dispatch':'0x68f0','esb_metadata_bytes':5,
            'payload_lengths':[17,21],'mask_payload_offset':7,'mask_bytes':5,
            'channel_values':'set bit i -> 2*(i+1), channels 2..80',
            'seed_payload_offset':12,'prefix_payload_offset':16,
            'current_pairing_known':False},
        'inference':'ESB-like initialization; mode 1 matches Nordic PRX. Current pairing/channel/addresses are not known.',
        'rf_capture_validated':False,
        'scope':'Extracted update image evidence only; not proof of trackers current installed firmware or a usable receiver profile'
    }

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('container',type=Path)
    p.add_argument('--out',type=Path)
    args=p.parse_args()
    result=inspect(args.container.read_bytes())
    text=json.dumps(result,indent=2)+'\n'
    if args.out: args.out.write_text(text,encoding='utf-8')
    else: print(text,end='')

if __name__=='__main__': main()
