#pragma once
#include "GameObject.h"
#include "OpenGL/Texture.h"

class Sphere : public GameObject {
public:
    Sphere(elevate::Vector3 pos, elevate::Vector3 scale, Shader* shdr, GameManager* gameMgr, float yaw = 0.0f)
        : GameObject(pos, scale, yaw, shdr, gameMgr)
    {}

	void GenerateSphere(float radius, unsigned int sectorCount, unsigned int stackCount);

    void LoadMesh();

    void CreateAndUploadVertexBuffer() const;

    void drawObject(glm::mat4 viewMat, glm::mat4 proj) override;

private:
    std::vector<float> vertices;
    std::vector<GLuint> indices;

    GLuint mVAO;
    GLuint mVBO;
    GLuint mEBO;
};