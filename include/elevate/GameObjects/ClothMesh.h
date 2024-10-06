#pragma once

#include "GameObject.h"
#include "OpenGL/Texture.h"
#include "Physics/particle.h"

class ClothMesh : public GameObject {
public:
    ClothMesh(elevate::Vector3 pos, elevate::Vector3 scale, Shader* shdr, GameManager* gameMgr, int width, int height, float yaw = 0.0f)
		: GameObject(pos, scale, yaw, shdr, gameMgr), width(width), height(height)
    {}

    void LoadMesh();

    void UpdateMesh(std::vector<elevate::Particle*> particles);

    void CreateAndUploadVertexBuffer();

    void drawObject(glm::mat4 viewMat, glm::mat4 proj) override;

private:
    std::vector<float> vertices;
    std::vector<GLuint> indices;

    GLuint mVAO;
    GLuint mVBO;
    GLuint mEBO;

    int width;
    int height;

};