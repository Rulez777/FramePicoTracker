/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
#error "Historical identity-bound candidate: configure identities and re-review before building"
/* Temporary RAM-only MPSL receiver for stock Roy build 1788551713.
 * No flash/NVMC/reset operations. Loaded by roy_ram_loader.py.
 * Experimental station sequence for up to three privately configured trackers.
 * Bounded IMU reception verified; persistent reconnect and 6DoF unverified.
 * Build with the same native/MPU/poll/offer/handover flags as roy_ram_rx.c;
 * loader additionally requires --station-sequence. Camera phase remains zero.
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int i32;
#define R(base, offset) (*(volatile u32 *)((base) + (offset)))
#define RADIO 0x40001000u
#define TIMER 0x40008000u
#define STATE_ADDR 0x20010c18u
#define DIAG(offset) (*(volatile u32 *)(STATE_ADDR+(offset)))
#define OPEN_FN 0x00010581u
#define REQUEST_FN 0x00018949u
#define CLOSE_FN 0x000189bfu
#define CONTEXT_ADDR (STATE_ADDR + 1504u)
typedef struct { u8 type; u8 pad[3]; u8 clock, priority; u16 pad2; u32 a, b; } Request;
typedef struct { u8 action; u8 pad[3]; Request *next; } Result;
typedef struct { u32 sequence, crc, rssi, channel; u16 length; u16 reserved; u8 bytes[258]; u16 capture_us; } Record;
typedef struct {
    u32 magic, stop, radio_enabled, slots, packets, dropped, blocked, phase;
    i32 result;
    u32 slot_us, interval_us;
    u32 frequency, mode, pcnf0, pcnf1, base0, prefix0, crccnf, crcpoly, crcinit, whiteiv;
    u8 session; u8 pad[3];
    Request request;
    Result reply;
    u32 write_count, read_count;
    u8 dma[258]; u8 align[2];
    Record records[4];
} State;
_Static_assert(sizeof(Request) == 16, "MPSL request ABI");
_Static_assert(sizeof(Result) == 8, "MPSL callback ABI");
_Static_assert(sizeof(State) <= 1664, "State leaves a private call stack");
_Static_assert(sizeof(Record) == 280, "Record wire ABI");
#ifdef ROY_RX_POLL_TICK
__attribute__((used,section(".rodata.tag"))) static const char poll_tick_tag[]="ROYRXPOLLTICK1";
#ifndef ROY_STATION_SERVICE
__attribute__((used,section(".rodata.tag"))) static const char slot_tag[]="ROYRXSLOT1";
#endif
#endif
static volatile State *const state = (volatile State *) STATE_ADDR;
typedef struct { u32 id, base; u8 prefix, checksum; u16 last_slot; } Peer;
typedef struct { Peer peers[3]; u8 offer[21], configure[33]; } Peers;
static volatile Peers *const peers=(volatile Peers *)CONTEXT_ADDR;
_Static_assert(sizeof(Peers)<=108,"Private table ends before diagnostics");
#ifdef ROY_SWIFT_MULTI
__attribute__((used,section(".rodata.tag"))) static const char build_tag[]="ROYSWIFTMULTI1";
#endif
#ifdef ROY_IR_TX_PROBE
__attribute__((used,section(".rodata.tag"))) static const char tx_tag[]="ROYIRTXPROBE1";
#endif
#ifdef ROY_SWIFT_SWEEP
__attribute__((used,section(".rodata.tag"))) static const char sweep_tag[]="ROYSWIFTSWEEP1";
#endif
#ifdef ROY_SWIFT_DUAL
__attribute__((used,section(".rodata.tag"))) static const char dual_tag[]="ROYSWIFTDUAL1";
#endif
#ifdef ROY_SWIFT_POLL
#ifdef ROY_SWIFT_STATION_OFFER
#ifdef ROY_SWIFT_STATION_HANDOVER
#ifdef ROY_STATION_SERVICE
__attribute__((used,section(".rodata.tag"))) static const char station_offer_tag[]="ROYSTATIONTHREE3";
#else
__attribute__((used,section(".rodata.tag"))) static const char station_offer_tag[]="ROYSTATIONTHREE2";
#endif
#else
__attribute__((used,section(".rodata.tag"))) static const char station_offer_tag[]="ROYSTATIONOFFER1";
#endif
#endif
#ifdef ROY_SWIFT_PAIRED_GATE_DRAFT
/* Compilation experiment only. Loader rejects this tag pending reachability
 * audit; not a reviewed hardware probe. */
__attribute__((used,section(".rodata.tag"))) static const char gate_draft_tag[]="ROYPAIREDGATEDRAFT1";
#endif
#ifdef ROY_SWIFT_POLL_FAST_RX
#endif
#if defined(ROY_SWIFT_POLL_IDENTITY) && !defined(ROY_SWIFT_STATION_HANDOVER)
__attribute__((used,section(".rodata.tag"))) static const char identity_poll_tag[]="ROYSWIFTPOLLID1";
#endif
#ifdef ROY_SWIFT_POLL_BOOTSTRAP
#endif
#ifdef ROY_SWIFT_POLL_MASK
__attribute__((used,section(".rodata.tag"))) static const char mask_poll_tag[]="ROYSWIFTPOLLMASK1";
#endif
#if defined(ROY_SWIFT_POLL_NOOP) && !defined(ROY_SWIFT_STATION_HANDOVER)
__attribute__((used,section(".rodata.tag"))) static const char noop_tag[]="ROYSWIFTPOLLNOOP1";
#endif
#endif
typedef Result *(*Callback)(u8, u32);
static i32 open_session(Callback cb, u8 *sid) { return ((i32(*)(Callback,u8*))OPEN_FN)(cb,sid); }
static i32 request_slot(u8 sid, const Request *r) { return ((i32(*)(u8,const Request*))REQUEST_FN)(sid,r); }
static i32 close_session(u8 sid) { return ((i32(*)(u8))CLOSE_FN)(sid); }

/* Debugger places an interrupted Thread context here before invoking entry.
 * Restore without BKPT: stopping after request can miss MPSL deadlines.
 * Layout: r0-r12, lr, pc, apsr, msp, psp, primask, basepri, faultmask, control.
 * Two scratch words below the original SP hold original r12 and return PC.
 */
__attribute__((naked,noreturn,unused)) static void return_to_thread(void) {
    __asm__ volatile(
        "ldr r12, =0x200111f8\n" /* DATA+1504 */
        "ldr r0, [r12, #84]\n"
        "tst r0, #2\n"
        "ite ne\n"
        "ldrne r1, [r12, #68]\n"
        "ldreq r1, [r12, #64]\n"
        "sub r1, r1, #8\n"
        "ldr r2, [r12, #48]\n"
        "str r2, [r1]\n"
        "ldr r2, [r12, #56]\n"
        "orr r2, r2, #1\n"
        "str r2, [r1, #4]\n"
        "ldr r2, [r12, #64]\n"
        "tst r0, #2\n"
        "it eq\n"
        "moveq r2, r1\n"
        "msr msp, r2\n"
        "ldr r2, [r12, #68]\n"
        "tst r0, #2\n"
        "it ne\n"
        "movne r2, r1\n"
        "msr psp, r2\n"
        "msr control, r0\n"
        "isb\n"
        "ldr r0, [r12, #76]\n"
        "msr basepri, r0\n"
        "ldr r0, [r12, #80]\n"
        "msr faultmask, r0\n"
        "ldr r0, [r12, #60]\n"
        "msr APSR_nzcvqg, r0\n"
        "ldmia r12, {r0-r11, lr}\n" /* lr requires separate offset */
        "ldr lr, [r12, #52]\n"
        "ldr r12, [r12, #72]\n"
        "msr primask, r12\n"
        "pop.w {r12, pc}\n"
    );
}

static void stop_radio(void) {
    R(TIMER,0x308) = 1u << 16;
    R(RADIO,0x308) = 0xffffffffu;
    R(RADIO,0x200) = 0;
    R(RADIO,0x10) = 1;
    for (u32 n=0; n<1024 && R(RADIO,0x550)!=0; n++) { __asm__ volatile("nop"); }
}

static void capture_record(u16 partial) {
    if (DIAG(1624)==2) peers->peers[DIAG(1644)].last_slot=(u16)state->slots;
    if (DIAG(1624)==2 && state->dma[0]>=22 && state->dma[3]==2 &&
        state->dma[4]>=18 && state->dma[4]<=42)
        DIAG(1660)|=1u<<DIAG(1644);
    u32 n=state->write_count;
    if (n-state->read_count<4) {
        volatile Record *rec=&state->records[n&3];
        R(TIMER,0x44)=1; /* Callback observation, not hardware RF edge. */
        rec->capture_us=(u16)R(TIMER,0x544);
        rec->sequence=state->packets;
        rec->crc=state->crccnf ? R(RADIO,0x40c) : 0;
        rec->rssi=R(RADIO,0x548)|(DIAG(1644)<<8);
        rec->channel=DIAG(1624);
        rec->length=state->crccnf ? 258 : (state->pcnf1&255);
        rec->reserved=partial;
#ifdef ROY_RX_POLL_TICK
        rec->reserved|=(u16)((state->slots&0x7fffu)<<1);
#endif
#ifdef ROY_SWIFT_DUAL
        rec->reserved|=(u16)(R(RADIO,0x408)<<8);
#endif
#ifdef ROY_SWIFT_MULTI
        rec->reserved|=(u16)(DIAG(1644)<<8);
#endif
        for (u32 i=0;i<rec->length;i++) rec->bytes[i]=state->dma[i];
        __asm__ volatile("dmb" ::: "memory");
        state->write_count=n+1;
    } else state->dropped++;
    state->packets++;
}

static Result *callback(u8 session, u32 signal) {
    (void)session;
#ifndef ROY_STATION_SERVICE
    DIAG(1632)=signal;
    DIAG(1636)|=1u<<signal;
#endif
    state->reply.action = 0;
#ifdef ROY_COUNTER_ONLY
    /* No peripheral writes, timers, next requests or kernel calls. */
    state->frequency = signal; /* diagnostic last signal, not an RF channel */
    state->mode |= 1u << signal; /* observed signal bitmap */
    if (signal == 0) {
        state->slots++;
        state->phase = 1;
        state->reply.action = 2; /* immediately END this timeslot */
#ifdef ROY_COUNTER_REPEAT
        if (!state->stop) {
            state->request.type=1;
            state->request.a=state->interval_us;
            state->request.b=state->slot_us;
            state->reply.next=(Request *)&state->request;
            state->reply.action=3;
        }
#endif
    } else if (signal == 9) {
        state->phase = 0;
        state->session = 255;
    } else if (signal == 5 || signal == 6 || signal == 7) {
        state->blocked++;
        state->phase = 3;
    } else if (signal == 8 || signal == 10) {
        state->stop = 1;
        state->phase = 4;
    }
    return (Result *)&state->reply;
#endif
    if (signal == 0) {
        state->slots++;
#ifdef ROY_STATION_SERVICE
        if (state->slots>=DIAG(1608))state->stop=1;
#endif
        state->phase = 2;
        R(TIMER,0x140) = 0;
        R(TIMER,0x540) = state->slot_us - 200;
#if defined(ROY_RX_POLL_TICK) && !defined(ROY_SWIFT_POLL)
        R(TIMER,0x540) = 100;
#endif
        R(TIMER,0x304) = 1u << 16;
        if (!state->stop) {
            if (!state->crccnf) for (u32 i=0;i<(state->pcnf1&255);i++) state->dma[i]=0;
            DIAG(1624)=state->frequency; /* Actual channel for this slot. */
#ifdef ROY_SWIFT_DUAL
            const u8 channels[6]={1,37,77,4,78,80};
            DIAG(1624)=channels[(state->slots-1u)%6u];
#endif
#ifdef ROY_SWIFT_SWEEP
            /* Passive diagnostic sweep. This does not assume the live mask or
             * predict hopping: visit each candidate even channel once/40 slots. */
            DIAG(1624)=2u+2u*((state->slots-1u)%40u);
#endif
#ifdef ROY_SWIFT_MULTI
            /* Three previously captured tails; no TX, ACK or pairing changes.
             * Host profile must be the reviewed raw MODE4 configuration. */
            const u32 bases[3]={0x4d060000u,0xec4c0000u,0x84090000u};
            const u32 prefixes[3]={0,0x80,0x40};
            u32 index=(state->slots-1u)%6u;
            DIAG(1644)=index%3u;
            DIAG(1624)=state->frequency==80u ? 80u : (index<3u ? 2u : 80u);
            R(RADIO,0x51c)=bases[DIAG(1644)];
            R(RADIO,0x524)=prefixes[DIAG(1644)];
#endif
            R(RADIO,0x308)=0xffffffffu;
            R(RADIO,0x200)=0;
            R(RADIO,0x508)=DIAG(1624);
            R(RADIO,0x510)=state->mode;
            R(RADIO,0x514)=state->pcnf0;
            R(RADIO,0x518)=state->pcnf1;
#ifndef ROY_SWIFT_MULTI
            R(RADIO,0x51c)=state->base0;
            R(RADIO,0x524)=state->prefix0;
#endif
#ifdef ROY_SWIFT_STATION_HANDOVER
            u32 flags=DIAG(1660);
        u32 index=(state->slots-1u)%3u;
            DIAG(1644)=index;
            if ((flags&(1u<<index)) && (u16)(state->slots-peers->peers[index].last_slot)>90u) {
                flags&=~(1u<<index); DIAG(1660)=flags;
            }
            u32 stage=(flags&(1u<<index)) ? 2u : (state->slots-1u)%5u;
            u32 paired_offer=!(flags&(1u<<index)) && stage==2u && ((state->slots/15u)&1u);
            u32 working=stage>=2u;
            if (working) {
                R(RADIO,0x51c)=peers->peers[index].base;
                R(RADIO,0x524)=peers->peers[index].prefix;
            }
#endif
            R(RADIO,0x530)=1;
#ifdef ROY_SWIFT_DUAL
            R(RADIO,0x520)=0x552c6a1eu;
            R(RADIO,0x524)=0xc000u; /* pipe0 working prefix0; pipe1 bootstrapC0 */
            R(RADIO,0x530)=3;
#endif
            R(RADIO,0x534)=state->crccnf;
            R(RADIO,0x538)=state->crcpoly;
            R(RADIO,0x53c)=state->crcinit;
            R(RADIO,0x554)=state->whiteiv;
            R(RADIO,0x504)=(u32)&state->dma[0];
            R(RADIO,0x10c)=0;
            R(RADIO,0x104)=0;
            R(RADIO,0x110)=0;
            R(RADIO,0x650)=0; /* Legacy ramp-up, no stale BLE MODECNF0. */
            R(RADIO,0x200)=19; /* READY_START | END_DISABLE | ADDRESS_RSSISTART */
            R(RADIO,0x304)=8; /* END interrupt */
#if defined(ROY_IR_TX_PROBE) || defined(ROY_SWIFT_POLL)
            /* Bounded laboratory probe: one fixed command per even channel,
             * at most320 packets total in this experimental build.
             * Standard tracker pairing writes are authorized; Frame flash untouched.
             * Identity is a conditional candidate; delivery is NOT assumed. */
#ifdef ROY_STATION_SERVICE
            if (1) {
#else
            if (state->slots<=
#ifdef ROY_SWIFT_STATION_HANDOVER
                1000u
#else
                40u
#endif
                ) {
#endif
                DIAG(1624)=2u*state->slots;
#ifdef ROY_SWIFT_POLL_MASK
                const u8 active_mask[5]={2,6,8,10,12};
                DIAG(1624)=active_mask[(state->slots-1u)%5u];
#elif defined(ROY_SWIFT_POLL_BOOTSTRAP)
                const u8 bootstrap_channels[3]={1,37,77};
                DIAG(1624)=bootstrap_channels[(state->slots-1u)%3u];
#endif
                R(RADIO,0x508)=DIAG(1624);
#ifndef ROY_SWIFT_STATION_OFFER
#ifdef ROY_SWIFT_POLL
                /* Zero application payload: no IR/pairing/configuration opcode.
                 * Probe response timing only; rotate ESB PID, NO_ACK clear. */
                state->dma[0]=0;
                state->dma[1]=(u8)(((state->slots-1u)&3u)<<1);
#ifdef ROY_SWIFT_POLL_NOOP
                /* Complete application header; parser opcode0 exits at9512. */
                state->dma[0]=3;
                state->dma[2]=0;
                state->dma[3]=0;
                state->dma[4]=0;
#ifdef ROY_SWIFT_POLL_IDENTITY
                state->dma[0]=9; /* Swift68FA: length9 identity request. */
                for (u32 i=2;i<11;i++) state->dma[i]=0;
#endif
#endif
#else
                const u8 command[11]={9,0,1,9,6,1,0,0,0,0,0};
                for (u32 i=0;i<11u;i++) state->dma[i]=command[i];
#endif
#endif
#ifdef ROY_SWIFT_PAIRED_GATE_DRAFT
                /* Fixed first-tracker working address is enforced at host.
                 * S1 bit3 MUST stay clear. Last ID byte deliberately differs
                 * from observed FICROWNER_PRIVATE_ID_REQUIRED; never use shared bootstrap. */
                const u8 gate[21]={4,0,0,0,0,0,0x55,0x3d,0,0,0,0,
                                  0x74,0x73,0x54,0x15,0,0,0,0,0};
                state->dma[0]=21;
                state->dma[1]=((state->slots-1u)&3u)<<1;
                for (u32 i=0;i<21;i++) state->dma[i+2]=gate[i];
#endif
#ifdef ROY_SWIFT_STATION_OFFER
                /* Observed FICR OWNER_PRIVATE_ID_REQUIRED; ordinary pairing storage authorized.
                 * Discovery/offer/three working polls repeat on bootstrap1/37/77.
                 * Duplicate seq0 settings are a retry hypothesis, not a complete
                 * per-command ACK/state engine. Fixed phase0 is not camera sync.
                 * Payload offsets reviewed against PhoenixBD20/Swift68F0. */
                if(paired_offer) working=0;
                u32 length=(flags&(1u<<index))?1:working?33:stage?21:9;
                state->dma[0]=length;
                state->dma[1]=working?3:2;
                for(u32 i=0;i<length;i++)
                    state->dma[i+2]=working?peers->configure[i]:stage?peers->offer[i]:i?0:0xc1;
                if (stage) {
                    u32 offset=working?7:19;
                    volatile u8 *id=(volatile u8 *)&peers->peers[index].id;
                    for(u32 i=0;i<4;i++)state->dma[offset+i]=id[i];
                    if (working) {
                        state->dma[34]=peers->peers[index].checksum;
                    } else state->dma[18]=index;
                }
                if (!working) {
                    const u8 channels[3]={1,37,77};
                    DIAG(1624)=paired_offer?((state->slots/30u)&1u?4u:78u):channels[((state->slots-1u)/15u)%3u];
                } else DIAG(1624)=2;
                R(RADIO,0x508)=DIAG(1624);

#endif
                R(RADIO,0x52c)=0; /* TXADDRESS pipe0. */
                R(RADIO,0x50c)=0; /* 0dBm, explicit bounded probe power. */
                R(RADIO,0x304)=0; /* Poll within our own slot. */
                R(RADIO,0x200)=3; /* READY_START | END_DISABLE. */
#ifdef ROY_SWIFT_POLL_FAST_RX
                R(RADIO,0x650)=1; /* Fast ramp-up; software owns timing. */
                R(RADIO,0x200)=11; /* READY_START|END_DISABLE|DISABLED_RXEN. */
#endif
                DIAG(1648)++;
                R(RADIO,0)=1; /* TXEN. */
                for (u32 n=0;n<20000u && !R(RADIO,0x10c);n++) __asm__ volatile("nop");
                if (R(RADIO,0x10c)) DIAG(1652)++;
#ifdef ROY_SWIFT_POLL_FAST_RX
                /* Wait until the shortcut has actually left TX state; removing
                 * DISABLED_RXEN at END before DISABLED would cancel handover. */
                for (u32 n=0;n<1024u && R(RADIO,0x550)>=9;n++) __asm__ volatile("nop");
                u32 rx_state=R(RADIO,0x550);
                if (rx_state>=1 && rx_state<=3) DIAG(1656)++;
                else {
                    stop_radio(); state->stop=1; state->phase=1;
                    state->reply.action=2;
                    return (Result *)&state->reply;
                }
#else
                R(RADIO,0x10)=1;
                for (u32 n=0;n<1024u && R(RADIO,0x550)!=0;n++) __asm__ volatile("nop");
#endif
                R(RADIO,0x10c)=0;
                R(RADIO,0x104)=0;
                R(RADIO,0x200)=19;
                R(RADIO,0x304)=8;
            }
#endif
#if defined(ROY_RX_POLL_TICK) && defined(ROY_SWIFT_POLL)
            /* TX setup can outlast100us. Arm the first polling compare only
             * after TX, never in the past. Keep the original cleanup deadline. */
            R(TIMER,0x44)=1;
            u32 now=R(TIMER,0x544);
            if (now>=state->slot_us-300) {
                stop_radio();
                state->phase=1;
                state->stop=1;
                state->reply.action=2;
                return (Result *)&state->reply;
            }
            R(TIMER,0x540)=now+100;
#endif
#ifdef ROY_SWIFT_POLL_FAST_RX
#ifndef ROY_STATION_SERVICE
            if (state->slots>
#ifdef ROY_SWIFT_STATION_HANDOVER
                1000u
#else
                40u
#endif
                ) R(RADIO,0x4)=1;
#endif
#else
            R(RADIO,0x4)=1; /* RXEN */
#endif
        }
    } else if (signal == 1 && R(TIMER,0x140)) {
        R(TIMER,0x140)=0;
#ifdef ROY_RX_POLL_TICK
        /* Bounded TIMER0 observations within our granted slot. No PPI, no
         * busy wait for packets and no claim of an exact RF-edge timestamp. */
        u32 tick=R(TIMER,0x540);
        if (tick < state->slot_us-200) {
            if (R(RADIO,0x10c)) {
                DIAG(1612)++;
                DIAG(1616)++;
                if (!state->crccnf || R(RADIO,0x400)) capture_record(0);
                else DIAG(1620)++;
                R(RADIO,0x10c)=0;
                R(RADIO,0x104)=0;
                if (R(RADIO,0x550)==0 && !state->stop) {
                    R(RADIO,0x110)=0;
                    R(RADIO,0x4)=1;
                }
            }
            u32 next=tick+100;
            R(TIMER,0x540)=next<state->slot_us-200 ? next : state->slot_us-200;
            return (Result *)&state->reply;
        }
#endif
        {
            DIAG(1628)=R(RADIO,0x550); /* State before DISABLE, not after. */
            if (R(RADIO,0x10c)) DIAG(1640)++;
        }
        if (R(RADIO,0x104)) DIAG(1612)++;
        stop_radio();
        if (R(RADIO,0x104)) {
            /* Stock IRQ path did not forward RADIO to this MPSL callback.
             * END is still independently observable before slot teardown. */
            if (R(RADIO,0x10c)) {
                DIAG(1616)++;
                if (!state->crccnf || R(RADIO,0x400)) capture_record(0);
                else DIAG(1620)++;
            } else if (!state->crccnf) capture_record(1);
        }
        state->phase=1;
        if (state->stop) {
            state->reply.action=2; /* END */
        } else {
            state->request.type=1; /* NORMAL */
            state->request.a=state->interval_us;
            state->request.b=state->slot_us;
            state->reply.next=(Request *)&state->request;
            state->reply.action=3; /* REQUEST closes this slot */
        }
    } else if (signal == 2 && state->phase==2 && R(RADIO,0x10c)) {
        R(RADIO,0x10c)=0;
        DIAG(1616)++;
        if (state->crccnf && !R(RADIO,0x400)) DIAG(1620)++;
        if (!state->crccnf || R(RADIO,0x400)) {
            capture_record(0);
        }
        /* END_DISABLE can still be finishing when the IRQ fires. */
        for (u32 n=0; n<1024 && R(RADIO,0x550)!=0; n++) { __asm__ volatile("nop"); }
        R(RADIO,0x110)=0;
        R(RADIO,0x104)=0;
        if (!state->crccnf) for (u32 i=0;i<(state->pcnf1&255);i++) state->dma[i]=0;
        if (!state->stop && state->phase==2 && R(RADIO,0x550)==0) R(RADIO,0x4)=1;
    } else if (signal==5 || signal==6 || signal==7) {
        state->blocked++;
        state->phase=3; /* Host may retry EARLIEST from thread context */
    } else if (signal==9) {
        state->phase=0;
        state->session=255;
    } else if (signal==8 || signal==10) {
        state->stop=1;
        state->phase=4;
    }
    return (Result *)&state->reply;
}

/* Called once by the debugger in privileged thread mode with IRQs masked.
 * All interrupted CPU registers and the stack are restored by the loader.
 * operation: 0=open, 1=retry request, 2=close (only after slot ended).
 */
#ifdef ROY_NATIVE_CALL
__attribute__((section(".entry"),used)) void entry(const void *shell, const u8 *bytes, u32 count) {
#ifdef ROY_MPU_RESERVE
    /* Reserve region 2 so stock PendSV places its stack guard in region 3.
     * Initial host entry briefly has MPU disabled; all subsequent execution
     * uses the enabled MPU with only this 4 KiB code reservation executable.
     */
    u32 irq;
    __asm__ volatile("mrs %0, primask\ncpsid i" : "=r"(irq) :: "memory");
    /* Restore the stock RAM policy atomically on CPU; old SWD selector writes
     * can race with PendSV and must never be used for retries or closing.
     */
    R(0xe000ed00u,0x98)=1;
    R(0xe000ed00u,0x9c)=0x20000011u;
    R(0xe000ed00u,0xa0)=0x110b0021u;
    volatile u8 *static_count=(volatile u8 *)0x20010a26u;
    if (*static_count==2) {
        R(0xe000ed00u,0x98)=2;
        R(0xe000ed00u,0x9c)=0x20011012u;
        R(0xe000ed00u,0xa0)=0x010b0017u;
        *static_count=3;
    }
    R(0xe000ed00u,0x94)=5;
    __asm__ volatile("dsb\nisb\nmsr primask, %0" :: "r"(irq) : "memory");
#endif
    u32 operation=count ? bytes[0]-'0' : 3;
    /* Host can distinguish a normal entry/return from cleared RAM after reset. */
    *(volatile u32 *)0x2001125cu += 1;
    /* Unhook immediately, returning ownership of the console to stock shell. */
    ((void(*)(const void*,void*))0x0002ee2fu)(shell, (void*)0);
#else
__attribute__((section(".entry"),used)) void entry(u32 operation) {
#endif
    i32 err=0;
#ifdef ROY_NATIVE_CALL
    /* Stock radio-test path serializes open/request/close through this semaphore.
     * Use K_NO_WAIT: never block the console while the stock MPSL worker owns it.
     */
    i32 locked=0;
    if (operation<3) {
        err=((i32(*)(u32,u32))0x00028e2du)(0,0);
        locked=!err;
        if (err) operation=3;
    }
#endif
    if (operation==0) {
        err=open_session(callback,(u8 *)&state->session);
        if (!err) state->phase=1;
    }
    if ((operation==0 && !err) || operation==1) {
        state->request.type=0;
        state->request.clock=0;
#ifdef ROY_COUNTER_RC
        state->request.clock=1; /* Timing-only diagnosis: never use this build for RF. */
#endif
        state->request.priority=1;
#ifdef ROY_COUNTER_HIGH
        state->request.priority=0; /* One-slot comparison with stock timesync priority. */
#endif
        state->request.a=state->slot_us;
        state->request.b=100000;
        err=request_slot(state->session,(const Request *)&state->request);
    }
    if (operation==2) err=close_session(state->session);
#ifdef ROY_NATIVE_CALL
    if (locked) ((void(*)(void))0x00028e3du)();
#endif
    state->result=err;
    *(volatile u32 *)0x20011258u = 0x52455431u;
#ifdef ROY_NATIVE_CALL
    return;
#elif defined(ROY_AUTO_RETURN)
    return_to_thread();
#else
    __asm__ volatile("bkpt #0");
    for (;;) {}
#endif
}
