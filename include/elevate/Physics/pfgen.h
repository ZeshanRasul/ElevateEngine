#pragma once

#include <vector>
#include "core.h"
#include "particle.h"

namespace elevate {

	class ParticleForceGenerator {
	public:
		virtual void updateForce(Particle* particle, real duration) = 0;
	};


	class ParticleForceRegistry {
	protected:
		struct ParticleForceRegistration {
			Particle* particle;
			ParticleForceGenerator* forceGen;
		};

		typedef std::vector<ParticleForceRegistration> Registry;
		Registry registrations;

	public:
		void add(Particle* particle, ParticleForceGenerator* forceGen);

		void remove(Particle* particle, ParticleForceGenerator* forceGen);

		void clear();

		void updateForces(real duration);
	};
};