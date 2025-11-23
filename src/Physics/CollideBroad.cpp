#include "Physics/CollideBroad.h"

template<class BoundVolumeClass>
inline elevate::BVHNode<BoundVolumeClass>::BoundingSphere::BoundingSphere(const BoundingSphere& one, const BoundingSphere& two)
{
	Vectro3 centerOffset = two.center - one.center;
	real distance = centerOffset.squreMagnitude();
	real radiusDiff = two.radius - one.radius;

	if (radiusDiff * radiusDiff >= distance)
	{
		if (one.radius > two.radius)
		{
			center = one.center;
			radius = one.radius;
		}
		else
		{
			center = two.center;
			radius = two.radius;
		}
	}
	else
	{
		distance = real_sqrt(distance);
		radius = (distance + one.radius + two.radius) * ((real)0.5);

		center = one.center;
		if (distance > 0)
		{
			center += centerOffset * ((radius - one.radius) / distance);
		}
	}

	bool BoundingSphere::overlaps(const BoundingSphere * other) const
	{
		real distanceSquared = (center - other->center).squareMagnitude();
		return distanceSquared < (radius + other->radius) * (radius + other->radius);
	}
}