#pragma once
#include "GameObject.h"
#include "OpenGL/Texture.h"

class Sphere : public GameObject {
public:
    Sphere(elevate::Vector3 pos, elevate::Vector3 scale, Shader* shdr, GameManager* gameMgr, glm::vec3 col, float yaw = 0.0f)
		: GameObject(pos, scale, yaw, shdr, gameMgr), color(col)
    {}

    Sphere()
        : GameObject(elevate::Vector3(0.0f, 0.0f, 0.0f), elevate::Vector3(1.0f, 1.0f, 1.0f), 0.0f, nullptr, nullptr)
    {
    };

	void GenerateSphere(float radius, unsigned int sectorCount, unsigned int stackCount);

    void LoadMesh();

    void CreateAndUploadVertexBuffer() const;

    void drawObject(glm::mat4 viewMat, glm::mat4 proj) override;
    void SetShader(Shader* shdr) { shader = shdr; }
    Shader* GetShader() const { return shader; }

private:
    std::vector<float> vertices;
    std::vector<GLuint> indices;

    GLuint mVAO;
    GLuint mVBO;
    GLuint mEBO;

    glm::vec3 color = {0.8f, 0.8f, 0.1f};
};