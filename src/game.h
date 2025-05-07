#pragma once
#include <span>
#include "box2d/box2d.h"

#include "platform.h"
#include "renderer.h"
#include "physics.h"

enum {
    GAME_WND_W = 1024,
    GAME_WND_H = 768
};

class Game {
public:
    Game();
    void Run();
    ~Game();
private:
    Platform m_platform;
    Renderer m_renderer;
    Physics m_physics;
};

