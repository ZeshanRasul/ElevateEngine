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

	class ParticleGravity : public ParticleForceGenerator {
	private:
		Vector3 gravity;

	public:
		ParticleGravity(const Vector3& gravity);

		virtual void updateForce(Particle* particle, real duration);
	};
	

	class ParticleDrag : public ParticleForceGenerator {
	private:
		real k1;
		real k2;

	public:
		ParticleDrag(real k1, real k2);

		virtual void updateForce(Particle* particle, real duration);
	};
	}
};