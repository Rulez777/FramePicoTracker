# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Offline FW189 fork: default stock cadence, boot-release gesture selects Frame.

Adds Thumb trampolines to the end of the app. No bootloader/signature changes,
device I/O or implicit OTA. Physical button timing and boot support unverified.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
from .tracker_grid_patch import BASE, IMAGE_BYTES, STOCK_SHA256, movw

MODE = 0x20005150
APPEND = (BASE + IMAGE_BYTES + 7) & ~7
MOVWS = [(0x6214, 4, 18910, 12000), (0x6218, 0, 4727, 3000),
         (0x630a, 0, 18910, 12000), (0x630e, 1, 4727, 3000),
         (0x6cde, 0, 3751, 2024), (0x6ce4, 0, 3355, 1628)]
DOUBLES = [(0x6cba, 'period'), (0x6cc4, 'normal'), (0x6d2c, 'period'),
           (0x6d36, 'alternate'), (0x6f64, 'period')]


def branch(site, target, link=False):
    displacement = (target & ~1) - (site + 4)
    if displacement & 1 or not -(1 << 24) <= displacement < (1 << 24):
        raise ValueError('Thumb branch out of range/alignment')
    value = displacement & 0x1ffffff
    s, i1, i2 = (value >> 24) & 1, (value >> 23) & 1, (value >> 22) & 1
    j1, j2 = 1 ^ i1 ^ s, 1 ^ i2 ^ s
    return struct.pack('<HH', 0xf000 | (s << 10) | ((value >> 12) & 0x3ff),
                       (0xd000 if link else 0x9000) | (j1 << 13) | (j2 << 11) | ((value >> 1) & 0x7ff))


def assembly():
    lines = ['.syntax unified', '.cpu cortex-m4', '.thumb', '.section .text',
             f'.equ mode_flag, {MODE}']
    def function(name):
        lines.extend([f'.global {name}', f'.type {name}, %function', '.thumb_func', name + ':'])
    for va, rd, old, new in MOVWS:
        function(f'select_{va:x}')
        lines.extend(['push {r2,r3}', 'mrs r2, APSR', 'ldr r3, =mode_flag',
                      'ldrb r3, [r3]', 'cmp r3, #1', 'ite eq',
                      f'movweq r{rd}, #{new}', f'movwne r{rd}, #{old}',
                      'msr APSR_nzcvq, r2', 'pop {r2,r3}', f'ldr.w pc, .Lret_{va:x}',
                      '.balign 4', f'.Lret_{va:x}:', f'.word {va + 5}', '.ltorg'])
    for va, table in DOUBLES:
        function(f'select_{va:x}')
        lines.extend(['push {r0,r1}', 'mrs r1, APSR', 'ldr r0, =mode_flag',
                      'ldrb r0, [r0]', 'cmp r0, #1', 'it eq',
                      f'ldreq r3, =frame_{table}', 'ldrd r2,r3,[r3]',
                      'msr APSR_nzcvq, r1', 'pop {r0,r1}', f'ldr.w pc, .Lret_{va:x}',
                      '.balign 4', f'.Lret_{va:x}:', f'.word {va + 5}', '.ltorg'])
    function('profile_boot_entry')
    lines.extend(['push {r0,r1,r2,r3}', 'mrs r1, APSR', 'ldr r2, =mode_flag',
                  'movs r3, #0', 'strb r3,[r2]', 'msr APSR_nzcvq,r1',
                  'pop {r0,r1,r2,r3}', 'push.w {r0,r1,r2,r4,r5,r6,r7,lr}', 'ldr r6, =0x200005bc',
                  'ldr.w pc, .Lboot_return', '.balign 4', '.Lboot_return:', '.word 0xc3f1', '.ltorg'])
    function('profile_boot_hold')
    lines.extend(['strh r3,[r5]', 'cmp r4,#0', 'bne 2f',
                  'cmp r3,#150', 'blo 1f', 'movs r0,#1',
                  'ldr.w pc, .Lhold_done', '1:', 'ldr.w pc, .Lhold_return',
                  '2:', 'cmp r2,#3', 'ldr.w pc, .Lhold_native', '.balign 4',
                  '.Lhold_done:', '.word 0xc499', '.Lhold_return:', '.word 0xc48b',
                  '.Lhold_native:', '.word 0xc449', '.ltorg'])
    function('profile_boot_release')
    lines.extend(['push {r1,r2}', 'cmp r4,#0', 'bne 1f', 'ldr r1, =0x20001ff2', 'ldrh r1,[r1]',
                  'cmp r1,#100', 'blo 1f', 'cmp r1,#150', 'bhs 1f',
                  'ldr r1, =mode_flag', 'movs r2,#1', 'strb r2,[r1]',
                  '1:', 'pop {r1,r2}', 'movs r0,#1', 'ldr.w pc, .Lrelease_return',
                  '.balign 4', '.Lrelease_return:', '.word 0xc499', '.ltorg'])
    function('profile_power_off')
    lines.extend(['push {r0,r1,r12,lr}', 'ldr r12, =0x5fe1', 'blx r12',
                  'mrs r1, APSR', 'ldr r0, =mode_flag', 'movs r12,#0',
                  'strb r12,[r0]', 'msr APSR_nzcvq,r1', 'pop {r0,r1,r12,pc}', '.ltorg'])
    function('profile_command')
    lines.extend(['push {r0,r1,r2,r3,r12,lr}', 'mrs r12,APSR',
                  'cmp r1,#18', 'bne 3f', 'ldrb r2,[r0]', 'cmp r2,#0x7e',
                  'bne 3f', 'ldrb r2,[r0,#1]', 'cmp r2,#1', 'bne 3f',
                  'pop {r0,r1,r2,r3,r12,lr}', 'push {r4,r5,r6,lr}',
                  'sub sp,#24', 'mov r4,r0', 'mov r5,sp',
                  'movs r2,#0xfe', 'strb r2,[r5]', 'movs r2,#1', 'strb r2,[r5,#1]',
                  'movs r6,#2', '1:', 'ldrb r2,[r4,r6]', 'strb r2,[r5,r6]',
                  'adds r6,#1', 'cmp r6,#18', 'blo 1b',
                  'ldr r4,=0x20000efe', 'movs r6,#0', '2:',
                  'ldrb r2,[r4,r6]', 'add r3,r5,r6', 'strb r2,[r3,#18]',
                  'adds r6,#1', 'cmp r6,#4', 'blo 2b',
                  'ldr r3,=mode_flag', 'ldrb r2,[r3]', 'cmp r2,#1', 'ite eq',
                  'moveq r2,#1', 'movne r2,#0', 'strb r2,[r5,#22]',
                  'movs r2,#1', 'strb r2,[r5,#23]', 'mov r0,r5', 'movs r1,#24',
                  'ldr r3,=0x9369', 'blx r3', 'add sp,#24', 'pop {r4,r5,r6,pc}',
                  '3:', 'msr APSR_nzcvq,r12', 'pop {r0,r1,r2,r3,r12,lr}',
                  'push {r4,r5,lr}', 'sub sp,#0x2c',
                  'ldr.w pc,.Lcommand_return', '.balign 4', '.Lcommand_return:',
                  '.word 0x8d1b', '.ltorg'])
    for name, value in [('period', 3000.), ('normal', 1810.5), ('alternate', 2207.)]:
        lines.extend(['.balign 8', f'frame_{name}:', f'.double {value}'])
    return '\n'.join(lines) + '\n'


def extract_elf(path):
    data = Path(path).read_bytes()
    if data[:6] != b'\x7fELF\x01\x01':raise ValueError('Expected little-endian ELF32')
    start = struct.unpack_from('<I', data, 32)[0]
    stride, count = struct.unpack_from('<HH', data, 46)
    sections = [struct.unpack_from('<10I', data, start + i * stride) for i in range(count)]
    loaded = [s for s in sections if s[1] == 1 and s[2] & 2 and s[5]]
    if not loaded or min(s[3] for s in loaded) != APPEND:raise ValueError('Unexpected fork ELF layout')
    code = bytearray(b'\xff' * (max(s[3] + s[5] for s in loaded) - APPEND))
    for s in loaded:code[s[3] - APPEND:s[3] - APPEND + s[5]] = data[s[4]:s[4] + s[5]]
    symbols = {}
    for s in sections:
        if s[1] != 2:continue
        strings = sections[s[6]];names = data[strings[4]:strings[4] + strings[5]]
        for pos in range(s[4], s[4] + s[5], s[9]):
            name, value, _, _, _, _ = struct.unpack_from('<IIIBBH', data, pos)
            text = names[name:].split(b'\0', 1)[0].decode()
            if text:symbols[text] = value
    return bytes(code), symbols


def build(stock, directory, compiler='clang', linker='ld.lld'):
    if len(stock) != IMAGE_BYTES or hashlib.sha256(stock).hexdigest() != STOCK_SHA256:
        raise ValueError('Only frozen FW189 stock is supported')
    directory = Path(directory);directory.mkdir(parents=True, exist_ok=False)
    source = directory/'dual-mode.S';source.write_bytes(assembly().encode())
    script = directory/'dual-mode.ld'
    script.write_text(f'ENTRY(profile_boot_entry)\nSECTIONS {{ .text {APPEND:#x} : {{ *(.text*) *(.rodata*) }} /DISCARD/ : {{ *(.ARM.exidx*) *(.ARM.extab*) *(.comment*) *(.note*) }} }}\n')
    obj = directory/'dual-mode.o';elf = directory/'dual-mode.elf'
    commands = [[str(compiler), '--target=arm-none-eabi', '-mcpu=cortex-m4', '-mthumb', '-c', str(source), '-o', str(obj)],
                [str(linker), '-T', str(script), str(obj), '-o', str(elf)]]
    for command in commands:subprocess.run(command, check=True)
    code, symbols = extract_elf(elf)
    result = bytearray(stock);edits = []
    def change(va, after, reason):
        offset = va - BASE;before = bytes(result[offset:offset + len(after)])
        result[offset:offset + len(after)] = after
        edits.append(dict(va=hex(va), offset=offset, before=before.hex(), after=after.hex(), reason=reason))
    for va, rd, old, _ in MOVWS:
        if stock[va - BASE:va - BASE + 4] != movw(rd, old):raise ValueError('Unexpected MOVW site')
        change(va, branch(va, symbols[f'select_{va:x}']), 'Stock/Frame MOVW selector; LR and APSR preserved')
    for va, _ in DOUBLES:
        if stock[va - BASE:va - BASE + 4] != bytes.fromhex('d3e90023'):
            raise ValueError(f'Unexpected double load at {va:#x}')
        change(va, branch(va, symbols[f'select_{va:x}']), 'Stock/Frame double selector; LR and APSR preserved')
    change(0xc3ec, branch(0xc3ec, symbols['profile_boot_entry']), 'Clear profile at native startup-button routine entry')
    change(0xc444, branch(0xc444, symbols['profile_boot_hold']), 'Ordinary resetreason0: select window ends at150 with event1; other reset reasons preserve native hold')
    change(0xc4aa, branch(0xc4aa, symbols['profile_boot_release']), 'Select Frame only at startup release count100..149')
    change(0x8d16, branch(0x8d16, symbols['profile_command']), 'Read-only command1/subcommand7e schema1 nonce16; native fallback unchanged')
    change(0xbce6, branch(0xbce6, symbols['profile_power_off'], link=True), 'Clear profile after native HFCLK stop in full power-off path')
    for va in (0x1348, 0x134c):
        if struct.unpack_from('<I', stock, va - BASE)[0] != MODE:raise ValueError('Unexpected BSS/heap boundary')
        change(va, struct.pack('<I', MODE + 8), 'Reserve eight zero-initialized bytes; heap512->504bytes, end unchanged')
    result.extend(b'\xff' * (APPEND - BASE - len(result)));result.extend(code)
    if BASE + len(result) >= 0x2f000:raise ValueError('Fork overlaps application metadata region')
    image = bytes(result)
    manifest = dict(schema=2, builder_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(), assembly_sha256=hashlib.sha256(source.read_bytes()).hexdigest(), name='fw189-stock-frame-dual-mode-experimental',
                    image_base=BASE, image_bytes=len(image), source_image_bytes=len(stock),
                    source_sha256=STOCK_SHA256, patched_sha256=hashlib.sha256(image).hexdigest(),
                    append_base=hex(APPEND), appended_code_bytes=len(code), mode_flag=hex(MODE),
                    frame_release_counts=[100, 149], ordinary_hold_limit_count=150, mode_query_subcommand=0x7e, mode_reply_subcommand=0xfe, mode_protocol_version=1, nominal_frame_gesture_s=[1.0, 1.49],
                    gesture_timing_hardware_verified=False, default_profile='stock',
                    heap_bytes_before=512, heap_bytes_after=504,
                    hardware_verified=False, bootloader_verified=False, approved_for_ota=False,
                    version_field_unchanged=True, edits=edits,
                    appended_sha256=hashlib.sha256(code).hexdigest(),
                    symbols={k:hex(v) for k,v in symbols.items() if k.startswith(('select_', 'profile_', 'frame_'))},
                    build_commands=commands, compiler=subprocess.check_output([str(compiler), '--version'], text=True))
    (directory/'fw189-dual-mode.bin').write_bytes(image)
    (directory/'fw189-stock.bin').write_bytes(stock)
    (directory/'patch-manifest.json').write_bytes((json.dumps(manifest, indent=2) + '\n').encode())
    return image, manifest


def reverse(image, manifest):
    if hashlib.sha256(image).hexdigest() != manifest['patched_sha256']:
        raise ValueError('Fork image hash mismatch')
    stock = bytearray(image[:manifest['source_image_bytes']])
    for item in manifest['edits']:
        offset = item['offset'];after = bytes.fromhex(item['after']);before = bytes.fromhex(item['before'])
        if stock[offset:offset + len(after)] != after:raise ValueError('Fork edit mismatch')
        stock[offset:offset + len(after)] = before
    if hashlib.sha256(stock).hexdigest() != STOCK_SHA256:raise ValueError('Reverse is not frozen stock')
    return bytes(stock)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('stock', type=Path);parser.add_argument('--out-dir', type=Path, required=True)
    parser.add_argument('--compiler', default='clang');parser.add_argument('--linker', default='ld.lld')
    args = parser.parse_args()
    image, manifest = build(args.stock.read_bytes(), args.out_dir, args.compiler, args.linker)
    assert reverse(image, manifest) == args.stock.read_bytes()
    print(json.dumps(dict(image_bytes=len(image), sha256=manifest['patched_sha256'], approved_for_ota=False)))
