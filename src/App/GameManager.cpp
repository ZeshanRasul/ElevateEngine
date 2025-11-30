#include <algorithm>

#include "App/GameManager.h"
#include "Physics/core.h"

#include "imgui/imgui.h"
#include "imgui/backend/imgui_impl_glfw.h"
#include "imgui/backend/imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include <iostream>

static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
static ImGuizmo::MODE currentMode = ImGuizmo::LOCAL;

DirLight dirLight = {
		glm::vec3(-0.2f, -1.0f, -0.3f),

		glm::vec3(0.58f, 0.58f, 0.74f),
		glm::vec3(0.85f),
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

	cubemapShader.loadShaders("C:/dev/ElevateEngine/src/Shaders/cubemap_vertex.glsl", "C:/dev/ElevateEngine/src/Shaders/cubemap_fragment.glsl");

	camera = new Camera(glm::vec3(18.0f, 5.0f, 18.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);

	inputManager->setContext(camera, this, width, height);

	cubemapFaces = {
	"src/Assets/Textures/Cubemap/right.png",
	"src/Assets/Textures/Cubemap/left.png",
	"src/Assets/Textures/Cubemap/top.png",
	"src/Assets/Textures/Cubemap/bottom.png",
	"src/Assets/Textures/Cubemap/front.png",
	"src/Assets/Textures/Cubemap/back.png"
	};

	cubemap = new Cubemap(&cubemapShader);
	cubemap->LoadMesh();
	cubemap->LoadCubemap(cubemapFaces);

	contacts = new elevate::Contact[2056];
	cData.contactArray = contacts;
	resolver.setIterations(256 * 16, 256 * 16);
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
	rbGravity = new Gravity(elevate::Vector3(0.0f, -9.81f * 0.5f, 0.0f));
	carEngine = new CarPropulsion(elevate::Vector3(0.0f, 0.0f, 0.0f));
	shapeFactory = new elevate::ShapeFactory();
	spawnFactory = new elevate::SpawnFactory(spawnContext);

	InitDebugDrawGL();

}

void GameManager::setupCamera(unsigned int width, unsigned int height)
{
	if (!showCar)
	{
		view = camera->GetViewMatrix();
	}
	cubemapView = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 5000.0f);
}

void GameManager::setSceneData()
{
	renderer->setScene(view, projection, cubemapView, dirLight);
}

void GameManager::reset()
{
	if (currentScene == SceneType::Aeroplane)
	{
		ResetPlane();
		return;
	}
	else if (currentScene == SceneType::Car)
	{
		// TODO: Reset car pos
		return;
	}

	ResetState();

	floor = spawnFactory->CreateFloor(
		elevate::Vector3(200.0f, 1.0f, 200.0f),
		&cubeShader,
		elevate::Vector3(0.0f, -1.0f, 0.0f)
	);
	floor->mesh->setGameManager(this);
	floor->mesh->SetColor(glm::vec3(0.3f, 0.8f, 0.3f));
	static_cast<Cube*>(floor->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Ground/TCom_Scifi_Floor2_4k_albedo.png");
	floor->name = "Floor";
	gameObjects.push_back(floor->mesh);
	allSceneObjects.push_back(floor);
	sceneObjectIndex++;

	wall = spawnFactory->CreateWall(
		elevate::Vector3(1.0f, 60.0f, 200.0f),
		elevate::Vector3(200.0f, 60.0f, 0.0f),
		&cubeShader
	);
	wall->mesh->setGameManager(this);
	wall->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	wall->mesh->SetTexTiling(4.0f);
	static_cast<Cube*>(wall->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
	wall->name = "Wall 1";
	gameObjects.push_back(wall->mesh);
	allSceneObjects.push_back(wall);
	sceneObjectIndex++;

	wall2 = spawnFactory->CreateWall(
		elevate::Vector3(1.0f, 60.0f, 200.0f),
		elevate::Vector3(-200.0f, 60.0f, 0.0f),
		&cubeShader
	);
	wall2->mesh->setGameManager(this);
	wall2->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	wall2->mesh->SetTexTiling(4.0f);
	static_cast<Cube*>(wall2->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
	wall2->name = "Wall 2";
	gameObjects.push_back(wall2->mesh);
	allSceneObjects.push_back(wall2);
	sceneObjectIndex++;

	wall3 = spawnFactory->CreateWall(
		elevate::Vector3(200.0f, 60.0f, 1.0f),
		elevate::Vector3(0.0f, 60.0f, 200.0f),
		&cubeShader
	);
	wall3->mesh->setGameManager(this);
	wall3->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	wall3->mesh->SetTexTiling(4.0f);
	wall3->name = "Wall 3";

	static_cast<Cube*>(wall3->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
	allSceneObjects.push_back(wall3);
	sceneObjectIndex++;
	gameObjects.push_back(wall3->mesh);

	wall4 = spawnFactory->CreateWall(
		elevate::Vector3(200.0f, 60.0f, 1.0f),
		elevate::Vector3(0.0f, 60.0f, -200.0f),
		&cubeShader
	);
	wall4->mesh->setGameManager(this);
	wall4->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.3f));
	wall4->mesh->SetTexTiling(4.0f);
	static_cast<Cube*>(wall4->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Wall/TCom_SciFiPanels09_512_albedo.png");
	wall4->name = "Wall 4";

	gameObjects.push_back(wall4->mesh);
	allSceneObjects.push_back(wall4);
	sceneObjectIndex++;

	spawnFactory->BuildDominoLine(
		elevate::Vector3(75.0f, 3.5f, -150.0f),
		elevate::Vector3(0.0f, 0.0f, 1.0f),
		15,
		elevate::Vector3(2.0f, 5.0f, 0.8f),
		0.5f,
		2.0f,
		dominoes);

	for (PhysicsObject* domino : dominoes)
	{
		int i = 0;
		elevate::Matrix3 tensor;
		tensor.setBlockInertiaTensor(elevate::Vector3(2.0f, 5.0f, 0.8f), 0.5f);
		domino->body->setInertiaTensor(tensor);
		domino->body->calculateDerivedData();
		domino->shape->calculateInternals();
		domino->name = "Domino " + std::to_string(i++);

		Cube* cube = new Cube(
			domino->body->getTransform().getAxisVector(3),
			elevate::Vector3(2.0f, 5.0f, 0.8f),
			&ammoShader,
			this);
		cube->LoadMesh();
		domino->mesh = cube;

		domino->mesh->setGameManager(this);
		domino->mesh->SetColor(glm::vec3(0.8f, 0.9f, 0.8f));
		gameObjects.push_back(domino->mesh);
		allSceneObjects.push_back(domino);
		sceneObjectIndex++;
	}

	ragdolls.push_back(spawnFactory->CreateRagdoll(elevate::Vector3(0.0f, 4.0f, 0.0f)));
	ragdolls.push_back(spawnFactory->CreateRagdoll(elevate::Vector3(10.0f, 4.0f, 0.0f)));
	ragdolls.push_back(spawnFactory->CreateRagdoll(elevate::Vector3(20.0f, 4.0f, 0.0f)));

	for (Ragdoll* ragdoll : ragdolls)
	{

		for (Bone& bone : ragdoll->bones)
		{
			Cube* c = new Cube(bone.getTransform().getAxisVector(3), bone.halfSize * 2, &ammoShader, this);
			c->LoadMesh();
			c->SetColor(glm::vec3(0.9f, 0.3f, 0.4f));
			bone.visual = c;
			gameObjects.push_back(c);
		}
	}

	firstHit = true;

	elevate::Vector3 blockPos(-50, 25, 50);
	elevate::Vector3 blockScale(15, 15, 15);

	Cube* cube = new Cube(blockPos, elevate::Vector3(15, 15, 15), &cubeShader, this);
	cube->LoadMesh();
	blockTexture = cube->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Block/cracked_concrete_wall_diff_2k.png");
	cube->SetAngle(0.0f);
	cube->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
	cube->SetColor(glm::vec3(0.2f, 0.1f, 0.5f));
	blocks[0].visual = cube;
	gameObjects.push_back(cube);

	// Set the first block
	// Set the first block
	blocks[0].halfSize = blockScale * 0.5f;
	blocks[0].body->setPosition(elevate::Vector3(blockPos));
	blocks[0].body->setOrientation(elevate::Quaternion(1, 0, 0, 0));
	blocks[0].body->setVelocity(elevate::Vector3(0, 0, 0));
	blocks[0].body->setRotation(elevate::Vector3(0, 0, 0));
	real mass = 100.0f;
	blocks[0].body->setMass(mass);
	elevate::Matrix3 it;
	it.setBlockInertiaTensor(blocks[0].halfSize, mass);
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

	stackCrateTexture = static_cast<Cube*>(crates[0]->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Crate/rusted_shutter_diff_2k.png");

	for (int i = 0; i < crates.size(); ++i)
	{
		crates[i]->mesh->SetShader(&cubeShader);
		crates[i]->mesh->setGameManager(this);
		crates[i]->mesh->SetColor(glm::vec3(0.3f, 0.7f, 0.2f));
		static_cast<Cube*>(crates[i]->mesh)->SetTexture(stackCrateTexture);
		crates[i]->name = "Crate " + std::to_string(i);
		gameObjects.push_back(crates[i]->mesh);
		allSceneObjects.push_back(crates[i]);
		sceneObjectIndex++;
	}

	numStackCubes = 5;

	elevate::Vector3 cubeHalfSize(2.0f, 2.0f, 2.0f);
	glm::vec3       renderScale(4.0f, 4.0f, 4.0f);

	for (int i = 0; i < numStackCubes; i++)
	{


		//	rbWorld->removeBody(stackBodies[i]);

		delete cStackBoxes[i];

		float centerY = 3.3f + i * 4.3f;

		elevate::Vector3 pos(-20.0f, centerY, -20.0f);
		elevate::Vector3 scale(renderScale.x, renderScale.y, renderScale.z);

		// Render cube
		Cube* cube = new Cube(pos, scale, &cubeShader, this);
		cube->LoadMesh();
		cube->SetAngle(0.0f);
		cube->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
		cube->SetColor(glm::vec3(0.2f, 0.7f, 0.3f));
		crateTexture = cube->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Crate/TCom_MetalSheetCladding2_Galvanized_512_albedo.png");
		gameObjects.push_back(cube);
		stackCubes[i] = cube;

		// Physics body
		elevate::RigidBody* body = new elevate::RigidBody();
		body->setAwake(true);
		body->setPosition(pos);
		body->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		body->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		body->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));

		real mass = 1.0f;
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



	spawnFactory->BuildBrickWall(
		elevate::Vector3(0.0f, 5.75f, -80.0f),
		15,
		5,
		elevate::Vector3(6.5f, 2.5f, 1.5f),
		0.5f,
		false,
		bricks,
		&cubeShader
	);

	brickTexture = static_cast<Cube*>(bricks[0]->mesh)->LoadTextureFromFile(
		"C:/dev/ElevateEngine/src/Assets/Textures/Brick/painted_brick_diff_2k.png");

	for (size_t i = 0; i < bricks.size(); ++i)
	{
		bricks[i]->mesh->SetShader(&cubeShader);
		static_cast<Cube*>(bricks[i]->mesh)->SetTexture(brickTexture);
		bricks[i]->mesh->setGameManager(this);
		bricks[i]->mesh->SetColor(glm::vec3(0.6f, 0.4f, 0.2f));
		gameObjects.push_back(bricks[i]->mesh);
	}



	crate = spawnFactory->SpawnCrate(
		elevate::Vector3(40.0f, 1.5f, 20.0f),
		&cubeShader,
		elevate::Vector3(8.0f, 4.0f, 2.0f),
		1.0f
	);

	crate->mesh->SetShader(&cubeShader);
	static_cast<Cube*>(crate->mesh)->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Crate/TCom_Scifi_Pattern_4K_albedo.png");
	//static_cast<Cube*>(crate->mesh)->SetTexture(crateTexture);
	crate->mesh->setGameManager(this);
	crate->mesh->SetColor(glm::vec3(0.8f, 0.3f, 0.1f));
	crate->name = "Crate Main";

	gameObjects.push_back(crate->mesh);
	allSceneObjects.push_back(crate);
	sceneObjectIndex++;
}

void GameManager::ResetState()
{
	hit = false;

	cData.contactCount = 0;

	//ResetPlane();
	showPlane = false;
	showCar = false;
	fpsSandboxDemo = false;


	for (int i = 0; i < runTimeBoxes.size(); i++)
	{
		gameObjects.erase(
			std::remove(
				gameObjects.begin(),
				gameObjects.end(),
				runTimeBoxes[i]->mesh),
			gameObjects.end()
		);
		delete runTimeBoxes[i]->mesh;
		delete runTimeBoxes[i];
	}

	for (int i = 0; i < crateStacks.size(); i++)
	{
		for (int j = 0; j < crateStacks[i].size(); j++)
		{
			gameObjects.erase(
				std::remove(
					gameObjects.begin(),
					gameObjects.end(),
					crateStacks[i][j]->mesh),
				gameObjects.end()
			);
			delete crateStacks[i][j]->mesh;
			delete crateStacks[i][j];
		}
	}

	for (int i = 0; i < runtimeWalls.size(); i++)
	{
		for (int j = 0; j < runtimeWalls[i].size(); j++)
		{
			gameObjects.erase(
				std::remove(
					gameObjects.begin(),
					gameObjects.end(),
					runtimeWalls[i][j]->mesh),
				gameObjects.end()
			);
			delete runtimeWalls[i][j]->mesh;
			delete runtimeWalls[i][j];
		}
	}



	for (int i = 0; i < runTimeSpheres.size(); i++)
	{
		gameObjects.erase(
			std::remove(
				gameObjects.begin(),
				gameObjects.end(),
				runTimeSpheres[i]->mesh),
			gameObjects.end()
		);
		delete runTimeSpheres[i]->mesh;
		delete runTimeSpheres[i];
	}

	for (PhysicsObject* domino : dominoes)
	{
		gameObjects.erase(
			std::remove(
				gameObjects.begin(),
				gameObjects.end(),
				domino->mesh),
			gameObjects.end()
		);
	}

	for (PhysicsObject* domino : dominoes)
	{
		delete domino->mesh;
		delete domino;
	}

	dominoes.clear();

	for (Ragdoll* ragdoll : ragdolls)
	{
		for (Bone& bone : ragdoll->bones)
		{
			if (bone.visual)
			{
				gameObjects.erase(
					std::remove(
						gameObjects.begin(),
						gameObjects.end(),
						bone.visual),
					gameObjects.end()
				);
			}

			delete bone.visual;
			bone.visual = nullptr;

		}
	}

	for (Ragdoll* ragdoll : ragdolls)
	{
		delete ragdoll;
	}
	ragdolls.clear();

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
	//for (int i = 0; i < cStackBoxes->size(); i++)
	//{
	//	delete cStackBoxes[i];
	//}

	//if (crate)
	//{
	//	gameObjects.erase(
	//		std::remove(
	//			gameObjects.begin(),
	//			gameObjects.end(),
	//			crate->mesh),
	//		gameObjects.end()
	//	);
	//	delete crate;
	//}

	for (GameObject* obj : gameObjects)
	{
		gameObjects.erase(
			std::remove(
				gameObjects.begin(),
				gameObjects.end(),
				obj),
			gameObjects.end()
		);
	}
	for (PhysicsObject* crate : crates)
	{
		gameObjects.erase(
			std::remove(
				gameObjects.begin(),
				gameObjects.end(),
				crate->mesh),
			gameObjects.end()
		);

		//delete crate;
	}


	for (PhysicsObject* crate : crates)
	{
		crates.erase(
			std::remove(
				crates.begin(),
				crates.end(),
				crate),
			crates.end()
		);

		//delete crate;
	}

	crates.clear();
	for (PhysicsObject* brick : bricks)
	{
		gameObjects.erase(
			std::remove(
				gameObjects.begin(),
				gameObjects.end(),
				brick->mesh),
			gameObjects.end()
		);

	}


	for (PhysicsObject* brick : bricks)
	{
		crates.erase(
			std::remove(
				crates.begin(),
				crates.end(),
				brick),
			crates.end()
		);

	}

	bricks.clear();
	for (PhysicsObject* obj : dominoes)
	{
		delete obj;
	}

	for (Ragdoll* ragdoll : ragdolls)
	{
		for (Bone& bone : ragdoll->bones)
		{
			delete bone.visual;
		}
		delete ragdoll;
	}

	for (int i = 0; i < numStackCubes; i++)
	{
		gameObjects.erase(
			std::remove(
				gameObjects.begin(),
				gameObjects.end(),
				stackCubes[i]),
			gameObjects.end()
		);
	}

	switch (currentScene)
	{
	case SceneType::DemoShowcase:
	{
		//reset();
		fpsSandboxDemo = true;
		break;
	}
	case SceneType::Aeroplane:
	{
		ResetPlane();
		showPlane = true;
		break;
	}
	case SceneType::Car:
	{
		car_throttle = 0.0f;
		showCar = true;
		break;
	}
	default:
		break;
	};
}
void GameManager::ResetCar()
{
	car->body->setPosition(elevate::Vector3(0.0f, 1.2f, 0.0f));
	car->throttle = 0.0f;
	car->steerAngle = 0.0f;
	car->body->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
	car->body->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));
	car->chassisMesh->SetOrientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
	car->chassisMesh->SetPosition(elevate::Vector3(0.0f, 10.2f, 0.0f));
	car->body->calculateDerivedData();
}
void GameManager::OnQPressed()
{
	rudder_control += 0.1f;
}

void GameManager::OnEPressed()
{
	rudder_control -= 0.1f;
}

void GameManager::OnWPressed()
{
	if (showPlane)
	{
		left_wing_control += 0.1f;
		right_wing_control += 0.1f;
	}

	if (showCar)
	{
		//car_throttle += 0.1f * 50;
//	car->throttle += 0.1f;
	}
}

void GameManager::OnSPressed()
{
	if (showPlane)
	{
		left_wing_control -= 0.1f;
		right_wing_control -= 0.1f;
	}

	if (showCar)
	{
		//	car->throttle -= 0.1f;
	}
}

void GameManager::OnDPressed()
{
	if (showPlane)
	{
		left_wing_control -= 0.1f;
		right_wing_control += 0.1f;
	}

	else if (showCar)
	{
		car->steerAngle += 0.05f;
	}

}

void GameManager::OnAPressed()
{
	if (showPlane)
	{
		left_wing_control += 0.1f;
		right_wing_control -= 0.1f;
	}
	else if (showCar)
	{
		car->steerAngle -= 0.05f;
	}
}

void GameManager::OnLeftClick()
{
	fireRound(AmmoType::Pistol);
}

void GameManager::OnZPressed()
{
	if (showPlane)
	{
		ResetPlane();
	}

	if (showCar)
	{
		ResetCar();
	}
}

void GameManager::OnHPressed()
{
	showUI = !showUI;
}

void GameManager::setUpDebugUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	ImGuiID dockspace_id = ImGui::GetID("My Dockspace");
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void GameManager::showDebugUI()
{
	if (showUI)
	{
		ShowLightControlWindow(dirLight);
		ShowSpawnObjectWindow();
		ShowPerformanceWindow();
		DrawPhysicsObjectsCombo();
		ShowCameraControlWindow(*camera);
		ShowEngineWindow();
	}
}

void GameManager::renderDebugUI()
{
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetMainViewport()->Pos.x,
		ImGui::GetMainViewport()->Pos.y,
		ImGui::GetMainViewport()->Size.x,
		ImGui::GetMainViewport()->Size.y);

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
	if (!inputManager->isCameraControlled())
	{
		return;
	}

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
	case AmmoType::Pistol: speed = 50.0f; break;
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

void GameManager::ShowSpawnObjectWindow()
{
	ImGui::Begin("Spawner");

	ImGui::Text("Spawn Aeroplane");
	if (ImGui::Button("Spawn Aeroplane"))
	{
		glm::vec3 camPos = camera->Position;
		glm::vec3 camFront = glm::normalize(camera->Front);
		glm::vec3 spawnPos = camPos + camFront * 10.0f;
		ResetPlane();
		showPlane = true;
	}

	ImGui::Checkbox("Aeroplane Physics Enabled", &showPlane);
	ImGui::Checkbox("Car Physics Enabled", &showCar);

	ImGui::Text("Spawn Box");
	ImGui::InputFloat("Box Mass", &boxMass);
	ImGui::InputFloat3("Box Size", boxSize);
	ImGui::ColorEdit4("Box Size", boxColor);

	if (ImGui::Button("Spawn Box"))
	{
		glm::vec3 camPos = camera->Position;
		glm::vec3 camFront = glm::normalize(camera->Front);

		glm::vec3 spawnPos = camPos + camFront * 6.0f;

		PhysicsObject* box = spawnFactory->SpawnBox(
			elevate::Vector3(spawnPos.x, spawnPos.y, spawnPos.z),
			elevate::Vector3(boxSize[0], boxSize[1], boxSize[2]),
			boxMass,
			"",
			PhysicsMaterialId::Default,
			&ammoShader
		);

		box->mesh->SetColor(glm::vec3(boxColor[0], boxColor[1], boxColor[2]));
		gameObjects.push_back(box->mesh);
		runTimeBoxes.push_back(box);
	}
	ImGui::Text("Spawn Sphere");
	ImGui::InputFloat("Sphere Radius", &sphereRadius);
	ImGui::InputFloat("Sphere Mass", &sphereMass);
	ImGui::ColorEdit4("Sphere Color", sphereColor);

	if (ImGui::Button("Spawn Sphere"))
	{
		glm::vec3 camPos = camera->Position;
		glm::vec3 camFront = glm::normalize(camera->Front);

		glm::vec3 spawnPos = camPos + camFront * 6.0f;

		PhysicsObject* sphere = spawnFactory->SpawnSphere(
			elevate::Vector3(spawnPos.x, spawnPos.y, spawnPos.z),
			sphereRadius,
			sphereMass,
			"",
			PhysicsMaterialId::Default,
			&ammoShader
		);

		sphere->mesh->SetColor(glm::vec3(sphereColor[0], sphereColor[1], sphereColor[2]));
		gameObjects.push_back(sphere->mesh);
		runTimeSpheres.push_back(sphere);
	}


	if (ImGui::Button("Spawn Ragdoll"))
	{
		glm::vec3 camPos = camera->Position;
		glm::vec3 camFront = glm::normalize(camera->Front);

		glm::vec3 spawnPos = camPos + camFront * 6.0f;

		Ragdoll* ragdoll = spawnFactory->CreateRagdoll(
			elevate::Vector3(spawnPos.x, spawnPos.y, spawnPos.z)
		);

		for (Bone& bone : ragdoll->bones)
		{
			Cube* c = new Cube(bone.getTransform().getAxisVector(3), bone.halfSize * 2, &ammoShader, this);
			c->LoadMesh();
			c->SetColor(glm::vec3(0.9f, 0.3f, 0.4f));
			bone.visual = c;
			gameObjects.push_back(c);
		}

		ragdolls.push_back(ragdoll);
	}

	ImGui::Text("Spawn Crate Stack");
	ImGui::InputInt("Stack Width", &stackWidth);
	ImGui::InputInt("Stack Height", &stackHeight);
	ImGui::InputInt("Stack Depth", &stackDepth);
	ImGui::InputFloat3("Crate Size", stackBoxSize);
	ImGui::InputFloat("Crate Mass", &stackCrateMass);

	if (ImGui::Button("Spawn Crate Stack"))
	{
		crateStacks.reserve(stackIndex + 1);


		crateStacks.push_back(std::vector<PhysicsObject*>());
		crateStacks[stackIndex].reserve(stackWidth * stackHeight * stackDepth);
		glm::vec3 camPos = camera->Position;
		glm::vec3 camFront = glm::normalize(camera->Front);

		glm::vec3 spawnPos = camPos + camFront * 6.0f;

		spawnFactory->BuildCrateStack(
			elevate::Vector3(spawnPos.x, spawnPos.y, spawnPos.z),
			stackWidth,
			stackHeight,
			stackDepth,
			elevate::Vector3(stackBoxSize[0], stackBoxSize[1], stackBoxSize[2]),
			stackCrateMass,
			&ammoShader,
			crateStacks[stackIndex++]
		);

		for (int i = 0; i < crateStacks[stackIndex - 1].size(); i++)
		{
			crateStacks[stackIndex - 1][i]->mesh->SetShader(&cubeShader);
			crateStacks[stackIndex - 1][i]->mesh->setGameManager(this);
			crateStacks[stackIndex - 1][i]->mesh->SetColor(glm::vec3(0.3f, 0.7f, 0.2f));
			static_cast<Cube*>(crateStacks[stackIndex - 1][i]->mesh)->SetTexture(crateTexture);
			gameObjects.push_back(crateStacks[stackIndex - 1][i]->mesh);
		}
	}

	ImGui::Text("Spawn Brick Wall");
	ImGui::InputInt("Wall Width", &wallWidth);
	ImGui::InputInt("Wall Height", &wallHeight);
	ImGui::InputFloat3("Brick Size", brickSizeInput);
	ImGui::Checkbox("Is Staggered?", &isStaggered);
	ImGui::InputFloat("Brick Mass", &brickMass);

	const elevate::Vector3 brickSize = elevate::Vector3(brickSizeInput[0], brickSizeInput[1], brickSizeInput[2]);

	if (ImGui::Button("Spawn Brick Wall"))
	{
		runtimeWalls.reserve(wallIndex + 1);

		runtimeWalls.push_back(std::vector<PhysicsObject*>());
		runtimeWalls[wallIndex].reserve(wallWidth * wallHeight);
		glm::vec3 camPos = camera->Position;
		glm::vec3 camFront = glm::normalize(camera->Front);

		glm::vec3 spawnPos = camPos + camFront * 6.0f;

		spawnFactory->BuildBrickWall(
			elevate::Vector3(spawnPos.x, spawnPos.y, spawnPos.z),
			wallWidth,
			wallHeight,
			brickSize,
			brickMass,
			isStaggered,
			runtimeWalls[wallIndex++],
			&cubeShader
		);

		for (int i = 0; i < runtimeWalls[wallIndex - 1].size(); i++)
		{
			runtimeWalls[wallIndex - 1][i]->mesh->SetShader(&cubeShader);
			runtimeWalls[wallIndex - 1][i]->mesh->setGameManager(this);
			runtimeWalls[wallIndex - 1][i]->mesh->SetColor(glm::vec3(0.3f, 0.7f, 0.2f));
			static_cast<Cube*>(runtimeWalls[wallIndex - 1][i]->mesh)->SetTexture(brickTexture);
			gameObjects.push_back(runtimeWalls[wallIndex - 1][i]->mesh);
		}
	}

	ImGui::Text("Spawn Domino Line");
	ImGui::InputFloat3("Line Direction", lineDirectionInput);
	ImGui::InputFloat3("Domino Size", dominoSizeInput);
	ImGui::InputInt("Domino Count", &dominoCount);
	ImGui::InputFloat("Domino Mass", &dominoMass);
	ImGui::InputFloat("Domino Spacing", &dominoSpacing);

	const elevate::Vector3 dominoDirection = elevate::Vector3(lineDirectionInput[0], lineDirectionInput[1], lineDirectionInput[2]);
	const elevate::Vector3 dominoSize = elevate::Vector3(dominoSizeInput[0], dominoSizeInput[1], dominoSizeInput[2]);

	if (ImGui::Button("Spawn Domino Line"))
	{
		runtimeDominoLines.reserve(dominoLineIndex + 1);

		runtimeDominoLines.push_back(std::vector<PhysicsObject*>());
		runtimeDominoLines[dominoLineIndex].reserve(dominoCount);
		glm::vec3 camPos = camera->Position;
		glm::vec3 camFront = glm::normalize(camera->Front);

		glm::vec3 spawnPos = camPos + camFront * 6.0f;

		spawnFactory->BuildDominoLine(
			elevate::Vector3(spawnPos.x, spawnPos.y, spawnPos.z),
			elevate::Vector3(dominoDirection.x, dominoDirection.y, dominoDirection.z),
			dominoCount,
			elevate::Vector3(dominoSize.x, dominoSize.y, dominoSize.z),
			dominoMass,
			dominoSpacing,
			runtimeDominoLines[dominoLineIndex++]
		);

		for (int i = 0; i < runtimeDominoLines[dominoLineIndex - 1].size(); i++)
		{
			runtimeDominoLines[dominoLineIndex - 1][i]->mesh->SetShader(&ammoShader);
			runtimeDominoLines[dominoLineIndex - 1][i]->mesh->setGameManager(this);
			runtimeDominoLines[dominoLineIndex - 1][i]->mesh->SetColor(glm::vec3(0.8f, 0.7f, 0.8f));
			gameObjects.push_back(runtimeDominoLines[dominoLineIndex - 1][i]->mesh);
		}
	}


	ImGui::Text("Spawn Fracture Box");
	ImGui::InputFloat3("Block Position", blockPos);
	ImGui::InputFloat3("Block Size", blockScale);
	ImGui::InputFloat("Block Mass", &blockMass);

	if (ImGui::Button("Spawn Fracture Box"))
	{
		elevate::Vector3 pos(blockPos[0], blockPos[1], blockPos[2]);
		elevate::Vector3 scale(blockScale[0], blockScale[1], blockScale[2]);

		Block block;
		Cube* cube = new Cube(pos, scale, &cubeShader, this);
		cube->LoadMesh();
		cube->SetTexture(blockTexture);
		cube->SetAngle(0.0f);
		cube->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
		cube->SetColor(glm::vec3(0.2f, 0.1f, 0.5f));
		block.visual = cube;
		gameObjects.push_back(cube);

		block.halfSize = scale * 0.5f;
		block.body->setPosition(pos);
		block.body->setOrientation(elevate::Quaternion(1, 0, 0, 0));
		block.body->setVelocity(elevate::Vector3(0, 0, 0));
		block.body->setRotation(elevate::Vector3(0, 0, 0));
		block.body->setMass(blockMass);
		elevate::Matrix3 it;
		it.setBlockInertiaTensor(block.halfSize, blockMass);
		block.body->setInertiaTensor(it);
		block.body->setDamping(0.9f, 0.9f);
		block.body->calculateDerivedData();
		block.calculateInternals();

		block.body->setAcceleration(elevate::Vector3::GRAVITY);
		block.body->setAwake(true);
		block.body->setCanSleep(true);
		block.exists = true;

		runtimeFractureBlocks.push_back(block);
	}
	ImGui::End();

}

void GameManager::ShowEngineWindow()
{
	ImGui::Begin("Simulation Controls");

	const char* preview = SceneTypeToString(currentScene);

	if (ImGui::BeginCombo("Level Select", preview)) {
		for (int i = 0; i < sceneTypes.size(); ++i) {
			SceneType type = sceneTypes[i];
			bool isSelected = (currentScene == type);

			if (ImGui::Selectable(SceneTypeToString(type), isSelected))
			{
				currentScene = type;
				selectedIndex = i;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Pause Simulation"))
	{
		isPaused = !isPaused;
	}

	if (ImGui::Button("Reset Simulation"))
	{
		reset();

	}

	if (ImGui::Button("Step"))
	{
		isPaused = true;
		update(1.0f / 60.0f);
	}

	ImGui::Checkbox("Enable Gravity", &enableGravity);

	ImGui::End();

	ImGui::Begin("Physics Properties");

	ImGui::SliderFloat("Friction", (float*)&friction, 0.0f, 1.0f);
	ImGui::SliderFloat("Restitution", (float*)&restitution, 0.0f, 1.0f);
	ImGui::SliderFloat("Tolerance", (float*)&tolerance, 0.0f, 1.0f);

	if (showCar)
	{
		//	ImGui::SliderFloat("Car Throttle", (float*)&car_throttle, 0.0f, 10.0f);
		//	ImGui::SliderFloat("Car Steering Angle", (float*)&car_steerAngle, 0.0f, 10.0f);
	}

	cData.friction = friction;
	cData.restitution = restitution;
	cData.tolerance = tolerance;

	ImGui::SliderFloat("Ground Plane OFfset", (float*)&groundPlaneOffset, -200.0f, 200.0f);

	ImGui::SliderFloat3("Gravity", gravity, -100.0f, 100.0f);

	if (showPlane)
	{
		ImGui::Text("Left Wing Control: %.2f", left_wing_control);
		ImGui::Text("Right Wing Control: %.2f", right_wing_control);
		ImGui::Text("Rudder Control: %.2f", rudder_control);
		ImGui::Text("Plane position: X: %.2f Y: %.2f Z: %.2f",
			aircraft.getPosition().x,
			aircraft.getPosition().y,
			aircraft.getPosition().z);
		ImGui::Text("Plane velocity: X: %.2f Y: %.2f Z: %.2f",
			aircraft.getVelocity().x,
			aircraft.getVelocity().y,
			aircraft.getVelocity().z);
	}

	if (showCar)
	{
		ImGui::Text("Car Throttle: %.2f", car->throttle);
		ImGui::Text("Car Throttle: %.2f", car->steerAngle);
		ImGui::Text("Car position: X: %.2f Y: %.2f Z: %.2f",
			car->body->getPosition().x,
			car->body->getPosition().y,
			car->body->getPosition().z);
		ImGui::Text("Car velocity: X: %.2f Y: %.2f Z: %.2f",
			car->body->getVelocity().x,
			car->body->getVelocity().y,
			car->body->getVelocity().z);
	}

	ImGui::End();
}

void GameManager::ShowPerformanceWindow()
{
	ImGui::Begin("Performance");

	ImGui::Text("FPS: %.1f", fps);
	ImGui::Text("Avg FPS: %.1f", avgFps);
	ImGui::Text("Physics Time: %.1f ms", physicsTime);
	ImGui::Text("Average Physics Time: %.1f ms", physicsTimeAvg);
	ImGui::Text("Frame Time: %.1f ms", frameTimeMs);
	ImGui::Text("Elapsed Time: %.1f s", timeElapsed);

	ImGui::End();

}

void GameManager::DrawPhysicsObjectsCombo()
{
	ImGui::Begin("Elevate Editor");

	ImGui::Text("Selected Physics Object");

	if (allSceneObjects.empty())
	{
		ImGui::TextDisabled("No physics objects in scene.");
		sceneObjectIndex = -1;
		selectedSceneObject = nullptr;
		ImGui::End();
		return;
	}

	if (sceneObjectIndex < 0 || sceneObjectIndex >= (int)allSceneObjects.size())
	{
		sceneObjectIndex = 0;
		selectedSceneObject = allSceneObjects[0];
	}

	PhysicsObject* current = allSceneObjects[sceneObjectIndex];
	const char* previewText = selectedSceneObject->name.c_str(); // current->GetName().c_str()

	if (ImGui::BeginCombo("##PhysicsObjectCombo", previewText))
	{
		for (int i = 0; i < (int)allSceneObjects.size(); ++i)
		{
			ImGui::PushID(i);
			PhysicsObject* obj = allSceneObjects[i];
			std::string itemLabel;
			itemLabel = obj->name;


			bool isSelected = (sceneObjectIndex == i);
			if (ImGui::Selectable(itemLabel.c_str(), isSelected))
			{
				sceneObjectIndex = i;
				selectedSceneObject = obj;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
			ImGui::PopID();
		}

		ImGui::EndCombo();
	}


	if (selectedSceneObject)
	{
		glm::mat4 transform = selectedSceneObject->mesh->GetWorldMatrix();

		//ImGuizmo::SetDrawlist();
		//ImGuizmo::SetRect(ImGui::GetWindowPos().x,
		//	ImGui::GetWindowPos().y,
		//	ImGui::GetWindowWidth(),
		//	ImGui::GetWindowHeight());

		glm::mat4 proj = projection;
		ImGuizmo::Manipulate(glm::value_ptr(view),
			glm::value_ptr(proj),
			currentOperation,
			currentMode,
			glm::value_ptr(transform));

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 pos, rot, scale;
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform),
				glm::value_ptr(pos),
				glm::value_ptr(rot),
				glm::value_ptr(scale));


			selectedSceneObject->mesh->SetPosition(elevate::Vector3(pos.x, pos.y, pos.z));
			selectedSceneObject->mesh->SetScale(elevate::Vector3(scale.x, scale.y, scale.z));

			// rot is Euler degrees from ImGuizmo (XYZ)
			selectedSceneObject->mesh->SetOrientation(glm::quat(glm::radians(rot)));

			selectedSceneObject->body->setPosition(elevate::Vector3(pos.x, pos.y, pos.z));
			selectedSceneObject->body->setOrientation(elevate::Quaternion(
				selectedSceneObject->mesh->GetOrientation().w,
				selectedSceneObject->mesh->GetOrientation().x,
				selectedSceneObject->mesh->GetOrientation().y,
				selectedSceneObject->mesh->GetOrientation().z
			));

			selectedSceneObject->body->calculateDerivedData();

			// Sync back to Elevate body
			selectedSceneObject->body->setPosition({ pos.x, pos.y, pos.z });
			selectedSceneObject->body->calculateDerivedData();
		}
	}
	ImGui::End();

}

void GameManager::CalculatePerformanceMetrics(float deltaTime)
{
	fps = 1.0f / deltaTime;
	physicsTime = physicsTime * 1000.0f;

	fpsSum += fps;
	physicsTimeSum += physicsTime;
	frameCount++;

	if (frameCount == numFramesAvg)
	{
		avgFps = fpsSum / numFramesAvg;
		physicsTimeAvg = physicsTimeSum / numFramesAvg;
		fpsSum = 0.0f;
		frameCount = 0;
	}

	frameTimeMs = deltaTime * 1000.0f;

	timeElapsed += deltaTime;
	physicsTime = 0.0f;
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

	if (isPaused) return;

	if (!enableGravity)
	{
		//		rbGravity->setGravity(elevate::Vector3(0.0f, 0.0f, 0.0f));
	}
	else
	{
		//		rbGravity->setGravity(elevate::Vector3(gravity[0], gravity[1], gravity[2]));
	}
	//rbWorld->startFrame();
	//rbWorld->runPhysics(1.0f / 60.0f);

	physicsTime = (float)glfwGetTime();

	if (showPlane)
	{
		aircraft.clearAccumulator();

		// Add the propeller force
		elevate::Vector3 propulsion(-10.0f, 0, 0);
		propulsion = aircraft.getTransform().transformDirection(propulsion);
		aircraft.addForce(propulsion);

		left_wing.setControl(glm::clamp(left_wing_control, -1.0f, 1.0f));
		right_wing.setControl(glm::clamp(right_wing_control, -1.0f, 1.0f));
		rudder.setControl(glm::clamp(rudder_control, -1.0f, 1.0f));

		//	aircraft.addForce(elevate::Vector3(gravity[0], gravity[1], gravity[2]));
		//	aircraft.calculateDerivedData();
	// 
		aircraft.calculateDerivedData();

		// Add the forces acting on the aircraft.
		rbRegistry.updateForces(deltaTime);
		//	rbGravity->updateForce(&aircraft, deltaTime);
			// Update the aircraft's physics.
		aircraft.integrate(deltaTime);

		for (AircraftParts& part : aircraftParts)
		{
			part.coll->calculateInternals();
		}

		generateContacts();

		resolver.resolveContacts(cData.contactArray, cData.contactCount, deltaTime);

		elevate::Vector3 pos = aircraft.getPosition();

		for (AircraftParts& part : aircraftParts)
		{
			part.coll->calculateInternals();
			aircraft.getTransform();
			elevate::Vector3 worldPos = aircraft.getPointInWorldSpace(part.offset);
			part.mesh->SetPosition(worldPos);
			part.mesh->SetOrientation(glm::quat(
				(float)aircraft.getOrientation().r,
				(float)aircraft.getOrientation().i,
				(float)aircraft.getOrientation().j,
				(float)aircraft.getOrientation().k));
		}

		pos = aircraft.getPosition();

		if (showPlane)
		{
			camera->Position = glm::vec3(aircraft.getPosition().x + 15.0f, aircraft.getPosition().y + 5.0f, aircraft.getPosition().z);
			camera->Front = glm::normalize(glm::vec3(aircraft.getPosition().x, aircraft.getPosition().y, aircraft.getPosition().z) - camera->Position);
			camera->UpdateCameraVectors();
			view = camera->GetViewMatrix();
		}

	}

	if (showCar)
	{


		car->body->setAwake(true);
		car->body->clearAccumulator();
		//		car->chassis->body->clearAccumulator();
		//		car->chassis->body->calculateDerivedData();


		Matrix4 transform = car->body->getTransform();
		Vector3 forward = transform.getAxisVector(2);
		forward.normalize();


		float riseRate = 3.0f;
		float fallRate = 5.0f;
		float rate = (fabs(targetThrottle) > fabs(car->throttle)) ? riseRate : fallRate;

		car->throttle += (targetThrottle - car->throttle) * rate * deltaTime;
		car->throttle = std::clamp(car->throttle, -1.0f, 1.0f);



		Vector3 rearAvg = (car->wheels[2].offset + car->wheels[3].offset) * 0.5f;
		Vector3 frontAvg = (car->wheels[0].offset + car->wheels[1].offset) * 0.5f;

		real engineForce = 8000.0f;

		real maxSpeed = 40.0f;
		real speed = car->body->getVelocity().magnitude();
		real maxSpeedFactor = 1.0f - speed / maxSpeed;
		real speedFactor = std::max(0.0f, (float)maxSpeedFactor);
		speedFactor = 1.0f;

		Vector3 forceWorld = forward * (car->throttle * engineForce * speedFactor);
		rearAvg.y = 0.0f;

		car->body->addForceAtBodyPoint(forceWorld, rearAvg);

		car->body->addForce(elevate::Vector3::GRAVITY * car->body->getMass());

		Vector3 right = transform.getAxisVector(0);
		forward = transform.getAxisVector(2);
		right.normalize();
		forward.normalize();

		Vector3 vel = car->body->getVelocity();
		real vRight = vel * right;
		real vForward = vel * forward;

		Vector3 lateralVelocity = right * vRight;

		real baseGrip = 2.0f;

		static real lastVForward = 0.0f;
		real vForwardNow = vel * forward;
		real accelForward = (vForwardNow - lastVForward) / deltaTime;
		lastVForward = vForwardNow;

		real transfer = std::clamp(-accelForward * 0.1f, (real)-0.5f, (real)0.5f);

		real frontGrip = baseGrip * (1.0f + transfer);
		real rearGrip = baseGrip * (1.0f - transfer);
		
		Vector3 frontAccel = lateralVelocity * -frontGrip;
		Vector3 frontForce = frontAccel * car->body->getMass() * 0.5f;
		car->body->addForceAtBodyPoint(frontForce, frontAvg);

		Vector3 rearAccel = lateralVelocity * -rearGrip;
		Vector3 rearForce = rearAccel * car->body->getMass() * 0.5f;
		car->body->addForceAtBodyPoint(rearForce, rearAvg);

		float maxSteerAngle = 0.5f;
		float steerAngleRad = car->steerAngle * maxSteerAngle;

		real brakeStrength = 10000.0f;
		if (car->brake > 0.0f && fabs(vForward) > 0.1f)
		{
			Vector3 brakeDir = (vForward > 0.0f) ? (forward * -1.0f) : forward;

			real brakeForceMag = brakeStrength * car->brake;

			real maxUsefulBrake = fabs(vForward) * car->body->getMass() / deltaTime;
			brakeForceMag = std::min(brakeForceMag, maxUsefulBrake);

			Vector3 brakeForce = brakeDir * brakeForceMag;

			car->body->addForce(brakeForce);
		}

		if (fabs(steerAngleRad) > 0.001f && fabs(vForward) > 0.5f)
		{
			float steerInput = steerAngleRad / maxSteerAngle;

			Vector3 frontLeftLocal = car->wheels[0].offset;
			Vector3 frontRightLocal = car->wheels[1].offset;

			real steerStrength = 2000.0f;
			real speedFactorSteer = std::clamp(fabs(vForward) / 10.0f, 0.0, 1.0);

			real steerForceMag = steerStrength * steerInput * speedFactorSteer;

			Vector3 forceLeft = right * steerForceMag;
			Vector3 forceRight = (right * - 1) * steerForceMag;

			car->body->addForceAtBodyPoint(forceLeft, frontLeftLocal);
			car->body->addForceAtBodyPoint(forceRight, frontRightLocal);
		}


		car->body->integrate(deltaTime);

		generateContacts();
		resolver.resolveContacts(cData.contactArray, cData.contactCount, deltaTime);

		car->body->calculateDerivedData();
		car->chassis->calculateInternals();

		Matrix4 boxTx = car->chassis->getTransform();;

		Vector3 chassisPos = boxTx.getAxisVector(3);
		car->chassisMesh->SetPosition(chassisPos);

		car->chassisMesh->SetOrientation(glm::quat(
			(float)car->body->getOrientation().r,
			(float)car->body->getOrientation().i,
			(float)car->body->getOrientation().j,
			(float)car->body->getOrientation().k));


		glm::quat bodyQ(
			(float)car->body->getOrientation().r,
			(float)car->body->getOrientation().i,
			(float)car->body->getOrientation().j,
			(float)car->body->getOrientation().k);

		glm::quat steerQ = glm::angleAxis(steerAngleRad, glm::vec3(0.0f, 1.0f, 0.0f));

		for (int i = 0; i < 4; ++i)
		{
			car->wheels[i].coll->calculateInternals();
			Matrix4 wheelTransform = car->wheels[i].coll->getTransform();
			Vector3 wheelPos = wheelTransform.getAxisVector(3);
			car->wheels[i].mesh->SetPosition(wheelPos);

			if (i < 2)
			{
				glm::quat wheelOrientation = bodyQ * steerQ;
				car->wheels[i].mesh->SetOrientation(wheelOrientation);
			}
			else
			{
				car->wheels[i].mesh->SetOrientation(bodyQ);
			}
		}

		auto updatePart = [&](Cube* mesh, const elevate::Vector3& localOffset)
			{
				if (!mesh) return;
				elevate::Vector3 worldPos = car->body->getPointInWorldSpace(localOffset);
				mesh->SetPosition(worldPos);
				mesh->SetOrientation(bodyQ);
			};

		updatePart(car->roofMesh, car->roofOffset);
		updatePart(car->hoodMesh, car->hoodOffset);
		updatePart(car->rearMesh, car->rearOffset);
		updatePart(car->frontBumperMesh, car->frontBumperOffset);

		if (showCar)
		{
			Vector3 carPos = car->body->getPosition();
			Matrix4 carTransform = car->body->getTransform();

			Vector3 forward = carTransform.getAxisVector(0) * -1;
			forward.normalize();

			Vector3 worldUp(0, 1, 0);
			Vector3 right = worldUp % forward;
			right.normalize();
			Vector3 camUp = forward % right;
			camUp.normalize();

			float back = 40.0f;
			float height = 4.5f;

			Vector3 camPos = carPos + right * -back + worldUp * height;
			Vector3 target = carPos + right * 4.5f;

			camera->Position = glm::vec3(camPos.x, camPos.y, camPos.z);
			camera->Front = glm::normalize(glm::vec3(target.x, target.y, target.z) - camera->Position);
			camera->Up = glm::vec3(camUp.x, camUp.y, camUp.z);
			view = glm::lookAt(camera->Position, camera->Position + camera->Front, camera->Up);
		}
	}

	if (fpsSandboxDemo)
	{

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

		for (Ragdoll* ragdoll : ragdolls)
		{
			for (Bone* bone = ragdoll->bones; bone < ragdoll->bones + 12; bone++)
			{
				bone->body->clearAccumulator();
				bone->body->calculateDerivedData();
				if (bone != &ragdoll->bones[7])
					bone->body->integrate(deltaTime);
				bone->calculateInternals();
			}
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

		for (int i = 0; i < crates.size(); i++)
		{
			crates[i]->body->clearAccumulator();
			crates[i]->body->calculateDerivedData();
			rbGravity->updateForce(crates[i]->body, deltaTime);
			crates[i]->body->integrate(deltaTime);
			crates[i]->shape->calculateInternals();
		}

		for (int i = 0; i < bricks.size(); i++)
		{
			bricks[i]->body->clearAccumulator();
			bricks[i]->body->calculateDerivedData();
			rbGravity->updateForce(bricks[i]->body, deltaTime);
			bricks[i]->body->integrate(deltaTime);
			bricks[i]->shape->calculateInternals();
		}

		for (int i = 0; i < dominoes.size(); i++)
		{
			dominoes[i]->body->clearAccumulator();
			dominoes[i]->body->calculateDerivedData();
			rbGravity->updateForce(dominoes[i]->body, deltaTime);
			dominoes[i]->body->integrate(deltaTime);
			dominoes[i]->shape->calculateInternals();
		}

		for (int i = 0; i < runTimeBoxes.size(); i++)
		{
			if (!runTimeBoxes[i]) continue;

			runTimeBoxes[i]->body->clearAccumulator();
			runTimeBoxes[i]->body->calculateDerivedData();
			rbGravity->updateForce(runTimeBoxes[i]->body, deltaTime);
			runTimeBoxes[i]->body->integrate(deltaTime);
			runTimeBoxes[i]->shape->calculateInternals();
		}

		for (int i = 0; i < runTimeSpheres.size(); i++)
		{
			if (!runTimeSpheres[i]) continue;

			runTimeSpheres[i]->body->clearAccumulator();
			runTimeSpheres[i]->body->calculateDerivedData();
			rbGravity->updateForce(runTimeSpheres[i]->body, deltaTime);
			runTimeSpheres[i]->body->integrate(deltaTime);
			runTimeSpheres[i]->shape->calculateInternals();
		}

		crate->body->clearAccumulator();
		crate->body->calculateDerivedData();
		rbGravity->updateForce(crate->body, deltaTime);
		crate->body->integrate(deltaTime);
		crate->shape->calculateInternals();

		for (int i = 0; i < crateStacks.size(); i++)
		{
			for (int j = 0; j < crateStacks[i].size(); j++)
			{
				crateStacks[i][j]->body->clearAccumulator();
				crateStacks[i][j]->body->calculateDerivedData();
				rbGravity->updateForce(crateStacks[i][j]->body, deltaTime);
				crateStacks[i][j]->body->integrate(deltaTime);
				crateStacks[i][j]->shape->calculateInternals();
			}
		}

		for (int i = 0; i < runtimeWalls.size(); i++)
		{
			for (int j = 0; j < runtimeWalls[i].size(); j++)
			{
				runtimeWalls[i][j]->body->clearAccumulator();
				runtimeWalls[i][j]->body->calculateDerivedData();
				rbGravity->updateForce(runtimeWalls[i][j]->body, deltaTime);
				runtimeWalls[i][j]->body->integrate(deltaTime);
				runtimeWalls[i][j]->shape->calculateInternals();
			}
		}

		for (int i = 0; i < runtimeDominoLines.size(); i++)
		{
			for (int j = 0; j < runtimeDominoLines[i].size(); j++)
			{
				runtimeDominoLines[i][j]->body->clearAccumulator();
				runtimeDominoLines[i][j]->body->calculateDerivedData();
				rbGravity->updateForce(runtimeDominoLines[i][j]->body, deltaTime);
				runtimeDominoLines[i][j]->body->integrate(deltaTime);
				runtimeDominoLines[i][j]->shape->calculateInternals();
			}
		}

		for (int i = 0; i < runtimeFractureBlocks.size(); i++)
		{
			if (!runtimeFractureBlocks[i].exists) continue;
			runtimeFractureBlocks[i].body->clearAccumulator();
			runtimeFractureBlocks[i].body->calculateDerivedData();
			rbGravity->updateForce(runtimeFractureBlocks[i].body, deltaTime);
			runtimeFractureBlocks[i].body->integrate(deltaTime);
			runtimeFractureBlocks[i].calculateInternals();
		}
		if (!showCar)
		{

			generateContacts();

			resolver.resolveContacts(cData.contactArray, cData.contactCount, deltaTime);
		}

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

		for (int i = 0; i < bricks.size(); i++)
		{
			bricks[i]->shape->calculateInternals();
			elevate::Matrix4 t = bricks[i]->body->getTransform();
			elevate::Vector3 p = t.getAxisVector(3);
			bricks[i]->mesh->SetPosition(p);
			bricks[i]->mesh->SetOrientation(glm::quat(
				(float)bricks[i]->body->getOrientation().r,
				(float)bricks[i]->body->getOrientation().i,
				(float)bricks[i]->body->getOrientation().j,
				(float)bricks[i]->body->getOrientation().k));
		}

		for (Ragdoll* ragdoll : ragdolls)
		{

			for (Bone* bone = ragdoll->bones; bone < ragdoll->bones + 12; bone++)
			{
				bone->body->calculateDerivedData();
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

		for (int i = 0; i < dominoes.size(); i++)
		{
			dominoes[i]->body->calculateDerivedData();
			dominoes[i]->shape->calculateInternals();
			elevate::Matrix4 t = dominoes[i]->body->getTransform();
			elevate::Vector3 p = t.getAxisVector(3);
			dominoes[i]->mesh->SetPosition(p);
			dominoes[i]->mesh->SetOrientation(glm::quat(
				(float)dominoes[i]->body->getOrientation().r,
				(float)dominoes[i]->body->getOrientation().i,
				(float)dominoes[i]->body->getOrientation().j,
				(float)dominoes[i]->body->getOrientation().k));
		}

		for (int i = 0; i < runTimeBoxes.size(); i++)
		{
			runTimeBoxes[i]->body->calculateDerivedData();
			runTimeBoxes[i]->shape->calculateInternals();
			elevate::Matrix4 t = runTimeBoxes[i]->body->getTransform();
			elevate::Vector3 p = t.getAxisVector(3);
			runTimeBoxes[i]->mesh->SetPosition(p);
			runTimeBoxes[i]->mesh->SetOrientation(glm::quat(
				(float)runTimeBoxes[i]->body->getOrientation().r,
				(float)runTimeBoxes[i]->body->getOrientation().i,
				(float)runTimeBoxes[i]->body->getOrientation().j,
				(float)runTimeBoxes[i]->body->getOrientation().k));
		}

		for (int i = 0; i < runTimeSpheres.size(); i++)
		{
			runTimeSpheres[i]->body->calculateDerivedData();
			runTimeSpheres[i]->shape->calculateInternals();
			elevate::Matrix4 t = runTimeSpheres[i]->body->getTransform();
			elevate::Vector3 p = t.getAxisVector(3);
			runTimeSpheres[i]->mesh->SetPosition(p);
			runTimeSpheres[i]->mesh->SetOrientation(glm::quat(
				(float)runTimeSpheres[i]->body->getOrientation().r,
				(float)runTimeSpheres[i]->body->getOrientation().i,
				(float)runTimeSpheres[i]->body->getOrientation().j,
				(float)runTimeSpheres[i]->body->getOrientation().k));
		}

		for (int i = 0; i < crateStacks.size(); i++)
		{
			for (int j = 0; j < crateStacks[i].size(); j++)
			{
				crateStacks[i][j]->body->calculateDerivedData();
				crateStacks[i][j]->shape->calculateInternals();
				elevate::Matrix4 t = crateStacks[i][j]->body->getTransform();
				elevate::Vector3 p = t.getAxisVector(3);
				crateStacks[i][j]->mesh->SetPosition(p);
				crateStacks[i][j]->mesh->SetOrientation(glm::quat(
					(float)crateStacks[i][j]->body->getOrientation().r,
					(float)crateStacks[i][j]->body->getOrientation().i,
					(float)crateStacks[i][j]->body->getOrientation().j,
					(float)crateStacks[i][j]->body->getOrientation().k));
			}
		}

		for (int i = 0; i < runtimeWalls.size(); i++)
		{
			for (int j = 0; j < runtimeWalls[i].size(); j++)
			{
				runtimeWalls[i][j]->body->calculateDerivedData();
				runtimeWalls[i][j]->shape->calculateInternals();
				elevate::Matrix4 t = runtimeWalls[i][j]->body->getTransform();
				elevate::Vector3 p = t.getAxisVector(3);
				runtimeWalls[i][j]->mesh->SetPosition(p);
				runtimeWalls[i][j]->mesh->SetOrientation(glm::quat(
					(float)runtimeWalls[i][j]->body->getOrientation().r,
					(float)runtimeWalls[i][j]->body->getOrientation().i,
					(float)runtimeWalls[i][j]->body->getOrientation().j,
					(float)runtimeWalls[i][j]->body->getOrientation().k));
			}
		}

		for (int i = 0; i < runtimeDominoLines.size(); i++)
		{
			for (int j = 0; j < runtimeDominoLines[i].size(); j++)
			{
				runtimeDominoLines[i][j]->body->calculateDerivedData();
				runtimeDominoLines[i][j]->shape->calculateInternals();
				elevate::Matrix4 t = runtimeDominoLines[i][j]->body->getTransform();
				elevate::Vector3 p = t.getAxisVector(3);
				runtimeDominoLines[i][j]->mesh->SetPosition(p);
				runtimeDominoLines[i][j]->mesh->SetOrientation(glm::quat(
					(float)runtimeDominoLines[i][j]->body->getOrientation().r,
					(float)runtimeDominoLines[i][j]->body->getOrientation().i,
					(float)runtimeDominoLines[i][j]->body->getOrientation().j,
					(float)runtimeDominoLines[i][j]->body->getOrientation().k));
			}
		}

		for (int i = 0; i < runtimeFractureBlocks.size(); i++)
		{
			if (!runtimeFractureBlocks[i].exists) continue;
			runtimeFractureBlocks[i].body->calculateDerivedData();
			runtimeFractureBlocks[i].calculateInternals();
			elevate::Matrix4 t = runtimeFractureBlocks[i].body->getTransform();
			elevate::Vector3 p = t.getAxisVector(3);
			runtimeFractureBlocks[i].visual->SetPosition(p);
			runtimeFractureBlocks[i].visual->SetOrientation(glm::quat(
				(float)runtimeFractureBlocks[i].body->getOrientation().r,
				(float)runtimeFractureBlocks[i].body->getOrientation().i,
				(float)runtimeFractureBlocks[i].body->getOrientation().j,
				(float)runtimeFractureBlocks[i].body->getOrientation().k));
		}

		crate->shape->calculateInternals();
		elevate::Matrix4 t = crate->body->getTransform();
		elevate::Vector3 p = t.getAxisVector(3);
		crate->mesh->SetPosition(p);
		crate->mesh->SetOrientation(glm::quat(
			(float)crate->body->getOrientation().r,
			(float)crate->body->getOrientation().i,
			(float)crate->body->getOrientation().j,
			(float)crate->body->getOrientation().k));

		if (hit)
		{
			for (int i = 0; i < 8; i++)
			{
				Cube* c = new Cube(elevate::Vector3(3.0f, 5.0f, 3.0f), elevate::Vector3(1.0f, 1.0f, 1.0f), &cubeShader, this);
				c->LoadMesh();
				//c->LoadTextureFromFile("C:/dev/ElevateEngine/src/Assets/Textures/Block/TCom_Concrete_WallAbstract_512_albedo.png");
				c->SetTexture(blockTexture);
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

		}

	}

	physicsTime = (float)glfwGetTime() - physicsTime;

	CalculatePerformanceMetrics(deltaTime);
	return;

}

void GameManager::generateContacts()
{
	hit = false;

	elevate::CollisionPlane plane;
	plane.direction = elevate::Vector3(0, 1, 0);
	plane.offset = groundPlaneOffset;

	cData.reset(2056);

	for (int i = 0; i < aircraftParts.size(); i++)
	{
		if (!cData.hasMoreContacts()) return;
		elevate::CollisionDetector::boxAndHalfSpace(*aircraftParts[i].coll, plane, &cData);

		if (!cData.hasMoreContacts()) return;
		elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(floor->shape), *aircraftParts[i].coll, &cData);
	}

	if (showCar)
	{
		for (int i = 0; i < 4; i++)
		{
			if (!cData.hasMoreContacts()) return;
			///		elevate::CollisionDetector::sphereAndHalfSpace(*car->wheels[i].coll, plane, &cData);
			if (!cData.hasMoreContacts()) return;
			elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(floor->shape), *car->wheels[i].coll, &cData);
		}

		if (!cData.hasMoreContacts()) return;
		//elevate::CollisionDetector::boxAndHalfSpace(*car->chassis, plane, &cData);
		if (!cData.hasMoreContacts()) return;
		elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(floor->shape), *car->chassis, &cData);
	}

	if (fpsSandboxDemo)
	{
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

			for (int i = 0; i < dominoes.size(); i++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(dominoes[i]->shape), *r.coll, &cData);

				for (int j = i + 1; j < dominoes.size(); j++)
				{
					if (!cData.hasMoreContacts()) return;
					elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(dominoes[i]->shape), *static_cast<CollisionBox*>(dominoes[j]->shape), &cData);
				}
			}

			for (int i = 0; i < runTimeBoxes.size(); i++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(runTimeBoxes[i]->shape), *r.coll, &cData);
			}
			for (int i = 0; i < runtimeFractureBlocks.size(); i++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(&runtimeFractureBlocks[i]), *r.coll, &cData);
			}

			for (int i = 0; i < crateStacks.size(); i++)
			{
				for (int j = 0; j < crateStacks[i].size(); j++)
				{
					if (!cData.hasMoreContacts()) return;
					elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(crateStacks[i][j]->shape), *r.coll, &cData);
				}
			}

			for (int i = 0; i < runtimeWalls.size(); i++)
			{
				for (int j = 0; j < runtimeWalls[i].size(); j++)
				{
					if (!cData.hasMoreContacts()) return;
					elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(runtimeWalls[i][j]->shape), *r.coll, &cData);
				}
			}

			for (int i = 0; i < runtimeDominoLines.size(); i++)
			{
				for (int j = 0; j < runtimeDominoLines[i].size(); j++)
				{
					if (!cData.hasMoreContacts()) return;
					elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(runtimeDominoLines[i][j]->shape), *r.coll, &cData);
				}
			}

			for (int i = 0; i < runTimeSpheres.size(); i++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::sphereAndSphere(*static_cast<CollisionSphere*>(runTimeSpheres[i]->shape), *r.coll, &cData);
			}

			for (int i = 0; i < crates.size(); i++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(crates[i]->shape), *r.coll, &cData);
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(crates[i]->shape), *static_cast<CollisionBox*>(floor->shape), &cData);
			}

			for (Ragdoll* ragdoll : ragdolls)
			{
				for (Bone* bone = ragdoll->bones; bone < ragdoll->bones + 12; bone++)
				{

					if (!cData.hasMoreContacts()) return;
					elevate::CollisionSphere boneSphere = bone->getCollisionSphere();

					if (!cData.hasMoreContacts()) return;
					elevate::CollisionDetector::sphereAndSphere(boneSphere, *r.coll, &cData);
				};
			}

			for (int i = 0; i < numStackCubes; ++i)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndSphere(*cStackBoxes[i], *r.coll, &cData);
			}

			for (int i = 0; i < bricks.size(); i++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(bricks[i]->shape), *r.coll, &cData);
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

			if (!cData.hasMoreContacts()) return;
			elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(crate->shape), *r.coll, &cData);

		}

		if (!cData.hasMoreContacts()) return;
		for (int i = 0; i < runTimeBoxes.size(); i++)
		{
			for (int j = i + 1; j < runTimeBoxes.size(); j++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(runTimeBoxes[i]->shape), *static_cast<CollisionBox*>(runTimeBoxes[j]->shape), &cData);
			}
			if (!cData.hasMoreContacts()) return;
			elevate::CollisionDetector::boxAndHalfSpace(*static_cast<CollisionBox*>(runTimeBoxes[i]->shape), plane, &cData);
		}

		for (int i = 0; i < runTimeSpheres.size(); i++)
		{
			if (!cData.hasMoreContacts()) return;
			elevate::CollisionDetector::sphereAndHalfSpace(*static_cast<CollisionSphere*>(runTimeSpheres[i]->shape), plane, &cData);
			for (int j = i + 1; j < runTimeSpheres.size(); j++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::sphereAndSphere(*static_cast<CollisionSphere*>(runTimeSpheres[i]->shape), *static_cast<CollisionSphere*>(runTimeSpheres[j]->shape), &cData);
			}
		}

		if (!cData.hasMoreContacts()) return;
		for (int i = 0; i < dominoes.size(); i++)
		{
			for (int j = i + 1; j < dominoes.size(); j++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(dominoes[i]->shape), *static_cast<CollisionBox*>(dominoes[j]->shape), &cData);
			}
		}


		elevate::CollisionDetector::boxAndHalfSpace(*static_cast<CollisionBox*>(crate->shape), plane, &cData);

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
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*cStackBoxes[i], *cStackBoxes[j], &cData);
			}
		}

		for (int i = 1; i < numStackCubes; ++i)
		{
			elevate::Matrix4 transform, otherTransform;
			elevate::Vector3 position, otherPosition;
			for (Block* block = blocks; block < blocks + 9; block++)
			{
				if (!block->exists) continue;

				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*block, *cStackBoxes[i], &cData);
			}

			for (PhysicsObject* brick : bricks)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(brick->shape), *cStackBoxes[i], &cData);
			}
		}

		for (int i = 0; i < crates.size(); i++)
		{
			if (!cData.hasMoreContacts()) return;
			elevate::CollisionDetector::boxAndHalfSpace(*static_cast<CollisionBox*>(crates[i]->shape), plane, &cData);

			for (int j = i + 1; j < crates.size(); j++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(crates[i]->shape), *static_cast<CollisionBox*>(crates[j]->shape), &cData);
			}

			for (PhysicsObject* brick : bricks)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(brick->shape), *static_cast<CollisionBox*>(crates[i]->shape), &cData);
			}
		}

		elevate::Matrix4 transform, otherTransform;
		elevate::Vector3 position, otherPosition;

		for (Ragdoll* ragdoll : ragdolls)
		{
			for (Bone* bone = ragdoll->bones; bone < ragdoll->bones + 12; bone++)
			{
				elevate::CollisionSphere boneSphere = bone->getCollisionSphere();
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::sphereAndHalfSpace(boneSphere, plane, &cData);


				elevate::CollisionDetector::boxAndSphere(*static_cast<CollisionBox*>(crate->shape), boneSphere, &cData);



				// Check for collisions with each other box
				for (Bone* other = bone + 1; other < ragdoll->bones + 12; other++)
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
			for (elevate::Joint* joint = ragdoll->joints; joint < ragdoll->joints + 11; joint++)
			{
				if (!cData.hasMoreContacts()) return;
				unsigned added = joint->addContact(cData.contacts, cData.contactsLeft);
				cData.addContacts(added);
			}
		}
		for (int i = 0; i < numStackCubes; ++i)
		{
			for (int j = i + 1; j < numStackCubes; ++j)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*cStackBoxes[i], *cStackBoxes[j], &cData);
			}
		}

		for (int i = 0; i < bricks.size(); i++)
		{
			if (!cData.hasMoreContacts()) return;
			elevate::CollisionDetector::boxAndHalfSpace(*static_cast<CollisionBox*>(bricks[i]->shape), plane, &cData);
		}

		for (int i = 0; i < bricks.size(); i++)
		{
			for (int j = i + 1; j < bricks.size(); j++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(bricks[i]->shape), *static_cast<CollisionBox*>(bricks[j]->shape), &cData);
			}


		}

		for (Block* block = blocks; block < blocks + 9; block++)
		{
			if (!block->exists) continue;

			if (!cData.hasMoreContacts()) return;
			elevate::CollisionDetector::boxAndHalfSpace(*block, plane, &cData);

			// Check for collisions with each other box
			for (Block* other = block + 1; other < blocks + 9; other++)
			{
				if (!other->exists) continue;

				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*block, *other, &cData);
			}

			elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(crate->shape), *block, &cData);

		}

		for (int i = 0; i < runtimeFractureBlocks.size(); i++)
		{
			if (!cData.hasMoreContacts()) return;
			elevate::CollisionDetector::boxAndHalfSpace(*static_cast<CollisionBox*>(&runtimeFractureBlocks[i]), plane, &cData);
			for (int j = i + 1; j < runtimeFractureBlocks.size(); j++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(&runtimeFractureBlocks[i]), *static_cast<CollisionBox*>(&runtimeFractureBlocks[j]), &cData);
			}
		}

		for (int i = 0; i < crateStacks.size(); i++)
		{
			for (int j = 0; j < crateStacks[i].size(); j++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndHalfSpace(*static_cast<CollisionBox*>(crateStacks[i][j]->shape), plane, &cData);
				for (int k = j + 1; k < crateStacks[i].size(); k++)
				{
					if (!cData.hasMoreContacts()) return;
					elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(crateStacks[i][j]->shape), *static_cast<CollisionBox*>(crateStacks[i][k]->shape), &cData);
				}
			}
		}

		for (int i = 0; i < runtimeWalls.size(); i++)
		{
			for (int j = 0; j < runtimeWalls[i].size(); j++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndHalfSpace(*static_cast<CollisionBox*>(runtimeWalls[i][j]->shape), plane, &cData);
				for (int k = j + 1; k < runtimeWalls[i].size(); k++)
				{
					if (!cData.hasMoreContacts()) return;
					elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(runtimeWalls[i][j]->shape), *static_cast<CollisionBox*>(runtimeWalls[i][k]->shape), &cData);
				}
			}
		}

		for (int i = 0; i < runtimeDominoLines.size(); i++)
		{
			for (int j = 0; j < runtimeDominoLines[i].size(); j++)
			{
				if (!cData.hasMoreContacts()) return;
				elevate::CollisionDetector::boxAndHalfSpace(*static_cast<CollisionBox*>(runtimeDominoLines[i][j]->shape), plane, &cData);
				for (int k = j + 1; k < runtimeDominoLines[i].size(); k++)
				{
					if (!cData.hasMoreContacts()) return;
					elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(runtimeDominoLines[i][j]->shape), *static_cast<CollisionBox*>(runtimeDominoLines[i][k]->shape), &cData);
				}
			}
		}

		for (int i = 0; i < dominoes.size(); i++)
		{
			elevate::CollisionDetector::boxAndHalfSpace(*static_cast<CollisionBox*>(dominoes[i]->shape), plane, &cData);
			elevate::CollisionDetector::boxAndBox(*static_cast<CollisionBox*>(dominoes[i]->shape), *static_cast<CollisionBox*>(floor->shape), &cData);
		}

		buildContactDebugLines();

	}
}
void GameManager::ResetPlane()
{

	rbRegistry.remove(&aircraft, &left_wing);
	rbRegistry.remove(&aircraft, &right_wing);
	rbRegistry.remove(&aircraft, &rudder);
	rbRegistry.remove(&aircraft, &tail);

	for (AircraftParts& part : aircraftParts)
	{
		delete part.mesh;
	}
	aircraftParts.clear();

	AircraftParts part1{};
	part1.offset = elevate::Vector3(-0.5f, 0.0f, 0.0f);
	part1.mesh = new Cube(elevate::Vector3(0, 0, 0), elevate::Vector3(2.0f, 0.8f, 1.0f), &ammoShader, this);
	part1.mesh->LoadMesh();
	part1.mesh->SetColor(glm::vec3(0.8f, 0.8f, 0.2f));
	part1.coll = new elevate::CollisionBox();
	part1.coll->halfSize = elevate::Vector3(1.0f, 0.4f, 0.5f);
	aircraftParts.push_back(part1);

	AircraftParts part2{};
	part2.offset = elevate::Vector3(1.0f, 0.15f, 0);
	part2.mesh = new Cube(elevate::Vector3(0, 0, 0), elevate::Vector3(2.75f, 0.5f, 0.5), &ammoShader, this);
	part2.mesh->LoadMesh();
	part2.mesh->SetColor(glm::vec3(0.8f, 0.8f, 0.2f));
	part2.coll = new elevate::CollisionBox();
	part2.coll->halfSize = elevate::Vector3(1.375f, 0.25f, 0.25f);
	aircraftParts.push_back(part2);

	AircraftParts part3{};
	part3.offset = elevate::Vector3(0, 0.3f, 0);
	part3.mesh = new Cube(elevate::Vector3(0, 0, 0), elevate::Vector3(0.8f, 0.1f, 6.0f), &ammoShader, this);
	part3.mesh->LoadMesh();
	part3.mesh->SetColor(glm::vec3(0.8f, 0.8f, 0.2f));
	part3.coll = new elevate::CollisionBox();
	part3.coll->halfSize = elevate::Vector3(0.4f, 0.05f, 3.0f);
	aircraftParts.push_back(part3);

	AircraftParts part4{};
	part4.offset = elevate::Vector3(2.0f, 0.775f, 0);
	part4.mesh = new Cube(elevate::Vector3(0, 0, 0), elevate::Vector3(0.75f, 1.15f, 0.1f), &ammoShader, this);
	part4.mesh->LoadMesh();
	part4.mesh->SetColor(glm::vec3(0.8f, 0.8f, 0.2f));
	part4.coll = new elevate::CollisionBox();
	part4.coll->halfSize = elevate::Vector3(0.375f, 0.575f, 0.05f);
	aircraftParts.push_back(part4);

	AircraftParts part5{};
	part5.offset = elevate::Vector3(1.9f, 0, 0);
	part5.mesh = new Cube(elevate::Vector3(0, 0, 0), elevate::Vector3(0.85f, 0.1f, 2.0f), &ammoShader, this);
	part5.mesh->LoadMesh();
	part5.mesh->SetColor(glm::vec3(0.8f, 0.8f, 0.2f));
	part5.coll = new elevate::CollisionBox();
	part5.coll->halfSize = elevate::Vector3(0.425f, 0.05f, 1.0f);
	aircraftParts.push_back(part5);

	aircraft.setPosition(elevate::Vector3(0, 5, 0));
	aircraft.setOrientation(elevate::Quaternion(1, 0, 0, 0));

	aircraft.setVelocity(elevate::Vector3(0, 0, 0));
	aircraft.setRotation(elevate::Vector3(0, 0, 0));
	rudder_control = 0.0f;
	left_wing_control = 0.0f;
	right_wing_control = 0.0f;

	right_wing.tensor = elevate::Matrix3(0, 0, 0, -1, -0.5f, 0, 0, 0, 0);

	right_wing.minTensor = elevate::Matrix3(0, 0, 0, -0.995f, -0.5f, 0, 0, 0, 0);
	right_wing.maxTensor = elevate::Matrix3(0, 0, 0, -1.005f, -0.5f, 0, 0, 0, 0);
	right_wing.position = elevate::Vector3(-1.0f, 0.0f, 2.0f);
	right_wing.windspeed = &windspeed;

	left_wing.tensor = elevate::Matrix3(0, 0, 0, -1, -0.5f, 0, 0, 0, 0);
	left_wing.minTensor = elevate::Matrix3(0, 0, 0, -0.995f, -0.5f, 0, 0, 0, 0);
	left_wing.maxTensor = elevate::Matrix3(0, 0, 0, -1.005f, -0.5f, 0, 0, 0, 0);
	left_wing.position = elevate::Vector3(-1.0f, 0.0f, -2.0f);
	left_wing.windspeed = &windspeed;

	rudder.tensor = elevate::Matrix3(0, 0, 0, 0, 0, 0, 0, 0, 0);
	rudder.minTensor = elevate::Matrix3(0, 0, 0, 0, 0, 0, 0.01f, 0, 0);
	rudder.maxTensor = elevate::Matrix3(0, 0, 0, 0, 0, 0, -0.01f, 0, 0);
	rudder.position = elevate::Vector3(2.0f, 0.5f, 0);
	rudder.windspeed = &windspeed;

	tail.tensor = elevate::Matrix3(0, 0, 0, -1, -0.5f, 0, 0, 0, -0.1f);
	tail.position = elevate::Vector3(2.0f, 0, 0);
	tail.windspeed = &windspeed;

	left_wing_control = 0;
	right_wing_control = 0;
	rudder_control = 0;

	windspeed = elevate::Vector3(0, 0, 0);


	// Set up the aircraft rigid body.
	aircraft.clearAccumulator();
	aircraft.setMass(2.5f);
	elevate::Matrix3 it;
	it.setBlockInertiaTensor(elevate::Vector3(2, 1, 1), 1);
	aircraft.setInertiaTensor(it);

	aircraft.setDamping(0.8f, 0.8f);

	aircraft.setAcceleration(elevate::Vector3::GRAVITY);
	aircraft.calculateDerivedData();

	aircraft.setAwake(true);
	aircraft.setCanSleep(false);

	for (auto& part : aircraftParts)
	{
		part.coll->body = &aircraft;
		part.coll->calculateInternals();
		part.mesh->SetPosition(elevate::Vector3(
			(float)(aircraft.getPosition().x + part.offset.x),
			(float)(aircraft.getPosition().y + part.offset.y),
			(float)(aircraft.getPosition().z + part.offset.z)
		));
		part.mesh->SetOrientation(glm::quat(
			(float)aircraft.getOrientation().r,
			(float)aircraft.getOrientation().i,
			(float)aircraft.getOrientation().j,
			(float)aircraft.getOrientation().k));
	}

	rbRegistry.add(&aircraft, &left_wing);
	rbRegistry.add(&aircraft, &right_wing);
	rbRegistry.add(&aircraft, &rudder);
	rbRegistry.add(&aircraft, &tail);


}
void GameManager::render()
{
	DebugDraw::Clear();

	if (!showCar && !showPlane)
	{
		DebugDrawCollisionBox(*static_cast<CollisionBox*>(crate->shape), glm::vec3(1.0f, 0.0f, 0.0f)); // red
	}
	DebugDrawCollisionBox(*static_cast<CollisionBox*>(floor->shape), glm::vec3(1.0f, 0.0f, 0.0f)); // red

	if (showCar)
	{

		DebugDrawCollisionBox(*static_cast<CollisionBox*>(car->chassis), glm::vec3(1.0f, 0.0f, 0.0f)); // red

		for (auto& wheel : car->wheels)
		{
			DebugDrawCollisionSphere(*wheel.coll, glm::vec3(0.0f, 1.0f, 0.0f)); // green
		}
	}

	renderer->drawCubemap(cubemap);

	for (auto obj : gameObjects) {
		renderer->draw(obj, view, projection);
	}

	if (showPlane)
	{
		for (AircraftParts& part : aircraftParts)
		{
			renderer->draw(part.mesh, view, projection);
		}
	}
	if (showCar)
	{
		for (CarVisuals part : carParts)
		{
			renderer->draw(part.mesh, view, projection);
		}
	}
	//drawDebugLines();

	glm::mat4 viewProjMatrix = projection * view;
	RenderDebugLines(lineShader.getProgramID(), viewProjMatrix);
}

