#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

std::vector<double> compact_led_cluster_test(const std::vector<double>& blobs, int cap_neighbors, bool prefer_8) {
    struct Cluster { std::vector<size_t> ids; double score=0; } best;
    const size_t n=blobs.size()/5;
    if(n<=48) { std::vector<double> all; for(size_t j=0;j<n;++j){all.push_back(blobs[5*j]);all.push_back(blobs[5*j+1]);} return all; }
    for(size_t seed=0;seed<n;++seed) {
        const double area=blobs[5*seed+4];
        if(!std::isfinite(area)||area<=0||area>400)continue;
        std::vector<std::pair<double,size_t>> neighbors;
        for(size_t j=0;j<n;++j) {
            const double a=blobs[5*j+4];
            const double dx=blobs[5*j]-blobs[5*seed],dy=blobs[5*j+1]-blobs[5*seed+1];
            const double d=dx*dx+dy*dy;
            if(std::isfinite(d)&&std::isfinite(a)&&a>0&&a<=400&&a>=area/3&&a<=area*3&&d<=200*200)
                neighbors.emplace_back(d,j);
        }
        std::sort(neighbors.begin(),neighbors.end());
        if(neighbors.size()>cap_neighbors)neighbors.resize(cap_neighbors);
        if(neighbors.size()<4)continue;
        double spread=0,variation=0;
        for(const auto& item:neighbors){spread+=item.first;variation+=std::abs(std::log(blobs[5*item.second+4]/area));}
        double sz_factor = prefer_8 ? (neighbors.size() == 8 ? 8.0 : (neighbors.size() < 8 ? neighbors.size() : 8.0 - (neighbors.size()-8)*0.5)) : neighbors.size();
        const double score=10000*sz_factor-spread/neighbors.size()-100*variation-area;
        if(best.ids.empty()||score>best.score){best.score=score;best.ids.clear();for(auto item:neighbors)best.ids.push_back(item.second);}
    }
    std::sort(best.ids.begin(),best.ids.end());
    std::vector<double> pixels;
    for(auto j:best.ids){pixels.push_back(blobs[5*j]);pixels.push_back(blobs[5*j+1]);}
    return pixels;
}

int main() {
    std::vector<double> blobs;
    for(int i=0;i<60;++i)blobs.insert(blobs.end(),{double(20+(i%10)*100),double(20+(i/10)*130),1.5,255,7});
    std::vector<double> ring;
    for(int i=0;i<8;++i){
        const double a=i*3.141592653589793/4,x=505+18*std::cos(a),y=475+18*std::sin(a);
        blobs.insert(blobs.end(),{x,y,1.5,220,7});ring.insert(ring.end(),{x,y});
    }
    
    cout << "Test with cap_neighbors=8:\n";
    auto p8 = compact_led_cluster_test(blobs, 8, false);
    cout << "p8 size: " << p8.size() << " equal to ring? " << (p8 == ring) << "\n";
    
    cout << "Test with cap_neighbors=12, prefer_8=true:\n";
    auto p12 = compact_led_cluster_test(blobs, 12, true);
    cout << "p12 size: " << p12.size() << " equal to ring? " << (p12 == ring) << "\n";

    return 0;
}
