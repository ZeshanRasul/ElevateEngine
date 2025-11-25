#pragma once
#include "Physics/core.h"
#include "body.h"

namespace elevate {

	class ContactResolver;

	class Contact
	{
		friend class ContactResolver;
	public:
		Contact() : body{ nullptr, nullptr },
			restitution(0),
			contactPoint(),
			contactNormal(),
			friction(0),
			penetration(0),
			desiredDeltaVelocity(0)
		{
		}

		RigidBody* body[2] = { nullptr, nullptr };

		real restitution;

		Vector3 contactPoint;
		Vector3 contactNormal;
		Vector3 relativeContactPosition[2];

		real friction;


		real penetration;

		void setBodyData(RigidBody* one, RigidBody* two, real friction, real restitution);

		void calculateContactBasis();

		void calculateInternals(real duration);
		void calculateDesiredDeltaVelocity(real duration);
	protected:

		Matrix3 contactToWorld;

		Vector3 contactVelocity;
		real desiredDeltaVelocity;

	protected:
		Vector3 calculateLocalVelocity(unsigned bodyIndex, real duration);

		inline Vector3 calculateFrictionlessImpulse(Matrix3* inverseInertiaTensor);
		inline Vector3 calculateFrictionImpulse(Matrix3* inverseInertiaTensor);

		void applyVelocityChange(Vector3 velocityChange[2],
			Vector3 rotationChange[2]);

		void applyPositionChange(Vector3 linearChange[2], Vector3 angularChange[2], real penetration);


		void matchAwakeState();

		void swapBodies()
		{
			contactNormal = contactNormal * -1;
			RigidBody* temp = body[0];
			body[0] = body[1];
			body[1] = temp;
		}
	};


	class ContactResolver
	{
	public:
		ContactResolver() = default;

		void resolveContacts(Contact* contactArray, unsigned numContacts, real duration);

		void prepareContacts(Contact* contactArray, unsigned numContacts, real duration);
	protected:

		void adjustVelocities(Contact* contactArray, unsigned numContacts, real duration);

		void adjustPositions(Contact* contacts, unsigned numContacts, real duration);

		unsigned velocityIterationsUsed = 0;
		unsigned velocityIterations = 20;
		real velocityEpsilon = 0.01f;

		unsigned positionIterationsUsed = 0;
		unsigned positionIterations = 20;
		real positionEpsilon = 0.01f;
	};

	class ContactGenerator
	{
	public:
		virtual unsigned addContact(Contact* contact, unsigned limit) const = 0;
	};
};