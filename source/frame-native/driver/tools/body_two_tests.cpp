#define _USE_MATH_DEFINES
#include "../pico_body.h"
#include <iostream>
#include <stdexcept>
using namespace pico_body;
static void check(bool value) { if(!value) throw std::runtime_error("two-foot body regression"); }
int main() { try {
    BodyRig rig; rig.SetAuto(2);
    Tracker::Output feet[2];
    for(auto &f:feet) { f.valid=true; f.positionTracked=true; f.still=true; f.opticalAgeNs=0; }
    feet[0].position={-.1,.05,0}; feet[1].position={.1,.05,0};
    BodyRig::Slot slots[2]={{0xA1B2C3D4,&feet[0]},{0xA1B2C3E2,&feet[1]}};
    for(int i=0;i<151;++i) rig.Step(i*.01,true,{0,1.7,0},{},slots,2);
    check(rig.Calibrated());
    check(rig.RoleOf(slots[0].chip)==LeftFoot && rig.RoleOf(slots[1].chip)==RightFoot);
    for(auto &f:feet) { f.still=false; f.opticalAgeNs=1e9; }
    for(int i=151;i<201;++i) rig.Step(i*.01,true,{(i-150)*.003,1.7,0},{},slots,2);
    check(feet[0].position.x>-.099 && feet[1].position.x>.101);
    // One tracker cannot activate auto-body; a real pelvis still takes precedence.
    BodyRig single;single.SetAuto(1);check(!single.Active());
    BodyRig three;three.SetAuto(3);Tracker::Output waist;waist.valid=true;waist.positionTracked=true;waist.still=true;waist.opticalAgeNs=0;waist.position={0,.95,0};
    for(auto &f:feet) {f.still=true;f.opticalAgeNs=0;}
    slots[0].o->position={-.1,.05,0};slots[1].o->position={.1,.05,0};
    BodyRig::Slot all[3]={slots[0],slots[1],{0xA1B2C3E1,&waist}};
    for(int i=0;i<151;++i)three.Step(i*.01,true,{0,1.7,0},{},all,3);
    check(three.Calibrated() && three.RoleOf(0xA1B2C3E1)==Pelvis);
    std::cout<<"body two/three tracker PASS\n";return 0;
}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
