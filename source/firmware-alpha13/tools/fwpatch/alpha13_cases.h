void alpha13_tests(const Bytes &image,const Elf &e) {
  constexpr unsigned M=0x20005160;
  const auto old=read("outputs/alpha13-baseline/alpha12.bin");
  for(unsigned mode:{0u,1u,2u,255u})for(unsigned gate:{0u,1u,255u}) {
    Cpu p(image),s(stock()),a12(old);
    unsigned queued=0,cleared=0;
    auto setup=[&](Cpu &c){c.store(0x20005150,mode,1);c.store(0x20001ff6,gate,1);
      c.calls[0xd890]=[&](Cpu &x){check(x.r[0]==2,"normal event ID 2");++queued;};
      c.calls[0x9730]=[&](Cpu &){++cleared;};};
    setup(p);setup(s);setup(a12);
    check(a12.run(0xb2b0),"frozen Alpha12 callback");
    check(queued==(gate?0u:1u)&&cleared==(gate?1u:0u),"Alpha12 gate=1 reproduces clear/no measurement");
    queued=cleared=0;
    check(p.run(0xb2b0),"Alpha13 timer callback");
    check(queued==((mode==1||!gate)?1u:0u),"Frame queues despite independent");
    check(cleared==((mode!=1&&gate)?1u:0u),"only stock gate clears");
    check(p.load(0x20001ff6,1)==gate && p.stores.empty(),"callback leaves independent and MMIO alone");
    if(mode!=1) {
      check(s.run(0xb2b0),"stock callback comparison");
      check(p.r==s.r&&p.apsr==s.apsr,"mode0 timer exact registers and flags");
    }
  }
  // Frozen reconcile skip with independent=1, without modeling any lifecycle.
  { Cpu c(old);c.store(0x20005150,1,1);c.store(0x20001ff6,1,1);
    c.store(M+16,0x4d414731,4);c.store(0x200005d0,1000,4);
    auto oe=elf("outputs/frame-alpha12.elf");
    check(c.run(oe.symbols.at("alpha_mag_reconcile")),"frozen Alpha12 reconcile returns");
    check(c.load(M+4,4)==0,"Alpha12 independent=1 reproduces skipped reconcile"); }
  for(unsigned mode:{0u,1u})for(unsigned tagged:{0u,1u})for(unsigned failure:{0u,1u}) {
    Cpu c(image);c.store(0x20005150,mode,1);c.store(0x20001ff6,1,1);
    c.store(M+16,0x4d414731,4);c.store(0x20001fba,98,1);
    unsigned reads=0,writes=0;
    c.store(c.r[13]+8,tagged?0x4131334d:0,4);
    c.store(c.r[13]+12,tagged?0xbececcb2:0,4);
    c.calls[0xd768]=[&](Cpu &p){++reads;check(p.primask==0&&p.ipsr==0,"I2C foreground");
      if(mode)check(p.load(M+13,1)==1,"measurement serializes mode commit");
      for(unsigned j=0;j<6;++j)p.store(p.r[2]+j,0x80+j,1);p.r[0]=failure?7:0;};
    c.calls[0xd6e4]=[&](Cpu &p){++writes;p.r[0]=0;};
    c.calls[0x1490]=[](Cpu &){};c.calls[0x14b0]=[](Cpu &){};
    c.stops={0xda1c};
    check(c.run(0xda18),"actual worker event dispatch");
    bool dropped=mode==0&&tagged;
    check(reads==(dropped?0u:1u)&&writes==reads,"queued Frame event discarded after mode0 commit");
    check(c.load(M+20,4)==unsigned(mode&&!failure),"MAG sequence only after successful read/publication");
    check(c.load(M+13,1)==0,"busy released");
    if(mode&&failure)check(c.load(0x20001b96,1)==0&&c.load(0x20001b98,4)==0,"I2C failure does not publish zero/stale vector");
    if(mode&&!failure) {
      check(c.load(0x20001b96,1)==1&&c.load(0x20001b98,2)==0x8081,"successful stock conversion/publication");
      c.stops.clear();c.r[0]=0x20003200;c.r[1]=8;c.r[14]=0xdead0001;
      c.store(0x20001ff8,1,1);c.store(0x20001080,2,1);c.store(0x20000124,0x419e2+176,4);
      check(c.run(e.symbols.at("alpha_snapshot")),"new diagnostics");
      check(c.load(0x20003200,1)==1&&c.load(0x20003201,1)==1&&c.load(0x20003202,4)==0x419e2+176,"strap IR enable full width selector");
      c.r[0]=0x20003200;c.r[1]=9;c.r[14]=0xdead0001;
      check(c.run(e.symbols.at("alpha_snapshot"))&&c.load(0x20003200,4)==1,"sequence page");
    }
  }
  for(unsigned mode:{0u,2u,255u})for(unsigned site:{0xb3f0u,0xda18u}) {
    Cpu p(image),s(stock());
    for(Cpu *c:{&p,&s}) {
      c->store(0x20005150,mode,1);c->stops={site+4};
      c->calls[0xd768]=[](Cpu &x){x.r[0]=0;};
      c->calls[0xb420]=[](Cpu &x){x.r[0]=77;};
    }
    check(p.run(site)&&s.run(site),"mode0 new hook continuation");
    check(p.r==s.r&&p.apsr==s.apsr&&p.primask==s.primask&&p.stores==s.stores,"mode0 read/event exact registers flags MMIO");
    check(std::equal(p.ram.begin(),p.ram.begin()+0x5d00,s.ram.begin()),"mode0 read/event no persistent RAM effects");
  }
  { // Actual callback -> D890 -> queue copy -> DA02/DA18 -> B3D8 -> 9740.
    Cpu c(image);c.store(0x20005150,1,1);c.store(0x20001ff6,1,1);
    c.store(0x20002044,1,1);c.store(M+16,0x4d414731,4);
    c.store(0x200003d8,1,1);c.store(0x20001fbc,0x20004000,4);
    c.store(0x20004000,0xf2,1);c.store(0x20004001,2,1);c.store(0x20004018,0xb2b1,4);
    c.calls[0x11fe4]=[](Cpu &p){p.r[0]=1;};
    c.calls[0x11e90]=[](Cpu &p){check(p.r[2]==1,"stock event2 queue priority");
      for(unsigned j=0;j<16;++j)p.store(0x20003000+j,p.load(p.r[1]+j,1),1);p.r[0]=0;};
    unsigned reads=0;unsigned minsp=c.r[13];
    c.calls[0xd768]=[&](Cpu &p){check(p.ipsr==0&&p.primask==0,"worker I2C unmasked");
      ++reads;for(unsigned j=0;j<6;++j)p.store(p.r[2]+j,128+j+reads,1);p.r[0]=0;};
    c.calls[0xd6e4]=[](Cpu &p){p.r[0]=0;};
    c.calls[0x1490]=[](Cpu &){};c.calls[0x14b0]=[](Cpu &){};
    c.tick=[&](Cpu &p){minsp=std::min(minsp,p.r[13]);};
    unsigned sp=c.r[13];
    for(unsigned n=1;n<=900;++n) {
      c.stops.clear();c.r[14]=0xdead0001;
      check(c.run(0xb2b0),"real tagged callback queue");
      check(c.load(0x20003000,1)==2&&c.load(0x20003004,4)==0x4131334d,"normal event2 private origin metadata");
      c.r[4]=0x20003000;c.r[7]=0x20003300;c.r[14]=0xdead0001;c.stops={0xd9de};
      c.store(0x200005d0,n*10,4);
      check(c.run(0xda02),"real worker MAG event path");
      check(c.load(M+20,4)==n&&c.load(0x20001b96,1)==(n&1),"sequence after alternating publication");
      check(c.r[13]==sp,"no event stack growth");
      c.r[0]=0x20003200;c.r[1]=6;c.r[14]=0xdead0001;c.stops.clear();
      check(c.run(0x9778),"stock tail selects completed buffer");
      for(unsigned j=0;j<3;++j)
        check(c.load(0x20003200+j*2,2)==(((128+j*2+n)&255)<<8|((129+j*2+n)&255)),"tail equals newest measured half");
    }
    check(reads==900&&sp-minsp<256,"bounded stack for 900 measurements, no extra allocations");
    // Queued Frame event + actual mode boundary: it must no longer read I2C.
    c.r[14]=0xdead0001;check(c.run(0xb2b0),"queue before transition");
    seed(c);c.store(0x20005151,1,1);c.stops={0x6c9c};
    check(c.run(0x6c98)&&c.load(0x20005150,1)==0,"actual mode0 commit with queued event");
    c.r[4]=0x20003000;c.r[7]=0x20003300;c.r[14]=0xdead0001;c.stops={0xd9de};
    check(c.run(0xda02)&&reads==900&&c.load(M+20,4)==900,"no Frame work after mode0 commits");
  }
}
