#pragma once

#include <array>
#include <variant>
#include "physics.h"
#include "renderer.h"
#include "platform.h"

class Entity {
public:
    Entity(Platform& platform, Renderer& renderer, Physics& physics, unsigned int texIdx);
    virtual void Render() = 0;
    virtual void Update() = 0;
protected:
    Platform &m_platformRef;
    Renderer &m_rendererRef;
    Physics &m_physicsRef;
    unsigned int m_texIdx;
};

constexpr std::array<b2Vec2, 6> g_softbodyVertices = {
    b2Vec2{ -0.25f, -0.30f },
    b2Vec2{ +0.25f, -0.30f },
    b2Vec2{ +0.25f, +0.00f },
    b2Vec2{ +0.25f, +0.30f },
    b2Vec2{ -0.25f, +0.30f },
    b2Vec2{ -0.25f, +0.00f }
};
constexpr std::array<PhysicsSoftBodyJointConn, 8> g_softbodyConnections = {
    PhysicsSoftBodyJointConn{ 0, 1 },
    PhysicsSoftBodyJointConn{ 1, 2 },
    PhysicsSoftBodyJointConn{ 2, 3 },
    PhysicsSoftBodyJointConn{ 3, 4 },
    PhysicsSoftBodyJointConn{ 4, 5 },
    PhysicsSoftBodyJointConn{ 5, 0 },
    PhysicsSoftBodyJointConn{ 0, 3 },
    PhysicsSoftBodyJointConn{ 1, 4 },
};

class Player : public Entity {
public:
    Player(Platform& platform, Renderer& renderer, Physics& physics, unsigned int texIdx);
    void Render() override;
    void Update() override;
private:
    PhysicsSoftBody m_physicsObject;
};

class Wall : public Entity {
public:
    Wall(Platform& platform, Renderer& renderer, Physics& physics, unsigned int texIdx, b2Vec2 pos, b2Vec2 size);
    void Render() override;
    void Update() {}
private:
    PhysicsRigidBox m_physicsObject;
};
