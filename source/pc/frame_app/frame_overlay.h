/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors.
 */
#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct PointerInput {
    enum class Type { Move, Down, Up, Leave };
    Type type;
    double x, y;
};
struct VrEvents {
    bool quit=false, closeRequested=false;
    std::vector<PointerInput> pointer;
};
enum class OverlayRepair { Unchanged, Repaired, Failed };

class VrOverlay {
public:
    enum class ConnectResult { Ok, NotRunning, Error };
    VrOverlay();
    ~VrOverlay();
    VrOverlay(const VrOverlay&)=delete;
    VrOverlay& operator=(const VrOverlay&)=delete;
    ConnectResult connect(int width,int height,std::string& error);
    void shutdown();
    VrEvents pollEvents();
    bool steamVrAlive() const;
    void showPanel();
    bool submitThumbnail(const uint8_t* rgba,int size);
    bool submitPanel(const uint8_t* rgba);
    OverlayRepair ensureOverlay();
    static int probe();
private:
    struct State;
    std::unique_ptr<State> state_;
};
