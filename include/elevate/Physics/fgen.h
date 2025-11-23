#pragma once

#include <vector>
#include "Physics/core.h"
#include "Physics/body.h"

using namespace elevate;

class RigidBodyForceGenerator {
public:
	virtual void updateForce(RigidBody* rigidBody, real duration) = 0;
};


class ForceRegistry {
protected:
	struct ForceRegistration {
		RigidBody* RigidBody;
		RigidBodyForceGenerator* forceGen;
	};

	typedef std::vector<ForceRegistration> Registry;
	Registry registrations;

public:
	void add(RigidBody* body, RigidBodyForceGenerator* forceGen);

	void remove(RigidBody* RigidBody, RigidBodyForceGenerator* forceGen) {};
	
	ForceRegistration* getRegistration(RigidBody* rb, RigidBodyForceGenerator* forceGen);

	void clear();

	void updateForces(real duration);
};

class Gravity : public RigidBodyForceGenerator {
private:
	Vector3 gravity;

public:
	Gravity(const Vector3& gravity)
		:gravity(gravity) {
	};

	virtual void updateForce(RigidBody* RigidBody, real duration);
};

class Spring : public RigidBodyForceGenerator {
private:
	Vector3 connectionPoint;

	Vector3 otherConnectionPoint;

	RigidBody* other;

	real springConstant;
	real restLength;

public:
	Spring(const Vector3& connectionPoint,
		RigidBody* other,
		const Vector3& otherConnectionPoint,
		real springConstant,
		real restLength);

	virtual void updateForce(RigidBody* RigidBody, real duration);
};