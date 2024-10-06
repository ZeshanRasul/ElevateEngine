#include "Physics/Demos/Cloth.h"
#include "App/GameManager.h"

Cloth::Cloth(elevate::Vector3 pos, elevate::Vector3 scale, Shader* shdr, GameManager* gameMgr, int width, int height)
{
	createCloth();
	createConstraints();
	clothMesh = new ClothMesh(elevate::Vector3(0.0f, 0.0f, 0.0f), elevate::Vector3(1.0f, 1.0f, 1.0f), shdr, gameMgr, width, height);
	clothMesh->CreateAndUploadVertexBuffer();
}

Cloth::~Cloth()
{
	for (auto p : particles)
	{
		delete p;
	}

	for (auto p : fixedParticles)
	{
		delete p;
	}

	for (auto c : constraints)
	{
		delete c;
	}
}

void Cloth::createCloth()
{
    particles.resize(NUM_PARTICLES_WIDTH * NUM_PARTICLES_HEIGHT);

    for (int y = 0; y < NUM_PARTICLES_HEIGHT; ++y) {
        for (int x = 0; x < NUM_PARTICLES_WIDTH; ++x) {
            elevate::Particle* particle = new elevate::Particle();
            particle->setPosition(elevate::Vector3(
                x * PARTICLE_DISTANCE,
                -y * PARTICLE_DISTANCE,
                0.0f));
            particle->setMass(1.0f);
            particle->setVelocity(0, 0, 0);
            particle->setDamping(0.99f);
            particle->clearAccumulator();

            particles[y * NUM_PARTICLES_WIDTH + x] = particle;
        }
    }

    // Fix the top-left and top-right particles to simulate hanging cloth
    particles[0]->setMass(0); // Infinite mass (fixed)
    particles[NUM_PARTICLES_WIDTH - 1]->setMass(0);
    fixedParticles.push_back(particles[0]);
    fixedParticles.push_back(particles[NUM_PARTICLES_WIDTH - 1]);

}

void Cloth::createConstraints()
{    // Structural constraints (rods)
    for (int y = 0; y < NUM_PARTICLES_HEIGHT; ++y) {
        for (int x = 0; x < NUM_PARTICLES_WIDTH; ++x) {
            if (x < NUM_PARTICLES_WIDTH - 1) {
                // Horizontal rods
                elevate::ParticleRod* rod = new elevate::ParticleRod();
                rod->particle[0] = particles[y * NUM_PARTICLES_WIDTH + x];
                rod->particle[1] = particles[y * NUM_PARTICLES_WIDTH + x + 1];
                rod->length = PARTICLE_DISTANCE;
                constraints.push_back(rod);
            }
            if (y < NUM_PARTICLES_HEIGHT - 1) {
                // Vertical rods
                elevate::ParticleRod* rod = new elevate::ParticleRod();
                rod->particle[0] = particles[y * NUM_PARTICLES_WIDTH + x];
                rod->particle[1] = particles[(y + 1) * NUM_PARTICLES_WIDTH + x];
                rod->length = PARTICLE_DISTANCE;
                constraints.push_back(rod);
            }
            // Shear constraints
            if (x < NUM_PARTICLES_WIDTH - 1 && y < NUM_PARTICLES_HEIGHT - 1) {
                // Diagonal right-down
                elevate::ParticleRod* rod = new elevate::ParticleRod();
                rod->particle[0] = particles[y * NUM_PARTICLES_WIDTH + x];
                rod->particle[1] = particles[(y + 1) * NUM_PARTICLES_WIDTH + x + 1];
                rod->length = sqrt(2 * PARTICLE_DISTANCE * PARTICLE_DISTANCE);
                constraints.push_back(rod);

                // Diagonal left-down
                rod = new elevate::ParticleRod();
                rod->particle[0] = particles[(y + 1) * NUM_PARTICLES_WIDTH + x];
                rod->particle[1] = particles[y * NUM_PARTICLES_WIDTH + x + 1];
                rod->length = sqrt(2 * PARTICLE_DISTANCE * PARTICLE_DISTANCE);
                constraints.push_back(rod);
            }
        }
    }
}
