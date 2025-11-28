#include "SceneManager.h"
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
		elevate::Vector3(500.0f, 1.0f, 500.0f),
		&gm->cubeShader,
		elevate::Vector3(0.0f, -1.0f, 0.0f)
	);
	gm->floor->mesh->setGameManager(gm);
	gm->floor->mesh->SetColor(glm::vec3(0.3f, 0.8f, 0.3f));
	static_cast<Cube*>(gm->floor->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Ground/TCom_Scifi_Floor2_512_albedo.png");
	gameObjects.push_back(gm->floor->mesh);

	CarVisuals carBody{};
	carBody.offset = elevate::Vector3(0.0f, -2.5f, 0.0f);
	carBody.mesh = new Cube(elevate::Vector3(0, 0, 0), elevate::Vector3(5.0f, 2.0f, 3.2f), &gm->ammoShader, gm);
	carBody.mesh->LoadMesh();
	carBody.mesh->SetColor(glm::vec3(0.2f, 0.2f, 0.8f));
	gm->carParts.push_back(carBody);
	CarVisuals wheelFL{};
	wheelFL.offset = elevate::Vector3(-1.25f, -1.1f, 1.5f);
	wheelFL.mesh = new Sphere(elevate::Vector3(0, 0, 0), elevate::Vector3(2.0f, 2.0f, 1.0f), &gm->ammoShader, gm,
		glm::vec3(0.1f, 0.1f, 0.1f));
	static_cast<Sphere*>(wheelFL.mesh)->GenerateSphere(0.5f, 16, 16);
	wheelFL.mesh->LoadMesh();
	gm->carParts.push_back(wheelFL);
	CarVisuals wheelFR{};
	wheelFR.offset = elevate::Vector3(1.25f, -1.1f, 1.5f);
	wheelFR.mesh = new Sphere(elevate::Vector3(0, 0, 0), elevate::Vector3(2.0f, 2.0f, 1.0f), &gm->ammoShader, gm,
		glm::vec3(0.1f, 0.1f, 0.1f));
	static_cast<Sphere*>(wheelFR.mesh)->GenerateSphere(0.5f, 16, 16);
	wheelFR.mesh->LoadMesh();
	gm->carParts.push_back(wheelFR);
	CarVisuals wheelRL{};
	wheelRL.offset = elevate::Vector3(-1.25f, -1.1f, -1.5f);
	wheelRL.mesh = new Sphere(elevate::Vector3(0, 0, 0), elevate::Vector3(2.0f, 2.0f, 1.0f), &gm->ammoShader, gm,
		glm::vec3(0.1f, 0.1f, 0.1f));
	static_cast<Sphere*>(wheelRL.mesh)->GenerateSphere(0.5f, 16, 16);
	wheelRL.mesh->LoadMesh();
	gm->carParts.push_back(wheelRL);
	CarVisuals wheelRR{};
	wheelRR.offset = elevate::Vector3(1.25f, -1.1f, -1.5f);
	wheelRR.mesh = new Sphere(elevate::Vector3(0, 0, 0), elevate::Vector3(2.0f, 2.0f, 1.0f), &gm->ammoShader, gm,
		glm::vec3(0.1f, 0.1f, 0.1f));
	static_cast<Sphere*>(wheelRR.mesh)->GenerateSphere(0.5f, 16, 16);
	wheelRR.mesh->LoadMesh();
	gm->carParts.push_back(wheelRR);

	elevate::Vector3 carPropulsion(gm->car_throttle * 50.0f, 0, 0);
	//car.addForce(carPropulsion);
	gm->carEngine->setThrottle(carPropulsion);

	gm->spawnContext.World->getForceRegistry().add(&gm->car, gm->carEngine);
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
	std::vector<GameObject*>& gameObjects = gm->GetGameObjects();


	//gm->ResetState();

	gm->fpsSandboxDemo = true;
	gameObjects.clear();
	gameObjects.reserve(300);
	//	gameObjects.resize(300);

	gm->floor = gm->spawnFactory->CreateFloor(
		elevate::Vector3(200.0f, 1.0f, 200.0f),
		&gm->cubeShader,
		elevate::Vector3(0.0f, -1.0f, 0.0f)
	);
	gm->floor->mesh->setGameManager(gm);
	gm->floor->mesh->SetColor(glm::vec3(0.3f, 0.8f, 0.3f));
	static_cast<Cube*>(gm->floor->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Ground/TCom_Scifi_Floor2_512_albedo.png");
	gameObjects.push_back(gm->floor->mesh);

	gm->wall = gm->spawnFactory->CreateWall(
		elevate::Vector3(1.0f, 60.0f, 200.0f),
		elevate::Vector3(200.0f, 60.0f, 0.0f),
		&gm->cubeShader
	);
	gm->wall->mesh->setGameManager(gm);
	gm->wall->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	gm->wall->mesh->SetTexTiling(4.0f);
	static_cast<Cube*>(gm->wall->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
	gameObjects.push_back(gm->wall->mesh);

	gm->wall2 = gm->spawnFactory->CreateWall(
		elevate::Vector3(1.0f, 60.0f, 200.0f),
		elevate::Vector3(-200.0f, 60.0f, 0.0f),
		&gm->cubeShader
	);
	gm->wall2->mesh->setGameManager(gm);
	gm->wall2->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	gm->wall2->mesh->SetTexTiling(4.0f);
	static_cast<Cube*>(gm->wall2->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
	gameObjects.push_back(gm->wall2->mesh);

	gm->wall3 = gm->spawnFactory->CreateWall(
		elevate::Vector3(200.0f, 60.0f, 1.0f),
		elevate::Vector3(0.0f, 60.0f, 200.0f),
		&gm->cubeShader
	);
	gm->wall3->mesh->setGameManager(gm);
	gm->wall3->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	gm->wall3->mesh->SetTexTiling(4.0f);
	static_cast<Cube*>(gm->wall3->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
	gameObjects.push_back(gm->wall3->mesh);

	gm->wall4 = gm->spawnFactory->CreateWall(
		elevate::Vector3(200.0f, 60.0f, 1.0f),
		elevate::Vector3(0.0f, 60.0f, -200.0f),
		&gm->cubeShader
	);
	gm->wall4->mesh->setGameManager(gm);
	gm->wall4->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	gm->wall4->mesh->SetTexTiling(4.0f);
	static_cast<Cube*>(gm->wall4->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
	gameObjects.push_back(gm->wall4->mesh);

//	gm->reset();

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

	gm->spawnFactory->BuildDominoLine(
		elevate::Vector3(75.0f, 3.5f, -150.0f),
		elevate::Vector3(0.0f, 0.0f, 1.0f),
		15,
		elevate::Vector3(2.0f, 5.0f, 0.8f),
		0.5f,
		2.0f,
		gm->dominoes);

	for (PhysicsObject* domino : gm->dominoes)
	{
		elevate::Matrix3 tensor;
		tensor.setBlockInertiaTensor(elevate::Vector3(2.0f, 5.0f, 0.8f), 0.5f);
		domino->body->setInertiaTensor(tensor);
		domino->body->calculateDerivedData();
		domino->shape->calculateInternals();

		Cube* cube = new Cube(
			domino->body->getTransform().getAxisVector(3),
			elevate::Vector3(2.0f, 5.0f, 0.8f),
			&gm->ammoShader,
			gm);
		cube->LoadMesh();
		domino->mesh = cube;

		domino->mesh->setGameManager(gm);
		domino->mesh->SetColor(glm::vec3(0.8f, 0.9f, 0.8f));
		gameObjects.push_back(domino->mesh);
	}



	gm->ragdolls.push_back(gm->spawnFactory->CreateRagdoll(elevate::Vector3(0.0f, 4.0f, 0.0f)));
	gm->ragdolls.push_back(gm->spawnFactory->CreateRagdoll(elevate::Vector3(10.0f, 4.0f, 0.0f)));
	gm->ragdolls.push_back(gm->spawnFactory->CreateRagdoll(elevate::Vector3(20.0f, 4.0f, 0.0f)));

	for (Ragdoll* ragdoll : gm->ragdolls)
	{

		for (Bone& bone : ragdoll->bones)
		{
			Cube* c = new Cube(bone.getTransform().getAxisVector(3), bone.halfSize * 2, &gm->ammoShader, gm);
			c->LoadMesh();
			c->SetColor(glm::vec3(0.9f, 0.3f, 0.4f));
			bone.visual = c;
			gameObjects.push_back(c);
		}
	}

	gm->firstHit = true;

	elevate::Vector3 blockPos(-50, 25, 50);
	elevate::Vector3 blockScale(15, 15, 15);

	Cube* cube = new Cube(blockPos, elevate::Vector3(15, 15, 15), &gm->cubeShader, gm);
	cube->LoadMesh();
	gm->blockTexture = cube->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Block/TCom_Concrete_WallAbstract_512_albedo.png");
	cube->SetAngle(0.0f);
	cube->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
	cube->SetColor(glm::vec3(0.2f, 0.1f, 0.5f));
	gm->blocks[0].visual = cube;
	gameObjects.push_back(cube);

	// Set the first block
	// Set the first block
	gm->blocks[0].halfSize = blockScale * 0.5f;
	gm->blocks[0].body->setPosition(elevate::Vector3(blockPos));
	gm->blocks[0].body->setOrientation(elevate::Quaternion(1, 0, 0, 0));
	gm->blocks[0].body->setVelocity(elevate::Vector3(0, 0, 0));
	gm->blocks[0].body->setRotation(elevate::Vector3(0, 0, 0));
	real mass = 100.0f;
	gm->blocks[0].body->setMass(mass);
	elevate::Matrix3 it;
	it.setBlockInertiaTensor(gm->blocks[0].halfSize, mass);
	gm->blocks[0].body->setInertiaTensor(it);
	gm->blocks[0].body->setDamping(0.9f, 0.9f);
	gm->blocks[0].body->calculateDerivedData();
	gm->blocks[0].calculateInternals();

	gm->blocks[0].body->setAcceleration(elevate::Vector3::GRAVITY);
	gm->blocks[0].body->setAwake(true);
	gm->blocks[0].body->setCanSleep(true);
	gm->blocks[0].exists = true;
	//	spawnContext.World->addBody(blocks[0].body);

		//	hit = false;

			//elevate::real strength = -random.randomReal(500.0f, 1000.0f);
			//for (unsigned i = 0; i < 12; i++)
			//{
			//	bones[i].body->addForceAtBodyPoint(
			//		elevate::Vector3(strength, 0, 0), elevate::Vector3()
			//	);
			//}
			//bones[6].body->addForceAtBodyPoint(
			//	elevate::Vector3(strength, 0, random.randomBinomial(1000.0f)),
			//	elevate::Vector3(random.randomBinomial(4.0f), random.randomBinomial(3.0f), 0)
			//);

			// Reset the contacts




	gm->spawnFactory->BuildCrateStack(
		elevate::Vector3(-10.0f, 10.0f, -40.0f),
		3,
		3,
		3,
		elevate::Vector3(3.1f, 3.1f, 3.1f),
		1.0f,
		&gm->ammoShader,
		gm->crates
	);

	for (int i = 0; i < gm->crates.size(); ++i)
	{
		gm->crates[i]->mesh->SetShader(&gm->cubeShader);
		gm->crates[i]->mesh->setGameManager(gm);
		gm->crates[i]->mesh->SetColor(glm::vec3(0.3f, 0.7f, 0.2f));
		static_cast<Cube*>(gm->crates[i]->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");

		gameObjects.push_back(gm->crates[i]->mesh);
	}

	gm->numStackCubes = 5;

	elevate::Vector3 cubeHalfSize(2.0f, 2.0f, 2.0f);
	glm::vec3       renderScale(4.0f, 4.0f, 4.0f);

	for (int i = 0; i < gm->numStackCubes; i++)
	{


		//	rbWorld->removeBody(stackBodies[i]);

		delete gm->cStackBoxes[i];

		float centerY = 3.3f + i * 4.3f;

		elevate::Vector3 pos(-20.0f, centerY, -20.0f);
		elevate::Vector3 scale(renderScale.x, renderScale.y, renderScale.z);

		// Render cube
		Cube* cube = new Cube(pos, scale, &gm->cubeShader, gm);
		cube->LoadMesh();
		cube->SetAngle(0.0f);
		cube->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
		cube->SetColor(glm::vec3(0.2f, 0.7f, 0.3f));
		gm->crateTexture = cube->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Crate/TCom_MetalSheetCladding2_Galvanized_512_albedo.png");
		gameObjects.push_back(cube);
		gm->stackCubes[i] = cube;

		// Physics body
		elevate::RigidBody* body = new elevate::RigidBody();
		body->setAwake(true);
		body->setPosition(pos);
		body->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		body->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		body->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));

		real mass = 1.0f;
		body->setMass(mass);

		elevate::Matrix3 boxInertia;
		boxInertia.setBlockInertiaTensor(cubeHalfSize, mass);
		body->setInertiaTensor(boxInertia);

		//rbWorld->addBody(body);
		//rbGravity->updateForce(body, 0); // or via registry below

//		rbWorld->getForceRegistry().add(body, rbGravity);

		gm->stackBodies[i] = body;

		// Collision box
		elevate::CollisionBox* cBox = new elevate::CollisionBox();
		cBox->body = body;
		cBox->halfSize = cubeHalfSize;
		cBox->body->calculateDerivedData();
		cBox->calculateInternals();
		gm->cStackBoxes[i] = cBox;
	}



	gm->spawnFactory->BuildBrickWall(
		elevate::Vector3(0.0f, 5.75f, -80.0f),
		15,
		5,
		elevate::Vector3(6.5f, 2.5f, 1.5f),
		0.5f,
		false,
		gm->bricks,
		&gm->cubeShader
	);

	gm->brickTexture = static_cast<Cube*>(gm->bricks[0]->mesh)->LoadTextureFromFile(
		"C:/dev/ElevateEngine/src/Assets/Textures/Brick/TCom_Brick_Glazed2_512_albedo.png");

	for (size_t i = 0; i < gm->bricks.size(); ++i)
	{
		gm->bricks[i]->mesh->SetShader(&gm->cubeShader);
		static_cast<Cube*>(gm->bricks[i]->mesh)->SetTexture(gm->brickTexture);
		gm->bricks[i]->mesh->setGameManager(gm);
		gm->bricks[i]->mesh->SetColor(glm::vec3(0.6f, 0.4f, 0.2f));
		gameObjects.push_back(gm->bricks[i]->mesh);
	}



	gm->crate = gm->spawnFactory->SpawnCrate(
		elevate::Vector3(40.0f, 1.5f, 20.0f),
		&gm->cubeShader,
		elevate::Vector3(8.0f, 4.0f, 2.0f),
		1.0f
	);

	gm->crate->mesh->SetShader(&gm->cubeShader);
	static_cast<Cube*>(gm->crate->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Crate/TCom_MetalThreadplate4_Aged_512_albedo.png");
	//static_cast<Cube*>(crate->mesh)->SetTexture(crateTexture);
	gm->crate->mesh->setGameManager(gm);
	gm->crate->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.1f));

	gameObjects.push_back(gm->crate->mesh);
}