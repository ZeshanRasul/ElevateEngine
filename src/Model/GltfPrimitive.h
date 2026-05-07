#pragma once

#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"

class GLTFPrimitive {

public:
	GLuint vao;
	GLuint indexBuffer;
	GLsizei indexCount;
	GLsizei vertexCount;
	GLenum indexType;
	GLenum mode;
	int material;
	std::vector<glm::vec3> verts;
	std::vector<unsigned int> indices;

	GLTFPrimitive() = default;
};