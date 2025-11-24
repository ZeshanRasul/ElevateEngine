#pragma once
#include <vector>
#include "elevate/Physics/body.h"
#include "elevate/Physics/fgen.h"
#include "elevate/Physics/contacts.h"
#include "elevate/Physics/CollideNarrow.h"

class World
{
public:
	typedef std::vector<elevate::RigidBody*> RigidBodies;

protected:
	RigidBodies bodies;

	bool calculateIterations;

	elevate::CollisionData collisionData;
	elevate::ContactResolver resolver;

	struct BodyRegistration
	{
		elevate::RigidBody* body;
		BodyRegistration* next;
	};
	unsigned maxContacts;
	Contact* contacts;
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
	void addBody(elevate::RigidBody* body)
	{
		BodyRegistration* reg = new BodyRegistration;
		reg->body = body;
		reg->next = firstBody;
		firstBody = reg;
		bodies.push_back(body);
	};

	ForceRegistry& getForceRegistry() {
		return	registry;
	};

	struct ContactGenRegistration
	{
		ContactGenerator* gen;
		ContactGenRegistration* next;
	};

	ContactGenRegistration* firstContactGen;

};
