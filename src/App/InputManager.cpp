#include "imgui/imgui.h"
#include "imgui/backend/imgui_impl_glfw.h"
#include "imgui/backend/imgui_impl_opengl3.h"

#include "App/InputManager.h"
#include "App/GameManager.h"

void InputManager::handleMouseMovement(double xPosIn, double yPosIn)
{
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

	if (!controlCamera)
		return;

    float xPos = static_cast<float>(xPosIn);
    float yPos = static_cast<float>(yPosIn);

    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera->ProcessMouseMovement(xOffset, yOffset);
}

void InputManager::handleMouseScroll(double xOffset, double yOffset)
{
    camera->ProcessMouseScroll(static_cast<float>(yOffset));
}

void InputManager::processInput(GLFWwindow* window, float deltaTime)
{
    bool spaceKeyCurrentlyPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (spaceKeyCurrentlyPressed && !spaceKeyPressed)
    {
        controlCamera = !controlCamera;

        if (!controlCamera)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    spaceKeyPressed = spaceKeyCurrentlyPressed;


    bool tabKeyCurrentlyPressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;


    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (controlCamera && camera->Mode == FLY)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera->ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera->ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera->ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera->ProcessKeyboard(RIGHT, deltaTime);
    }

    bool leftClickCurrentlyPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;

    if (leftClickCurrentlyPressed && !leftClickPressed)
    {
		mGameManager->fireRound();
    }

    leftClickPressed = leftClickCurrentlyPressed;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		mGameManager->setCurrentShotType(ShotType::PISTOL);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        mGameManager->setCurrentShotType(ShotType::ARTILLERY);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        mGameManager->setCurrentShotType(ShotType::FIREBALL);
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        mGameManager->setCurrentShotType(ShotType::LASER);
}

void InputManager::setContext(Camera* cam, GameManager* gameMgr, unsigned int width, unsigned int height)
{
    camera = cam;
	mGameManager = gameMgr;

    lastX = width / 2.0f;
    lastY = height / 2.0f;
}

