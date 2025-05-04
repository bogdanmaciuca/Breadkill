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

    void RenderBox(const std::span<const b2Vec2>& vertices, unsigned int texIdx);
    void RenderCircleAsRect(b2Vec2 position, float radius, unsigned int texIdx);
    void RenderBread(const std::span<const b2Vec2>& vertices, unsigned int texIdx);
};

