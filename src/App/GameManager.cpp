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
		elevate::Vector3 pos = { 0.0f, -30.0f, 0.0f };
		elevate::Vector3 scale = { 5.0f, 5.0f, 5.0f };
		sphere = new Sphere(pos, scale, &ammoShader, this, {0.0f, 0.8f, 0.3f});
		sphere->GenerateSphere(2.5f, 32, 32);
		sphere->LoadMesh();
		sphereBody = new RigidBody();
		sphereBody->setPosition(elevate::Vector3(0.0f, -30.0f, 0.0f));
		sphereBody->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		sphereBody->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		sphereBody->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));
		sphereBody->setMass(FLT_MAX);
		sphereBody->setAwake(true);
		Matrix3 tensor;
		real coeff = 0.4f * sphereBody->getMass() * 0.5f * 0.5f;
		tensor.setInertiaTensorCoeffs(coeff, coeff, coeff);
		sphereBody->setInertiaTensor(tensor);
		gameObjects.push_back(sphere);
		rbWorld = new World(100, 50);
		rbGravity = new Gravity(elevate::Vector3(0.0f, -9.81f, 0.0f));
		pos = { 0.0f, 30.0f, 0.0f };
		scale = { 1.0f, 1.0f, 1.0f };
		sphere2 = new Sphere(pos, scale, &cubeShader, this, {0.9f, 0.1f, 0.4f});
		sphere2->GenerateSphere(1.5f, 32, 32);
		sphere2->LoadMesh();
		sphereBody2 = new RigidBody();
		sphereBody2->setPosition(elevate::Vector3(0.0f, 30.0f, 0.0f));
		sphereBody2->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		sphereBody2->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		sphereBody2->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));
		sphereBody2->setMass(100.0f);
		tensor;
		coeff = 0.4f * sphereBody2->getMass() * 0.5f * 0.5f;
		tensor.setInertiaTensorCoeffs(coeff, coeff, coeff);
		sphereBody2->setInertiaTensor(tensor);

		sphereBody2->setAwake(true);
		gameObjects.push_back(sphere2);
		cSphere0 = new CollisionSphere();
		cSphere1 = new CollisionSphere();
		cSphere0->body = sphereBody;
		cSphere0->radius = 12.5f;
		cSphere1->body = sphereBody2;
		cSphere1->radius = 1.5f;
		cSpheres[0] = cSphere0;
		cSpheres[1] = cSphere1;
		cSpheres[0]->body->calculateDerivedData();
		cSpheres[1]->body->calculateDerivedData();
		cSpheres[0]->calculateInternals();
		cSpheres[1]->calculateInternals();
		rbWorld->getForceRegistry().add(cSpheres[1]->body, rbGravity);
		rbWorld->addBody(cSpheres[0]->body);
		rbWorld->addBody(cSpheres[1]->body);
		cSphere0->getTransform();
		cSphere1->getTransform();

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


	if (cubeDemo)
	{
		testBody->integrate(deltaTime);
		cube->SetPosition(testBody->getPosition());
	}

	if (sphereDemo)
	{
	
		//cSpheres[0]->body->integrate(deltaTime);
		//cSpheres[1]->body->integrate(deltaTime);
	//	sphere2->SetPosition(sphereBody2->getPosition());
		Matrix4 sphere1Mat;
		Matrix4 sphere0Mat;
		//cSphere1.body->getGLTransformMatrix(sphere1Mat);
		//cSphere0->body->calculateDerivedData();
		//cSphere1->body->calculateDerivedData();
		cSpheres[0]->calculateInternals();
		cSpheres[1]->calculateInternals();
	
		sphere1Mat = cSphere1->body->getTransform();
		elevate::Vector3 newPos = sphere1Mat.getAxisVector(3);
		sphere2->SetPosition(newPos);
	
		sphere0Mat = cSphere0->body->getTransform();
		newPos = sphere0Mat.getAxisVector(3);
		sphere->SetPosition(newPos);
	}


	//rbWorld->generateContacts();

}

void GameManager::generateContacts()
{
	cData.contactArray = contacts;
	cData.reset(256);
	cData.friction = (real)0.4;
	cData.restitution = (real)0.9;
	cData.tolerance = (real)0.9;
	cData.contacts = contacts;


	CollisionDetector::sphereAndSphere(*cSphere0, *cSphere1, &cData);
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