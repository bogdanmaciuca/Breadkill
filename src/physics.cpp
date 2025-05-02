#include "physics.h"

#include <vector>
#include "box2d/box2d.h"

std::vector<b2Vec2> PhysicsRigidBox::GetWorldVertices() {
    std::vector<b2Vec2> vertices(polygon.count);
    for (int i = 0; i < polygon.count; i++) {
        vertices[i] = b2TransformPoint(b2Body_GetTransform(Id), polygon.vertices[i]);
    }
    return vertices;
}

b2Vec2 PhysicsRigidCircle::GetPosition() {
    return b2Body_GetPosition(Id);
}

float PhysicsRigidCircle::GetRadius() {
    return circle.radius;
}

void PhysicsSoftBody::ApplyImpulse(float impulseX, float impulseY) {
    for (const auto& v : vertices) {
        b2Body_ApplyLinearImpulseToCenter(v.Id, b2Vec2{impulseX, impulseY}, true);
    }
}

Physics::Physics() {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{ 0.0f, 9.81f };
    m_worldId = b2CreateWorld(&worldDef);
}

void Physics::Update(float timestep) {
    b2World_Step(m_worldId, timestep, PHYSICS_SUBSTEP_COUNT);
}

PhysicsRigidBox Physics::CreateBox(b2Vec2 position, b2Vec2 size, bool dynamic) {
    PhysicsRigidBox object;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = (dynamic ? b2_dynamicBody : b2_staticBody);
    bodyDef.position = position;
    object.Id = b2CreateBody(m_worldId, &bodyDef);
    object.polygon = b2MakeBox(size.x / 2.0f, size.y / 2.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.material.friction = 0.3f;
    b2CreatePolygonShape(object.Id, &shapeDef, &object.polygon);

    return object;
}

PhysicsRigidCircle Physics::CreateCircle(b2Vec2 position, float radius, bool dynamic) {
    PhysicsRigidCircle object;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = (dynamic ? b2_dynamicBody : b2_staticBody);
    bodyDef.position = position;
    object.Id = b2CreateBody(m_worldId, &bodyDef);
    object.circle = b2Circle{ .center = b2Vec2_zero, .radius = radius };
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.material.friction = 0.3f;
    b2CreateCircleShape(object.Id, &shapeDef, &object.circle);

    return object;
}

PhysicsSoftBody Physics::CreateSoftBody(
    b2Vec2 position,
    const std::span<const b2Vec2>& vertices,
    const std::span<const PhysicsSoftBodyJointConn>& jointConns
) {
    PhysicsSoftBody object;

    for (const auto& vertex : vertices) {
        object.vertices.push_back(CreateCircle(b2Add(position, vertex), 0.02f, true));
    }
    for (const auto& conn : jointConns) {
        b2DistanceJointDef jointDef = b2DefaultDistanceJointDef();
        jointDef.bodyIdA = object.vertices[conn.vertex1].Id;
        jointDef.bodyIdB = object.vertices[conn.vertex2].Id;
        jointDef.localAnchorA = b2Vec2_zero;
        jointDef.localAnchorB = b2Vec2_zero;
        jointDef.collideConnected = false;
        jointDef.dampingRatio = 0.9f;
        jointDef.hertz = 10.0f;
        jointDef.minLength = 0.15f;
        jointDef.maxLength = 3.0f;
        jointDef.length = b2Distance(b2Body_GetWorldPoint(jointDef.bodyIdA, jointDef.localAnchorA), b2Body_GetWorldPoint(jointDef.bodyIdB, jointDef.localAnchorB));
        jointDef.enableSpring = true;
        jointDef.enableLimit = true;
        object.joints.push_back(b2CreateDistanceJoint(m_worldId, &jointDef));
    }

    return object;
}
