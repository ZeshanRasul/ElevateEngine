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
	: window(window),
	resolver(256 * 4)
{

	inputManager = new InputManager();

	window->setInputManager(inputManager);

	renderer = window->getRenderer();

	ammoShader.loadShaders("C:/dev/ElevateEngine/src/Shaders/vertex.glsl", "C:/dev/ElevateEngine/src/Shaders/fragment.glsl");

	cubeShader.loadShaders("C:/dev/ElevateEngine/src/Shaders/vertex.glsl", "C:/dev/ElevateEngine/src/Shaders/fragment_tex.glsl");
	lineShader.loadShaders("C:/dev/ElevateEngine/src/Shaders/line_vert.glsl", "C:/dev/ElevateEngine/src/Shaders/line_frag.glsl");

	camera = new Camera(glm::vec3(18.0f, 5.0f, 18.0f), glm::vec3(0.0f, 1.0f, 0.0f), -138.0f);

	inputManager->setContext(camera, this, width, height);

	contacts = new elevate::Contact[1028];
	cData.contactArray = contacts;
	resolver.setIterations(256 * 4, 256 * 4);
	glGenVertexArrays(1, &m_DebugLineVAO);
	glGenBuffers(1, &m_DebugLineVBO);

	glBindVertexArray(m_DebugLineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_DebugLineVBO);


	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);

	glEnableVertexAttribArray(1); // color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	spawnContext.World = new World(300, 100);
	rbGravity = new Gravity(elevate::Vector3(0.0f, -9.81f * 0.25f, 0.0f));
	shapeFactory = new elevate::ShapeFactory();
	spawnFactory = new elevate::SpawnFactory(spawnContext);

	gameObjects.clear();
	gameObjects.reserve(300);
	//	gameObjects.resize(300);

	crate = spawnFactory->SpawnCrate(
		elevate::Vector3(40.0f, 0.0f, 20.0f),
		&ammoShader,
		elevate::Vector3(8.0f, 4.0f, 2.0f),
		1.0f
	);

	crate->mesh->SetShader(&ammoShader);
	crate->mesh->setGameManager(this);
	crate->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.1f));

	gameObjects.push_back(crate->mesh);

	ball = spawnFactory->SpawnGrenade(
		elevate::Vector3(10.0f, 2.0, 0.0f),
		elevate::Vector3(0.0f, -9.81f, 0.0f),
		&ammoShader,
		3.0f,
		5.0f
	);

	ball->mesh->SetShader(&ammoShader);
	ball->mesh->setGameManager(this);
	ball->mesh->SetColor(glm::vec3(0.1f, 0.1f, 0.8f));

	gameObjects.push_back(ball->mesh);

	spawnFactory->BuildCrateStack(
		elevate::Vector3(-10.0f, 10.0f, -40.0f),
		3,
		3,
		3,
		elevate::Vector3(3.1f, 3.1f, 3.1f),
		1.0f,
		&ammoShader,
		crates
	);

	for (int i = 0; i < crates.size(); ++i)
	{
		crates[i]->mesh->SetShader(&ammoShader);
		crates[i]->mesh->setGameManager(this);
		crates[i]->mesh->SetColor(glm::vec3(0.3f, 0.7f, 0.2f));
		gameObjects.push_back(crates[i]->mesh);
	}

	spawnFactory->BuildBrickWall(
		elevate::Vector3(0.0f, 0.0f, -10.0f),
		3,
		3,
		elevate::Vector3(1.0f, 0.5f, 0.5f),
		2.0f,
		true,
		bricks,
		&ammoShader
	);

	for (size_t i = 0; i < bricks.size(); ++i)
	{
		bricks[i]->mesh->SetShader(&ammoShader);
		bricks[i]->mesh->setGameManager(this);
		bricks[i]->mesh->SetColor(glm::vec3(0.6f, 0.4f, 0.2f));
		gameObjects.push_back(bricks[i]->mesh);
	}

	floor = spawnFactory->CreateFloor(
		elevate::Vector3(200.0f, 1.0f, 200.0f),
		&cubeShader,
		elevate::Vector3(0.0f, -1.0f, 0.0f)
	);
	floor->mesh->setGameManager(this);
	floor->mesh->SetColor(glm::vec3(0.3f, 0.8f, 0.3f));
	static_cast<Cube*>(floor->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Ground/TCom_Scifi_Floor2_4k_albedo.png");
	gameObjects.push_back(floor->mesh);

	wall = spawnFactory->CreateWall(
		elevate::Vector3(1.0f, 60.0f, 200.0f),
		elevate::Vector3(200.0f, 60.0f, 0.0f),
		&cubeShader
	);
	wall->mesh->setGameManager(this);
	wall->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	wall->mesh->SetTexTiling(4.0f);
	static_cast<Cube*>(wall->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_4k_albedo.png");
	gameObjects.push_back(wall->mesh);

	wall2 = spawnFactory->CreateWall(
		elevate::Vector3(1.0f, 60.0f, 200.0f),
		elevate::Vector3(-200.0f, 60.0f, 0.0f),
		&cubeShader
	);
	wall2->mesh->setGameManager(this);
	wall2->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	wall2->mesh->SetTexTiling(4.0f);
	static_cast<Cube*>(wall2->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_4k_albedo.png");
	gameObjects.push_back(wall2->mesh);

	wall3 = spawnFactory->CreateWall(
		elevate::Vector3(200.0f, 60.0f, 1.0f),
		elevate::Vector3(0.0f, 60.0f, 200.0f),
		&cubeShader
	);
	wall3->mesh->setGameManager(this);
	wall3->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	wall3->mesh->SetTexTiling(4.0f);
	static_cast<Cube*>(wall3->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_4k_albedo.png");
	gameObjects.push_back(wall3->mesh);

	wall4 = spawnFactory->CreateWall(
		elevate::Vector3(200.0f, 60.0f, 1.0f),
		elevate::Vector3(0.0f, 60.0f, -200.0f),
		&cubeShader
	);
	wall4->mesh->setGameManager(this);
	wall4->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	wall4->mesh->SetTexTiling(4.0f);
	static_cast<Cube*>(wall4->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_4k_albedo.png");
	gameObjects.push_back(wall4->mesh);

	if (fpsSandboxDemo)
	{
		cubeDemo = false;
		sphereDemo = false;


		reset();

		// Right Knee
		joints[0].set(
			bones[0].body, elevate::Vector3(0, 1.07f, 0),
			bones[1].body, elevate::Vector3(0, -1.07f, 0),
			0.15f
		);

		// Left Knee
		joints[1].set(
			bones[2].body, elevate::Vector3(0, 1.07f, 0),
			bones[3].body, elevate::Vector3(0, -1.07f, 0),
			0.15f
		);

		// Right elbow
		joints[2].set(
			bones[9].body, elevate::Vector3(0, 0.96f, 0),
			bones[8].body, elevate::Vector3(0, -0.96f, 0),
			0.15f
		);

		// Left elbow
		joints[3].set(
			bones[11].body, elevate::Vector3(0, 0.96f, 0),
			bones[10].body, elevate::Vector3(0, -0.96f, 0),
			0.15f
		);

		// Stomach to Waist
		joints[4].set(
			bones[4].body, elevate::Vector3(0.054f, 0.50f, 0),
			bones[5].body, elevate::Vector3(-0.043f, -0.45f, 0),
			0.15f
		);

		joints[5].set(
			bones[5].body, elevate::Vector3(-0.043f, 0.411f, 0),
			bones[6].body, elevate::Vector3(0, -0.411f, 0),
			0.15f
		);

		joints[6].set(
			bones[6].body, elevate::Vector3(0, 0.521f, 0),
			bones[7].body, elevate::Vector3(0, -0.752f, 0),
			0.15f
		);

		// Right hip
		joints[7].set(
			bones[1].body, elevate::Vector3(0, 1.066f, 0),
			bones[4].body, elevate::Vector3(0, -0.458f, -0.5f),
			0.15f
		);

		// Left Hip
		joints[8].set(
			bones[3].body, elevate::Vector3(0, 1.066f, 0),
			bones[4].body, elevate::Vector3(0, -0.458f, 0.5f),
			0.105f
		);

		// Right shoulder
		joints[9].set(
			bones[6].body, elevate::Vector3(0, 0.367f, -0.8f),
			bones[8].body, elevate::Vector3(0, 0.888f, 0.32f),
			0.15f
		);

		// Left shoulder
		joints[10].set(
			bones[6].body, elevate::Vector3(0, 0.367f, 0.8f),
			bones[10].body, elevate::Vector3(0, 0.888f, -0.32f),
			0.15f
		);

		ammoCount = MaxAmmoRounds;

		for (int i = 0; i < ammoCount; ++i)
		{
			elevate::Vector3 startPos(0.0f, -100.0f, 0.0f); // far below world

			elevate::Vector3 scale(1.0f, 1.0f, 1.0f);
			Sphere* s = new Sphere(startPos, scale, &ammoShader, this,
				glm::vec3(0.9f, 0.1f, 0.1f));
			s->GenerateSphere(0.5f, 16, 16);
			s->LoadMesh();
			gameObjects.push_back(s);

			elevate::RigidBody* body = new elevate::RigidBody();
			body->setAwake(false);
			body->setPosition(startPos);
			body->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
			body->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
			body->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));

			real mass = 1.7f;
			body->setMass(mass);
			body->setDamping(0.97f, 0.97f);

			real r = 0.5f;
			real coeff = (real)0.4f * mass * r * r;
			elevate::Matrix3 inertia;
			inertia.setInertiaTensorCoeffs(coeff, coeff, coeff);
			body->setInertiaTensor(inertia);

			//spawnContext.World->addBody(body);
			//spawnContext.World->getForceRegistry().add(body, rbGravity);

			elevate::CollisionSphere* cs = new elevate::CollisionSphere();
			cs->body = body;
			cs->radius = r;
			cs->body->calculateDerivedData();
			cs->calculateInternals();

			ammoPool[i].visual = s;
			ammoPool[i].body = body;
			ammoPool[i].coll = cs;
			ammoPool[i].active = false;
			ammoPool[i].lifetime = 0.0f;
		}

		for (unsigned i = 0; i < 12; i++)
		{
			Cube* c = new Cube(bones[i].getTransform().getAxisVector(3), bones[i].halfSize * 2, &ammoShader, this);
			c->LoadMesh();
			//		elevate::Vector3 color = random.randomVector(1.0f);
			c->SetColor(glm::vec3(87.0f, 0.8f, 0.9f));
			bones[i].visual = c;
			gameObjects.push_back(c);
			//spawnContext.World->addBody(bones[i].body);
			//	spawnContext.World->getForceRegistry().add(bones[i].body, );
		}

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

void GameManager::reset()
{
	bones[0].setState(
		elevate::Vector3(0, 0.993, -0.5),
		elevate::Vector3(0.301, 1.0, 0.234));
	bones[1].setState(
		elevate::Vector3(0, 3.159, -0.56),
		elevate::Vector3(0.301, 1.0, 0.234));
	bones[2].setState(
		elevate::Vector3(0, 0.993, 0.5),
		elevate::Vector3(0.301, 1.0, 0.234));
	bones[3].setState(
		elevate::Vector3(0, 3.15, 0.56),
		elevate::Vector3(0.301, 1.0, 0.234));
	bones[4].setState(
		elevate::Vector3(-0.054, 4.683, 0.013),
		elevate::Vector3(0.415, 0.392, 0.690));
	bones[4].body->setMass(FLT_MAX);
	bones[5].setState(
		elevate::Vector3(0.043, 5.603, 0.013),
		elevate::Vector3(0.301, 0.367, 0.693));
	bones[6].setState(
		elevate::Vector3(0, 6.485, 0.013),
		elevate::Vector3(0.435, 0.367, 0.786));
	bones[7].setState(
		elevate::Vector3(0, 7.759, 0.013),
		elevate::Vector3(0.45, 0.598, 0.421));
	bones[7].body->setMass(FLT_MAX);
	bones[8].setState(
		elevate::Vector3(0, 5.946, -1.066),
		elevate::Vector3(0.267, 0.888, 0.207));
	bones[9].setState(
		elevate::Vector3(0, 4.024, -1.066),
		elevate::Vector3(0.267, 0.888, 0.207));
	bones[10].setState(
		elevate::Vector3(0, 5.946, 1.066),
		elevate::Vector3(0.267, 0.888, 0.207));
	bones[11].setState(
		elevate::Vector3(0, 4.024, 1.066),
		elevate::Vector3(0.267, 0.888, 0.207));

	//for (Bone bone : bones)
	//{
	//	spawnContext.World->addBody(bone.body);
	//}

	// Only the first block exists
	for (Block* block = blocks; block < blocks + 9; block++)
	{
		if (!block->exists)
			continue;
		block->exists = false;

		gameObjects.erase(
			std::remove(
				gameObjects.begin(),
				gameObjects.end(),
				block->visual),
			gameObjects.end()
		);
	}

	firstHit = true;

	Cube* cube = new Cube(elevate::Vector3(-10, 7, 10), elevate::Vector3(5, 5, 5), &cubeShader, this);
	cube->LoadMesh();
	cube->SetAngle(0.0f);
	cube->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
	cube->SetColor(glm::vec3(0.2f, 0.1f, 0.5f));
	blocks[0].visual = cube;
	gameObjects.push_back(cube);

	// Set the first block
	// Set the first block
	blocks[0].halfSize = elevate::Vector3(4, 4, 4);
	blocks[0].body->setPosition(elevate::Vector3(-50, 7, 50));
	blocks[0].body->setOrientation(elevate::Quaternion(1, 0, 0, 0));
	blocks[0].body->setVelocity(elevate::Vector3(0, 0, 0));
	blocks[0].body->setRotation(elevate::Vector3(0, 0, 0));
	blocks[0].body->setMass(100.0f);
	elevate::Matrix3 it;
	it.setBlockInertiaTensor(blocks[0].halfSize, 100.0f);
	blocks[0].body->setInertiaTensor(it);
	blocks[0].body->setDamping(0.9f, 0.9f);
	blocks[0].body->calculateDerivedData();
	blocks[0].calculateInternals();

	blocks[0].body->setAcceleration(elevate::Vector3::GRAVITY);
	blocks[0].body->setAwake(true);
	blocks[0].body->setCanSleep(true);
	blocks[0].exists = true;
	//	spawnContext.World->addBody(blocks[0].body);

		//	hit = false;

			//elevate::real strength = -random.randomReal(500.0f, 1000.0f);
			//for (unsigned i = 0; i < 12; i++)
			//{
			//	bones[i].body->addForceAtBodyPoint(
			//		elevate::Vector3(strength, 0, 0), elevate::Vector3()
			//	);
			//}
			//bones[6].body->addForceAtBodyPoint(
			//	elevate::Vector3(strength, 0, random.randomBinomial(1000.0f)),
			//	elevate::Vector3(random.randomBinomial(4.0f), random.randomBinomial(3.0f), 0)
			//);

			// Reset the contacts

	numStackCubes = 5; // choose 3?5 for testing

	elevate::Vector3 cubeHalfSize(1.0f, 1.0f, 1.0f);
	glm::vec3       renderScale(2.0f, 2.0f, 2.0f);

	for (int i = 0; i < numStackCubes; i++)
	{
		gameObjects.erase(
			std::remove(
				gameObjects.begin(),
				gameObjects.end(),
				stackCubes[i]),
			gameObjects.end()
		);

		//	rbWorld->removeBody(stackBodies[i]);

		delete cStackBoxes[i];

		float centerY = 2.0f + i * 2.0f;

		elevate::Vector3 pos(-20.0f, centerY, -20.0f);
		elevate::Vector3 scale(renderScale.x, renderScale.y, renderScale.z);

		// Render cube
		Cube* cube = new Cube(pos, scale, &cubeShader, this);
		cube->LoadMesh();
		cube->SetAngle(0.0f);
		cube->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
		cube->SetColor(glm::vec3(0.2f, 0.7f, 0.3f));
		gameObjects.push_back(cube);
		stackCubes[i] = cube;

		// Physics body
		elevate::RigidBody* body = new elevate::RigidBody();
		body->setAwake(true);
		body->setPosition(pos);
		body->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		body->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		body->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));

		real mass = 0.2f;
		body->setMass(mass);

		elevate::Matrix3 boxInertia;
		boxInertia.setBlockInertiaTensor(cubeHalfSize, mass);
		body->setInertiaTensor(boxInertia);

		//rbWorld->addBody(body);
		//rbGravity->updateForce(body, 0); // or via registry below

//		rbWorld->getForceRegistry().add(body, rbGravity);

		stackBodies[i] = body;

		// Collision box
		elevate::CollisionBox* cBox = new elevate::CollisionBox();
		cBox->body = body;
		cBox->halfSize = cubeHalfSize;
		cBox->body->calculateDerivedData();
		cBox->calculateInternals();
		cStackBoxes[i] = cBox;
	}

	hit = false;

	cData.contactCount = 0;
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

void GameManager::fireRound(AmmoType type)
{
	AmmoRound* round = nullptr;
	for (int i = 0; i < ammoCount; ++i)
	{
		if (!ammoPool[i].active)
		{
			round = &ammoPool[i];
			break;
		}
	}

	if (!round)
	{
		return;
	}

	glm::vec3 camPos = camera->Position;
	glm::vec3 camFront = glm::normalize(camera->Front);

	glm::vec3 spawnPos = camPos + camFront * 2.0f;

	elevate::RigidBody* body = round->body;

	body->setAwake(true);
	body->clearAccumulator();
	body->setPosition(elevate::Vector3(spawnPos.x, spawnPos.y, spawnPos.z));
	body->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
	body->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));
	body->setMass(3.0f);
	spawnContext.World->getForceRegistry().add(body, rbGravity);

	real speed = 50.0f;
	switch (type)
	{
	case AmmoType::Pistol: speed = 70.0f; break;
	case AmmoType::Rifle:  speed = 88.0f; break;
	case AmmoType::Rocket: speed = 22.0f; break;
	}
	elevate::Vector3 v(camFront.x * speed,
		camFront.y * speed,
		camFront.z * speed);
	body->setVelocity(v);

	body->calculateDerivedData();
	round->coll->calculateInternals();

	round->active = true;
	round->lifetime = 5.0f;

	round->visual->SetPosition(elevate::Vector3(spawnPos.x, spawnPos.y, spawnPos.z));
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
	//deltaTime = glm::clamp(deltaTime, 0.0f, 1/ 60.0f);
//	RemoveDestroyedGameObjects();
	inputManager->processInput(window->getWindow(), deltaTime);

	//rbWorld->startFrame();
	//rbWorld->runPhysics(1.0f / 60.0f);

	if (fpsSandboxDemo)
	{
		//rbWorld->startFrame();
		//
		//rbWorld->runPhysics(1.0f / 60.0f);

		for (int i = 0; i < ammoCount; ++i)
		{
			AmmoRound& r = ammoPool[i];
			if (!r.active) continue;
			r.body->clearAccumulator();
			r.body->calculateDerivedData();
			rbGravity->updateForce(r.body, deltaTime);
			r.body->integrate(deltaTime);
			r.coll->calculateInternals();
		}
		for (Bone* bone = bones; bone < bones + 12; bone++)
		{
			bone->body->clearAccumulator();
			bone->body->calculateDerivedData();
			if (bone != &bones[4] && bone != &bones[7])
				bone->body->integrate(deltaTime);
			bone->calculateInternals();
		}

		for (int i = 0; i < numStackCubes; i++)
		{
			stackBodies[i]->clearAccumulator();
			stackBodies[i]->calculateDerivedData();
			rbGravity->updateForce(stackBodies[i], deltaTime);
			stackBodies[i]->integrate(deltaTime);
			cStackBoxes[i]->calculateInternals();
		}

		for (Block* block = blocks; block < blocks + 9; block++)
		{
			if (block->exists)
			{
				block->body->clearAccumulator();
				block->body->calculateDerivedData();
				rbGravity->updateForce(block->body, deltaTime);

				block->body->integrate(deltaTime);
				block->calculateInternals();
			}
		}

		for (int i = 0; i < crates.size(); ++i)
		{
			crates[i]->body->clearAccumulator();
			crates[i]->body->calculateDerivedData();
			rbGravity->updateForce(crates[i]->body, deltaTime);
			crates[i]->body->integrate(deltaTime);
			crates[i]->shape->calculateInternals();
		}

		generateContacts();

		resolver.resolveContacts(cData.contactArray, cData.contactCount, deltaTime);

		for (int i = 0; i < ammoCount; ++i)
		{
			AmmoRound& r = ammoPool[i];
			if (!r.active) continue;
			r.coll->calculateInternals();
			elevate::Vector3 p = r.body->getTransform().getAxisVector(3);
			r.visual->SetPosition(p);
			r.visual->SetOrientation(glm::quat(
				(float)r.body->getOrientation().r,
				(float)r.body->getOrientation().i,
				(float)r.body->getOrientation().j,
				(float)r.body->getOrientation().k));

			r.lifetime -= 1.0f / 60.0f;

			elevate::Vector3 pos = r.body->getPosition();
			if (r.lifetime <= 0.0f || pos.y < -100.0f)
			{
				r.active = false;
				r.body->setAwake(false);
				r.body->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
				r.body->setPosition(elevate::Vector3(0.0f, -100.0f, 0.0f));
				r.coll->calculateInternals();
				r.visual->SetPosition(r.body->getPosition());
				continue;
			}

		}

		for (int i = 0; i < crates.size(); i++)
		{
			crates[i]->shape->calculateInternals();
			elevate::Matrix4 t = crates[i]->body->getTransform();
			elevate::Vector3 p = t.getAxisVector(3);
			crates[i]->mesh->SetPosition(p);
			crates[i]->mesh->SetOrientation(glm::quat(
				(float)crates[i]->body->getOrientation().r,
				(float)crates[i]->body->getOrientation().i,
				(float)crates[i]->body->getOrientation().j,
				(float)crates[i]->body->getOrientation().k));
		}

		for (Bone* bone = bones; bone < bones + 12; bone++)
		{
			//bone->body->calculateDerivedData();
			bone->calculateInternals();
			elevate::Matrix4 t = bone->body->getTransform();

			elevate::Vector3 p = t.getAxisVector(3);
			bone->visual->SetPosition(p);
			bone->visual->SetOrientation(glm::quat(
				(float)bone->body->getOrientation().r,
				(float)bone->body->getOrientation().i,
				(float)bone->body->getOrientation().j,
				(float)bone->body->getOrientation().k));
		}

		for (int i = 0; i < numStackCubes; ++i)
		{
			stackBodies[i]->calculateDerivedData();
			cStackBoxes[i]->calculateInternals();

			elevate::Matrix4 t = stackBodies[i]->getTransform();
			elevate::Vector3 p = t.getAxisVector(3);

			stackCubes[i]->SetPosition(p);
			stackCubes[i]->SetOrientation(glm::quat(
				(float)stackBodies[i]->getOrientation().r,
				(float)stackBodies[i]->getOrientation().i,
				(float)stackBodies[i]->getOrientation().j,
				(float)stackBodies[i]->getOrientation().k));
		}



		for (Block* block = blocks; block < blocks + 9; block++)
		{
			if (block->exists)
			{
				block->body->calculateDerivedData();
				block->calculateInternals();

				elevate::Matrix4 t = block->getTransform();
				elevate::Vector3 p = t.getAxisVector(3);

				block->visual->SetPosition(p);
				block->visual->SetOrientation(glm::quat(
					(float)block->body->getOrientation().r,
					(float)block->body->getOrientation().i,
					(float)block->body->getOrientation().j,
					(float)block->body->getOrientation().k));
			}


			/*for (Block* block = blocks; block < blocks + 9; block++)
			{
				if (block->exists)
				{
					elevate::Matrix4 t = block->getTransform();
					elevate::Vector3 p = t.getAxisVector(3);

					block->visual->SetPosition(p);
					block->visual->SetOrientation(glm::quat(
						(float)block->body->getOrientation().r,
						(float)block->body->getOrientation().i,
						(float)block->body->getOrientation().j,
						(float)block->body->getOrientation().k));
				}

			}*/
		}

		if (hit)
		{
			for (int i = 0; i < 8; i++)
			{
				Cube* c = new Cube(elevate::Vector3(3.0f, 5.0f, 3.0f), elevate::Vector3(1.0f, 1.0f, 1.0f), &cubeShader, this);
				c->LoadMesh();
				c->SetAngle(0.0f);
				c->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
				c->SetColor(glm::vec3(0.2f, 0.1f, 0.3f));
				cubes[i] = c;
				gameObjects.push_back(c);
			}

			blocks[0].divideBlock(
				cData.contactArray[fracture_contact],
				blocks,
				blocks + 1,
				cubes
			);

			blocks[0].exists = false;

			gameObjects.erase(
				std::remove(
					gameObjects.begin(),
					gameObjects.end(),
					blocks[0].visual),
				gameObjects.end()
			);

			ball_active = false;

			hit = false;
		}
		return;
	}
}

void GameManager::generateContacts()
{
	hit = false;

	elevate::CollisionPlane plane;
	plane.direction = elevate::Vector3(0, 1, 0);
	plane.offset = 0;

	cData.reset(256);
	cData.friction = (real)0.9;
	cData.restitution = (real)0.6;
	cData.tolerance = (real)0.1;

	if (fpsSandboxDemo)
	{
		for (int i = 0; i < numStackCubes; ++i)
		{
			elevate::CollisionDetector::boxAndHalfSpace(*cStackBoxes[i], plane, &cData);
			if (elevate::boxAndHalfSpaceIntersect(*cStackBoxes[i], plane))
			{
				cStackBoxes[i]->isOverlapping = true;
			}
		}

		for (int i = 0; i < numStackCubes; ++i)
		{
			for (int j = i + 1; j < numStackCubes; ++j)
			{
				elevate::CollisionDetector::boxAndBox(*cStackBoxes[i], *cStackBoxes[j], &cData);
			}


			elevate::Matrix4 transform, otherTransform;
			elevate::Vector3 position, otherPosition;
			for (Block* block = blocks; block < blocks + 9; block++)
			{
				if (!block->exists) continue;

				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*block, *cStackBoxes[i], &cData);

			}
		}
			for (int i = 0; i < crates.size(); i++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(crates[i]->shape), *static_cast<CollisionBox*>(floor->shape), &cData);
			}

		for (int i = 0; i < ammoCount; ++i)
		{
			AmmoRound& r = ammoPool[i];
			if (!r.active)
				continue;

			if (!cData.hasMoreContacts()) return;
			elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(wall->shape), *r.coll, &cData);
			elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(wall2->shape), *r.coll, &cData);
			elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(wall3->shape), *r.coll, &cData);
			elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(wall4->shape), *r.coll, &cData);
			elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(floor->shape), *r.coll, &cData);


			for (int i = 0; i < crates.size(); i++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(crates[i]->shape), *r.coll, &cData);
				elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(crates[i]->shape), *static_cast<CollisionBox*>(floor->shape), &cData);
			}

			for (Bone* bone = bones; bone < bones + 12; bone++)
			{
				if (!cData.hasMoreContacts()) return;

				elevate::CollisionSphere boneSphere = bone->getCollisionSphere();

				elevate::CollisionDetector::sphereAndSphere(boneSphere, *r.coll, &cData);
			};

			for (int i = 0; i < numStackCubes; ++i)
			{
				for (int j = i + 1; j < numStackCubes; ++j)
				{
					elevate::CollisionDetector::boxAndSphere(*cStackBoxes[i], *r.coll, &cData);
				}
			}

			elevate::Matrix4 transform, otherTransform;
			elevate::Vector3 position, otherPosition;
			for (Block* block = blocks; block < blocks + 9; block++)
			{
				if (!cData.hasMoreContacts()) return;
				if (elevate::CollisionDetector::boxAndSphere(*block, *r.coll, &cData))
				{
					if (firstHit && block->exists)
					{
						hit = true;
						firstHit = false;
						fracture_contact = cData.contactCount - 1;
					}

				}
			}

		}
		elevate::Matrix4 transform, otherTransform;
		elevate::Vector3 position, otherPosition;
		for (Bone* bone = bones; bone < bones + 12; bone++)
		{
			elevate::CollisionSphere boneSphere = bone->getCollisionSphere();
			if (!cData.hasMoreContacts()) return;
			elevate::CollisionDetector::sphereAndHalfSpace(boneSphere, plane, &cData);





			// Check for collisions with each other box
			for (Bone* other = bone + 1; other < bones + 12; other++)
			{
				if (!cData.hasMoreContacts()) return;

				elevate::CollisionSphere otherSphere = other->getCollisionSphere();

				elevate::CollisionDetector::sphereAndSphere(
					boneSphere,
					otherSphere,
					&cData
				);
			}
		}
		// Check for joint violation
		for (elevate::Joint* joint = joints; joint < joints + 11; joint++)
		{
			if (!cData.hasMoreContacts()) return;
			unsigned added = joint->addContact(cData.contacts, cData.contactsLeft);
			cData.addContacts(added);
		}



		// Perform collision detection
		for (Block* block = blocks; block < blocks + 9; block++)
		{
			if (!block->exists) continue;

			// Check for collisions with the ground plane
			if (!cData.hasMoreContacts()) return;
			elevate::CollisionDetector::boxAndHalfSpace(*block, plane, &cData);

			// Check for collisions with each other box
			for (Block* other = block + 1; other < blocks + 9; other++)
			{
				if (!other->exists) continue;

				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*block, *other, &cData);
			}

		}
		buildContactDebugLines();

	}
}
void GameManager::render()
{
	for (auto obj : gameObjects) {
		renderer->draw(obj, view, projection);
	}

	drawDebugLines();

}
