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
    // Set up car scene
}

void Scenes::LoadAeroplaneTest(GameManager* gm)
{
    // Set up aeroplane scene
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
}