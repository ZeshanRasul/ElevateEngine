#pragma once

#include "particle.h"

namespace elevate {

	class ParticleContactResolver;

	class ParticleContact {

		friend class ParticleContactResolver;

	public:
		Particle* particle[2];

		real restitution;
		Vector3 contactNormal;

		real penetration; //Interpenetration depth in the direction of the contact normal
		Vector3 particleMovement[2]; // Amount of movement required by each particle to resolve the interpenetration

	protected:
		void resolve(real duration);
		real calculateSeparatingVelocity() const;

	private:
		void resolveVelocity(real duration);
		void resolveInterpenetration(real duration);
	};

	class ParticleContactResolver {
	
	protected:
		unsigned iterations;
		unsigned iterationsUsed;

	public:
		ParticleContactResolver(unsigned iterations);
		
		void setIterations(unsigned iterations);
		void resolveContacts(ParticleContact* contactArray, unsigned numContacts, real duration);
	};

	class ParticleContactGenerator
	{
	public:
		virtual unsigned addContact(ParticleContact* contact, unsigned limit) const = 0;
	};
}