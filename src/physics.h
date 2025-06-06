#pragma once
#include <vector>
#include <span>
#include "box2d/box2d.h"

b2Vec2 operator+(b2Vec2 left, b2Vec2 right);

enum {
    PHYSICS_SUBSTEP_COUNT = 8
};

struct PhysicsRigidBox {
    b2BodyId Id;
    b2Polygon polygon;
    std::vector<b2Vec2> GetWorldVertices() const;
};

struct PhysicsRigidCircle {
    b2BodyId Id;
    b2Circle circle;
    float GetRadius() const;
    b2Vec2 GetPosition() const;
    std::vector<b2Vec2> GetWorldVertices() const;
    void ApplyImpulse(float impulseX, float impulseY);
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
    // timestep -> interval of time between simulation iterations; recommanded to be constant
    void Update(float timestep);
    PhysicsRigidBox CreateBox(b2Vec2 position, b2Vec2 size, bool dynamic = false);
    PhysicsRigidCircle CreateCircle(b2Vec2 position, float radius, bool dynamic = false);
    // vertices -> (of the softbody)
    // jointConns -> springs connecting pairs of vertices so that the body seems squishy
    PhysicsSoftBody CreateSoftBody(
        b2Vec2 position,
        const std::span<const b2Vec2>& vertices,
        const std::span<const PhysicsSoftBodyJointConn>& jointConns);
private:
    b2WorldId m_worldId;
};

