#include "CollideNarrow.h"

void elevate::CollisionPrimitive::calculateInternals()
{
	transform = body->getTransform() * offset;
}
