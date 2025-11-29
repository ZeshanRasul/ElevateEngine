#include <cmath>

#include "Physics/precision.h"
#include "OpenGL/DebugDraw.h"

std::vector<DebugLine> DebugDraw::s_Lines;

void DebugDraw::AddLine(const glm::vec3& a, const glm::vec3& b, const glm::vec3& color)
{
    s_Lines.push_back({ a, b, color });
}

void DebugDraw::Clear()
{
    s_Lines.clear();
}

const std::vector<DebugLine>& DebugDraw::GetLines()
{
    return s_Lines;
}


