#pragma once
#include <vector>
#include <span>
#include "box2d/box2d.h"

enum {
    PHYSICS_SUBSTEP_COUNT = 8
};

struct PhysicsRigidBox {
    b2BodyId Id;
    b2Polygon polygon;
    std::vector<b2Vec2> GetWorldVertices();
};

struct PhysicsRigidCircle {
    b2BodyId Id;
    b2Circle circle;
    float GetRadius();
    b2Vec2 GetPosition();
};

struct PhysicsSoftBody {
    std::vector<PhysicsRigidCircle> vertices;
    std::vector<b2JointId> joints;
    void ApplyImpulse(float impulseX, float impulseY);
};

struct PhysicsSoftBodyJointConn {
    uint32_t vertex1;
    uint32_t vertex2;
};

class Physics {
public:
    Physics();
    void Update(float timestep);
    PhysicsRigidBox CreateBox(b2Vec2 position, b2Vec2 size, bool dynamic = false);
    PhysicsRigidCircle CreateCircle(b2Vec2 position, float radius, bool dynamic = false);
    PhysicsSoftBody CreateSoftBody(
        b2Vec2 position,
        const std::span<const b2Vec2>& vertices,
        const std::span<const PhysicsSoftBodyJointConn>& jointConns);
private:
    b2WorldId m_worldId;
};

