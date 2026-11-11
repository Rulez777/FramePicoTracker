// Read-only SteamVR view capture: an OpenVR *overlay* application (submits nothing, shows nothing in the headset) that
// copies the compositor mirror texture (what the HMD shows, incl. compositor-drawn device render models) to PPM files.
// Usage: mirror_grab <out_dir> <count> <interval_ms> [scale]
// Also prints, per frame, the tracked generic trackers (index, pose valid, position) so an image can be matched to poses.
#include <EGL/egl.h>
#include <GL/gl.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <cstring>
#include <ctime>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "openvr.h"
#include "pose_wire.h"

static bool init_egl() {
    EGLDisplay d = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (d == EGL_NO_DISPLAY || !eglInitialize(d, nullptr, nullptr)) return false;
    const EGLint cfg_attr[] = {EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT, EGL_NONE};
    EGLConfig cfg; EGLint n = 0;
    if (!eglChooseConfig(d, cfg_attr, &cfg, 1, &n) || n < 1) return false;
    const EGLint pb[] = {EGL_WIDTH, 16, EGL_HEIGHT, 16, EGL_NONE};
    EGLSurface s = eglCreatePbufferSurface(d, cfg, pb);
    if (!eglBindAPI(EGL_OPENGL_API)) return false;
    EGLContext c = eglCreateContext(d, cfg, EGL_NO_CONTEXT, nullptr);
    return c != EGL_NO_CONTEXT && eglMakeCurrent(d, s, s, c);
}

int main(int argc, char **argv) {
    if (argc < 4) { fprintf(stderr, "usage: mirror_grab out_dir count interval_ms [scale]\n"); return 2; }
    std::string out = argv[1]; int count = atoi(argv[2]), interval = atoi(argv[3]), scale = argc > 4 ? atoi(argv[4]) : 2;
    if (!init_egl()) { fprintf(stderr, "egl init failed 0x%x\n", eglGetError()); return 3; }
    printf("GL %s | %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));
    vr::EVRInitError e = vr::VRInitError_None;
    vr::IVRSystem *sys = vr::VR_Init(&e, vr::VRApplication_Overlay);
    if (e != vr::VRInitError_None) { fprintf(stderr, "VR_Init %d\n", (int)e); return 4; }
    printf("hmd activity %d scene pid %u fullscreen %d\n", (int)sys->GetTrackedDeviceActivityLevel(vr::k_unTrackedDeviceIndex_Hmd),
           vr::VRCompositor()->GetCurrentSceneFocusProcess(), vr::VRCompositor()->IsFullscreen());
    {
        vr::HmdColor_t fc = vr::VRCompositor()->GetCurrentFadeColor(false), bc = vr::VRCompositor()->GetCurrentFadeColor(true);
        printf("fade fg %.2f %.2f %.2f a%.2f bg a%.2f grid alpha %.2f\n", fc.r, fc.g, fc.b, fc.a, bc.a,
               vr::VRCompositor()->GetCurrentGridAlpha());
    }
    if (const char *awake = getenv("BENCH_AWAKE")) {  // temporary bench override: 1 = keep compositor awake, 0 = restore defaults
        vr::EVRSettingsError se = vr::VRSettingsError_None;
        if (awake[0] == '1') {
            vr::VRSettings()->SetBool(vr::k_pch_Power_Section, vr::k_pch_Power_PauseCompositorOnStandby_Bool, false, &se);
            // turnOffScreensTimeout is owned by the pipeline power tool (range 0..600, sets 300 during runs): never touch it here.
        } else {
            vr::VRSettings()->RemoveKeyInSection(vr::k_pch_Power_Section, vr::k_pch_Power_PauseCompositorOnStandby_Bool, &se);
        }
        // Screen saving on the bench: the OLED stays on (XRService stops the cameras when the screens turn off),
        // but shows black, so its pixels are off. 0 removes the black fade again.
        if (awake[0] == '1' || awake[0] == '0') {
            const float a = awake[0] == '1' ? 1.f : 0.f;
            vr::VRCompositor()->FadeToColor(0.5f, 0, 0, 0, a, false);
            vr::VRCompositor()->FadeToColor(0.5f, 0, 0, 0, a, true);
        }
        // The compositor drops this app's fade when the app exits, so BENCH_HOLD=<seconds> keeps the black
        // screen for the length of a bench run (refreshed every second), then lets it go.
        if (const char *hold = getenv("BENCH_HOLD")) {
            const int seconds = atoi(hold);
            for (int s = 0; awake[0] == '1' && s < seconds; ++s) {
                vr::VRCompositor()->FadeToColor(0.f, 0, 0, 0, 1.f, false);
                vr::VRCompositor()->FadeToColor(0.f, 0, 0, 0, 1.f, true);
                sleep(1);
            }
        }
        if (awake[0] == 'r')  // one-off repair of the 3600 s timeout written by an earlier version of this tool
            vr::VRSettings()->RemoveKeyInSection(vr::k_pch_Power_Section, vr::k_pch_Power_TurnOffScreensTimeout_Float, &se);
        printf("bench awake=%c err %d pause=%d timeout=%.0f\n", awake[0], (int)se,
               vr::VRSettings()->GetBool(vr::k_pch_Power_Section, vr::k_pch_Power_PauseCompositorOnStandby_Bool),
               vr::VRSettings()->GetFloat(vr::k_pch_Power_Section, vr::k_pch_Power_TurnOffScreensTimeout_Float));
    }
    if (const char *serialWanted = getenv("POSE_LOG")) {  // POSE_LOG=<serial>: <count> polls every <interval> ms
        for (int k = 0; k < count; ++k) {
            vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
            sys->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, poses, vr::k_unMaxTrackedDeviceCount);
            timespec ts{}; clock_gettime(CLOCK_MONOTONIC, &ts);
            for (unsigned i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
                if (!getenv("POSE_ANY") && sys->GetTrackedDeviceClass(i) != vr::TrackedDeviceClass_GenericTracker) continue;  // POSE_ANY: controllers too
                char serial[64] = {};
                sys->GetStringTrackedDeviceProperty(i, vr::Prop_SerialNumber_String, serial, sizeof(serial));
                if (strcmp(serial, serialWanted)) continue;
                const auto &m = poses[i].mDeviceToAbsoluteTracking.m;
                printf("P %.6f %d %d %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f\n", ts.tv_sec + ts.tv_nsec / 1e9,
                       poses[i].bPoseIsValid, (int)poses[i].eTrackingResult, m[0][3], m[1][3], m[2][3], m[0][0], m[0][1], m[0][2],
                       m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);
                fflush(stdout);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
        vr::VR_Shutdown();
        return 0;
    }
    if (const char *rm = getenv("RENDERMODEL")) {  // diagnose render model loading by name
        vr::RenderModel_t *model = nullptr; vr::EVRRenderModelError re = vr::VRRenderModelError_Loading;
        for (int i = 0; i < 100 && re == vr::VRRenderModelError_Loading; ++i) {
            re = vr::VRRenderModels()->LoadRenderModel_Async(rm, &model);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        char path[512] = {}; vr::EVRRenderModelError pe = vr::VRRenderModelError_None;
        vr::VRRenderModels()->GetRenderModelOriginalPath(rm, path, sizeof(path), &pe);
        printf("rendermodel %s err %d (%s) verts %u tris %u path '%s'\n", rm, (int)re,
               vr::VRRenderModels()->GetRenderModelErrorNameFromEnum(re), model ? model->unVertexCount : 0,
               model ? model->unTriangleCount : 0, path);
        for (unsigned i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
            if (sys->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_GenericTracker) {
                char name[256] = {};
                sys->GetStringTrackedDeviceProperty(i, vr::Prop_RenderModelName_String, name, sizeof(name));
                printf("device %u render model '%s'\n", i, name);
            }
        if (model) vr::VRRenderModels()->FreeRenderModel(model);
        vr::VR_Shutdown();
        return 0;
    }
    if (getenv("SYNTHETIC_POSE")) {  // VISUAL-CHAIN TEST ONLY: fake chip, pose placed 0.35 m in front of the HMD
        vr::TrackedDevicePose_t hp[1];
        sys->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseRawAndUncalibrated, 0, hp, 1);
        const auto &m = hp[0].mDeviceToAbsoluteTracking.m;
        float pos[3] = {m[0][3] - 0.35f * m[0][2], m[1][3] - 0.35f * m[1][2], m[2][3] - 0.35f * m[2][2]};
        int s = socket(AF_INET, SOCK_DGRAM, 0);
        sockaddr_in a{}; a.sin_family = AF_INET; a.sin_port = htons(39571); a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        for (int i = 0; i < 30 * interval / 1000; ++i) {
            timespec ts{}; clock_gettime(CLOCK_MONOTONIC, &ts);
            PoseFrameWire p{}; memcpy(p.magic, "PTP2", 4); p.chip = 0x7E570001u; p.flags = 3; p.sequence = i + 1;
            p.sourceTime = uint64_t(ts.tv_sec) * 1000000000ull + ts.tv_nsec;
            memcpy(p.position, pos, sizeof(pos)); p.quaternion[3] = 1; p.batteryPercent = -1; p.quality = 1;
            sendto(s, &p, sizeof(p), 0, (sockaddr *)&a, sizeof(a));
            if (i == 45) {  // after 1.5 s of updates
                vr::ScreenshotHandle_t sh = 0;
                std::string prev = out + "/synthetic-preview", vrf = out + "/synthetic-vr";
                printf("synthetic at %.3f %.3f %.3f screenshot err %d\n", pos[0], pos[1], pos[2],
                       (int)vr::VRScreenshots()->RequestScreenshot(&sh, vr::VRScreenshotType_Stereo, prev.c_str(), vrf.c_str()));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        close(s);
        vr::VR_Shutdown();
        return 0;
    }
    if (const char *shot = getenv("SCREENSHOT")) {  // compositor-side capture: SCREENSHOT=<base path>, <count> shots
        vr::EVRScreenshotError se = vr::VRScreenshotError_None;
        for (int k = 0; k < count; ++k) {
            vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
            sys->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, poses, vr::k_unMaxTrackedDeviceCount);
            timespec ts{}; clock_gettime(CLOCK_MONOTONIC, &ts);
            for (unsigned i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
                if (sys->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_GenericTracker) {
                    char serial[64] = {};
                    sys->GetStringTrackedDeviceProperty(i, vr::Prop_SerialNumber_String, serial, sizeof(serial));
                    const auto &m = poses[i].mDeviceToAbsoluteTracking.m;
                    printf("shot %d mono %.3f tracker %s valid %d result %d pos %.4f %.4f %.4f\n", k, ts.tv_sec + ts.tv_nsec / 1e9,
                           serial, poses[i].bPoseIsValid, (int)poses[i].eTrackingResult, m[0][3], m[1][3], m[2][3]);
                }
            vr::ScreenshotHandle_t sh = 0;
            std::string base = count > 1 ? std::string(shot) + "-" + std::to_string(k) : std::string(shot);
            std::string prev = base + "-preview", vrf = base + "-vr";
            se = vr::VRScreenshots()->RequestScreenshot(&sh, vr::VRScreenshotType_Stereo, prev.c_str(), vrf.c_str());
            printf("shot %d request err %d handle %u\n", k, (int)se, (unsigned)sh); fflush(stdout);
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
        vr::VR_Shutdown();
        return se == vr::VRScreenshotError_None ? 0 : 5;
    }
    bool show = getenv("MIRROR_SHOW") != nullptr;
    if (show) { vr::VRCompositor()->ShowMirrorWindow(); std::this_thread::sleep_for(std::chrono::milliseconds(1500)); printf("mirror window visible %d\n", vr::VRCompositor()->IsMirrorWindowVisible()); }
    for (int k = 0; k < count; ++k) {
        if (k) std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
        sys->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, poses, vr::k_unMaxTrackedDeviceCount);
        for (unsigned i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i) {
            vr::ETrackedDeviceClass cls = sys->GetTrackedDeviceClass(i);
            if (cls != vr::TrackedDeviceClass_GenericTracker && cls != vr::TrackedDeviceClass_HMD) continue;
            const auto &m = poses[i].mDeviceToAbsoluteTracking.m;
            printf("frame %d dev %u class %d connected %d valid %d result %d pos %.4f %.4f %.4f\n", k, i, (int)cls,
                   poses[i].bDeviceIsConnected, poses[i].bPoseIsValid, (int)poses[i].eTrackingResult, m[0][3], m[1][3], m[2][3]);
        }
        vr::glUInt_t tex = 0; vr::glSharedTextureHandle_t h = nullptr;
        vr::EVRCompositorError ce = vr::VRCompositor()->GetMirrorTextureGL(vr::Eye_Left, &tex, &h);
        if (ce != vr::VRCompositorError_None) { printf("frame %d mirror error %d\n", k, (int)ce); fflush(stdout); continue; }
        vr::VRCompositor()->LockGLSharedTextureForAccess(h);
        glBindTexture(GL_TEXTURE_2D, tex);
        GLint w = 0, hh = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &hh);
        std::vector<unsigned char> px((size_t)w * hh * 4);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, px.data());
        GLenum ge = glGetError();
        vr::VRCompositor()->UnlockGLSharedTextureForAccess(h);
        vr::VRCompositor()->ReleaseSharedGLTexture(tex, h);
        if (w <= 0 || hh <= 0 || ge != GL_NO_ERROR) { printf("frame %d bad texture %dx%d gl 0x%x\n", k, w, hh, ge); continue; }
        int ow = w / scale, oh = hh / scale; double sum = 0;
        std::string fn = out + "/mirror-" + std::to_string(k) + ".ppm";
        FILE *f = fopen(fn.c_str(), "wb");
        fprintf(f, "P6\n%d %d\n255\n", ow, oh);
        std::vector<unsigned char> row((size_t)ow * 3);
        for (int y = 0; y < oh; ++y) {
            const unsigned char *src = &px[(size_t)(hh - 1 - y * scale) * w * 4];  // GL rows are bottom-up
            for (int x = 0; x < ow; ++x) { for (int c = 0; c < 3; ++c) { row[x * 3 + c] = src[x * scale * 4 + c]; sum += row[x * 3 + c]; } }
            fwrite(row.data(), 1, row.size(), f);
        }
        fclose(f);
        printf("frame %d saved %s %dx%d mean %.2f\n", k, fn.c_str(), w, hh, sum / ((double)ow * oh * 3));
        fflush(stdout);
    }
    if (show) vr::VRCompositor()->HideMirrorWindow();
    vr::VR_Shutdown();
    return 0;
}
