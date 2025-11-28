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


	s_gameManager->ResetState();

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


	gm->ResetState();
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


	gm->ResetState();
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


	gm->ResetState();

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

	gm->reset();

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

}