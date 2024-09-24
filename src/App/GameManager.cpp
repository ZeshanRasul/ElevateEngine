#include "App/GameManager.h"
#include "Physics/core.h"

#include "imgui/imgui.h"
#include "imgui/backend/imgui_impl_glfw.h"
#include "imgui/backend/imgui_impl_opengl3.h"

DirLight dirLight = {
        glm::vec3(-0.2f, -1.0f, -0.3f),

        glm::vec3(0.15f, 0.15f, 0.15f),
        glm::vec3(0.4f),
        glm::vec3(0.1f, 0.1f, 0.1f)
};

GameManager::GameManager(Window* window, unsigned int width, unsigned int height)
    : window(window)
{
    inputManager = new InputManager();

    window->setInputManager(inputManager);

    renderer = window->getRenderer();
 
    ammoShader.loadShaders("C:/dev/ElevateEngine/src/Shaders/vertex.glsl", "C:/dev/ElevateEngine/src/Shaders/fragment.glsl");

    cubeShader.loadShaders("C:/dev/ElevateEngine/src/Shaders/vertex.glsl", "C:/dev/ElevateEngine/src/Shaders/fragment.glsl");

    camera = new Camera(glm::vec3(0.0f, 2.0f, 0.0f));

    inputManager->setContext(camera, this, width, height);

	elevate::Vector3 pos = { 0.0f, 0.0f, -10.0f };
	elevate::Vector3 pos2 = { 0.0f, 0.0f, -20.0f };
	elevate::Vector3 pos3 = { 0.0f, 0.0f, -30.0f };
	elevate::Vector3 pos4 = { 0.0f, 0.0f, -40.0f };
	elevate::Vector3 scale = { 1.0f, 1.0f, 1.0f };
    cube = new Cube(pos, scale, &cubeShader, this);
    cube->LoadMesh();
    cube2 = new Cube(pos2, scale, &cubeShader, this);
    cube2->LoadMesh();
    cube3 = new Cube(pos3, scale, &cubeShader, this);
    cube3->LoadMesh();
    cube4 = new Cube(pos4, scale, &cubeShader, this);
    cube4->LoadMesh();

	pos = { 0.0f, maxDepth, 0.0f };
	scale = { 100.0f, 0.1f, 100.0f };
	waterCubeTop = new Cube(pos, scale, &cubeShader, this);
	waterCubeTop->LoadMesh();

	pos = { 0.0f, (0.0f - maxDepth), 0.0f };
	waterCubeBottom = new Cube(pos, scale, &cubeShader, this);
	waterCubeBottom->LoadMesh();

	scale = { 0.2f, 0.2f, 0.2f };
	elevate::Vector3 spherePos = { 0.0f, 2.0f, -10.0f };
	pistolSphere = new Sphere(spherePos, scale, &ammoShader, this, glm::vec3(0.3f, 0.3f, 0.3f));
    pistolSphere->GenerateSphere(1.0f, 30, 30);
    pistolSphere->LoadMesh();

    scale = { 0.6f, 0.6f, 0.6f };
    artillerySphere = new Sphere(spherePos, scale, &ammoShader, this, glm::vec3(0.0f, 1.0f, 0.0f));
    artillerySphere->GenerateSphere(1.0f, 30, 30);
    artillerySphere->LoadMesh();

    scale = { 0.3f, 0.3f, 0.3f };
    fireballSphere = new Sphere(spherePos, scale, &ammoShader, this, glm::vec3(1.0f, 0.0f, 0.0f));
    fireballSphere->GenerateSphere(1.0f, 30, 30);
    fireballSphere->LoadMesh();

    scale = { 0.1f, 0.1f, 0.1f };
    laserSphere = new Sphere(spherePos, scale, &ammoShader, this, glm::vec3(1.0f, 0.3f, 1.0f));
    laserSphere->GenerateSphere(1.0f, 30, 30);
    laserSphere->LoadMesh();

    scale = { 0.6f, 0.6f, 0.6f };
    spherePos = { 0.0f, 2.0f, -10.0f };
    waterSphere = new Sphere(spherePos, scale, &ammoShader, this, glm::vec3(0.0f, 0.0f, 1.0f));
    waterSphere->GenerateSphere(1.0f, 30, 30);
    waterSphere->LoadMesh();

	// TODO: Create gameobjects and add to gameObjects vector
    gameObjects.push_back(cube);
    gameObjects.push_back(cube2);
    gameObjects.push_back(cube3);
    gameObjects.push_back(cube4);
	gameObjects.push_back(waterCubeTop);
	gameObjects.push_back(waterCubeBottom);

    for (AmmoRound* shot = ammo; shot < ammo + ammoRounds; shot++) {
		elevate::Particle* particle = new elevate::Particle();
        particle->setPosition(elevate::Vector3(0.0f, 2.0f, -10.0f));
        shot->SetParticle(particle);
        shot->SetSphere(pistolSphere);
        shot->SetType(UNUSED);
    }


	floatingSphere = new FloatingSphere();
	floatingSphere->SetParticle(new elevate::Particle());
	floatingSphere->GetParticle()->setPosition(elevate::Vector3(0.0f, 1.0f, -10.0f));
	floatingSphere->GetParticle()->setMass(2.0f);
    floatingSphere->GetParticle()->setDamping(0.98f);
    floatingSphere->SetSphere(waterSphere);
	buoyancyFG = new elevate::ParticleBuoyancy(maxDepth, 0.001f, waterHeight, 1000.0f);
	registry.add(floatingSphere->GetParticle(), buoyancyFG);
}

void GameManager::setupCamera(unsigned int width, unsigned int height)
{
    view = camera->GetViewMatrix();
    projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 500.0f);
}

void GameManager::setSceneData()
{
    renderer->setScene(view, projection, dirLight);
}

void GameManager::setUpDebugUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GameManager::showDebugUI()
{
    ShowLightControlWindow(dirLight);
    ShowCameraControlWindow(*camera);
	ShowAmmoWindow();
    ShowBuoyancyWindow();
}

void GameManager::renderDebugUI()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GameManager::ShowLightControlWindow(DirLight& light)
{
    ImGui::Begin("Directional Light Control");

    ImGui::Text("Light Direction");

    ImGui::DragFloat3("Direction", (float*)&light.direction, dirLight.direction.x, dirLight.direction.y, dirLight.direction.z);
    ImGui::ColorEdit4("Ambient", (float*)&light.ambient);
    ImGui::ColorEdit4("Diffuse", (float*)&light.diffuse);
    ImGui::ColorEdit4("Specular", (float*)&light.specular);

    ImGui::End();
}

void GameManager::ShowAmmoWindow()
{
    ImGui::Begin("Ammo Details");

    switch (currentShotType)
    {

    case PISTOL:
	{
		ImGui::Text("Pistol");
        break;
	}
    case ARTILLERY:
    {
        ImGui::Text("Artillery");
        break;
    }
	case FIREBALL:
	{
		ImGui::Text("Fireball");
        break;
    }
	case LASER:
	{
		ImGui::Text("Laser");
        break;
	}
	}

    ImGui::End();
}

void GameManager::ShowBuoyancyWindow()
{
    ImGui::Begin("Water Height");
    ImGui::DragFloat("Height", &waterHeight, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("Max Depth", &maxDepth, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("Sphere Volume", &floatingSphereVolume, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("Water Density", &waterDensity, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("Sphere Mass", &floatingSphereMass, 0.1f, 0.0f, 100.0f);
    ImGui::End();
}

void GameManager::RemoveDestroyedGameObjects()
{
    for (auto it = gameObjects.begin(); it != gameObjects.end(); ) {
        if ((*it)->isDestroyed) {
            delete* it;
            *it = nullptr;
            it = gameObjects.erase(it);
        }
        else {
            ++it;
        }
    }

}

void GameManager::ShowCameraControlWindow(Camera& cam)
{
    ImGui::Begin("Camera Control");

    std::string modeText = "";

    if (cam.Mode == FLY)
    {
        modeText = "Flycam";


        cam.UpdateCameraVectors();
    }
     
    ImGui::Text(modeText.c_str());

    ImGui::InputFloat3("Position", (float*)&cam.Position);

    ImGui::InputFloat("Pitch", (float*)&cam.Pitch);
    ImGui::InputFloat("Yaw", (float*)&cam.Yaw);
    ImGui::InputFloat("Zoom", (float*)&cam.Zoom);

    ImGui::End();
}

void GameManager::fireRound()
{
	AmmoRound* shot = nullptr;

	for (shot = ammo; shot < ammo + ammoRounds; shot++) {
		if (shot->GetType() == UNUSED) {
			break;
		}
	}
    
	if (shot >= ammo + ammoRounds) {
		return;
	}

    switch (currentShotType)
    {
    case PISTOL:
    {
        shot->SetSphere(pistolSphere);
        shot->GetParticle()->setMass(2.0f);
        shot->GetParticle()->setVelocity(0.0f, 0.0f, -35.0f);
        shot->GetParticle()->setAcceleration(0.0f, -1.0f, 0.0f);
        shot->GetParticle()->setDamping(0.99f);
        break;
    }

    case ARTILLERY:
    {
        shot->SetSphere(artillerySphere);
        shot->GetParticle()->setMass(200.0f); 
        shot->GetParticle()->setVelocity(0.0f, 30.0f, -40.0f); // 50m/s
        shot->GetParticle()->setAcceleration(0.0f, -20.0f, 0.0f);
        shot->GetParticle()->setDamping(0.99f);
        break;
    }
    case FIREBALL:
    {
        shot->SetSphere(fireballSphere);
        shot->GetParticle()->setMass(1.0f); 
        shot->GetParticle()->setVelocity(0.0f, 0.0f, -10.0f); // 5m/s
        shot->GetParticle()->setAcceleration(0.0f, 0.6f, 0.0f); // Floats up
        shot->GetParticle()->setDamping(0.9f);
        break;
    }
    case LASER:
    {
        shot->SetSphere(laserSphere);
        shot->GetParticle()->setMass(0.1f); // 0.1kg - almost no weight
        shot->GetParticle()->setVelocity(0.0f, 0.0f, -100.0f); // 100m/s
        shot->GetParticle()->setAcceleration(0.0f, 0.0f, 0.0f); // No gravity
        shot->GetParticle()->setDamping(0.99f);
        break;
    }
    }

	shot->GetParticle()->setPosition(elevate::Vector3(0.0f, 2.0f, -1.0f));
    shot->SetStartTime(glfwGetTime());
	shot->SetType(currentShotType);

	shot->GetParticle()->clearAccumulator();
}

void GameManager::update(float deltaTime)
{
    RemoveDestroyedGameObjects();
    inputManager->processInput(window->getWindow(), deltaTime);

	for (AmmoRound* shot = ammo; shot < ammo + ammoRounds; shot++) {
		if (shot->GetType() != UNUSED) {
			shot->GetParticle()->integrate(deltaTime);

            if (shot->GetParticle()->getPosition().y < 0.0f || glfwGetTime() - shot->GetStartTime() > 5.0f
                || shot->GetParticle()->getPosition().z > 200.0f) {
				shot->SetType(UNUSED);
            }
        }
	}
    buoyancyFG->setWaterHeight(waterHeight);
	waterCubeTop->SetPosition(elevate::Vector3(0.0f, maxDepth, 0.0f));
	waterCubeBottom->SetPosition(elevate::Vector3(0.0f, (0.0f - maxDepth), 0.0f));
	registry.updateForces(deltaTime);
	floatingSphere->GetParticle()->integrate(deltaTime);
}

void GameManager::render()
{
    for (auto obj : gameObjects) {
        renderer->draw(obj, view, projection);
    }

	ammoShader.use();
    ammoShader.setVec3("dirLight.direction", dirLight.direction);
    ammoShader.setVec3("dirLight.ambient", dirLight.ambient);
    ammoShader.setVec3("dirLight.diffuse", dirLight.diffuse);
    ammoShader.setVec3("dirLight.specular", dirLight.specular);
	
    for (AmmoRound* shot = ammo; shot < ammo + ammoRounds; shot++) {
		if (shot->GetType() != UNUSED) {
			shot->render(view, projection);
		}
	}

	floatingSphere->render(view, projection);
}