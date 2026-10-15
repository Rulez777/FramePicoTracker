# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
"""Revision07 candidate: addressed remote mode staging at native RX boundary.

Offline only; no OTA permission. Manual boot selection and revision06 counter
repair retained. Protocol2 is deliberately separate from installed revision05.
"""
import argparse
import hashlib
import json
from pathlib import Path
from . import tracker_dual_mode_patch as parent
from .tracker_dual_mode_boot43 import assembly as counter_assembly


def assembly():
    source=counter_assembly()
    start=source.index('.global profile_command\n')
    end=source.index('.balign 8\nframe_period:',start)
    source=source[:start]+REMOTE+source[end:]
    # Pending work must never survive manual startup/full power-off.
    source=source.replace('strb r3,[r2]\nldr r2, =0x20001ff2',
                          'str r3,[r2]\nstr r3,[r2,#4]\nldr r2,=0x20000f21\nstrb r3,[r2]\nstrb r3,[r2,#1]\nstrb r3,[r2,#2]\nstrb r3,[r2,#3]\nstrb r3,[r2,#4]\nldr r2, =0x20001ff2')
    source=source.replace('strb r12,[r0]\nmsr',
                          'str r12,[r0]\nstr r12,[r0,#4]\nldr r0,=0x20000f21\nstrb r12,[r0]\nstrb r12,[r0,#1]\nstrb r12,[r0,#2]\nstrb r12,[r0,#3]\nstrb r12,[r0,#4]\nmsr')
    return source


REMOTE=r'''
.global profile_command
.type profile_command,%function
.thumb_func
profile_command:
push {r0,r1,r2,r3,r12,lr}
mrs r12,APSR
cmp r1,#18
bne .Lsetter_check
ldrb r2,[r0]
cmp r2,#0x7e
bne .Lfallback
ldrb r2,[r0,#1]
cmp r2,#1
beq .Lquery
cmp r2,#2
beq .Lquery
b .Lfallback
.Lsetter_check:
cmp r1,#24
bne .Lfallback
ldrb r2,[r0]
cmp r2,#0x7d
bne .Lfallback
ldrb r2,[r0,#1]
cmp r2,#2
bne .Lfallback
ldrb r2,[r0,#22]
cmp r2,#1
bhi .Lfallback
ldrb r2,[r0,#23]
cmp r2,#40
bhi .Lfallback
ldr r3,=0x20000efe
ldrb r2,[r3]
ldrb r3,[r0,#18]
cmp r2,r3
bne .Lfallback
ldr r3,=0x20000efe
ldrb r2,[r3,#1]
ldrb r3,[r0,#19]
cmp r2,r3
bne .Lfallback
ldr r3,=0x20000efe
ldrb r2,[r3,#2]
ldrb r3,[r0,#20]
cmp r2,r3
bne .Lfallback
ldr r3,=0x20000efe
ldrb r2,[r3,#3]
ldrb r3,[r0,#21]
cmp r2,r3
bne .Lfallback
.Lquery:
pop {r0,r1,r2,r3,r12,lr}
push {r4,r5,r6,r7,r8,lr}
sub sp,#32
mov r4,r0
mov r5,sp
mov r7,r1
movs r2,#0xfe
cmp r7,#24
it eq
moveq r2,#0xfd
strb r2,[r5]
ldrb r2,[r4,#1]
strb r2,[r5,#1]
movs r6,#2
.Lcopy_nonce:
ldrb r2,[r4,r6]
strb r2,[r5,r6]
adds r6,#1
cmp r6,#18
blo .Lcopy_nonce
ldr r3,=0x20000efe
movs r6,#0
.Lcopy_chip:
ldrb r2,[r3,r6]
add r7,r5,r6
strb r2,[r7,#18]
adds r6,#1
cmp r6,#4
blo .Lcopy_chip
// Recover query/set kind after using r7 as the chip-copy destination.
ldrb r7,[r4]
cmp r7,#0x7d
ite eq
moveq r7,#24
movne r7,#18
ldr r6,=mode_flag
cmp r7,#24
bne .Lquery_reply
// Only stage; never write live mode in the foreground command callback.
// Status 0=same mode/no pending, 1=staged, 2=OTA busy (pending cancelled).
mrs r8,PRIMASK
cpsid i
ldrb r3,[r4,#22]
strb r3,[r5,#23]
ldr r2,=0x20002240
ldrb r2,[r2]
cmp r2,#0
bne .Lota_busy
ldrb r2,[r6]
cmp r2,r3
bne .Lstage
ldr r2,=0x20000f21
ldrb r1,[r2]
ldrb r0,[r4,#23]
cmp r1,r0
bne .Lstage
ldrb r2,[r2,#1]
cmp r2,#0
beq .Lsame
.Lstage:
adds r3,#1
ldrb r2,[r4,#23]
strb r2,[r6,#2]
strb r3,[r6,#1]
movs r2,#1
b .Lstatus
.Lsame:
movs r2,#0
strb r2,[r6,#1]
b .Lstatus
.Lota_busy:
movs r2,#0
strb r2,[r6,#1]
movs r2,#2
.Lstatus:
strb r2,[r5,#24]
dmb sy
msr PRIMASK,r8
ldrb r2,[r6]
strb r2,[r5,#22]
movs r2,#7
strb r2,[r5,#25]
movs r1,#26
b .Lreply
.Lquery_reply:
ldrb r2,[r4,#1]
cmp r2,#2
bne .Lquery_snapshot
mrs r8,PRIMASK
cpsid i
.Lquery_snapshot:
ldrb r2,[r6]
cmp r2,#1
ite eq
moveq r2,#1
movne r2,#0
strb r2,[r5,#22]
ldrb r2,[r4,#1]
cmp r2,#2
ite eq
moveq r2,#7
movne r2,#1
strb r2,[r5,#23]
movs r1,#24
ldrb r2,[r4,#1]
cmp r2,#2
bne .Lreply
ldr r3,=0x20000f21
ldrb r2,[r3]
strb r2,[r5,#24]
ldrb r2,[r3,#1]
strb r2,[r5,#25]
movs r1,#26
dmb sy
msr PRIMASK,r8
.Lreply:
mov r0,r5
ldr r3,=0x9369
blx r3
add sp,#32
pop {r4,r5,r6,r7,r8,pc}
.Lfallback:
msr APSR_nzcvq,r12
pop {r0,r1,r2,r3,r12,lr}
push {r4,r5,lr}
sub sp,#0x2c
ldr.w pc,.Lcommand_return
.balign 4
.Lcommand_return: .word 0x8d1b
.ltorg

.global profile_rx_boundary
.type profile_rx_boundary,%function
.thumb_func
profile_rx_boundary:
// Replaces exactly native BL57ec at6c98. Restore its return registers/flags.
push {r4,r5,r6,lr}
mrs r5,PRIMASK
push {r0,r1,r2,r3,r12,lr}
ldr r6,=mode_flag
ldrb r6,[r6,#1]
cmp r6,#0
beq .Lhalt
// Pending transition: prevent a TIMER0 IRQ from starting TX during halt.
cpsid i
.Lhalt:
pop {r0,r1,r2,r3,r12,lr}
ldr r3,=0x57ed
blx r3
push {r0,r1,r2,r3,r12,lr}
mrs r4,APSR
cmp r6,#0
beq .Lboundary_done
cmp r0,#0
bne .Lboundary_done
ldr r6,=mode_flag
ldrb r2,[r6,#1]
cmp r2,#1
blo .Lboundary_done
cmp r2,#2
bhi .Lcancel
ldr r3,=0x20002240
ldrb r3,[r3]
cmp r3,#0
bne .Lcancel
ldr r3,=0x40001000
ldr r3,[r3,#0x550]
cmp r3,#0
bne .Lboundary_done
ldr r3,=0x20000e10
ldrb r3,[r3]
cmp r3,#0
beq .Lcancel
// fe1 is command19 AUTONOMOUS TX permission, not pairing proof. Mode must
// be selectable before normal configuration reaches19. Leave this flag alone;
// native ADDRESS rearm still observes it, and command19 remains host-controlled.
ldr r3,=0x20000f6a
ldrb r3,[r3]
cmp r3,#0
bne .Lboundary_done
// Current native RX-delay callback has reached its receive-complete branch;
// following native6cac/6d22/641c schedules RX delay from the new profile.
// Stop and clear the independent TIMER0 before committing its new selectors.
ldr r3,=0x40008000
movs r1,#1
str r1,[r3,#0x10]
str r1,[r3,#0x0c]
movs r1,#0
str r1,[r3,#0x140]
str r1,[r3,#0x144]
ldr r3,=0xe000e280
movw r1,#0x100
str r1,[r3]
subs r2,#1
strb r2,[r6]
ldr r3,=0x20000f21
ldrb r2,[r6,#2]
strb r2,[r3]
strb r2,[r3,#3]
movs r2,#0
strb r2,[r3,#1]
strb r2,[r3,#2]
movs r2,#0
strb r2,[r6,#1]
dmb sy
// Leave TIMER0 stopped. Native62e4 belongs ONLY to the RADIO ADDRESS/Rx
// path at532c; calling it while Disabled would create an unsolicited80us TX.
b .Lboundary_done
.Lcancel:
movs r2,#0
strb r2,[r6,#1]
.Lboundary_done:
dmb sy
msr PRIMASK,r5
msr APSR_nzcvq,r4
pop {r0,r1,r2,r3,r12,lr}
pop {r4,r5,r6,pc}
.ltorg
'''


def build(stock,directory):
    original=parent.assembly
    parent.assembly=assembly
    try:image,manifest=parent.build(stock,directory)
    finally:parent.assembly=original
    # Validate native call bytes before adding the new hook.
    site=0x6c98;before=stock[site-parent.BASE:site-parent.BASE+4]
    if before!=parent.branch(site,0x57ec,link=True):raise ValueError('Unexpected RX boundary')
    code,symbols=parent.extract_elf(Path(directory)/'dual-mode.elf')
    after=parent.branch(site,symbols['profile_rx_boundary'],link=True)
    image=bytearray(image);image[site-parent.BASE:site-parent.BASE+4]=after;image=bytes(image)
    manifest['edits'].append(dict(va=hex(site),offset=site-parent.BASE,before=before.hex(),after=after.hex(),reason='Apply pending profile after native receive-complete radio halt; leave TIMER0 stopped until native ADDRESS rearm'))
    manifest.update(name='fw189-dual-mode-revision07-remote44',revision=7,
                    patched_sha256=hashlib.sha256(image).hexdigest(),remote_schema=2,
                    mode_set_subcommand=0x7d,mode_set_reply_subcommand=0xfd,
                    pending_mode_address=hex(parent.MODE+1),
                    button_counter_reset_each_entry=True,approved_for_ota=False,
                    revision_builder_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest())
    Path(directory,'fw189-dual-mode.bin').write_bytes(image)
    Path(directory,'patch-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8')
    assert parent.reverse(image,manifest)==stock
    return image,manifest


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('stock',type=Path)
    parser.add_argument('--out-dir',type=Path,required=True);args=parser.parse_args()
    image,manifest=build(args.stock.read_bytes(),args.out_dir)
    print(json.dumps(dict(bytes=len(image),sha256=manifest['patched_sha256'],approved_for_ota=False)))
