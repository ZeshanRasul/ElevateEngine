#include "Physics/pworld.h"

using namespace elevate;

ParticleWorld::ParticleWorld(unsigned maxContacts, unsigned iterations)
	:
	resolver(iterations),
	maxContacts(maxContacts),
	contacts(new ParticleContact[maxContacts]),
	calculateIterations(iterations == 0)
{}

ParticleWorld::~ParticleWorld()
{
	delete[] contacts;
}

void ParticleWorld::startFrame()
{
	for (Particles::iterator p = particles.begin(); p != particles.end(); p++)
	{
		(*p)->clearAccumulator();
	}
}

unsigned ParticleWorld::generateContacts()
{
	unsigned limit = maxContacts;
	ParticleContact* nextContact = contacts;

	for (ContactGenerators::iterator g = contactGenerators.begin(); g != contactGenerators.end(); g++)
	{
		unsigned used = (*g)->addContact(nextContact, limit);
		limit -= used;
		nextContact += used;

		if (limit <= 0) break;
	}

	return maxContacts - limit;
}

void ParticleWorld::integrate(real duration)
{
	for (Particles::iterator p = particles.begin(); p != particles.end(); p++)
	{
		(*p)->integrate(duration);
	}
}

void ParticleWorld::runPhysics(real duration)
{
	registry.updateForces(duration);

	integrate(duration);

	unsigned usedContacts = generateContacts();

	if (usedContacts)
	{ 
		if (calculateIterations)
		{
			resolver.setIterations(usedContacts * 2);
		}
		resolver.resolveContacts(contacts, usedContacts, duration);
	}
}

ParticleWorld::Particles& ParticleWorld::getParticles()
{
	return particles;
}

ParticleWorld::ContactGenerators& ParticleWorld::getContactGenerators()
{
	return contactGenerators;
}

ParticleForceRegistry& ParticleWorld::getForceRegistry()
{
	return registry;
}

ParticleContactResolver& ParticleWorld::getResolver()
{
	return resolver;
}

ParticleClothContactGenerator::ParticleClothContactGenerator(std::vector<Particle*>& particles, float minDist)
	: 
	particles(particles), minDistance(minDist)
{}

unsigned ParticleClothContactGenerator::addContact(ParticleContact* contact, unsigned limit) const
{
	unsigned numContacts = 0;

	// Loop through all particles and detect proximity-based contacts
	for (size_t i = 0; i < particles.size(); ++i) {
		for (size_t j = i + 1; j < particles.size(); ++j) {
			elevate::Particle* p1 = particles[i];
			elevate::Particle* p2 = particles[j];

			// Check distance between particles
			elevate::Vector3 diff = p1->getPosition() - p2->getPosition();
			float dist = diff.magnitude();

			if (dist < minDistance && numContacts < limit) {
				// Fill the contact structure
				contact->particle[0] = p1;
				contact->particle[1] = p2;
				diff.normalize();
				contact->contactNormal = diff;
				contact->penetration = minDistance - dist;
				contact->restitution = 0.2f;  // Some restitution for bounce effect

				// Move to the next contact
				contact++;
				numContacts++;
			}
		}
	}

	return numContacts;
}


