#include "Physics/fgen.h"

Spring::Spring(const Vector3& connectionPoint, RigidBody* other, const Vector3& otherConnectionPoint, real springConstant, real restLength)
	: connectionPoint(connectionPoint),
	other(other),
	otherConnectionPoint(otherConnectionPoint),
	springConstant(springConstant),
	restLength(restLength)
{
}

void Spring::updateForce(RigidBody* body, real duration)
{
	Vector3 lws = body->getPointinWorldSpace(connectionPoint);
	Vector3 ows = other->getPointinWorldSpace(otherConnectionPoint);

	Vector3 force = lws - ows;

	real magnitude = force.magnitude();
	magnitude = real_abs(magnitude - restLength);
	magnitude *= springConstant;

	force.normalize();
	force *= -magnitude;
	body->addForceAtPoint(force, lws);
}

void Gravity::updateForce(RigidBody* RigidBody, real duration)
{
	if (!RigidBody->hasFiniteMass()) return;
	RigidBody->addForce(gravity * RigidBody->getMass());
}

void ForceRegistry::add(RigidBody* body, RigidBodyForceGenerator* forceGen)
{
	ForceRegistration registration;
	registration.RigidBody = body;
	registration.forceGen = forceGen;
	registrations.push_back(registration);
}

ForceRegistry::ForceRegistration* ForceRegistry::getRegistration(RigidBody* rb, RigidBodyForceGenerator* forceGen)
{
	for (Registry::iterator reg = registrations.begin(); reg != registrations.end(); reg++)
	{
		if (reg->RigidBody == rb && reg->forceGen == forceGen)
		{
			return &(*reg);
		}
	}
	return nullptr;
}

void ForceRegistry::updateForces(real duration)
{
	for (Registry::iterator reg = registrations.begin(); reg != registrations.end(); reg++)
	{
		reg->forceGen->updateForce(reg->RigidBody, duration);
	}
};