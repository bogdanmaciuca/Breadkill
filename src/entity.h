#pragma once

#include <array>
#include "physics.h"
#include "renderer.h"
#include "platform.h"

constexpr float PLAYER_FORCE      = 0.008f;
constexpr float ENEMY_FORCE       = 0.0004f;
constexpr float BULLET_RADIUS     = 0.10f;
constexpr float BULLET_SPEED_COEF = 1.0f;

class Entity {
public:
    Entity(Platform& platform, Physics& physics, unsigned int texIdx);
    virtual void Render() = 0;
    virtual void Update() = 0;
protected:
    Platform &m_platformRef;
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
    Player(Platform& platform, Physics& physics, unsigned int texIdx);
    void Render() override;
    void Update() override;
    b2Vec2 GetPosition();
    void ApplyImpulse(float x, float y);
private:
    PhysicsSoftBody m_physicsObject;
};

class Wall : public Entity {
public:
    Wall(Platform& platform, Physics& physics, unsigned int texIdx, b2Vec2 pos, b2Vec2 size);
    void Render() override;
    void Update() override {}
private:
    PhysicsRigidBox m_physicsObject;
};

class Enemy : public Entity {
public:
    Enemy(Platform& platform, Physics& physics, unsigned int texIdx, b2Vec2 pos);
    void Render() override;
    void Update() override;
private:
    PhysicsSoftBody m_physicsObject;
};

class Bullet : public Entity {
public:
    Bullet(Platform& platform, Physics& physics, unsigned int texIdx, b2Vec2 pos, b2Vec2 dir);
    void Render() override;
    void Update() override {};
private:
    PhysicsRigidCircle m_physicsObject;
};

class EntityFactory {
public:
    EntityFactory(Platform& platform, Physics& physics);
    Entity* MakePlayer();
    Entity* MakeEnemy(b2Vec2 pos);
    Entity* MakeWall(b2Vec2 pos, b2Vec2 size);
    Entity* MakeBullet(b2Vec2 pos, b2Vec2 dir);
private:
    Platform& m_platformRef;
    Physics& m_physicsRef;
    float m_playerForce;
    float m_enemyForce;
    float m_bulletRadius;
    float m_bulletSpeedCoef;
};
