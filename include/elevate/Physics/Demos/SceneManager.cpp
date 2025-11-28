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

    default:
        break;
    }

    s_currentScene = id;
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