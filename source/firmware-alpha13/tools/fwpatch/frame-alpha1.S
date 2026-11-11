.syntax unified
.cpu cortex-m4
.thumb
.section .text
.equ mode_flag, 536891728
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
ldr r1,=0x20005158
movs r2,#0
str r2,[r1]
str r2,[r1,#4]
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
cmp r1,#19
beq .Lsensor_check
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
.Lsensor_check:
cmp r1,#19
bne .Lsetter_check
ldrb r2,[r0]
cmp r2,#0x7e
bne .Lfallback
ldrb r2,[r0,#1]
cmp r2,#3
bne .Lfallback
ldr r3,=mode_flag
ldrb r3,[r3]
cmp r3,#1
bne .Lfallback
ldrb r2,[r0,#18]
#ifdef ALPHA13
cmp r2,#9
#else
cmp r2,#7
#endif
bhi .Lfallback
b .Lquery
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
movs r2,#12
strb r2,[r5,#25]
movs r1,#26
b .Lreply
.Lquery_reply:
ldrb r2,[r4,#1]
cmp r2,#3
bne .Lstandard_query
mrs r8,PRIMASK
cpsid i
movs r2,#1
strb r2,[r5,#22]
movs r2,#12
strb r2,[r5,#23]
ldrb r1,[r4,#18]
strb r1,[r5,#24]
movs r2,#6
strb r2,[r5,#25]
mov r0,r5
adds r0,#26
bl alpha_snapshot
dmb sy
msr PRIMASK,r8
movs r1,#32
b .Lreply
.Lstandard_query:
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
moveq r2,#12
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
 bne .Lmask_halt
b .Lhalt
.Lmask_halt:
// Pending transition: prevent a TIMER0 IRQ from starting TX during halt.
cpsid i
.Lhalt:
pop {r0,r1,r2,r3,r12,lr}
cmp r6,#0
beq .Lstock_halt
bl alpha_halt
b .Lhalt_return
.Lstock_halt:
ldr r3,=0x57ed
blx r3
.Lhalt_return:
push {r0,r1,r2,r3,r12,lr}
mrs r4,APSR
cmp r6,#0
beq .Lboundary_done
cmp r0,#0
bne .Lboundary_done
ldr r6,=mode_flag
ldrb r2,[r6,#1]
// Foreground MAG open/close may block on I2C with IRQs enabled. Defer this
// staged mode commit until its stock lifecycle has returned; never call MAG here.
#ifdef ALPHA12
ldrb r3,[r6]
cmp r3,#1
bne .Lmag_commit_ready
ldr r3,=0x20005170
ldr r3,[r3]
ldr r1,=0x4d414731
cmp r3,r1
bne .Lmag_commit_ready
ldr r3,=0x2000516d
ldrb r3,[r3]
cmp r3,#1
beq .Lboundary_done
.Lmag_commit_ready:
#endif
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
// Preserve the acknowledged history when only Frame N changes.
cmp r2,#2
bne .Lclear_ack
ldrb r1,[r6]
cmp r1,#1
beq .Lack_ready
.Lclear_ack:
ldr r3,=0x20005158
movs r1,#0
str r1,[r3]
.Lack_ready:
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
.balign 8
frame_period:
.double 3000.0
.balign 8
frame_normal:
.double 1810.5
.balign 8
frame_alternate:
.double 2207.0


// These gates restore mode-0 registers/APSR and execute the displaced stock
// instruction. There is no new startup/reset hook.
.global alpha_imu_gate
.type alpha_imu_gate,%function
.thumb_func
alpha_imu_gate:
push {r0,r1}
mrs r0,APSR
ldr r1,=mode_flag
ldrb r1,[r1]
cmp r1,#1
beq 1f
msr APSR_nzcvq,r0
pop {r0,r1}
push.w {r3,r4,r5,r6,r7,r8,r9,r10,r11,lr}
ldr.w pc,2f
1:
msr APSR_nzcvq,r0
pop {r0,r1}
b alpha_imu
.balign 4
2: .word 0x945d
.ltorg

// Worker queue receive succeeded; preserve all live registers and flags.
#ifdef ALPHA12
.global alpha_mag_worker_gate
.type alpha_mag_worker_gate,%function
.thumb_func
alpha_mag_worker_gate:
push {r0,r1,r2,r3,r12,lr}
mrs r0,APSR
push {r0,r1}
bl alpha_mag_reconcile
pop {r0,r1}
msr APSR_nzcvq,r0
pop {r0,r1,r2,r3,r12,lr}
ldm.w r4,{r0,r1,r2,r3}
ldr.w pc,1f
.balign 4
1: .word 0xda07
.ltorg

.global alpha_mag_product_gate
.type alpha_mag_product_gate,%function
.thumb_func
alpha_mag_product_gate:
push {r0,r1,r2}
mrs r2,APSR
ldr r1,=mode_flag
ldrb r1,[r1]
cmp r1,#1
beq 1f
msr APSR_nzcvq,r2
pop {r0,r1,r2}
ldr.w pc,2f
1:
msr APSR_nzcvq,r2
pop {r0,r1,r2}
push {r4,lr}
ldr r4,=0xb307
blx r4
push {r0,r1,r2,r3,r12,lr}
mrs r4,APSR
bl alpha_mag_product
msr APSR_nzcvq,r4
pop {r0,r1,r2,r3,r12,lr}
pop {r4,lr}
bx lr
.balign 4
2: .word 0xb307
.ltorg
#endif
.global alpha_ack_gate
.type alpha_ack_gate,%function
.thumb_func
alpha_ack_gate:
push {r0,r1,r2}
mrs r2,APSR
ldr r1,=mode_flag
ldrb r1,[r1]
cmp r1,#1
beq 1f
msr APSR_nzcvq,r2
pop {r0,r1,r2}
push {r4,lr}
ldr r1,=0x20001afc
ldr.w pc,2f
1:
msr APSR_nzcvq,r2
pop {r0,r1,r2}
b alpha_ack
.balign 4
2: .word 0x97f9
.ltorg

#ifdef ALPHA13
// Normal event ID 2, stock queue priority; private payload distinguishes a
// Frame-produced event surviving a mode commit from a stock-produced event.
.global alpha13_timer_gate
.type alpha13_timer_gate,%function
.thumb_func
alpha13_timer_gate:
push {r0,r2}
mrs r0,APSR
ldr r2,=mode_flag
ldrb r2,[r2]
cmp r2,#1
beq 1f
msr APSR_nzcvq,r0
pop {r0,r2}
ldr r3,=0x20001ff6
ldrb r1,[r3]
ldr.w pc,2f
1:
msr APSR_nzcvq,r0
pop {r0,r2}
movs r0,#2
ldr r1,=0x4131334d
ldr r2,=0xbececcb2
ldr.w pc,3f
.balign 4
2: .word 0xb2b5
3: .word 0xd891
.ltorg

.global alpha13_event_gate
.type alpha13_event_gate,%function
.thumb_func
alpha13_event_gate:
// Entered by BL at DA18: worker event copy starts at SP+4.
push {r0,r1,r2,r3,r12,lr}
mrs r12,APSR
ldr r0,=mode_flag
ldrb r0,[r0]
cmp r0,#1
beq 1f
ldr r0,[sp,#32]
ldr r1,=0x4131334d
cmp r0,r1
bne 2f
ldr r0,[sp,#36]
ldr r1,=0xbececcb2
cmp r0,r1
bne 2f
// Frame work queued before leaving mode 1 is discarded, no I2C/publication.
msr APSR_nzcvq,r12
pop {r0,r1,r2,r3,r12,pc}
1:
msr APSR_nzcvq,r12
pop {r0,r1,r2,r3,r12,lr}
b alpha13_measure
2:
msr APSR_nzcvq,r12
pop {r0,r1,r2,r3,r12,lr}
ldr.w pc,3f
.balign 4
3: .word 0xb421
.ltorg

.global alpha13_read_gate
.type alpha13_read_gate,%function
.thumb_func
alpha13_read_gate:
push {r4,r5,r6,lr}
mrs r5,APSR
ldr r4,=mode_flag
ldrb r4,[r4]
cmp r4,#1
beq 1f
msr APSR_nzcvq,r5
pop {r4,r5,r6,lr}
ldr.w pc,3f
1:
msr APSR_nzcvq,r5
ldr r4,=0xd769
blx r4
ldr r4,=0x2000516e
cmp r0,#0
ite eq
moveq r1,#1
movne r1,#0
strb r1,[r4]
pop {r4,r5,r6,pc}
.balign 4
3: .word 0xd769
.ltorg
#endif
.global alpha_dispatch_guard
.type alpha_dispatch_guard,%function
.thumb_func
alpha_dispatch_guard:
push {r0,r1,r2,r3,r12}
mrs r12,APSR
ldr r0,=mode_flag
ldrb r0,[r0]
cmp r0,#1
bne 2f
mov r0,r9
mov r1,r6
subs r1,r1,r0
cmp r1,#3
blo 3f
mov r3,r7
add r3,r0
ldrb r2,[r3,#2]
cmp r2,#253
bhs 3f
adds r2,#3
cmp r2,r1
bhi 3f
add r0,r2
cmp r0,#255
bhi 3f
2:
msr APSR_nzcvq,r12
pop {r0,r1,r2,r3,r12}
add.w r10,r7,r9
ldr.w pc,4f
3:
msr APSR_nzcvq,r12
pop {r0,r1,r2,r3,r12}
ldr.w pc,5f
.balign 4
4: .word 0x951b
5: .word 0x9513
.ltorg
