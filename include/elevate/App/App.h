#pragma once

#include "Window/Window.h"
#include "App/GameManager.h"

class App {
public:
	App(unsigned int screenWidth, unsigned int screenHeight);
	~App();

	void run();

	Window& getWindow() { return *mWindow; }

private:
	Window* mWindow = nullptr;
	GameManager* mGameManager = nullptr;

	unsigned int width;
	unsigned int height;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
};