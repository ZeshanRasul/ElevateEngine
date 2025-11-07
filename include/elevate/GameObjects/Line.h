#pragma once
#include "GameObject.h"

class Line : public GameObject
{
public:
	Line(elevate::Vector3 pos, elevate::Vector3 scale, Shader* shdr, GameManager* gameMgr, glm::vec3 col, float yaw = 0.0f)
		: GameObject(pos, scale, yaw, shdr, gameMgr)
	{
	}

	void LoadMesh();

	void CreateAndUploadVertexBuffer();

	void UpdateVertexBuffer(glm::vec3 rayOrigin, glm::vec3 rayEnd);

	void DrawLine(glm::mat4 viewMat, glm::mat4 proj, glm::vec3 lineColor);
	
	void drawObject(glm::mat4 viewMat, glm::mat4 proj) override;

	void SetLineColor(glm::vec3 col) { lineCol = col; }

private:
	float m_vertices[6] = {
		0.0f, 0.0f, 0.0f, // Origin
		1.0f, 1.0f, 1.0f // End
	};

	glm::vec3 lineCol;

	GLuint vao;
	GLuint vbo;
	GLuint ebo;
};