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

	camera = new Camera(glm::vec3(18.0f, 5.0f, 18.0f), glm::vec3(0.0f, 1.0f, 0.0f), -138.0f);

	inputManager->setContext(camera, this, width, height);

	// Debug line GL setup
	glGenVertexArrays(1, &m_DebugLineVAO);
	glGenBuffers(1, &m_DebugLineVBO);

	glBindVertexArray(m_DebugLineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_DebugLineVBO);

	// Each vertex: vec3 position + vec3 color = 6 floats
	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);

	glEnableVertexAttribArray(1); // color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (fpsSandboxDemo)
	{
		// If you want this instead of the sphere/cube demo:
		cubeDemo = false;
		sphereDemo = false;

		rbWorld = new World(200, 100);
		rbGravity = new Gravity(elevate::Vector3(0.0f, -9.81f, 0.0f));

		numEnvBoxes = 0;

		// Convenience: a lambda to create a static box in the environment
		auto addEnvBox = [&](const elevate::Vector3& pos,
			const elevate::Vector3& scale)
			{
				if (numEnvBoxes >= MaxEnvBoxes) return;

				Cube* cube = new Cube(pos, scale, &cubeShader, this);
				cube->LoadMesh();
				cube->SetAngle(0.0f);
				cube->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
				gameObjects.push_back(cube);

				elevate::RigidBody* body = new elevate::RigidBody();
				body->setAwake(false);
				body->setPosition(pos);
				body->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
				body->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
				body->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));

				body->setMass(0.0f);
				rbWorld->addBody(body);

				elevate::CollisionBox* cbox = new elevate::CollisionBox();
				cbox->body = body;

				cbox->halfSize = elevate::Vector3(
					scale.x * 0.5f,
					scale.y * 0.5f,
					scale.z * 0.5f
				);
				cbox->isOverlapping = false;

				body->calculateDerivedData();
				cbox->calculateInternals();

				envBodies[numEnvBoxes] = body;
				envBoxes[numEnvBoxes] = cbox;
				envCubes[numEnvBoxes] = cube;

				++numEnvBoxes;
			};

		addEnvBox(
			elevate::Vector3(0.0f, -1.0f, 0.0f),  
			elevate::Vector3(50.0f, 1.0f, 50.0f)  
		);


		// +X wall
		addEnvBox(
			elevate::Vector3(25.0f, 3.0f, 0.0f), 
			elevate::Vector3(1.0f, 8.0f, 50.0f)  
		);

		// -X wall
		addEnvBox(
			elevate::Vector3(-25.0f, 3.0f, 0.0f),
			elevate::Vector3(1.0f, 8.0f, 50.0f)
		);

		// +Z wall
		addEnvBox(
			elevate::Vector3(0.0f, 3.0f, 25.0f),
			elevate::Vector3(50.0f, 8.0f, 1.0f)
		);

		// -Z wall
		addEnvBox(
			elevate::Vector3(0.0f, 3.0f, -25.0f),
			elevate::Vector3(50.0f, 8.0f, 1.0f)
		);

		// Optional: a central pillar to shoot at
		addEnvBox(
			elevate::Vector3(0.0f, 3.0f, 0.0f),
			elevate::Vector3(2.0f, 6.0f, 2.0f)
		);
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

}

void GameManager::ShowBuoyancyWindow()
{

}

void GameManager::RemoveDestroyedGameObjects()
{
	//for (auto it = gameObjects.begin(); it != gameObjects.end(); ) {
	//	if ((*it)->isDestroyed) {
	//		delete* it;
	//		*it = nullptr;
	//		it = gameObjects.erase(it);
	//	}
	//	else {
	//		++it;
	//	}
	//}

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

void GameManager::update(float deltaTime)
{

	RemoveDestroyedGameObjects();
	inputManager->processInput(window->getWindow(), deltaTime);

	rbWorld->startFrame();
	rbWorld->runPhysics(deltaTime);
	generateContacts();
	resolver.resolveContacts(cData.contacts, cData.contactCount, deltaTime);
	rbWorld->startFrame();

	


}

void GameManager::generateContacts()
{
	cData.reset(256);
	cData.friction = (real)0.1;
	cData.restitution = (real)0.9;
	cData.tolerance = (real)0.01;
	cData.contactArray = contacts;
	cData.contacts = contacts;

	buildContactDebugLines();

}

void GameManager::render()
{
	for (auto obj : gameObjects) {
		renderer->draw(obj, view, projection);
	}

	drawDebugLines();

}
