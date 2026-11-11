.syntax unified
.cpu cortex-m4
.thumb
.section .text
.equ mode_flag, 536891728
.equ breadcrumb_ram, 0x20005310
.equ breadcrumb_snapshot, 0x20005328
// Each call preserves every register, APSR and incoming PRIMASK.
.macro crumb stage, event=0, power=0, command=0
push {r0,r1,r2,r3,r12,lr}
mrs r12,APSR
push {r12,lr}
movs r0,#\stage
movs r1,#\event
movs r2,#\power
movs r3,#\command
bl breadcrumb_write
pop {r12,lr}
msr APSR_nzcvq,r12
pop {r0,r1,r2,r3,r12,lr}
.endm
.global select_6214
.type select_6214, %function
.thumb_func
select_6214:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r4, #12000
movwne r4, #18910
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_6214
.balign 4
.Lret_6214:
.word 25113
.ltorg
.global select_6218
.type select_6218, %function
.thumb_func
select_6218:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r0, #3000
movwne r0, #4727
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_6218
.balign 4
.Lret_6218:
.word 25117
.ltorg
.global select_630a
.type select_630a, %function
.thumb_func
select_630a:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r0, #12000
movwne r0, #18910
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_630a
.balign 4
.Lret_630a:
.word 25359
.ltorg
.global select_630e
.type select_630e, %function
.thumb_func
select_630e:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r1, #3000
movwne r1, #4727
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_630e
.balign 4
.Lret_630e:
.word 25363
.ltorg
.global select_6cde
.type select_6cde, %function
.thumb_func
select_6cde:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r0, #2024
movwne r0, #3751
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_6cde
.balign 4
.Lret_6cde:
.word 27875
.ltorg
.global select_6ce4
.type select_6ce4, %function
.thumb_func
select_6ce4:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r0, #1628
movwne r0, #3355
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_6ce4
.balign 4
.Lret_6ce4:
.word 27881
.ltorg
.global select_6cba
.type select_6cba, %function
.thumb_func
select_6cba:
push {r0,r1}
mrs r1, APSR
ldr r0, =mode_flag
ldrb r0, [r0]
cmp r0, #1
it eq
ldreq r3, =frame_period
ldrd r2,r3,[r3]
msr APSR_nzcvq, r1
pop {r0,r1}
ldr.w pc, .Lret_6cba
.balign 4
.Lret_6cba:
.word 27839
.ltorg
.global select_6cc4
.type select_6cc4, %function
.thumb_func
select_6cc4:
push {r0,r1}
mrs r1, APSR
ldr r0, =mode_flag
ldrb r0, [r0]
cmp r0, #1
it eq
ldreq r3, =frame_normal
ldrd r2,r3,[r3]
msr APSR_nzcvq, r1
pop {r0,r1}
ldr.w pc, .Lret_6cc4
.balign 4
.Lret_6cc4:
.word 27849
.ltorg
.global select_6d2c
.type select_6d2c, %function
.thumb_func
select_6d2c:
push {r0,r1}
mrs r1, APSR
ldr r0, =mode_flag
ldrb r0, [r0]
cmp r0, #1
it eq
ldreq r3, =frame_period
ldrd r2,r3,[r3]
msr APSR_nzcvq, r1
pop {r0,r1}
ldr.w pc, .Lret_6d2c
.balign 4
.Lret_6d2c:
.word 27953
.ltorg
.global select_6d36
.type select_6d36, %function
.thumb_func
select_6d36:
push {r0,r1}
mrs r1, APSR
ldr r0, =mode_flag
ldrb r0, [r0]
cmp r0, #1
it eq
ldreq r3, =frame_alternate
ldrd r2,r3,[r3]
msr APSR_nzcvq, r1
pop {r0,r1}
ldr.w pc, .Lret_6d36
.balign 4
.Lret_6d36:
.word 27963
.ltorg
.global select_6f64
.type select_6f64, %function
.thumb_func
select_6f64:
push {r0,r1}
mrs r1, APSR
ldr r0, =mode_flag
ldrb r0, [r0]
cmp r0, #1
it eq
ldreq r3, =frame_period
ldrd r2,r3,[r3]
msr APSR_nzcvq, r1
pop {r0,r1}
ldr.w pc, .Lret_6f64
.balign 4
.Lret_6f64:
.word 28521
.ltorg
.global profile_boot_entry
.type profile_boot_entry, %function
.thumb_func
profile_boot_entry:
push {r0,r1,r2,r3}
mrs r1, APSR
ldr r2, =mode_flag
movs r3, #0
str r3,[r2]
str r3,[r2,#4]
ldr r2,=0x20000f21
strb r3,[r2]
strb r3,[r2,#1]
strb r3,[r2,#2]
strb r3,[r2,#3]
strb r3,[r2,#4]
ldr r2, =0x20001ff2
strh r3,[r2]
msr APSR_nzcvq,r1
pop {r0,r1,r2,r3}
push.w {r0,r1,r2,r4,r5,r6,r7,lr}
ldr r6, =0x200005bc
ldr.w pc, .Lboot_return
.balign 4
.Lboot_return:
.word 0xc3f1
.ltorg
.global profile_boot_hold
.type profile_boot_hold, %function
.thumb_func
profile_boot_hold:
strh r3,[r5]
cmp r4,#0
bne 2f
cmp r3,#150
blo 1f
movs r0,#1
ldr.w pc, .Lhold_done
1:
ldr.w pc, .Lhold_return
2:
cmp r2,#3
ldr.w pc, .Lhold_native
.balign 4
.Lhold_done:
.word 0xc499
.Lhold_return:
.word 0xc48b
.Lhold_native:
.word 0xc449
.ltorg
.global profile_boot_release
.type profile_boot_release, %function
.thumb_func
profile_boot_release:
push {r1,r2}
cmp r4,#0
bne 1f
ldr r1, =0x20001ff2
ldrh r1,[r1]
cmp r1,#100
blo 1f
cmp r1,#150
bhs 1f
ldr r1, =mode_flag
movs r2,#1
strb r2,[r1]
1:
pop {r1,r2}
movs r0,#1
ldr.w pc, .Lrelease_return
.balign 4
.Lrelease_return:
.word 0xc499
.ltorg
.global profile_power_off
.type profile_power_off, %function
.thumb_func
profile_power_off:
push {r0,r1,r12,lr}
ldr r12, =0x5fe1
blx r12
mrs r1, APSR
ldr r0, =mode_flag
movs r12,#0
str r12,[r0]
str r12,[r0,#4]
ldr r0,=0x20000f21
strb r12,[r0]
strb r12,[r0,#1]
strb r12,[r0,#2]
strb r12,[r0,#3]
strb r12,[r0,#4]
msr APSR_nzcvq,r1
pop {r0,r1,r12,pc}
.ltorg

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
cmp r2,#12
beq .Lquery
cmp r2,#13
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
movs r2,#11
strb r2,[r5,#25]
movs r1,#26
b .Lreply
.Lquery_reply:
ldrb r2,[r4,#1]
cmp r2,#12
bhs .Lbreadcrumb_page
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
moveq r2,#11
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
b .Lreply
.ltorg
.Lbreadcrumb_page:
mrs r8,PRIMASK
cpsid i
ldr r3,=breadcrumb_snapshot
ldr r2,[r3]
ldr r7,=0x42435231
cmp r2,r7
beq .Lbreadcrumb_source
ldr r3,=breadcrumb_ram
.Lbreadcrumb_source:
ldrb r2,[r4,#1]
cmp r2,#13
it eq
addeq r3,#10
movs r6,#0
.Lbreadcrumb_copy:
ldrb r2,[r3,r6]
add r7,r5,r6
strb r2,[r7,#22]
adds r6,#1
cmp r6,#10
blo .Lbreadcrumb_copy
dmb sy
msr PRIMASK,r8
movs r1,#32
b .Lreply
.ltorg
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
crumb 10
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
crumb 12
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
crumb 11
dmb sy
msr PRIMASK,r5
msr APSR_nzcvq,r4
pop {r0,r1,r2,r3,r12,lr}
pop {r4,r5,r6,pc}
.ltorg
.balign 8
frame_period:
.double 3000.0
.balign 8
frame_normal:
.double 1810.5
.balign 8
frame_alternate:
.double 2207.0

.global delay_6538
.type delay_6538, %function
.thumb_func
delay_6538:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r0, #6467
movwne r0, #9922
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_delay_6538
.balign 4
.Lret_delay_6538:
.word 25917
.ltorg
.global delay_65a2
.type delay_65a2, %function
.thumb_func
delay_65a2:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r0, #6467
movwne r0, #9922
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_delay_65a2
.balign 4
.Lret_delay_65a2:
.word 26023
.ltorg
.global delay_67de
.type delay_67de, %function
.thumb_func
delay_67de:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r0, #6467
movwne r0, #9922
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_delay_67de
.balign 4
.Lret_delay_67de:
.word 26595
.ltorg
.global delay_6c42
.type delay_6c42, %function
.thumb_func
delay_6c42:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r0, #6467
movwne r0, #9922
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_delay_6c42
.balign 4
.Lret_delay_6c42:
.word 27719
.ltorg
.global delay_6c5c
.type delay_6c5c, %function
.thumb_func
delay_6c5c:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r0, #6467
movwne r0, #9922
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_delay_6c5c
.balign 4
.Lret_delay_6c5c:
.word 27745
.ltorg
.global delay_6eac
.type delay_6eac, %function
.thumb_func
delay_6eac:
push {r2,r3}
mrs r2, APSR
ldr r3, =mode_flag
ldrb r3, [r3]
cmp r3, #1
ite eq
movweq r0, #7525
movwne r0, #10980
msr APSR_nzcvq, r2
pop {r2,r3}
ldr.w pc, .Lret_delay_6eac
.balign 4
.Lret_delay_6eac:
.word 28337
.ltorg
// Retained record: magic:u32, seq:u32, event:u8, command:u8,
// stage:u8, PRIMASK:u8, RESETREAS:u32, power_origin:u32.
// Heap end is moved to 0x20005310; startup BSS end stays 0x20005158.
.global breadcrumb_write
.thumb_func
breadcrumb_write:
push {r4,r5,r6,r7,lr}
mrs r7,PRIMASK
cpsid i
ldr r4,=breadcrumb_ram
ldr r5,[r4]
ldr r6,=0x42435231
cmp r5,r6
beq .Lcrumb_valid
movs r5,#0
str r5,[r4,#4]
str r5,[r4,#8]
str r5,[r4,#12]
str r5,[r4,#16]
str r6,[r4]
.Lcrumb_valid:
ldr r5,[r4,#4]
adds r5,#1
str r5,[r4,#4]
strb r0,[r4,#10]
strb r7,[r4,#11]
cmp r1,#0
it ne
strbne r1,[r4,#8]
cmp r3,#0
it ne
strbne r3,[r4,#9]
cmp r2,#0
it ne
strne r2,[r4,#16]
dmb sy
msr PRIMASK,r7
pop {r4,r5,r6,r7,pc}
.ltorg
.global breadcrumb_boot
.thumb_func
breadcrumb_boot:
push {r0,r1,r2,r3,r12,lr}
mrs r12,APSR
push {r12,lr}
// Freeze the prior session before any dispatcher query can overwrite it.
ldr r0,=breadcrumb_ram
ldr r3,=breadcrumb_snapshot
ldr r1,[r0]
ldr r2,=0x42435231
cmp r1,r2
beq .Lboot_freeze
movs r1,#0
str r1,[r0,#4]
str r1,[r0,#8]
str r1,[r0,#16]
str r2,[r0]
str r1,[r3]
str r1,[r3,#4]
str r1,[r3,#8]
str r1,[r3,#16]
b .Lboot_record
.Lboot_freeze:
ldr r1,[r0]
str r1,[r3]
ldr r1,[r0,#4]
str r1,[r3,#4]
ldr r1,[r0,#8]
str r1,[r3,#8]
ldr r1,[r0,#16]
str r1,[r3,#16]
.Lboot_record:
ldr r1,=0x40000400
ldr r1,[r1]
str r1,[r0,#12]
str r1,[r3,#12]
dmb sy
pop {r12,lr}
msr APSR_nzcvq,r12
pop {r0,r1,r2,r3,r12,lr}
ldr r0,=0x20000668
ldr r1,=0x2000514c
ldr.w pc,.Lboot_stock
.balign 4
.Lboot_stock: .word 0x126f
.ltorg
.global breadcrumb_disable_entry
.thumb_func
breadcrumb_disable_entry:
crumb 20,2
push {r4,lr}
ldr r1,=0x200008c8
ldr.w pc,.Ldisable_stock
.balign 4
.Ldisable_stock: .word 0x57f1
.ltorg
.global breadcrumb_disable_exit
.thumb_func
breadcrumb_disable_exit:
push {r4,lr}
ldr r4,=0x852d
blx r4
crumb 21,3
pop {r4,pc}
.ltorg
.global breadcrumb_disable_invalid
.thumb_func
breadcrumb_disable_invalid:
crumb 22,4
movw r0,#0x101
pop {r4,pc}
.balign 4

.ltorg
.global breadcrumb_address
.thumb_func
breadcrumb_address:
push {r4,lr}
crumb 30,1
ldr r4,=0x62e5
blx r4
crumb 31,1
pop {r4,pc}
.ltorg
.global breadcrumb_systemoff_entry
.thumb_func
breadcrumb_systemoff_entry:
crumb 40
ldr r5,=0x200005bc
push {r3,lr}
ldr.w pc,.Lsystemoff_stock
.balign 4
.Lsystemoff_stock: .word 0x8059
.ltorg
.global breadcrumb_systemoff_write
.thumb_func
breadcrumb_systemoff_write:
crumb 41
str r2,[r3,#0x500]
ldr.w pc,.Lsystemoff_after
.balign 4
.Lsystemoff_after: .word 0x80ab
.ltorg
.global breadcrumb_battery
.thumb_func
breadcrumb_battery:
crumb 50,0,1
strb r3,[r6,#0x2e]
ldr.w pc,.Lbattery_stock
.balign 4
.Lbattery_stock: .word 0x83ef
.ltorg
.global breadcrumb_button
.thumb_func
breadcrumb_button:
crumb 60,0,2
strb r3,[r4,#0x2e]
ldr.w pc,.Lbutton_stock
.balign 4
.Lbutton_stock: .word 0xc37b
.ltorg
.global breadcrumb_battery_exit
.thumb_func
breadcrumb_battery_exit:
crumb 51
cbz r0,.Lbattery_ok
movs r3,#0
ldr.w pc,.Lbattery_fail_return
.Lbattery_ok:
ldr.w pc,.Lbattery_ok_return
.balign 4
.Lbattery_fail_return: .word 0x8401
.Lbattery_ok_return: .word 0x840b
.ltorg
.global breadcrumb_button_exit
.thumb_func
breadcrumb_button_exit:
crumb 61
cbz r0,.Lbutton_ok
movs r3,#0
ldr.w pc,.Lbutton_fail_return
.Lbutton_ok:
ldr.w pc,.Lbutton_ok_return
.balign 4
.Lbutton_fail_return: .word 0xc38d
.Lbutton_ok_return: .word 0xc3c5
.ltorg
.global breadcrumb_command
.thumb_func
breadcrumb_command:
// Displaced load obtains the actual dispatcher opcode, without reading payload.
ldrb r0,[r10,#1]
push {r0,r1,r2,r3,r12,lr}
mrs r12,APSR
push {r12,lr}
mov r3,r0
movs r0,#80
movs r1,#0
movs r2,#0
bl breadcrumb_write
pop {r12,lr}
msr APSR_nzcvq,r12
pop {r0,r1,r2,r3,r12,lr}
ldr.w pc,.Lcommand_dispatch_return
.balign 4
.Lcommand_dispatch_return: .word 0x9523
.ltorg

.global breadcrumb_event_post
.thumb_func
breadcrumb_event_post:
push {r0,r1,r2,r3,r12,lr}
mrs r12,APSR
push {r12,lr}
cmp r0,#2
bne .Levent_post_resume
movs r0,#70
movs r1,#0
movs r2,#3
movs r3,#0
bl breadcrumb_write
.Levent_post_resume:
pop {r12,lr}
msr APSR_nzcvq,r12
pop {r0,r1,r2,r3,r12,lr}
push {r4}
ldr r2,=0x20001ba9
ldr.w pc,.Levent_post_stock
.balign 4
.Levent_post_stock: .word 0x9a99
.ltorg
.global breadcrumb_battery_event2
.thumb_func
breadcrumb_battery_event2:
crumb 52,0,1
push {r4}
ldr r2,=0x20001ba9
ldr.w pc,.Lbattery_event_post
.balign 4
.Lbattery_event_post: .word 0x9a99
.ltorg
.global breadcrumb_button_event2
.thumb_func
breadcrumb_button_event2:
crumb 62,0,2
push {r4}
ldr r2,=0x20001ba9
ldr.w pc,.Lbutton_event_post
.balign 4
.Lbutton_event_post: .word 0x9a99
.ltorg