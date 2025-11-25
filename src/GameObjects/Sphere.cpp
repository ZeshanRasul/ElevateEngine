#include "GameObjects/Sphere.h"

void Sphere::GenerateSphere(float radius, unsigned int sectorCount, unsigned int stackCount)
{
    float x, y, z, xy;                      
    float lengthInv = 1.0f / radius;  

    float sectorStep = 2 * elevate::PI / sectorCount;
    float stackStep = elevate::PI / stackCount;
    float sectorAngle, stackAngle;

    // Loop through stacks
    for (unsigned int i = 0; i <= stackCount; ++i) {
        stackAngle = elevate::PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // Loop through sectors
        for (unsigned int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // Vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // x = r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // y = r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(x * lengthInv);
            vertices.push_back(y * lengthInv);
            vertices.push_back(z * lengthInv);
            vertices.push_back((float)j / sectorCount);
            vertices.push_back((float)i / sectorCount);
        }
    }

    // Generate indices
    unsigned int k1, k2;
    for (unsigned int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);  // beginning of current stack
        k2 = k1 + sectorCount + 1;   // beginning of next stack

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                // First triangle
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) {
                // Second triangle
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

void Sphere::LoadMesh()
{
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);
    glBindVertexArray(mVAO);
    CreateAndUploadVertexBuffer();
    glBindVertexArray(0);
}

void Sphere::CreateAndUploadVertexBuffer() const
{
    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Sphere::drawObject(glm::mat4 viewMat, glm::mat4 proj)
{
    glm::mat4 modelMat = glm::mat4(1.0f);
    glm::vec3 posVec = glm::vec3(GetPosition().x, GetPosition().y, GetPosition().z);
    glm::vec3 scaleVec = glm::vec3(GetScale().x, GetScale().y, GetScale().z);
    modelMat = glm::translate(modelMat, posVec);
  //  modelMat *= glm::mat4_cast(orientation);
    modelMat = glm::scale(modelMat, scaleVec);
    std::vector<glm::mat4> matrixData;
    matrixData.push_back(viewMat);
    matrixData.push_back(proj);
    matrixData.push_back(modelMat);
    mUniformBuffer.uploadUboData(matrixData, 0);

    shader->setVec3("objectColor", color);

    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}