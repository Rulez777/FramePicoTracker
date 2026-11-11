#define FWVERIFY_LIBRARY
#include "verify.cpp"
void prepare_imu(Cpu &c, unsigned latest=20) {
  c.store(0x20005150,1,1); c.store(0x20001082,1,1);
  c.store(0x20001afc,latest,1);
  for(unsigned age=0;age<8;++age) {
    unsigned counter=(latest-age)&255,slot=counter&7;
    c.store(0x20001afd+slot,1,1);
    for(unsigned j=0;j<18;++j) c.store(0x20001afc+9+18*slot+j,counter+j,1);
    c.store(0x20001afc+9+18*slot+17,counter,1);
  }
  for(unsigned j=0;j<12;++j)c.store(0x20001b98+j,0x80+j,1);
  c.r[0]=0x20000fd0; c.r[1]=0x20003000;c.r[2]=7;c.r[3]=0x20003200;
  c.store(c.r[13],0x20003400,4); c.store(c.r[13]+4,0x20003401,4);
  c.store(0x20003400,0,1);c.store(0x20003401,54,1);
  // Valid command-11 ACK, then len FD with a nonzero opcode.
  const unsigned bytes[]={0,11,1,12,0,31,253};
  for(unsigned j=0;j<7;++j)c.store(0x20003000+j,bytes[j],1);
}
#include "alpha11_cases.h"
#include "alpha12_cases.h"
#include "alpha13_cases.h"
int main(int argc,char **argv) {
 try {
  auto image=read(argc>1?argv[1]:"outputs/frame-alpha1/image.bin"); auto e=elf(argc>2?argv[2]:"outputs/frame-alpha1.elf");
  if(e.symbols.count("alpha_mag_reconcile")) alpha12_tests(image,e);
  if(e.symbols.count("alpha13_timer_gate")) alpha13_tests(image,e);
  alpha11_tests(image);
  command_tests(image,e,false,12);
  for(auto sym:e.symbols)if(sym.first.find("select_")==0) {
    unsigned site=std::stoul(sym.first.substr(7),nullptr,16);
    for(unsigned mode:{0u,1u,2u,255u})for(unsigned flags:{0u,0xf8000000u}) {
      Cpu c(image),a(read("outputs/frame-release-a/image.bin"));
      for(Cpu *p:{&c,&a}) {
        p->store(0x20005150,mode,1);p->apsr=flags;p->stops={site+4};
        p->r[3]=site==0x6cc4?0x6d48:site==0x6d36?0x6d50:site==0x6f64?0x6f80:0x6d40;
      }
      check(c.run(site)&&a.run(site),"inherited selector execution");
      check(c.r==a.r && c.apsr==a.apsr,"all A timing selector behavior retained");
    }
  }
  for(unsigned pm:{0u,1u}) {
    Cpu c(image),s(stock());seed(c);seed(s);c.primask=s.primask=pm;
    c.stops={0x6c9c};
    check(c.run(0x6c98)&&s.run(0x57ec),"mode0 unrequested halt uses stock");
    for(unsigned r=0;r<13;++r)check(c.r[r]==s.r[r],"mode0 boundary stock registers");
    check(c.apsr==s.apsr && c.primask==s.primask && c.stores==s.stores,"mode0 boundary stock flags and MMIO");
  }
  for(unsigned length=0;length<=65;++length) {
    Cpu c(image);c.r[7]=0x20000abd;c.r[6]=0x20000fd0;
    c.store(c.r[7],length,1);c.stops={0x5a64};
    check(c.run(0x5a50),"stock reply room arithmetic executed");
    check(c.load(0x20000fe5,1)==std::min(65-length,54u),"min(41h-header,36h) room");
  }
  for(bool patched:{false,true}) {
    Cpu c(patched?image:stock());prepare_imu(c);
    c.r[0]=0x20000fd0;
    auto initial=c.r;
    check(c.run(0x9458,3000)==patched,"stock IMU FD hang reproduced / Alpha exits");
    Cpu d(patched?image:stock());prepare_imu(d);d.r[0]=0x20000fd0;
    d.r[1]=initial[1];d.r[2]=initial[2];
    check(d.run(0x94fc,3000)==patched,"stock dispatcher FD hang reproduced / Alpha exits");
  }
  // Every header size and cursor: malformed records terminate before dereference.
  for(unsigned cursor:{0u,1u,4u,250u,252u,254u})
   for(unsigned remaining=0;remaining<=255-cursor;++remaining)
    for(unsigned n:{0u,1u,18u,252u,253u,254u,255u}) {
     Cpu c(image);c.store(0x20005150,1,1);
     c.r[6]=cursor+remaining;c.r[7]=0x20003000;c.r[9]=cursor;
     c.store(0x20003000+cursor+2,n,1);
     c.stops={0x951a,0x9512};
     check(c.run(0x9516),"dispatcher guard bounded");
     bool valid=remaining>=3 && n<253 && n+3<=remaining && cursor+n+3<=255;
     check(c.r[15]==(valid?0x951au:0x9512u),"dispatcher exact bounds");
    }
  // Entry gates reach original mode-0 instructions with identical machine state.
  for(unsigned mode:{0u,2u,255u})for(unsigned pm:{0u,1u})
   for(unsigned site:{0x9458u,0x97f4u,0x9516u}) {
    Cpu c(image),s(stock());c.primask=s.primask=pm;
    c.store(0x20005150,mode,1);s.store(0x20005150,mode,1);
    c.r[7]=s.r[7]=0x20003000;c.r[9]=s.r[9]=4;
    c.stops={site+4};s.stops=c.stops;
    check(c.run(site)&&s.run(site),"mode-0 original continuation");
    check(c.r==s.r && c.apsr==s.apsr && c.primask==s.primask,"mode-0 stock registers/flags/PRIMASK");
    check(c.stores==s.stores,"mode-0 stock MMIO effects");
   }
  for(unsigned pm:{0u,1u})for(unsigned mag:{0u,1u}) {
    Cpu c(image);prepare_imu(c);c.primask=pm;c.store(0x20001081,mag,1);
    check(c.run(0x9458),"malformed IMU walker returns");
    check(c.primask==pm,"IMU PRIMASK");
    check(c.load(0x20003201,1)==2,"IMU record opcode");
    check(c.load(0x20003202,1)==(mag?51u:41u),"compact full payload");
    check(c.load(0x20003203,1)==(mag?0xcbu:0xceu),"compact marker");
    check(c.load(0x20003204,1)==32 && c.load(0x20003205,1)==33,"temperature always retained");
  }
  for(unsigned room:{35u,44u,54u})for(unsigned mag:{0u,1u}) {
    Cpu c(image);prepare_imu(c);c.store(0x20001081,mag,1);
    c.store(0x20003401,room,1);
    c.store(0x20000fd0+0x74,0x20003600,4);c.store(0x20003604,0x20003700,4);
    c.store(0x2000370e,1,1);c.store(0x2000370f,32,1);
    check(c.run(0x9458),"queued sensor status coexistence");
    check(c.load(0x20003401,1)>=35,"control reply is not starved by full IMU ring");
    check(c.load(0x2000370f,1)==32,"queue is inspected read-only");
  }
  for(unsigned latest:{0u,1u,7u,20u,254u,255u})
   for(unsigned lag:{0u,1u,2u,7u,8u,127u,128u,255u})
    for(unsigned room=0;room<=54;++room)for(unsigned turn:{0u,1u}) {
     Cpu c(image);prepare_imu(c,latest);
     unsigned ack=(latest-lag)&255;
     c.store(0x20005158,ack,1);c.store(0x20005159,1,1);
     c.store(0x20003001,11,1);c.store(0x20003003,ack,1);
     c.store(0x20001081,turn,1);c.store(0x20003401,room,1);
     auto before=c.ram;
     check(c.run(0x9458),"capacity/ack/wrap IMU bounded");
     unsigned total=c.load(0x20003400,1),mag=turn^1;
     if(room<3+(mag?9u:3u)) {
       check(total==0 && c.load(0x20001081,1)==turn,"small capacity preserves mag turn");
       continue;
     }
     unsigned size=c.load(0x20003202,1),marker=c.load(0x20003203,1),count=marker&3;
     check(total==size+3 && total<=room && size==3+16*count+6*mag,"reply exact capacity equation");
     check(marker==(0xc8|count|(mag?4:0)),"version/count/mag marker");
     std::set<unsigned> sent;
     unsigned last_age=9;
     for(unsigned j=0;j<count;++j) {
       unsigned pos=0x20003206+16*j,counter=c.load(pos+15,1),age=(latest-counter)&255;
       check(age<8 && age<last_age && (lag>=128 || (((counter-ack)&255)>0 && ((counter-ack)&255)<128)),"unique ascending post-ack counters");
       last_age=age;sent.insert(counter&7);
       for(unsigned k=0;k<16;++k) {
         unsigned source=0x20001afc+9+18*(counter&7)+(k<12?k:k+2);
         check(c.load(pos+k,1)==before[source-0x20000000],"gyro accel timestamp counter lossless");
       }
     }
     for(unsigned slot=0;slot<8;++slot)
       check(c.load(0x20001afd+slot,1)==(sent.count(slot)?0u:1u),"only transmitted flags cleared");
     if(mag)for(unsigned k=0;k<6;++k)
       check(c.load(0x20003206+16*count+k,1)==0x86+k,"all magnetometer axes retained");
     unsigned expected=0;
     for(unsigned age=0;age<8;++age){unsigned counter=(latest-age)&255,diff=(counter-ack)&255;if(lag>=128 || (diff>0 && diff<128))++expected;}
     unsigned capacity=(room-3-3-6*mag)/16;
     check(count==std::min(expected,capacity),"maximal eligible unique sample count");
    }
  {
    Cpu c(image);prepare_imu(c);
    std::set<unsigned> seen;
    for(unsigned reply=0;reply<6;++reply) {
      c.r[0]=0x20000fd0;c.r[1]=0x20003000;c.r[2]=7;c.r[3]=0x20003200;c.r[14]=0xdead0001;
      c.store(0x20003400,0,1);c.store(0x20003401,54,1);
      check(c.run(0x9458),"repeated reply terminates");
      unsigned n=c.load(0x20003203,1)&3;
      for(unsigned j=0;j<n;++j)check(seen.insert(c.load(0x20003215+16*j,1)).second,"no flags resent across replies");
      if(reply>=4)check(n==0,"exhausted ring has no old samples");
    }
    check(seen.size()==8,"all eight ring samples drained exactly once");
  }
  // Schema 3 is Frame-only and copies only existing sensor RAM.
  for(unsigned mode:{0u,1u,2u,255u})for(unsigned pm:{0u,1u})for(unsigned page=0;page<(e.symbols.count("alpha13_timer_gate")?11u:9u);++page) {
    Cpu c(image);seed(c);c.primask=pm;c.store(0x20005150,mode,1);
    for(unsigned a=0x20000000;a<0x20005000;++a)c.store(a,(a*17+7)&255,1);
    c.r[0]=0x20003000;c.r[1]=19;
    c.store(c.r[0],0x7e,1);c.store(c.r[0]+1,3,1);c.store(c.r[0]+18,page,1);
    c.stops={0x8d1a};auto before=c.ram;
    check(c.run(0x8d16),"sensor query terminates");
    check(c.primask==pm,"snapshot PRIMASK restored");
    if(mode==1 && page<(e.symbols.count("alpha13_timer_gate")?10u:8u)) {
      check(c.reply.size()==32 && c.reply[23]==12 && c.reply[24]==page && c.reply[25]==6,"sensor schema header");
      const unsigned addresses[]={0x20001faf,0x20001fa0,0x20000442,0x20001fb0,0,0,0,0x20001f87,
       0x20000fb2,0x20000fb3,0x20000138,0x20000139,0x2000013a,0x2000013b,0x2000013c,0x2000013d,
       0x20001fc0,0x20001fc1,0x20001fc2,0x20001fc3,0x20001fba,0x20000fa8,0,0x20000445,
       0x20000ef0,0x20000ef1,0x20000ef2,0x20000ef3,0x20000f6a,0x20000f6b,0x20000f6c,0x20000f6d,
       0x20000ffe,0x20005153,0x20005154,0x20005155,0x20005156,0x20005157,0x20005151,0x20000f21};
      for(unsigned j=0;j<6;++j){unsigned k=page*6+j;if(k<40 && addresses[k])
       check(c.reply[26+j]==before[addresses[k]-0x20000000],"exact sensor snapshot byte");}
      check(c.stores.empty(),"snapshot no MMIO writes");
    }else check(c.reply.empty() && c.r[15]==0x8d1a,"schema3 falls back outside Frame or invalid page");
    check(std::equal(before.begin(),before.begin()+0x5000,c.ram.begin()),"sensor RAM read-only");
  }
  for(unsigned pm:{0u,1u})for(unsigned arrival:{0u,100u,999999u})for(unsigned mode:{0u,1u}) {
    Cpu c(image);seed(c);c.primask=pm;c.store(0x20005150,mode,1);
    c.store(0x20005151,mode?1:2,1);c.store(0x20005152,3,1);
    c.disabled_arrives=false;c.mmio[0x40001550]=3;
    c.mmio[0x40001200]=3;c.mmio[0x40001304]=0x18;
    c.store(0x20000e04,0x50e9,4);
    c.mmio[0xe000e200]=0x100; // pending TIMER IRQ
    bool disable_seen=false,irq_fired=false,pending=true;
    c.tick=[&](Cpu &x){
      if(x.mmio[0x40001010]==1)disable_seen=true;
      if(disable_seen && x.steps>=arrival){x.mmio[0x40001110]=1;x.mmio[0x40001550]=0;}
      if(x.mmio[0xe000e280]&0x100)pending=false;
      if(disable_seen && !x.primask && pending) {
        check(x.load(0x20005153,1)==1,"pending TIMER IRQ only runs after recorded timeout");
        irq_fired=true;pending=false;
      }
      if(x.load(0x20005150,1)!=mode)
        check(x.mmio[0x40001550]==0,"no active RADIO mode commit");
    };
    c.stops.insert(0x6c9c);
    check(c.run(0x6c98,40000),"bounded halt always returns");
    check(c.primask==pm,"halt restores incoming PRIMASK");
    if(arrival<999999) {
      check(c.load(0x20005150,1)==(mode?0u:1u),"mode committed after disabled");
      check(c.load(0x20000f21,1)==3,"new N committed");
      check(!irq_fired && !pending,"pending TIMER cleared before PRIMASK restored");
    }else{
      check(c.load(0x20005150,1)==mode && c.load(0x20000f21,1)==1,"timeout retains mode and N");
      check(c.load(0x20005153,1)==1 && c.load(0x20005154,4)==1,"timeout recorded");
      check(c.load(0x200008c8,1)==0 && c.load(0x20000e04,4)==0,"timeout scheduler idle");
      check(c.mmio[0x40001200]==0 && c.mmio[0x40001304]==0,"timeout SHORTS INTEN cleared");
      check(irq_fired==(pm==0),"timeout retains pending TIMER for stock scheduler");
    }
  }
  for(unsigned pm:{0u,1u}) {
    Cpu c(image);seed(c);c.primask=pm;
    for(unsigned attempt=0;attempt<5;++attempt) {
      c.r[0]=0x20003000;c.r[1]=24;c.r[14]=0xdead0001;
      c.store(c.r[0],0x7d,1);c.store(c.r[0]+1,2,1);
      unsigned requested=attempt<3?1:0;
      c.store(c.r[0]+22,requested,1);c.store(c.r[0]+23,3,1);
      for(unsigned j=0;j<4;++j){c.store(0x20000efe + j,0xa0+j,1);c.store(c.r[0]+18+j,0xa0+j,1);}
      check(c.run(0x8d16),"repeated 7D setter returns");
      check(c.reply[25]==12 && c.primask==pm,"repeated setter revision/PRIMASK");
      bool same=attempt==2 || attempt==4;
      check(c.reply[24]==(same?0u:1u),"repeated 7D same/staged status");
      if(same)continue;
      c.disabled_arrives=attempt!=0;c.mmio[0x40001550]=0;
      c.store(0x200008c8,4,1);c.stops={0x6c9c};c.r[14]=0xdead0001;
      check(c.run(0x6c98,40000),"repeated transition bounded");
      check(c.load(0x20005150,1)==(attempt==0?0u:requested),"repeat after timeout commits only successful request");
      check(c.load(0x20005154,4)==1,"failure count preserved across repeated requests");
      c.stops.clear();
    }
  }
  // DISABLED event alone cannot authorize a commit while STATE remains active.
  for(unsigned pm:{0u,1u}) {
    Cpu c(image);seed(c);c.primask=pm;c.store(0x20005151,2,1);c.store(0x20005152,3,1);
    c.mmio[0x40001550]=9;c.stops={0x6c9c};
    check(c.run(0x6c98,40000),"active RADIO bounded despite DISABLED event");
    check(c.load(0x20005150,1)==0 && c.load(0x20000f21,1)==1,"active RADIO never commits");
  }
  for(unsigned pm:{0u,1u}) {
    Cpu c(image);seed(c);c.primask=pm;
    c.store(0x20005150,1,1);c.store(0x20005151,2,1);c.store(0x20005152,5,1);
    c.store(0x20005158,77,1);c.store(0x20005159,1,1);c.stops={0x6c9c};
    check(c.run(0x6c98),"Frame N change returns");
    check(c.load(0x20000f21,1)==5 && c.load(0x20005158,1)==77 && c.load(0x20005159,1)==1,"N-only transition retains ACK history");
  }
  for(unsigned previous:{0u,127u,255u})for(unsigned delta=0;delta<256;++delta)for(unsigned pm:{0u,1u}) {
    Cpu c(image);prepare_imu(c);c.primask=pm;
    c.store(0x20005158,previous,1);c.store(0x20005159,1,1);
    c.r[0]=(previous+delta)&255;
    check(c.run(0x97f4),"command11 Frame gate returns");
    unsigned ack=(previous+delta)&255;
    bool fresh=((20-previous)&255)>=128 && ((20-ack)&255)<8;
    check(c.load(0x20005158,1)==(delta<128 || fresh?ack:previous),"ACK monotonic or retained-ring resynchronization");
    check(c.primask==pm,"ACK preserves PRIMASK");
    for(unsigned slot=0;slot<8;++slot)check(c.load(0x20001afd+slot,1)==1,"ACK does not fabricate flags");
  }
  for(unsigned cfg:{7u,8u,15u,0x17u,0x80u,0x81u})for(unsigned variant:{0u,1u,2u}) {
    Cpu c(image);c.r[0]=0x20003000;c.r[1]=0;
    c.store(0x20000442,cfg,1);c.store(0x20001faf,variant,1);c.store(0x20001fb0,1,1);
    check(c.run(e.symbols.at("alpha_snapshot")),"ODR snapshot returns");
    check(c.load(0x20003004,1)==unsigned(cfg<128 && variant<=1),"ordinary active config validity");
    check(c.load(0x20003005,1)==(variant<=1?cfg&15:0u),"gyro ODR code follows exact stock mask");
    c.r[0]=0x20003000;c.r[1]=1;c.r[14]=0xdead0001;
    check(c.run(e.symbols.at("alpha_snapshot")) && c.load(0x20003000,1)==(variant<=1?cfg&15:0u),"accel ODR code follows exact stock mask");
  }
  // A's boot/button and timing sites are inherited; no C2/reset/OTA sites.
  auto a=read("outputs/frame-release-a/image.bin"),s=stock();
  for(unsigned va=0x1000;va<0x1000+s.size();++va) {
    bool baseline_patch=false;
    for(auto p:diagnostic_sites)if(va>=p.va && va<p.va+hex(p.before).size())baseline_patch=true;
    bool added=(va>=0x9458 && va<0x945c)||(va>=0x9516 && va<0x951a)||(va>=0x97f4 && va<0x97f8)||
      (e.symbols.count("alpha13_timer_gate") && ((va>=0xb2b0 && va<0xb2b4)||(va>=0xda18 && va<0xda1c)||(va>=0xb3f0 && va<0xb3f4)))||
      (e.symbols.count("alpha_mag_reconcile") && ((va>=0xda02 && va<0xda06)||(va>=0xb448 && va<0xb44c)));
    if(!baseline_patch && !added)check(image[va-0x1000]==s[va-0x1000],"protected stock byte equality");
  }
  check(std::equal(a.begin(),a.begin()+0x200,image.begin()),"vectors equal release A and stock");
  for(unsigned va:{0x126au,0x532cu,0x581cu,0x8d58u,0x7b8eu,0x7be2u,0x65c0u,0x785au})
    check(std::equal(s.begin()+va-0x1000,s.begin()+va-0x1000+4,image.begin()+va-0x1000),"no boot/reset/observer/delay hooks");
  std::cout<<"PASS Alpha 1 checks "<<cases<<"\n";
 }catch(const std::exception &x){std::cerr<<"FAIL after "<<cases<<": "<<x.what()<<"\n";return 1;}
}
