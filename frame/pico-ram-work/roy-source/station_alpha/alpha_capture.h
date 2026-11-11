#pragma once
#include <stdint.h>
// Bounded command walker shared by the compact module and host model.
static inline unsigned pico_alpha_ack(const volatile uint8_t *d,unsigned len) {
 unsigned ack=0;
 for(unsigned o=2;o+3<=len+1;) {
  unsigned n=d[o+2]; if(n>len-1-o)break;
  if(d[o+1]==2) {
   unsigned end=0;
   if(n&1) {
    unsigned m=d[o+3],c=m&3;
    if((m&0xf8)==0xc8 && n==3+16*c+((m&4)?6u:0u) && c)end=2+16*c;
   } else {
    if(n==18||n==24)end=17;
    if(n==36||n==42)end=35;
   }
   if(end)ack=0x100|d[o+3+end];
  }
  o+=3+n;
 }
 return ack;
}
