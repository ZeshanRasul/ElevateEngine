#include "imgui/imgui.h"
#include "imgui/backend/imgui_impl_glfw.h"
#include "imgui/backend/imgui_impl_opengl3.h"

#include "App/InputManager.h"

void InputManager::handleMouseMovement(double xPosIn, double yPosIn)
{
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

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
		//TODO: Add shooting logic
    }

    leftClickPressed = leftClickCurrentlyPressed;
}

void InputManager::setContext(Camera* cam, unsigned int width, unsigned int height)
{
    camera = cam;
   
    lastX = width / 2.0f;
    lastY = height / 2.0f;
}

