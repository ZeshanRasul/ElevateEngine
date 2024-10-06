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
	
	class ParticleWindForce : public ParticleForceGenerator {
	private:
		Vector3 windDirection;

	public:
		ParticleWindForce(const Vector3& direction);

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
	
	class ParticleSpring : public ParticleForceGenerator {
	private:
		Particle* other;
		real springConstant;
		real restLength;

	public:
		ParticleSpring(Particle* other, real springConstant, real restLength);

		virtual void updateForce(Particle* particle, real duration);
	};

	class ParticleAnchoredSpring : public ParticleForceGenerator {
	protected:
		Vector3* anchor;
		real springConstant;
		real restLength;
		
	public:
		ParticleAnchoredSpring(Vector3* anchor, real springConstant, real restLength);

		virtual void updateForce(Particle* particle, real duration);
	};
	
	class ParticleAnchoredBungee : public ParticleAnchoredSpring {
	public:
		virtual void updateForce(Particle* particle, real duration);
	};

	class ParticleBungee : public ParticleForceGenerator {
	private:
		Particle* other;
		real springConstant;
		real restLength;

	public:
		ParticleBungee(Particle* other, real springConstant, real restLength);

		virtual void updateForce(Particle* particle, real duration);
	};

	class ParticleBuoyancy : public ParticleForceGenerator {
	private:
		real maxDepth;
		real volume;
		real waterHeight;
		real liquidDensity;
		
	public:
		ParticleBuoyancy(real maxDepth, real volume, real waterHeight, real liquidDensity = 1000.0f);

		void setWaterHeight(real height) { waterHeight = height; }

		virtual void updateForce(Particle* particle, real duration);
	};

	class ParticleFakeSpring : public ParticleForceGenerator {
	private:
		Vector3* anchor;
		real springConstant;
		real damping;

	public:
		ParticleFakeSpring(Vector3* anchor, real springConstant, real damping);

		virtual void updateForce(Particle* particle, real duration);
	};
};