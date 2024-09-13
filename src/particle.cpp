#include <assert.h>
#include "particle.h"

using namespace elevate;

void Particle::integrate(real duration)
{
	if (inverseMass <= 0.0f) return;

	assert(duration > 0.0);

	position.addScaledVector(velocity, duration);

	Vector3 resultingAcc = acceleration;

	velocity.addScaledVector(resultingAcc, duration);
	velocity *= real_pow(damping, duration);

	clearAccumulator();
}

void Particle::setDamping(const real newDamping)
{
	damping = newDamping;
}

real Particle::getDamping() const
{
	return damping;
}

void Particle::setMass(const real mass) {
	if (mass <= 0) {
		inverseMass = 0;
	}
	else {
		inverseMass = 1 / mass;
	}
}

void Particle::setInverseMass(const real invMass) {
	inverseMass = invMass;
}

void Particle::setPosition(const Vector3& pos)
{
	position = pos;
}

void Particle::setPosition(const real x, const real y, const real z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

Vector3 Particle::getPosition() const
{
	return position;
}

void Particle::setVelocity(const Vector3& vel)
{
	velocity = vel;
}

void Particle::setVelocity(const real x, const real y, const real z)
{
	velocity.x = x;
	velocity.y = y;
	velocity.z = z;
}

Vector3 Particle::getVelocity() const
{
	return velocity;
}

void Particle::setAcceleration(const Vector3& acc)
{
	acceleration = acc;
}

void Particle::setAcceleration(const real x, const real y, const real z)
{
	acceleration.x = x;
	acceleration.y = y;
	acceleration.z = z;
}

Vector3 Particle::getAcceleration() const
{
	return acceleration;
}

void Particle::clearAccumulator()
{
	forceAccum.clear();
}

