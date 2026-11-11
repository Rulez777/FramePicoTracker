/* Alpha 1.2: foreground only, stock lifecycle owns the MAG device. */
typedef unsigned char u8;
typedef unsigned u32;
#define B(a) (*(volatile u8 *)(a))
#define W(a) (*(volatile u32 *)(a))
#define M 0x20005160u
/* Reserved RAM: +0 last OS tick, +4 attempts, +8 successes, +12 product
   result (0 unknown, 1 pass, 255 fail), +16 initialization cookie. */
u32 alpha_mag_timer_flags(void) {
  u32 h=W(0x20001fbcu);
  if (!h) return 0;
  if ((h&3) || h<0x20000000u || h>0x20005fe0u) return 1;
  /* RTX timer object: ID f2, state 1=stopped/2=running, callback +24. */
  if (B(h)!=0xf2 || W(h+24)!=0xb2b1u) return 1;
  return 3u | (B(h+1)==2 ? 4u:0u);
}
void alpha_mag_product(u32 result) {
  if (B(0x20005150u)==1) B(M+12)=(result==1)?1:255;
}
void alpha_mag_reconcile(void) {
  u32 pm,ipsr;
  __asm__ volatile("mrs %0, primask\nmrs %1, ipsr":"=r"(pm),"=r"(ipsr));
  if (B(0x20005150u)!=1 || pm || ipsr) return;
  /* A previous interrupted runtime may leave SRAM intact. This worker is
     serial: at its next entry no earlier invocation still owns lifecycle. */
  B(M+13)=0;
  u32 now=W(0x200005d0u);
  if (W(M+16)!=0x4d414731u) {
    W(M)=now; W(M+4)=0; W(M+8)=0; W(M+12)=0;
    W(M+16)=0x4d414731u;
#ifdef ALPHA13
    W(M+20)=0;
#endif
    return;
  }
  if ((u32)(now-W(M))<1000u) return;
  W(M)=now;
  /* Gate is stock station/power policy, shared with IR. Never clear it. */
#ifndef ALPHA13
  if (B(0x20001ff6u)) return;
#endif
  u32 flags=alpha_mag_timer_flags();
  if (B(0x200003d8u)==1 && (flags&4)) return;
  B(M+13)=1;
  __asm__ volatile("dmb sy":::"memory");
  /* Boundary defers mode commits while this worker owns the lifecycle.
     If it committed just before busy, recheck without masking interrupts. */
  if (B(0x20005150u)!=1) { B(M+13)=0; return; }
  W(M+4)=W(M+4)+1;
  /* A product-ID failure leaves device active, but no valid timer. Do not
     pass a null/stale/foreign object to the stock timer destructor. */
  if (!(flags&2)) W(0x20001fbcu)=0;
  if (B(0x200003d8u)) ((void (*)(void))0xa75du)();
  /* Stock stop deletes the timer but retains its pointer. Clear that stale
     pointer before open so a failed product check cannot expose freed RAM. */
  W(0x20001fbcu)=0;
  ((void (*)(void))0xa751u)();
  if (B(0x200003d8u)==1 && (alpha_mag_timer_flags()&4))
    W(M+8)=W(M+8)+1;
  __asm__ volatile("dmb sy":::"memory");
  B(M+13)=0;
}
