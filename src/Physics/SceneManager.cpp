#include "Physics/Demos/SceneManager.h"
#include "Physics/Factory.h"

void SceneManager::Init(GameManager* gm)
{
	s_gameManager = gm;
}

void SceneManager::LoadScene(SceneType type)
{
	if (!s_gameManager)
	{
		return;
	}

	switch (type)
	{
	case SceneType::Empty:
		break;

	case SceneType::Car:
		Scenes::LoadCarTest(s_gameManager);
		break;

	case SceneType::Aeroplane:
		Scenes::LoadAeroplaneTest(s_gameManager);
		break;

	case SceneType::Ragdoll:
		Scenes::LoadRagdollTest(s_gameManager);
		break;

	case SceneType::FractureWall:
		Scenes::LoadFractureWall(s_gameManager);
		break;

	case SceneType::DominoChain:
		Scenes::LoadDominoChain(s_gameManager);
		break;

	case SceneType::DemoShowcase:
		Scenes::LoadDemoShowcase(s_gameManager);
		break;
	default:
		break;
	}

	s_currentScene = type;
}

SceneType SceneManager::GetScene(SceneType type)
{
	return s_currentScene;
}

void Scenes::LoadCarTest(GameManager* gm)
{
	std::vector<GameObject*>& gameObjects = gm->GetGameObjects();

	gm->showCar = true;

	gameObjects.clear();
	gameObjects.reserve(300);

	gm->floor = gm->spawnFactory->CreateFloor(
		elevate::Vector3(1000.0f, 1.0f, 1000.0f),
		&gm->cubeShader,
		elevate::Vector3(0.0f, 0.0f, 0.0f)
	);
	gm->floor->mesh->setGameManager(gm);
	gm->floor->mesh->SetColor(glm::vec3(0.3f, 0.8f, 0.3f));
	static_cast<Cube*>(gm->floor->mesh)->LoadTextureFromFile("src/Assets/Textures/Ground/TCom_Pavement_RoadNew_SolidLineYellow_1K_albedo.png");
	gm->floor->mesh->SetTexTiling(100.0f);
	gm->floor->mesh->SetOrientation(glm::quat(0.707f, 0.0f, 0.707f, 0.0f));
	gameObjects.push_back(gm->floor->mesh);

	real chassisMass = 1200.0f;
	real wheelMass = 25.0f;
	real totalMass = chassisMass + 4.0f * wheelMass;

	gm->car = new Car();

	gm->carBody = new elevate::RigidBody();
	gm->carBody->setAwake(true);
	gm->carBody->setCanSleep(false);
	gm->carBody->getTransform().setOrientationAndPos(
		elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f),
		elevate::Vector3(0.0f, 7.5f, 0.0f)
	);
	gm->carBody->setPosition(elevate::Vector3(0.0f, 4.9f, 0.0f));
	gm->car->body = gm->carBody;
	gm->car->chassis = new CollisionBox();
	gm->car->chassis->body = gm->carBody;
	gm->car->chassis->halfSize = elevate::Vector3(3.3f, 2.5f, 4.0f);
	gm->car->chassis->body->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
	gm->car->chassis->body->getTransform().getAxisVector(3);

	gm->car->throttle = 0.0f;
	gm->car->brake = 0.0f;
	gm->car->steerAngle = 0.0f;
	gm->car->body->setMass(totalMass);
	elevate::Vector3 halfSize = gm->car->chassis->halfSize;

	real wheelRadius = 1.55f;
	real wheelWidth = 0.7f;

	real wheelOffsetX = halfSize.x + wheelRadius * 0.05f;
	real wheelOffsetZ = halfSize.z + wheelRadius * 0.05f;

	Vector3 wheelOffsets[4] =
	{
		Vector3(-wheelOffsetX, -halfSize.y,  wheelOffsetZ),
		Vector3(wheelOffsetX, -halfSize.y,  wheelOffsetZ),
		Vector3(-wheelOffsetX, -halfSize.y, -wheelOffsetZ),
		Vector3(wheelOffsetX, -halfSize.y, -wheelOffsetZ)
	};

	for (int i = 0; i < 4; ++i)
	{
		Car::Wheel& w = gm->car->wheels[i];

		w.offset = wheelOffsets[i];

		elevate::Vector3 worldPos = gm->car->body->getPointInWorldSpace(wheelOffsets[i]);

		w.mesh = new Sphere(
			worldPos,
			elevate::Vector3(wheelWidth, wheelRadius, wheelRadius),
			&gm->ammoShader,
			gm,
			glm::vec3(0.0f)
		);
		w.mesh->SetColor(glm::vec3(0.1f, 0.1f, 0.1f));
		w.mesh->LoadMesh();
		gameObjects.push_back(w.mesh);
	}

	elevate::Vector3 carComWorld = gm->car->body->getTransform().getAxisVector(3);

	auto AddCarPart = [&](const elevate::Vector3& localOffset,
		const elevate::Vector3& size,
		const glm::vec3& color) -> Cube*
		{
			elevate::Vector3 worldPos = gm->car->body->getPointInWorldSpace(localOffset);

			Cube* part = new Cube(worldPos, size, &gm->ammoShader, gm);
			part->LoadMesh();
			part->SetColor(color);

			CarVisuals visual;
			visual.offset = localOffset;
			visual.size = size;
			visual.color = color;
			visual.mesh = part;

			gm->car->visualParts.push_back(visual);
			gameObjects.push_back(part);

			return part;
		};

	gm->car->chassisMesh = AddCarPart(
		elevate::Vector3(0.0f, -0.35f, 0.0f),
		elevate::Vector3(6.8f, 1.1f, 8.4f),
		glm::vec3(0.75f, 0.05f, 0.04f)
	);

	AddCarPart(
		elevate::Vector3(0.0f, 0.55f, 1.7f),
		elevate::Vector3(5.8f, 0.75f, 3.8f),
		glm::vec3(0.9f, 0.08f, 0.06f)
	);

	AddCarPart(
		elevate::Vector3(0.0f, 0.65f, -2.4f),
		elevate::Vector3(5.9f, 0.85f, 3.0f),
		glm::vec3(0.65f, 0.04f, 0.04f)
	);

	AddCarPart(
		elevate::Vector3(0.0f, 1.55f, -0.65f),
		elevate::Vector3(4.6f, 1.9f, 3.4f),
		glm::vec3(0.8f, 0.08f, 0.06f)
	);

	AddCarPart(
		elevate::Vector3(0.0f, 1.65f, 0.25f),
		elevate::Vector3(4.8f, 1.15f, 1.4f),
		glm::vec3(0.1f, 0.13f, 0.16f)
	);

	AddCarPart(
		elevate::Vector3(0.0f, 1.65f, -1.55f),
		elevate::Vector3(4.5f, 1.05f, 1.2f),
		glm::vec3(0.08f, 0.1f, 0.13f)
	);

	AddCarPart(
		elevate::Vector3(0.0f, 2.55f, -0.65f),
		elevate::Vector3(4.2f, 0.35f, 2.6f),
		glm::vec3(0.68f, 0.04f, 0.04f)
	);

	AddCarPart(
		elevate::Vector3(0.0f, -0.75f, 4.35f),
		elevate::Vector3(7.3f, 0.55f, 0.55f),
		glm::vec3(0.08f, 0.08f, 0.09f)
	);

	AddCarPart(
		elevate::Vector3(0.0f, -0.75f, -4.35f),
		elevate::Vector3(7.1f, 0.55f, 0.55f),
		glm::vec3(0.08f, 0.08f, 0.09f)
	);

	AddCarPart(
		elevate::Vector3(-3.65f, -0.4f, 0.0f),
		elevate::Vector3(0.35f, 0.5f, 6.8f),
		glm::vec3(0.05f, 0.05f, 0.06f)
	);

	AddCarPart(
		elevate::Vector3(3.65f, -0.4f, 0.0f),
		elevate::Vector3(0.35f, 0.5f, 6.8f),
		glm::vec3(0.05f, 0.05f, 0.06f)
	);

	AddCarPart(
		elevate::Vector3(-1.9f, -0.15f, 4.85f),
		elevate::Vector3(1.0f, 0.35f, 0.15f),
		glm::vec3(1.0f, 0.9f, 0.45f)
	);

	AddCarPart(
		elevate::Vector3(1.9f, -0.15f, 4.85f),
		elevate::Vector3(1.0f, 0.35f, 0.15f),
		glm::vec3(1.0f, 0.9f, 0.45f)
	);

	AddCarPart(
		elevate::Vector3(-2.0f, -0.1f, -4.85f),
		elevate::Vector3(0.9f, 0.35f, 0.15f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	);

	AddCarPart(
		elevate::Vector3(2.0f, -0.1f, -4.85f),
		elevate::Vector3(0.9f, 0.35f, 0.15f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	);

	real a = halfSize.x;
	real b = halfSize.y;
	real c = halfSize.z;

	real m = totalMass;

	gm->car->body->setMass(m);

	real ix = (real(1.0 / 3.0)) * m * (b * b + c * c);
	real iy = (real(1.0 / 3.0)) * m * (a * a + c * c);
	real iz = (real(1.0 / 3.0)) * m * (a * a + b * b);

	Matrix3 inertia;
	inertia.setDiagonal(ix, iy, iz);
	gm->car->body->setInertiaTensor(inertia);
	gm->car->body->setAngularDamping(0.8f);
	gm->car->body->setLinearDamping(0.99f);

	gm->car->body->getTransform();

	gm->car->body->calculateDerivedData();
	gm->car->chassis->calculateInternals();

	gm->spawnFactory->BuildDominoLine(
		elevate::Vector3(0.0f, 2.25f, 250.0f),
		elevate::Vector3(0.0f, 0.0f, 1.0f),
		8,
		elevate::Vector3(20.0f, 0.3f, 4.3f),
		2.5f,
		10.0f,
		gm->dominoes);

	for (PhysicsObject* domino : gm->dominoes)
	{
		int i = 0;
		elevate::Matrix3 tensor;
		tensor.setBlockInertiaTensor(elevate::Vector3(10.0f, 0.15f, 7.15f), 2.5f);
		domino->body->setInertiaTensor(tensor);
		domino->body->calculateDerivedData();
		domino->shape->calculateInternals();
		domino->name = "Domino " + std::to_string(i++);
		DebugDrawCollisionBox(*static_cast<CollisionBox*>(domino->shape), glm::vec3(1.0f, 0.0f, 0.0f));

		Cube* cube = new Cube(
			domino->body->getTransform().getAxisVector(3),
			elevate::Vector3(20.0f, 0.3f, 4.3f),
			&gm->ammoShader,
			gm);
		cube->LoadMesh();
		domino->mesh = cube;

		domino->mesh->setGameManager(gm);
		domino->mesh->SetColor(glm::vec3(0.8f, 0.9f, 0.8f));
		gameObjects.push_back(domino->mesh);
	}

	return;
}

void Scenes::LoadAeroplaneTest(GameManager* gm)
{
	std::vector<GameObject*>& gameObjects = gm->GetGameObjects();

	gm->showPlane = true;

	gameObjects.clear();
	gameObjects.reserve(300);

	gm->floor = gm->spawnFactory->CreateFloor(
		elevate::Vector3(500.0f, 1.0f, 500.0f),
		&gm->cubeShader,
		elevate::Vector3(0.0f, -1.0f, 0.0f)
	);
	gm->floor->mesh->setGameManager(gm);
	gm->floor->mesh->SetColor(glm::vec3(0.3f, 0.8f, 0.3f));
	static_cast<Cube*>(gm->floor->mesh)->LoadTextureFromFile("src/Assets/Textures/Ground/TCom_Scifi_Floor2_512_albedo.png");
	gameObjects.push_back(gm->floor->mesh);

	gm->ResetPlane();
}

void Scenes::LoadRagdollTest(GameManager* gm)
{
	// Set up ragdoll blast scene
}

void Scenes::LoadFractureWall(GameManager* gm)
{
	// Set up fracture wall scene
}

void Scenes::LoadDominoChain(GameManager* gm)
{
	// Set up domino chain scene
}

void Scenes::LoadDemoShowcase(GameManager* gm)
{
	gm->showCar = false;
	gm->showPlane = false;
	gm->reset();

	std::vector<GameObject*>& gameObjects = gm->GetGameObjects();

	gm->ammoCount = 32;

	for (int i = 0; i < gm->ammoCount; ++i)
	{
		elevate::Vector3 startPos(0.0f, -100.0f, 0.0f); // far below world

		elevate::Vector3 scale(1.0f, 1.0f, 1.0f);
		Sphere* s = new Sphere(startPos, scale, &gm->ammoShader, gm,
			glm::vec3(0.9f, 0.1f, 0.1f));
		s->GenerateSphere(0.5f, 16, 16);
		s->LoadMesh();
		gameObjects.push_back(s);

		elevate::RigidBody* body = new elevate::RigidBody();
		body->setAwake(false);
		body->setPosition(startPos);
		body->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		body->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		body->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));

		real mass = 1.7f;
		body->setMass(mass);
		body->setDamping(0.97f, 0.97f);

		real r = 0.5f;
		real coeff = (real)0.4f * mass * r * r;
		elevate::Matrix3 inertia;
		inertia.setInertiaTensorCoeffs(coeff, coeff, coeff);
		body->setInertiaTensor(inertia);

		elevate::CollisionSphere* cs = new elevate::CollisionSphere();
		cs->body = body;
		cs->radius = r;
		cs->body->calculateDerivedData();
		cs->calculateInternals();

		gm->ammoPool[i].visual = s;
		gm->ammoPool[i].body = body;
		gm->ammoPool[i].coll = cs;
		gm->ammoPool[i].active = false;
		gm->ammoPool[i].lifetime = 0.0f;


	}
}