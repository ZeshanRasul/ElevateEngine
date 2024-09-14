#pragma once

#include "OpenGL/Renderer.h"
#include "OpenGL/RenderData.h"
#include "Window/Window.h"

#include "App/InputManager.h"

#include "App/Camera.h"
#include "GameObjects/Cube.h"

class GameManager {
public:
    GameManager(Window* window, unsigned int width, unsigned int height);

    ~GameManager() {
        delete camera;
        for (auto it = gameObjects.begin(); it != gameObjects.end(); ) {
            if (*it) {
                delete* it;            
            }
            it = gameObjects.erase(it); 
        }
        delete inputManager;
    }

    void setupCamera(unsigned int width, unsigned int height);
    void setSceneData();

    void update(float deltaTime);

    void render();

    void setUpDebugUI();
    void showDebugUI();
    void renderDebugUI();

private:
    void ShowCameraControlWindow(Camera& cam);
    void ShowLightControlWindow(DirLight& light);

    void RemoveDestroyedGameObjects();

    Renderer* renderer;
    Window* window;
    Camera* camera;

    InputManager* inputManager;

    std::vector<GameObject*> gameObjects;

    Shader pistolShader{};
    Shader artilleryShader{};
    Shader fireballShader{};
	Shader laserShader{};

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
};
