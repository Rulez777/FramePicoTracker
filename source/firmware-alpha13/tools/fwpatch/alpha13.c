/* Frame-only event consumer. Actual stock B3D8 performs I2C and conversion. */
typedef unsigned char u8;
typedef unsigned u32;
#define B(a) (*(volatile u8 *)(a))
#define W(a) (*(volatile u32 *)(a))
void alpha13_measure(void) {
  u8 sample[8];
  B(0x2000516du)=1;
  __asm__ volatile("dmb sy":::"memory");
  if (B(0x20005150u)!=1) { B(0x2000516du)=0; return; }
  B(0x20001fbau)++;
  B(0x2000516eu)=0;
  ((void (*)(void *))0xb3d9u)(sample);
  if (B(0x2000516eu)==1) {
    ((void (*)(const void *))0x9741u)(sample);
    __asm__ volatile("dmb sy":::"memory");
    W(0x20005174u)++;
  }
  __asm__ volatile("dmb sy":::"memory");
  B(0x2000516du)=0;
}
