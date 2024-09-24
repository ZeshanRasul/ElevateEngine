#pragma once
#include "Physics/particle.h"
#include "GameObjects/Sphere.h"

class FloatingSphere {
public:

	FloatingSphere() : sphere(nullptr)
	{}

	void SetSphere(Sphere* sph) {
		sphere = sph;
	}

	void SetParticle(elevate::Particle* p) {
		particle = p;
	}

	elevate::Particle* GetParticle() const {
		return particle;
	}

	void render(glm::mat4 view, glm::mat4 proj) {
		elevate::Vector3 pos = particle->getPosition();
		sphere->SetPosition(pos);
		sphere->Draw(view, proj);
	}

private:
	elevate::Particle* particle;
	Sphere* sphere;
};

