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
	lineShader.loadShaders("C:/dev/ElevateEngine/src/Shaders/line_vert.glsl", "C:/dev/ElevateEngine/src/Shaders/line_frag.glsl");

	camera = new Camera(glm::vec3(0.0f, 40.0f, 60.0f));

	inputManager->setContext(camera, this, width, height);

	if (cubeDemo)
	{
		elevate::Vector3 pos = { 0.0f, 10.0f, 0.0f };
		elevate::Vector3 scale = { 3.0f, 3.0f, 3.0f };
		cube = new Cube(pos, scale, &cubeShader, this);
		cube->LoadMesh();
		testBody = new RigidBody();
		testBody->setPosition(elevate::Vector3(0.0f, 30.0f, 0.0f));
		testBody->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		testBody->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		testBody->setMass(2.0f);
		gameObjects.push_back(cube);
		rbWorld = new World(100, 50);
		rbGravity = new Gravity(elevate::Vector3(0.0f, -9.81f * 0.15f, 0.0f));
		rbWorld->getForceRegistry().add(testBody, rbGravity);
	}

	if (sphereDemo)
	{
		elevate::Vector3 pos = { 0.0f, -50.0f, 0.0f };
		elevate::Vector3 scale = { 20.0f, 20.0f, 20.0f };
		sphere = new Sphere(pos, scale, &ammoShader, this, {0.0f, 0.8f, 0.3f});
		sphere->GenerateSphere(15.0f, 32, 32);
		sphere->LoadMesh();
		sphereBody = new RigidBody();
		sphereBody->setPosition(elevate::Vector3(0.0f, -50.0f, 0.0f));
		sphereBody->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		sphereBody->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		sphereBody->setMass(5000.0f);
		gameObjects.push_back(sphere);
		rbWorld = new World(100, 50);
		rbGravity = new Gravity(elevate::Vector3(0.0f, -9.81f, 0.0f));
		pos = { 0.0f, 30.0f, 0.0f };
		scale = { 4.0f, 4.0f, 4.0f };
		sphere2 = new Sphere(pos, scale, &cubeShader, this, {0.9f, 0.1f, 0.4f});
		sphere2->GenerateSphere(4.0f, 32, 32);
		sphere2->LoadMesh();
		sphereBody2 = new RigidBody();
		sphereBody2->setPosition(elevate::Vector3(0.0f, 40.0f, 0.0f));
		sphereBody2->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		sphereBody2->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		sphereBody2->setMass(12.0f);
		gameObjects.push_back(sphere2);
		rbWorld->getForceRegistry().add(sphereBody2, rbGravity);
		cSphere0.body = sphereBody;
		cSphere0.radius = 10.0f;
		cSphere1.body = sphereBody2;
		cSphere0.radius = 0.5f;
		cSpheres[0] = cSphere0;
		cSpheres[1] = cSphere1;
	}
}

void GameManager::setupCamera(unsigned int width, unsigned int height)
{
	view = camera->GetViewMatrix();
	projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 5000.0f);
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

	if (!showBuoyanceDemo)
		ShowAmmoWindow();

	if (showBuoyanceDemo)
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
	//ImGui::Begin("Water Height");
	//ImGui::DragFloat("Height", &waterHeight, 0.1f, 0.0f, 100.0f);
	 //ImGui::DragFloat("Max Depth", &maxDepth, 0.1f, 0.0f, 100.0f);
	 //ImGui::DragFloat("Sphere Volume", &floatingSphereVolume, 0.1f, 0.0f, 100.0f);
	 //ImGui::DragFloat("Water Density", &waterDensity, 0.1f, 0.0f, 100.0f);
	 //ImGui::DragFloat("Sphere Mass", &floatingSphereMass, 0.1f, 0.0f, 100.0f);
	//ImGui::End();
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

	rbWorld->startFrame();
	rbWorld->runPhysics(deltaTime);
	rbRegistry.updateForces(deltaTime);
	generateContacts();
	resolver.resolveContacts(cData.contacts, cData.contactCount, deltaTime);
	cSphere0.calculateInternals();
	cSphere1.calculateInternals();

	if (cubeDemo)
	{
		testBody->integrate(deltaTime);
		cube->SetPosition(testBody->getPosition());
	}

	if (sphereDemo)
	{
	//	sphereBody2->integrate(deltaTime);
	//	sphere2->SetPosition(sphereBody2->getPosition());
		sphere2->SetPosition(elevate::Vector3(cSphere1.body->getPosition().x, cSphere1.body->getPosition().y, cSphere1.body->getPosition().z));
	}


	//if (pushDirX < -3.0f)
	//{
	//	pushDirX = -3.0f;
	//}
	//else if (pushDirX > 3.0f)
	//{
	//	pushDirX = 3.0f;
	//}

	//pushForce = new elevate::ParticleGravity(elevate::Vector3(pushDirX / Sphere2->GetParticle()->getMass(), 0.0f, 0.0f));
	//pWorld->getForceRegistry().add(Sphere2->GetParticle(), pushForce);


	//pWorld->startFrame();

	//for (AmmoRound* shot = ammo; shot < ammo + ammoRounds; shot++) {
	//	if (shot->GetType() != UNUSED) {
	//		shot->GetParticle()->integrate(deltaTime);

	//		if (shot->GetParticle()->getPosition().y < 0.0f || glfwGetTime() - shot->GetStartTime() > 5.0f
	//			|| shot->GetParticle()->getPosition().z > 200.0f) {
	//			shot->SetType(UNUSED);
	//		}
	//	}
	//}
	//// buoyancyFG->setWaterHeight(waterHeight);
	//waterCubeTop->SetPosition(elevate::Vector3(0.0f, 6.0f, -10.0f));
	//waterCubeBottom->SetPosition(elevate::Vector3(0.0f, (0.0f - 10.0f), 0.0f));

	//pWorld->runPhysics(deltaTime);

	////registry.updateForces(deltaTime);
	//////floatingSphere->GetParticle()->integrate(deltaTime);
	////Sphere0->GetParticle()->integrate(deltaTime);
	////Sphere1->GetParticle()->integrate(deltaTime);
	////Sphere2->GetParticle()->integrate(deltaTime);
	////elevate::Particle particle0 = *Sphere0->GetParticle();
	////float abX = particle0.getPosition().x;
	////float abY = particle0.getPosition().x;
	////float abZ = particle0.getPosition().x;
	//glm::vec3 abStart = glm::vec3(Sphere0->GetParticle()->getPosition().x, Sphere0->GetParticle()->getPosition().y, Sphere0->GetParticle()->getPosition().z);
	//glm::vec3 bcStart = glm::vec3(Sphere1->GetParticle()->getPosition().x, Sphere1->GetParticle()->getPosition().y, Sphere1->GetParticle()->getPosition().z);
	//glm::vec3 cdStart = glm::vec3(Sphere2->GetParticle()->getPosition().x, Sphere2->GetParticle()->getPosition().y, Sphere2->GetParticle()->getPosition().z);
	//lineab->UpdateVertexBuffer(abStart, bcStart);
	//linebc->UpdateVertexBuffer(bcStart, cdStart);
	//linecd->UpdateVertexBuffer(anchorPos, abStart);
}

void GameManager::generateContacts()
{
	cData.reset(256);
	cData.friction = (real)0.4;
	cData.restitution = (real)0.9;
	cData.tolerance = (real)0.1;

	CollisionDetector::sphereAndSphere(cSpheres[0], cSpheres[1], &cData);
};


void GameManager::render()
{
	for (auto obj : gameObjects) {
		renderer->draw(obj, view, projection);
	}
}
//	ammoShader.use();
//	ammoShader.setVec3("dirLight.direction", dirLight.direction);
//	ammoShader.setVec3("dirLight.ambient", dirLight.ambient);
//	ammoShader.setVec3("dirLight.diffuse", dirLight.diffuse);
//	ammoShader.setVec3("dirLight.specular", dirLight.specular);
//
//	for (AmmoRound* shot = ammo; shot < ammo + ammoRounds; shot++) {
//		if (shot->GetType() != UNUSED) {
//			shot->render(view, projection);
//		}
//	}
//
//	if (showBuoyanceDemo)
//	{
//		Sphere0->render(view, projection);
//		Sphere1->render(view, projection);
//		Sphere2->render(view, projection);
//		lineab->DrawLine(view, projection, glm::vec3(1.0f, 0.0f, 0.0f));
//		linebc->DrawLine(view, projection, glm::vec3(0.0f, 1.0f, 0.0f));
//		linecd->DrawLine(view, projection, glm::vec3(0.0f, 0.0f, 1.0f));
//	}
//}