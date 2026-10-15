/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors.
 * Independent project implementation using the public OpenVR and EGL APIs.
 */
#include "frame_overlay.h"
#include <openvr.h>
#include <EGL/egl.h>
#include <GL/gl.h>
#include <chrono>
#include <cstdio>
#include <thread>

namespace {
constexpr const char* key="pico.frame-tracker";
bool overlayError(vr::EVROverlayError result,const char* operation) {
    if(result==vr::VROverlayError_None)return false;
    std::fprintf(stderr,"%s: %s\n",operation,vr::VROverlay()->GetOverlayErrorNameFromEnum(result));
    return true;
}
}

struct VrOverlay::State {
    bool connected=false;
    int width=0,height=0;
    vr::VROverlayHandle_t panel=vr::k_ulOverlayHandleInvalid,thumbnail=vr::k_ulOverlayHandleInvalid;
    EGLDisplay display=EGL_NO_DISPLAY;
    EGLContext context=EGL_NO_CONTEXT;
    EGLSurface surface=EGL_NO_SURFACE;
    GLuint textures[2]={0,0};

    bool create(std::string& error) {
        auto* api=vr::VROverlay();
        auto result=api->CreateDashboardOverlay(key,"FramePicoTracker",&panel,&thumbnail);
        if(result!=vr::VROverlayError_None){error=api->GetOverlayErrorNameFromEnum(result);return false;}
        vr::HmdVector2_t scale={{static_cast<float>(width),static_cast<float>(height)}};
        if(overlayError(api->SetOverlayWidthInMeters(panel,1.6f),"overlay width") ||
           overlayError(api->SetOverlayInputMethod(panel,vr::VROverlayInputMethod_Mouse),"overlay input") ||
           overlayError(api->SetOverlayMouseScale(panel,&scale),"overlay mouse scale")) {
            error="Could not configure dashboard overlay";return false;
        }
        return true;
    }

    bool upload(vr::VROverlayHandle_t handle,GLuint texture,const uint8_t* rgba,int w,int h) {
        if(!connected || !rgba || w<=0 || h<=0)return false;
        if(!eglMakeCurrent(display,surface,surface,context))return false;
        glBindTexture(GL_TEXTURE_2D,texture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
        if(glGetError()!=GL_NO_ERROR){std::fprintf(stderr,"Overlay texture upload failed\n");return false;}
        // Complete writes before the runtime copies from this application's texture.
        glFinish();
        vr::Texture_t data={reinterpret_cast<void*>(static_cast<uintptr_t>(texture)),vr::TextureType_OpenGL,vr::ColorSpace_Gamma};
        auto result=vr::VROverlay()->SetOverlayTexture(handle,&data);
        return !overlayError(result,"SetOverlayTexture");
    }
};

VrOverlay::VrOverlay():state_(std::make_unique<State>()){}
VrOverlay::~VrOverlay(){shutdown();}
VrOverlay::ConnectResult VrOverlay::connect(int width,int height,std::string& error) {
    shutdown();
    if(width<=0 || height<=0){error="Invalid panel dimensions";return ConnectResult::Error;}
    if(!vr::VR_IsRuntimeInstalled() || !vr::VR_IsHmdPresent()){error="SteamVR headset unavailable";return ConnectResult::NotRunning;}
    vr::EVRInitError init=vr::VRInitError_None;
    // Background initialization does not request launching SteamVR.
    vr::VR_Init(&init,vr::VRApplication_Background);
    if(init!=vr::VRInitError_None){error=vr::VR_GetVRInitErrorAsEnglishDescription(init);return ConnectResult::NotRunning;}
    vr::VR_Shutdown();
    vr::VR_Init(&init,vr::VRApplication_Overlay);
    if(init!=vr::VRInitError_None){error=vr::VR_GetVRInitErrorAsEnglishDescription(init);return ConnectResult::Error;}
    state_->connected=true;state_->width=width;state_->height=height;
    if(!vr::VROverlay()){error="OpenVR overlay interface unavailable";shutdown();return ConnectResult::Error;}
    state_->display=eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint major=0,minor=0;
    if(state_->display==EGL_NO_DISPLAY || !eglInitialize(state_->display,&major,&minor) || !eglBindAPI(EGL_OPENGL_API)) {
        error="Cannot initialize EGL desktop OpenGL on the current display";shutdown();return ConnectResult::Error;
    }
    EGLint attributes[]={EGL_SURFACE_TYPE,EGL_PBUFFER_BIT,EGL_RENDERABLE_TYPE,EGL_OPENGL_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_ALPHA_SIZE,8,EGL_NONE};
    EGLConfig config=nullptr;EGLint count=0;
    if(!eglChooseConfig(state_->display,attributes,&config,1,&count) || count!=1) {
        error="No EGL OpenGL pbuffer configuration";shutdown();return ConnectResult::Error;
    }
    EGLint size[]={EGL_WIDTH,1,EGL_HEIGHT,1,EGL_NONE};
    state_->surface=eglCreatePbufferSurface(state_->display,config,size);
    state_->context=eglCreateContext(state_->display,config,EGL_NO_CONTEXT,nullptr);
    if(state_->surface==EGL_NO_SURFACE || state_->context==EGL_NO_CONTEXT || !eglMakeCurrent(state_->display,state_->surface,state_->surface,state_->context)) {
        error="Cannot create current EGL OpenGL context";shutdown();return ConnectResult::Error;
    }
    glGenTextures(2,state_->textures);
    if(!state_->textures[0] || !state_->textures[1] || !state_->create(error)){shutdown();return ConnectResult::Error;}
    return ConnectResult::Ok;
}
void VrOverlay::shutdown() {
    if(!state_)return;
    auto& s=*state_;
    if(s.connected && vr::VROverlay()) {
        for(auto handle:{s.panel,s.thumbnail})if(handle!=vr::k_ulOverlayHandleInvalid) {
            overlayError(vr::VROverlay()->ClearOverlayTexture(handle),"clear overlay");
            overlayError(vr::VROverlay()->DestroyOverlay(handle),"destroy overlay");
        }
        // Let the runtime release its texture references before GL resources go away.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if(s.connected)vr::VR_Shutdown();
    if(s.display!=EGL_NO_DISPLAY) {
        if(s.context!=EGL_NO_CONTEXT && eglMakeCurrent(s.display,s.surface,s.surface,s.context)) {
            glFinish();glDeleteTextures(2,s.textures);
        }
        eglMakeCurrent(s.display,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
        if(s.context!=EGL_NO_CONTEXT)eglDestroyContext(s.display,s.context);
        if(s.surface!=EGL_NO_SURFACE)eglDestroySurface(s.display,s.surface);
        eglTerminate(s.display);
    }
    state_=std::make_unique<State>();
}
VrEvents VrOverlay::pollEvents() {
    VrEvents result;
    if(!state_->connected || !vr::VROverlay()){result.quit=true;return result;}
    vr::VREvent_t event{};
    while(vr::VRSystem()->PollNextEvent(&event,sizeof(event)))if(event.eventType==vr::VREvent_Quit){result.quit=true;vr::VRSystem()->AcknowledgeQuit_Exiting();}
    while(vr::VROverlay()->PollNextOverlayEvent(state_->panel,&event,sizeof(event))) {
        switch(event.eventType) {
        case vr::VREvent_Quit:result.quit=true;vr::VRSystem()->AcknowledgeQuit_Exiting();break;
        case vr::VREvent_OverlayClosed:result.closeRequested=true;break;
        case vr::VREvent_MouseMove:case vr::VREvent_MouseButtonDown:case vr::VREvent_MouseButtonUp:
            if(event.eventType!=vr::VREvent_MouseMove && event.data.mouse.button!=vr::VRMouseButton_Left)break;
            result.pointer.push_back({event.eventType==vr::VREvent_MouseMove?PointerInput::Type::Move:event.eventType==vr::VREvent_MouseButtonDown?PointerInput::Type::Down:PointerInput::Type::Up,event.data.mouse.x,state_->height-event.data.mouse.y});break;
        case vr::VREvent_FocusLeave:result.pointer.push_back({PointerInput::Type::Leave,-1,-1});break;
        default:break;
        }
    }
    return result;
}
bool VrOverlay::steamVrAlive() const {return state_->connected && vr::VRSystem() && vr::VROverlay();}
void VrOverlay::showPanel(){if(state_->connected)vr::VROverlay()->ShowDashboard(key);}
bool VrOverlay::submitThumbnail(const uint8_t* rgba,int size){return state_->upload(state_->thumbnail,state_->textures[1],rgba,size,size);}
bool VrOverlay::submitPanel(const uint8_t* rgba){return state_->upload(state_->panel,state_->textures[0],rgba,state_->width,state_->height);}
OverlayRepair VrOverlay::ensureOverlay() {
    if(!state_->connected || !vr::VROverlay())return OverlayRepair::Failed;
    vr::VROverlayHandle_t found=vr::k_ulOverlayHandleInvalid;
    auto error=vr::VROverlay()->FindOverlay(key,&found);
    if(error==vr::VROverlayError_None)return found==state_->panel?OverlayRepair::Unchanged:OverlayRepair::Failed;
    if(error!=vr::VROverlayError_UnknownOverlay)return OverlayRepair::Failed;
    // Remove only this instance's old handles. Never destroy a foreign key owner.
    for(auto handle:{state_->panel,state_->thumbnail})if(handle!=vr::k_ulOverlayHandleInvalid)vr::VROverlay()->DestroyOverlay(handle);
    state_->panel=state_->thumbnail=vr::k_ulOverlayHandleInvalid;
    std::string message;
    if(!state_->create(message)){std::fprintf(stderr,"Overlay repair failed: %s\n",message.c_str());return OverlayRepair::Failed;}
    return OverlayRepair::Repaired;
}
int VrOverlay::probe() {
    vr::EVRInitError error=vr::VRInitError_None;vr::VR_Init(&error,vr::VRApplication_Background);
    if(error!=vr::VRInitError_None)return 1;
    vr::VROverlayHandle_t handle=vr::k_ulOverlayHandleInvalid;
    bool found=vr::VROverlay() && vr::VROverlay()->FindOverlay(key,&handle)==vr::VROverlayError_None;
    std::printf("FramePicoTracker overlay: %s\n",found?"present":"absent");vr::VR_Shutdown();return found?0:1;
}
