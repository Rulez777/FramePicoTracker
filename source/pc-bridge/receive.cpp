#include "net.h"
#include <iostream>
#include <thread>
int main(int argc,char** argv) try {
    std::string bind="127.0.0.1:39590";double seconds=4;bool expect=false;
    for(int i=1;i<argc;++i){std::string a=argv[i];if(a=="--expect-test")expect=true;
        else if(a=="--bind"&&i+1<argc)bind=argv[++i];else if(a=="--seconds"&&i+1<argc)seconds=std::stod(argv[++i]);else return 2;}
    bridge::Network network;bridge::Receiver receiver;receiver.bindTo(bind);unsigned count=0;bool content=false;
    double end=bridge::now()+seconds;
    while(bridge::now()<end){unsigned n=receiver.poll();count+=n;
        if(n){const auto& p=receiver.stream.packet;content=p.head.valid&&p.trackers.size()==3;
            for(unsigned i=0;i<p.trackers.size();++i)content=content&&p.trackers[i].chip==0x7E570001+i&&p.trackers[i].role==static_cast<bridge::Role>(i+1);}
        std::this_thread::sleep_for(std::chrono::milliseconds(2));}
    bool stale=!receiver.stream.fresh(bridge::now());
    std::cout<<"received="<<count<<" rejected="<<receiver.stream.rejected<<" synthetic_content="<<content<<" stale="<<stale<<"\n";
    return expect?((count>=90&&content&&stale)?0:1):(count?0:1);
}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}
