#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Tools/tiny_gltf.h"
#include "OpenGL/Shader.h"
#include "Physics/core.h"
#include "GltfPrimitive.h"
#include "OpenGL/UniformBuffer.h"

class GltfModel
{
public:
	GltfModel(std::string modelFilename, Shader* shader);

	bool LoadModel();
	void CreateVertexBuffers();
	void CreateIndexBuffer();
	void Draw(glm::mat4 view, glm::mat4 proj, glm::mat4 world);

	void UploadVertexBuffers();
	void UploadIndexBuffer();

	std::string m_filename;
	std::shared_ptr<tinygltf::Model> m_model = nullptr;

	std::vector<glm::vec3> m_vertices{};
	GLuint m_vao = 0;
	std::vector<GLuint> m_vertexVbo{};
	GLuint m_indexVbo = 0;
	std::map<std::string, GLint> m_attributes =
	{
		{"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2}, {"TEXCOORD_1", 3}, {"COLOR_0", 4}, {"COLOR_1", 5},
		{"JOINTS_0", 6}, {"WEIGHTS_0", 7}, {"TANGENT", 8}
	};
	std::vector<int> m_attribAccessors{};

	Shader* m_shader = nullptr;

	void SetPosition(const glm::vec3& position);
	void SetPosition(const elevate::Vector3& position);

	void SetScale(float uniformScale);
	void SetScale(const glm::vec3& scale);
	void SetScale(const elevate::Vector3& scale);

	void SetOrientation(const glm::quat& orientation);

	const glm::mat4& GetModelMatrix() const;

private:
	void RecalculateModelMatrix();

	glm::vec3 m_position = glm::vec3(0.0f);
	glm::vec3 m_scale = glm::vec3(1.0f);
	glm::quat m_orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	UniformBuffer mUniformBuffer{};

	struct GLTFPrimitive {
		GLuint vao;
		GLuint indexBuffer;
		GLsizei indexCount;
		GLsizei vertexCount;
		GLenum indexType;
		GLenum mode;
		int material;
		std::vector<glm::vec3> verts;
		std::vector<unsigned int> indices;

	};

	struct GLTFMesh {
		std::vector<GLTFPrimitive> primitives;
	};
	std::vector<GLTFMesh> meshData;
};