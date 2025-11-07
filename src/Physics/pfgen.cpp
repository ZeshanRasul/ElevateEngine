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

void ParticleDrag::updateForce(Particle * particle, real duration)
{
	Vector3	force = particle->getVelocity();

	// Calculate the total drag coefficient.
	real dragCoeff = force.magnitude();
	dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;

	// Calculate the final force and apply it.
	force.normalize();
	force *= -dragCoeff;
	particle->addForce(force);
}

ParticleSpring::ParticleSpring(Particle* other, real springConstant, real restLength)
	: other(other), springConstant(springConstant), restLength(restLength)
{}

void ParticleSpring::updateForce(Particle * particle, real duration)
{
	Vector3 force = particle->getPosition();
	force -= other->getPosition();

	// Calculate the magnitude of the force.
	real magnitude = force.magnitude();
	magnitude = real_abs(magnitude - restLength);
	magnitude *= springConstant;

	// Calculate the final force and apply it.
	force.normalize();
	force *= -magnitude;
	particle->addForce(force);
}

ParticleAnchoredSpring::ParticleAnchoredSpring(Vector3 anchor, real springConstant, real restLength)
	: anchor(anchor), springConstant(springConstant), restLength(restLength)
{}

void ParticleAnchoredSpring::updateForce(Particle * particle, real duration)
{
	Vector3 force = particle->getPosition();
	force -= anchor;

	// Calculate the magnitude of the force.
	real magnitude = force.magnitude();
	magnitude = (restLength - magnitude);
	magnitude *= springConstant;

	// Calculate the final force and apply it.
	force.normalize();
	force *= magnitude;
	particle->addForce(force);
}

ParticleBungee::ParticleBungee(Particle* other, real springConstant, real restLength)
	: other(other), springConstant(springConstant), restLength(restLength)
{}

void ParticleBungee::updateForce(Particle * particle, real duration)
{
	Vector3 force = particle->getPosition();
	force -= other->getPosition();

	// Check if the bungee is compressed.
	real magnitude = force.magnitude();
	if (magnitude <= restLength) return;

	// Calculate the magnitude of the force.
	magnitude = (restLength - magnitude);
	magnitude *= springConstant;

	// Calculate the final force and apply it.
	force.normalize();
	force *= -magnitude;
	particle->addForce(force);
}

void ParticleAnchoredBungee::updateForce(Particle* particle, real duration)
{
	Vector3 force = particle->getPosition();
	force -= anchor;

	// Calculate the magnitude of the force.
	real magnitude = force.magnitude();
	if (magnitude < restLength) return;

	magnitude = magnitude - restLength;
	magnitude *= springConstant;

	// Calculate the final force and apply it.
	force.normalize();
	force *= -magnitude;
	particle->addForce(force);
}

ParticleBuoyancy::ParticleBuoyancy(real maxDepth, real volume, real waterHeight, real liquidDensity)
	: maxDepth(maxDepth), volume(volume), waterHeight(waterHeight), liquidDensity(liquidDensity)
{}

void ParticleBuoyancy::updateForce(Particle * particle, real duration)
{
	// Calculate the submersion depth.
	real depth = particle->getPosition().y;

	// Check if we're out of the water.
	if (depth >= waterHeight + maxDepth) return;
	Vector3 force(0, 0, 0);

	// Check if we're at maximum depth.
	if (depth <= waterHeight - maxDepth)
	{
		force.y = liquidDensity * volume;
		particle->addForce(force);
		return;
	}

	// Otherwise we are partly submerged.
	force.y = liquidDensity * volume * (depth - maxDepth - waterHeight) / 2 * maxDepth;
	particle->addForce(force);
}

ParticleFakeSpring::ParticleFakeSpring(Vector3* anchor, real springConstant, real damping)
	: anchor(anchor), springConstant(springConstant), damping(damping)
{}

void ParticleFakeSpring::updateForce(Particle * particle, real duration)
{
	// Check that the particle does not have infinite mass.
	if (!particle->hasFiniteMass()) return;

	// Calculate the relative position of the particle to the anchor.
	Vector3 position = particle->getPosition();
	position -= *anchor;

	// Calculate the constants and check if they are in bounds.
	real gamma = 0.5f * real_sqrt(4 * springConstant - damping * damping);
	if (gamma == 0.0f) return;
	Vector3 c = position * (damping / (2.0f * gamma)) + particle->getVelocity() * (1.0f / gamma);

	// Calculate the target position.
	Vector3 target = position * real_cos(gamma * duration) + c * real_sin(gamma * duration);
	target *= real_exp(-0.5f * duration * damping);

	// Calculate the resulting acceleration and therefore the force.
	Vector3 accel = (target - position) * ((real)1.0 / (duration * duration)) - particle->getVelocity() * ((real)1.0 / duration);
	particle->addForce(accel * particle->getMass());
}
