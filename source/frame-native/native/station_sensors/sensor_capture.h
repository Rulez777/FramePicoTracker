#pragma once
#include <stdint.h>
// Pure C capture helpers shared by the SRAM candidate and offline tests.
typedef struct {
    uint32_t number,crc,rssi,channel;
    uint16_t length,reserved;
    uint8_t bytes[80];
    uint32_t chip,cycles;
    uint16_t us,pad;
} PicoSensorRecord;
static inline unsigned pico_sensor_ack_revision(const volatile uint8_t *dma,unsigned len,unsigned revision) {
    unsigned ack=0;
    for(unsigned o=2;o+3<=len+1;) {
        unsigned n=dma[o+2];
        if(n>len+1-o-3) break;
        if(dma[o+1]==2) {
            if(revision==12 || (revision==0 && (n&1) && (dma[o+3]&0xf8)==0xc8)) {
                unsigned marker=n?dma[o+3]:0,count=marker&3;
                if((marker&0xf8)==0xc8 && n==3+16*count+((marker&4)?6u:0u) && count)
                    ack=0x100|dma[o+5+16*count];
            } else if(n==18||n==24||n==36||n==42) ack=0x100|dma[o+2+(n>=36?36:18)];
        }
        o+=3+n;
    }
    return ack;
}
static inline unsigned pico_sensor_ack(const volatile uint8_t *dma,unsigned len) {
    return pico_sensor_ack_revision(dma,len,0);
}
static inline void pico_sensor_record(volatile PicoSensorRecord *r,const volatile uint8_t *dma,
                                     unsigned len,uint32_t chip,uint32_t cycles,uint32_t seq,
                                     uint32_t generation,uint32_t rssi,uint32_t channel) {
    r->number=seq;r->crc=generation<<16;r->rssi=rssi&255;r->channel=channel;
#ifdef PICO_SENSOR_COPY
    typedef uint32_t sensor_word __attribute__((may_alias));
    *(volatile sensor_word *)&r->length=len+2;
#else
    r->length=(uint16_t)(len+2);r->reserved=0;
#endif
#ifdef PICO_SENSOR_COPY
    PICO_SENSOR_COPY(r->bytes,dma,len+2);
#else
    for(unsigned k=0;k<len+2;++k)r->bytes[k]=dma[k];
#endif
    r->chip=chip;r->cycles=cycles;
#ifdef PICO_SENSOR_COPY
    *(volatile sensor_word *)&r->us=0;
#else
    r->us=0;r->pad=0;
#endif
}
