# Dashboard overlay backend

The current dashboard no longer depends on frame-mic-tuner. The old dependency supplied a general overlay and Vulkan texture helper, not microphone functionality or tracker research. Historical references remain for provenance and do not describe the current build.

`source/pc/frame_app/frame_overlay.cpp` and `.h` are a new project implementation. They create a SteamVR dashboard with the public OpenVR API, upload Cairo's converted RGBA pixels to desktop OpenGL textures in an EGL pbuffer context, and submit them through `SetOverlayTexture`. They do not use `SetOverlayRaw`, vendor overlay sources, or code copied from the previous helper.

The wrapper supports panel and thumbnail creation, pointer events, dashboard selection, quit handling, an overlay presence probe and recreation when its handle disappears. It refuses a foreign owner of the same overlay key. Texture errors stop the UI rather than silently displaying a stale control panel. Closing the UI still does not stop the radio service.

## Build

Install component development packages for Cairo, libcurl, json-c, EGL and desktop OpenGL, a C++17 compiler and pkg-config. Acquire official OpenVR headers separately. On the Frame Linux ARM64 build host, in a separate copy of `source/`:

```sh
export OPENVR_INCLUDE_DIR=/your/official/openvr/headers
export OPENVR_LIB_DIR=/opt/steamvr/bin/linuxarm64
bash scripts/build_frame_app.sh
```

No dependency is downloaded by the script. It builds a local executable only. It does not install, connect to Roy or start a service. The context needs an EGL display compatible with SteamVR and desktop OpenGL support. This differs from the previous Vulkan path and must be validated on the actual runtime.

## Validation boundary

Offline compilation is distinct from target validation. Before calling this replacement equivalent to the previously installed UI, check it on the intended Frame session: dashboard visibility, image orientation and colors, pointer coordinates and click routing, thumbnail behavior, repeated open and close, SteamVR quit, overlay recreation and graceful failure when the EGL display is unavailable. A successful build does not prove those behaviors.

A separate Grok review was requested but timed out without findings. It is not counted as a completed review of this replacement. The object compilation used official API headers; the complete application has not been linked or run on Frame in this change.

No device operation is authorized by this document. Source and runtime dependencies remain under their respective licenses. The frozen October 15 handoff and previously installed private application are unchanged by this public source update.
