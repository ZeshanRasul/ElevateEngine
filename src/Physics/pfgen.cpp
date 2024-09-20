#include "Physics/pfgen.h"

using namespace elevate;

void ParticleForceRegistry::add(Particle* particle, ParticleForceGenerator* forceGen)
{
	ParticleForceRegistry::ParticleForceRegistration registration;
	registration.particle = particle;
	registration.forceGen = forceGen;
	registrations.push_back(registration);
}

void ParticleForceRegistry::remove(Particle* particle, ParticleForceGenerator* forceGen)
{
	Registry::iterator i = registrations.begin();
	for (; i != registrations.end(); i++) {
		if (i->particle == particle && i->forceGen == forceGen) {
			registrations.erase(i);
			return;
		}
	}
}

void ParticleForceRegistry::clear()
{
	registrations.clear();
}

void ParticleForceRegistry::updateForces(real duration)
{
	Registry::iterator i = registrations.begin();
	for (; i != registrations.end(); i++) {
		i->forceGen->updateForce(i->particle, duration);
	}
}
