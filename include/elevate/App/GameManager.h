#pragma once

#include "OpenGL/Renderer.h"
#include "OpenGL/RenderData.h"
#include "Window/Window.h"

#include "App/InputManager.h"

#include "App/Camera.h"
#include "GameObjects/Cube.h"
#include "GameObjects/Sphere.h"
#include "Physics/Demos/AmmoRound.h"
#include "Physics/Demos/FloatingSphere.h"
#include "Physics/pfgen.h"

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

    glm::mat4 getView() const { return view; }
    glm::mat4 getProjection() const { return projection; }

    void fireRound();
	void setCurrentShotType(ShotType type) { currentShotType = type; }

    void update(float deltaTime);

    void render();

    void setUpDebugUI();
    void showDebugUI();
    void renderDebugUI();

private:
    void ShowCameraControlWindow(Camera& cam);
    void ShowLightControlWindow(DirLight& light);
    void ShowAmmoWindow();
	void ShowBuoyancyWindow();

    void RemoveDestroyedGameObjects();

    Renderer* renderer;
    Window* window;
    Camera* camera;

    InputManager* inputManager;

    std::vector<GameObject*> gameObjects;

    Shader ammoShader{};
	Shader cubeShader{};

    Cube* cube;
    Cube* cube2;
    Cube* cube3;
    Cube* cube4;
    Cube* waterCubeTop;
    Cube* waterCubeBottom;
    Sphere* pistolSphere;
    Sphere* artillerySphere;
    Sphere* fireballSphere;
    Sphere* laserSphere;
    Sphere* waterSphere;
    Sphere* springSphere;
    Sphere* springSphere2;
    Sphere* springSphere3;

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    const static unsigned ammoRounds = 16;
    AmmoRound ammo[ammoRounds];
    ShotType currentShotType = PISTOL;

	FloatingSphere* Sphere0;
	FloatingSphere* Sphere1;
	FloatingSphere* Sphere2;
	elevate::ParticleForceRegistry registry;
	elevate::ParticleAnchoredSpring* springFG;
	elevate::ParticleAnchoredBungee* bungeeFG;
    elevate::ParticleGravity* gravityFG;
	float floatingSphereMass = 2.0f;
	float waterHeight = 10.0f;
	float maxDepth = 5.0f;
	float floatingSphereVolume = 0.1f;
	float waterDensity = 1000.0f;

    bool showBuoyanceDemo = true;
};
