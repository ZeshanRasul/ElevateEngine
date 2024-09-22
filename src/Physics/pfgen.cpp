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

ParticleGravity::ParticleGravity(const Vector3& gravity)
	: gravity(gravity)
{}

void ParticleGravity::updateForce(Particle * particle, real duration)
{
	if (!particle->hasFiniteMass()) return;

	particle->addForce(gravity * particle->getMass());
}

ParticleDrag::ParticleDrag(real k1, real k2)
	: k1(k1), k2(k2)
{}
