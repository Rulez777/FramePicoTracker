#define main fwpatch_cli_main
#include "main.cpp"
#undef main
#include <array>
#include <functional>
#include <set>
#include <cstring>
struct Cpu {
  Bytes flash, ram = Bytes(0x6000, 0);
  std::array<uint32_t, 16> r{};
  std::map<uint32_t, uint32_t> mmio;
  std::vector<std::pair<uint32_t, uint32_t>> stores;
  uint32_t apsr = 0xa8000000, primask = 0, ipsr = 0;
  unsigned it = 0, steps = 0;
  bool disabled_arrives = true;
  bool radio_model = false, rssi_running = false;
  unsigned radio_phase = 0, shortcut_txen = 0, shortcut_rxen = 0;
  void radio_disabled() {
    mmio[0x40001110]=1; mmio[0x40001550]=0; radio_phase=0;
    unsigned shorts=mmio[0x40001200];
    if(shorts&0x100) rssi_running=false;
    if(shorts&4) { ++shortcut_txen; mmio[0x40001550]=9; radio_phase=1; }
    else if(shorts&8) { ++shortcut_rxen; mmio[0x40001550]=1; radio_phase=1; }
  }
  void radio_tick() {
    unsigned shorts=mmio[0x40001200];
    if(radio_phase==1) {
      mmio[0x40001100]=1; // READY
      mmio[0x40001550]=mmio[0x40001550]==9?10:2;
      bool tx=mmio[0x40001550]==10;
      if(shorts&1) mmio[0x40001550]=tx?11:3; // START
      radio_phase=((shorts&1) && tx)?2:0; // RX awaits an external packet
    } else if(radio_phase==2) {
      mmio[0x40001104]=1; // ADDRESS
      if(shorts&0x10) rssi_running=true;
      radio_phase=3;
    } else if(radio_phase==3) {
      mmio[0x4000110c]=1; // END
      radio_phase=0;
      if((shorts&2) && disabled_arrives) radio_disabled();
    }
  }
  Bytes reply;
  std::set<unsigned> stops;
  std::map<unsigned, std::function<void(Cpu &)>> calls;
  std::function<void(Cpu &)> tick;
  Cpu(const Bytes &b) : flash(b) {
    for (unsigned i = 0; i < 13; ++i)
      r[i] = 0x12340000 + i * 0x101;
    r[13] = 0x20005e00;
    r[14] = 0xdead0001;
    calls[0x9368] = [](Cpu &c) {
      c.reply.clear();
      for (unsigned i = 0; i < c.r[1]; ++i)
        c.reply.push_back(uint8_t(c.load(c.r[0] + i, 1)));
    };
    calls[0x852c] = [](Cpu &) {};
    calls[0x5fe0] = [](Cpu &) {};
    calls[0x62e4] = [](Cpu &c) { c.mmio[0x40008000] = 1; };
  }
  uint32_t load(uint32_t a, unsigned n) {
    uint32_t v = 0;
    if (a >= 0x1000 && uint64_t(a) + n <= 0x1000 + flash.size()) {
      for (unsigned i = 0; i < n; ++i)
        v |= uint32_t(flash[a - 0x1000 + i]) << (8 * i);
      return v;
    }
    if (a >= 0x20000000 && uint64_t(a) + n <= 0x20006000) {
      for (unsigned i = 0; i < n; ++i)
        v |= uint32_t(ram[a - 0x20000000 + i]) << (8 * i);
      return v;
    }
    require(n == 4 && ((a >= 0x40000000 && a < 0x60000000) ||
                       (a >= 0xe000e000 && a < 0xe0010000)),
            "unmapped read " + std::to_string(a) + " pc " +
                std::to_string(r[15]));
    return mmio[a];
  }
  void store(uint32_t a, uint32_t v, unsigned n) {
    if (a >= 0x20000000 && uint64_t(a) + n <= 0x20006000) {
      for (unsigned i = 0; i < n; ++i)
        ram[a - 0x20000000 + i] = uint8_t(v >> (8 * i));
      return;
    }
    require(n == 4 && ((a >= 0x40000000 && a < 0x60000000) ||
                       (a >= 0xe000e000 && a < 0xe0010000)),
            "unmapped write " + std::to_string(a) + " pc " +
                std::to_string(r[15]));
    unsigned previous=mmio[a];
    mmio[a] = v;
    stores.push_back({a, v});
    if (a == 0x40001308) mmio[0x40001304] &= ~v;
    if (a == 0x40001304) mmio[a] = previous | v;
    if(radio_model && v==1) {
      if(a==0x40001000 || a==0x40001004) {
        mmio[0x40001550]=a==0x40001000?9:1;radio_phase=1;
      }
      if(a==0x40001014) rssi_running=true;
      if(a==0x40001018) rssi_running=false;
    }
    if (a == 0x40001010 && v == 1 && disabled_arrives) {
      if(radio_model) radio_disabled();
      else mmio[0x40001110] = 1;
    }
  }
  bool cond(unsigned c) {
    bool n = apsr >> 31, z = apsr >> 30 & 1, k = apsr >> 29 & 1,
         v = apsr >> 28 & 1;
    switch (c) {
    case 0:
      return z;
    case 1:
      return !z;
    case 2:
      return k;
    case 3:
      return !k;
    case 4:
      return n;
    case 5:
      return !n;
    case 6:
      return v;
    case 7:
      return !v;
    case 8:
      return k && !z;
    case 9:
      return !k || z;
    case 10:
      return n == v;
    case 11:
      return n != v;
    case 12:
      return !z && n == v;
    case 13:
      return z || n != v;
    case 14:
      return true;
    default:
      throw std::runtime_error("bad condition");
    }
  }
  void nz(uint32_t x) {
    apsr = (apsr & 0x3fffffff) | (x & 0x80000000) | (x == 0 ? 0x40000000 : 0);
  }
  uint32_t add(uint32_t a, uint32_t b, bool flags) {
    uint32_t v = a + b;
    if (flags) {
      nz(v);
      apsr = (apsr & ~0x30000000u) |
             (uint64_t(a) + b > 0xffffffff ? 0x20000000 : 0) |
             ((~(a ^ b) & (a ^ v) & 0x80000000) ? 0x10000000 : 0);
    }
    return v;
  }
  uint32_t sub(uint32_t a, uint32_t b, bool flags) {
    uint32_t v = a - b;
    if (flags) {
      nz(v);
      apsr = (apsr & ~0x30000000u) | (a >= b ? 0x20000000 : 0) |
             (((a ^ b) & (a ^ v) & 0x80000000) ? 0x10000000 : 0);
    }
    return v;
  }
  static int32_t sext(uint32_t v, unsigned bits) {
    return int32_t(v << (32 - bits)) >> (32 - bits);
  }
  void push(unsigned mask) {
    unsigned count = 0;
    for (unsigned i = 0; i < 16; ++i)
      count += (mask >> i) & 1;
    r[13] -= 4 * count;
    unsigned a = r[13];
    for (unsigned i = 0; i < 16; ++i)
      if (mask >> i & 1) {
        store(a, r[i], 4);
        a += 4;
      }
  }
  void pop(unsigned mask) {
    unsigned a = r[13];
    for (unsigned i = 0; i < 16; ++i)
      if (mask >> i & 1) {
        r[i] = load(a, 4);
        if (i == 15)
          r[i] &= ~1u;
        a += 4;
      }
    r[13] = a;
  }
  unsigned imm12(unsigned h, unsigned w) {
    return ((h >> 10) & 1) << 11 | ((w >> 12) & 7) << 8 | (w & 255);
  }
  uint32_t expand(unsigned v) {
    if ((v >> 10) == 0) {
      unsigned b = v & 255;
      switch (v >> 8) {
      case 0:
        return b;
      case 1:
        return b << 16 | b;
      case 2:
        return b << 24 | b << 8;
      case 3:
        return b * 0x01010101;
      }
    }
    unsigned n = v >> 7, x = 128 | (v & 127);
    return x >> n | x << (32 - n);
  }
  void step() {
    unsigned pc = r[15], h = load(pc, 2);
    unsigned w = (h & 0xf800)>=0xe800 ? load(pc+2,2) : 0;
    bool wide = (h & 0xf800) >= 0xe800, inside = it != 0;
    unsigned next = pc + (wide ? 4 : 2);
    r[15] = next;
    ++steps;
    if (inside) {
      bool yes = cond(it >> 4);
      if ((it & 7) == 0)
        it = 0;
      else
        it = (it & 0xe0) | ((it << 1) & 31);
      if (!yes)
        return;
    }
    auto jump = [&](unsigned p) { r[15] = p & ~1u; };
    if (wide) {
      if((h&0xfff0)==0xfb10 && (w&0xf0)==0) { // SMLABB
        unsigned rd=(w>>8)&15,ra=w>>12;
        r[rd]=uint32_t(int32_t(int16_t(r[h&15]))*int32_t(int16_t(r[w&15])))+r[ra];return;
      }
      if (h == 0xe92d) {
        push(w);
        return;
      }
      if (h == 0xe8bd) {
        pop(w);
        return;
      }
      if ((h & 0xffd0)==0xe890) {
        unsigned base=h&15,a=r[base];
        for(unsigned j=0;j<16;++j)if(w&(1u<<j)){r[j]=load(a,4);a+=4;}
        if(h&32)r[base]=a;
        if(w&0x8000)r[15]&=~1u;
        return;
      }
      if ((h & 0xffd0)==0xe880) {
        unsigned base=h&15,a=r[base];
        for(unsigned j=0;j<16;++j)if(w&(1u<<j)){store(a,r[j],4);a+=4;}
        if(h&32)r[base]=a;
        return;
      }
      if (h == 0xf3ef) {
        unsigned sys = w & 255;
        r[(w >> 8) & 15] = sys == 16 ? primask : sys == 5 ? ipsr : apsr;
        require(sys == 0 || sys == 16 || sys == 5, "MRS sys");
        return;
      }
      if ((h & 0xfff0) == 0xf380) {
        unsigned sys = w & 255;
        require(sys == 0 || sys == 16, "MSR sys");
        if (sys == 16)
          primask = r[h & 15] & 1;
        else
          apsr = r[h & 15] & 0xf8000000;
        return;
      }
      if (h == 0xf3bf && (w == 0x8f5f || w == 0x8f4f || w == 0x8f6f))
        return;
      if(h==0xfa5f || h==0xfa1f) {
        r[(w>>8)&15]=r[w&15]&(h==0xfa5f?255u:65535u);return;
      }
      if ((h & 0xfbf0) == 0xf240 || (h & 0xfbf0) == 0xf2c0) {
        unsigned v = (h & 15) << 12 | imm12(h, w), rd = (w >> 8) & 15;
        if ((h & 0xfbf0) == 0xf240)
          r[rd] = v;
        else
          r[rd] = (r[rd] & 65535) | (v << 16);
        return;
      }
      if ((h & 0xffef) == 0xf04f) {
        unsigned rd = (w >> 8) & 15;
        r[rd] = expand(imm12(h, w));
        if (h & 16)
          nz(r[rd]);
        return;
      }
      if ((h & 0xfa00)==0xf000 && !(w&0x8000)) {
        unsigned op=(h>>5)&15, rn=h&15, rd=(w>>8)&15;
        uint32_t a=rn==15?0:r[rn], b=expand(imm12(h,w)), v=0;
        bool flags=(h&16)!=0;
        switch(op) {
        case 0:v=a&b;if(flags)nz(v);break;
        case 1:v=a&~b;if(flags)nz(v);break;
        case 2:v=a|b;if(flags)nz(v);break;
        case 3:v=a|~b;if(flags)nz(v);break;
        case 4:v=a^b;if(flags)nz(v);break;
        case 8:v=add(a,b,flags);break;
        case 13:v=sub(a,b,flags);break;
        case 14:v=sub(b,a,flags);break;
        default:throw std::runtime_error("unsupported modified immediate ALU "+std::to_string(op));
        }
        if(rd!=15)r[rd]=v;
        return;
      }
      if ((h & 0xfe00) == 0xea00) {
        unsigned rd=(w>>8)&15,rn=h&15,op=(h>>5)&15;
        unsigned sh=((w>>12)&7)*4+((w>>6)&3),type=(w>>4)&3;
        uint32_t a=rn==15?0:r[rn],b=r[w&15],v=0;
        if(type==0)b<<=sh;
        else if(type==1)b=sh?b>>sh:0;
        else if(type==2)b=sh?uint32_t(int32_t(b)>>sh):uint32_t(int32_t(b)>>31);
        else {require(sh!=0,"RRX unsupported");b=(b>>sh)|(b<<(32-sh));}
        bool flags=h&16;
        if(h&0x100) {
          if(op==8)v=add(a,b,flags);
          else if(op==13)v=sub(a,b,flags);
          else if(op==14)v=sub(b,a,flags);
          else throw std::runtime_error("shifted arithmetic unsupported");
        }else {
          if(op==0)v=a&b;else if(op==1)v=a&~b;
          else if(op==2)v=a|b;else if(op==3)v=a|~b;
          else if(op==4)v=a^b;else throw std::runtime_error("shifted logic unsupported");
          if(flags)nz(v);
        }
        if(rd!=15)r[rd]=v;
        return;
      }
      if ((h & 0xfff0) == 0xf8d0 || (h & 0xfff0) == 0xf8c0 ||
          (h & 0xfff0) == 0xf890 || (h & 0xfff0) == 0xf880 ||
          (h & 0xfff0) == 0xf8b0 || (h & 0xfff0) == 0xf8a0) {
        unsigned kind = h & 0xfff0,
                 base = (h & 15) == 15 ? ((pc + 4) & ~3u) : r[h & 15],
                 rd = w >> 12,
                 n = (kind == 0xf890 || kind == 0xf880)   ? 1
                     : (kind == 0xf8b0 || kind == 0xf8a0) ? 2
                                                          : 4;
        unsigned a = base + (w & 4095);
        if (kind == 0xf8d0 || kind == 0xf890 || kind == 0xf8b0) {
          r[rd] = load(a, n);
          if (rd == 15)
            jump(r[15]);
        } else
          store(a, r[rd], n);
        return;
      }
      if((h&0xfff0)==0xf990 || (h&0xfff0)==0xf9b0) {
        unsigned n=(h&0x20)?2:1,a=r[h&15]+(w&4095),rd=w>>12;
        auto v=load(a,n);r[rd]=n==1?uint32_t(int32_t(int8_t(v))):uint32_t(int32_t(int16_t(v)));return;
      }
      if ((h & 0xfff0) == 0xe9d0) {
        unsigned a = r[h & 15] + (w & 255) * 4;
        r[w >> 12] = load(a, 4);
        r[(w >> 8) & 15] = load(a + 4, 4);
        return;
      }
      if ((h & 0xfff0) == 0xe9c0) {
        unsigned a=r[h&15]+(w&255)*4;
        store(a,r[w>>12],4);store(a+4,r[(w>>8)&15],4);return;
      }
      if ((h&0xff80)==0xf800 && (h&0x60)!=0x60) {
        unsigned rn=h&15,rd=w>>12,kind=(h>>5)&3;
        unsigned n=kind==0?1:kind==1?2:4, base=r[rn],a;
        if(w&0x800) {
          unsigned delta=w&255,updated=(w&0x200)?base+delta:base-delta;
          a=(w&0x400)?updated:base;
          if(w&0x100)r[rn]=updated;
        }else a=base+(r[w&15]<<((w>>4)&3));
        if(h&16){r[rd]=load(a,n);if(rd==15)jump(r[rd]);}else store(a,r[rd],n);
        return;
      }
      if ((h & 0xf800) == 0xf000 && (w & 0x8000)) {
        unsigned s = (h >> 10) & 1;
        if ((w & 0xd000) == 0x8000) {
          unsigned v = s << 20 | ((w >> 11) & 1) << 19 | ((w >> 13) & 1) << 18 |
                       (h & 63) << 12 | (w & 2047) << 1;
          if (cond((h >> 6) & 15))
            jump(pc + 4 + sext(v, 21));
          return;
        }
        unsigned v = s << 24 | (1 ^ ((w >> 13) & 1) ^ s) << 23 |
                     (1 ^ ((w >> 11) & 1) ^ s) << 22 | (h & 1023) << 12 |
                     (w & 2047) << 1;
        if (w & 0x4000)
          r[14] = (pc + 4) | 1;
        jump(pc + 4 + sext(v, 25));
        return;
      }
    } else {
      if((h&0xffc0)==0xba40) {
        unsigned rd=h&7,rm=(h>>3)&7;auto v=r[rm];
        r[rd]=((v&0x00ff00ffu)<<8)|((v&0xff00ff00u)>>8);return;
      }
      if((h&0xff00)==0xb200) {
        unsigned rd=h&7,rm=(h>>3)&7,op=(h>>6)&3;
        r[rd]=op==0?uint32_t(int32_t(int16_t(r[rm]))):op==1?uint32_t(int32_t(int8_t(r[rm]))):op==2?r[rm]&65535:r[rm]&255;
        return;
      }
      if((h&0xe000)==0 && (h&0x1800)!=0x1800) {
        unsigned rd=h&7,rm=(h>>3)&7,sh=(h>>6)&31,op=h>>11;
        uint32_t v=r[rm],carry=(apsr>>29)&1;
        if(op==0){if(sh){carry=(v>>(32-sh))&1;v<<=sh;}}
        else if(op==1){if(!sh)sh=32;carry=(v>>(sh-1))&1;v=sh==32?0:v>>sh;}
        else {if(!sh)sh=32;carry=(v>>(sh-1))&1;v=uint32_t(int32_t(v)>>(sh==32?31:sh));}
        r[rd]=v;if(!inside){nz(v);apsr=(apsr&~0x20000000u)|(carry<<29);}return;
      }
      if((h&0xfc00)==0x4000 && (h&0xffc0)!=0x4280) {
        unsigned rd=h&7,rm=(h>>3)&7,op=(h>>6)&15;
        uint32_t a=r[rd],b=r[rm],v;
        if(op==0)v=a&b;else if(op==1)v=a^b;else if(op==12)v=a|b;
        else if(op==13)v=a*b;else if(op==14)v=a&~b;else if(op==15)v=~b;
        else if(op==9)v=sub(0,b,!inside);
        else if(op==5) {
          uint32_t carry=(apsr>>29)&1;
          uint64_t sum=uint64_t(a)+b+carry;v=uint32_t(sum);
          if(!inside) {
            apsr=(apsr&~0x30000000u)|(uint32_t(sum>>32)<<29)|
              ((~(a^b)&(a^v)&0x80000000u)>>3);
          }
        }
        else throw std::runtime_error("narrow ALU unsupported "+std::to_string(op));
        r[rd]=v;if(!inside)nz(v);return;
      }
      if ((h & 0xff00) == 0xbf00) {
        if (h & 15)
          it = h & 255;
        return;
      }
      if (h == 0xb672) {
        primask = 1;
        return;
      }
      if (h == 0xb662) {
        primask = 0;
        return;
      }
      if ((h & 0xfe00) == 0xb400) {
        push((h & 255) | ((h & 256) ? 1 << 14 : 0));
        return;
      }
      if ((h & 0xfe00) == 0xbc00) {
        pop((h & 255) | ((h & 256) ? 1 << 15 : 0));
        return;
      }
      if ((h & 0xf800) == 0x4800) {
        r[(h >> 8) & 7] = load(((pc + 4) & ~3u) + (h & 255) * 4, 4);
        return;
      }
      if ((h & 0xe000) == 0x6000 || (h & 0xf000) == 0x8000) {
        unsigned kind = h >> 11,
                 n = (kind == 14 || kind == 15)   ? 1
                     : (kind == 16 || kind == 17) ? 2
                                                  : 4,
                 a = r[(h >> 3) & 7] + ((h >> 6) & 31) * n, rd = h & 7;
        if (kind & 1)
          r[rd] = load(a, n);
        else
          store(a, r[rd], n);
        return;
      }
      if ((h & 0xf000) == 0x5000) {
        unsigned kind = (h >> 9) & 7, a = r[(h >> 3) & 7] + r[(h >> 6) & 7],
                 rd = h & 7,
                 n = (kind == 2 || kind == 3 || kind == 6)   ? 1
                     : (kind == 1 || kind == 5 || kind == 7) ? 2
                                                             : 4;
        require(kind != 3 && kind != 7, "signed reg load unsupported");
        if (kind >= 4)
          r[rd] = load(a, n);
        else
          store(a, r[rd], n);
        return;
      }
      if ((h & 0xf000) == 0x9000) {
        unsigned rd = (h >> 8) & 7, a = r[13] + (h & 255) * 4;
        if (h & 0x800)
          r[rd] = load(a, 4);
        else
          store(a, r[rd], 4);
        return;
      }
      if ((h & 0xf800) == 0xa000) {
        r[(h >> 8) & 7] = ((pc+4)&~3u) + (h & 255)*4;
        return;
      }
      if ((h & 0xf800) == 0xa800) {
        r[(h >> 8) & 7] = r[13] + (h & 255) * 4;
        return;
      }
      if ((h & 0xff00) == 0xb000) {
        r[13] = (h & 128) ? r[13] - (h & 127) * 4 : r[13] + (h & 127) * 4;
        return;
      }
      if ((h & 0xf500) == 0xb100) {
        if ((r[h & 7] != 0) == bool(h & 0x800))
          jump(pc + 4 + ((h >> 3) & 31) * 2 + ((h >> 9) & 1) * 64);
        return;
      }
      if ((h & 0xffc0) == 0x4280) {
        sub(r[h & 7], r[(h >> 3) & 7], true);
        return;
      }
      if ((h & 0xfc00) == 0x4400) {
        unsigned op = (h >> 8) & 3, rd = (h & 7) | ((h >> 4) & 8),
                 rm = (h >> 3) & 15;
        if (op == 0)
          r[rd] += r[rm];
        else if (op == 1)
          sub(r[rd], r[rm], true);
        else if (op == 2)
          r[rd] = r[rm];
        else {
          unsigned target = r[rm];
          if (h & 128)
            r[14] = (pc + 2) | 1;
          jump(target);
        }
        return;
      }
      if ((h & 0xe000) == 0x2000) {
        unsigned op = (h >> 11) & 3, rd = (h >> 8) & 7, v = h & 255;
        if (op == 0) {
          r[rd] = v;
          if (!inside)
            nz(v);
        } else if (op == 1)
          sub(r[rd], v, true);
        else if (op == 2)
          r[rd] = add(r[rd], v, !inside);
        else
          r[rd] = sub(r[rd], v, !inside);
        return;
      }
      if ((h & 0xf800) == 0x1800) {
        unsigned rd = h & 7, a = r[(h >> 3) & 7],
                 v = (h & 0x400) ? (h >> 6) & 7 : r[(h >> 6) & 7];
        r[rd] = (h & 0x200) ? sub(a, v, !inside) : add(a, v, !inside);
        return;
      }
      if ((h & 0xf800) == 0xe000) {
        jump(pc + 4 + sext((h & 2047) << 1, 12));
        return;
      }
      if ((h & 0xf000) == 0xd000 && (h >> 8 & 15) < 14) {
        if (cond((h >> 8) & 15))
          jump(pc + 4 + sext((h & 255) << 1, 9));
        return;
      }
    }
    std::ostringstream s;
    s << "unsupported opcode at " << std::hex << pc << ": " << h << " " << w;
    throw std::runtime_error(s.str());
  }
  bool run(unsigned start, unsigned budget = 20000) {
    r[15] = start & ~1u;
    for (unsigned i = 0; i < budget; ++i) {
      if (radio_model) radio_tick();
      if (tick) tick(*this);
      if (stops.count(r[15]) || r[15] == 0xdead0000)
        return true;
      auto c = calls.find(r[15]);
      if (c != calls.end()) {
        c->second(*this);
        r[15] = r[14] & ~1u;
      } else
        step();
    }
    return false;
  }
};
static unsigned cases = 0;
void check(bool b, const std::string &s) {
  require(b, s);
  ++cases;
}
#include "verification_cases.h"
#include "round2_cases.h"
#ifndef FWVERIFY_LIBRARY
int main() {
  try {
    static_and_negative_tests();
    delay_path_tests();
    for (std::string name : {"a", "b1", "c2"}) {
      std::string path = "outputs/frame-release-" + name;
      auto image = read(path + "/image.bin");
      auto e = elf(path + ".elf");
      for (auto &sym : e.symbols)
        if (sym.first.find("select_") == 0 || sym.first.find("delay_") == 0) {
          unsigned site = std::stoul(sym.first.substr(sym.first.find('_') + 1),
                                     nullptr, 16);
          for (unsigned mode : {0u, 1u, 2u, 255u})
            for (unsigned flags : {0u, 0xf8000000u, 0xa8000000u}) {
              Cpu c(image);
              c.apsr = flags;
              c.store(0x20005150, mode, 1);
              c.r[0] = 0x12345678;
              c.r[1] = 0x87654321;
              c.r[3] = 0x6d40;
              if (site == 0x6cc4)
                c.r[3] = 0x6d48;
              if (site == 0x6d36)
                c.r[3] = 0x6d50;
              if (site == 0x6f64)
                c.r[3] = 0x6f80;
              auto original = c.r;
              c.stops.insert(site + 4);
              check(c.run(site), "selector returned");
              check(c.r[13] == original[13] && c.r[14] == original[14] &&
                        c.apsr == flags,
                    "selector SP/LR/APSR");
              unsigned rd = 0, old = 0, value = 0;
              switch (site) {
              case 0x6214:
                rd = 4;
                old = 18910;
                value = 12000;
                break;
              case 0x6218:
                old = 4727;
                value = 3000;
                break;
              case 0x630a:
                old = 18910;
                value = 12000;
                break;
              case 0x630e:
                rd = 1;
                old = 4727;
                value = 3000;
                break;
              case 0x6cde:
                old = 3751;
                value = 2024;
                break;
              case 0x6ce4:
                old = 3355;
                value = 1628;
                break;
              default:
                if (sym.first.find("delay_") == 0) {
                  old = site == 0x6eac ? 10980 : 9922;
                  value = site == 0x6eac ? 7525 : 6466;
                } else
                  rd = 16;
              }
              if (rd != 16) {
                check(c.r[rd] == (mode == 1 ? value : old),
                      "selector immediate");
                for (unsigned r = 0; r < 13; ++r)
                  if (r != rd)
                    check(c.r[r] == original[r],
                          "selector register preservation");
              } else {
                unsigned addr = original[3];
                if (mode == 1)
                  addr = e.symbols.at(site == 0x6cc4   ? "frame_normal"
                                      : site == 0x6d36 ? "frame_alternate"
                                                       : "frame_period");
                check(c.r[2] == c.load(addr, 4) &&
                          c.r[3] == c.load(addr + 4, 4),
                      "double selector value");
                for (unsigned r = 0; r < 13; ++r)
                  if (r != 2 && r != 3)
                    check(c.r[r] == original[r],
                          "double selector preservation");
              }
            }
        }
      diagnostic_reply_equivalence(image);
      command_tests(image, e, name == "c2");
      boundary_tests(image, e, name == "c2");
      if (name == "c2") {
        breadcrumb_tests(image, e);
        hook_equivalence_tests(image);
        frozen_record_tests(image);
        round2_tests(image, e);
      }
      std::cout << "PASS " << name << " selectors/commands/boundary\n";
    }
    std::cout << "PASS checks " << cases << "\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "FAIL after " << cases << " checks: " << e.what() << "\n";
    return 1;
  }
}
#endif
