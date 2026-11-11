// Offline pixel statistics of captured xrtap PGM frames (no detector library).
// Build: g++ -std=c++17 -O2 -o lower_stats lower_stats.cpp
// Usage: lower_stats <frames_dir> <camera e.g. video9> [thr1 thr2 thr3]
// Per frame: kind (short/long by gap to previous frame: short gap <15 ms -> this frame follows the pair partner),
// max pixel, count of pixels >= thr, number of 8-connected components >= thr with area 2..250 (detector limits).
// Output: per-frame CSV on stdout (ts,gap_ms,max,n1,c1,n2,c2,n3,c3), summary on stderr.
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
namespace fs=std::filesystem;
static bool read_pgm(const fs::path&p,int&w,int&h,std::vector<uint8_t>&px){
    std::ifstream f(p,std::ios::binary); std::string m; int mx; f>>m>>w>>h>>mx; f.get();
    if(m!="P5"||w<=0||h<=0) return false; px.resize(size_t(w)*h); f.read((char*)px.data(),px.size()); return bool(f);
}
static void comps(const std::vector<uint8_t>&px,int w,int h,int thr,int&n,int&c,std::vector<int>&lab){
    n=0;c=0; lab.assign(px.size(),0); std::vector<int> st; int id=0;
    for(int i=0;i<w*h;++i){ if(px[i]>=thr){++n; if(!lab[i]){++id; int area=0; st.push_back(i); lab[i]=id;
        while(!st.empty()){int k=st.back();st.pop_back();++area;int x=k%w,y=k/w;
            for(int dy=-1;dy<=1;++dy)for(int dx=-1;dx<=1;++dx){int xx=x+dx,yy=y+dy; if(xx<0||yy<0||xx>=w||yy>=h)continue;int j=yy*w+xx; if(px[j]>=thr&&!lab[j]){lab[j]=id;st.push_back(j);}}}
        if(area>=2&&area<=250)++c;}}}
}
int main(int argc,char**argv){
    if(argc<3){std::cerr<<"usage\n";return 2;}
    int thr[3]={180,120,80}; for(int i=0;i<3&&3+i<argc;++i)thr[i]=atoi(argv[3+i]);
    std::vector<std::pair<long long,fs::path>> files; std::string pre=std::string(argv[2])+"_";
    for(auto&e:fs::directory_iterator(argv[1])){auto n=e.path().filename().string(); if(n.rfind(pre,0)==0) files.push_back({atoll(n.c_str()+pre.size()),e.path()});}
    std::sort(files.begin(),files.end());
    long long prev=0; std::vector<uint8_t> px; std::vector<int> lab;
    long hist[2][3][6]={}; long frames[2]={}; double maxsum[2]={};
    for(auto&[ts,p]:files){int w,h; if(!read_pgm(p,w,h,px))continue; double gap=prev?(ts-prev)/1e6:0; prev=ts;
        int kind=gap>0&&gap<15?1:0; int mx=*std::max_element(px.begin(),px.end());
        std::printf("%lld,%.3f,%d",ts,gap,mx); ++frames[kind]; maxsum[kind]+=mx;
        for(int t=0;t<3;++t){int n,c;comps(px,w,h,thr[t],n,c,lab);std::printf(",%d,%d",n,c); hist[kind][t][std::min(c,5)]++;}
        std::printf("\n");}
    for(int k=0;k<2;++k){std::cerr<<(k?"after-short-gap":"after-long-gap")<<" frames="<<frames[k]<<" mean_max="<<(frames[k]?maxsum[k]/frames[k]:0)<<"\n";
        for(int t=0;t<3;++t){std::cerr<<"  thr"<<thr[t]<<" comps 0..5+:";for(int i=0;i<6;++i)std::cerr<<" "<<hist[k][t][i];std::cerr<<"\n";}}
}
