#pragma once
#include "GameObjects/GameObject.h"

class Plane : public GameObject {
public:
	Plane(elevate::Vector3 pos, elevate::Vector3 scale, Shader* shdr, GameManager* gameMgr, float yaw = 0.0f)
		: GameObject(pos, scale, yaw, shdr, gameMgr), mVAO(0), mVBO(0), mEBO(0)
	{
	}

	void LoadMesh();
	void CreateAndUploadVertexBuffer() const;
	void drawObject(glm::mat4 viewMat, glm::mat4 proj) override;

private:

	float vertices[32] =
	{
		-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // 0
		 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 1
		 1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // 2
		-1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f   // 3
	};

	GLuint indices[6]  = {0,1,2, 2,3,0};

	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;

	glm::vec3 color = glm::vec3(1.0f, 0.4f, 1.0f);
};