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

	protected:

		Matrix3 contactToWorld;

		Vector3 contactVelocity;

		real desiredDeltaVelocity;

	protected:

		void calculateInternals(real duration);

		void calculateLocalVelocity(unsigned bodyIndex, real duration);

		inline Vector3 calculateFrictionlessImpulse(Matrix3* inverseInertiaTensor);

		void applyVelocityChange(Vector3 velocityChange[2],
			Vector3 rotationChange[2]);

		void applyPositionChange(Vector3 linearChange[2],
			Vector3 angularChange[2],
			real penetration);
	};

}