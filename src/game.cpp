#include "game.h"

#include <array>
#include <chrono>
#include "SDL3/SDL.h"

Game::Game()
    : m_renderer(
        m_platform.GetWindowHandle(),
        std::array<const std::string, 2>{
        "C:/Users/Bogdan/Documents/C_Projects/breadkill/res/stone.png",
        "C:/Users/Bogdan/Documents/C_Projects/breadkill/res/bread.png"
        }
    )
{
}
Game::~Game() {
}

void Game::Run() {
    PhysicsRigidBox ground = m_physics.CreateBox(b2Vec2{5.0f, 5.0f}, b2Vec2{8.0f, 2.0f});

    constexpr std::array<b2Vec2, 6> softbodyVertices = {
        b2Vec2{ -0.25f, -0.30f },
        b2Vec2{ +0.25f, -0.30f },
        b2Vec2{ +0.25f, +0.00f },
        b2Vec2{ +0.25f, +0.30f },
        b2Vec2{ -0.25f, +0.30f },
        b2Vec2{ -0.25f, +0.00f }
    };
    constexpr std::array<PhysicsSoftBodyJointConn, 8> softbodyConnections = {
        PhysicsSoftBodyJointConn{ 0, 1 },
        PhysicsSoftBodyJointConn{ 1, 2 },
        PhysicsSoftBodyJointConn{ 2, 3 },
        PhysicsSoftBodyJointConn{ 3, 4 },
        PhysicsSoftBodyJointConn{ 4, 5 },
        PhysicsSoftBodyJointConn{ 5, 0 },
        PhysicsSoftBodyJointConn{ 0, 3 },
        PhysicsSoftBodyJointConn{ 1, 4 },
    };
    PhysicsSoftBody softbody = m_physics.CreateSoftBody(b2Vec2{ 5.0f, 1.0f }, softbodyVertices, softbodyConnections);

    constexpr float physicsTimestep = 1/60.0f;

    auto then = std::chrono::steady_clock::now();
    while (!m_platform.WindowShouldClose()) {
        auto now = std::chrono::steady_clock::now();
        float sinceLastStep = std::chrono::duration<float>(now - then).count();

        // Event handling
        m_platform.HandleEvents();

        // Input
        float mouseX, mouseY;
        bool clickIsPressed;
        m_platform.GetMousePosition(&mouseX, &mouseY, &clickIsPressed);
        int wndWidth, wndHeight;
        m_platform.GetWindowSize(&wndWidth, &wndHeight);
        mouseX *= 10.0f / (float)wndWidth;
        mouseY *= 10.0f / (float)wndWidth;
        if (clickIsPressed) {
            softbody.ApplyImpulse(
                0.001f * (mouseX - softbody.vertices[0].GetPosition().x),
                0.001f * (mouseY - softbody.vertices[0].GetPosition().y)
            );
        }

        // Physics
        if (sinceLastStep > physicsTimestep) {
            m_physics.Update(sinceLastStep);
            then = now;
        }

        // Rendering
        std::vector<b2Vec2> groundVertices = ground.GetWorldVertices();
        RenderBox(groundVertices, 0);

        std::array<b2Vec2, 6> softbodyWorldVertices;
        for (int i = 0; i < softbodyVertices.size(); i++) {
            softbodyWorldVertices[i] = softbody.vertices[i].GetPosition();
        }
        RenderBread(softbodyWorldVertices, 1);

        //for (int i = 0; i < softbodyVertices.size(); i++) {
        //    RenderCircleAsRect(softbody.vertices[i].GetPosition(), 0.03f, 0);
        //}

        m_renderer.RenderScene();
    }
}

void Game::RenderBox(const std::span<const b2Vec2>& vertices, unsigned int texIdx) {
    float width = abs(vertices[0].x - vertices[1].x) * 0.5f;
    float height = abs(vertices[1].y - vertices[2].y) * 0.5f;
    RendererTriangle triangle;
    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[1].x, .y = vertices[1].y, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .texIdx = texIdx };
    triangle.points[0].u = 0;     triangle.points[0].v = height;
    triangle.points[1].u = width; triangle.points[1].v = height;
    triangle.points[2].u = width; triangle.points[2].v = 0;
    m_renderer.PushTriangle(triangle);
    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x = vertices[3].x, .y = vertices[3].y, .texIdx = texIdx };
    triangle.points[0].u = 0;     triangle.points[0].v = height;
    triangle.points[1].u = width; triangle.points[1].v = 0;
    triangle.points[2].u = 0;     triangle.points[2].v = 0;
    m_renderer.PushTriangle(triangle);
}

void Game::RenderCircleAsRect(b2Vec2 position, float radius, unsigned int texIdx) {
    RendererTriangle triangle;
    const std::array<b2Vec2, 4> vertices = {
        b2Vec2{ .x = position.x - radius, .y = position.y - radius },
        b2Vec2{ .x = position.x + radius, .y = position.y - radius },
        b2Vec2{ .x = position.x + radius, .y = position.y + radius },
        b2Vec2{ .x = position.x - radius, .y = position.y + radius },
    };
    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[1].x, .y = vertices[1].y, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .texIdx = texIdx };
    m_renderer.PushTriangle(triangle);
    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x = vertices[3].x, .y = vertices[3].y, .texIdx = texIdx };
    m_renderer.PushTriangle(triangle);
}

void Game::RenderBread(const std::span<const b2Vec2>& vertices, unsigned int texIdx) {
    b2Vec2 center = b2Vec2_zero;
    for (const auto& v : vertices) {
        center = b2Add(center, v);
    }
    center = b2MulSV(1.0f / vertices.size(), center);

    RendererTriangle triangle;

    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .u = 0.0f, .v = 1.0f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[1].x, .y = vertices[1].y, .u = 1.0f, .v = 1.0f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    m_renderer.PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[1].x, .y = vertices[1].y, .u = 1.0f, .v = 1.0f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .u = 1.0f, .v = 0.5f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    m_renderer.PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .u = 1.0f, .v = 0.5f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[3].x, .y = vertices[3].y, .u = 1.0f, .v = 0.0f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    m_renderer.PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[3].x, .y = vertices[3].y, .u = 1.0f, .v = 0.0f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[4].x, .y = vertices[4].y, .u = 0.0f, .v = 0.0f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    m_renderer.PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[4].x, .y = vertices[4].y, .u = 0.0f, .v = 0.0f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[5].x, .y = vertices[5].y, .u = 0.0f, .v = 0.5f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    m_renderer.PushTriangle(triangle);
    
    triangle.points[0] = RendererVertex{ .x = vertices[5].x, .y = vertices[5].y, .u = 0.0f, .v = 0.5f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .u = 0.0f, .v = 1.0f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    m_renderer.PushTriangle(triangle);

}

