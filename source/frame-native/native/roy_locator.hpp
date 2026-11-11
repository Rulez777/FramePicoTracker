#pragma once

// Roy firmware entry points. Signatures are the first 48 bytes of each Thumb
// function in roy-old.bin. BL/B.W immediates and word-aligned 0x20xxxxxx
// literals are masked. Each signature must match exactly once.
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include "roy_signatures.hpp"

namespace pico {

struct RoyLayout {
    uint32_t fn_request, hook, static_word, regions, code, data, code_size, data_size;
    uint32_t fn_open;
    uint32_t fn_close;
    uint32_t fn_lock;
    uint32_t fn_unlock;
    uint32_t fn_print;
    uint32_t mpsl;
    uint32_t rtt;
    bool ok;
    std::string why;
};

namespace roy_detail {

constexpr std::size_t k_sig_len = 48;

// 0x10581 open
constexpr uint8_t k_open[k_sig_len] = {
    0x2d, 0xe9, 0xf0, 0x41, 0x0e, 0x46, 0x05, 0x00, 0x04, 0xd0, 0x00, 0xf0, 0x47, 0xf8, 0x07, 0x46,
    0x00, 0x24, 0x0b, 0xe0, 0x6f, 0xf0, 0x15, 0x00, 0xbd, 0xe8, 0xf0, 0x81, 0x20, 0x46, 0x00, 0xf0,
    0x43, 0xf8, 0x90, 0xf8, 0x26, 0x00, 0x30, 0xb1, 0x64, 0x1c, 0xe4, 0xb2, 0xbc, 0x42, 0xf5, 0xd3};
constexpr uint8_t k_open_mask[k_sig_len] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xf8, 0x00, 0xd0, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xf8,
    0x00, 0xd0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

// 0x189bf close
constexpr uint8_t k_close[k_sig_len] = {
    0x10, 0xb5, 0x04, 0x46, 0xf7, 0xf7, 0x31, 0xfe, 0x01, 0x00, 0x05, 0xd0, 0x91, 0xf8, 0x24, 0x00,
    0x6f, 0xf0, 0x22, 0x02, 0x78, 0xb9, 0x02, 0xe0, 0x6f, 0xf0, 0x01, 0x00, 0x10, 0xbd, 0x91, 0xf8,
    0x26, 0x00, 0x40, 0xb1, 0x02, 0x28, 0x08, 0xd0, 0x01, 0x28, 0x06, 0xd0, 0x01, 0x20, 0x81, 0xf8};
constexpr uint8_t k_close_mask[k_sig_len] = {
    0xff, 0xff, 0xff, 0xff, 0x00, 0xf8, 0x00, 0xd0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff};

// 0x28e2d lock
constexpr uint8_t k_lock[k_sig_len] = {
    0x02, 0x46, 0x0b, 0x46, 0x01, 0x48, 0x02, 0xf0, 0x7d, 0xbc, 0x00, 0xbf, 0xc4, 0x0e, 0x00, 0x20,
    0x01, 0x48, 0x02, 0xf0, 0xf1, 0xbc, 0x00, 0xbf, 0xc4, 0x0e, 0x00, 0x20, 0x10, 0xb5, 0x17, 0x49,
    0x17, 0x48, 0x88, 0xb0, 0x0a, 0xf0, 0xfd, 0xfc, 0x00, 0xf0, 0x0c, 0xfe, 0x03, 0xa8, 0xd8, 0xf7};
constexpr uint8_t k_lock_mask[k_sig_len] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xf8, 0x00, 0xd0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0x00, 0xf8, 0x00, 0xd0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x00, 0xf8, 0x00, 0xd0, 0x00, 0xf8, 0x00, 0xd0, 0xff, 0xff, 0xff, 0xff};

// 0x28e3d unlock
constexpr uint8_t k_unlock[k_sig_len] = {
    0x01, 0x48, 0x02, 0xf0, 0xf1, 0xbc, 0x00, 0xbf, 0xc4, 0x0e, 0x00, 0x20, 0x10, 0xb5, 0x17, 0x49,
    0x17, 0x48, 0x88, 0xb0, 0x0a, 0xf0, 0xfd, 0xfc, 0x00, 0xf0, 0x0c, 0xfe, 0x03, 0xa8, 0xd8, 0xf7,
    0xff, 0xf8, 0x14, 0x48, 0x0a, 0xf0, 0xb5, 0xfa, 0xd0, 0xb1, 0x13, 0x4a, 0x07, 0xca, 0x6b, 0x46};
constexpr uint8_t k_unlock_mask[k_sig_len] = {
    0xff, 0xff, 0x00, 0xf8, 0x00, 0xd0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x00, 0xf8, 0x00, 0xd0, 0x00, 0xf8, 0x00, 0xd0, 0xff, 0xff, 0x00, 0xf8,
    0x00, 0xd0, 0xff, 0xff, 0x00, 0xf8, 0x00, 0xd0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

// 0x2ee2f print
constexpr uint8_t k_print[k_sig_len] = {
    0x83, 0x68, 0x99, 0x63, 0x09, 0xb9, 0xff, 0xf7, 0x8f, 0xbe, 0x70, 0x47, 0x10, 0xb5, 0x02, 0x69,
    0x83, 0x68, 0x12, 0x68, 0x01, 0x68, 0x04, 0x46, 0xc0, 0x68, 0x98, 0x47, 0x23, 0x69, 0x00, 0x22,
    0x1a, 0x60, 0x10, 0xbd, 0x38, 0xb5, 0xcb, 0x68, 0x1b, 0x7c, 0x02, 0x2b, 0x05, 0x46, 0x0c, 0x46};
constexpr uint8_t k_print_mask[k_sig_len] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xf8, 0x00, 0xd0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

inline int count_hits(const std::vector<uint8_t> &flash, const uint8_t *bytes, const uint8_t *mask, std::size_t &hit, std::size_t len = k_sig_len) {
    int n = 0;
    hit = 0;
    if (flash.size() < len) return 0;
    for (std::size_t i = 0; i + len <= flash.size(); i += 2) {
        std::size_t k = 0;
        for (; k < len; ++k) {
            if ((flash[i + k] & mask[k]) != (bytes[k] & mask[k])) break;
        }
        if (k == len) {
            hit = i;
            ++n;
        }
    }
    return n;
}

inline void note(std::string &why, const char *name, int n) {
    if (!why.empty()) why.push_back(';');
    why.append(name);
    why.append(n == 0 ? ":0" : ":many");
}

}  // namespace roy_detail

inline RoyLayout locate_roy(const std::vector<uint8_t> &flash) {
    RoyLayout out{};
    // TODO: derive mpsl and rtt from the flash image.
    out.mpsl = 0x2001056cu;
    out.rtt = 0x2001064cu;

    struct Slot {
        const uint8_t *bytes;
        const uint8_t *mask;
        const char *name;
        uint32_t *dest;
    };
    const Slot slots[] = {
        {roy_detail::k_open, roy_detail::k_open_mask, "fn_open", &out.fn_open},
        {roy_detail::k_close, roy_detail::k_close_mask, "fn_close", &out.fn_close},
        {roy_detail::k_lock, roy_detail::k_lock_mask, "fn_lock", &out.fn_lock},
        {roy_detail::k_unlock, roy_detail::k_unlock_mask, "fn_unlock", &out.fn_unlock},
        {roy_detail::k_print, roy_detail::k_print_mask, "fn_print", &out.fn_print},
    };

    bool all = true;
    for (const Slot &s : slots) {
        std::size_t hit = 0;
        const int n = roy_detail::count_hits(flash, s.bytes, s.mask, hit);
        if (n != 1) {
            all = false;
            roy_detail::note(out.why, s.name, n);
            continue;
        }
        *s.dest = static_cast<uint32_t>(hit) | 1u;
    }
    auto read32 = [&](size_t at) -> uint32_t {
        if (at > flash.size() || flash.size()-at < 4) { all=false; return 0; }
        return uint32_t(flash[at]) | (uint32_t(flash[at+1])<<8) |
               (uint32_t(flash[at+2])<<16) | (uint32_t(flash[at+3])<<24);
    };
    auto reference = [&](const uint8_t* bytes, const uint8_t* mask, size_t len,
                         const char* name, size_t offset, bool thumb=false) {
        size_t hit=0;
        const int n=roy_detail::count_hits(flash,bytes,mask,hit,len);
        if(n!=1) { all=false; roy_detail::note(out.why,name,n); return uint32_t(0); }
        return thumb ? (uint32_t(hit)|1u) : read32(hit+offset);
    };
    using namespace roy_detail;
    out.fn_request=reference(k_request,k_request_mask,sizeof(k_request),"request",0,true);
    out.mpsl=reference(k_mpsl_ref,k_mpsl_ref_mask,sizeof(k_mpsl_ref),"mpsl",72);
    out.rtt=reference(k_rtt_ref,k_rtt_ref_mask,sizeof(k_rtt_ref),"rtt",16);
    out.static_word=reference(k_static_ref,k_static_ref_mask,sizeof(k_static_ref),"static",16);
    out.regions=reference(k_regions_ref,k_regions_ref_mask,sizeof(k_regions_ref),"regions",16);
    // shell initializer loads its const shell descriptor. print stores the bypass
    // callback at descriptor->ctx + 56 (6883/6399 in the print signature).
    const auto shell=reference(k_shell_ref,k_shell_ref_mask,sizeof(k_shell_ref),"shell",32);
    if(shell && uint64_t(shell)+12u<=flash.size()) out.hook=read32(shell+8u)+56u;
    else { all=false; out.why+=";shell descriptor"; }
    auto ram=[](uint32_t v) { return v>=0x20000000u && v<0x20040000u; };
    if(!ram(out.mpsl)||!ram(out.rtt)||!ram(out.hook)||!ram(out.static_word)||
       !ram(out.regions)||out.regions!=out.static_word+2u|| (out.hook&3u)) {
        all=false; out.why+=";RAM references";
    }
    out.ok = all;
    return out;
}

// RTT block is 24-byte header followed by three up and three down descriptors.
inline RoyLayout locate_roy_buffers(RoyLayout r, const std::vector<uint8_t>& block) {
    auto reject=[&](const char* why){r.ok=false;r.why+=std::string(";")+why;};
    if(!r.ok) return r;
    if(block.size()!=168 || std::memcmp(block.data(),"SEGGER RTT",10)!=0) {
        reject("RTT header/length"); return r;
    }
    auto w=[&](size_t o){return uint32_t(block[o])|(uint32_t(block[o+1])<<8)|
        (uint32_t(block[o+2])<<16)|(uint32_t(block[o+3])<<24);};
    if(w(16)!=3 || w(20)!=3) {reject("RTT descriptor counts");return r;}
    auto range=[](uint32_t p,uint32_t size){return p>=0x20000000u && size &&
        uint64_t(p)+size<=0x20040000u;};
    auto buffer=[&](size_t index,uint32_t& start,uint32_t& size){
        const size_t up=24+index*24,down=96+index*24;
        const uint32_t lo=w(down+4),hi=w(up+4),ls=w(down+8),hs=w(up+8);
        if(!range(lo,ls)||!range(hi,hs)||uint64_t(lo)+ls!=hi) return false;
        start=(lo+3u)&~3u;
        size=hi+hs-start;
        return range(start,size);
    };
    if(!buffer(1,r.code,r.code_size)||!buffer(2,r.data,r.data_size)) {
        reject("RTT buffers not contiguous/in RAM");return r;
    }
    if(r.code_size<2044 || r.data_size<2044 ||
       uint64_t(r.code)+r.code_size>(uint64_t(r.code&~0xfffu)+4096u) ||
       !(uint64_t(r.code)+r.code_size<=r.data || uint64_t(r.data)+r.data_size<=r.code))
        reject("RTT capacity/MPU/overlap");
    auto overlaps=[](uint32_t a,uint32_t n,uint32_t b,uint32_t m){
        return uint64_t(a)<uint64_t(b)+m && uint64_t(b)<uint64_t(a)+n;
    };
    const uint32_t protected_starts[]={r.rtt,r.mpsl,r.hook,r.static_word,w(28),w(100)};
    const uint32_t protected_sizes[]={168,48,4,4,w(32),w(104)};
    for(size_t i=0;i<6;++i) if(overlaps(r.code,r.code_size,protected_starts[i],protected_sizes[i])||
        overlaps(r.data,r.data_size,protected_starts[i],protected_sizes[i])) reject("RTT reservation overlaps live metadata");
    // Console buffers are independent and need not have fixed addresses.
    const auto up=w(28),us=w(32),down=w(100),ds=w(104);
    if(!range(up,us)||!range(down,ds)||us!=4096||ds!=64)
        reject("console RTT ABI");
    return r;
}
}  // namespace pico
