#include "entity.h"

#include <random>
#include "renderer.h"
#include "physics.h"

void RenderSoftbody(const PhysicsSoftBody& softbody, unsigned int texIdx) {
    std::array<b2Vec2, 6> vertices;
    for (int i = 0; i < g_softbodyVertices.size(); i++) {
        vertices[i] = softbody.vertices[i].GetPosition();
    }
    b2Vec2 center = b2Vec2_zero;
    for (const auto& v : vertices) {
        center = b2Add(center, v);
    }
    center = b2MulSV(1.0f / vertices.size(), center);

    RendererTriangle triangle;

    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .u = 0.0f, .v = 1.0f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[1].x, .y = vertices[1].y, .u = 1.0f, .v = 1.0f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    Renderer::GetInstance().PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[1].x, .y = vertices[1].y, .u = 1.0f, .v = 1.0f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .u = 1.0f, .v = 0.5f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    Renderer::GetInstance().PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .u = 1.0f, .v = 0.5f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[3].x, .y = vertices[3].y, .u = 1.0f, .v = 0.0f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    Renderer::GetInstance().PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[3].x, .y = vertices[3].y, .u = 1.0f, .v = 0.0f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[4].x, .y = vertices[4].y, .u = 0.0f, .v = 0.0f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    Renderer::GetInstance().PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[4].x, .y = vertices[4].y, .u = 0.0f, .v = 0.0f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[5].x, .y = vertices[5].y, .u = 0.0f, .v = 0.5f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    Renderer::GetInstance().PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[5].x, .y = vertices[5].y, .u = 0.0f, .v = 0.5f, .texIdx = texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .u = 0.0f, .v = 1.0f, .texIdx = texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = texIdx };
    Renderer::GetInstance().PushTriangle(triangle);
}

Entity::Entity(Platform& platform, Physics& physics, unsigned int m_texIdx)
    : m_platformRef(platform), m_physicsRef(physics), m_texIdx(m_texIdx)
{}

Wall::Wall(Platform& platform, Physics& physics, unsigned int texIdx, b2Vec2 pos, b2Vec2 size)
    : Entity(platform, physics, texIdx)
{
    m_physicsObject = m_physicsRef.CreateBox(pos, size);
}

void Wall::Render() {
    std::vector<b2Vec2> vertices = m_physicsObject.GetWorldVertices();
    float width = abs(vertices[0].x - vertices[1].x) * 0.5f;
    float height = abs(vertices[1].y - vertices[2].y) * 0.5f;
    RendererTriangle triangle;
    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .texIdx = m_texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[1].x, .y = vertices[1].y, .texIdx = m_texIdx };
    triangle.points[2] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .texIdx = m_texIdx };
    triangle.points[0].u = 0;     triangle.points[0].v = height;
    triangle.points[1].u = width; triangle.points[1].v = height;
    triangle.points[2].u = width; triangle.points[2].v = 0;
    Renderer::GetInstance().PushTriangle(triangle);
    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .texIdx = m_texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .texIdx = m_texIdx };
    triangle.points[2] = RendererVertex{ .x = vertices[3].x, .y = vertices[3].y, .texIdx = m_texIdx };
    triangle.points[0].u = 0;     triangle.points[0].v = height;
    triangle.points[1].u = width; triangle.points[1].v = 0;
    triangle.points[2].u = 0;     triangle.points[2].v = 0;
    Renderer::GetInstance().PushTriangle(triangle);
}

Player::Player(Platform& platform, Physics& physics, unsigned int m_texIdx)
    : Entity(platform, physics, m_texIdx)
{
    m_physicsObject = m_physicsRef.CreateSoftBody(
        b2Vec2{ 5.0f, 1.0f }, g_softbodyVertices, g_softbodyConnections);
}

void Player::Render() {
    RenderSoftbody(m_physicsObject, m_texIdx);
}

void Player::Update() {
}

b2Vec2 Player::GetPosition() {
    return m_physicsObject.vertices[0].GetPosition();
}

void Player::ApplyImpulse(float x, float y) {
    m_physicsObject.ApplyImpulse(x, y);
}

Enemy::Enemy(Platform& platform, Physics& physics, unsigned int texIdx, b2Vec2 pos)
    : Entity(platform, physics, texIdx)
{
    m_physicsObject = m_physicsRef.CreateSoftBody(
        pos, g_softbodyVertices, g_softbodyConnections);
}

void Enemy::Render() {
    RenderSoftbody(m_physicsObject, m_texIdx);
}

void Enemy::Update() {
    std::random_device device;
    std::mt19937 rng(device());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    b2Vec2 randomDir{ .x = dist(rng), .y = dist(rng) };
    randomDir = b2Normalize(randomDir);

    m_physicsObject.ApplyImpulse(
        ENEMY_FORCE * randomDir.x,
        ENEMY_FORCE * randomDir.y
    );
}

Bullet::Bullet(Platform& platform, Physics& physics, unsigned int texIdx, b2Vec2 pos, b2Vec2 dir)
    : Entity(platform, physics, texIdx)
{
    m_physicsObject = physics.CreateCircle(pos, BULLET_RADIUS, true);
    m_physicsObject.ApplyImpulse(
        BULLET_SPEED_COEF * dir.x,
        BULLET_SPEED_COEF * dir.y
    );
}

void Bullet::Render() {
    std::vector<b2Vec2> vertices = m_physicsObject.GetWorldVertices();
    float width = abs(vertices[0].x - vertices[1].x) * 0.5f;
    float height = abs(vertices[1].y - vertices[2].y) * 0.5f;
    RendererTriangle triangle;
    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .texIdx = m_texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[1].x, .y = vertices[1].y, .texIdx = m_texIdx };
    triangle.points[2] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .texIdx = m_texIdx };
    triangle.points[0].u = 0; triangle.points[0].v = 1;
    triangle.points[1].u = 1; triangle.points[1].v = 1;
    triangle.points[2].u = 1; triangle.points[2].v = 0;
    Renderer::GetInstance().PushTriangle(triangle);
    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .texIdx = m_texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .texIdx = m_texIdx };
    triangle.points[2] = RendererVertex{ .x = vertices[3].x, .y = vertices[3].y, .texIdx = m_texIdx };
    triangle.points[0].u = 0; triangle.points[0].v = 1;
    triangle.points[1].u = 1; triangle.points[1].v = 0;
    triangle.points[2].u = 0; triangle.points[2].v = 0;
    Renderer::GetInstance().PushTriangle(triangle);
}


