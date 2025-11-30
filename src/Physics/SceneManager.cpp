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

	// Reset game manager


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


	//	gm->ResetState();
	gm->showCar = true;

	gameObjects.clear();
	gameObjects.reserve(300);
	//	gameObjects.resize(300);

	gm->floor = gm->spawnFactory->CreateFloor(
		elevate::Vector3(1000.0f, 1.0f, 1000.0f),
		&gm->cubeShader,
		elevate::Vector3(0.0f, 0.0f, 0.0f)
	);
	gm->floor->mesh->setGameManager(gm);
	gm->floor->mesh->SetColor(glm::vec3(0.3f, 0.8f, 0.3f));
	static_cast<Cube*>(gm->floor->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Ground/TCom_Scifi_Floor2_4k_albedo.png");
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
		elevate::Vector3(0.0f, 0.0f, 0.0f)
	);
	gm->carBody->setPosition(elevate::Vector3(0.0f, 10.5f, 0.0f));
	gm->car->body = gm->carBody;
	gm->car->chassis = new CollisionBox();
	gm->car->chassis->body = gm->carBody;
	gm->car->chassis->halfSize = elevate::Vector3(2.3f, 1.5f, 4.0f);
	gm->car->chassis->body->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
	gm->car->chassis->body->getTransform().getAxisVector(3);

	gm->car->throttle = 0.0f;
	gm->car->brake = 0.0f;
	gm->car->steerAngle = 0.0f;
	gm->car->body->setMass(totalMass);
	elevate::Vector3 halfSize = gm->car->chassis->halfSize;

	gm->car->chassisMesh = new Cube(elevate::Vector3(0.0f, 10.5f, 0.0f), halfSize * 2, &gm->ammoShader, gm);
	gm->car->chassisMesh->LoadMesh();
	gm->car->chassisMesh->SetColor(glm::vec3(0.8f, 0.1f, 0.1f));
	gameObjects.push_back(gm->car->chassisMesh);

	real wheelRadius = 0.55f;
	real wheelWidth = 0.4f;

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
		w.coll = new elevate::CollisionSphere();
		w.coll->radius = wheelRadius;
		w.coll->offset.setOrientationAndPos(
			elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f),
			wheelOffsets[i]
		);
		w.offset = wheelOffsets[i];

		elevate::Vector3 worldPos = gm->car->body->getPointInWorldSpace(wheelOffsets[i]);

		w.mesh = new Sphere(worldPos, elevate::Vector3(wheelRadius, wheelRadius, wheelRadius), &gm->ammoShader, gm, glm::vec3(0.0f));
		w.mesh->SetColor(glm::vec3(0.1f, 0.1f, 0.1f));
		w.mesh->LoadMesh();
		gameObjects.push_back(w.mesh);
		w.coll->body = gm->car->body;
		//w.coll->body->setMass(wheelMass);
		w.coll->body->calculateDerivedData();
		w.coll->calculateInternals();
	}

	elevate::Vector3 carComWorld = gm->car->body->getTransform().getAxisVector(3);

	gm->car->roofOffset = elevate::Vector3(0.0f, halfSize.y * 0.7f, -halfSize.z * 0.3f);
	elevate::Vector3 roofSize(halfSize.x * 1.4f, halfSize.y * 0.7f, halfSize.z * 0.8f);

	{
		elevate::Vector3 roofWorldPos = gm->car->body->getPointInWorldSpace(gm->car->roofOffset);
		gm->car->roofMesh = new Cube(roofWorldPos, roofSize * 2.0f, &gm->ammoShader, gm);
		gm->car->roofMesh->LoadMesh();
		gm->car->roofMesh->SetColor(glm::vec3(0.8f, 0.8f, 0.85f));
		gameObjects.push_back(gm->car->roofMesh);
	}

	gm->car->hoodOffset = elevate::Vector3(0.0f, -halfSize.y * 0.2f, halfSize.z * 0.5f);
	elevate::Vector3 hoodSize(halfSize.x * 1.6f, halfSize.y * 0.5f, halfSize.z * 0.5f);

	{
		elevate::Vector3 hoodWorldPos = gm->car->body->getPointInWorldSpace(gm->car->hoodOffset);
		gm->car->hoodMesh = new Cube(hoodWorldPos, hoodSize * 2.0f, &gm->ammoShader, gm);
		gm->car->hoodMesh->LoadMesh();
		gm->car->hoodMesh->SetColor(glm::vec3(0.7f, 0.1f, 0.1f));
		gameObjects.push_back(gm->car->hoodMesh);
	}

	gm->car->rearOffset = elevate::Vector3(0.0f, -halfSize.y * 0.2f, -halfSize.z * 0.5f);
	elevate::Vector3 rearSize(halfSize.x * 1.5f, halfSize.y * 0.6f, halfSize.z * 0.5f);

	{
		elevate::Vector3 rearWorldPos = gm->car->body->getPointInWorldSpace(gm->car->rearOffset);
		gm->car->rearMesh = new Cube(rearWorldPos, rearSize * 2.0f, &gm->ammoShader, gm);
		gm->car->rearMesh->LoadMesh();
		gm->car->rearMesh->SetColor(glm::vec3(0.75f, 0.15f, 0.15f));
		gameObjects.push_back(gm->car->rearMesh);
	}

	gm->car->frontBumperOffset = elevate::Vector3(0.0f, -halfSize.y * 0.8f, halfSize.z * 0.95f);
	elevate::Vector3 bumperSize(halfSize.x * 1.7f, halfSize.y * 0.3f, halfSize.z * 0.15f);

	{
		elevate::Vector3 bumperWorldPos = gm->car->body->getPointInWorldSpace(gm->car->frontBumperOffset);
		gm->car->frontBumperMesh = new Cube(bumperWorldPos, bumperSize * 2.0f, &gm->ammoShader, gm);
		gm->car->frontBumperMesh->LoadMesh();
		gm->car->frontBumperMesh->SetColor(glm::vec3(0.2f, 0.2f, 0.25f));
		gameObjects.push_back(gm->car->frontBumperMesh);
	}

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

	return;
}

void Scenes::LoadAeroplaneTest(GameManager* gm)
{
	std::vector<GameObject*>& gameObjects = gm->GetGameObjects();


	//	gm->ResetState();
	gm->showPlane = true;

	gameObjects.clear();
	gameObjects.reserve(300);
	//	gameObjects.resize(300);

	gm->floor = gm->spawnFactory->CreateFloor(
		elevate::Vector3(500.0f, 1.0f, 500.0f),
		&gm->cubeShader,
		elevate::Vector3(0.0f, -1.0f, 0.0f)
	);
	gm->floor->mesh->setGameManager(gm);
	gm->floor->mesh->SetColor(glm::vec3(0.3f, 0.8f, 0.3f));
	static_cast<Cube*>(gm->floor->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Ground/TCom_Scifi_Floor2_512_albedo.png");
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
	gm->reset();
//	std::vector<GameObject*>& gameObjects = gm->GetGameObjects();
//
//
//	//gm->ResetState();
//
//	gm->fpsSandboxDemo = true;
//	gameObjects.clear();
//	gameObjects.reserve(300);
//	//	gameObjects.resize(300);
//
//	gm->floor = gm->spawnFactory->CreateFloor(
//		elevate::Vector3(200.0f, 1.0f, 200.0f),
//		&gm->cubeShader,
//		elevate::Vector3(0.0f, -1.0f, 0.0f)
//	);
//	gm->floor->mesh->setGameManager(gm);
//	gm->floor->mesh->SetColor(glm::vec3(0.3f, 0.8f, 0.3f));
//	static_cast<Cube*>(gm->floor->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Ground/TCom_Scifi_Floor2_512_albedo.png");
//	gameObjects.push_back(gm->floor->mesh);
//
//	gm->wall = gm->spawnFactory->CreateWall(
//		elevate::Vector3(1.0f, 60.0f, 200.0f),
//		elevate::Vector3(200.0f, 60.0f, 0.0f),
//		&gm->cubeShader
//	);
//	gm->wall->mesh->setGameManager(gm);
//	gm->wall->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
//	gm->wall->mesh->SetTexTiling(4.0f);
//	static_cast<Cube*>(gm->wall->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
//	gameObjects.push_back(gm->wall->mesh);
//
//	gm->wall2 = gm->spawnFactory->CreateWall(
//		elevate::Vector3(1.0f, 60.0f, 200.0f),
//		elevate::Vector3(-200.0f, 60.0f, 0.0f),
//		&gm->cubeShader
//	);
//	gm->wall2->mesh->setGameManager(gm);
//	gm->wall2->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
//	gm->wall2->mesh->SetTexTiling(4.0f);
//	static_cast<Cube*>(gm->wall2->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
//	gameObjects.push_back(gm->wall2->mesh);
//
//	gm->wall3 = gm->spawnFactory->CreateWall(
//		elevate::Vector3(200.0f, 60.0f, 1.0f),
//		elevate::Vector3(0.0f, 60.0f, 200.0f),
//		&gm->cubeShader
//	);
//	gm->wall3->mesh->setGameManager(gm);
//	gm->wall3->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
//	gm->wall3->mesh->SetTexTiling(4.0f);
//	static_cast<Cube*>(gm->wall3->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
//	gameObjects.push_back(gm->wall3->mesh);
//
//	gm->wall4 = gm->spawnFactory->CreateWall(
//		elevate::Vector3(200.0f, 60.0f, 1.0f),
//		elevate::Vector3(0.0f, 60.0f, -200.0f),
//		&gm->cubeShader
//	);
//	gm->wall4->mesh->setGameManager(gm);
//	gm->wall4->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
//	gm->wall4->mesh->SetTexTiling(4.0f);
//	static_cast<Cube*>(gm->wall4->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
//	gameObjects.push_back(gm->wall4->mesh);
//
//	//	gm->reset();
//
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

		//spawnContext.World->addBody(body);
		//spawnContext.World->getForceRegistry().add(body, rbGravity);

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
//
//	gm->spawnFactory->BuildDominoLine(
//		elevate::Vector3(75.0f, 3.5f, -150.0f),
//		elevate::Vector3(0.0f, 0.0f, 1.0f),
//		15,
//		elevate::Vector3(2.0f, 5.0f, 0.8f),
//		0.5f,
//		2.0f,
//		gm->dominoes);
//
//	for (PhysicsObject* domino : gm->dominoes)
//	{
//		elevate::Matrix3 tensor;
//		tensor.setBlockInertiaTensor(elevate::Vector3(2.0f, 5.0f, 0.8f), 0.5f);
//		domino->body->setInertiaTensor(tensor);
//		domino->body->calculateDerivedData();
//		domino->shape->calculateInternals();
//
//		Cube* cube = new Cube(
//			domino->body->getTransform().getAxisVector(3),
//			elevate::Vector3(2.0f, 5.0f, 0.8f),
//			&gm->ammoShader,
//			gm);
//		cube->LoadMesh();
//		domino->mesh = cube;
//
//		domino->mesh->setGameManager(gm);
//		domino->mesh->SetColor(glm::vec3(0.8f, 0.9f, 0.8f));
//		gameObjects.push_back(domino->mesh);
//	}
//
//
//
//	gm->ragdolls.push_back(gm->spawnFactory->CreateRagdoll(elevate::Vector3(0.0f, 4.0f, 0.0f)));
//	gm->ragdolls.push_back(gm->spawnFactory->CreateRagdoll(elevate::Vector3(10.0f, 4.0f, 0.0f)));
//	gm->ragdolls.push_back(gm->spawnFactory->CreateRagdoll(elevate::Vector3(20.0f, 4.0f, 0.0f)));
//
//	for (Ragdoll* ragdoll : gm->ragdolls)
//	{
//
//		for (Bone& bone : ragdoll->bones)
//		{
//			Cube* c = new Cube(bone.getTransform().getAxisVector(3), bone.halfSize * 2, &gm->ammoShader, gm);
//			c->LoadMesh();
//			c->SetColor(glm::vec3(0.9f, 0.3f, 0.4f));
//			bone.visual = c;
//			gameObjects.push_back(c);
//		}
//	}
//
//	gm->firstHit = true;
//
//	elevate::Vector3 blockPos(-50, 25, 50);
//	elevate::Vector3 blockScale(15, 15, 15);
//
//	Cube* cube = new Cube(blockPos, elevate::Vector3(15, 15, 15), &gm->cubeShader, gm);
//	cube->LoadMesh();
//	gm->blockTexture = cube->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Block/TCom_Concrete_WallAbstract_512_albedo.png");
//	cube->SetAngle(0.0f);
//	cube->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
//	cube->SetColor(glm::vec3(0.2f, 0.1f, 0.5f));
//	gm->blocks[0].visual = cube;
//	gameObjects.push_back(cube);
//
//	// Set the first block
//	// Set the first block
//	gm->blocks[0].halfSize = blockScale * 0.5f;
//	gm->blocks[0].body->setPosition(elevate::Vector3(blockPos));
//	gm->blocks[0].body->setOrientation(elevate::Quaternion(1, 0, 0, 0));
//	gm->blocks[0].body->setVelocity(elevate::Vector3(0, 0, 0));
//	gm->blocks[0].body->setRotation(elevate::Vector3(0, 0, 0));
//	real mass = 100.0f;
//	gm->blocks[0].body->setMass(mass);
//	elevate::Matrix3 it;
//	it.setBlockInertiaTensor(gm->blocks[0].halfSize, mass);
//	gm->blocks[0].body->setInertiaTensor(it);
//	gm->blocks[0].body->setDamping(0.9f, 0.9f);
//	gm->blocks[0].body->calculateDerivedData();
//	gm->blocks[0].calculateInternals();
//
//	gm->blocks[0].body->setAcceleration(elevate::Vector3::GRAVITY);
//	gm->blocks[0].body->setAwake(true);
//	gm->blocks[0].body->setCanSleep(true);
//	gm->blocks[0].exists = true;
//	//	spawnContext.World->addBody(blocks[0].body);
//
//		//	hit = false;
//
//			//elevate::real strength = -random.randomReal(500.0f, 1000.0f);
//			//for (unsigned i = 0; i < 12; i++)
//			//{
//			//	bones[i].body->addForceAtBodyPoint(
//			//		elevate::Vector3(strength, 0, 0), elevate::Vector3()
//			//	);
//			//}
//			//bones[6].body->addForceAtBodyPoint(
//			//	elevate::Vector3(strength, 0, random.randomBinomial(1000.0f)),
//			//	elevate::Vector3(random.randomBinomial(4.0f), random.randomBinomial(3.0f), 0)
//			//);
//
//			// Reset the contacts
//
//
//
//
//	gm->spawnFactory->BuildCrateStack(
//		elevate::Vector3(-10.0f, 10.0f, -40.0f),
//		3,
//		3,
//		3,
//		elevate::Vector3(3.1f, 3.1f, 3.1f),
//		1.0f,
//		&gm->ammoShader,
//		gm->crates
//	);
//
//	for (int i = 0; i < gm->crates.size(); ++i)
//	{
//		gm->crates[i]->mesh->SetShader(&gm->cubeShader);
//		gm->crates[i]->mesh->setGameManager(gm);
//		gm->crates[i]->mesh->SetColor(glm::vec3(0.3f, 0.7f, 0.2f));
//		static_cast<Cube*>(gm->crates[i]->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
//
//		gameObjects.push_back(gm->crates[i]->mesh);
//	}
//
//	gm->numStackCubes = 5;
//
//	elevate::Vector3 cubeHalfSize(2.0f, 2.0f, 2.0f);
//	glm::vec3       renderScale(4.0f, 4.0f, 4.0f);
//
//	for (int i = 0; i < gm->numStackCubes; i++)
//	{
//
//
//		//	rbWorld->removeBody(stackBodies[i]);
//
//		delete gm->cStackBoxes[i];
//
//		float centerY = 3.3f + i * 4.3f;
//
//		elevate::Vector3 pos(-20.0f, centerY, -20.0f);
//		elevate::Vector3 scale(renderScale.x, renderScale.y, renderScale.z);
//
//		// Render cube
//		Cube* cube = new Cube(pos, scale, &gm->cubeShader, gm);
//		cube->LoadMesh();
//		cube->SetAngle(0.0f);
//		cube->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
//		cube->SetColor(glm::vec3(0.2f, 0.7f, 0.3f));
//		gm->crateTexture = cube->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Crate/TCom_MetalSheetCladding2_Galvanized_512_albedo.png");
//		gameObjects.push_back(cube);
//		gm->stackCubes[i] = cube;
//
//		// Physics body
//		elevate::RigidBody* body = new elevate::RigidBody();
//		body->setAwake(true);
//		body->setPosition(pos);
//		body->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
//		body->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
//		body->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));
//
//		real mass = 1.0f;
//		body->setMass(mass);
//
//		elevate::Matrix3 boxInertia;
//		boxInertia.setBlockInertiaTensor(cubeHalfSize, mass);
//		body->setInertiaTensor(boxInertia);
//
//		//rbWorld->addBody(body);
//		//rbGravity->updateForce(body, 0); // or via registry below
//
////		rbWorld->getForceRegistry().add(body, rbGravity);
//
//		gm->stackBodies[i] = body;
//
//		// Collision box
//		elevate::CollisionBox* cBox = new elevate::CollisionBox();
//		cBox->body = body;
//		cBox->halfSize = cubeHalfSize;
//		cBox->body->calculateDerivedData();
//		cBox->calculateInternals();
//		gm->cStackBoxes[i] = cBox;
//	}
//
//
//
//	gm->spawnFactory->BuildBrickWall(
//		elevate::Vector3(0.0f, 5.75f, -80.0f),
//		15,
//		5,
//		elevate::Vector3(6.5f, 2.5f, 1.5f),
//		0.5f,
//		false,
//		gm->bricks,
//		&gm->cubeShader
//	);
//
//	gm->brickTexture = static_cast<Cube*>(gm->bricks[0]->mesh)->LoadTextureFromFile(
//		"C:/dev/ElevateEngine/src/Assets/Textures/Brick/TCom_Brick_Glazed2_512_albedo.png");
//
//	for (size_t i = 0; i < gm->bricks.size(); ++i)
//	{
//		gm->bricks[i]->mesh->SetShader(&gm->cubeShader);
//		static_cast<Cube*>(gm->bricks[i]->mesh)->SetTexture(gm->brickTexture);
//		gm->bricks[i]->mesh->setGameManager(gm);
//		gm->bricks[i]->mesh->SetColor(glm::vec3(0.6f, 0.4f, 0.2f));
//		gameObjects.push_back(gm->bricks[i]->mesh);
//	}
//
//
//
//	gm->crate = gm->spawnFactory->SpawnCrate(
//		elevate::Vector3(40.0f, 1.5f, 20.0f),
//		&gm->cubeShader,
//		elevate::Vector3(8.0f, 4.0f, 2.0f),
//		1.0f
//	);
//
//	gm->crate->mesh->SetShader(&gm->cubeShader);
//	static_cast<Cube*>(gm->crate->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Crate/TCom_MetalThreadplate4_Aged_512_albedo.png");
//	//static_cast<Cube*>(crate->mesh)->SetTexture(crateTexture);
//	gm->crate->mesh->setGameManager(gm);
//	gm->crate->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.1f));
//
//	gameObjects.push_back(gm->crate->mesh);
}