// Offline execution of the actual hook bytes. No peripheral timing claims.
#include <cstring>
void delay_path_tests() {
  auto b = read("outputs/frame-release-b1/image.bin");
  auto a = read("outputs/frame-release-a/image.bin");
  auto c = read("outputs/frame-release-c2/image.bin");
  for (unsigned site : {0x6538u,0x65a2u,0x67deu,0x6c42u,0x6c5cu}) {
    check(std::equal(c.begin()+site-0x1000,c.begin()+site-0x1000+4,
                     a.begin()+site-0x1000),"C2 has no B delay hooks");
    for (unsigned mode : {0u,1u,2u,255u}) {
      Cpu selector(b); selector.store(0x20005150,mode,1);
      selector.stops.insert(site+4);
      check(selector.run(site),"B1 site selects delay");
      unsigned us=selector.r[0];
      check(us==(mode==1?6466u:9922u),"B1 delay selection");
      Cpu rtc(b); rtc.r[0]=us;rtc.store(0x20000f02,1,1);
      // Explicit software floating point call models; stock dispatch, conversion
      // call order and RTC CLEAR/CC0 stores execute from original image bytes.
      rtc.calls[0x13e90]=[](Cpu &p) {
        double v=p.r[0]; uint64_t bits;std::memcpy(&bits,&v,8);
        p.r[0]=uint32_t(bits);p.r[1]=uint32_t(bits>>32);
      };
      rtc.calls[0x13a40]=[](Cpu &p) {
        uint64_t lhs=uint64_t(p.r[1])<<32|p.r[0];
        uint64_t rhs=uint64_t(p.r[3])<<32|p.r[2];
        double x,y;std::memcpy(&x,&lhs,8);std::memcpy(&y,&rhs,8);
        check(y==30.5,"stock RTC divisor exactly 30.5");
        double v=x/y;uint64_t bits;std::memcpy(&bits,&v,8);
        p.r[0]=uint32_t(bits);p.r[1]=uint32_t(bits>>32);
      };
      rtc.calls[0x13de8]=[](Cpu &p) {
        uint64_t bits=uint64_t(p.r[1])<<32|p.r[0];double x;
        std::memcpy(&x,&bits,8);p.r[0]=uint32_t(x);
      };
      check(rtc.run(0x641c),"stock RTC scheduling branch returns");
      check(rtc.mmio[0x4000b008]==1&&rtc.mmio[0x4000b540]==(mode==1?212u:325u),
            "stock RTC clear and quantized CC0");
    }
  }
  for (auto *p : {&b,&c})
    check(std::equal(p->begin()+0x5eac,p->begin()+0x5eb0,a.begin()+0x5eac),
          "10980 site stays stock");
  check(6466*2==212*61,"Frame nominal is RTC representable");
}
void round2_tests(const Bytes &image, const Elf &e) {
  auto original = stock();
  Cpu legacy(read("outputs/frame-release-c/image.bin"));
  seed(legacy);legacy.r[10]=0x20003000;
  legacy.store(0x20003001,0x18,1);legacy.stops.insert(0x9522);
  check(legacy.run(0x951e)&&legacy.load(0x2000531a,1)==80,
        "regression reproduces old mode zero dispatcher instrumentation");
  check(std::equal(image.begin()+0x432c, image.begin()+0x4330,
                   original.begin()+0x432c), "ADDRESS call exactly stock");
  for (unsigned site : {0x57ecu,0x5828u,0x5830u,0x8054u,0x80a6u,
                         0x83eau,0x83fcu,0xc376u,0xc388u,0x951eu,
                         0x8406u,0xc398u,0x9a94u})
    for (unsigned mode : {0u,1u,2u,255u})
      for (unsigned pm : {0u,1u})
        for (unsigned zero : {0u,1u,2u}) {
          Cpu now(image), base(original);
          for (Cpu *p : {&now,&base}) {
            seed(*p); p->primask=pm;
            p->store(0x20005150,mode,1);
            p->r[0]=zero; p->r[3]=3;
            p->r[4]=p->r[6]=0x20000fd0;
            p->r[10]=0x20003000; p->store(0x20003001,0x18,1);
            if(site==0x57ec) p->store(0x200008c8,zero?0:4,1);
            if(site==0x5828 || site==0x5830) p->push((1u<<4)|(1u<<14));
            if(site==0x5828) { p->r[4]=0; p->stops.insert(0x582c); }
            if(site==0x8054) p->stops.insert(0x8058);
            if(site==0x80a6) {p->r[2]=1;p->r[3]=0x40000000;p->stops.insert(0x80aa);}
            if(site==0x83ea) p->stops.insert(0x83ee);
            if(site==0xc376) p->stops.insert(0xc37a);
            if(site==0x83fc) p->stops.insert(zero?0x8400:0x840a);
            if(site==0xc388) p->stops.insert(zero?0xc38c:0xc3c4);
            if(site==0x951e) p->stops.insert(0x9522);
            if(site==0x8406||site==0xc398||site==0x9a94) p->stops.insert(0x9a98);
          }
          // BL exit wrappers return to the stock continuation after the callee.
          if(site==0x5828) now.r[14]=0x582d;
          check(now.run(site)&&base.run(site),"mode gate stock continuation");
          for(unsigned r=0;r<=13;++r)
            check(now.r[r]==base.r[r],"mode gate displaced register equivalence");
          check(now.apsr==base.apsr&&now.primask==base.primask,"mode gate flags/PRIMASK");
          check(now.mmio==base.mmio,"mode gate peripheral equivalence");
          if(mode!=1)
            check(std::equal(now.ram.begin()+0x5310,now.ram.begin()+0x5350,
                             base.ram.begin()+0x5310),"non Frame has no breadcrumb writes");
          else if(site!=0x9a94 || zero==2)
            check(now.load(0x20005310,4)==0x42435231,"Frame hook publishes breadcrumb");
        }
  // Each instruction boundary is a possible asynchronous reset. An interrupted
  // record is either the complete previous record, invalid, or complete new one.
  for(unsigned mode : {0u,2u,255u}) {
    Cpu c(image);c.store(0x20005150,mode,1);
    auto before=c.ram;
    check(c.run(e.symbols.at("breadcrumb_write")),"writer second mode check returns");
    check(std::equal(before.begin()+0x5310,before.begin()+0x5350,
                     c.ram.begin()+0x5310),"writer rejects mode changed after wrapper gate");
  }
  for(unsigned valid : {0u,1u}) {
    Cpu c(image);
    c.store(0x20005310,valid?0x42435231:0,4);
    c.store(0x20005314,44,4);c.store(0x20005318,0x11223344,4);
    c.store(0x20005150,1,1);
    c.r[0]=80;c.r[1]=2;c.r[2]=3;c.r[3]=0x18;
    c.r[15]=e.symbols.at("breadcrumb_write")&~1u;
    bool committed=false;
    for(unsigned n=0;n<150 && c.r[15]!=0xdead0000;++n) {
      if(c.load(0x20005310,4)==0x42435231) {
        unsigned seq=c.load(0x20005314,4);
        if(seq==(valid?45u:1u)) {
          committed=true;
          check(c.load(0x20005318,4)==0x00501802 && c.load(0x20005320,4)==3,
                "transaction validity follows all payload stores");
        } else check(valid && seq==44 && c.load(0x20005318,4)==0x11223344,
                     "precommit record remains old or invalid");
      }
      c.step();
    }
    check(committed,"transaction committed");
  }
  {
    Cpu c(image);
    c.store(0x20005310,0x42435231,4);
    c.store(0x20005314,77,4);c.store(0x20005318,0x11223344,4);
    c.store(0x20005320,2,4);c.mmio[0x40000400]=0x10008;
    c.r[15]=e.symbols.at("breadcrumb_boot")&~1u;
    bool frozen=false;
    for(unsigned n=0;n<150&&c.r[15]!=0x126e;++n) {
      if(c.load(0x20005328,4)==0x42435231) {
        frozen=true;
        check(c.load(0x2000532c,4)==77&&c.load(0x20005330,4)==0x11223344&&
              c.load(0x20005334,4)==0x10008&&c.load(0x20005338,4)==2,
              "snapshot validity follows complete copy and reset reason");
      }
      c.step();
    }
    check(frozen,"boot snapshot transaction committed");
  }
  for(unsigned frozen : {0u,1u}) {
    Cpu c(image);
    for(unsigned i=0;i<20;++i) {
      c.store(0x20005310+i,0x20+i,1);
      c.store(0x20005328+i,0x60+i,1);
    }
    c.store(0x20005328,frozen?0x42435231:0,4);
    for(unsigned i=0;i<34;++i) c.store(0x20005bde + i,0xa0+i,1);
    auto before=c.ram;
    Bytes payload;
    for(unsigned page=12;page<=17;++page) {
      c.r[0]=0x20003000;c.r[1]=18;c.r[14]=0xdead0001;
      c.store(c.r[0],0x7e,1);c.store(c.r[0]+1,page,1);
      check(c.run(0x8d16),"six diagnostic pages return");
      check(c.reply.size()==32&&c.reply[22]==frozen,"source byte on every page");
      payload.insert(payload.end(),c.reply.begin()+23,c.reply.end());
    }
    for(unsigned i=0;i<54;++i)
      check(payload[i]==before[(i<20?(frozen?0x5328:0x5310):0x5bde - 20)+i],
            "complete record and stock crash record exposed");
    check(std::equal(before.begin()+0x5bde,before.begin()+0x5c00,
                     c.ram.begin()+0x5bde),"stock crash record never written");
  }
}
