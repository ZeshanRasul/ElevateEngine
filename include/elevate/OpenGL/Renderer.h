#pragma once
#include "RenderData.h"
#include "App/Camera.h"
#include "GameObjects/GameObject.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Cubemap.h"

#include <GLFW/glfw3.h>

class Renderer {
public:
	Renderer(GLFWwindow* window);

	bool init(unsigned int width, unsigned int height);
	
	void setScene(glm::mat4 viewMat, glm::mat4 proj, glm::mat4 cubemapView, DirLight light);
	void draw(GameObject* gameObj, glm::mat4 viewMat, glm::mat4 proj);
	void drawCubemap(Cubemap* cubemap);
	void clear();

	void cleanup();

private:
	RenderData mRenderData{};

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 cubemapView = glm::mat4(1.0f);
	
	DirLight sun;
};