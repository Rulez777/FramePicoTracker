#include "alpha_capture.h"
/* Sensor-preserving ackring8 extension: one peer, 14 complete 80-byte envelopes.
 * Original scheduler, phase correction and module-owned DWT are retained. */
/* Roy build1788551713 only. Temporary SRAM, no NVMC/flash operations.
 * ABI4: MCU owns discovery/offer/early confirm; host supplies working packets
 * through three seqlocked mailboxes. Source is experimental until end-to-end
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
static volatile Mailbox *const boxes=(volatile Mailbox *)(DATA+1664);
_Static_assert((__builtin_offsetof(State,request)+__builtin_offsetof(Request,clock))%2==0,"request clock/priority alignment");
_Static_assert(__builtin_offsetof(State,dma)==120 && ((DATA+120)&1)==0,"DMA header halfword alignment");
_Static_assert(sizeof(State)==1500,"ABI4 record and DMA reservation");
_Static_assert(sizeof(Record)==112,"ABI4 ten-record ring");
_Static_assert(sizeof(Peer)==32 && sizeof(Mailbox)==64,"ABI4 peer/mailbox");

__attribute__((noinline)) static void copy_bytes(volatile u8 *dst,const volatile u8 *src,u32 count) {
    /* All private callers prove count>0: 4,5,len+2 or validated 1..56. */
    do {*dst++=*src++;} while(--count);
}

__attribute__((noinline)) static void stop_radio(void) {
    R(TIMER,0x308)=1u<<16;R(RADIO,0x308)=~0u;R(RADIO,0x200)=0;R(RADIO,0x10)=1;
    for(u32 n=0;n<1024 && R(RADIO,0x550);++n)__asm__ volatile("nop");
}

/* DATA+1880 MCU phase correction for the next distance (us, signed); +1884 bit31|RTC1 target tick; +1888 slot cycle length.
 * +1868/+1872 trace saves; +1876 handshake slot length. */
// D(1892)=slot RX end in us (host: slot length - margin, default margin 200us): 80us froze a controller within
// 0.25s on 2026-10-06; the radio must be idle well before the next BLE event.
__attribute__((noinline)) static void capture(void) {
    volatile Peer *p=&peers[0];
    u32 len=s->dma[0];
    if(len>78){s->dropped++;return;}
    u32 sum=0;
    for(u32 n=2;n<len+1;++n)sum+=s->dma[n];
    u32 valid=0;
    if(p->stage>=2 && len && (u8)sum==s->dma[len+1]) {
        valid=1;p->last_rx=R(0xe0001000u,4);p->reply_s1=s->dma[1]|(D(1648)<<16);
        u32 ack=pico_alpha_ack(s->dma,len);if(ack)D(1924)=ack;
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
    *(volatile u32 *)(slot+4)=R(0xe0001000u,4);*(volatile u16 *)(slot+8)=(u16)D(1648);
    *(volatile u32 *)slot=0x80000000u|(rest<<24)|((u32)(s->dma[1]&3)<<19)|(0<<21)|(s->packets&0x7ffffu);
    if(rest)copy_bytes(slot+10,&s->dma[2],rest);
    s->enabled=0x494d5536u;
    __asm__ volatile("dmb" ::: "memory");s->write_count=number+1;
}

__attribute__((always_inline)) static inline void mailbox(void) {
                volatile Mailbox *m=&boxes[0];u32 generation=m->generation;
                __asm__ volatile("dmb" ::: "memory");
                u32 meta=m->meta,length=meta&255;
                u32 reference=((volatile u32 *)&m->payload[48])[0],ticks=((volatile u32 *)&m->payload[48])[1];
                // Only phase-reference bit16 is admitted; other upper metadata flags reject.
                if(!((generation&1)|(meta>>17)) && length-1<56) {
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
                        *(volatile alias_u16 *)&s->dma[0]=(u16)(length|(meta&0x300));D(1648)=generation&0xffff;
                    } else length=0;
                } else length=0;
                if(!length){*(volatile alias_u16 *)&s->dma[0]=0x201;s->dma[2]=0;D(1648)=0;}
}

__attribute__((noinline)) static void prepare_packet(void) {
            D(1648)=0; // Peer index remains initialized zero.
            volatile Peer *p=&peers[0];


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
                { // Phase101 requires a known nonzero target identity.
                    s->dma[2]=4;s->dma[4]=0x7a;s->dma[8]=0x55;s->dma[9]=1;
                    copy_bytes(&s->dma[14],(volatile u8 *)(DATA+1856),4);
                    copy_bytes(&s->dma[19],(volatile u8 *)&p->id,4);
                    s->dma[18]=0;p->stage=2;p->last_rx=R(0xe0001000u,4);
                }
            } else if(p->stage==2) {
                // Confirm on the very next own slot, not after a host/SWD round trip.
                static const u8 confirm[5]={0,9,6,1,0};
                copy_bytes(&s->dma[2],confirm,5);
                copy_bytes(&s->dma[7],(volatile u8 *)&p->id,4);
                {u32 sum=0;for(u32 n=2;n<11;++n)sum+=s->dma[n];s->dma[11]=(u8)sum;}
                *(volatile alias_u16 *)&s->dma[0]=0x20a;
            } else {
                mailbox();
                // IMU ack on the MCU (D1928 = 0x100|tag enables): the tracker picks which ring
                // samples to send from the command-11 counter (FW189 0x97f4); the host round trip
                // lagged >11 samples and forced latest-5. Appended last, so it overrides a stale
                // host command 11 in the same packet (records are processed in order).
                u32 a=D(1924),t=D(1928),len=s->dma[0];
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
        D(1916)=R(0xe0001000u,4);s->slots++;s->phase=2;  // ackring7: slot_us (diagnostic State word, never read by the host) dropped for code space
#ifndef ROY_SERVICE_MODE
        {   // Phase lock to the stock BLE 3000us grid (RTC1, 32768Hz): 3000us is
            // exactly 12288 units of 1/(32768*125)s. D(1884)=bit31|RTC tick of the
            // target slot start. Next distance lands the next slot on target.
            // Service build omits this: controllers are off, nothing to avoid,
            // and the freed space funds the block re-request below. D(1880) stays 0.
            // 2026-10-30 ackring4: code space. The host never sets D(1884) (d1884=0 in every station_counters
            // line), so this block always produced D(1880)=0; keep exactly that.
            // ackring7: D(1880) is no longer read at all (the slot check and the request distance below used it), so
            // the write is gone too; behaviour equals D(1880)=0.
        }
#endif
        if(s->slots>=D(1608))s->stop=1;
        R(TIMER,0x140)=0;R(TIMER,0x540)=D(1892);R(TIMER,0x304)=1u<<16;
        if(!s->stop) {
            // ackring7: the D(1880) idle check is gone (D(1880) was always 0 here).
            // 6000us BLE-locked slots in a 7-slot cycle: each tracker every 42ms
            // (42000 mod 4727.5 = 4180us, inside its RX window); a just-offered
            // peer takes the next slot to confirm within ~23ms; others idle.
            u32 i=0; // Phase101 deliberately supports target peer0 only.
            if(peers[0].stage!=2 && (s->slots-1)%D(1888))goto slot_idle;
            if(peers[0].reserved&1)goto slot_idle;
            prepare_packet();
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

            // ackring7: D(1908) TX-end diagnostic counter dropped for code space (s12 still counts slots).
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
#ifndef ROY_SERVICE_MODE
        // Phase99: host-enabled (D1896) immediate re-request, same API/args as service mode.
        if(D(1896) && !s->stop && !peers[0].reserved){s->result=request_initial();
            if(!s->result)s->phase=1;else s->stop=1;}
#endif
#ifdef ROY_SERVICE_MODE
        // Low-priority signals must return NONE. Stock dispatch sets the session
        // requestable before BLOCKED/CANCELLED; use the API outside a timeslot.
        // Returning REQUEST here asserts in stock MPSL (module106,line472).
        if(D(1896) && !s->stop && !peers[0].reserved){s->result=request_initial();
            if(!s->result)s->phase=1;else s->stop=1;}
#endif
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
    u32 operation=count?bytes[0]-'0':3; // Native-call diagnostic omitted in this bounded candidate.
    // Ack variant: operation4 (RTC row diagnostic, unused by the native producer) removed for code space.
    ((void(*)(const void*,void*))ROY_FN_PRINT)(shell,(void*)0);
    if(operation>=4)return;
    i32 error=0,locked=0;
    if(operation==0){D(1868)=R(0xe0001000u,0);D(1872)=R(0xe000ed00u,0xfc);
        R(0xe000ed00u,0xfc)=D(1872)|(1u<<24);R(0xe0001000u,0)=D(1868)|1;}
    if(operation<3){error=((i32(*)(u32,u32))ROY_FN_LOCK)(0,0);locked=!error;if(error)operation=3;}
    if(operation==0){error=((i32(*)(void*,u8*))ROY_FN_OPEN)(callback,(u8 *)&s->session);if(!error)s->phase=1;}
    if((operation==0 && !error)||operation==1){*(volatile alias_u16 *)&s->request.clock=(u16)(D(1616)<<8);
        error=request_initial();}
    if(operation==2){error=((i32(*)(u8))ROY_FN_CLOSE)(s->session);
        if(!error){R(0xe0001000u,0)=D(1868);R(0xe000ed00u,0xfc)=D(1872);}}
    if(locked)((void(*)(void))ROY_FN_UNLOCK)();s->result=error; // Unused trap-only marker omitted.
}
