#pragma once
#include "assert.h"

#include "Physics/contacts.h"
#include "Physics/body.h"

namespace elevate {
	struct CollisionData
	{
		CollisionData() : contactArray(nullptr),
			contacts(nullptr),
			contactsLeft(0),
			contactCount(0),
			friction(0),
			restitution(1),
			tolerance(0)
		{
		}

		Contact* contactArray{};

		Contact* contacts{};

		int contactsLeft;

		unsigned contactCount;

		real friction;

		real restitution;

		real tolerance;
		bool hasMoreContacts() const
		{
			return contactsLeft > 0;
		}

		void reset(unsigned maxContacts)
		{
			contactsLeft = maxContacts;
			contactCount = 0;
			contacts = contactArray;
		};

		void addContacts(unsigned count)
		{
			contactsLeft -= count;
			contactCount += count;
			contacts += count;
		};
	};

	class CollisionPrimitive
	{
	protected:
		Matrix4 transform;

	public:
		friend class IntersectionTests;
		friend class CollisionDetector;

		RigidBody* body;

		Matrix4 offset;

		void calculateInternals();

		Vector3 getAxis(unsigned index) const
		{
			return transform.getAxisVector(index);
		}

		const Matrix4& getTransform() const
		{
			return transform;
		}

	};

	class CollisionSphere : public CollisionPrimitive
	{
	public:
		real radius;
	};

	class CollisionBox : public CollisionPrimitive
	{
	public:
		Vector3 halfSize;
	};





	static inline real transformToAxis(const CollisionBox& box, const Vector3& axis)
	{
		return
			box.halfSize.x * real_abs(axis * box.getAxis(0)) +
			box.halfSize.y * real_abs(axis * box.getAxis(1)) +
			box.halfSize.z * real_abs(axis * box.getAxis(2));
	};

	static inline real penetrationOnAxis(const CollisionBox& one, const CollisionBox& two, const Vector3& axis, const Vector3& toCentre)
	{
		// Project the half-size of one onto axis
		real oneProject = transformToAxis(one, axis);
		real twoProject = transformToAxis(two, axis);

		// Project this onto the axis
		real distance = real_abs(toCentre * axis);

		// Return the overlap (i.e. positive indicates
		// overlap, negative indicates separation).
		return oneProject + twoProject - distance;
	}

	static inline bool tryAxis(
		const CollisionBox& one,
		const CollisionBox& two,
		Vector3 axis,
		const Vector3& toCentre,
		unsigned index,

		// These values may be updated
		real& smallestPenetration,
		unsigned& smallestCase
	)
	{
		// Make sure we have a normalized axis, and don't check almost parallel axes
		if (axis.squareMagnitude() < 0.0001) return true;
		axis.normalize();

		real penetration = penetrationOnAxis(one, two, axis, toCentre);

		if (penetration < 0) return false;
		if (penetration < smallestPenetration) {
			smallestPenetration = penetration;
			smallestCase = index;
		}
		return true;
	}

	static void fillPointFaceBoxBox(
		CollisionBox& one,
		CollisionBox& two,
		const Vector3& toCentre,
		CollisionData* data,
		unsigned best,
		real pen
	)
	{
		Contact* contact = data->contacts;

		Vector3 normal = one.getAxis(best);
		if (one.getAxis(best) * toCentre > 0)
		{
			normal = normal * -1.0f;

			Vector3 vertex = two.halfSize;
			if (two.getAxis(0) * normal < 0) vertex.x = -vertex.x;
			if (two.getAxis(1) * normal < 0) vertex.y = -vertex.y;
			if (two.getAxis(2) * normal < 0) vertex.z = -vertex.z;

			contact->contactNormal = normal;
			contact->penetration = pen;
			contact->contactPoint = two.getTransform() * vertex;
			contact->setBodyData(one.body, two.body,
				data->friction, data->restitution);
			data->addContacts(1);

			data->contacts[data->contactCount - 1] = *contact;
			data->contactArray[data->contactCount - 1] = *contact;
		}

	}
	static inline Vector3 contactPoint(
		const Vector3& pOne,
		const Vector3& dOne,
		real oneSize,
		const Vector3& pTwo,
		const Vector3& dTwo,
		real twoSize,

		bool useOne)
	{
		Vector3 toSt, cOne, cTwo;
		real dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
		real denom, mua, mub;

		smOne = dOne.squareMagnitude();
		smTwo = dTwo.squareMagnitude();
		dpOneTwo = dTwo * dOne;

		toSt = pOne - pTwo;
		dpStaOne = dOne * toSt;
		dpStaTwo = dTwo * toSt;

		denom = smOne * smTwo - dpOneTwo * dpOneTwo;

		if (real_abs(denom) < 0.0001f) {
			return useOne ? pOne : pTwo;
		}

		mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
		mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

		if (mua > oneSize ||
			mua < -oneSize ||
			mub > twoSize ||
			mub < -twoSize)
		{
			return useOne ? pOne : pTwo;
		}
		else
		{
			cOne = pOne + dOne * mua;
			cTwo = pTwo + dTwo * mub;

			return cOne * 0.5 + cTwo * 0.5;
		}
	}
	class CollisionDetector
	{
	public:
		static unsigned sphereAndSphere(CollisionSphere& one, CollisionSphere& two, CollisionData* data)
		{

			if (data->contactsLeft <= 0) return 0;

			Vector3 positionOne = one.body->getPosition();
			Vector3 positionTwo = two.body->getPosition();

			Vector3 midline = positionOne - positionTwo;
			real size = midline.magnitude();

			if (size <= 0.0f || size > one.radius + two.radius)
			{
				return 0;
			}

			Vector3 normal = midline * (((real)1.0) / size);

			Contact* contact = data->contacts;
			contact->contactNormal = normal;
			contact->contactPoint = positionOne + midline * (real)0.5;
			contact->penetration = (one.radius + two.radius - size);
			contact->setBodyData(one.body, two.body, data->friction, data->restitution);

			data->addContacts(1);
			data->contacts[data->contactCount - 1] = *contact;
			data->contactArray[data->contactCount - 1] = *contact;

			return 1;

		};


#define CHECK_OVERLAP(axis, index) \
    if (!tryAxis(one, two, (axis), toCentre, (index), pen, best)) return 0;

		static unsigned boxAndBox(CollisionBox& one, CollisionBox& two, CollisionData* data)
		{
			Vector3 toCentre = two.getAxis(3) - one.getAxis(3);

			real pen = REAL_MAX;
			unsigned best = 0xffffff;

			CHECK_OVERLAP(one.getAxis(0), 0);
			CHECK_OVERLAP(one.getAxis(1), 1);
			CHECK_OVERLAP(one.getAxis(2), 2);

			CHECK_OVERLAP(two.getAxis(0), 3);
			CHECK_OVERLAP(two.getAxis(1), 4);
			CHECK_OVERLAP(two.getAxis(2), 5);

			unsigned bestSingleAxis = best;

			CHECK_OVERLAP(one.getAxis(0) % two.getAxis(0), 6);
			CHECK_OVERLAP(one.getAxis(0) % two.getAxis(1), 7);
			CHECK_OVERLAP(one.getAxis(0) % two.getAxis(2), 8);
			CHECK_OVERLAP(one.getAxis(1) % two.getAxis(0), 9);
			CHECK_OVERLAP(one.getAxis(1) % two.getAxis(1), 10);
			CHECK_OVERLAP(one.getAxis(1) % two.getAxis(2), 11);
			CHECK_OVERLAP(one.getAxis(2) % two.getAxis(0), 12);
			CHECK_OVERLAP(one.getAxis(2) % two.getAxis(1), 13);
			CHECK_OVERLAP(one.getAxis(2) % two.getAxis(2), 14);

			assert(best != 0xffffff);

			if (best < 3)
			{
				fillPointFaceBoxBox(one, two, toCentre, data, best, pen);
				return 1;
			}
			else if (best < 6)
			{
				fillPointFaceBoxBox(two, one, toCentre * -1.0f, data, best - 3, pen);
				return 1;
			}
			else
			{
				best -= 6;
				unsigned oneAxisIndex = best / 3;
				unsigned twoAxisIndex = best % 3;
				Vector3 oneAxis = one.getAxis(oneAxisIndex);
				Vector3 twoAxis = two.getAxis(twoAxisIndex);
				Vector3 axis = oneAxis % twoAxis;
				axis.normalize();

				if (axis * toCentre > 0) axis = axis * -1.0f;

				Vector3 ptOnOneEdge = one.halfSize;
				Vector3 ptOnTwoEdge = two.halfSize;
				for (unsigned i = 0; i < 3; i++)
				{
					if (i == oneAxisIndex) ptOnOneEdge[i] = 0;
					else if (one.getAxis(i) * axis > 0) ptOnOneEdge[i] = -ptOnOneEdge[i];

					if (i == twoAxisIndex) ptOnTwoEdge[i] = 0;
					else if (two.getAxis(i) * axis < 0) ptOnTwoEdge[i] = -ptOnTwoEdge[i];
				}

				ptOnOneEdge = one.getTransform() * ptOnOneEdge;
				ptOnTwoEdge = two.getTransform() * ptOnTwoEdge;

				Vector3 vertex = contactPoint(
					ptOnOneEdge, oneAxis, one.halfSize[oneAxisIndex],
					ptOnTwoEdge, twoAxis, two.halfSize[twoAxisIndex],
					bestSingleAxis > 2
				);

				Contact* contact = data->contacts;

				contact->penetration = pen;
				contact->contactNormal = axis;
				contact->contactPoint = vertex;
				contact->setBodyData(one.body, two.body,
					data->friction, data->restitution);
				data->addContacts(1);

				data->contacts[data->contactCount - 1] = *contact;
				data->contactArray[data->contactCount - 1] = *contact;

				return 1;
			}
			return 0;
		}
	};
#undef CHECK_OVERLAP

}