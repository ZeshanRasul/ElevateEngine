#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Physics/CollideNarrow.h"

struct DebugLine
{
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 color;
};

class DebugDraw
{
public:
    static void AddLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color);
    static void Clear();
    static const std::vector<DebugLine>& GetLines();

private:
    static std::vector<DebugLine> s_Lines;
};

static glm::vec3 ToGlm(const elevate::Vector3& v)
{
    return glm::vec3(v.x, v.y, v.z);
}



static void DebugDrawCollisionBox(const elevate::CollisionBox& box, const glm::vec3& color)
{
    using elevate::Vector3;

    const Vector3 hs = box.halfSize * 1.01f;

    const Vector3 axis0 = box.getAxis(0);
    const Vector3 axis1 = box.getAxis(1);
    const Vector3 axis2 = box.getAxis(2);
    const Vector3 center = box.getAxis(3);

    Vector3 corners[8];

    for (int i = 0; i < 8; ++i)
    {
        const elevate::real sx = (i & 1) ? hs.x : -hs.x;
        const elevate::real sy = (i & 2) ? hs.y : -hs.y;
        const elevate::real sz = (i & 4) ? hs.z : -hs.z;

        corners[i] = center +
            axis0 * sx +
            axis1 * sy +
            axis2 * sz;
    }

    // Cube edges: 12 edges connecting the 8 corners
    static const int edgeIndices[12][2] =
    {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // bottom face
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // top face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // vertical edges
    };

    for (int e = 0; e < 12; ++e)
    {
        const glm::vec3 p0 = ToGlm(corners[edgeIndices[e][0]]);
        const glm::vec3 p1 = ToGlm(corners[edgeIndices[e][1]]);
        DebugDraw::AddLine(p0, p1, color);
    }
}

static void DebugDrawCollisionSphere(const elevate::CollisionSphere& sphere,
    const glm::vec3& color,
    int segments = 24)
{
    using elevate::Vector3;

    const Vector3 centerV = sphere.getAxis(3); // world-space center
    const float r = static_cast<float>(sphere.radius) * 1.01f;
    const glm::vec3 center = ToGlm(centerV);

    const float twoPi = 6.28318530718f;
    const float step = twoPi / static_cast<float>(segments);

    // XY circle
    for (int i = 0; i < segments; ++i)
    {
        const float a0 = i * step;
        const float a1 = (i + 1) * step;

        const glm::vec3 p0 = center + glm::vec3(std::cos(a0) * r, std::sin(a0) * r, 0.0f);
        const glm::vec3 p1 = center + glm::vec3(std::cos(a1) * r, std::sin(a1) * r, 0.0f);

        DebugDraw::AddLine(p0, p1, color);
    }

    // XZ circle
    for (int i = 0; i < segments; ++i)
    {
        const float a0 = i * step;
        const float a1 = (i + 1) * step;

        const glm::vec3 p0 = center + glm::vec3(std::cos(a0) * r, 0.0f, std::sin(a0) * r);
        const glm::vec3 p1 = center + glm::vec3(std::cos(a1) * r, 0.0f, std::sin(a1) * r);

        DebugDraw::AddLine(p0, p1, color);
    }

    // YZ circle
    for (int i = 0; i < segments; ++i)
    {
        const float a0 = i * step;
        const float a1 = (i + 1) * step;

        const glm::vec3 p0 = center + glm::vec3(0.0f, std::cos(a0) * r, std::sin(a0) * r);
        const glm::vec3 p1 = center + glm::vec3(0.0f, std::cos(a1) * r, std::sin(a1) * r);

        DebugDraw::AddLine(p0, p1, color);
    }
}
