#include "Physics/CollideBroad.h"

template<class BoundVolumeClass>
inline elevate::BVHNode<BoundVolumeClass>::BoundingSphere::BoundingSphere(const BoundingSphere& one, const BoundingSphere& two)
{
	Vector3 centerOffset = two.center - one.center;
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

}

template<class BoundingVolumeClass>
elevate::BVHNode<BoundingVolumeClass>::~BVHNode()
{
	if (parent)
	{
		BVHNode<BoundingVolumeClass>* sibling;
		if (parent->children[0] == this)
		{
			sibling = parent->children[1];
		}
		else
		{
			sibling = parent->children[0];
		}

		parent->volume = sibling->volume;
		parent->body = sibling->body;
		parent->children[0] = sibling->children[0];
		parent->children[1] = sibling->children[1];

		sibling->parent = NULL;
		sibling->body = NULL;
		sibling->children[0] = NULL;
		sibling->children[1] = NULL;
		delete sibling;

		parent->recalculateBoundingVolume();
	}

	if (children[0])
	{
		children[0]->parent = NULL;
		delete children[0];
	}

	if (children[1])
	{
		children[1]->parent = NULL;
		delete children[1];
	}
}
