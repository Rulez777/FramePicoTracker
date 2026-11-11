#include "frame_pair.hpp"
#include "imu_slot.hpp"
#include <iostream>
using namespace pico;
static unsigned checks=0;
static void check(bool b){++checks;if(!b)throw std::runtime_error("pair test "+std::to_string(checks));}
// Independent FW189 model: 5C2E FICR filter, 6932..6946 unpaired slot/seed,
// 6124 bytewise address sum, 6DD6 prefix, 8D88 targeted confirm -> 605C.
struct Tracker {
    uint32_t chip,seed=0;unsigned slot=0;bool paired=false,offer=false,pending=false;
    void receive_offer(const Bytes &dma){
        if(dma.size()!=23||dma[0]!=21||(dma[1]&8)||word(dma,19)!=chip)return;
        if(!paired){seed=word(dma,14);slot=dma[18];}offer=true;
    }
    bool listens(uint32_t base,unsigned prefix)const{
        uint32_t sum=0;for(unsigned byte=0;byte<4;++byte){unsigned mask=0xff;sum|=(((chip>>(8*byte))&mask)+((seed>>(8*byte))&mask))%256<<(8*byte);}
        return offer&&base==pair_reverse(sum)&&prefix==(pair_reverse(slot)>>24);
    }
    void confirm(const Bytes &body){if(offer&&body.size()==6&&body[0]==1&&word(body,2)==chip){pending=!paired;paired=true;}}
};
static Bytes offer(uint32_t chip,unsigned slot){Bytes p(23);p[0]=21;p[1]=2;p[2]=4;p[4]=0x7a;p[8]=0x55;p[9]=1;auto seed=le32(0x15547374),id=le32(chip);std::copy(seed.begin(),seed.end(),p.begin()+14);std::copy(id.begin(),id.end(),p.begin()+19);p[18]=uint8_t(slot);return p;}
static Record product_record(uint32_t chip,const std::string &serial){
    Bytes p(35);p[0]=0x82;p[1]=189;std::copy(serial.begin(),serial.end(),p.begin()+8);
    Bytes payload{0,1,35};payload.insert(payload.end(),p.begin(),p.end());unsigned sum=0;for(auto b:payload)sum+=b;payload.push_back(uint8_t(sum));
    Record r;r.chip=chip;r.hardware_crc=true;r.channel=2;r.profile=0;r.dma={uint8_t(payload.size()),2};r.dma.insert(r.dma.end(),payload.begin(),payload.end());return r;
}
int main(){try{
    const uint32_t chips[]={0xA1B2C3D4,0xA1B2C3E2,0xA1B2C3E1};
    for(unsigned host=0;host<3;++host)for(unsigned slot=0;slot<3;++slot)for(auto chip:chips){
        auto state=pair_state(chip,slot);auto prefix=word(state,1512);check(pico_offer_slot(prefix)==slot);
        Tracker old{chip};old.receive_offer(offer(chip,host));check(old.listens(word(state,1508),prefix)==(host==slot));
        Tracker fixed{chip};fixed.receive_offer(offer(chip,pico_offer_slot(prefix)));check(fixed.listens(word(state,1508),prefix));
        auto body=Bytes{1,0};auto id=le32(chip);body.insert(body.end(),id.begin(),id.end());fixed.confirm(body);check(fixed.paired&&fixed.pending);
        Tracker wrong{chip^1u};wrong.receive_offer(offer(chip,slot));check(!wrong.offer&&!wrong.paired);
        Tracker bonded{chip,0x15547374,slot,true};bonded.receive_offer(offer(chip,host));check(bonded.listens(word(state,1508),prefix));
    }
    // Explicit regression: both old single(slot0) and multi(host index1) fail
    // a newly unpaired target2 whose host uses PICO slot2. Baseline paired hides it.
    for(unsigned oldslot:{0u,1u}){Tracker t{0xA1B2C3E2};t.receive_offer(offer(t.chip,oldslot));check(!t.listens(pair_base(t.chip,0x15547374),0x40));}
    check(pair_base(0xA1B2C3E2,0x15547374)==0x6a6c606d);
    check(pair_base(0xA1B2C3D4u,0x15547374u)!=pair_reverse(0xA1B2C3D4u+0x15547374u));
    PairProduct good(0xA1B2C3E2,"PCTESTTRACKER0003");auto r=product_record(0xA1B2C3E2,"PCTESTTRACKER0003");good.receive(r);check(good.products==1&&good.ack&&good.serial=="PCTESTTRACKER0003");
    for(unsigned kind=0;kind<7;++kind){auto bad=r;if(kind==0)bad.hardware_crc=false;if(kind==1)bad.chip^=1;if(kind==2)bad.channel=1;if(kind==3)bad.partial=true;if(kind==4)bad.dma.back()^=1;if(kind==5)bad.dma.pop_back();if(kind==6)bad.dma[1]=10;PairProduct p(0xA1B2C3E2,"");p.receive(bad);check(p.products==0);}
    PairProduct mismatch(0xA1B2C3E2,"PCTESTTRACKER0002");mismatch.receive(r);check(!mismatch.products);
    // Real ring decoder, not the tracker model: single flag hardcodes chip65.
    Bytes slotbytes(80);auto rest=unsigned(r.dma[0]-1);auto meta=le32(0x80000000u|(rest<<24)|(2u<<19));std::copy(meta.begin(),meta.end(),slotbytes.begin());std::copy(r.dma.begin()+2,r.dma.end()-1,slotbytes.begin()+10);
    auto legacy=expand_alpha_slot(slotbytes.data(),{0xA1B2C3E2,0,0,0},false);check(legacy.chip!=0xA1B2C3E2);
    auto mapped=expand_alpha_slot(slotbytes.data(),{0xA1B2C3E2,0,0,0},true);check(mapped.chip==0xA1B2C3E2);PairProduct mapped_product(0xA1B2C3E2,"");mapped_product.receive(mapped);check(mapped_product.products==1);
    std::cout<<"frame_pair_tests PASS "<<checks<<" synthetic-only; no RF or persistence proof\n";
    return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
