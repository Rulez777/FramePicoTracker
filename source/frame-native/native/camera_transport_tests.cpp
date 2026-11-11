#include "native_test_runtime.hpp"
#include <string>
#include "camera_transport.hpp"
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
using namespace pico::camera;
namespace {
unsigned checks=0;
void check(bool v) { ++checks; if(!v) throw std::runtime_error("check "+std::to_string(checks)); }
template<class F> void rejects(F f) { bool bad=false; try { f(); } catch(const std::runtime_error&) { bad=true; } check(bad); }
void add(Bytes& a,const Bytes& b) { a.insert(a.end(),b.begin(),b.end()); }
Bytes word(uint32_t n) { return {uint8_t(n),uint8_t(n>>8),uint8_t(n>>16),uint8_t(n>>24)}; }
Bytes number(double n) { uint64_t bits; std::memcpy(&bits,&n,8); Bytes b=word(uint32_t(bits)); add(b,word(uint32_t(bits>>32))); return b; }
void field(Bytes& b,uint32_t k,const Bytes& v) { add(b,word(k)); add(b,word(static_cast<uint32_t>(v.size()))); add(b,v); }
Bytes record(uint32_t kind,const Bytes& b) { Bytes r{'o','c','c','i','p','i','t','l'}; add(r,word(kind)); add(r,word(static_cast<uint32_t>(b.size()))); add(r,b); return r; }
void write(const std::filesystem::path& p,const Bytes& b,bool append=false) {
    std::ofstream f(p,std::ios::binary|(append?std::ios::app:std::ios::trunc));
    f.write(reinterpret_cast<const char*>(b.data()),static_cast<std::streamsize>(b.size())); if(!f) throw std::runtime_error("fixture write");
}
Bytes payload(const Bytes& ts,size_t packet) {
    size_t p=packet*188; check(ts.at(p)==0x47);
    size_t skip=(ts.at(p+3)&0x20)?size_t(ts.at(p+4))+5:4;
    check(skip<=188); return Bytes(ts.begin()+p+skip,ts.begin()+p+188);
}
void parser() {
    Bytes b; field(b,14,{0x65}); // NAL before selector ignored
    field(b,17,{2}); field(b,15,word(7)); field(b,3,number(123.25)); field(b,4,number(.0008));
    const std::string sensor="CAMUPPERLEFT";
    field(b,55,Bytes(sensor.begin(),sensor.end()));
    field(b,13,{0,0,0,2,0x67,0xaa,0,0,0,1,0x68}); field(b,14,{0x65,0xbb});
    field(b,17,{2}); field(b,3,number(124)); field(b,14,{0x61});
    auto units=camera_packets(38,b);
    check(units.size()==2); check(units[0].key.kind==38 && units[0].key.selector==2 && units[0].key.ordinal==0);
    check(units[1].key.ordinal==1); check(units[0].camera_id==7u); check(units[0].timestamp==123.25);
    check(units[0].exposure==.0008); check(units[0].upper()); check(!units[1].upper());
    const Bytes expected{0,0,0,1,0x67,0xaa,0,0,0,1,0x68,0,0,0,1,0x65,0xbb};
    check(units[0].annex_b==expected);
    check(parameter_sets(expected)==Bytes({0,0,0,1,0x67,0xaa,0,0,0,1,0x68}));
    check(parameter_sets({0,0,1,0x67,1,0,0,1,0x65,2,0,0,0,1,0x68,3})==Bytes({0,0,0,1,0x67,1,0,0,0,1,0x68,3}));
    check(parameter_sets({0x67,9})==Bytes({0,0,0,1,0x67,9}));
    check(camera_packets(32,b).size()==2); check(camera_packets(1,{1}).empty());
    for(size_t n=1;n<8;++n) rejects([&] { camera_packets(38,Bytes(n,0)); });
    rejects([&] { camera_packets(38,{17,0,0,0,2,0,0,0,1}); });
    Bytes missing; field(missing,17,{0}); field(missing,14,{1}); rejects([&] { camera_packets(38,missing); });
    for(const Bytes& bad:std::vector<Bytes>{{1},{0,0,0,0},{0,0,0,2,1}}) {
        Bytes broken; field(broken,17,{0}); field(broken,3,number(1)); field(broken,13,bad);
        rejects([&] { camera_packets(38,broken); });
    }
    Bytes metadata; field(metadata,17,{0}); field(metadata,3,number(1)); check(camera_packets(38,metadata).empty());
    // Nonfinite source values are retained verbatim here; clock/mux must reject them.
    field(metadata,3,number(std::numeric_limits<double>::quiet_NaN())); field(metadata,14,{1});
    check(std::isnan(*camera_packets(38,metadata)[0].timestamp));
    Bytes empty; field(empty,17,{1}); field(empty,13,{});
    rejects([&] { camera_packets(38,empty); });
    field(empty,3,number(1)); auto zero=camera_packets(38,empty);
    check(zero.size()==1 && zero[0].annex_b.empty());
}
void reader(const std::filesystem::path& root) {
    const auto p=root/"growing.occ"; const auto r=record(38,{1,2,3});
    for(size_t cut=0;cut<r.size();++cut) {
        write(p,Bytes(r.begin(),r.begin()+cut)); GrowingOcc source(p);
        check(source.poll().empty()); check(source.offset()==0);
        write(p,Bytes(r.begin()+cut,r.end()),true);
        auto got=source.poll(); check(got.size()==1 && got[0].kind==38 && got[0].payload==Bytes({1,2,3}));
        check(source.offset()==r.size()); check(source.records_emitted()==1 && source.bytes_emitted()==r.size());
        check(source.poll().empty());
    }
    Bytes many; for(int i=0;i<130;++i) add(many,r);
    write(p,many);
    GrowingOcc bounded(p); check(bounded.poll().size()==128); check(bounded.poll().size()==2);
    rejects([&] { bounded.poll(0); }); rejects([&] { bounded.poll(1,1); });
    GrowingOcc small(p,4); check(small.poll(100,38).size()==2); check(small.offset()==38);
    Bytes history=r; add(history,Bytes(r.begin(),r.begin()+18)); write(p,history);
    GrowingOcc tail(p); check(tail.seek_complete_tail()==19); check(tail.records_emitted()==0);
    check(tail.poll().empty()); write(p,{3},true); check(tail.poll().size()==1);
    write(p,{}); rejects([&] { tail.poll(); }); rejects([&] { tail.seek_complete_tail(); });
    write(p,r); GrowingOcc replaced(p); check(replaced.poll().size()==1);
    auto old=root/"previous.occ"; std::filesystem::remove(old); std::filesystem::rename(p,old); write(p,r);
    rejects([&] { replaced.poll(); }); rejects([&] { replaced.seek_complete_tail(); });
    auto bad=r; bad[0]='x'; write(p,bad); GrowingOcc corrupt(p); rejects([&] { corrupt.poll(); });
    bad=record(1,{}); bad[12]=5; write(p,bad); GrowingOcc oversized(p,4); rejects([&] { oversized.poll(1,100); });
    rejects([&] { GrowingOcc invalid(p,0); });
}
void mux() {
    check(mpeg_crc({'1','2','3','4','5','6','7','8','9'})==0x0376e6e7); // CRC-32/MPEG-2 check vector
    TimestampMux m; auto first=m.encode({0,0,0,1,0x65},1);
    check(first.pts==90000); check(first.transport.size()==564);
    auto pat=payload(first.transport,0), pmt=payload(first.transport,1), pes=payload(first.transport,2);
    check(pat[0]==0 && pmt[0]==0); check(mpeg_crc(Bytes(pat.begin()+1,pat.end()))==0);
    check(mpeg_crc(Bytes(pmt.begin()+1,pmt.end()))==0);
    check(Bytes(pat.begin()+1,pat.begin()+13)==Bytes({0,0xb0,0x0d,0,1,0xc1,0,0,0,1,0xe1,0}));
    check(Bytes(pmt.begin()+1,pmt.begin()+18)==Bytes({2,0xb0,0x12,0,1,0xc1,0,0,0xe1,1,0xf0,0,0x1b,0xe1,1,0xf0,0}));
    check(pes==Bytes({0,0,1,0xe0,0,13,0x84,0x80,5,0x21,0,5,0xbf,0x21,0,0,0,1,0x65}));
    check(Bytes(first.transport.begin()+376,first.transport.begin()+379)==Bytes({0x47,0x41,1}));
    // PTS rounding ties (binary64 product is exactly half), wrapping, no arrival clock.
    for(auto pair:std::vector<std::pair<double,uint64_t>>{{0,0},{.5/90000,0},{1.5/90000,2},{2.5/90000,2},{8589934592.0/90000,0}})
        check(m.encode({1},pair.first).pts==pair.second);
    for(double t:{-1.0,std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::max()}) rejects([&] { m.encode({1},t); });
    rejects([&] { m.encode({},0); });
    for(size_t n:{size_t(1),size_t(162),size_t(163),size_t(346),size_t(347),size_t(65527),size_t(65528)}) {
        TimestampMux fresh; Bytes au(n); for(size_t i=0;i<n;++i) au[i]=uint8_t(i);
        auto ts=fresh.encode(au,0).transport; check(ts.size()%188==0);
        Bytes reassembled;
        for(size_t i=2;i<ts.size()/188;++i) { check((ts[i*188+3]&15)==((i-2)&15)); add(reassembled,payload(ts,i)); }
        check(Bytes(reassembled.begin()+14,reassembled.end())==au);
        auto len=unsigned(reassembled[4])*256+reassembled[5]; check(len==(n<=65527?n+8:0));
        auto second=fresh.encode({1},0); check((second.transport[3]&15)==1); check((second.transport[191]&15)==1);
    }
}
}
int main() {
    // Dedicated fixture directory, only remove explicitly created fixture files.
    auto root=std::filesystem::path("build")/"camera-fixtures";
    try {
        std::filesystem::create_directories(root); parser(); reader(root); mux();
        std::cout<<"camera transport: "<<checks<<" checks passed\n"; return 0;
    } catch(const std::exception& e) { std::cerr<<e.what()<<"\n"; return 1; }
}

