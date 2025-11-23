#pragma once
#include <vector>
#include "elevate/Physics/body.h"
#include "elevate/Physics/fgen.h"

class World
{
public:
	typedef std::vector<elevate::RigidBody*> RigidBodies;

protected:
	RigidBodies bodies;

	bool calculateIterations;

	struct BodyRegistration
	{
		elevate::RigidBody* body;
		BodyRegistration* next;
	};
	unsigned maxContacts;
	BodyRegistration* firstBody;
	ForceRegistry registry;

public:
	World(unsigned maxContacts, unsigned iterations = 0)
	{
		maxContacts = maxContacts;
	};
	~World() {};

	unsigned generateContacts();
	void integrate(real duration);
	void runPhysics(real duration);
	void startFrame();

	ForceRegistry& getForceRegistry() {
		return	registry;
	};
};
