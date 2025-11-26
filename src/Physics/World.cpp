#include "elevate/Physics/World.h"

void World::startFrame()
{
	for (RigidBodies::iterator p = bodies.begin(); p != bodies.end(); p++)
	{
		(*p)->clearAccumulator();
		(*p)->calculateDerivedData();
	}
}

unsigned World::generateContacts()
{
	unsigned limit = maxContacts;
	Contact* nextContact = contacts;


	ContactGenRegistration* reg = firstContactGen;
	while (reg)
	{
		unsigned used = reg->gen->addContact(nextContact, limit);
		limit -= used;
		nextContact += used;

		// We've run out of contacts to fill. This means we're missing
		// contacts.
		if (limit <= 0) break;

		reg = reg->next;
	}

	// Return the number of contacts used.
	return maxContacts - limit;
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
	BodyRegistration* reg = firstBody;
	while (reg)
	{
		// Remove all forces from the accumulator
		reg->body->integrate(duration);

		// Get the next registration
		reg = reg->next;
	}

	// Generate contacts
	unsigned usedContacts = generateContacts();

	// And process them
	if (calculateIterations) resolver.setIterations(usedContacts * 4, usedContacts * 4);
	resolver.resolveContacts(contacts, usedContacts, duration);

}