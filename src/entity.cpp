#include "entity.h"

Entity::Entity(Platform& platform, Renderer& renderer, Physics& physics, unsigned int m_texIdx)
    : m_platformRef(platform), m_rendererRef(renderer), m_physicsRef(physics), m_texIdx(m_texIdx)
{}

Player::Player(Platform& platform, Renderer& renderer, Physics& physics, unsigned int m_texIdx)
    : Entity(platform, renderer, physics, m_texIdx)
{
    m_physicsObject = m_physicsRef.CreateSoftBody(
        b2Vec2{ 5.0f, 1.0f }, g_softbodyVertices, g_softbodyConnections);
}

void Player::Render() {
    std::array<b2Vec2, 6> vertices;
    for (int i = 0; i < g_softbodyVertices.size(); i++) {
        vertices[i] = m_physicsObject.vertices[i].GetPosition();
    }
    b2Vec2 center = b2Vec2_zero;
    for (const auto& v : vertices) {
        center = b2Add(center, v);
    }
    center = b2MulSV(1.0f / vertices.size(), center);

    RendererTriangle triangle;

    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .u = 0.0f, .v = 1.0f, .texIdx = m_texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[1].x, .y = vertices[1].y, .u = 1.0f, .v = 1.0f, .texIdx = m_texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = m_texIdx };
    m_rendererRef.PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[1].x, .y = vertices[1].y, .u = 1.0f, .v = 1.0f, .texIdx = m_texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .u = 1.0f, .v = 0.5f, .texIdx = m_texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = m_texIdx };
    m_rendererRef.PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .u = 1.0f, .v = 0.5f, .texIdx = m_texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[3].x, .y = vertices[3].y, .u = 1.0f, .v = 0.0f, .texIdx = m_texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = m_texIdx };
    m_rendererRef.PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[3].x, .y = vertices[3].y, .u = 1.0f, .v = 0.0f, .texIdx = m_texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[4].x, .y = vertices[4].y, .u = 0.0f, .v = 0.0f, .texIdx = m_texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = m_texIdx };
    m_rendererRef.PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[4].x, .y = vertices[4].y, .u = 0.0f, .v = 0.0f, .texIdx = m_texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[5].x, .y = vertices[5].y, .u = 0.0f, .v = 0.5f, .texIdx = m_texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = m_texIdx };
    m_rendererRef.PushTriangle(triangle);

    triangle.points[0] = RendererVertex{ .x = vertices[5].x, .y = vertices[5].y, .u = 0.0f, .v = 0.5f, .texIdx = m_texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .u = 0.0f, .v = 1.0f, .texIdx = m_texIdx };
    triangle.points[2] = RendererVertex{ .x =      center.x, .y =      center.y, .u = 0.5f, .v = 0.5f, .texIdx = m_texIdx };
    m_rendererRef.PushTriangle(triangle);
}

void Player::Update() {
    float mouseX, mouseY;
    bool clickIsPressed;
    m_platformRef.GetMousePosition(&mouseX, &mouseY, &clickIsPressed);
    int wndWidth, wndHeight;
    m_platformRef.GetWindowSize(&wndWidth, &wndHeight);
    mouseX *= 10.0f / (float)wndWidth;
    mouseY *= 10.0f / (float)wndWidth;
    if (clickIsPressed) {
        m_physicsObject.ApplyImpulse(
            0.001f * (mouseX - m_physicsObject.vertices[0].GetPosition().x),
            0.001f * (mouseY - m_physicsObject.vertices[0].GetPosition().y)
        );
    }
}

Wall::Wall(Platform& platform, Renderer& renderer, Physics& physics, unsigned int texIdx, b2Vec2 pos, b2Vec2 size)
    : Entity(platform, renderer, physics, texIdx)
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
    m_rendererRef.PushTriangle(triangle);
    triangle.points[0] = RendererVertex{ .x = vertices[0].x, .y = vertices[0].y, .texIdx = m_texIdx };
    triangle.points[1] = RendererVertex{ .x = vertices[2].x, .y = vertices[2].y, .texIdx = m_texIdx };
    triangle.points[2] = RendererVertex{ .x = vertices[3].x, .y = vertices[3].y, .texIdx = m_texIdx };
    triangle.points[0].u = 0;     triangle.points[0].v = height;
    triangle.points[1].u = width; triangle.points[1].v = 0;
    triangle.points[2].u = 0;     triangle.points[2].v = 0;
    m_rendererRef.PushTriangle(triangle);
}
