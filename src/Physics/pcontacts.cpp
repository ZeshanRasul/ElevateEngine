#include "Physics/pcontacts.h"

using namespace elevate;

void ParticleContact::resolve(real duration)
{
	resolveVelocity(duration);
	resolveInterpenetration(duration);
}

real ParticleContact::calculateSeparatingVelocity() const
{
	Vector3 relativeVelocity = particle[0]->getVelocity();

	if (particle[1])
	{
		relativeVelocity -= particle[1]->getVelocity();
	}

	return relativeVelocity * contactNormal;
}

void ParticleContact::resolveVelocity(real duration)
{
	real separatingVelocity = calculateSeparatingVelocity();

	if (separatingVelocity > 0)
	{
		// The contact is either separating, or stationary - no impulse is required.
		return;
	}

	real newSepVelocity = -separatingVelocity * restitution;

	// Check the velocity build-up due to acceleration only
	Vector3 accCausedVelocity = particle[0]->getAcceleration();
	if (particle[1])
	{
		accCausedVelocity -= particle[1]->getAcceleration();
	}
	real accCausedSepVelocity = accCausedVelocity * contactNormal * duration;

	// If we have a closing velocity due to acceleration build-up, remove it from the new separating velocity
	if (accCausedSepVelocity < 0)
	{
		newSepVelocity += restitution * accCausedSepVelocity;

		if (newSepVelocity < 0)
		{
			newSepVelocity = 0;
		}
	}

	real deltaVelocity = newSepVelocity - separatingVelocity;

	// We apply the change in velocity to each object in proportion to their inverse mass
	real totalInverseMass = particle[0]->getInverseMass();
	
	if (particle[1])
	{
		totalInverseMass += particle[1]->getInverseMass();
	}

	// If all particles have infinite mass, then impulses have no effect
	if (totalInverseMass <= 0)
	{
		return;
	}

	//Calculate the impulse to apply
	real impulse = deltaVelocity / totalInverseMass;

	// Find the amount of impulse per unit of inverse mass
	Vector3 impulsePerIMass = contactNormal * impulse;

	// Apply impulses: they are applied in the direction of the contact, and are proportional to the inverse mass
	particle[0]->setVelocity(particle[0]->getVelocity() + impulsePerIMass * particle[0]->getInverseMass());

	if (particle[1])
	{
		particle[1]->setVelocity(particle[1]->getVelocity() + impulsePerIMass * -particle[1]->getInverseMass());
	}
}

void ParticleContact::resolveInterpenetration(real duration)
{
	if (penetration <= 0) return;

	real totalInverseMass = particle[0]->getInverseMass();

	if (particle[1])
	{
		totalInverseMass += particle[1]->getInverseMass();
	}

	// If all particles have infinite mass, then we do nothing
	if (totalInverseMass <= 0) return;

	// Find the amount of penetration resolution per unit of inverse mass
	Vector3 movePerIMass = contactNormal * (penetration / totalInverseMass);

	// Calculate the movement amounts
	particleMovement[0] = movePerIMass * particle[0]->getInverseMass();

	if (particle[1])
	{
		particleMovement[1] = movePerIMass * -particle[1]->getInverseMass();
	} 
	else
	{
		particleMovement[1].clear();
	}

	// Apply the penetration resolution
	particle[0]->setPosition(particle[0]->getPosition() + particleMovement[0]);

	if (particle[1])
	{
		particle[1]->setPosition(particle[1]->getPosition() + particleMovement[1]);
	}
}

ParticleContactResolver::ParticleContactResolver(unsigned iterations) 
	: iterations(iterations)
{}

void ParticleContactResolver::setIterations(unsigned iterations)
{
	ParticleContactResolver::iterations = iterations;
}

void ParticleContactResolver::resolveContacts(ParticleContact* contactArray, unsigned numContacts, real duration)
{
	unsigned i;

	iterationsUsed = 0;
	while (iterationsUsed < iterations)
	{
		real max = REAL_MAX;
		unsigned maxIndex = numContacts;

		for (i = 0; i < numContacts; i++)
		{
			real sepVel = contactArray[i].calculateSeparatingVelocity();
			if (sepVel < max && (sepVel < 0 || contactArray[i].penetration > 0))
			{
				max = sepVel;
				maxIndex = i;
			}
		}

		if (maxIndex == numContacts) break;

		contactArray[maxIndex].resolve(duration);

		// Update the interpenetrations for all particles
		Vector3* move = contactArray[maxIndex].particleMovement;

		for (i = 0; i < numContacts; i++)
		{
			if (contactArray[i].particle[0] == contactArray[maxIndex].particle[0])
			{
				contactArray[i].penetration -= move[0] * contactArray[i].contactNormal;
			}
			else if (contactArray[i].particle[0] == contactArray[maxIndex].particle[1])
			{
				contactArray[i].penetration -= move[1] * contactArray[i].contactNormal;
			}

			if (contactArray[i].particle[1])
			{
				if (contactArray[i].particle[1] == contactArray[maxIndex].particle[0])
				{
					contactArray[i].penetration += move[0] * contactArray[i].contactNormal;
				}
				else if (contactArray[i].particle[1] == contactArray[maxIndex].particle[1])
				{
					contactArray[i].penetration += move[1] * contactArray[i].contactNormal;
				}
			}
		}

		iterationsUsed++;
	}
}

