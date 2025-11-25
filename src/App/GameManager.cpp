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

	camera = new Camera(glm::vec3(0.0f, 5.0f, 40.0f));

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

	if (stackDemo)
	{
		rbWorld = new World(200, 100);
		rbGravity = new Gravity(elevate::Vector3(0.0f, -9.81f * 0.25, 0.0f));

		elevate::Vector3 planePos = { 0.0f, 0.0f, 0.0f };
		elevate::Vector3 planeScale = { 50.0f, 1.0f, 50.0f };

		plane = new Plane(planePos, planeScale, &cubeShader, this);
		plane->LoadMesh();
		plane->SetAngle(0.0f);
		plane->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
		gameObjects.push_back(plane);

		cPlane = new elevate::CollisionPlane();
		cPlane->direction = elevate::Vector3(0.0f, 1.0f, 0.0f);
		cPlane->direction.normalize();
		cPlane->offset = 0.0f; // plane at y = 0

		numStackCubes = 3; // choose 3–5 for testing

		elevate::Vector3 cubeHalfSize(1.0f, 1.0f, 1.0f);
		glm::vec3       renderScale(2.0f, 2.0f, 2.0f);

		for (int i = 0; i < numStackCubes; i++)
		{
			float centerY = 2.1f + i * 2.1f;

			elevate::Vector3 pos(0.0f, centerY, 0.0f);
			elevate::Vector3 scale(renderScale.x, renderScale.y, renderScale.z);

			// Render cube
			Cube* cube = new Cube(pos, scale, &cubeShader, this);
			cube->LoadMesh();
			cube->SetAngle(0.0f);
			cube->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
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

			rbWorld->addBody(body);
			rbGravity->updateForce(body, 0); // or via registry below

			rbWorld->getForceRegistry().add(body, rbGravity);

			stackBodies[i] = body;

			// Collision box
			elevate::CollisionBox* cBox = new elevate::CollisionBox();
			cBox->body = body;
			cBox->halfSize = cubeHalfSize;
			cBox->body->calculateDerivedData();
			cBox->calculateInternals();
			cStackBoxes[i] = cBox;
		}
	}

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
		rbWorld = new World(100, 50);
		rbGravity = new Gravity(elevate::Vector3(0.0f, -9.81f, 0.0f));

		elevate::Vector3 pos = { 30.0f, -30.0f, 0.0f };
		elevate::Vector3 scale = { 5.0f, 5.0f, 5.0f };
		sphere = new Sphere(pos, scale, &ammoShader, this, { 0.0f, 0.8f, 0.3f });
		sphere->GenerateSphere(2.5f, 32, 32);
		sphere->LoadMesh();
		sphereBody = new RigidBody();
		sphereBody->setPosition(elevate::Vector3(30.0f, -30.0f, 0.0f));
		sphereBody->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		sphereBody->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		sphereBody->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));
		sphereBody->setMass(1000.0f);
		sphereBody->setAwake(true);
		Matrix3 tensor;
		real coeff = 0.4f * sphereBody->getMass() * 0.5f * 0.5f;
		tensor.setInertiaTensorCoeffs(coeff, coeff, coeff);
		sphereBody->setInertiaTensor(tensor);
		gameObjects.push_back(sphere);


		pos = { -20.0f, 35.0f, 20.0f };
		scale = { 1.0f, 1.0f, 1.0f };
		sphere2 = new Sphere(pos, scale, &cubeShader, this, { 0.9f, 0.1f, 0.4f });
		sphere2->GenerateSphere(1.5f, 32, 32);
		sphere2->LoadMesh();
		sphereBody2 = new RigidBody();
		sphereBody2->setPosition(elevate::Vector3(-20.0f, 35.0f, 20.0f));
		sphereBody2->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		sphereBody2->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		sphereBody2->setRotation(elevate::Vector3(0.0f, 0.0f, 0.0f));
		sphereBody2->setMass(10.0f);
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

		pos = { -20.0f, 30.0f, -10.0f };
		scale = { 4.0f, 4.0f, 4.0f };
		cube = new Cube(pos, scale, &cubeShader, this);
		cube->SetOrientation(glm::quat(0.0f, 0.2706f, 0.2706f, 0.9239f));
		cube->LoadMesh();
		gameObjects.push_back(cube);
		testBody = new RigidBody();
		testBody->setAwake(true);
		testBody->setPosition(elevate::Vector3(-20.0f, 30.0f, -10.0f));
		testBody->setOrientation(elevate::Quaternion(0.0f, 0.2706f, 0.2706f, 0.9239f));
		testBody->setMass(5.0f);
		tensor;
		coeff = 0.4f * testBody->getMass() * 1.0f * 1.0f;
		tensor.setInertiaTensorCoeffs(coeff, coeff, coeff);
		testBody->setInertiaTensor(tensor);

		Matrix3 boxInertia;
		//boxInertia.setBlockInertiaTensor(elevate::Vector3(2.0f, 2.0f, 2.0f), testBody->getMass());
		//testBody->setInertiaTensor(boxInertia);

		cBox0 = new CollisionBox();
		rbWorld->getForceRegistry().add(testBody, rbGravity);
		rbWorld->addBody(testBody);
		cBox0->body = testBody;
		cBox0->halfSize = elevate::Vector3(2.0f, 2.0f, 2.0f);
		cBox0->body->calculateDerivedData();
		cBox0->calculateInternals();
		cBox0->getTransform();

		pos = { -20.0f, -35.0f, -10.0f };
		scale = { 60.0f, 2.0f, 60.0f };

		cube2 = new Cube(pos, scale, &cubeShader, this);
		cube2->LoadMesh();
		gameObjects.push_back(cube2);
		testBody2 = new RigidBody();
		testBody2->setAwake(true);
		testBody2->setPosition(elevate::Vector3(-20.0f, -35.0f, -10.0f));
		testBody2->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));

		testBody2->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		testBody2->setMass(100000.0f);
		tensor;
		coeff = 0.4f * testBody2->getMass() * 1.0f * 1.0f;
		tensor.setInertiaTensorCoeffs(coeff, coeff, coeff);
		testBody2->setInertiaTensor(tensor);

		//boxInertia.setBlockInertiaTensor(elevate::Vector3(30.0f, 1.0f, 30.0f), testBody2->getMass());
		//testBody2->setInertiaTensor(boxInertia);

		cBox1 = new CollisionBox();
		cBox1->halfSize = elevate::Vector3(30.0f, 1.0f, 30.0f);
		rbWorld->addBody(testBody2);
		cBox1->body = testBody2;
		cBox1->body->calculateDerivedData();
		cBox1->calculateInternals();
		cBox1->getTransform();

		pos = { 0.0f, 0.0f, 0.0f };
		scale = { 50.0f, 1.0f, 50.0f };
		plane = new Plane(pos, scale, &cubeShader, this);
		plane->LoadMesh();
		plane->SetAngle(0.0f);
		plane->SetRotAxis(Vector3(0.0f, 0.0f, 0.0f));
		gameObjects.push_back(plane);
		//planeBody = new RigidBody();
		//planeBody->setAwake(true);
		//planeBody->setPosition(elevate::Vector3(0.0f, -25.0f, 0.0f));
		//planeBody->setOrientation(elevate::Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
		//planeBody->setVelocity(elevate::Vector3(0.0f, 0.0f, 0.0f));
		//planeBody->setMass(100.0f); 
		//
		//rbWorld->addBody(planeBody);
		//Matrix3 planeInertia;
		//planeInertia.setBlockInertiaTensor(elevate::Vector3(25.0f, 1.0f, 25.0f), planeBody->getMass());
		//planeBody->setInertiaTensor(planeInertia);
		cPlane = new CollisionPlane();
		cPlane->direction = elevate::Vector3(0.0f, 1.0f, 0.0f);
		cPlane->direction.normalize();
		cPlane->offset = 1.0f;
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
	//rbRegistry.updateForces(deltaTime);
	generateContacts();
	resolver.resolveContacts(cData.contacts, cData.contactCount, deltaTime);
//	rbWorld->startFrame();
	if (stackDemo)
	{
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
	}


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

		//cBox0->body->calculateDerivedData();
		//cBox1->body->calculateDerivedData();

		cBox0->calculateInternals();
		cBox1->calculateInternals();
		//cBox1->isOverlapping = false;

		Matrix4 box0Mat;
		Matrix4 box1Mat;

		box0Mat = cBox0->body->getTransform();
		elevate::Vector3 box0Pos = box0Mat.getAxisVector(3);
		float rotMat[12] = {
			box0Mat.data[0], box0Mat.data[1], box0Mat.data[2],
			box0Mat.data[4], box0Mat.data[5], box0Mat.data[6],
			box0Mat.data[8], box0Mat.data[9], box0Mat.data[10]
		};
		glm::mat3 rot = glm::make_mat3(rotMat);
		cube->SetOrientation(glm::quat(
			(float)cBox0->body->getOrientation().i,
			(float)cBox0->body->getOrientation().j,
			(float)cBox0->body->getOrientation().k,
			(float)cBox0->body->getOrientation().r));
		//	cube->SetRotationMatrix(glm::mat4(rot));
		cube->SetPosition(box0Pos);
		box1Mat = cBox1->body->getTransform();
		Matrix4 box1MatBackup = box1Mat;
		elevate::Vector3 box1Pos = box1MatBackup.getAxisVector(3);
		cube2->SetPosition(box1Pos);


		float rotMat1[12] = {
			box1Mat.data[0], box1Mat.data[1], box1Mat.data[2],

			box1Mat.data[4], box1Mat.data[5], box1Mat.data[6],
			box1Mat.data[8], box1Mat.data[9], box1Mat.data[10]
		};
		glm::mat3 rot1 = glm::make_mat3(rotMat1);
		cube2->SetOrientation(glm::quat(
			(float)cBox1->body->getOrientation().i,
			(float)cBox1->body->getOrientation().j,
			(float)cBox1->body->getOrientation().k,
			(float)cBox1->body->getOrientation().r));

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
	cData.contacts = contacts;
	cData.reset(256);

	cData.friction = (real)0.6;
	cData.restitution = (real)0.0;
	cData.tolerance = (real)0.1;

	if (stackDemo)
	{
		for (int i = 0; i < numStackCubes; ++i)
		{
			elevate::CollisionDetector::boxAndHalfSpace(*cStackBoxes[i], *cPlane, &cData);
			if (elevate::boxAndHalfSpaceIntersect(*cStackBoxes[i], *cPlane))
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
		}

		buildContactDebugLines();
		return;
	}
	else if (sphereDemo)
	{
		elevate::CollisionDetector::boxAndHalfSpace(*cBox0, *cPlane, &cData);
		elevate::CollisionDetector::sphereAndSphere(*cSphere0, *cSphere1, &cData);
		elevate::CollisionDetector::boxAndBox(*cBox0, *cBox1, &cData);

		if (elevate::boxAndBoxIntersect(*cBox0, *cBox1))
		{
			cBox0->isOverlapping = true;
			cBox1->isOverlapping = true;
		}

		elevate::CollisionDetector::boxAndSphere(*cBox0, *cSphere0, &cData);
		elevate::CollisionDetector::boxAndSphere(*cBox1, *cSphere1, &cData);
		elevate::CollisionDetector::sphereAndHalfSpace(*cSphere1, *cPlane, &cData);

		buildContactDebugLines();
		return;
	}
}

void GameManager::render()
{
	for (auto obj : gameObjects) {
		renderer->draw(obj, view, projection);
	}

	drawDebugLines();

}
