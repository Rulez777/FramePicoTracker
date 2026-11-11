// Alpha 1 patch 1.1 regressions. The original Alpha 1 image is a frozen negative.
void alpha11_tests(const Bytes &image) {
  auto old=read("outputs/alpha1-original/image.bin");
  check(sha256(old)=="a21d01f5f34e00d9ef6ce17cc43c64692aa39c79c110292a19ff0448c01b4ad0","frozen Alpha 1 negative image");
  for(unsigned pm:{0u,1u}) {
    Cpu c(image),a(read("outputs/frame-release-a/image.bin"));
    for(Cpu *p:{&c,&a}) {
      seed(*p);p->radio_model=true;p->primask=pm;p->store(0x20005150,1,1);
      p->mmio[0x40001200]=0x117;p->mmio[0x40001550]=3;p->mmio[0x40001304]=0x1b;
      p->stops={0x6c9c};
    }
    check(c.run(0x6c98) && a.run(0x6c98),"unrequested Frame boundary stock path");
    check(c.r==a.r && c.apsr==a.apsr && c.primask==a.primask && c.stores==a.stores && c.ram==a.ram,"unrequested Frame boundary exactly A stock effects");
  }
  for(bool fixed:{false,true}) {
    const auto &b=fixed?image:old;
    for(unsigned pending:{0u,2u})for(unsigned pm:{0u,1u}) {
      Cpu c(b);seed(c);c.radio_model=true;c.primask=pm;
      c.store(0x20005150,1,1);c.store(0x20005151,pending,1);c.store(0x20005152,3,1);
      c.mmio[0x40001200]=0x117;c.mmio[0x40001550]=3;c.mmio[0x40001304]=0x1b;
      c.rssi_running=true;c.stops={0x6c9c};
      check(c.run(0x6c98,50000),"stock SHORTS boundary bounded");
      check((c.shortcut_txen==0)==fixed,"old DISABLE triggers unsolicited TXEN; fixed does not");
      check(c.primask==pm,"real SHORTS boundary PRIMASK");
      if(fixed) {
        check(c.mmio[0x40001200]==0 && c.mmio[0x40001304]==0,"stock cleanup precedes DISABLE");
        check(c.load(0x200008c8,1)==0,"scheduler idle after boundary");
        check(c.load(0x20005154,4)==0,"no false halt timeout");
      } else check(c.load(0x20005154,4)==1 && c.shortcut_txen>1,"old automatic TX loop times out");
    }
    // A half-cycle-old ACK must not freeze a fresh retained ring.
    for(unsigned gap:{128u,129u,192u,255u}) {
      Cpu c(b);prepare_imu(c,20);c.r[2]=0;
      c.store(0x20005158,(20-gap)&255,1);c.store(0x20005159,1,1);
      c.store(0x20001081,1,1);
      for(unsigned slot=0;slot<8;++slot)c.store(0x20001afd+slot,slot==(20&7)?1:0,1);
      check(c.run(0x9458),"ACK gap regression returns");
      unsigned count=c.load(0x20003203,1)&3;
      check((count==1)==fixed,"old stale ACK freezes ring; fixed drains immediately");
    }
    // Two type-1 records in the same list, and a third in another priority.
    Cpu q(b);prepare_imu(q);q.r[2]=0;
    q.store(0x20000fd0+0x74,0x20003600,4);q.store(0x20003604,0x20003700,4);
    q.store(0x20003704,0x20003740,4);
    q.store(0x20000fd0+0x78,0x20003620,4);q.store(0x20003624,0x20003780,4);
    for(unsigned r:{0x20003700u,0x20003740u,0x20003780u}) {
      q.store(r+14,1,1);q.store(r+15,8,1);
    }
    auto before=q.ram;
    check(q.run(0x9458),"multi-record reservation returns");
    check((q.load(0x20003401,1)>=33)==fixed,"old first-only reservation starves queued records");
    check(std::equal(before.begin()+0x3600,before.begin()+0x3800,q.ram.begin()+0x3600),"reservation leaves queues unchanged");
  }
  // Exercise each implemented nRF52 shortcut independently as well as 0x117.
  for(unsigned shorts:{0u,1u,2u,4u,8u,0x10u,0x100u,0x117u}) {
    Cpu c(image);c.radio_model=true;c.mmio[0x40001200]=shorts;c.rssi_running=true;
    c.store(0x40001010,1,4);
    check(c.shortcut_txen==unsigned(bool(shorts&4)) && c.shortcut_rxen==unsigned(bool(shorts&8)),"DISABLED TXEN/RXEN semantics");
    check(c.rssi_running==!(shorts&0x100),"DISABLED RSSISTOP semantics");
    c.mmio[0x40001550]=9;c.radio_phase=1;c.radio_tick();
    check(c.radio_phase==((shorts&1)?2u:0u),"READY START semantics");
    c.rssi_running=false;c.radio_phase=2;c.radio_tick();
    check(c.rssi_running==bool(shorts&0x10),"ADDRESS RSSISTART semantics");
    c.mmio[0x40001110]=0;c.radio_phase=3;c.radio_tick();
    check(c.mmio[0x40001110]==unsigned(bool(shorts&2)),"END DISABLE semantics");
  }
  for(unsigned half:{0u,1u}) {
    Cpu c(image);prepare_imu(c);c.store(0x20001b96,half,1);
    check(c.run(0x9458),"both magnetometer buffers execute");
    unsigned count=c.load(0x20003203,1)&3;
    for(unsigned j=0;j<6;++j)
      check(c.load(0x20003206+16*count+j,1)==0x80+6*(1-half)+j,"selected double-buffer half preserved");
  }
  for(unsigned layout:{0u,1u,2u}) {
    Cpu c(image);prepare_imu(c);c.r[2]=0;
    c.store(0x20000fd0+0x74,0x20003600,4);c.store(0x20003604,0x20003700,4);
    c.store(0x20003704,layout==0?0x20003700:layout==1?0x20007000:0,4);
    c.store(0x2000370e,1,1);c.store(0x2000370f,layout==2?253:0,1);
    check(c.run(0x9458),"cyclic invalid or oversized queue reservation bounded");
    check(c.load(0x20003400,1)==0,"invalid queue leaves room for stock control handling");
  }
  // Execute the actual stock receive tail, including both float timing paths.
  // Only compiler soft-double library primitives are modeled here.
  for(bool timeout:{false,true})for(unsigned pm:{0u,1u})
   for(unsigned path:{0u,1u}) {
    Cpu c(image);seed(c);c.radio_model=true;c.primask=pm;
    c.disabled_arrives=!timeout;c.mmio[0x40001200]=0x117;c.mmio[0x40001550]=3;
    c.store(0x20005151,2,1);c.store(0x20005152,3,1);c.r[4]=0x20000ef5;
    c.stops={0x6c9c};
    check(c.run(0x6c98,50000),"receive boundary success/timeout returns");
    check(c.load(0x20005150,1)==(timeout?0u:1u),"tail sees committed mode only on success");
    check(c.primask==pm && c.shortcut_txen==0,"tail starts without unsolicited TX or masked IRQ leak");
    c.store(0x20000e08,path,1);c.store(0x20000858,1,1);
    auto get_double=[](Cpu &x,unsigned reg) {
      uint64_t bits=uint64_t(x.r[reg])|(uint64_t(x.r[reg+1])<<32);
      double value;std::memcpy(&value,&bits,8);return value;
    };
    auto put_double=[](Cpu &x,double value) {
      uint64_t bits;std::memcpy(&bits,&value,8);
      x.r[0]=uint32_t(bits);x.r[1]=uint32_t(bits>>32);
    };
    c.calls[0x13e40]=[=](Cpu &x){put_double(x,double(x.r[0]));};
    c.calls[0x13828]=[=](Cpu &x){put_double(x,get_double(x,0)*get_double(x,2));};
    c.calls[0x13508]=[=](Cpu &x){put_double(x,get_double(x,0)+get_double(x,2));};
    c.calls[0x13de8]=[=](Cpu &x){x.r[0]=uint32_t(get_double(x,0));};
    c.stops={0x641c};
    check(c.run(0x6c9c),"actual 6C9C receive tail reaches next-slot 641C");
    check(c.load(0x20000ef5,1)==1 && c.load(0x20000f75,1)==1,"stock receive tail markers committed");
    unsigned n=timeout?1:3;
    check(c.r[0]==unsigned(n*(timeout?4727.5:3000.0)+(timeout?(path?3538.0:3934.5):(path?1810.5:2207.0))),"next slot uses old timing after timeout / new after commit");
    // Scheduled RX phase ends at 6C74 -> 6C78 -> 6C7E -> 5730.
    c.stops={0x5730};c.calls[0x7574]=[](Cpu &){};
    for(unsigned j=0;j<7;++j)c.store(c.r[13]+20+4*j,j==6?0xdead0001:0,4);
    check(c.run(0x6c74),"actual stock scheduled RX phase tail reaches 5730");
    c.stops.clear();c.calls[0x84f4]=[](Cpu &){};c.calls[0x6164]=[](Cpu &){};
    c.calls[0xdead1000]=[](Cpu &){};c.store(0x20000e0c,0xdead1001,4);
    c.store(0x20000094,0x113,4);c.r[14]=0xdead0001;
    check(c.run(0x5730),"stock receive rearm executes after success and timeout");
    check(c.load(0x200008c8,1)==4 && c.mmio[0x40001200]==0x117 && c.mmio[0x40001004]==1,"stock scheduler re-arms state 4 SHORTS and RXEN");
   }
  // Empty ring policy is deliberate: retain temperature and alternate mag.
  for(unsigned turn:{0u,1u}) {
    Cpu c(image);prepare_imu(c);c.r[2]=0;c.store(0x20001081,turn,1);
    for(unsigned slot=0;slot<8;++slot)c.store(0x20001afd+slot,0,1);
    check(c.run(0x9458),"empty ring returns");
    check(c.load(0x20003202,1)==(turn?3u:9u),"empty ring retains temperature and due mag, no invented IMU");
  }
}
