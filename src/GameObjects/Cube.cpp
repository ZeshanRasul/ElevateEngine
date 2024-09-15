#include "GameObjects/Cube.h"
#include "App/GameManager.h"

void Cube::LoadMesh()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);
	glBindVertexArray(mVAO);
	CreateAndUploadVertexBuffer();
	glBindVertexArray(0);
}

void Cube::CreateAndUploadVertexBuffer() const
{
	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void Cube::drawObject(glm::mat4 viewMat, glm::mat4 proj)
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	glm::vec3 posVec = glm::vec3(GetPosition().x, GetPosition().y, GetPosition().z);
	glm::vec3 scaleVec = glm::vec3(GetScale().x, GetScale().y, GetScale().z);
	modelMat = glm::translate(modelMat, posVec);
	modelMat = glm::scale(modelMat, scaleVec);
	std::vector<glm::mat4> matrixData;
	matrixData.push_back(viewMat);
	matrixData.push_back(proj);
	matrixData.push_back(modelMat);
	mUniformBuffer.uploadUboData(matrixData, 0);

	shader->setVec3("objectColor", color);

	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}