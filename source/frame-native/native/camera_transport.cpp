#include "device_config.hpp"
#include <utility>
#include "camera_transport.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <limits>
#include <stdexcept>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <sys/stat.h>
#endif

namespace pico::camera {
namespace {
[[noreturn]] void fail(const char* s) { throw std::runtime_error(s); }
uint32_t le(const uint8_t* p) {
    return uint32_t(p[0]) | uint32_t(p[1])<<8 | uint32_t(p[2])<<16 | uint32_t(p[3])<<24;
}
double real(const uint8_t* p) {
    uint64_t bits=uint64_t(le(p)) | uint64_t(le(p+4))<<32;
    double v; static_assert(sizeof(v)==sizeof(bits)); std::memcpy(&v,&bits,8); return v;
}
void append(Bytes& a, const Bytes& b) { a.insert(a.end(),b.begin(),b.end()); }
void nal(Bytes& a, const uint8_t* p, size_t n) {
    a.insert(a.end(),{0,0,0,1}); a.insert(a.end(),p,p+n);
}
struct File {
#ifdef _WIN32
    HANDLE handle;
    explicit File(const std::filesystem::path& p) : handle(CreateFileW(p.c_str(),GENERIC_READ,
        FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,0,nullptr)) {
        if(handle==INVALID_HANDLE_VALUE) fail("Cannot open OCC");
    }
    ~File() { CloseHandle(handle); }
    std::array<uint64_t,2> identity() {
        BY_HANDLE_FILE_INFORMATION s{};
        if(!GetFileInformationByHandle(handle,&s)) fail("Cannot stat OCC");
        return {s.dwVolumeSerialNumber,uint64_t(s.nFileIndexHigh)<<32|s.nFileIndexLow};
    }
    uint64_t size() { LARGE_INTEGER s{}; if(!GetFileSizeEx(handle,&s)) fail("Cannot size OCC");
    return uint64_t(s.QuadPart); }
    Bytes read(uint64_t off, size_t n) {
        LARGE_INTEGER pos{}; pos.QuadPart=static_cast<LONGLONG>(off);
        if(!SetFilePointerEx(handle,pos,nullptr,FILE_BEGIN)) fail("Cannot seek OCC");
        Bytes b(n); DWORD got=0;
        if(!ReadFile(handle,b.data(),static_cast<DWORD>(n),&got,nullptr)) fail("Cannot read OCC");
        b.resize(got); return b;
    }
#else
    FILE* handle;
    explicit File(const std::filesystem::path& p) : handle(std::fopen(p.c_str(),"rb")) { if(!handle) fail("Cannot open OCC"); }
    ~File() { std::fclose(handle); }
    struct stat stat_value() { struct stat s{}; if(fstat(fileno(handle),&s)) fail("Cannot stat OCC");
    return s; }
    std::array<uint64_t,2> identity() { auto s=stat_value(); return {uint64_t(s.st_dev),uint64_t(s.st_ino)}; }
    uint64_t size() { return uint64_t(stat_value().st_size); }
    Bytes read(uint64_t off,size_t n) {
        if(off>uint64_t(std::numeric_limits<off_t>::max()) || fseeko(handle,static_cast<off_t>(off),SEEK_SET)) fail("Cannot seek OCC");
        Bytes b(n); auto got=std::fread(b.data(),1,n,handle);
        if(std::ferror(handle)) fail("Cannot read OCC");
        b.resize(got); return b;
    }
#endif
    File(const File&)=delete; File& operator=(const File&)=delete;
};
uint32_t header_size(const Bytes& h,uint32_t limit) {
    if(std::memcmp(h.data(),"occipitl",8)) fail("Invalid OCC header");
    auto n=le(h.data()+12); if(n>limit) fail("Record exceeds limit");
    return n;
}
}
bool AccessUnit::upper() const { return sensor && (*sensor==pico::devices::upper(0) || *sensor==pico::devices::upper(1)); }
std::vector<AccessUnit> camera_packets(uint32_t kind,const Bytes& data) {
    if(kind!=32 && kind!=38) return {};
    std::vector<AccessUnit> streams; std::map<uint8_t,uint32_t> ordinals;
    AccessUnit* current=nullptr;
    std::vector<bool> has_chunks;
    size_t off=0;
    while(off<data.size()) {
        if(data.size()-off<8) fail("Incomplete TLV header");
        auto field=le(data.data()+off), n=le(data.data()+off+4); off+=8;
        if(n>data.size()-off) fail("Incomplete TLV value");
        const auto* p=data.data()+off;
        if(field==17 && n==1) {
            AccessUnit unit{}; unit.key={kind,p[0],ordinals[p[0]]++};
            streams.push_back(std::move(unit)); has_chunks.push_back(false); current=&streams.back();
        } else if(current) {
            if(field==15 && n==4) current->camera_id=le(p);
            else if(field==3 && n==8) current->timestamp=real(p);
            else if(field==4 && n==8) current->exposure=real(p);
            else if(field==55) {
                std::string s;
                for(size_t i=0;i<n && p[i];++i) {
                    if(p[i]<128) s.push_back(char(p[i])); else s+="\xef\xbf\xbd";
                }
                current->sensor=std::move(s);
            } else if(field==14) { has_chunks.back()=true; nal(current->annex_b,p,n); }
            else if(field==13) {
                has_chunks.back()=true;
                size_t k=0;
                while(k<n) {
                    if(n-k<4) fail("Incomplete NAL length");
                    uint32_t len=uint32_t(p[k])<<24|uint32_t(p[k+1])<<16|uint32_t(p[k+2])<<8|p[k+3]; k+=4;
                    if(!len || len>n-k) fail("Invalid NAL size");
                    nal(current->annex_b,p+k,len); k+=len;
                }
            }
        }
        off+=n;
    }
    std::vector<AccessUnit> result;
    for(size_t i=0;i<streams.size();++i) if(has_chunks[i]) {
        auto& s=streams[i];
        if(!s.timestamp) fail("Camera access unit has no source timestamp");
        result.push_back(std::move(s));
    }
    return result;
}
Bytes parameter_sets(const Bytes& data) {
    // re.split reference also considers any prefix before the first start code.
    Bytes out; size_t begin=0,i=0;
    auto save=[&](size_t end) { if(end>begin && ((data[begin]&31)==7 || (data[begin]&31)==8)) nal(out,data.data()+begin,end-begin); };
    while(i+3<=data.size()) {
        size_t length=0;
        if(data[i]==0 && data[i+1]==0) {
            if(data[i+2]==1) length=3;
            else if(i+4<=data.size() && data[i+2]==0 && data[i+3]==1) length=4;
        }
        if(length) { save(i); i+=length; begin=i; } else ++i;
    }
    save(data.size()); return out;
}
GrowingOcc::GrowingOcc(std::filesystem::path p,uint32_t limit) : path_(std::move(p)),limit_(limit) {
    if(!limit) fail("Invalid OCC record limit");
}
std::vector<Record> GrowingOcc::poll(size_t count,uint64_t budget) {
    if(!count || budget<limit_) fail("Invalid OCC polling budget");
    File file(path_); auto id=file.identity();
    if(identity_ && *identity_!=id) fail("Recording was replaced; decoder reset required");
    identity_=id;
    if(file.size()<offset_) fail("Recording was truncated");
    uint64_t consumed=0; std::vector<Record> out;
    for(size_t i=0;i<count;++i) {
        auto h=file.read(offset_,16); if(h.size()<16) break;
        auto n=header_size(h,limit_);
        if(uint64_t(n)+16>budget-consumed) break;
        auto b=file.read(offset_+16,n); if(b.size()!=n) break;
        out.push_back({le(h.data()+8),std::move(b)});
        offset_+=16+uint64_t(n); consumed+=16+uint64_t(n); ++records_; bytes_+=16+uint64_t(n);
    }
    return out;
}
uint64_t GrowingOcc::seek_complete_tail() {
    File file(path_); auto id=file.identity();
    if(identity_ && *identity_!=id) fail("Recording was replaced; decoder reset required");
    identity_=id; auto end=file.size();
    if(end<offset_) fail("Recording was truncated");
    while(end-offset_>=16) {
        auto h=file.read(offset_,16); if(h.size()!=16) fail("Recording changed during tail scan");
        auto n=header_size(h,limit_); if(uint64_t(n)+16>end-offset_) break;
        offset_+=16+uint64_t(n);
    }
    return offset_;
}
uint32_t mpeg_crc(const Bytes& b) {
    uint32_t crc=0xffffffff;
    for(auto v:b) { crc^=uint32_t(v)<<24; for(int i=0;i<8;++i) crc=(crc<<1)^((crc&0x80000000)?0x04c11db7:0); }
    return crc;
}
Bytes TimestampMux::packet(uint16_t pid,const Bytes& payload,bool start,std::optional<uint64_t> pcr) {
    if(payload.size()>(pcr?176u:184u)) fail("TS payload too large");
    auto cc=continuity_[pid]; continuity_[pid]=uint8_t((cc+1)&15);
    Bytes out{0x47,uint8_t((start?0x40:0)|(pid>>8)),uint8_t(pid)};
    if(payload.size()==184) out.push_back(uint8_t(0x10|cc));
    else {
        out.push_back(uint8_t(0x30|cc)); auto length=183-payload.size(); out.push_back(uint8_t(length));
        if(length) {
            out.push_back(pcr?0x10:0);
            if(pcr) { auto v=*pcr; for(int shift:{25,17,9,1}) out.push_back(uint8_t(v>>shift));
            out.push_back(uint8_t(((v&1)<<7)|0x7e)); out.push_back(0); }
            out.resize(4+1+length,0xff);
        }
    }
    append(out,payload); if(out.size()!=188) fail("Invalid TS packet size");
    return out;
}
Muxed TimestampMux::encode(const Bytes& au,double timestamp) {
    if(!std::isfinite(timestamp) || timestamp<0 || au.empty()) fail("Timestamped access unit required");
    // Round the binary64 product to nearest with ties even, regardless of fenv.
    const double ticks=timestamp*90000;
    if(!std::isfinite(ticks)) fail("Timestamp tick overflow");
    double rounded=std::floor(ticks), fraction=ticks-rounded;
    if(fraction>0.5 || (fraction==0.5 && std::fmod(rounded,2.0)!=0)) rounded+=1;
    auto pts=static_cast<uint64_t>(std::fmod(rounded,8589934592.0));
    Bytes out;
    auto table=[&](uint16_t pid,Bytes b) {
        auto crc=mpeg_crc(b); for(int shift:{24,16,8,0}) b.push_back(uint8_t(crc>>shift));
        b.insert(b.begin(),0); append(out,packet(pid,b,true));
    };
    table(0,{0x00,0xb0,0x0d,0,1,0xc1,0,0,0,1,0xe1,0});
    table(0x100,{2,0xb0,0x12,0,1,0xc1,0,0,0xe1,1,0xf0,0,0x1b,0xe1,1,0xf0,0});
    const size_t length=au.size()<=65527?au.size()+8:0;
    Bytes pes{0,0,1,0xe0,uint8_t(length>>8),uint8_t(length),0x84,0x80,5,
        uint8_t(0x21|((pts>>29)&14)),uint8_t(pts>>22),uint8_t(1|((pts>>14)&254)),uint8_t(pts>>7),uint8_t(1|((pts<<1)&254))};
    append(pes,au);
    size_t off=0;
    while(off<pes.size()) {
        bool first=off==0; auto n=std::min(first?size_t(176):size_t(184),pes.size()-off);
        append(out,packet(0x101,Bytes(pes.begin()+off,pes.begin()+off+n),first,first?std::optional<uint64_t>(pts):std::nullopt)); off+=n;
    }
    return {pts,std::move(out)};
}
} // namespace pico::camera

