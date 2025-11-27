#pragma once

#include "core.h"

namespace elevate {
	class RigidBody {
	public:
		RigidBody() : linearDamping(0.99f)               
			, angularDamping(0.95f)
			, inverseMass(1.0f)                   
			, mass(1.0f)
			, orientation(0, 0, 0, 1)             
			, position(0.0f, 0.0f, 0.0f)
			, velocity(0.0f, 0.0f, 0.0f)
			, acceleration(0.0f, 0.0f, 0.0f)
			, rotation(0.0f, 0.0f, 0.0f)          
			, inverseInertiaTensor(Matrix3())         
			, inverseInertiaTensorWorld(Matrix3())    
			, forceAccum(0.0f, 0.0f, 0.0f)
			, torqueAccum(0.0f, 0.0f, 0.0f)
			, isAwake(true)
			, motion(0.0f)
			, canSleep(true)
			, sleepEpsilon(0.7f)
			, lastFrameAcceleration(0.0f, 0.0f, 0.0f)
			, transformMatrix(Matrix4()) {};

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
		void setInverseMass(const real invMass) { inverseMass = invMass; }
		real getInverseMass() const { return inverseMass; };

		bool hasFiniteMass() const {
			return 1.0f / inverseMass != 0.0f;
		}

		Matrix4 getTransform() { return transformMatrix; }

		void setPosition(const Vector3& pos) { position = pos; }
		void setPosition(const real x, const real y, const real z);
		Vector3 getPosition() const { return position; }

		void setVelocity(const Vector3& vel) { velocity = vel; }
		void setVelocity(const real x, const real y, const real z);
		Vector3 getVelocity() const { return velocity; }
		void addVelocity(const Vector3& deltaVel)
		{
			velocity += deltaVel;
		};

		void setAcceleration(const Vector3& acc) { acceleration = acc; }
	//	void setAcceleration(const real x, const real y, const real z);
		Vector3 getAcceleration() const { return acceleration; };

		void addRotation(const Vector3& deltaRot)
		{
			rotation += deltaRot;
		};
		Vector3 getRotation() const { return rotation; }
		void setRotation(const Vector3& rot) { rotation = rot; }

		Quaternion getOrientation() const { return orientation; }
		void setOrientation(const Quaternion& orient) { orientation = orient; }

		Vector3 getPointInLocalSpace(const Vector3& point) const
		{
			return transformMatrix.transformInverse(point);
		}

		Vector3 getDirectionInLocalSpace(const Vector3& direction) const
		{
			return transformMatrix.transformInverseDirection(direction);
		}

		Vector3 getPointInWorldSpace(const Vector3& point) const;

		Matrix3 getInverseInertiaTensor() const;
		void setInverseInertiaTensor(const Matrix3& tensor);

		void setInertiaTensor(const Matrix3& inertiaTensor)
		{
			inverseInertiaTensor.setInverse(inertiaTensor);
		}

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
		void getGLTransformMatrix(real* mat) { mat = transformMatrix.data; }


		void setAwake(const bool awake) { isAwake = awake; }
		bool getAwake() const { return isAwake; }

		void setDamping(const real linear, const real angular)
		{
			linearDamping = linear;
			angularDamping = angular;
		}
		
		void setLinearDamping(const real linear) { linearDamping = linear; }
		real getLinearDamping() const { return linearDamping; }
		void setAngularDamping(const real angular) { angularDamping = angular; }
		real getAngularDamping() const { return angularDamping; }

		void setCanSleep(const bool canSleep) { this->canSleep = canSleep; }

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

		bool isAwake = true;
		real motion = 0.0f;
		bool canSleep = true;
		real sleepEpsilon = 0.5f;

		Vector3 lastFrameAcceleration;

		Matrix4 transformMatrix;

	};
};