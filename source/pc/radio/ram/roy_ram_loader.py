# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Experimental RAM-only Roy MPSL loader. Run on Frame with gpiod access.

Uses unused manufacturing RTT buffers for module/data, never flash or pairing.
Requires the exact supplied stock image and an explicit --run flag. Default is
an MPSL scheduling experiment WITHOUT enabling the radio. Successful runs unload;
if a live callback cannot be closed, retain its code and recovery checkpoint.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import socket
import signal
import struct
import subprocess
import time
from pathlib import Path

CODE = 0x2001151C
DATA = 0x20010C18
RTT = 0x2001064C
MPSL = 0x2001056C
SHELL_HOOK = 0x2000D8CC  # shell_rtt ctx 0x2000d894 + bypass 0x38
MPU_STATIC_WORD = 0x20010A24  # byte +2 is Zephyr static MPU region count
REGS = "r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 sp lr pc xpsr msp psp primask basepri faultmask control"
STOCK_SHA = "0375ab90c91fa0bce1522f8f510cabfca6667cceadce1e4737b73b82507edd50"
CONFIG = "/opt/steamvr/drivers/frame_hmd/resources/firmware/roy/openocd.cfg"
EARLY_TRAPS = (0x13e54,0x130a,0x13de8,0x20c3a,0x20c9e,0x29e14)

def passive_profile(path: Path) -> dict:
    """Restrict this experimental RX build to the reviewed Swift framing."""
    values=json.loads(path.read_text())
    keys='frequency mode pcnf0 pcnf1 base0 prefix0 crccnf crcpoly crcinit whiteiv'.split()
    parsed={key:int(values[key],0) if isinstance(values[key],str) else values[key] for key in keys}
    if any(type(value) is not int or not 0<=value<=0xffffffff for value in parsed.values()):
        raise ValueError('Radio profile fields must be unsigned 32-bit integers')
    ble_reference=(parsed['mode']==3 and parsed['frequency'] in (2,26,80)
        and parsed['pcnf0']==0x100108 and parsed['pcnf1']==0x02030040
        and parsed['base0']==0x89bed600 and parsed['prefix0']==0x8e
        and parsed['crccnf']==0x103 and parsed['crcpoly']==0x65b
        and parsed['crcinit']==0x555555
        and parsed['whiteiv']=={2:37,26:38,80:39}.get(parsed['frequency']))
    if ble_reference: return parsed
    ble_raw_reference=(parsed['mode']==3 and parsed['frequency'] in (2,26,80)
        and parsed['pcnf0']==0 and parsed['pcnf1'] in (0x02032828,0x00032828)
        and parsed['base0']==0x89bed600 and parsed['prefix0']==0x8e
        and parsed['crccnf']==0 and parsed['crcpoly']==0x65b
        and parsed['crcinit']==0x555555
        and parsed['whiteiv']==(0 if parsed['pcnf1']==0x00032828 else
            {2:37,26:38,80:39}.get(parsed['frequency'])))
    if ble_raw_reference: return parsed
    ble_short_address=(parsed['mode']==3 and parsed['frequency'] in (2,26,80)
        and parsed['pcnf0']==0 and parsed['pcnf1']==0x00022828
        and (parsed['base0'],parsed['prefix0']) in ((0x89be0000,0xd6),(0xbed60000,0x89),(0x89be0000,0x8e))
        and parsed['crccnf']==0 and parsed['crcpoly']==0x65b
        and parsed['crcinit']==0x555555 and parsed['whiteiv']==0)
    if ble_short_address:return parsed
    swift_partial_tail=((parsed['mode']==4 and parsed['pcnf0'] in (0,0x01000000) or parsed['mode']==1 and parsed['pcnf0']==0) and 2<=parsed['frequency']<=80
        and parsed['frequency']%2==0
        and parsed['pcnf1']==0x01022828 and parsed['base0'] in (0x4d060000,0xec4c0000,0x84090000)
        and (parsed['prefix0']==0xc0 or (parsed['base0'],parsed['prefix0']) in
            ((0x4d060000,0x00),(0xec4c0000,0x80),(0x84090000,0x40)))
        and parsed['crccnf']==0
        and parsed['crcpoly']==0x11021 and parsed['crcinit']==0xffff
        and parsed['whiteiv']==0)
    if swift_partial_tail:return parsed
    recovered_full_address=(parsed['mode']==4 and (2<=parsed['frequency']<=80 and parsed['frequency']%2==0
        or parsed['frequency'] in (1,37,77) and parsed['base0']==0x4d06b671) and parsed['pcnf0']==0x40008
        and parsed['pcnf1']==0x01040096 and parsed['base0'] in (0x4d0625b8,0x4d06b671,0x4d0698dd,0x4d06e942)
        and parsed['prefix0']==0 and parsed['crccnf']==2
        and parsed['crcpoly']==0x11021 and parsed['crcinit']==0xffff
        and parsed['whiteiv']==0)
    if recovered_full_address:return parsed
    third_full_address=(parsed['mode']==4 and parsed['frequency'] in (2,6,8,10,12)
        and parsed['pcnf0']==0x40008 and parsed['pcnf1']==0x01040096
        and parsed['base0']==0x84096f68 and parsed['prefix0']==0x40
        and parsed['crccnf']==2 and parsed['crcpoly']==0x11021
        and parsed['crcinit']==0xffff and parsed['whiteiv']==0)
    if third_full_address:return parsed
    ble_preamble_probe=(parsed['mode']==3 and parsed['frequency'] in (2,26,80)
        and parsed['pcnf0']==0 and parsed['pcnf1']==0x01012828
        and parsed['base0']==0 and parsed['prefix0'] in (0x55,0xaa)
        and parsed['crccnf']==0 and parsed['whiteiv']==0)
    if ble_preamble_probe:return parsed
    if not 0<=parsed['frequency']<=100 or parsed['mode']!=4:
        raise ValueError('Unreviewed RF channel/mode/framing')
    supported_raw=(parsed['pcnf0']==0x01000000 and parsed['pcnf1']==0x01022828
                   and parsed['base0']==0x55555500 and parsed['prefix0']==0x55)
    legacy_raw=(parsed['pcnf0']==0 and parsed['pcnf1'] in (0x01012828,0x01011818)
                and parsed['base0']==0 and parsed['prefix0'] in (0x55,0xaa))
    if (supported_raw or legacy_raw) and parsed['crccnf']==0:
        # Experimental preamble capture derived from radiobit (nRF51).
        # BALEN1 is outside documented nRF52833 range; hardware not validated.
        if parsed['whiteiv']!=0: raise ValueError('Raw capture must not whiten')
        return parsed
    if parsed['pcnf0']!=0x40008:
        raise ValueError('Unreviewed packet framing')
    if parsed['pcnf1']!=0x01040096 or parsed['crccnf']!=2 or parsed['crcpoly']!=0x11021 or parsed['crcinit']!=0xffff or parsed['whiteiv']!=0:
        raise ValueError('Unreviewed payload capacity/address/CRC/whitening')
    if (parsed['base0'],parsed['prefix0'])!=(0x552c6a1e,0xc0):
        raise ValueError('Unreviewed full address')
    return parsed



def initialize_three_peers(state: bytearray, identities=None, seed=None) -> list[dict]:
    if len(state)!=2048: raise ValueError('Expected reserved2048-byte state')
    if identities is None or seed is None:
        raise ValueError('Explicit private peer registry and station seed required')
    values=identities
    if len(values)!=3 or len(set(values))!=3 or any(len(bytes.fromhex(v))!=4 or int(v,16)==0 for v in values):
        raise ValueError('ABI3 requires three distinct nonzero LE32 identities; generic enrollment needs the next ABI')
    ids=[bytes.fromhex(value) for value in values]+[bytes(4)]*(3-len(values))
    if not isinstance(seed,bytes) or len(seed)!=4:
        raise ValueError('Station seed must be four explicit LE bytes')
    configure=bytes([0,9,6,1,0,0,0,0,0,0,17,1,7,0,25,1,1,0,19,1,0,0,4,1,22,0,10,4,0,0,0,0,0])
    profiles=[]
    for index,identity in enumerate(ids):
        address=bytes((a+b)&255 for a,b in zip(seed,identity))
        base=int(f'{int.from_bytes(address,"little"):032b}'[::-1],2)
        prefix=int(f'{index:08b}'[::-1],2)
        command=bytearray(configure);command[5:9]=identity
        checksum=sum(command[:-1])&255
        struct.pack_into('<IIBBH',state,1504+index*12,int.from_bytes(identity,'little'),base,prefix,checksum,0)
        profiles.append({'id_le':identity.hex(),'base':base,'prefix':prefix})
    offer=bytes.fromhex('04007a000000550100000000')+seed+bytes(5)
    assert len(offer)==21 and len(configure)==33
    state[1540:1561]=offer
    state[1561:1594]=configure
    return profiles


def clear_traps(debug: Debugger) -> None:
    debug.command("; ".join(f"rbp {address:#x}" for address in EARLY_TRAPS)+
                  "; cortex_m vector_catch none")


def snapshot_v4_ring(debug,header):
    """Copy only published records, then free exactly the captured count."""
    write_count,read_count=struct.unpack_from("<II",header,112)
    pending=(write_count-read_count)&0xffffffff
    if write_count<read_count:
        raise RuntimeError('SRAM record sequence rollover requires a fresh session')
    if pending>10:
        raise RuntimeError('SRAM record ring counters invalid; stopping without consuming overwritten data')
    if not pending:return None
    first=read_count%10;count=min(pending,10-first,0x100000000-read_count)
    ring=bytearray(1120)
    ring[first*112:(first+count)*112]=debug.read(DATA+380+first*112,count*112)
    if count<pending:ring[:(pending-count)*112]=debug.read(DATA+380,(pending-count)*112)
    debug.write(DATA+116,struct.pack("<I",write_count))
    return bytes(ring)


def check_fault(debug: Debugger, report: dict) -> None:
    if "halted" not in debug.command("poll; nrf52.cpu curstate"):
        return
    report["fault"]=fault_snapshot(debug)
    report["fault"]["module_state_hex"]=debug.read(DATA,120).hex()
    report["fault"]["mpsl_context_hex"]=debug.read(MPSL,48).hex()
    # Do not interfere with stock fatal/reset handling after preserving evidence.
    clear_traps(debug)
    debug.command("resume")
    time.sleep(.15)
    raise RuntimeError("Stock fault/assert/reset intercepted; inspect fault snapshot")


def session_is_active(context: bytes) -> bool:
    """Fixed stock ABI: status byte +0x26, not stale callback/request fields."""
    if len(context)!=48:
        raise ValueError("Expected exact 48-byte stock MPSL context")
    return context[0x26]!=0


def elf_image(path: Path) -> tuple[int, bytes]:
    data=path.read_bytes()
    if data[:7] != b"\x7fELF\x01\x01\x01" or struct.unpack_from("<H",data,18)[0]!=40:
        raise ValueError("Expected little endian 32-bit ARM ELF")
    entry, shoff = struct.unpack_from("<I",data,24)[0], struct.unpack_from("<I",data,32)[0]
    entsize, count = struct.unpack_from("<HH",data,46)
    image=bytearray()
    for index in range(count):
        _,kind,flags,address,offset,size,*_=struct.unpack_from("<10I",data,shoff+index*entsize)
        if not flags & 2 or not size:
            continue
        if kind!=1 or address<CODE or address+size>CODE+2045:
            raise ValueError("Module allocates outside reserved code buffer")
        end=address-CODE+size
        image.extend(b"\0"*max(0,end-len(image)))
        image[address-CODE:end]=data[offset:offset+size]
    if not CODE <= (entry & ~1) < CODE+len(image):
        raise ValueError("Bad entry point")
    if (len(image)+3)//4*4 > 2045:
        raise ValueError('Aligned module exceeds reserved code buffer')
    return entry,bytes(image)


class Debugger:
    def __init__(self, log: Path):
        # Old loaders do not hold our new advisory lock. Refuse their listener
        # rather than accidentally send SWD commands to somebody else's process.
        try:
            existing=socket.create_connection(('127.0.0.1',6666),timeout=.2)
        except OSError:
            pass
        else:
            existing.close()
            raise RuntimeError('OpenOCD listener already present; refusing second owner')
        self.log=log.open("w")
        self.sock=None
        self.process=None
        try:
            self.process=subprocess.Popen(["openocd","-f",CONFIG,"-c",
                "bindto 127.0.0.1; gdb_port disabled; telnet_port disabled; tcl_port 6666; init; poll; poll off"],
                stdout=self.log,stderr=subprocess.STDOUT)
            deadline=time.monotonic()+8
            while time.monotonic()<deadline:
                if self.process.poll() is not None:
                    raise RuntimeError("OpenOCD exited; inspect log")
                try:
                    self.sock=socket.create_connection(("127.0.0.1",6666),timeout=1)
                    self.sock.setsockopt(socket.IPPROTO_TCP,socket.TCP_NODELAY,1)
                    self.sock.settimeout(10)
                    return
                except OSError:time.sleep(.05)
            raise RuntimeError("OpenOCD TCL connection timeout")
        except BaseException:
            if self.sock is not None:self.sock.close()
            if self.process is not None and self.process.poll() is None:
                self.process.terminate()
                try:self.process.wait(timeout=2)
                except subprocess.TimeoutExpired:self.process.kill();self.process.wait(timeout=2)
            self.log.close()
            raise

    def command(self, text: str) -> str:
        self.sock.sendall(text.encode()+b"\x1a")
        data=bytearray()
        while b"\x1a" not in data:
            chunk=self.sock.recv(65536)
            if not chunk: raise RuntimeError("OpenOCD disconnected")
            data.extend(chunk)
        result=data.split(b"\x1a",1)[0].decode().strip()
        if any(x in result.lower() for x in ("error:","invalid command", "not halted", "couldn't", "failed", "timed out")):
            raise RuntimeError(result)
        return result

    def read(self, address: int, count: int) -> bytes:
        values=self.command(f"read_memory {address:#x} 32 {(count+3)//4}").split()
        if len(values)!=(count+3)//4:
            raise RuntimeError("Incomplete debugger memory read")
        return b"".join(struct.pack("<I",int(x,0)) for x in values)[:count]

    def read_many(self,regions):
        """Read disjoint aligned words in one OpenOCD request, in given order."""
        if any(address&3 or count&3 or count<=0 for address,count in regions):
            raise ValueError('Invalid batched memory read')
        command='concat '+ ' '.join(f'[read_memory {address:#x} 32 {count//4}]' for address,count in regions)
        values=self.command(command).split()
        if len(values)!=sum(count//4 for address,count in regions):
            raise RuntimeError('Incomplete batched debugger memory read')
        return b''.join(struct.pack('<I',int(x,0)) for x in values)

    def write(self, address: int, data: bytes) -> None:
        for command in self._write_commands(address,data):
            self.command(command)

    def write_many(self, regions) -> None:
        """Validate all regions before executing ordered writes in one TCL request."""
        commands=[command for address,data in regions
                  for command in self._write_commands(address,data)]
        if commands:
            # An error aborts the script before later writes (including a commit).
            self.command('; '.join(commands))

    @staticmethod
    def _write_commands(address: int, data: bytes):
        # Only module buffers, RTT counts and read-counter are writable.
        if not (DATA<=address and address+len(data)<=DATA+2048
                or CODE<=address and address+len(data)<=CODE+2045
                or address==SHELL_HOOK and len(data)==4
                or address==MPU_STATIC_WORD and len(data)==4
                or address in (0xe0001000,0xe000edfc) and len(data)==4
                or address==RTT+16 and len(data)==8):
            raise ValueError("Write is outside RAM module reservation")
        if address & 3 or len(data) & 3:
            raise ValueError("Unaligned RAM write")
        commands=[]
        for offset in range(0,len(data),64):
            chunk=data[offset:offset+64]
            words=" ".join(hex(x[0]) for x in struct.iter_unpack("<I",chunk))
            commands.append(f"write_memory {address+offset:#x} 32 {{{words}}}")
        return commands

    def close(self):
        try:
            self.command("shutdown")
        except (OSError,RuntimeError): pass
        if self.sock: self.sock.close()
        try: self.process.wait(timeout=2)
        except subprocess.TimeoutExpired:
            self.process.terminate()
            try: self.process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                self.process.kill();self.process.wait(timeout=2)
        finally: self.log.close()


def registers(debug: Debugger) -> dict[str,int]:
    words=debug.command(f"get_reg -force {{{REGS}}}").split()
    return {name:int(value,0) for name,value in zip(words[::2],words[1::2])}


def mpu_regions(debug: Debugger) -> tuple[int,list[dict]]:
    result=debug.command("halt; wait_halt 100; set saved_mpu [read_memory 0xe000ed90 32 3]; "
        "set mpu_values $saved_mpu; for {set n 0} {$n < 8} {incr n} {"
        "mww 0xe000ed98 $n; lappend mpu_values {*}[read_memory 0xe000ed9c 32 2]}; "
        "mww 0xe000ed98 [lindex $saved_mpu 2]; resume; set mpu_values").split()
    values=[int(x,0) for x in result]
    control=values[1]
    regions=[{"index":i,"base":values[3+i*2],"attributes":values[4+i*2]} for i in range(8)]
    return control,regions

def region_contains(region: dict, address: int) -> bool:
    value=region["attributes"]
    if not value & 1: return False
    size=1<<(((value>>1)&31)+1)
    base=(region["base"] & ~31) & ~(size-1)
    if not base<=address<base+size: return False
    if size>=256 and value & (1 << (8+(address-base)//(size//8))): return False
    return True


def ram_execute_permissions(debug: Debugger) -> list[dict]:
    # Exact static MPU region captured from this firmware while halted. Apply
    # XN change inside invoke immediately before running RAM, avoiding a
    # context switch that could replace the permission before execution.
    return [{"index":1,"base":0x20000001,"attributes":0x110b0021}]

def restore_permissions(debug: Debugger, regions: list[dict]) -> None:
    if not regions: return
    commands=["set mpu_index [read_memory 0xe000ed98 32 1]"]
    for region in regions:
        commands.append(f"mww 0xe000ed98 {region['index']}; mww 0xe000eda0 {region['attributes']:#x}")
    commands.extend(["mww 0xe000ed98 [lindex $mpu_index 0]"])
    debug.command("; ".join(commands))


def fault_snapshot(debug: Debugger) -> dict:
    regs=registers(debug)
    words=struct.unpack("<6I",debug.read(0xe000ed28,24))
    result={"registers":regs,"cfsr":words[0],"hfsr":words[1],"dfsr":words[2],
            "mmfar":words[4],"bfar":words[5]}
    for name in ("msp","psp"):
        address=regs[name]
        if 0x20000000<=address<=0x2001ffe0:
            result[name+"_frame"]=list(struct.unpack("<8I",debug.read(address,32)))
            result[name+"_stack_hex"]=debug.read(address,min(256,0x20020000-address)).hex()
    result["scb_state"]=list(struct.unpack("<4I",debug.read(0xe000ed04,16)))
    result["mpu_control"]=struct.unpack("<I",debug.read(0xe000ed94,4))[0]
    saved_index=struct.unpack("<I",debug.read(0xe000ed98,4))[0]
    result["mpu_regions"]=[]
    for index in range(8):
        debug.command(f"mww 0xe000ed98 {index}")
        base,attributes=struct.unpack("<2I",debug.read(0xe000ed9c,8))
        result["mpu_regions"].append(dict(index=index,base=base,attributes=attributes))
    debug.command(f"mww 0xe000ed98 {saved_index}")
    return result


def invoke_native(debug: Debugger, entry: int, operation: int, disable_mpu: bool=False,
                  reserved_mpu: bool=False) -> None:
    from roy_rtt_console import send
    if debug.read(SHELL_HOOK,4)!=b"\0"*4:
        raise RuntimeError("Stock console already has a bypass callback")
    if disable_mpu:
        debug.command("mww 0xe000ed94 0")
    elif not reserved_mpu:
        debug.command("set original_mpu_index [read_memory 0xe000ed98 32 1]; "
            "mww 0xe000ed98 1; mww 0xe000eda0 0x010b0021; "
            "mww 0xe000ed98 [lindex $original_mpu_index 0]")
    debug.write(SHELL_HOOK,struct.pack("<I",entry))
    send(debug,str(operation),wait=.01)


def invoke(debug: Debugger, entry: int, operation: int, disable_mpu: bool=False,
           diagnostic: dict | None=None, auto_return: bool=False) -> None:
    # Firmware API addresses apply ONLY to the validated build.
    saved=None
    returned=False
    try:
        for _ in range(20):
            debug.command("halt; wait_halt 100")
            regs=registers(debug)
            if not (regs["xpsr"] & 511) and not (regs["control"] & 1):
                saved=regs
                break
            debug.command("resume")
            time.sleep(.005)
        if saved is None:
            raise RuntimeError("Could not obtain privileged thread context")
        if auto_return:
            names=[f"r{i}" for i in range(13)]+["lr","pc","xpsr","msp","psp",
                "primask","basepri","faultmask","control"]
            debug.write(DATA+1504,struct.pack("<22I",*(saved[name] for name in names)))
        initial=dict(saved)
        initial.update(r0=operation, pc=entry & ~1, primask=1,
                       xpsr=(saved["xpsr"] & ~0x0600FC00) | 0x1000000)
        # Dedicated stack at the top of reserved state/manufacturing buffer.
        if saved["control"] & 2: initial["psp"]=DATA+2032
        else: initial["msp"]=DATA+2032
        initial["sp"]=DATA+2032
        text=" ".join(f"{name} {value:#x}" for name,value in initial.items())
        debug.command("set saved_index [read_memory 0xe000ed98 32 1]; "
            "mww 0xe000ed98 1; mww 0xe000eda0 0x010b0021; "
            "mww 0xe000ed98 [lindex $saved_index 0]; "+
            ("mww 0xe000ed94 0; " if disable_mpu else "")+
            "set_reg {"+text+"}; resume"+("" if auto_return else "; wait_halt 200"))
        if auto_return:
            saved=None  # Module restores context before enabling interrupts.
            returned=True
            return
        halted=registers(debug)
        if not CODE<=halted["pc"]<CODE+2045 or halted["xpsr"] & 511:
            if diagnostic is not None:
                diagnostic["fault"]=fault_snapshot(debug)
            raise RuntimeError("Module did not return at its debug trap")
        returned=True
    finally:
        if auto_return and returned:
            pass
        elif saved is not None and returned:
            debug.command("halt; wait_halt 100")
            text=" ".join(f"{name} {value:#x}" for name,value in saved.items())
            debug.command("set_reg {"+text+"}; resume")
        else:
            # Do not overwrite registers in a fault/handler context with a
            # thread snapshot. Let the stock handler finish after recording it.
            if diagnostic is not None:
                debug.command("rbp 0x13e54; cortex_m vector_catch none")
            debug.command("resume")


def run(args) -> dict:
    if (getattr(args, 'tracker_ota_job', None) is not None
            and not getattr(args, 'authorize_stock_reflash', False)
            and not getattr(args, 'authorize_unverified_fork', False)):
        try:from ..tracker_ota_readiness import require_hardware_ota_clearance
        except ImportError:from radio.tracker_ota_readiness import require_hardware_ota_clearance
        require_hardware_ota_clearance()
    try:
        from .ownership import RoyOwnership
    except ImportError:
        from ownership import RoyOwnership
    previous = {}
    args._stop_flag = [False]
    if getattr(args, 'station_service', False):
        for sig in (signal.SIGTERM,signal.SIGINT):
            previous[sig]=signal.signal(sig,lambda *_: args._stop_flag.__setitem__(0,True))
    try:
        with RoyOwnership():
            return _run_owned(args)
    finally:
        for sig,handler in previous.items():
            signal.signal(sig,handler)


def _run_owned(args) -> dict:
    reviewed_profile=passive_profile(args.profile) if args.rx_only else None
    scan_channels=[int(value) for value in args.channels.split(',')] if args.channels else []
    if scan_channels and (not args.rx_only or any(not 0<=value<=100 for value in scan_channels)):
        raise ValueError('Channel scanning requires reviewed passive RX and channels0..100')
    image=args.stock.read_bytes()
    if hashlib.sha256(image).hexdigest()!=STOCK_SHA:
        raise ValueError("Wrong stock image for this fixed-address experimental loader")
    entry,module=elf_image(args.module)
    report={"flash_written":False,"module_entry":hex(entry),"module_size":len(module),
            "radio_enabled":bool(args.profile),"samples":[],"unloaded":False}
    report['active_ir_tx_probe']=getattr(args,'ir_tx_probe',False)
    report['timer_poll_interval_us']=100 if b"ROYRXPOLLTICK1" in args.module.read_bytes() else None
    peer_state=bytearray(2048)
    ota_job=None
    if getattr(args,'station_three',False):
        identities=json.loads(args.peer_registry.read_text())
        station_config=json.loads(args.station_config.read_text())
        if station_config.get('version')!=1:
            raise ValueError('Unsupported station config version')
        query_schema=station_config.get('tracker_mode_query_schema')
        if query_schema is not None and (type(query_schema) is not int or query_schema not in (1,2)):
            raise ValueError('Mode query schema must be 1 or 2')
        if station_config.get('tracker_desired_mode') is not None and query_schema==1:
            raise ValueError('Remote mode changes require schema2')
        # The service callback owns retries; a SWD observation of phase3 must
        # not cause the host to submit a concurrent request.
        if station_config.get('service_poll'):args.repeat_host=False
        # Stock PICO station cadence (PhoenixSta 2.96): slots on the 4727.5us
        # tracker grid at NORMAL priority so BLE wins any overlap.
        for key in ('interval_us','steady_interval_us','mpsl_priority','slot_us'):
            if station_config.get(key) is not None:setattr(args,key,int(station_config[key]))
        if station_config.get('ble_phase_lock'):
            # MCU rephases slots on the measured 3000us controller grid.
            # A nonmultiple makes the nominal tracker-window calculation
            # disagree with phase-corrected distances. Reject before RF.
            if args.interval_us % 3000 or args.steady_interval_us % 3000:
                raise ValueError('BLE phase lock requires poll intervals divisible by 3000us')
        if station_config.get('ble_post_return_guard_us') is not None:
            # Exact, bounded early-return experiment only. Never a service OTA
            # bypass or a general relaxation for unknown radio callbacks.
            coexist=station_config.get('coexistence',{})
            if (hashlib.sha256(module).hexdigest()!='87756a16ead10622405d5a8fec1e05fbd60193d44850368ed5f8b8cad5fe1ce0'
                or not station_config.get('ble_phase_lock')
                or args.slot_us!=600 or args.interval_us!=6000 or args.steady_interval_us!=6000
                or station_config.get('slot_cycle')!=7
                or station_config.get('slot_end_margin_us')!=100
                or station_config.get('ble_phase_lead_us')!=100
                or station_config['ble_post_return_guard_us']!=100
                or coexist.get('min_controllers')!=2
                or coexist.get('freeze_limit_ms')!=150
                or getattr(args,'tracker_ota_job',None) is not None):
                raise ValueError('Early-return quiet window requires exact phase66 coexistence experiment')
        dense_frame=station_config.get('dense_frame_diagnostic',False)
        if dense_frame:
            coexist=station_config.get('coexistence',{})
            if (hashlib.sha256(module).hexdigest() not in ('54cd4b6137a5ca65f2dc9c6d8f72087448d8f8d87c01843dda1699abc97f2a46','b14a214100bbf00c5bd25592c753eedaa4326375343699d6748933c35be2b11a','f0771efd3caed44f2af55d5a176dcb577fde5a62f817187edd25e14434fabf79')
                    or not station_config.get('service_poll') or station_config.get('ble_phase_lock')
                    or args.interval_us not in (3000,2989,2600) or args.steady_interval_us!=args.interval_us or args.slot_us!=900
                    or station_config.get('slot_cycle')!=1 or station_config.get('slot_end_margin_us')!=100
                    or station_config.get('tracker_dual_mode_ids')!=['OWNER_PRIVATE_ID_REQUIRED']
                    or station_config.get('tracker_mode_decimation')!={'stock':0,'frame':0}
                    or station_config.get('tracker_mode_query_schema')!=2
                    or station_config.get('tracker_desired_mode') is not None
                    or station_config.get('optics_enabled') is not False
                    or station_config.get('diagnostic_pause_peers')!=[1,2]
                    or coexist.get('controllers_off') is not True or coexist.get('min_controllers')!=0
                    or getattr(args,'tracker_ota_job',None) is not None):
                raise ValueError('Dense Frame diagnostic requires exact no-OTA controllers-OFF profile')
        if not dense_frame and (station_config.get('tracker_desired_mode') is not None or station_config.get('tracker_mode_query_schema')==2):
            # Reject transition mismatch BEFORE opening debugger/SWD/radio.
            if getattr(args,'tracker_ota_job',None) is not None:
                raise ValueError('Automatic mode switching is forbidden in an OTA session')
            try:from .station_v4_runtime import validate_remote_mode_grid
            except ImportError:from station_v4_runtime import validate_remote_mode_grid
            validate_remote_mode_grid(station_config.get('tracker_desired_mode') or 'frame',
                station_config.get('tracker_dual_mode_ids',[]),station_config.get('tracker_mode_decimation'),
                args.interval_us*station_config.get('slot_cycle',7)
                    if args.interval_us==args.steady_interval_us else None)
        if not dense_frame and hashlib.sha256(module).hexdigest() in ('d68b808dff0ca1a172d76a32b7e52209914172f52d98a2523290e3d0802adb4a','b14a214100bbf00c5bd25592c753eedaa4326375343699d6748933c35be2b11a'):
            # Single-peer capture-ACK build has one global latest-reply token.
            # Reject unsafe configurations before opening debugger/SWD/RF.
            try:from .tracker_ota_runtime import validate_service27_plan
            except ImportError:from tracker_ota_runtime import validate_service27_plan
            validate_service27_plan(dict(radio_profile=('service38' if hashlib.sha256(module).hexdigest()=='b14a214100bbf00c5bd25592c753eedaa4326375343699d6748933c35be2b11a' else 'service36'),slot=0,source_grid_us=4727.5),station_config,args)
        if getattr(args,'tracker_ota_job',None) is not None:
            try:from .tracker_ota_runtime import load_job,preflight,apply_cli_authorization
            except ImportError:from radio.ram.tracker_ota_runtime import load_job,preflight,apply_cli_authorization
            ota_job=apply_cli_authorization(load_job(args.tracker_ota_job,args.peer_registry,
                allow_unverified_fork=getattr(args,'authorize_unverified_fork',False)),args)
            preflight(ota_job,station_config,args)
        seed=bytes.fromhex(station_config['seed_le'])
        decimation_by_id=station_config.get('tracker_poll_decimation_by_id',{})
        if not isinstance(decimation_by_id,dict) or any(not isinstance(key,str) or len(key)!=8 or
                any(c not in '0123456789abcdef' for c in key) or type(value) is not int or not 0<=value<=40
                for key,value in decimation_by_id.items()):
            raise ValueError('Invalid explicit per-tracker command16 N mapping')
        if getattr(args,'station_v4',False):
            try:from .station_v4_runtime import initialize_v4,load_registry
            except ImportError:from station_v4_runtime import initialize_v4,load_registry
            report['station_peers']=initialize_v4(peer_state,load_registry(args.peer_registry),seed,
                                                args.interval_us,args.mpsl_priority,
                                                args.steady_interval_us,args.slot_us,station_config.get('half_us_extra',False),
                                                station_config.get('diagnostic_pause_all',False),
                                                station_config.get('slot_cycle',7),tuple(station_config.get('diagnostic_pause_peers',()))+((1,2) if station_config.get('staggered_enroll') else ()),
                                                station_config.get('slot_end_margin_us',200),dense_frame=dense_frame)
        else:
            report['station_peers']=initialize_three_peers(peer_state,identities,seed)
    if reviewed_profile:
        report['capture_kind']=('swift_partial_tail_conditional' if reviewed_profile['mode'] in (1,4) and reviewed_profile['base0'] in (0x4d060000,0xec4c0000,0x84090000)
            else 'ble_short_address_unvalidated' if reviewed_profile['mode']==3 and reviewed_profile['pcnf1']==0x00022828
            else 'ble_preamble_unvalidated' if reviewed_profile['mode']==3 and reviewed_profile['base0']==0
            else 'ble_raw_reference' if reviewed_profile['mode']==3 and not reviewed_profile['crccnf']
            else 'ble_hardware_crc_reference' if reviewed_profile['mode']==3
            else 'preamble_unvalidated' if reviewed_profile['crccnf']==0 else 'hardware_crc')
    report['deployment'] = {
        'stock_sha256': STOCK_SHA,
        'elf_sha256': hashlib.sha256(args.module.read_bytes()).hexdigest(),
        'code_sha256': hashlib.sha256(module).hexdigest(),
        'code_bytes': len(module), 'code_limit': 2045,
        'data_bytes': 2048, 'config_version': 1,
        'abi': 4 if getattr(args,'station_v4',False) else 3 if getattr(args, 'station_service', False) else 'diagnostic',
    }
    args.out.with_suffix('.manifest.json').write_text(
        json.dumps(report['deployment'], indent=2)+'\n')
    guard=None
    optics=None
    if getattr(args,'station_v4',False):
        try:from .controller_guard import ControllerGuard
        except ImportError:from controller_guard import ControllerGuard
        coexistence=station_config.get('coexistence',{})
        guard=ControllerGuard(args.tracking_watch,metrics_log=coexistence.get('metrics_log'),
                              freeze_limit_ms=coexistence.get('freeze_limit_ms'),
                              min_controllers=coexistence.get('min_controllers',2),
                              forbid_controllers=coexistence.get('controllers_off') is True)
        try:
            # Wait up to 5s for one fresh sample with HMD and both controllers
            # valid; the first sample after the monitor starts can lag.
            until=time.monotonic()+5
            while time.monotonic()<until:
                try:
                    if guard.history and guard.check(require_now=True):break
                except RuntimeError:pass
                time.sleep(.05)
            guard.check(require_now=True)
            if station_config.get('optics_enabled',True):
                try:from ...optical.station_optics import StationOptics
                except ImportError:from optical.station_optics import StationOptics
                optics=StationOptics(station_config['optics'],guard)
                optics.start()
        except BaseException:
            if optics is not None:optics.close()
            guard.close()
            raise
    try:debug=Debugger(args.log)
    except BaseException:
        if optics is not None:optics.close()
        if guard is not None:guard.close()
        raise
    saved_code=saved_data=saved_counts=None
    opened=False
    state_initialized=False
    invocation_started=False
    permissions=[]
    original_debug_enable=None
    saved_static_word=None
    trace_controls=None
    runtime = None
    previous_handlers = {}
    try:
        if args.counter_only or args.timer_only or args.rx_only:
            original_debug_enable=struct.unpack("<I",debug.read(0xe000edf0,4))[0]&1
            debug.command("mww 0xe000edf0 0xa05f0001")
            debug.command("; ".join(f"bp {address:#x} 2 hw" for address in EARLY_TRAPS)+
                "; cortex_m vector_catch hard_err mm_err bus_err state_err int_err nocp_err chk_err")
            report["counter_only"]=args.counter_only
            report["timer_only"]=args.timer_only
            report["rx_only"]=args.rx_only and not getattr(args,'ir_tx_probe',False)
        if args.inspect_mpu:
            control,regions=mpu_regions(debug)
            report.update(mpu_control=control,mpu_regions=regions)
            return report
        # Full application flash was captured and matched offline. Repeated full
        # SWD reads interfere with the stock host's timesync. Check exact entry
        # points and image endpoints before every short runtime experiment.
        for offset in (0,0x10580,0x18948,0x189BE,len(image)-64):
            if debug.read(offset,64)!=image[offset:offset+64]:
                raise RuntimeError("Live firmware anchor differs from stock")
        report["firmware_check"]="five exact flash anchors; full SHA checked against earlier live capture"
        phase_lock=None
        if getattr(args,'station_v4',False) and station_config.get('ble_phase_lock'):
            # Learn the stock BLE controller schedule before taking any slot;
            # never start RF without a quiet window to sit in.
            try:from .ble_phase import sample as ble_sample,quiet_window,PhaseLock,quiet_width_us
            except ImportError:from ble_phase import sample as ble_sample,quiet_window,PhaseLock,quiet_width_us
            samples=[];until=time.monotonic()+float(station_config.get('ble_phase_seconds',6))
            while time.monotonic()<until:samples.append(ble_sample(debug))
            # Retain the observations before admission: a rejected profile must
            # remain independently analyzable, without another hardware run.
            sample_path=args.out.with_suffix('.ble-samples.json')
            sample_path.write_text(json.dumps({'format':1,'fields':['rtc_tick','busy','base0'],
                'samples':samples},separators=(',',':'))+'\n')
            report['ble_samples_file']=str(sample_path)
            reference,center,width,profile=quiet_window(samples,
                max_busy_fraction=station_config.get('ble_max_busy_fraction'))
            report['ble_quiet_window']={'reference_tick':reference,'center_us':round(center*1e6),
                                        'width_us':round(width*1e6),'samples':len(samples),'profile':profile}
            required_width=quiet_width_us(args.slot_us,
                station_config.get('slot_end_margin_us',200),
                station_config.get('ble_phase_lead_us',100),
                station_config.get('ble_post_return_guard_us'))
            report['ble_quiet_window']['required_width_us']=required_width
            if width*1e6+1e-6<required_width:
                raise RuntimeError(f'BLE quiet window {width*1e6:.0f}us too short for {args.slot_us}us slots')
            phase_lock=PhaseLock(reference,center,args.slot_us/1e6,width_s=width,
                                 lead_s=float(station_config.get('ble_phase_lead_us',100))/1e6)
            # Sampler phase bias: measured live (2026-10-07) by where our own slot
            # activity appears in the same sampler; corrects the slot target.
            phase_lock.target=(phase_lock.target+float(station_config.get('ble_phase_offset_us',0))/1e6)%phase_lock.period
        if session_is_active(debug.read(MPSL,48)):
            raise RuntimeError("MPSL session is in use; cannot reserve its only slot")
        if debug.read(SHELL_HOOK,4)!=bytes(4):
            raise RuntimeError('Native shell hook already owned; refusing SRAM mutation')
        if debug.read(RTT,10)!=b"SEGGER RTT": raise RuntimeError("RTT ABI mismatch")
        saved_counts=debug.read(RTT+16,8)
        if saved_counts!=struct.pack("<II",3,3): raise RuntimeError("Manufacturing RTT layout differs")
        saved_code=debug.read(CODE,2044)
        saved_data=debug.read(DATA,2048)
        if args.reserve_mpu:
            saved_static_word=debug.read(MPU_STATIC_WORD,4)
            if saved_static_word[2]!=2:
                raise RuntimeError("Unexpected Zephyr static MPU count")
        checkpoint=args.out.with_suffix(".recovery.json")
        if getattr(args,'station_v4',False):
            trace_controls={hex(address):debug.read(address,4).hex()
                            for address in (0xe0001000,0xe000edfc)}
        checkpoint.write_text(json.dumps({"stock_sha":STOCK_SHA,
            "code":saved_code.hex(),"data":saved_data.hex(),
            "counts":saved_counts.hex(),"trace_controls":trace_controls},indent=2)+"\n")
        report["recovery_checkpoint"]=str(checkpoint)
        if saved_static_word is not None:
            recovery=json.loads(checkpoint.read_text())
            recovery["mpu_static_word"]=saved_static_word.hex()
            checkpoint.write_text(json.dumps(recovery,indent=2)+"\n")
        # Normal console (buffer 0) stays available; manufacturing channels 1/2 reserved.
        debug.write(RTT+16,struct.pack("<II",1,1))
        state=bytearray(2048)
        struct.pack_into("<I",state,0,0x314d5250)
        struct.pack_into("<II",state,36,args.slot_us,args.interval_us)
        state[84]=255
        if args.profile:
            profile=reviewed_profile or json.loads(args.profile.read_text())
            keys="frequency mode pcnf0 pcnf1 base0 prefix0 crccnf crcpoly crcinit whiteiv".split()
            struct.pack_into("<I",state,8,1)
            struct.pack_into("<10I",state,44,*(int(profile[k],0) if isinstance(profile[k],str)
                                              else profile[k] for k in keys))
        if getattr(args,'station_three',False):
            if getattr(args,'station_v4',False):
                state[1504:]=peer_state[1504:]
                if phase_lock is not None:
                    # MCU locks every slot start to this RTC1 tick modulo 3000us.
                    struct.pack_into('<I',state,1884,0x80000000|phase_lock.target_tick())
                struct.pack_into('<I',state,1876,args.slot_us)
                # Service mode: MCU re-requests the earliest slot after a block
                # instead of idling the session (controllers off, no BLE grid).
                struct.pack_into('<I',state,1896,1 if station_config.get('service_poll') else 0)
            else:state[1504:1594]=peer_state[1504:1594]
        service = getattr(args, "station_service", False)
        runtime = None
        if service:
            if getattr(args,'station_v4',False):
                try:from .station_v4_runtime import StationV4Runtime
                except ImportError:from station_v4_runtime import StationV4Runtime
                if ota_job is not None:
                    try:from .tracker_ota_runtime import TrackerOtaRuntime
                    except ImportError:from radio.ram.tracker_ota_runtime import TrackerOtaRuntime
                    runtime=TrackerOtaRuntime(args.live_state,report['station_peers'],args.peer_registry,debug,seed,optics,
                                              ota_job=ota_job)
                    # SPI erase/final image CRC can exceed the normal 3s reply
                    # timeout. The OpenVR guard and MCU host watchdog stay on.
                    struct.pack_into('<I',state,1612,40*64_000_000)
                    report['tracker_ota_target']=ota_job['chip_id_le']
                    report['tracker_ota_image_sha256']=ota_job['sha256']
                else:
                    runtime=StationV4Runtime(args.live_state,report['station_peers'],args.peer_registry,debug,seed,optics)
                    runtime.service27_counters=(hashlib.sha256(module).hexdigest() in (
                        '624e6ab6b1b951aab909214105ddea2876bc2946ac69b7f67a0978eed8ded120',
                        'd68b808dff0ca1a172d76a32b7e52209914172f52d98a2523290e3d0802adb4a','b14a214100bbf00c5bd25592c753eedaa4326375343699d6748933c35be2b11a','f0771efd3caed44f2af55d5a176dcb577fde5a62f817187edd25e14434fabf79')
                        and station_config.get('slot_cycle')==1
                        and set(station_config.get('diagnostic_pause_peers',()))=={1,2})
                runtime.phase_lock=phase_lock;runtime.slot_cycle=station_config.get('slot_cycle',7)
                runtime.decimation_by_id=decimation_by_id
                runtime.enable_dual_mode(station_config.get('tracker_dual_mode_ids',[]),
                                         station_config.get('tracker_mode_decimation'),
                                         desired_profile=station_config.get('tracker_desired_mode'),
                                         query_schema=station_config.get('tracker_mode_query_schema'),
                                         dense_frame=dense_frame,
                                         poll_period_us=(args.interval_us*station_config.get('slot_cycle',7)
                                             if args.interval_us==args.steady_interval_us
                                             else None))
                runtime.radio_log=station_config.get('live_radio_log')
                runtime.radio_log_samples=int(station_config.get('live_radio_log_samples',1))
                if station_config.get('staggered_enroll'):
                    runtime.stagger_paused={1,2}-set(station_config.get('diagnostic_pause_peers',()));runtime.stagger_since=time.monotonic()
                if station_config.get('tracker_poll_decimation',True):
                    if station_config.get('tracker_poll_decimation_n') is None:
                        runtime.set_poll_period(args.interval_us*station_config.get('slot_cycle',7))
                    else:
                        # Empirical override while the FW189 window formula is unconfirmed.
                        runtime.decimation=int(station_config['tracker_poll_decimation_n'])
                        runtime.decimation_m=int(station_config.get('tracker_poll_decimation_m',2))
                        if station_config.get('tracker_imu_mode') is not None:runtime.imu_mode=int(station_config['tracker_imu_mode'])
                        for engine in runtime.engines:
                            if engine:runtime._configure_engine(engine)
            else:
                from station_runtime import StationRuntime
                runtime = StationRuntime(args.live_state, report["station_peers"], getattr(args,"peer_registry",None))
            struct.pack_into("<I", state, 1608, 1000)
        debug.write(DATA,bytes(state))
        state_initialized=True
        debug.write(CODE,module+b"\0"*((-len(module))%4))
        if debug.read(CODE,len(module))!=module:
            raise RuntimeError("RAM code failed readback immediately after write")
        permissions=[] if args.reserve_mpu else ram_execute_permissions(debug)
        report["temporary_mpu_permissions"]=permissions
        recovery=json.loads(checkpoint.read_text())
        recovery["mpu_permissions"]=permissions
        checkpoint.write_text(json.dumps(recovery,indent=2)+"\n")
        actual=debug.read(CODE,len(module))
        report["code_mismatches"]=[{"offset":i,"expected":a,"actual":b}
            for i,(a,b) in enumerate(zip(module,actual)) if a!=b][:32]
        if debug.read(DATA,4)!=struct.pack("<I",0x314d5250) or actual!=module:
            raise RuntimeError("Module RAM changed before invocation; target may have restarted")
        invocation_started=True
        if guard is not None:guard.check(require_now=True)
        if args.native_shell:
            invoke_native(debug,entry,3 if args.trap_only else 0,args.disable_mpu or args.reserve_mpu,
                          args.reserve_mpu)
        else:
            invoke(debug,entry,3 if args.trap_only else 0,args.disable_mpu,
                   report if args.counter_only else None,args.auto_return)
        if (args.counter_only or args.timer_only or args.rx_only) and args.native_shell:
            check_fault(debug,report)
        if args.reserve_mpu:
            report["reserved_static_count"]=debug.read(MPU_STATIC_WORD,4)[2]
            report["mpu_control_during_run"]=struct.unpack("<I",debug.read(0xe000ed94,4))[0]
            if report["reserved_static_count"]!=3 or report["mpu_control_during_run"]!=5:
                raise RuntimeError("RAM MPU reservation not acknowledged")
        if args.trap_only:
            if debug.read(DATA,4)!=struct.pack("<I",0x314d5250):
                raise RuntimeError("Module state lost during entry")
            if args.native_shell:
                marker,count=struct.unpack("<2I",debug.read(DATA+1600,8))
                report["native_return_marker"]=hex(marker)
                report["native_entry_count"]=count
                if marker!=0x52455431 or count!=1:
                    raise RuntimeError("Native entry did not acknowledge return")
            else:
                report["trap_returned"]=True
            return report
        status=struct.unpack("<9I",debug.read(DATA,36))
        report["open_status"]=status
        if status[0]!=0x314d5250:
            report["target_restarted_or_ram_lost"]=True
            raise RuntimeError("Module state lost during open/request")
        opened=debug.read(DATA+84,1)[0]!=255
        if status[8]!=0: raise RuntimeError(f"MPSL open/request returned {status[8]:#x}")
        stopping = getattr(args, '_stop_flag', [False])
        deadline=time.monotonic()+args.duration if args.duration else float('inf')
        last_heartbeat = 0
        while time.monotonic()<deadline and not stopping[0] and not (runtime and runtime.reload_requested):
            loop_t0=time.monotonic()
            if guard is not None:guard.check()
            if args.counter_only or args.timer_only or args.rx_only:
                check_fault(debug,report)
            v4=getattr(args,'station_v4',False)
            header=debug.read(DATA,120 if v4 else 36)
            sample=struct.unpack_from("<9I",header)
            ring=snapshot_v4_ring(debug,header) if v4 and args.profile else None
            if scan_channels:
                channel=scan_channels[len(report['samples'])%len(scan_channels)]
                debug.write(DATA+44,struct.pack('<I',channel))
            report["samples"].append(list(sample))
            if service:
                report["samples"] = report["samples"][-100:]
                loop_t1=time.monotonic()
                if getattr(args,'station_v4',False):
                    runtime.radio_crc_rx=sample[4]
                    runtime.radio_ring_dropped=sample[5]
                    runtime.poll()
                loop_t2=time.monotonic()
                if loop_t2-loop_t0>1.0:
                    # Diagnose host stalls that let the MCU heartbeat run out.
                    stalls=report.setdefault('loop_stalls',[])
                    if len(stalls)<50:stalls.append({'at':round(loop_t0,3),'guard_header_s':round(loop_t1-loop_t0,3),
                                                      'runtime_poll_s':round(loop_t2-loop_t1,3)})
                if sample[1]:
                    raise RuntimeError("MCU service heartbeat expired; stopping and restoring")
                if time.monotonic()-last_heartbeat > (.05 if getattr(args,'station_v4',False) else .5):
                    # 1000 slots (~6s at 6ms): the optics thread can stall this loop
                    # >1s; 200 slots expired the MCU heartbeat mid-session (2026-10-07).
                    debug.write(DATA+1608, struct.pack('<I', sample[3]+1000))
                    runtime.publish(sample[3])
                    last_heartbeat = time.monotonic()
            if args.counter_only:
                report["last_signal"],report["signal_bitmap"]=struct.unpack("<2I",debug.read(DATA+44,8))
            if sample[0]!=0x314d5250:
                report["target_restarted_or_ram_lost"]=True
                raise RuntimeError("Module state disappeared after scheduling; MPSL runtime test failed")
            if args.profile:
                write_count,read_count=(struct.unpack_from("<II",header,112) if v4 else
                                        struct.unpack("<II",debug.read(DATA+112,8)))
                v4=getattr(args,'station_v4',False)
                capacity=10 if v4 else 4
                if (write_count-read_count)&0xffffffff > capacity:
                    raise RuntimeError('SRAM record ring counters invalid; stopping without consuming overwritten data')
                report.setdefault("raw_records",[])
                dense_records=[]
                while read_count!=write_count:
                    record=ring[(read_count%10)*112:(read_count%10+1)*112] if v4 else debug.read(DATA+380+(read_count%capacity)*280,280)
                    sequence,crc,rssi,channel,length,partial=struct.unpack_from("<4I2H",record)
                    report["raw_records"].append({"sequence":sequence,"crc":crc,
                        "tx_generation":(crc>>16) if getattr(args,'station_v4',False) else None,
                        "source_chip_id":struct.unpack_from('<I',record,100)[0] if v4 else None,
                        "roy_cycles":struct.unpack_from('<I',record,104)[0] if v4 else None,
                        "hardware_crc_valid":bool(profile['crccnf']) and not bool(partial&1),
                        "host_read_monotonic_ns":time.monotonic_ns(),
                        "host_read_unix_ns":time.time_ns(),
                        "timestamp_scope":"SWD host read time, not RF arrival time",
                        "callback_observation_us":struct.unpack_from("<H",record,108 if v4 else 278)[0] if b"ROYTIMEDRX1" in args.module.read_bytes() or getattr(args,"station_three",False) else None,
                        "callback_time_scope":"Slot-relative TIMER0 sample when callback records END; not RF-arrival timestamp",
                        "rssi":rssi&255 if getattr(args,'station_three',False) else rssi,"channel":channel,"partial":bool(partial&1),
                        "profile_index":(rssi>>8) if getattr(args,'station_three',False) else (partial>>8) if getattr(args,'swift_multi',False) else None,
                        "rx_pipe_match":(partial>>8) if getattr(args,'swift_dual',False) else None,
                        "slot_number_mod32768":(partial>>1) if b"ROYRXSLOT1" in args.module.read_bytes() or getattr(args,'station_service',False) else None,
                        "dma_hex":record[20:20+length].hex()})
                    if service:
                        if dense_frame and station_config.get('dense_mailbox_batch',False):dense_records.append(report["raw_records"][-1])
                        else:runtime.record(report["raw_records"][-1])
                        report["raw_records"] = report["raw_records"][-1000:]
                    read_count=(read_count+1)&0xffffffff
                if not v4:debug.write(DATA+116,struct.pack("<I",read_count))
                if service and dense_frame and station_config.get('dense_mailbox_batch',False):runtime.record_batch(dense_records)
            if sample[7]==3 and not args.counter_only and not args.native_shell:
                invoke(debug,entry,1)
            if sample[7]==3 and args.native_shell and args.repeat_host:
                invoke_native(debug,entry,1,args.disable_mpu,args.reserve_mpu)
                report["host_retries"]=report.get("host_retries",0)+1
            time.sleep(.005 if getattr(args,'station_v4',False) else .02)
        if service:
            runtime.publish(report["samples"][-1][3] if report["samples"] else 0, False)
            for sig, handler in previous_handlers.items(): signal.signal(sig, handler)
        report["slots_granted"]=report["samples"][-1][3] if report["samples"] else 0
        if getattr(args,'ir_tx_probe',False):
            attempted,completed=struct.unpack('<2I',debug.read(DATA+1648,8))
            report['tx_diagnostics']={'attempted':attempted,'end_observed':completed,
                'maximum_commands':40,'identity_candidate_le':'OWNER_PRIVATE_ID_REQUIRED',
                'command_delivery_verified':False,'led_activation_verified':False}
        if (getattr(args,'station_v4',False) and station_config.get('service_poll')
                and hashlib.sha256(module).hexdigest() in ('624e6ab6b1b951aab909214105ddea2876bc2946ac69b7f67a0978eed8ded120','d68b808dff0ca1a172d76a32b7e52209914172f52d98a2523290e3d0802adb4a','b14a214100bbf00c5bd25592c753eedaa4326375343699d6748933c35be2b11a')):
            report['actual_service27_tx_end']=struct.unpack('<I',debug.read(DATA+1912,4))[0]
        if getattr(args,'swift_poll',False):
            attempted,completed=struct.unpack('<2I',debug.read(DATA+1648,8))
            report['tx_diagnostics']={'attempted':attempted,'end_observed':completed,
                'maximum_polls':1000 if getattr(args,'station_three',False) else 320 if getattr(args,'station_sequence',False) else 12 if getattr(args,'station_handover',False) else 40,'application_payload_bytes':[1,9,21,33] if getattr(args,'station_sequence',False) else 21 if getattr(args,'station_offer',False) else 9 if getattr(args,'poll_identity',False) else 3 if args.poll_noop else 0,
                'scope':'RF TX completion; response attribution requires separate evidence'}
            if getattr(args,'poll_fast_rx',False):
                report['tx_diagnostics']['hardware_rx_handovers']=struct.unpack('<I',debug.read(DATA+1656,4))[0]
            if getattr(args,'station_handover',False):
                report['station_handover']={'bootstrap_offer_slot_limit':None if getattr(args,'station_sequence',False) else 12,
                    'working_base':'4d06b671' if getattr(args,'station_target','third')=='first' else '84096f68',
                    'working_prefix':0 if getattr(args,'station_target','third')=='first' else 64,
                    'working_channels':[2],
                    'scope':'Configured receiver stages; not proof of tracker transition',
                    'sequence_experiment':getattr(args,'station_sequence',False),
                    'sequence':'discovery/offer/three-working-polls' if getattr(args,'station_sequence',False) else None,
                    'working_commands':[9,17,25,19,4,10] if getattr(args,'station_sequence',False) else [],
                    'phase_value':0 if getattr(args,'station_sequence',False) else None}
                for record in report['raw_records']:
                    slot=record.get('slot_number_mod32768')
                    bootstrap=record['channel'] in [1,37,77] if getattr(args,'station_three',False) else slot is not None and ((slot-1)%5<2 if getattr(args,'station_sequence',False) else slot<=12)
                    record['receiver_stage']='bootstrap' if bootstrap else 'working' if slot is not None else 'unknown'
        if getattr(args,'station_three',False):
            report['peers_with_imu_bits']=struct.unpack('<I',debug.read(DATA+1660,4))[0]
        if args.rx_only:
            address_slots,ends,crc_errors,last_channel=struct.unpack('<4I',debug.read(DATA+1612,16))
            report['rx_diagnostics']={'slots_with_address_event':address_slots,
                'end_events':ends,'crc_errors':crc_errors,'last_actual_channel':last_channel,
                'scan_channels':scan_channels}
            report['rx_diagnostics']['state_at_timeout']=struct.unpack('<I',debug.read(DATA+1628,4))[0]
            report['rx_diagnostics']['end_seen_at_timeout']=struct.unpack('<I',debug.read(DATA+1640,4))[0]
            last_signal,signals=struct.unpack('<2I',debug.read(DATA+1632,8))
            report['mpsl_signals']={'last':last_signal,'seen':[bit for bit in range(11) if signals&(1<<bit)]}
    finally:
        # Exceptions can precede the normal status assignment. Preserve observed
        # MCU slots so cleanup publication cannot misleadingly report zero RF.
        if report.get('samples'):
            report['slots_granted']=report['samples'][-1][3]
        try:
            if args.counter_only or args.timer_only or args.rx_only:
                clear_traps(debug)
            # An interrupted invocation may have opened a session before throwing.
            if state_initialized and invocation_started:
                session_context=debug.read(MPSL,48)
                callback=struct.unpack_from("<I",session_context,32)[0]
                opened=session_is_active(session_context) and CODE<=(callback & ~1)<CODE+2045
            if opened:
                debug.write(DATA+4,struct.pack("<I",1))
                deadline=time.monotonic()+.3
                while struct.unpack_from("<I",debug.read(DATA+28,4))[0]==2:
                    if time.monotonic()>deadline: raise RuntimeError("Radio slot has not finished; retaining module RAM")
                    time.sleep(.005)
                if args.native_shell: invoke_native(debug,entry,2,args.disable_mpu,args.reserve_mpu)
                else: invoke(debug,entry,2,auto_return=args.auto_return)
                deadline=time.monotonic()+.3
                while struct.unpack_from("<I",debug.read(DATA+28,4))[0]!=0:
                    if time.monotonic()>deadline: raise RuntimeError("MPSL close not acknowledged; retaining module RAM")
                    time.sleep(.005)
            if saved_code is not None:
                if session_is_active(debug.read(MPSL,48)):
                    raise RuntimeError("MPSL session remains active; retaining module RAM")
                report["session_closed_verified"]=True
                if trace_controls is not None:
                    for address,value in trace_controls.items():
                        debug.write(int(address,16),bytes.fromhex(value))
                        if debug.read(int(address,16),4)!=bytes.fromhex(value):
                            raise RuntimeError('Roy trace control restoration failed')
                    report['trace_controls_restored_verified']=True
                if args.native_shell and debug.read(SHELL_HOOK,4)!=b"\0"*4:
                    raise RuntimeError("Native callback still referenced; retaining code")
                if saved_static_word is not None:
                    debug.write(MPU_STATIC_WORD,saved_static_word)
                    time.sleep(.03)  # PendSV restores stock guard in region 2.
                    if debug.read(MPU_STATIC_WORD,4)!=saved_static_word:
                        raise RuntimeError("Static MPU count restoration failed")
                    report["mpu_static_count_restored"]=True
                debug.write(CODE,saved_code)
                debug.write(DATA,saved_data)
                debug.write(RTT+16,saved_counts)
                if (debug.read(CODE,len(saved_code))!=saved_code or debug.read(DATA,2048)!=saved_data
                        or debug.read(RTT+16,8)!=saved_counts):
                    raise RuntimeError("RAM checkpoint restoration failed readback")
                report["ram_restored_verified"]=True
                report["unloaded"]=True
            restore_permissions(debug,permissions)
            if args.disable_mpu or args.reserve_mpu:
                debug.command("mww 0xe000ed94 5")
                if struct.unpack('<I',debug.read(0xe000ed94,4))[0]!=5:
                    raise RuntimeError('MPU control restoration failed readback')
            report["mpu_restored"]=True
            if original_debug_enable is not None:
                debug.command(f"mww 0xe000edf0 {0xa05f0000|original_debug_enable:#x}")
        finally:
            try:
                if runtime is not None:
                    runtime.publish(report.get('slots_granted', 0), False)
                    if getattr(runtime,'ota',None) is not None:
                        report['tracker_ota']=runtime.ota.status()
                        report['flash_written_scope']='Roy flash only; tracker OTA is reported separately'
            finally:
                # Independent cleanup: one failing resource must not leak the others.
                try:
                    for sig, handler in previous_handlers.items():
                        signal.signal(sig, handler)
                finally:
                    try:debug.close()
                    finally:
                        try:
                            if optics is not None:optics.close()
                        finally:
                            try:
                                if guard is not None:guard.close()
                            finally:
                                args.out.write_text(json.dumps(report,indent=2)+"\n")
    return report


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--stock",type=Path,required=True)
    parser.add_argument("--module",type=Path,required=True)
    parser.add_argument("--out",type=Path,required=True)
    parser.add_argument("--log",type=Path,required=True)
    parser.add_argument("--run",action="store_true")
    parser.add_argument("--counter-only",action="store_true",help="Use ROY_COUNTER_ONLY build and capture faults; enable no radio/timers")
    parser.add_argument("--timer-only",action="store_true",help="Use timer callback build, no RF; capture early faults")
    parser.add_argument("--rx-only",action="store_true",help="Bounded passive raw RX with native shell, MPU reservation and fault capture; no TX")
    parser.add_argument("--auto-return",action="store_true",help="Use ROY_AUTO_RETURN build; no debugger stop after firmware API")
    parser.add_argument("--native-shell",action="store_true",help="Use ROY_NATIVE_CALL build; invoke from stock shell Thread without halting CPU")
    parser.add_argument("--repeat-host",action="store_true",help="Retry EARLIEST from native shell after BLOCKED/IDLE; diagnostic polling only")
    parser.add_argument("--trap-only",action="store_true",help="Execute debug return only; call no MPSL APIs")
    parser.add_argument("--disable-mpu",action="store_true",help="Diagnostic only: transiently disable MPU; restore stock CTRL=5")
    parser.add_argument("--reserve-mpu",action="store_true",help="Use ROY_MPU_RESERVE build: temporary static executable region 2, MPU enabled during callbacks")
    parser.add_argument("--inspect-mpu",action="store_true",help="Inspect MPU and immediately resume; load no module")
    parser.add_argument("--profile",type=Path,help="Explicit raw RX register profile; omit for scheduling-only test")
    parser.add_argument("--channels",default='',help="Comma-separated RX channels to rotate between samples")
    parser.add_argument('--swift-multi',action='store_true',help='Use separately compiled ROY_SWIFT_MULTI RX module')
    parser.add_argument('--swift-sweep',action='store_true',help='Passive firmware-owned even-channel sweep with tagged full slot0 profile')
    parser.add_argument('--swift-dual',action='store_true',help='Tagged passive working/bootstrap addresses on six reviewed initial channels')
    parser.add_argument('--swift-poll',action='store_true',help='Active bounded zero-payload poll; requires separately tagged module')
    parser.add_argument('--poll-fast-rx',action='store_true',help='Explicit tagged hardware TX-to-RX handover for identity probe')
    parser.add_argument('--poll-identity',action='store_true',help='Explicit length9 identity request; bootstrap tagged module only')
    parser.add_argument('--station-offer',action='store_true',help='Explicit targeted21-byte bootstrap negotiation; tracker pairing storage authorized')
    parser.add_argument('--station-three',action='store_true',help='Private three-ID experimental station; explicit version-gated module')
    parser.add_argument('--station-target',choices=['first','third'],default='third',help='Private laboratory sequence profile; must match module tag')
    parser.add_argument('--station-sequence',action='store_true',help='Explicit experimental discovery/offer/working command sequence, max320 TX')
    parser.add_argument('--station-handover',action='store_true',help='Tagged station offer then passive third-address reception')
    parser.add_argument('--poll-bootstrap',action='store_true',help='Explicit bounded bootstrap no-op poll on1/37/77; tagged module only')
    parser.add_argument('--poll-active-mask',action='store_true',help='Explicit bounded poll on observed mask2/6/8/10/12; tagged module only')
    parser.add_argument('--poll-noop',action='store_true',help='Explicit alternate tagged poll with complete three-byte opcode0 header')
    parser.add_argument('--ir-tx-probe',action='store_true',help='Explicit active fixed-command probe, max40 packets; requires tagged build')
    parser.add_argument("--station-service",action="store_true")
    parser.add_argument('--station-v4',action='store_true')
    parser.add_argument('--tracker-ota-job',type=Path,help='Explicit single-target FW189 OTA job; never taken from service config')
    parser.add_argument('--pause-before-ota-start',action='store_true',
                        help='Validate live target but do not create or begin an OTA sender')
    parser.add_argument('--authorize-unverified-fork',action='store_true',
                        help='Explicit user-accepted no-SWD risk; exact iter05 image and slot0 target only')
    parser.add_argument('--authorize-stock-reflash',action='store_true',
                        help='User-authorized reflash of the frozen stock FW189 image only; patched images stay blocked')
    parser.add_argument('--tracking-watch',type=Path,default=Path(__file__).parents[2]/'frame_app'/'tracking_watch')
    parser.add_argument('--mpsl-priority',type=int,choices=(0,1),default=1)
    parser.add_argument("--live-state",type=Path)
    parser.add_argument("--peer-registry",type=Path)
    parser.add_argument("--station-config",type=Path)
    parser.add_argument("--duration",type=float,default=.5)
    parser.add_argument("--slot-us",type=int,default=500)
    parser.add_argument("--interval-us",type=int,default=10000)
    parser.add_argument("--steady-interval-us",type=int,default=5000,help="ABI4 cadence once all three peers stream")
    args=parser.parse_args()
    if args.tracker_ota_job is not None and not (args.run and args.station_v4 and args.station_service and args.station_three):
        parser.error('Tracker OTA requires explicit --run and the guarded ABI4 station lifecycle')
    if args.station_three and (not args.peer_registry or not args.station_config):
        parser.error('Three-peer ABI3 requires --peer-registry and --station-config; no embedded private identities')
    if b'ROYSTATIONTHREE1' in args.module.read_bytes():
        parser.error('Old three-peer RAM ABI1 requires its archived loader; current ABI is2')
    v4_tag=b'ROYSTATIONTHREE4' in args.module.read_bytes()
    if v4_tag!=args.station_v4:parser.error('ABI4 requires matching --station-v4 module')
    service_tag=b'ROYSTATIONTHREE3' in args.module.read_bytes() or v4_tag
    if service_tag != args.station_service or args.station_service and not args.live_state:
        parser.error('Service requires ABI3 and explicit --live-state')
    three_sequence=b'ROYSTATIONTHREE2' in args.module.read_bytes() or service_tag
    if three_sequence != args.station_three or args.station_three and not args.station_sequence:
        parser.error('Three tracker station requires matching tag and explicit --station-three --station-sequence')
    first_sequence=b'ROYSTATIONONE1' in args.module.read_bytes()
    sequence_tag=b'ROYSTATIONSEQ1' in args.module.read_bytes() or first_sequence or three_sequence
    if args.station_sequence and not args.station_three and (args.station_target=='first') != first_sequence:
        parser.error('Station target must match the compiled private sequence profile')
    if sequence_tag != args.station_sequence or args.station_sequence and not args.station_handover:
        parser.error('Station sequence requires matching tag and explicit --station-handover --station-sequence')
    handover_tag=b'ROYSTATIONNEXT1' in args.module.read_bytes() or sequence_tag
    if handover_tag != args.station_handover or args.station_handover and not args.station_offer:
        parser.error('Station handover requires explicit matching tag and --station-offer')
    if (b'ROYSTATIONOFFER1' in args.module.read_bytes() or handover_tag) != args.station_offer:
        parser.error('Station offer requires explicit --station-offer and matching module tag')
    if args.station_offer and not (args.poll_identity and args.poll_bootstrap and args.poll_fast_rx and args.swift_poll and args.poll_noop and args.native_shell and args.reserve_mpu):
        parser.error('Station offer requires reviewed native bootstrap identity/fastRX lifecycle')
    if b'ROYPAIREDGATEDRAFT1' in args.module.read_bytes():
        parser.error('Paired gate draft is compile-only: downstream persistence audit incomplete')
    tagged_tick=b'ROYRXPOLLTICK1' in args.module.read_bytes()
    if tagged_tick:
        if not (args.rx_only or args.swift_poll) or not args.native_shell or not args.reserve_mpu or not args.profile or args.slot_us!=2000 or args.swift_multi or args.swift_dual or args.swift_sweep or args.ir_tx_probe:
            parser.error('Poll-tick module requires passive native-shell reserved MPU full-profile 2000us RX')
        p=passive_profile(args.profile)
        expected=(0x552c6a1e,0xc0,2) if args.poll_bootstrap else (0x4d06b671,0,2)
        actual=(p['base0'],p['prefix0'],p['crccnf'])
        third_passive=args.rx_only and not args.swift_poll and actual==(0x84096f68,0x40,2)
        if actual!=expected and not third_passive:
            parser.error('Poll-tick module restricted to selected reviewed full address')
    tagged_tx=b'ROYIRTXPROBE1' in args.module.read_bytes()
    if (b'ROYSWIFTDUAL1' in args.module.read_bytes()) != args.swift_dual:
        parser.error('Dual module requires explicit --swift-dual and vice versa')
    if args.swift_dual:
        if not args.rx_only or args.swift_multi or args.swift_sweep or args.swift_poll or args.ir_tx_probe or args.channels or not args.profile:
            parser.error('Dual receive requires passive full profile without other modes')
        p=passive_profile(args.profile)
        if (p['base0'],p['prefix0'],p['crccnf'])!=(0x4d06b671,0,2) or args.slot_us!=2000:
            parser.error('Dual receive requires full slot0 CRC profile and2000us slot')
    if (b'ROYSWIFTPOLLNOOP1' in args.module.read_bytes() or handover_tag) != args.poll_noop or args.poll_noop and not args.swift_poll:
        parser.error('Noop poll requires its tagged module and explicit --swift-poll --poll-noop')
    if (b'ROYSWIFTPOLLFAST1' in args.module.read_bytes() or three_sequence) != args.poll_fast_rx or args.poll_fast_rx and not (args.poll_identity or (args.swift_poll and args.poll_noop and not args.poll_bootstrap)):
        parser.error('Fast RX requires tagged identity or reviewed working-address no-op probe')
    if (b'ROYSWIFTPOLLID1' in args.module.read_bytes() or handover_tag) != args.poll_identity or args.poll_identity and not args.poll_bootstrap:
        parser.error('Identity poll requires tagged bootstrap module and --poll-identity')
    if (b'ROYSWIFTPOLLBOOT1' in args.module.read_bytes() or three_sequence) != args.poll_bootstrap:
        parser.error('Bootstrap poll requires explicit --poll-bootstrap and matching module')
    if args.poll_bootstrap and (not args.swift_poll or not args.poll_noop or args.poll_active_mask):
        parser.error('Bootstrap requires --swift-poll --poll-noop without active-mask')
    if (b'ROYSWIFTPOLLMASK1' in args.module.read_bytes()) != args.poll_active_mask:
        parser.error('Observed-mask poll module requires explicit --poll-active-mask and vice versa')
    if args.poll_active_mask and (not args.swift_poll or not args.poll_noop):
        parser.error('Observed-mask poll requires explicit --swift-poll --poll-noop')
    if (b'ROYSWIFTPOLL1' in args.module.read_bytes() or three_sequence) != args.swift_poll:
        parser.error('Poll module requires explicit --swift-poll and vice versa')
    if args.swift_poll:
        if args.rx_only or args.counter_only or args.timer_only or args.swift_multi or args.swift_sweep or args.ir_tx_probe or args.channels or not args.profile:
            parser.error('Poll cannot use passive/IR/scheduling/scanning flags')
        p=passive_profile(args.profile)
        expected=(0x552c6a1e,0xc0,2) if args.poll_bootstrap else (0x4d06b671,0,2)
        # ABI4 replies end within 264..474us of slot start (hardware 2026-10-06);
        # a 1000us slot keeps a 2x margin and halves air time for Valve controllers.
        slot_ok=500<=args.slot_us<=2000 if args.station_v4 else args.slot_us==2000
        if (p['base0'],p['prefix0'],p['crccnf'])!=expected or not slot_ok:
            parser.error('Poll requires selected reviewed full CRC profile and2000us slot (ABI4: 1000..2000us)')
        args.rx_only=True
    if (b'ROYSWIFTSWEEP1' in args.module.read_bytes()) != args.swift_sweep:
        parser.error('Sweep module requires explicit --swift-sweep and vice versa')
    if args.swift_sweep:
        if not args.rx_only or args.swift_multi or args.ir_tx_probe or args.channels or not args.profile:
            parser.error('Sweep requires passive full profile and no other scanning mode')
        p=passive_profile(args.profile)
        if (p['base0'],p['prefix0'],p['crccnf'])!=(0x4d06b671,0,2) or args.slot_us!=2000:
            parser.error('Sweep requires confirmed slot0 full CRC profile and2000us slot')
    if tagged_tx != args.ir_tx_probe:
        parser.error('Active TX build requires explicit --ir-tx-probe and vice versa')
    if args.ir_tx_probe:
        if args.rx_only or args.counter_only or args.timer_only or args.swift_multi or args.channels or not args.profile:
            parser.error('Active probe cannot use passive/scheduling/scanning flags')
        p=passive_profile(args.profile)
        if (p['base0'],p['prefix0'],p['frequency'])!=(0x4d06b671,0,80) or p['crccnf']!=2 or args.slot_us!=2000:
            parser.error('Active probe requires confirmed slot0 full CRC profile and2000us slot')
        args.rx_only=True  # Reuse native-shell/fault/restoration lifecycle, not passive semantics.
    if args.swift_multi:
        if b'ROYSWIFTMULTI1' not in args.module.read_bytes():
            parser.error('Multi RX flag requires the tagged multi-profile build')
        if not args.rx_only or not args.profile or args.channels:
            parser.error('Multi RX requires raw profile and firmware-owned channels')
        p=passive_profile(args.profile)
        if (p['mode'],p['pcnf0'],p['pcnf1'],p['base0'],p['prefix0'],p['crccnf'])!=(4,0,0x01022828,0x4d060000,0,0) or p['frequency'] not in (2,80):
            parser.error('Multi RX requires the exact paired-prefix0 raw profile')
    if sum((args.counter_only,args.timer_only,args.rx_only))>1:
        parser.error("Choose counter-only, timer-only or rx-only")
    if args.reserve_mpu and (not (args.counter_only or args.timer_only or args.rx_only) or not args.native_shell or args.disable_mpu):
        parser.error("MPU reservation diagnosis requires native counter/timer-only, without --disable-mpu")
    if (args.counter_only or args.timer_only) and args.profile:
        parser.error("Scheduling-only build cannot enable a radio profile")
    if args.rx_only and (not args.profile or not args.reserve_mpu or not args.native_shell):
        parser.error("Passive RX requires explicit profile, native shell and MPU reservation")
    if not args.run and not args.inspect_mpu: parser.error("Runtime RAM changes require explicit --run")
    minimum_slot=100 if args.counter_only else 300
    if not minimum_slot<=args.slot_us<=2000 or not args.slot_us+1000<=args.interval_us<=100000:
        parser.error("Invalid diagnostic timing")
    if args.duration < 0 or not args.station_service and not 0<args.duration<=10: parser.error("Diagnostic duration must be <=10 seconds")
    report=run(args)
    args.out.write_text(json.dumps(report,indent=2)+"\n")
    print(json.dumps({key:report.get(key) for key in ("slots_granted","session_closed_verified","ram_restored_verified","mpu_restored","unloaded")}))


if __name__=="__main__": main()
