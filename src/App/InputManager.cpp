#include "imgui/imgui.h"
#include "imgui/backend/imgui_impl_glfw.h"
#include "imgui/backend/imgui_impl_opengl3.h"

#include "App/InputManager.h"
#include "App/GameManager.h"
#include <algorithm>

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

    camera->ProcessMouseMovement(xOffset, yOffset, false);
}

void InputManager::handleMouseScroll(double xOffset, double yOffset)
{
    camera->ProcessMouseScroll(static_cast<float>(yOffset));
}

void InputManager::processInput(GLFWwindow* window, float deltaTime)
{
    bool pKeyCurrentlyPressed = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;

    if (pKeyCurrentlyPressed && !pKeyPressed)
    {
		mGameManager->togglePause();
    }

    pKeyPressed = pKeyCurrentlyPressed;

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
		mGameManager->fireRound(AmmoType::Pistol);
    }

    leftClickPressed = leftClickCurrentlyPressed;

  //  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		//mGameManager->setCurrentShotType(ShotType::PISTOL);
  //  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
  //      mGameManager->setCurrentShotType(ShotType::ARTILLERY);
  //  if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
  //      mGameManager->setCurrentShotType(ShotType::FIREBALL);
  //  if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
  //      mGameManager->setCurrentShotType(ShotType::LASER);


    //bool leftArrowCurrentlyPressed = glfwGetMouseButton(window, GLFW_KEY_LEFT) == GLFW_PRESS;

    //if (leftArrowCurrentlyPressed && !leftArrowPressed)
    //{
    //    mGameManager->fireRound();
    //}

    //leftClickPressed = leftClickCurrentlyPressed;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        mGameManager->setPushDir(-3.0f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        mGameManager->setPushDir(3.0f);
    
    bool rKeyCurrentlyPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);

    if (rKeyCurrentlyPressed && !rKeyPressed)
		mGameManager->reset();

    rKeyPressed = rKeyCurrentlyPressed;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        mGameManager->targetThrottle = 1.0f;
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        mGameManager->targetThrottle = -1.0f;
	else
		mGameManager->targetThrottle = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        mGameManager->targetSteer = 1.0f;
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        mGameManager->targetSteer = -1.0f;
	else
		mGameManager->targetSteer = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        mGameManager->targetBrake = 1.0f;
    else
		mGameManager->targetBrake = 0.0f;

    float brakeRate = 10.0f;
    mGameManager->car->brake += (mGameManager->targetBrake - mGameManager->car->brake) * brakeRate * deltaTime;
    mGameManager->car->brake = std::clamp(mGameManager->car->brake, 0.0f, 1.0f);

    float steerRate = 6.0f;
    mGameManager->car->steerAngle += (mGameManager->targetSteer - mGameManager->car->steerAngle) * steerRate * deltaTime;

    static bool qPrev = false, ePrev = false, wPrev = false, sPrev = false, dPrev = false, aPrev = false, zPrev = false, hPrev = false, clickPrev = false;

    if (IsKeyPressedOnce(window, GLFW_KEY_Q, qPrev)) mGameManager->OnQPressed();
    if (IsKeyPressedOnce(window, GLFW_KEY_E, ePrev)) mGameManager->OnEPressed();
    if (IsKeyPressedOnce(window, GLFW_KEY_W, wPrev)) mGameManager->OnWPressed();
    if (IsKeyPressedOnce(window, GLFW_KEY_S, sPrev)) mGameManager->OnSPressed();
    if (IsKeyPressedOnce(window, GLFW_KEY_D, dPrev)) mGameManager->OnDPressed();
    if (IsKeyPressedOnce(window, GLFW_KEY_A, aPrev)) mGameManager->OnAPressed();
    if (IsKeyPressedOnce(window, GLFW_KEY_Z, zPrev)) mGameManager->OnZPressed();
    if (IsKeyPressedOnce(window, GLFW_KEY_H, hPrev)) mGameManager->OnHPressed();
    if (IsKeyPressedOnce(window, GLFW_MOUSE_BUTTON_1, clickPrev)) mGameManager->OnLeftClick();

}

void InputManager::setContext(Camera* cam, GameManager* gameMgr, unsigned int width, unsigned int height)
{
    camera = cam;
	mGameManager = gameMgr;

    lastX = width / 2.0f;
    lastY = height / 2.0f;
}

