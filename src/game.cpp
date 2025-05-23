#include "game.h"

#include <array>
#include <chrono>
#include <vector>
#include "entity.h"

template<class T>
class SmartPtr {
public:
    SmartPtr(T* pPtr) : m_pPtr(pPtr) {}
    SmartPtr(SmartPtr&& other) {
        m_pPtr = other.m_pPtr;
        other.m_pPtr = nullptr;
    }
    ~SmartPtr() {
        if (m_pPtr != nullptr)
            free(m_pPtr);
    }
    T* operator->() {
        return m_pPtr;
    }
    T* GetRawPtr() {
        return m_pPtr;
    }
private:
    T* m_pPtr = nullptr;
};

Game::Game() {
}

Game::~Game() {
}

void Game::Run() {
    Renderer::GetInstance().Initialize(
        m_platform.GetWindowHandle(),
        std::array<const std::string, 4>{
        "res/stone.png",
        "res/bread.png",
        "res/blackbread.png",
        "res/bullet.png"
        }
    );

    std::vector<SmartPtr<Entity>> objects;
    // Player
    objects.push_back(SmartPtr<Entity>(new Player(m_platform, m_physics, 1)));
    Player* pPlayer = dynamic_cast<Player*>(objects[0].GetRawPtr());
    // Walls
    objects.push_back(SmartPtr<Entity>(new Wall(m_platform, m_physics, 0, b2Vec2{ 5.0f, 7.0f }, b2Vec2{ 10.0f, 1.0f })));
    objects.push_back(SmartPtr<Entity>(new Wall(m_platform, m_physics, 0, b2Vec2{ 0.0f, 3.0f }, b2Vec2{ 1.0f, 7.0f })));
    objects.push_back(SmartPtr<Entity>(new Wall(m_platform, m_physics, 0, b2Vec2{ 10.0f, 3.0f }, b2Vec2{ 1.0f, 7.0f })));
    objects.push_back(SmartPtr<Entity>(new Wall(m_platform, m_physics, 0, b2Vec2{ 5.0f, 3.0f }, b2Vec2{ 6.0f, 1.0f })));
    // Enemies
    objects.push_back(SmartPtr<Entity>(new Enemy(m_platform, m_physics, 2, b2Vec2{ 4.0f, 1.0f })));
    objects.push_back(SmartPtr<Entity>(new Enemy(m_platform, m_physics, 2, b2Vec2{ 6.0f, 1.0f })));

    constexpr float physicsTimestep = 1/60.0f;

    bool clickIsPressed = true, clickHasBeenReleased = true;

    auto then = std::chrono::steady_clock::now();
    while (!m_platform.WindowShouldClose()) {
        auto now = std::chrono::steady_clock::now();
        float sinceLastStep = std::chrono::duration<float>(now - then).count();

        // Event handling
        m_platform.HandleEvents();

        // Object spawning logic
        float mouseX, mouseY;
        m_platform.GetMousePosition(&mouseX, &mouseY, &clickIsPressed);
        int wndWidth, wndHeight;
        m_platform.GetWindowSize(&wndWidth, &wndHeight);
        mouseX *= 10.0f / (float)wndWidth;
        mouseY *= 10.0f / (float)wndWidth;
        if (clickIsPressed && clickHasBeenReleased == true) {
            pPlayer->ApplyImpulse(
                PLAYER_FORCE * -(mouseX - pPlayer->GetPosition().x),
                PLAYER_FORCE * -(mouseY - pPlayer->GetPosition().y)
            );
            // Bullet dir
            b2Vec2 bulletDir = b2MulSV(1.0f, b2Normalize(b2Sub(b2Vec2{ .x = mouseX, .y = mouseY }, pPlayer->GetPosition())));
            objects.push_back(SmartPtr<Entity>(new Bullet(
                    m_platform, m_physics, 3,
                    pPlayer->GetPosition() + bulletDir,
                    bulletDir
            )));
            clickHasBeenReleased = false;
        }
        else if (!clickIsPressed) {
            clickHasBeenReleased = true;
        }

        // Physics
        if (sinceLastStep > physicsTimestep) {
            m_physics.Update(sinceLastStep);
            then = now;
        }

        // Update and render
        for (auto& object : objects) {
            object->Update();
            object->Render();
        }

        Renderer::GetInstance().RenderScene();
    }
    Renderer::GetInstance().Release();
}

