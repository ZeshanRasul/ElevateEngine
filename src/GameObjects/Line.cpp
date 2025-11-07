#include "Gameobjects/Line.h"

void Line::LoadMesh()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	CreateAndUploadVertexBuffer();
	glBindVertexArray(0);
}

void Line::CreateAndUploadVertexBuffer()
{
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

	// m_position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Line::UpdateVertexBuffer(glm::vec3 rayOrigin, glm::vec3 rayEnd)
{
	m_vertices[0] = rayOrigin.x;
	m_vertices[1] = rayOrigin.y;
	m_vertices[2] = rayOrigin.z;
	m_vertices[3] = rayEnd.x;
	m_vertices[4] = rayEnd.y;
	m_vertices[5] = rayEnd.z;

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertices), m_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Line::DrawLine(glm::mat4 viewMat, glm::mat4 proj, glm::vec3 lineColor)
{
	shader->use();
	shader->setMat4("view", viewMat);
	shader->setMat4("projection", proj);
	shader->setVec3("lineColor", lineColor);


	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void Line::drawObject(glm::mat4 viewMat, glm::mat4 proj)
{
	DrawLine(viewMat, proj, lineCol);
}
