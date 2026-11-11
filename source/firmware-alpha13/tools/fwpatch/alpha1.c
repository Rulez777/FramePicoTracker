/* Alpha 1 runtime extensions; no startup or flash operations. */
typedef unsigned char u8;
typedef unsigned u32;
#define B(a) (*(volatile u8 *)(a))
#define W(a) (*(volatile u32 *)(a))
#define STATE 0x20005150u
#define AUX 0x20005158u
#ifdef ALPHA12
u32 alpha_mag_timer_flags(void);
#endif
void alpha_ack(u32 ack) {
  if (B(AUX+1) && ((ack-B(AUX))&255)>=128 &&
      !(((B(0x20001afcu)-B(AUX))&255)>=128 &&
        ((B(0x20001afcu)-ack)&255)<8)) return;
  B(AUX)=ack; B(AUX+1)=1;
}
void alpha_imu(void *unused, const u8 *in, u32 len, u8 *out, u8 *used, u8 *room) {
  if (!B(0x20001082u) || B(0x20000f6au) || *room<=3) return;
  for (u32 pos=0; pos<len;) {
    if (len-pos<3) break;
    u32 n=in[pos+2];
    if (n>=253 || n+3>len-pos || pos+n+3>255) break;
    if (in[pos+1]==11 && n>=1) alpha_ack(in[pos+3]);
    pos+=n+3;
  }
  /* Stock drains queued control replies AFTER IMU (5AA2 -> 5AAE).
     Reserve all queued records that can precede sensor space. Walk at most
     18 nodes (54/3); malformed/cyclic lists consume the reservation budget. */
  u32 available=*room;
  for (u32 q=0;q<3;++q) {
    u32 head=W((u32)unused+0x74+4*q);
    if (!head) continue;
    u32 record=W(head+4);
    for (u32 nodes=0;record && available>3 && nodes<18;++nodes) {
      if (record<0x20000000u || record>0x20005ff0u || (record&3)) return;
      u32 size=B(record+15)+3;
      if (size<=available) available-=size;
      else available=0;
      record=W(record+4);
    }
    if (record) return;
  }
  if (available<=3) return;
  u32 cap=available-3;
  if (cap>51) cap=51;
  u32 mag=(B(0x20001081u)^1u)&1;
  if (cap < (mag?9u:3u)) return;
  u8 *body=out+*used+3;
  u32 size=3, count=0, latest;
  u32 pm; __asm__ volatile("mrs %0, primask\ncpsid i":"=r"(pm)::"memory");
  latest=B(0x20001afcu);
  /* Eight-sample retention cannot justify a half-cycle-old ACK horizon.
     Expire that horizon; flags/counter equality still prevent invented data. */
  if (B(AUX+1) && ((latest-B(AUX))&255)>=128) B(AUX+1)=0;
  const volatile u8 *last=(volatile u8 *)(0x20001afcu+9+18*(latest&7));
  body[1]=last[12]; body[2]=last[13];
  for (u32 age=8; age>0; --age) {
    u32 c=(latest+1-age)&255, slot=c&7;
    volatile u8 *sample=(volatile u8 *)(0x20001afcu+9+18*slot);
    u32 diff=(c-B(AUX))&255;
    if (B(AUX+1) && (!diff || diff>=128)) continue;
    if (B(0x20001afdu+slot)!=1 || sample[17]!=c) continue;
    if (size+16+(mag?6u:0u)>cap) break;
    for (u32 j=0;j<12;++j) body[size+j]=sample[j];
    for (u32 j=0;j<4;++j) body[size+12+j]=sample[14+j];
    B(0x20001afdu+slot)=0;
    size+=16; ++count;
  }
  if (mag) {
    const volatile u8 *m=(volatile u8 *)(0x20001b98u+6*((1-B(0x20001b96u))&1));
    for (u32 j=0;j<6;++j) body[size+j]=m[j];
    size+=6;
  }
  body[0]=(u8)(0xc8u|count|(mag?4:0));
  B(0x20001081u)=mag;
  __asm__ volatile("dmb sy\nmsr primask, %0"::"r"(pm):"memory");
  out[*used+1]=2; out[*used+2]=(u8)size;
  *used+=(u8)(size+3); *room-=(u8)(size+3);
}
/* Stock 57FC..5812 order is mandatory: DISABLED_TXEN must be removed first.
   Both exits leave software state idle for the stock receive-tail scheduler. */
u32 alpha_halt(void) {
  u32 state=B(0x200008c8u);
  if (state!=4 && state!=5) return 0x101;
  B(0x20000fe3u)=0;
  W(0x40001200u)=0; W(0x40001308u)=0xffffffffu;
  W(0x20000e04u)=0;
  W(0x40001110u)=0; W(0x40001010u)=1;
  u32 budget=2048;
  while (budget && (!W(0x40001110u) || W(0x40001550u))) --budget;
  B(0x200008c8u)=0;
  ((void (*)(void))0x852d)();
  if (!budget) {
    B(STATE+3)=1; W(STATE+4)=W(STATE+4)+1;
    B(STATE+1)=0;
    return 0x102;
  }
  return 0;
}
/* Eight 6-byte sensor status pages. FAF is an enum, FA0 is WHO_AM_I. */
void alpha_snapshot(u8 *out, u32 page) {
#ifdef ALPHA13
  u8 data[60];
#else
  u8 data[48];
#endif
  for (u32 i=0;i<sizeof(data);++i) data[i]=0;
  data[0]=B(0x20001fafu); data[1]=B(0x20001fa0u);
  data[2]=B(0x20000442u); data[3]=B(0x20001fb0u);
  u32 cfg=data[2], variant=data[0];
  data[4]=(cfg<128 && variant<=1 && data[3])?1:0;
  data[5]=(variant<=1)?(u8)(cfg&15):0;
  data[6]=(variant<=1)?(u8)(cfg&15):0;
  data[7]=B(0x20001f87u);
  data[8]=B(0x20000fb2u); data[9]=B(0x20000fb3u);
  for (u32 i=0;i<6;++i) data[10+i]=B(0x20000138u+i);
  for (u32 i=0;i<4;++i) data[16+i]=B(0x20001fc0u+i);
  data[20]=B(0x20001fbau);
  data[21]=B(0x20000fa8u);
  data[23]=B(0x20000445u);
  for (u32 i=0;i<4;++i) data[24+i]=B(0x20000ef0u+i);
  for (u32 i=0;i<4;++i) data[28+i]=B(0x20000f6au+i);
  data[32]=B(0x20000ffeu); data[33]=B(STATE+3);
  for (u32 i=0;i<4;++i) data[34+i]=B(STATE+4+i);
  data[38]=B(STATE+1); data[39]=B(0x20000f21u);
#ifdef ALPHA12
  data[40]=B(0x200003d8u); data[41]=B(0x20001ff6u);
  data[42]=(u8)alpha_mag_timer_flags(); data[43]=B(0x20000443u);
  data[44]=B(0x20001ba9u);
  if (W(0x20005170u)==0x4d414731u) {
    data[45]=B(0x2000516cu); data[46]=B(0x20005164u);
    data[47]=B(0x20005168u);
  }
#endif
#ifdef ALPHA13
  data[22]=0x13;
  /* Page 8/9: raw input plus read-only stock admission result, full width
     selection in TIMER1 ticks and successful publication sequence. */
  data[48]=B(0x20001ff8u);
  data[49]=(u8)((u32 (*)(void))0xc5d5u)();
  for (u32 i=0;i<4;++i) data[50+i]=B(0x20000124u+i);
  if (W(0x20005170u)==0x4d414731u)
    for (u32 i=0;i<4;++i) data[54+i]=B(0x20005174u+i);
#endif
  for (u32 i=0;i<6;++i) out[i]=data[page*6+i];
}
