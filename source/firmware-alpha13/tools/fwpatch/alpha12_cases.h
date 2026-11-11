// CPU executes actual lifecycle, product check, timer startup and MAG worker.
// I2C and RTX allocation/scheduling are explicitly modeled peripherals.
void alpha12_tests(const Bytes &image,const Elf &e) {
  constexpr unsigned M=0x20005160,H=0x20004000;
  const bool a13=e.symbols.count("alpha13_timer_gate")!=0;
  unsigned allocations=0,frees=0,starts=0,reads=0,product=0x10;
  auto setup=[&](Cpu &c) {
    c.store(0x200003e0,0xbd11,4);c.store(0x200003e4,0xbd1b,4);
    c.store(0x20000442,7,1);c.store(0x20001fba,98,1);
    c.r[4]=0x20003000;c.stops={0xda06};
    c.calls[0xd768]=[&](Cpu &p){
      if(p.r[1]==0x39)p.store(p.r[2],product,1);
      else for(unsigned j=0;j<p.r[3];++j)p.store(p.r[2]+j,128+j+reads,1);
      ++reads;
    };
    c.calls[0xd6e4]=[](Cpu &){};
    c.calls[0xb318]=[](Cpu &){}; // Analog self-test thresholds, unchanged stock.
    c.calls[0xb2c4]=[](Cpu &){};
    c.calls[0xb398]=[](Cpu &){};
    c.calls[0x10c6c]=[](Cpu &){};
    c.calls[0x1490]=[](Cpu &){};c.calls[0x14b0]=[](Cpu &){};
    c.calls[0x13204]=[&](Cpu &p){
      check(p.primask==0 && p.ipsr==0,"timer allocation is foreground unmasked");
      check(p.r[0]==0xb2b1 && p.r[1]==1,"stock periodic MAG callback");
      ++allocations;p.store(H,0xf2,1);p.store(H+1,1,1);p.store(H+24,p.r[0],4);p.r[0]=H;
    };
    c.calls[0x13244]=[&](Cpu &p){check(p.r[0]==H && p.r[1]==10,"stock 10 tick MAG start");
      ++starts;p.store(H+1,2,1);p.r[0]=0;};
    c.calls[0x132dc]=[&](Cpu &p){check(p.r[0]==H && p.load(H,1)==0xf2,"free valid timer only");
      ++frees;p.store(H,0,2);p.r[0]=0;};
  };
  auto wake=[&](Cpu &c,unsigned tick) {
    c.store(0x200005d0,tick,4);c.r[14]=0xdead0001;
    check(c.run(0xda02),"actual worker queue continuation");
  };
  {
    Cpu old(read("outputs/alpha11-baseline/image.bin"));setup(old);old.store(0x20005150,1,1);
    wake(old,0);wake(old,1000);wake(old,10000);
    check(old.load(0x20001fba,1)==98 && allocations==0,"frozen Alpha1.1 reproduces stopped MAG/no reconcile");
  }
  for(unsigned result:{0x10u,0u})for(unsigned mode:{0u,2u,255u}) {
    Cpu p(image),s(stock());setup(p);setup(s);product=result;
    p.store(0x20005150,mode,1);s.store(0x20005150,mode,1);
    p.stops=s.stops={0xb44c};
    check(p.run(0xb448)&&s.run(0xb448),"mode0 product check stock path");
    for(unsigned j=0;j<16;++j)check(p.r[j]==s.r[j],"mode0 product register "+std::to_string(j)+" "+std::to_string(p.r[j])+" vs "+std::to_string(s.r[j]));
    check(p.apsr==s.apsr && p.primask==s.primask && p.stores==s.stores,"mode0 product check identical state");
    check(p.load(M+12,4)==0,"mode0 product diagnostic inactive");
  }
  product=0x10;
  for(unsigned h:{0u,1u,0x1000u,0x20005ff0u,H}) {
    Cpu p(image);p.store(0x20001fbc,h,4);p.r[14]=0xdead0001;
    check(p.run(e.symbols.at("alpha_mag_timer_flags")),"timer validity diagnostic bounded for null/invalid/stale pointer");
    check(p.r[0]==(h?1u:0u),"invalid timer is never reported running");
  }
  if(a13) {
    unsigned saved_alloc=allocations,saved_free=frees,saved_start=starts;
    for(unsigned handle:{0u,1u,0x1000u,0x20005ff0u,H}) {
      Cpu p(image);setup(p);p.store(0x20005150,1,1);p.store(0x20001ff6,1,1);
      p.store(0x200003d8,1,1);p.store(0x20001fbc,handle,4);
      p.store(M+16,0x4d414731,4);p.store(H,0xf2,1);p.store(H+24,0xdead0001,4);
      unsigned before_alloc=allocations,before_free=frees;
      wake(p,1000);
      check(allocations==before_alloc+1&&frees==before_free,"null/deleted/foreign timer repaired without invalid destructor");
      check(p.load(0x20001ff6,1)==1&&p.load(0x20001fbc,4)==H,"timer repair preserves independent");
    }
    allocations=saved_alloc;frees=saved_free;starts=saved_start;
  }
  {
    Cpu p(image);seed(p);p.store(0x20005150,1,1);p.store(0x20005151,1,1);
    p.store(M+16,0x4d414731,4);p.store(M+13,1,1);p.stops={0x6c9c};
    check(p.run(0x6c98),"busy worker boundary returns and stock tail may rearm");
    check(p.load(0x20005150,1)==1 && p.load(0x20005151,1)==1,"mode commit defers during MAG lifecycle");
    p.store(M+13,0,1);p.r[14]=0xdead0001;seed(p); // next stock receive cycle re-armed
    check(p.run(0x6c98) && p.load(0x20005150,1)==0,"pending mode0 commits after worker releases");
  }
  {
    Cpu p(image);seed(p);p.store(0x20005151,2,1);p.store(M+16,0x4d414731,4);p.store(M+13,1,1);
    p.stops={0x6c9c};check(p.run(0x6c98) && p.load(0x20005150,1)==1,"stale SRAM busy cannot block mode0 to Frame entry");
    p.stops.clear();p.r[14]=0xdead0001;
    check(p.run(e.symbols.at("alpha_mag_reconcile")) && p.load(M+13,1)==0,"serial worker clears interrupted-runtime busy before early exits");
  }
  for(unsigned mode:{0u,2u,255u})for(unsigned pm:{0u,1u}) {
    Cpu c(image),old(read("outputs/alpha11-baseline/image.bin"));setup(c);setup(old);
    c.store(0x20005150,mode,1);old.store(0x20005150,mode,1);c.primask=old.primask=pm;
    auto before=c.ram;wake(c,1000);wake(old,1000);
    check(c.r==old.r && c.apsr==old.apsr && c.primask==old.primask && c.stores==old.stores,"mode0 worker identical registers/flags/MMIO");
    check(std::equal(c.ram.begin(),c.ram.begin()+0x5d00,old.ram.begin()),"mode0 no runtime RAM changes except transient stack");
    (void)before;
  }
  for(unsigned pm:{0u,1u})for(unsigned irq:{0u,16u})if(pm||irq) {
    Cpu c(image);setup(c);c.store(0x20005150,1,1);c.primask=pm;c.ipsr=irq;
    wake(c,2000);check(c.load(M+16,4)==0,"reconcile never runs masked or in ISR");
  }
  Cpu c(image);setup(c);c.store(0x20005150,1,1);
  wake(c,0);
  for(unsigned t=1;t<1000;++t)wake(c,t);
  check(allocations==0,"no fast retry before interval");
  wake(c,1000);
  check(allocations==1 && starts==1 && c.load(M+4,4)==1 && c.load(M+8,4)==1,"first stock lifecycle open succeeds exactly once");
  check(c.load(M+12,1)==1 && c.load(0x200003d8,1)==1,"product check captured and lifecycle active");
  unsigned sp=c.r[13];
  for(unsigned t=1001;t<=10000;++t) {
    wake(c,t);
    if(t%10==0) {
      c.stops.clear();c.r[14]=0xdead0001;check(c.run(0xb420),"stock MAG worker measures");c.stops={0xda06};
    }
    check(c.r[13]==sp,"no stack growth across worker cycles");
  }
  check(c.load(0x20001fba,1)==((98+900)&255),"real MAG worker counter advances at modeled 100Hz");
  check(allocations==1 && frees==0 && starts==1,"healthy timer not recreated over time");
  check(c.load(0x20000442,1)==7,"IMU code7 unchanged");
  // Actual stock standby profile closes only MAG in this fixture.
  c.stops.clear();c.r[14]=0xdead0001;check(c.run(0xa5e8),"stock standby device profile executes");c.stops={0xda06};
  check(c.load(0x200003d8,1)==0 && frees==1,"stock profile closes/free MAG via owner");
  wake(c,11000);
  check(allocations==2 && frees==1 && starts==2 && c.load(M+8,4)==2,"reconcile reopens after stock close with one live allocation");
  c.store(H+1,1,1);wake(c,12000);
  check(allocations==3 && frees==2 && starts==3,"stopped timer repaired through close/open owner");
  c.store(0x20001ff6,1,1);c.store(H+1,1,1);wake(c,13000);
  check(c.load(0x20001ff6,1)==1 && allocations==(a13?4u:3u),"independent preserved; Alpha13 reconciles with gate set");
  c.calls[0x14a82]=[](Cpu &p){for(unsigned j=0;j<p.r[2];++j)p.store(p.r[0]+j,p.r[1],1);};
  unsigned counter=c.load(0x20001fba,1);
  c.calls[0xd890]=[](Cpu &p){check(p.r[0]==2,"MAG event 2 queued");};
  c.stops.clear();c.r[14]=0xdead0001;check(c.run(0xb2b0),"actual gated stock timer callback executes");c.stops={0xda06};
  check(c.load(0x20001fba,1)==counter && (a13 || c.load(0x20001b98,4)==0),"gate clears tail through stock callback without measuring");
  c.store(0x20001ff6,0,1);wake(c,14000);
  check(allocations==4 && frees==3,"gate release permits next bounded repair");
  c.stops.clear();c.r[14]=0xdead0001;check(c.run(0xa75c),"stock close before negative product fixture");c.stops={0xda06};
  product=0;unsigned n=allocations,attempt=c.load(M+4,4),success=c.load(M+8,4);
  for(unsigned t=14001;t<=17000;++t)wake(c,t);
  check(allocations==n && c.load(M+4,4)==attempt+3 && c.load(M+8,4)==success,"product failure bounded to one attempt per 1000 ticks, no allocation");
  check(c.load(M+12,1)==255 && c.load(0x20001fbc,4)==0,"product failure diagnosed, stale pointer cleared");
  product=0x10;wake(c,18000);check(allocations==n+1,"product recovery resumes");
  c.store(M,0xffffff00u,4);wake(c,0x2e7);check(c.load(M,4)==0xffffff00u,"unsigned tick wrap rate limit before 1000");
  wake(c,0x2e8);check(c.load(M,4)==0x2e8,"unsigned tick wrap reaches interval");
  for(unsigned page:{6u,7u}) {
    c.stops.clear();c.r[0]=0x20003200;c.r[1]=page;c.r[14]=0xdead0001;
    check(c.run(e.symbols.at("alpha_snapshot")),"extended snapshot executes");
    if(page==6)check(c.load(0x20003204,1)==1 && c.load(0x20003205,1)==0,"MAG state/gate spare fields");
    else check(c.load(0x20003200,1)==7 && c.load(0x20003203,1)==1 && c.load(0x20003204,1)==c.load(M+4,1),"timer/product/attempt fields");
  }
}
