#pragma once
#include <vector>
#include "elevate/Physics/body.h"
#include "elevate/Physics/fgen.h"
#include "elevate/Physics/contacts.h"
#include "elevate/Physics/CollideNarrow.h"

class World
{
public:
	typedef std::vector<RigidBody*> RigidBodies;

protected:
	RigidBodies bodies;

	bool calculateIterations;

	CollisionData collisionData;
	ContactResolver resolver;

	struct BodyRegistration
	{
		RigidBody* body;
		BodyRegistration* next;
	};
	unsigned maxContacts;
	Contact* contacts;
	BodyRegistration* firstBody;
	ForceRegistry registry;

public:
	World(unsigned maxContacts, unsigned iterations = 0)
		:
		firstBody(NULL),
		resolver(iterations),
		firstContactGen(NULL),
		maxContacts(maxContacts)
	{
		maxContacts = maxContacts;
	};
	~World() {};

	unsigned generateContacts();
	void integrate(real duration);
	void runPhysics(real duration);
	void startFrame();
	void addBody(RigidBody* body)
	{
		BodyRegistration* reg = new BodyRegistration;
		reg->body = body;
		reg->next = firstBody;
		firstBody = reg;
		bodies.push_back(body);
	};

	void removeBody(RigidBody* body)
	{
		bodies.erase(std::remove(bodies.begin(), bodies.end(), body), bodies.end());
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
