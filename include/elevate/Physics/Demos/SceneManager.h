#pragma once
#include "App/GameManager.h"

enum class SceneType
{
	None,
	Empty,
	Car,
	Aeroplane,
	Ragdoll,
	FractureWall,
	DominoChain,
	DemoShowcase,
	Count
};

namespace Scenes
{
	void LoadCarTest(GameManager* gm);
	void LoadAeroplaneTest(GameManager* gm);
	void LoadRagdollTest(GameManager* gm);
	void LoadFractureWall(GameManager* gm);
	void LoadDominoChain(GameManager* gm);
	void LoadDemoShowcase(GameManager* gm);
}

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	static void Init(GameManager* gm);
	
	static void LoadScene(SceneType type);
	static SceneType GetScene(SceneType type);

private:
	static GameManager* s_gameManager;
	static SceneType s_currentScene;
}