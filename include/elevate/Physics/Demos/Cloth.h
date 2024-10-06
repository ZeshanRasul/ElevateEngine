#pragma once
#include <vector>

#include "Physics/particle.h"
#include "Physics/plinks.h"

#include "GameObjects/ClothMesh.h"

class Cloth {
public:
	Cloth(elevate::Vector3 pos, elevate::Vector3 scale, Shader* shdr, class GameManager* gameMgr, int width, int height);
	~Cloth();

	std::vector<elevate::Particle*> getParticles() const {
		return particles;
	};

	std::vector<elevate::Particle*> getFixedParticles() const {
		return fixedParticles;
	};

	std::vector<elevate::ParticleLink*> getConstraints() const {
		return constraints;
	};

	int getConstraintsSize() const {
		return constraints.size();
	}

	ClothMesh* getClothMesh() {
		return clothMesh;
	}

	void CreateClothMesh();
	void UpdateClothMesh();

private:
	void createCloth();
	void createConstraints();

private:
	const int NUM_PARTICLES_WIDTH = 40;
	const int NUM_PARTICLES_HEIGHT = 20;
	const float PARTICLE_DISTANCE = 0.5f;

	std::vector<elevate::Particle*> particles;
	std::vector<elevate::Particle*> fixedParticles;
	std::vector<elevate::ParticleLink*> constraints;

	ClothMesh* clothMesh;
};