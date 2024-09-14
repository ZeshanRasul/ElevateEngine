#include "App/GameManager.h"

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

    /*
    pistolShader.loadShaders("C:/dev/NPC_RL_Prototype/NPC_RL_Prototype/src/Shaders/vertex_gpu_dquat.glsl", "C:/dev/NPC_RL_Prototype/NPC_RL_Prototype/src/Shaders/fragment_gpu_dquat.glsl");
    artilleryShader.loadShaders("C:/dev/NPC_RL_Prototype/NPC_RL_Prototype/src/Shaders/vertex_gpu_dquat.glsl", "C:/dev/NPC_RL_Prototype/NPC_RL_Prototype/src/Shaders/fragment_gpu_dquat.glsl");
    fireballShader.loadShaders("C:/dev/NPC_RL_Prototype/NPC_RL_Prototype/src/Shaders/gridVertex.glsl", "C:/dev/NPC_RL_Prototype/NPC_RL_Prototype/src/Shaders/gridFragment.glsl");
    laserShader.loadShaders("C:/dev/NPC_RL_Prototype/NPC_RL_Prototype/src/Shaders/crosshair_vert.glsl", "C:/dev/NPC_RL_Prototype/NPC_RL_Prototype/src/Shaders/crosshair_frag.glsl");
    */
    camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));

    inputManager->setContext(camera, width, height);

	// TODO: Create gameobjects and add to gameObjects vector
    // gameObjects.push_back(player);
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

void GameManager::update(float deltaTime)
{
    RemoveDestroyedGameObjects();
    inputManager->processInput(window->getWindow(), deltaTime);

    // TODO: Integrate Physics 
}

void GameManager::render()
{
    for (auto obj : gameObjects) {
        renderer->draw(obj, view, projection);
    }
}