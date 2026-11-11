#include "roy_locator.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

static std::vector<uint8_t> load_bin(const char *path) {
    std::ifstream in(path, std::ios::binary);
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

static bool fns(const pico::RoyLayout &r, uint32_t open, uint32_t close, uint32_t lock, uint32_t unlock, uint32_t print) {
    return r.ok && r.fn_open == open && r.fn_close == close && r.fn_lock == lock && r.fn_unlock == unlock &&
           r.fn_print == print && r.mpsl == 0x2001056cu && r.rtt == 0x2001064cu;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "FAIL\n";
        return 1;
    }
    const std::vector<uint8_t> old_bin = load_bin(argv[1]);
    const std::vector<uint8_t> new_bin = load_bin(argv[2]);
    const pico::RoyLayout old_layout = pico::locate_roy(old_bin);
    const pico::RoyLayout new_layout = pico::locate_roy(new_bin);

    bool pass = fns(old_layout, 0x10581u, 0x189bfu, 0x28e2du, 0x28e3du, 0x2ee2fu) &&
                fns(new_layout, 0x10581u, 0x189bfu, 0x28efdu, 0x28f0du, 0x2ef5bu);

    if (old_bin.size() <= 0x10580u) {
        pass = false;
    } else {
        std::vector<uint8_t> flipped = old_bin;
        flipped[0x10580] = static_cast<uint8_t>(flipped[0x10580] ^ 0x01u);
        const pico::RoyLayout bad = pico::locate_roy(flipped);
        if (bad.ok || bad.why.empty()) pass = false;
    }

    pass=pass && old_layout.fn_request==0x18949 && new_layout.fn_request==0x18949 &&
        old_layout.hook==0x2000d8cc && new_layout.hook==old_layout.hook &&
        old_layout.static_word==0x20010a24 && new_layout.static_word==0x20010a25 &&
        old_layout.regions==0x20010a26 && new_layout.regions==0x20010a27;
    // Every required signature must be unique, and every missing one rejects.
    const size_t signatures[]={0x10580,0x189be,0x28e2c,0x28e3c,0x2ee2e,
        0x18948,0x29b8c,0x1f8bc,0x20a94,0x1e214,0x2af84};
    for(auto at:signatures) {
        auto bad=old_bin;bad[at]^=1;
        if(pico::locate_roy(bad).ok) {std::cerr<<"corrupt accepted "<<std::hex<<at<<"\n";pass=false;}
        bad=old_bin;
        bad.insert(bad.end(),old_bin.begin()+at,old_bin.begin()+at+80);
        if(pico::locate_roy(bad).ok) {std::cerr<<"duplicate accepted "<<std::hex<<at<<"\n";pass=false;}
    }
    auto block=[](bool newer){
        std::vector<uint8_t> b(168);std::memcpy(b.data(),"SEGGER RTT",10);
        auto w=[&](size_t o,uint32_t v){for(int k=0;k<4;k++)b[o+k]=uint8_t(v>>(8*k));};
        w(16,3);w(20,3);w(28,0x20012060+unsigned(newer));w(32,4096);
        w(100,0x20012020+unsigned(newer));w(104,64);
        w(52,0x20011918+(newer?2:1));w(56,1024);
        w(124,0x20011518+(newer?2:1));w(128,1024);
        w(76,0x20011018+unsigned(newer));w(80,1024);
        w(148,0x20010c18+unsigned(newer));w(152,1024);return b;
    };
    auto old_buffers=pico::locate_roy_buffers(old_layout,block(false));
    auto new_buffers=pico::locate_roy_buffers(new_layout,block(true));
    pass=pass && old_buffers.ok && new_buffers.ok && old_buffers.code==0x2001151c &&
        old_buffers.code_size==2045 && old_buffers.data_size==2048 &&
        new_buffers.code_size==2046 && new_buffers.data==0x20010c1c && new_buffers.data_size==2045;
    for(auto offset:{0u,16u,20u,52u,56u,124u,128u,148u,152u,104u}) {
        auto b=block(true);b[offset]^=1;
        if(offset==56) {b[56]=1;b[57]=0;}
        if(pico::locate_roy_buffers(new_layout,b).ok) {std::cerr<<"RTT corruption accepted "<<offset<<"\n";pass=false;}
    }
    if(pico::locate_roy({}).ok || pico::locate_roy_buffers(new_layout,{}).ok)pass=false;
    std::cout << (pass ? "PASS\n" : "FAIL\n");
    return pass ? 0 : 1;
}
