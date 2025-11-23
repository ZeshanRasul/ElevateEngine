#include "elevate/Physics/World.h"

using namespace elevate;

void World::startFrame()
{
	for (RigidBodies::iterator p = bodies.begin(); p != bodies.end(); p++)
	{
		(*p)->clearAccumulator();
		(*p)->calculateDerivedData();
	}
}

void World::integrate(real duration)
{
	for (RigidBodies::iterator p = bodies.begin(); p != bodies.end(); p++)
	{
		(*p)->integrate(duration);
	}
}

void World::runPhysics(real duration)
{
	registry.updateForces(duration);
	BodyRegistration* reg = firstBody;
	while (reg)
	{
		// Remove all forces from the accumulator
		reg->body->integrate(duration);

		// Get the next registration
		reg = reg->next;
	}// Resolve the contacts
	
}