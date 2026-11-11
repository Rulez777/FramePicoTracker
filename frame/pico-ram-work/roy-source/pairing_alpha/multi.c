#include "alpha_capture.h"
#include "pair_slot.h"
/* Sensor-preserving multi4 extension: K=1..3, 14 complete 80-byte envelopes.
 * Peer/mailbox indices 0..2 retain the proven ABI4 layout. DWT is host-owned.
 * No installation or RF qualification is implied by an offline build. */
/* Roy build1788551713 only. Temporary SRAM, no NVMC/flash operations.
 * ABI4: MCU owns discovery/offer/early confirm; host supplies working packets
 * through seqlocked mailboxes. Source is experimental until end-to-end
 * controller coexistence, reconnect and optical synchronization are measured.
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef u16 alias_u16 __attribute__((may_alias));
typedef unsigned int u32;
typedef int i32;
#define R(b,o) (*(volatile u32 *)((b)+(o)))
#ifndef ROY_DATA
#define ROY_DATA 0x20010c18u
#endif
#define DATA ROY_DATA
#ifndef ROY_FN_OPEN
#define ROY_FN_OPEN 0x10581
#endif
#ifndef ROY_FN_CLOSE
#define ROY_FN_CLOSE 0x189bf
#endif
#ifndef ROY_FN_LOCK
#define ROY_FN_LOCK 0x28e2d
#endif
#ifndef ROY_FN_UNLOCK
#define ROY_FN_UNLOCK 0x28e3d
#endif
#ifndef ROY_FN_PRINT
#define ROY_FN_PRINT 0x2ee2f
#endif
#ifndef ROY_FN_REQUEST
#define ROY_FN_REQUEST 0x18949
#endif
#ifndef ROY_MPU_BASE1
#define ROY_MPU_BASE1 0x20000011
#endif
#ifndef ROY_MPU_ATTR1
#define ROY_MPU_ATTR1 0x110b0021
#endif
/* 0x20011012 = code region base (0x20011000) | 0x12 and must follow ROY_CODE. */
#ifndef ROY_MPU_BASE2
#define ROY_MPU_BASE2 0x20011012
#endif
#ifndef ROY_MPU_ATTR2
#define ROY_MPU_ATTR2 0x010b0017
#endif
#ifndef ROY_REGIONS
#define ROY_REGIONS 0x20010a26
#endif
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
static volatile Mailbox *const boxes=(volatile Mailbox *)(DATA+1664);  // 3 x 64 B (1664..1855); 1856 = station seed
// multi4: ABI4 has only three clean peer slots. peers[3] (1600..1631) would overlap the host words D(1608) slot
// limit, D(1612) timeout, D(1616) priority (live_prepared_state.hpp put 1608/1612/1616). The extra state must stay
// inside DATA..DATA+2047, the part the host checkpoints and restores (roy_owner.cpp:131,262); D1932.. was only used
// by the ack2/diag diagnostic rings and the host never writes it.
#ifdef MULTI_PEER3_TAIL
// variant: peer 3 at 1936, its mailbox at 1968, acks at 2032 (branch per access)
#define PEER(i) ((volatile Peer *)((i)<3?DATA+1504+32*(i):DATA+1936))
#define BOX(i) ((volatile Mailbox *)((i)<3?DATA+1664+64*(i):DATA+1968))
#define ACK(i) D(2032+4*(i))
#define CUR D(1644)
#define GEN D(1648)
#define OFFER D(1932)
#else
// default: peer 3 uses ABI4's fifth peer slot (1632..1663, never written by the host); index i -> slot i+(i==3)
// without a branch. That slot held ackring8's scratch words D(1644) (current peer) and D(1648) (mailbox generation
// of the packet in flight), so they move to 1932/1936; mailbox 3 at 1944, acks at 2008.
#define PEER(i) ((volatile Peer *)(DATA+1504+32*(i)))
#define BOX(i) ((volatile Mailbox *)(DATA+1664+64*(i)))
#define ACK(i) D(2008+4*(i))  // per-peer newest IMU counter, bit8 = valid (ackring8: one word D(1924))
#define CUR D(1932)
#define GEN D(1936)
#define OFFER D(1940)
#endif
_Static_assert((__builtin_offsetof(State,request)+__builtin_offsetof(Request,clock))%2==0,"request clock/priority alignment");
_Static_assert(__builtin_offsetof(State,dma)==120 && ((DATA+120)&1)==0,"DMA header halfword alignment");
_Static_assert(sizeof(State)==1500,"ABI4 record and DMA reservation");
_Static_assert(sizeof(Record)==112,"ABI4 ten-record ring");
_Static_assert(sizeof(Peer)==32 && sizeof(Mailbox)==64,"ABI4 peer/mailbox");
_Static_assert(1944+64<=2008 && 2008+16<=2048 && 1936+32<=1968 && 1968+64<=2032 && 2032+16<=2048,"multi4 extra words inside the 2048-byte prepared image / SRAM checkpoint");

__attribute__((noinline)) static void copy_bytes(volatile u8 *dst,const volatile u8 *src,u32 count) {
    /* All private callers prove count>0: 4,5,len+2 or validated 1..56. */
    do {*dst++=*src++;} while(--count);
}

__attribute__((noinline)) static void stop_radio(void) {
    R(TIMER,0x308)=1u<<16;R(RADIO,0x308)=~0u;R(RADIO,0x200)=0;R(RADIO,0x10)=1;
    for(u32 n=0;n<1024 && R(RADIO,0x550);++n)__asm__ volatile("nop");
}

// CUR = peer index of the current slot (set in the START branch before prepare_packet, read by capture).
// D(1892)=slot RX end in us (host: slot length - margin, default margin 200us): 80us froze a controller within
// 0.25s on 2026-10-06; the radio must be idle well before the next BLE event.
__attribute__((noinline)) static void capture(void) {
    u32 i=CUR;volatile Peer *p=PEER(i);
    u32 len=s->dma[0];
    if(len>78){s->dropped++;return;}
    u32 sum=0;
    for(u32 n=2;n<len+1;++n)sum+=s->dma[n];
    u32 valid=0;
    if(p->stage>=2 && len && (u8)sum==s->dma[len+1]) {
        valid=1;p->last_rx=R(0xe0001000u,4);p->reply_s1=s->dma[1]|(GEN<<16);
        u32 ack=pico_alpha_ack(s->dma,len);if(ack)ACK(i)=ack;
        // Promote only on the working channel, i.e. after the confirm slot,
        // never on a reply to the startup-channel offer itself.
        if(p->stage==2 && R(RADIO,0x508)==2)p->stage=3;
    }
    s->packets++;
    // Complete working-channel envelopes: 14 x 80 bytes in the unchanged 1120-byte ring.
    // Raw header retains peer, sequence, s1, cycles and mailbox generation.
    if(!valid || R(RADIO,0x508)!=2)return;
    u32 number=s->write_count,rest=len-1;
    if(number-s->read_count>=14 || rest>70u){s->dropped++;return;}
    volatile u8 *slot=(volatile u8 *)s->records+(number%14)*80;
    *(volatile u32 *)(slot+4)=R(0xe0001000u,4);*(volatile u16 *)(slot+8)=(u16)GEN;
    *(volatile u32 *)slot=0x80000000u|(rest<<24)|((u32)(s->dma[1]&3)<<19)|(i<<21)|(s->packets&0x7ffffu);
    if(rest)copy_bytes(slot+10,&s->dma[2],rest);
    s->enabled=0x494d5537u;
    __asm__ volatile("dmb" ::: "memory");s->write_count=number+1;
}

__attribute__((always_inline)) static inline void mailbox(u32 i) {
#ifdef MULTI_SHARED_BOX  // alternative: one mailbox, target peer in meta bits 24..25, host serializes commands
                volatile Mailbox *m=&boxes[0];u32 generation=m->generation;
#else
                volatile Mailbox *m=BOX(i);u32 generation=m->generation;
#endif
                __asm__ volatile("dmb" ::: "memory");
                u32 meta=m->meta,length=meta&255;
                u32 reference=((volatile u32 *)&m->payload[48])[0],ticks=((volatile u32 *)&m->payload[48])[1];
                // Only phase-reference bit16 is admitted; other upper metadata flags reject.
#ifdef MULTI_SHARED_BOX
                if(!((generation&1)|((meta>>17)&0x7f)) && (meta>>24)==i && length-1<56) {
#else
                if(!((generation&1)|(meta>>17)) && length-1<56) {
#endif
                    copy_bytes(&s->dma[2],m->payload,length);
                    __asm__ volatile("dmb" ::: "memory");
                    if(generation==m->generation) {
                        if(meta>>16) {
                            if(length!=8 || s->dma[3]!=10 || s->dma[4]!=4 || ticks>=533333)length=0;
                            else {
                                ticks=(ticks+((R(0xe0001000u,4)-reference)>>2))%533333;
                                u32 sum=s->dma[2]+s->dma[3]+s->dma[4];
                                for(u32 n=5;n<9;n++){u8 byte=(u8)ticks;s->dma[n]=byte;sum+=byte;ticks>>=8;}
                                s->dma[9]=(u8)sum;
                            }
                        }
                        *(volatile alias_u16 *)&s->dma[0]=(u16)(length|(meta&0x300));GEN=generation&0xffff;
                    } else length=0;
                } else length=0;
                if(!length){*(volatile alias_u16 *)&s->dma[0]=0x201;s->dma[2]=0;GEN=0;}
}

__attribute__((noinline)) static void prepare_packet(u32 i) {
            GEN=0;
            volatile Peer *p=PEER(i);

            // DWT 64MHz cycles, independent of the handshake/fast slot cadence.
            u32 timeout=p->stage==2?1472000u:D(1612);
            if(p->stage && R(0xe0001000u,4)-p->last_rx>timeout)p->stage=0;
            u32 base=p->base,prefix=p->prefix,channel=2;
            if(p->stage<2) {
                static const u8 startup[6]={1,37,77,4,78,78};
                u32 search=(p->search++)%6;
                channel=startup[search];
                if(search<3){base=0x552c6a1e;prefix=0xc0;}
                u32 length=21;
                *(volatile alias_u16 *)&s->dma[0]=(u16)(length|0x200);
                for(u32 n=0;n<length;++n)s->dma[n+2]=0;
                { // Every peer needs a known nonzero target identity (host enrollment).
                    s->dma[2]=4;s->dma[4]=0x7a;s->dma[8]=0x55;s->dma[9]=1;
                    copy_bytes(&s->dma[14],(volatile u8 *)(DATA+1856),4);
                    copy_bytes(&s->dma[19],(volatile u8 *)&p->id,4);
                    s->dma[18]=(u8)pico_offer_slot(p->prefix);p->stage=2;p->last_rx=R(0xe0001000u,4);
                    OFFER=i+1;  // this peer confirms in the next slot (one offer outstanding at a time)
                }
            } else if(p->stage==2) {
                // Confirm on the very next own slot, not after a host/SWD round trip.
                static const u8 confirm[5]={0,9,6,1,0};
                copy_bytes(&s->dma[2],confirm,5);
                copy_bytes(&s->dma[7],(volatile u8 *)&p->id,4);
                {u32 sum=0;for(u32 n=2;n<11;++n)sum+=s->dma[n];s->dma[11]=(u8)sum;}
                *(volatile alias_u16 *)&s->dma[0]=0x20a;
            } else {
                mailbox(i);
                // Per-peer IMU ack on the MCU (D1928 = 0x100|tag enables), appended last (FW189 0x97f4).
                u32 a=ACK(i),t=D(1928),len=s->dma[0];
                if((a&t&0x100) && len<=52) {
                    volatile u8 *e=&s->dma[len+1];u8 c=*e;
                    e[0]=(u8)t;e[1]=11;e[2]=1;e[3]=(u8)a;e[4]=(u8)(c+t+12+a);s->dma[0]=(u8)(len+4);
                }
            }
 R(RADIO,0x308)=~0u;R(RADIO,0x200)=0;
 R(RADIO,0x508)=channel;R(RADIO,0x51c)=base;R(RADIO,0x524)=prefix;
}

__attribute__((noinline)) static i32 request_initial(void) {
    s->request.type=0;s->request.a=D(1876);s->request.b=100000;
    return ((i32(*)(u8,void*))ROY_FN_REQUEST)(s->session,(void*)&s->request);
}

static Result *callback(u8 session,u32 signal) {
    (void)session;s->reply.action=0;
    if(signal==0) {
        D(1916)=R(0xe0001000u,4);s->slots++;s->phase=2;
        if(s->slots>=D(1608))s->stop=1;
        R(TIMER,0x140)=0;R(TIMER,0x540)=D(1892);R(TIMER,0x304)=1u<<16;
        if(!s->stop) {
            // multi4: round robin over D(1888) peers (1..4); each peer is polled every D(1888) x interval_us,
            // which must equal (N+1) x 3000 us of that tracker (command 16 N, read back by query).
            // A peer waiting for its confirm (stage 2) takes the next slot, as in phase90.
            u32 i=(s->slots-1)%D(1888);
#if defined(MULTI_CONFIRM_SCAN)
            for(u32 k=0;k<4;++k)if(PEER(k)->stage==2)i=k;   // phase90 style scan: +44 B
#elif !defined(MULTI_NO_CONFIRM_PRIORITY)
            {u32 c=OFFER;if(c && PEER(c-1)->stage==2)i=c-1;}  // OFFER = 1 + the peer that sent the last offer
#endif
            if(i>2 || (PEER(i)->reserved&1))goto slot_idle;
            CUR=i;
            prepare_packet(i);
            R(RADIO,0x510)=4;R(RADIO,0x514)=0x40008;
            R(RADIO,0x518)=0x01040096;
            R(RADIO,0x530)=1;R(RADIO,0x534)=2;R(RADIO,0x538)=0x11021;
            R(RADIO,0x53c)=0xffff;R(RADIO,0x554)=0;R(RADIO,0x504)=(u32)s->dma;
            R(RADIO,0x10c)=0;R(RADIO,0x104)=0;R(RADIO,0x110)=0;
            R(RADIO,0x52c)=0;R(RADIO,0x50c)=0;R(RADIO,0x304)=0;
            // Stock MODECNF0 is 0x201 (fast ramp-up, DTX=Center), set once at boot
            // and never rewritten by the BLE stack: keep DTX or controllers degrade.
            R(RADIO,0x650)=0x201;R(RADIO,0x200)=11;R(RADIO,0)=1;
            while(!R(RADIO,0x10c)) {
                if(R(0xe0001000u,4)-D(1916)>=(D(1892)-100)*64u)goto slot_end;
            }

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
        // A completed END_DISABLE frame is already off air. Retain it even
        // if compare delivery was late; never restart RX past the fixed end.
        if(R(RADIO,0x10c)) {
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
            if(!s->stop){s->request.type=1;s->request.a=s->interval_us;s->request.b=D(1876);
                s->reply.next=(Request *)&s->request;s->reply.action=3;}
        }
    } else if(signal>=5 && signal<=7){s->blocked++;s->phase=3;
        // Phase99: host-enabled (D1896) immediate re-request. multi4: peer 0 pause flag kept as the session pause.
        if(D(1896) && !s->stop && !peers[0].reserved){s->result=request_initial();
            if(!s->result)s->phase=1;else s->stop=1;}
    }
    else if(signal==9){s->phase=0;s->session=255;}
    else if(signal==8 || signal==10){s->stop=1;s->phase=4;}
    return (Result *)&s->reply;
}

__attribute__((section(".entry"),used)) void ROYSTATIONTHREE4(const void *shell,const u8 *bytes,u32 count) {
    u32 irq;__asm__ volatile("mrs %0, primask\ncpsid i":"=r"(irq)::"memory");
    R(0xe000ed00u,0x98)=1;R(0xe000ed00u,0x9c)=ROY_MPU_BASE1;R(0xe000ed00u,0xa0)=ROY_MPU_ATTR1;
    volatile u8 *regions=(volatile u8 *)ROY_REGIONS;
    if(*regions==2){R(0xe000ed00u,0x98)=2;R(0xe000ed00u,0x9c)=ROY_MPU_BASE2;
        R(0xe000ed00u,0xa0)=ROY_MPU_ATTR2;*regions=3;}
    R(0xe000ed00u,0x94)=5;__asm__ volatile("dsb\nisb\nmsr primask, %0"::"r"(irq):"memory");
    u32 operation=count?bytes[0]-'0':3;
    ((void(*)(const void*,void*))ROY_FN_PRINT)(shell,(void*)0);
    if(operation>=4)return;
    i32 error=0,locked=0;
#ifdef MULTI_MODULE_DWT  // default (no MULTI_MODULE_DWT): the host saves DEMCR/DWT_CTRL, sets TRCENA/CYCCNTENA over SWD before op0 and restores after op2
    if(operation==0){D(1868)=R(0xe0001000u,0);D(1872)=R(0xe000ed00u,0xfc);
        R(0xe000ed00u,0xfc)=D(1872)|(1u<<24);R(0xe0001000u,0)=D(1868)|1;}
#endif
    if(operation<3){error=((i32(*)(u32,u32))ROY_FN_LOCK)(0,0);locked=!error;if(error)operation=3;}
    if(operation==0){error=((i32(*)(void*,u8*))ROY_FN_OPEN)(callback,(u8 *)&s->session);if(!error)s->phase=1;}
    if((operation==0 && !error)||operation==1){*(volatile alias_u16 *)&s->request.clock=(u16)(D(1616)<<8);
        error=request_initial();}
    if(operation==2){error=((i32(*)(u8))ROY_FN_CLOSE)(s->session);
#ifdef MULTI_MODULE_DWT
        if(!error){R(0xe0001000u,0)=D(1868);R(0xe000ed00u,0xfc)=D(1872);}
#endif
    }
    if(locked)((void(*)(void))ROY_FN_UNLOCK)();s->result=error;
}
