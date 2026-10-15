/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
/* Roy build1788551713 only. Temporary SRAM, no NVMC/flash operations.
 * ABI4: MCU owns discovery/offer/early confirm; host supplies working packets
 * through three seqlocked mailboxes. Source is experimental until end-to-end
 * controller coexistence, reconnect and optical synchronization are measured.
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int i32;
#define R(b,o) (*(volatile u32 *)((b)+(o)))
#define DATA 0x20010c18u
#define RADIO 0x40001000u
#define TIMER 0x40008000u
#define D(o) R(DATA,o)
typedef struct {u8 type,pad[3],clock,priority;u16 pad2;u32 a,b;} Request;
typedef struct {u8 action,pad[3];Request *next;} Result;
typedef struct {u32 number,crc,rssi,channel;u16 length,reserved;u8 bytes[80];u32 chip,cycles;u16 us,pad;} Record;
typedef struct {
    u32 magic,stop,enabled,slots,packets,dropped,blocked,phase;
    i32 result;
    u32 slot_us,interval_us;
    u32 radio[10];
    u8 session,pad[3];Request request;Result reply;
    u32 write_count,read_count;u8 dma[258],align[2];Record records[10];
} State;
typedef struct {u32 id,base,prefix,stage,last_rx,search,reply_s1,reserved;} Peer;
typedef struct {u32 generation,meta;u8 payload[56];} Mailbox;
static volatile State *const s=(volatile State *)DATA;
static volatile Peer *const peers=(volatile Peer *)(DATA+1504);
static volatile Mailbox *const boxes=(volatile Mailbox *)(DATA+1664);
_Static_assert(sizeof(State)==1500,"ABI4 record and DMA reservation");
_Static_assert(sizeof(Record)==112,"ABI4 ten-record ring");
_Static_assert(sizeof(Peer)==32 && sizeof(Mailbox)==64,"ABI4 peer/mailbox");
__attribute__((used,section(".rodata.tag"))) static const char tag[]="ROYSTATIONTHREE4";

static void stop_radio(void) {
    R(TIMER,0x308)=1u<<16;R(RADIO,0x308)=~0u;R(RADIO,0x200)=0;R(RADIO,0x10)=1;
    for(u32 n=0;n<1024 && R(RADIO,0x550);++n)__asm__ volatile("nop");
}

/* DATA+1880 MCU phase correction for the next distance (us, signed); +1884 bit31|RTC1 target tick; +1888 slot cycle length.
 * +1868/+1872 trace saves; +1876 handshake slot length. */
// D(1892)=slot RX end in us (host: slot length - margin, default margin 200us): 80us froze a controller within
// 0.25s on 2026-10-06; the radio must be idle well before the next BLE event.
static void capture(void) {
    u32 i=D(1644);volatile Peer *p=&peers[i];
    u32 len=s->dma[0];
    if(len>78){s->dropped++;return;}
    u32 sum=0;
    for(u32 n=2;n<len+1;++n)sum+=s->dma[n];
    // capture() is called only for CRCSTATUS=1 (RADIO+0x400).
    // RADIO+0x40c is the received CRC VALUE, not its validity status.
    u32 valid=p->stage>=2 && len && (u8)sum==s->dma[len+1];
    if(valid) {
        p->last_rx=R(0xe0001000u,4);p->reply_s1=s->dma[1]|(D(1648)<<16);
        // Promote only on the working channel, i.e. after the confirm slot,
        // never on a reply to the startup-channel offer itself.
        if(p->stage==2 && R(RADIO,0x508)==2)p->stage=3;
    }
    s->packets++;
    u32 number=s->write_count;
    if(number-s->read_count<10) {
        volatile Record *r=&s->records[number%10];
        r->number=D(1904);r->crc=R(RADIO,0x40c)|(D(1648)<<16);
        r->rssi=R(RADIO,0x548)|(i<<8);r->channel=R(RADIO,0x508);
        r->length=len+2;r->reserved=(u16)((s->slots&0x7fff)<<1);
        R(TIMER,0x44)=1;r->us=(u16)R(TIMER,0x544);r->chip=D(1620);r->cycles=R(0xe0001000u,4);
        for(u32 n=0;n<len+2;++n)r->bytes[n]=s->dma[n];
        __asm__ volatile("dmb" ::: "memory");s->write_count=number+1;
        // Tag0 fallback replies are retained for diagnosis but rejected by OTA.
        // Never retire their reliable queue: a later tagged reply must deliver it.
        D(1900)=valid && p->stage==3 && D(1648);
    } else s->dropped++;
}

static Result *callback(u8 session,u32 signal) {
    (void)session;s->reply.action=0;
    if(signal==0) {
        D(1916)=R(0xe0001000u,4);s->slots++;s->phase=2;s->slot_us=s->request.a;
#ifndef ROY_SERVICE_MODE
        {   // Phase lock to the stock BLE 3000us grid (RTC1, 32768Hz): 3000us is
            // exactly 12288 units of 1/(32768*125)s. D(1884)=bit31|RTC tick of the
            // target slot start. Next distance lands the next slot on target.
            // Service build omits this: controllers are off, nothing to avoid,
            // and the freed space funds the block re-request below. D(1880) stays 0.
            u32 ref=D(1884);i32 e=0;
            if(ref>>31){e=(i32)((((R(0x40011000u,0x504)-ref)&0xffffffu)*125u)%12288u);if(e>6144)e-=12288;}
            D(1880)=(u32)(-((e*250)>>10));
        }
#endif
        if(s->request.type==1)s->slot_us=s->request.b;
        if(s->slots>=D(1608))s->stop=1;
        R(TIMER,0x140)=0;R(TIMER,0x540)=D(1892);R(TIMER,0x304)=1u<<16;
        if(!s->stop) {
            // 6000us BLE-locked slots in a 7-slot cycle: each tracker every 42ms
            // (42000 mod 4727.5 = 4180us, inside its RX window); a just-offered
            // peer takes the next slot to confirm within ~23ms; others idle.
            u32 i=(s->slots-1)%D(1888);   // host-set cycle length (default 7)
            for(u32 k=0;k<3;++k)if(peers[k].stage==2)i=k;
            if(i>2)goto slot_idle;
            D(1644)=i;D(1648)=0;
            volatile Peer *p=&peers[i];
            if(p->reserved&1)goto slot_idle;
            D(1620)=p->id;
            // DWT 64MHz cycles, independent of the handshake/fast slot cadence.
            u32 timeout=p->stage==2?1472000u:D(1612);
            if(p->stage && R(0xe0001000u,4)-p->last_rx>timeout)p->stage=0;
            u32 base=p->base,prefix=p->prefix,channel=2;
            if(p->stage<2) {
                const u8 startup[6]={1,37,77,4,78,78};
                u32 search=(p->search++)%6;
                channel=startup[search];
                if(search<3 || !p->id){base=0x552c6a1e;prefix=0xc0;}
                u32 length=p->id?21:9;
                s->dma[0]=length;s->dma[1]=2;
                for(u32 n=0;n<length;++n)s->dma[n+2]=0;
                if(p->id) {
                    s->dma[2]=4;s->dma[4]=0x7a;s->dma[8]=0x55;s->dma[9]=1;
                    for(u32 n=0;n<4;++n) {
                        s->dma[14+n]=((volatile u8 *)(DATA+1856))[n];
                        s->dma[19+n]=((volatile u8 *)&p->id)[n];
                    }
                    s->dma[18]=i;p->stage=2;p->last_rx=R(0xe0001000u,4);
                } else s->dma[2]=0xc1;
            } else if(p->stage==2) {
                // Confirm on the very next own slot, not after a host/SWD round trip.
                const u8 confirm[5]={0,9,6,1,0};
                for(u32 n=0;n<5;++n)s->dma[n+2]=confirm[n];
                for(u32 n=0;n<4;++n)s->dma[n+7]=((volatile u8 *)&p->id)[n];
                {u32 sum=0;for(u32 n=2;n<11;++n)sum+=s->dma[n];s->dma[11]=(u8)sum;}
                s->dma[0]=10;s->dma[1]=2;
            } else {
                volatile Mailbox *m=&boxes[i];u32 generation=m->generation;
                __asm__ volatile("dmb" ::: "memory");
                u32 meta=m->meta,length=meta&255;
                if(!(generation&1) && length && length<=56) {
                    for(u32 n=0;n<length;++n)s->dma[n+2]=m->payload[n];
                    __asm__ volatile("dmb" ::: "memory");
                    if(generation==m->generation) {
                        s->dma[0]=length;s->dma[1]=(meta>>8)&3;D(1648)=generation&0xffff;
                    } else length=0;
                } else length=0;
                if(!length){s->dma[0]=1;s->dma[1]=2;s->dma[2]=0;D(1648)=0;}
                s->dma[1]=2|D(1900);D(1900)=0;
            }
            R(RADIO,0x308)=~0u;R(RADIO,0x200)=0;
            R(RADIO,0x508)=channel;R(RADIO,0x510)=4;R(RADIO,0x514)=0x40008;
            R(RADIO,0x518)=0x01040096;R(RADIO,0x51c)=base;R(RADIO,0x524)=prefix;
            R(RADIO,0x530)=1;R(RADIO,0x534)=2;R(RADIO,0x538)=0x11021;
            R(RADIO,0x53c)=0xffff;R(RADIO,0x554)=0;R(RADIO,0x504)=(u32)s->dma;
            R(RADIO,0x10c)=0;R(RADIO,0x104)=0;R(RADIO,0x110)=0;
            R(RADIO,0x52c)=0;R(RADIO,0x50c)=0;R(RADIO,0x304)=0;
            // Stock MODECNF0 is 0x201 (fast ramp-up, DTX=Center), set once at boot
            // and never rewritten by the BLE stack: keep DTX or controllers degrade.
            R(RADIO,0x650)=0x201;R(RADIO,0x200)=11;D(1904)=R(0xe0001000u,4);D(1908)++;R(RADIO,0)=1;
            while(!R(RADIO,0x10c)){
                if(R(0xe0001000u,4)-D(1916)>=(D(1892)-100)*64u)goto slot_end;
            }
            D(1912)++;
            for(u32 n=0;n<1024 && R(RADIO,0x550)>=9;++n)__asm__ volatile("nop");
            if(!R(RADIO,0x550)){s->stop=1;stop_radio();}
            R(RADIO,0x10c)=0;R(RADIO,0x104)=0;R(RADIO,0x200)=19;
            R(TIMER,0x44)=1;u32 now=R(TIMER,0x544);
            // Do not arm a compare beyond the fixed RX deadline or in the past.
            // A late TX ends this slot, without stopping the whole session.
            u32 end=D(1892);if(now>=end)goto slot_end;
            u32 next=now+100;R(TIMER,0x540)=next<end?next:end;
slot_idle:;
        }
    } else if(signal==1 && R(TIMER,0x140)) {
        R(TIMER,0x140)=0;R(TIMER,0x44)=1;u32 tick=R(TIMER,0x544),end=D(1892);
        // IRQ delivery can be late: the programmed compare is not current time.
        if(tick>=end)goto slot_end;
        if(R(RADIO,0x10c)) {
            // Latest END replaces the native last-reply list even when its
            // frame is corrupt/unretained: never reuse an older ACK token.
            D(1900)=0;
            if(R(RADIO,0x400))capture();
            R(RADIO,0x10c)=0;R(RADIO,0x104)=0;
            if(tick<end && !s->stop && !R(RADIO,0x550))R(RADIO,0x4)=1;
        }
        // Extending RX while a reply is in flight (+150us) froze controllers on
        // 2026-10-07: the slot end stays fixed.
        if(tick<end && !s->stop) {
            u32 next=tick+100;R(TIMER,0x540)=next<end?next:end;
        } else {
slot_end:
            stop_radio();s->phase=1;s->reply.action=2;
            // One cadence for handshake and streaming (size budget): the
            // former separate steady mode starved the Valve controllers.
            if(!s->stop){s->request.type=1;s->request.a=s->interval_us+D(1880);s->request.b=D(1876);
                s->reply.next=(Request *)&s->request;s->reply.action=3;}
        }
    } else if(signal>=5 && signal<=7){s->blocked++;s->phase=3;
#ifdef ROY_SERVICE_MODE
        // Low-priority signals must return NONE. Stock dispatch sets the session
        // requestable before BLOCKED/CANCELLED; use the API outside a timeslot.
        // Returning REQUEST here asserts in stock MPSL (module106,line472).
        if(!s->stop){if(s->request.type==1 && *((volatile u8 *)0x20010592)==2){u32 elapsed=((R(0xe0001000u,4)-D(1916))>>6)+300;s->request.a=(elapsed/s->interval_us+1)*s->interval_us;s->request.b=s->slot_us;}else{s->request.type=0;s->request.a=s->slot_us;s->request.b=100000;}
            s->result=((i32(*)(u8,void*))0x18949)(s->session,(void*)&s->request);
            if(!s->result)s->phase=1;else s->stop=1;}
#endif
    }
    else if(signal==9){s->phase=0;s->session=255;}
    else if(signal==8 || signal==10){s->stop=1;s->phase=4;}
    return (Result *)&s->reply;
}

__attribute__((section(".entry"),used)) void entry(const void *shell,const u8 *bytes,u32 count) {
    u32 irq;__asm__ volatile("mrs %0, primask\ncpsid i":"=r"(irq)::"memory");
    R(0xe000ed00u,0x98)=1;R(0xe000ed00u,0x9c)=0x20000011;R(0xe000ed00u,0xa0)=0x110b0021;
    volatile u8 *regions=(volatile u8 *)0x20010a26;
    if(*regions==2){R(0xe000ed00u,0x98)=2;R(0xe000ed00u,0x9c)=0x20011012;
        R(0xe000ed00u,0xa0)=0x010b0017;*regions=3;}
    R(0xe000ed00u,0x94)=5;__asm__ volatile("dsb\nisb\nmsr primask, %0"::"r"(irq):"memory");
    u32 operation=count?bytes[0]-'0':3;D(1604)++;
    ((void(*)(const void*,void*))0x2ee2f)(shell,(void*)0);
    i32 error=0,locked=0;
    if(operation==0){D(1868)=R(0xe0001000u,0);D(1872)=R(0xe000ed00u,0xfc);
        R(0xe000ed00u,0xfc)=D(1872)|(1u<<24);R(0xe0001000u,0)=D(1868)|1;}
    if(operation<3){error=((i32(*)(u32,u32))0x28e2d)(0,0);locked=!error;if(error)operation=3;}
    if(operation==0){error=((i32(*)(void*,u8*))0x10581)(callback,(u8 *)&s->session);if(!error)s->phase=1;}
    if((operation==0 && !error)||operation==1){s->request.type=0;s->request.clock=0;
        s->request.priority=D(1616);s->request.a=s->slot_us;s->request.b=100000;
        error=((i32(*)(u8,void*))0x18949)(s->session,(void*)&s->request);}
    if(operation==2){error=((i32(*)(u8))0x189bf)(s->session);
        if(!error){R(0xe0001000u,0)=D(1868);R(0xe000ed00u,0xfc)=D(1872);}}
    if(locked)((void(*)(void))0x28e3d)();s->result=error;D(1600)=0x52455431;
}
