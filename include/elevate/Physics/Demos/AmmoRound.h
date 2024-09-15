#pragma once
#include "Physics/particle.h"
#include "GameObjects/Sphere.h"


enum ShotType {
	UNUSED = 0,
	PISTOL,
	ARTILLERY,
	FIREBALL,
	LASER
};

class AmmoRound {
public:

	AmmoRound() : type(UNUSED), sphere(nullptr)
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

	void SetType(ShotType shotType) {
		type = shotType;
	}

	ShotType GetType() const {
		return type;
	}

	void SetStartTime(double time) {
		startTime = time;
	}

	double GetStartTime() const {
		return startTime;
	}

	void render(glm::mat4 view, glm::mat4 proj) {
		elevate::Vector3 pos = particle->getPosition();
		sphere->SetPosition(pos);
		sphere->Draw(view, proj);
	}

private:
	elevate::Particle* particle;
	ShotType type = UNUSED;
	double startTime;
	Sphere* sphere;
};

