#pragma once

#include "core.h"

namespace elevate {
	class RigidBody {
	public:
		RigidBody() : linearDamping(1.0f), inverseMass(1.0f) {};

		void integrate(real duration);
		void calculateDerivedData();

		void setDamping(const real newDamping);
		real getDamping() const;

		real getMass() const {
			if (inverseMass == 0) {
				return REAL_MAX;
			}
			else
			{
				return ((real)1.0) / inverseMass;
			}
		}
		void setMass(const real mass) { inverseMass = 1.0f / mass; }
		void setInverseMass(const real invMass);
		real getInverseMass() const { return inverseMass; };

		bool hasFiniteMass() const {
			return 1.0f / inverseMass != 0.0f;
		}

		void setPosition(const Vector3& pos) { position = pos; };
		void setPosition(const real x, const real y, const real z);
		Vector3 getPosition() const { return position; }

		void setVelocity(const Vector3& vel) { velocity = vel; }
		void setVelocity(const real x, const real y, const real z);
		Vector3 getVelocity() const;

		void setAcceleration(const Vector3& acc);
		void setAcceleration(const real x, const real y, const real z);
		Vector3 getAcceleration() const { return acceleration; };

		Quaternion getOrientation() const { return orientation; }
		void setOrientation(const Quaternion& orient) { orientation = orient; }

		Matrix3 getInverseInertiaTensor() const;
		void setInverseInertiaTensor(const Matrix3& tensor);

		void getInverseInertiaTensorWorld(Matrix3* inverseInertiaTensor) const
		{
			*inverseInertiaTensor = inverseInertiaTensorWorld;
		};

		Vector3 getPointinWorldSpace(const Vector3& point) const;

		Vector3 getLastFrameAcceleration() const { return lastFrameAcceleration; }
		void clearAccumulator();

		void addForce(const Vector3& force);
		void addForceAtPoint(const Vector3& force, const Vector3& point);
		void addForceAtBodyPoint(const Vector3& force, const Vector3& point);


		Matrix4 getTransformMatrix() const { return transformMatrix; }

		void setAwake(const bool awake) { isAwake = awake; }
		bool getAwake() const { return isAwake; }
	protected:
		real linearDamping;
		real angularDamping;

		real inverseMass;
		real mass;

		Quaternion orientation;
		Vector3 position;
		Vector3 velocity;
		Vector3 acceleration;

		Vector3 rotation;
		Matrix3 inverseInertiaTensor;
		Matrix3 inverseInertiaTensorWorld;

		Vector3 forceAccum;
		Vector3 torqueAccum;

		bool isAwake;

		Vector3 lastFrameAcceleration;

		Matrix4 transformMatrix;

	};
};