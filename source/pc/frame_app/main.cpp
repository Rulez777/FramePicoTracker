/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
#include "frame_overlay.h"
#include <cairo/cairo.h>
#include <curl/curl.h>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctime>
#include <unistd.h>
#include <json-c/json.h>

static json_object* field(json_object* object,const char* key){json_object* value=nullptr;if(object)json_object_object_get_ex(object,key,&value);return value;}
static bool flag(json_object* object,const char* key){auto* value=field(object,key);return value&&json_object_get_type(value)==json_type_boolean&&json_object_get_boolean(value);}
static std::string stringField(json_object* object,const char* key){auto* value=field(object,key);return value&&json_object_get_type(value)==json_type_string?json_object_get_string(value):"Unknown";}

static volatile sig_atomic_t stop=0, show=0;
static void signalHandler(int s){if(s==SIGUSR1)show=1;else stop=1;}
static size_t receive(char* data,size_t a,size_t b,void* output){static_cast<std::string*>(output)->append(data,a*b);return a*b;}
static std::string api(const std::string& route){
    if(route=="/service/restart"){
        int code=std::system("systemctl --user restart frame-pico-api.service");
        if(code!=0)return "API restart failed. Check API service status.";
        return "API service restarted";
    }
    CURL* c=curl_easy_init(); if(!c)return "API unavailable";
    std::string response;
    curl_easy_setopt(c,CURLOPT_URL,("http://127.0.0.1:18741"+route).c_str());
    curl_easy_setopt(c,CURLOPT_WRITEFUNCTION,receive);curl_easy_setopt(c,CURLOPT_WRITEDATA,&response);
    curl_easy_setopt(c,CURLOPT_TIMEOUT,12L);curl_easy_setopt(c,CURLOPT_NOSIGNAL,1L);
    if(route!="/status")curl_easy_setopt(c,CURLOPT_POSTFIELDS,"");
    auto error=curl_easy_perform(c);long code=0;curl_easy_getinfo(c,CURLINFO_RESPONSE_CODE,&code);curl_easy_cleanup(c);
    if(error!=CURLE_OK)return std::string("Connection error: ")+curl_easy_strerror(error);
    if(code!=200&&code!=202)return "API error "+std::to_string(code)+": "+response;
    if(route=="/radio/start")return "Tracking station start requested. Check live connection status.";
    return response;
}
static void text(cairo_t* c,double x,double y,double size,const std::string& value){
    cairo_set_font_size(c,size);cairo_move_to(c,x,y);cairo_show_text(c,value.c_str());
}
int main(int argc,char** argv){
    if(argc>1&&std::string(argv[1])=="--probe")return VrOverlay::probe();
    int lock=open("/home/steamos/pico-ram-work/frame-pico-ui.lock",O_CREAT|O_RDWR,0600);
    if(lock<0)return 1;
    if(flock(lock,LOCK_EX|LOCK_NB)!=0){FILE* f=fopen("/home/steamos/pico-ram-work/frame-pico-ui.pid","r");int pid=0;if(f){if(fscanf(f,"%d",&pid)==1&&pid>1)kill(pid,SIGUSR1);fclose(f);}close(lock);return 0;}
    FILE* pf=fopen("/home/steamos/pico-ram-work/frame-pico-ui.pid","w");if(pf){fprintf(pf,"%d\n",getpid());fclose(pf);}
    signal(SIGINT,signalHandler);signal(SIGTERM,signalHandler);signal(SIGUSR1,signalHandler);
    curl_global_init(CURL_GLOBAL_DEFAULT);
    VrOverlay overlay;std::string error;
    if(overlay.connect(1200,700,error)!=VrOverlay::ConnectResult::Ok){fprintf(stderr,"%s\n",error.c_str());return 1;}
    std::vector<unsigned char> rgba(1200*700*4),thumb(128*128*4,255);
    if(!overlay.submitThumbnail(thumb.data(),128)){fprintf(stderr,"Could not submit dashboard thumbnail\n");overlay.shutdown();curl_global_cleanup();close(lock);return 1;}
    overlay.showPanel();
    auto* surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,1200,700);auto* c=cairo_create(surface);
    std::string state="Connecting to diagnostic agent...",result="";
    std::future<std::string> pending=std::async(std::launch::async,api,std::string("/status"));
    bool recordRequest=false;auto refresh=std::chrono::steady_clock::now();
    bool savedPreview=false,uiFailed=false;
    double mouseX=-1,mouseY=-1;
    std::string queuedRoute;
    std::vector<std::string> updateRoutes;
    bool gameAvailable=false,serviceAvailable=false;
    auto hovered=[&](double x,double y,double w,double h){return mouseX>=x&&mouseX<=x+w&&mouseY>=y&&mouseY<=y+h;};
    auto button=[&](double x,double y,double w,double h,bool close=false){
        bool hover=hovered(x,y,w,h);
        cairo_set_source_rgb(c,close?(hover?.55:.30):(hover?.18:.10),close?.15:(hover?.62:.42),close?.17:(hover?.82:.60));
        cairo_rectangle(c,x,y,w,h);cairo_fill(c);
        if(hover){cairo_set_source_rgb(c,.75,.95,1);cairo_set_line_width(c,2);cairo_rectangle(c,x+1,y+1,w-2,h-2);cairo_stroke(c);}
    };
    while(!stop&&overlay.steamVrAlive()){
        auto events=overlay.pollEvents();if(events.quit||events.closeRequested)break;
        if(show){show=0;overlay.showPanel();}
        if(pending.valid()&&pending.wait_for(std::chrono::milliseconds(0))==std::future_status::ready){
            auto value=pending.get();if(recordRequest){result=value;recordRequest=false;}else state=value;
        }
        for(auto p:events.pointer){
            if(p.type==PointerInput::Type::Leave){mouseX=mouseY=-1;continue;}
            mouseX=p.x;mouseY=p.y;
            if(p.type!=PointerInput::Type::Up)continue;
            if(hovered(40,180,270,80)&&gameAvailable)queuedRoute="/profiles/game";
            if(hovered(330,180,270,80)&&serviceAvailable)queuedRoute="/profiles/service";
            if(hovered(650,180,240,80))queuedRoute="/service/restart";
            for(size_t i=0;i<updateRoutes.size();++i)if(!updateRoutes[i].empty()&&hovered(940,310+i*65,210,55))queuedRoute=updateRoutes[i];
            if(hovered(950,600,210,60))stop=1;
        }
        if(!queuedRoute.empty()&&!pending.valid()){
            recordRequest=true;
            result=queuedRoute=="/service/restart"?"Restarting API service...":queuedRoute=="/profiles/game"?"Selecting game radio...":queuedRoute=="/profiles/service"?"Selecting service radio...":"Preparing firmware update...";
            pending=std::async(std::launch::async,api,queuedRoute);queuedRoute.clear();
        }
        auto now=std::chrono::steady_clock::now();
        if(!pending.valid()&&now-refresh>std::chrono::seconds(2)){pending=std::async(std::launch::async,api,std::string("/status"));refresh=now;}
        cairo_set_source_rgb(c,.04,.07,.10);cairo_paint(c);cairo_select_font_face(c,"Sans",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_source_rgb(c,.9,.95,1);text(c,40,65,42,"FramePicoTracker");
        text(c,40,110,24,"Connected trackers");
        auto* appStatus=json_tokener_parse(state.c_str());
        auto* app=field(appStatus,"application");auto* profiles=field(app,"profiles");
        gameAvailable=flag(field(profiles,"game"),"available");serviceAvailable=flag(field(profiles,"service"),"available");
        button(40,180,270,80);
        cairo_set_source_rgb(c,gameAvailable?1:.5,gameAvailable?1:.5,gameAvailable?1:.5);text(c,60,230,23,"Game radio / mode 1");
        button(330,180,270,80);
        cairo_set_source_rgb(c,serviceAvailable?1:.5,serviceAvailable?1:.5,serviceAvailable?1:.5);text(c,348,230,23,"Service / mode 0");
        button(650,180,240,80);
        cairo_set_source_rgb(c,1,1,1);text(c,667,230,22,"Restart API");
        auto* trackerRows=field(app,"trackers");updateRoutes.clear();int visible=0;
        if(trackerRows&&json_object_get_type(trackerRows)==json_type_array){
            for(size_t i=0;i<json_object_array_length(trackerRows)&&i<3;++i){
                auto* peer=json_object_array_get_idx(trackerRows,i);
                auto id=stringField(peer,"id_le");
                text(c,40,340+visible*65,24,"Tracker "+id+"  "+stringField(peer,"mode_status"));
                text(c,40,365+visible*65,18,stringField(peer,"status"));
                bool updateRequired=flag(peer,"update_required"),updateAvailable=flag(peer,"update_available");
                updateRoutes.push_back(updateAvailable?"/trackers/"+id+"/update":"");
                if(updateRequired){button(940,310+visible*65,210,55);cairo_set_source_rgb(c,updateAvailable?1:.5,updateAvailable?1:.5,updateAvailable?1:.5);text(c,956,345+visible*65,19,"Update firmware");}
                ++visible;
            }
        }
        if(!visible)text(c,40,350,25,"No fresh detected trackers");
        if(!gameAvailable)text(c,40,470,17,"Game: "+stringField(field(profiles,"game"),"reason"));
        if(!serviceAvailable)text(c,40,495,17,"Service: "+stringField(field(profiles,"service"),"reason"));
        if(appStatus)json_object_put(appStatus);
        if(!result.empty())text(c,40,530,18,result.substr(0,105));
        text(c,40,645,18,"Closing this window leaves the radio service running.");
        button(950,600,210,60,true);
        cairo_set_source_rgb(c,1,1,1);text(c,990,640,24,"Close");
        cairo_surface_flush(surface);auto* pixels=cairo_image_surface_get_data(surface);
        if(!savedPreview&&!pending.valid()){
            cairo_surface_write_to_png(surface,"/home/steamos/pico-ram-work/frame-pico-ui-preview.png");savedPreview=true;
        }
        for(size_t i=0;i<rgba.size();i+=4){rgba[i]=pixels[i+2];rgba[i+1]=pixels[i+1];rgba[i+2]=pixels[i];rgba[i+3]=pixels[i+3];}
        auto repair=overlay.ensureOverlay();
        if(repair==OverlayRepair::Failed){fprintf(stderr,"Dashboard overlay unavailable\n");uiFailed=true;break;}
        if(repair==OverlayRepair::Repaired && !overlay.submitThumbnail(thumb.data(),128)){fprintf(stderr,"Could not restore dashboard thumbnail\n");uiFailed=true;break;}
        if(!overlay.submitPanel(rgba.data())){fprintf(stderr,"Could not submit dashboard panel\n");uiFailed=true;break;}
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if(pending.valid())pending.wait();
    cairo_destroy(c);cairo_surface_destroy(surface);overlay.shutdown();curl_global_cleanup();close(lock);return uiFailed?1:0;
}
