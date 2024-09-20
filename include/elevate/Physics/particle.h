#pragma once

#include "core.h"

namespace elevate {
	class Particle {
	public:
		Particle() : damping(1.0f), inverseMass(1.0f), position(Vector3()){};

		void integrate(real duration);
		
		void setDamping(const real newDamping);
		real getDamping() const;

		void setMass(const real mass);
		void setInverseMass(const real invMass);

		void setPosition(const Vector3& pos);
		void setPosition(const real x, const real y, const real z);
		Vector3 getPosition() const;

		void setVelocity(const Vector3& vel);
		void setVelocity(const real x, const real y, const real z);
		Vector3 getVelocity() const;

		void setAcceleration(const Vector3& acc);
		void setAcceleration(const real x, const real y, const real z);
		Vector3 getAcceleration() const;

		void clearAccumulator();

		void addForce(const Vector3& force);

	protected:

		/**
		 * Damping is required to remove energy added
		 * through numerical instability in the integrator.
		 */
		real damping; 
		
		real inverseMass;

		Vector3 position;
		Vector3 velocity;
		Vector3 acceleration;

		Vector3 forceAccum;
	};
}