#pragma once
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
			contact->setBodyData(*one.body, *two.body, data->friction, data->restitution);

			data->addContacts(1);
			data->contacts[data->contactCount - 1] = *contact;
			data->contactArray[data->contactCount - 1] = *contact;

			return 1;
		
		};
	};
}