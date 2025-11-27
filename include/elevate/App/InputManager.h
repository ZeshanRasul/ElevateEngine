#pragma once

#include "App/Camera.h"
#include "GLFW/glfw3.h"

class InputManager {
public:
    static void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn) {
        // Retrieve the InputManager instance
        InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));

        if (inputManager) {
            inputManager->handleMouseMovement(xPosIn, yPosIn);
        }
    }

    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
    {
        // Retrieve the InputManager instance
        InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));

        if (inputManager) {
            inputManager->handleMouseScroll(xOffset, yOffset);
        }
    }

    void handleMouseMovement(double xpos, double ypos);
    void handleMouseScroll(double xOffset, double yOffset);
    void processInput(GLFWwindow* window, float deltaTime);


    void setContext(Camera* cam, class GameManager* gameMgr, unsigned int width, unsigned int height);

private:
    Camera* camera;
	class GameManager* mGameManager;
    
    float lastX = 0;
    float lastY = 0;
    bool firstMouse = true;

    bool controlCamera = true;
    bool isTabPressed = false;
    bool tabBeenPressed = false;
    bool spaceKeyPressed = false;
	bool shiftKeyPressed = false;
	bool leftClickPressed = false;
	bool rKeyPressed = false;
	bool pKeyPressed = false;
};
