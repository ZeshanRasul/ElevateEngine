#pragma once
#include <vector>

#include "pfgen.h"
#include "plinks.h"

namespace elevate {

	class ParticleWorld
	{
	public:
		typedef std::vector<Particle*> Particles;
		typedef std::vector<ParticleContactGenerator*> ContactGenerators;

	protected:
		Particles particles;

		bool calculateIterations;

		ParticleForceRegistry registry;
		ParticleContactResolver resolver;
		ContactGenerators contactGenerators;
		ParticleContact* contacts;

		unsigned maxContacts;

	public:
		ParticleWorld(unsigned maxContacts, unsigned iterations = 0);
		~ParticleWorld();

		unsigned generateContacts();
		void integrate(real duration);
		void runPhysics(real duration);
		void startFrame();

		Particles& getParticles();
		ContactGenerators& getContactGenerators();
		ParticleForceRegistry& getForceRegistry();
		ParticleContactResolver& getResolver();
	};

	class ParticleClothContactGenerator : public ParticleContactGenerator
	{
	private:
		std::vector<Particle*> particles;
		float minDistance;

	public:
		ParticleClothContactGenerator(std::vector<Particle*>& particles, float minDist);
		virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;
	};
}