#pragma once
#include "body.h"

namespace elevate {

	struct PotentialContact
	{
		elevate::RigidBody* body[2];
	};

	template<class BoundVolumeClass>
	class BVHNode
	{
	public:
		BVHNode* children[2];

		BoundVolumeClass volume;

		Rigidbody* body;

		bool isLeaf() const
		{
			return body != NULL;
		}

		unsigned getPotentialContacts(PotentialContact* contacts, unsigned limit) const;

		template<class BoundingVolumeClass>
		bool BVHNode<BoundingVolumeClass>::overlaps(const BVHNode<BoundingVolumeClass>* other) const
		{
			return volume->overlaps(other->volume);
		}

		template<class BoundingVolumeClass>
		unsigned BVHNode<BoundingVolumeClass>::getPotentialContacts(PotentialContact* contacts, unsigned limit) const
		{
			if (isLeaf() || limit == 0) return 0;

			return children[0]->getPotentialContactsWith(children[1], contacts, limit);
		}

		template<class BoundingVolumeClass>
		unsigned BVHNode<BoundVolumeClass>::getPotentialContactsWith(const BVHNode<BoundingVolumeClass>* other, PotentialContact* contacts, unsigned limit) const
		{
			if (!overlaps(other) || limit == 0) return 0;

			if (isLeaf() && other->isLeaf())
			{
				contacts->body[0] = body;
				contacts->body[1] = other->body;
				return 1;
			}

			if (other->isLeaf() || (!isLeaf() && volume->getSize() >= other->volume->getSize()))
			{
				unsigned count = children[0]->getPotentialContactsWith(other, contacts, limit);

				if (limit > count)
				{
					return count + children[1]->getPotentialContactsWith(other, contacts + count, limit - count);
				}
				else
				{
					return count;
				}
			}
			else
			{
				unsigned count = getPotentialContactsWith(other->children[0], contacts, limit);

				if (limit > count)
				{
					return count + getPotentialContactsWith(other->children[1], contacts + count, limit - count);
				}
				else
				{
					return count;
				}
			}
		}

		struct BoundingSphere
		{
			Vector3 center;
			real radius;

		public:
			BoundingSphere(const Vector3& center, real radius)
				: center(center),
				radius(radius)
			{
			};

			BoundingSphere(const BoundingSphere& one, const BoundingSphere& two);

			bool overlaps(const BoundingSphere* other) const;
		};
	};
}