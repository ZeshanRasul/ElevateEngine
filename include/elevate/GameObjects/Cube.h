#pragma once
#include "GameObject.h"
#include "OpenGL/Texture.h"

class Cube : public GameObject {
public:
    Cube(elevate::Vector3 pos, elevate::Vector3 scale, Shader* shdr, GameManager* gameMgr, float yaw = 0.0f)
        : GameObject(pos, scale, yaw, shdr, gameMgr)
    {}

    void LoadMesh();

    void CreateAndUploadVertexBuffer() const;

    void drawObject(glm::mat4 viewMat, glm::mat4 proj) override;

private:

    float vertices[192] = {
        // Positions           // Normals          // Texture Coords
        // Back face (z = -0.5)
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  // 0 Back-bottom-left
         0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,  // 1 Back-bottom-right
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  // 2 Back-top-right
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,  // 3 Back-top-left

        // Front face (z = 0.5)
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  // 4 Front-bottom-left
         0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  // 5 Front-bottom-right
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  // 6 Front-top-right
        -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 1.0f,  // 7 Front-top-left

        // Left face (x = -0.5)
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  // 8 Left-top-front
        -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  // 9 Left-top-back
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // 10 Left-bottom-back
        -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  // 11 Left-bottom-front

        // Right face (x = 0.5)
         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  // 12 Right-top-front
         0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  // 13 Right-top-back
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // 14 Right-bottom-back
         0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  // 15 Right-bottom-front

         // Bottom face (y = -0.5)
         -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  // 16 Bottom-back-left
          0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f,  // 17 Bottom-back-right
          0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  // 18 Bottom-front-right
         -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  // 19 Bottom-front-left

         // Top face (y = 0.5)
         -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  // 20 Top-back-left
          0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f,  // 21 Top-back-right
          0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  // 22 Top-front-right
         -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f   // 23 Top-front-left
    };

    GLuint indices[36] = {
        // Back face
        0, 1, 2,    2, 3, 0,    

        // Front face
        4, 5, 6,    6, 7, 4,    

        // Left face
        8, 9, 10,   10, 11, 8,  

        // Right face
        12, 13, 14, 14, 15, 12, 

        // Bottom face
        16, 17, 18, 18, 19, 16, 

        // Top face
        20, 21, 22, 22, 23, 20  
    };

    GLuint mVAO;
    GLuint mVBO;
    GLuint mEBO;

	glm::vec3 color = glm::vec3(1.0f, 0.4f, 0.0f);
};                  