#include "game.h"

#include <array>
#include <chrono>
#include <memory>
#include <vector>
#include "SDL3/SDL.h"
#include "entity.h"

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
    std::vector<std::unique_ptr<Entity>> objects;
    objects.push_back(std::make_unique<Player>(m_platform, m_renderer, m_physics, 1));
    objects.push_back(std::make_unique<Wall>(m_platform, m_renderer, m_physics, 0, b2Vec2{ 5.0f, 5.0f }, b2Vec2{ 8.0f, 1.0f }));

    constexpr float physicsTimestep = 1/60.0f;

    auto then = std::chrono::steady_clock::now();
    while (!m_platform.WindowShouldClose()) {
        auto now = std::chrono::steady_clock::now();
        float sinceLastStep = std::chrono::duration<float>(now - then).count();

        // Event handling
        m_platform.HandleEvents();

        // Physics
        if (sinceLastStep > physicsTimestep) {
            m_physics.Update(sinceLastStep);
            then = now;
        }

        for (auto& object : objects) {
            object->Update();
            object->Render();
        }

        m_renderer.RenderScene();
    }
}

