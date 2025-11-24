#pragma once
#include "Physics/core.h"
#include "body.h"

namespace elevate {

	class ContactResolver;

	class Contact
	{
		friend class ContactResolver;
	public:
		RigidBody* body[2];

		real restitution;

		Vector3 contactPoint;
		Vector3 contactNormal;
		Vector3 relativeContactPosition[2];

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
		void resolveContacts(Contact* contactArray, unsigned numContacts, real duration);

		void prepareContacts(Contact* contactArray, unsigned numContacts, real duration);
	protected:

		void adjustVelocities(Contact* contactArray, unsigned numContacts, real duration);

		void adjustPositions(Contact* contacts, unsigned numContacts, real duration);

		unsigned velocityIterationsUsed;
		unsigned velocityIterations;
		real velocityEpsilon;

		unsigned positionIterationsUsed;
		unsigned positionIterations;
		real positionEpsilon;
	};
}