#pragma once

#include "pcontacts.h"

namespace elevate {

	class ParticleLink : public ParticleContactGenerator
	{
	public:
		Particle* particle[2];

	protected:
		real currentLength() const;
	};

	class ParticleCable : public ParticleLink
	{
	public:
		real maxLength;
		real restitution;

	public:
		virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;
	};
	

	class ParticleRod : public ParticleLink
	{
	public:
		real length;

	public:
		virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;
	};
}