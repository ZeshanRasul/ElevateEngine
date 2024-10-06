#include "GameObjects/ClothMesh.h"

void ClothMesh::LoadMesh()
{
}

void ClothMesh::UpdateMesh(std::vector<elevate::Particle*> particles)
{
    std::vector<float> vertices(width * height * 8);

    // Update vertex positions and normals
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;
            elevate::Vector3 pos = particles[index]->getPosition();

            // Positions
            vertices[index * 8 + 0] = pos.x;
            vertices[index * 8 + 1] = pos.y;
            vertices[index * 8 + 2] = pos.z;

            // Normals (placeholder, compute proper normals in a full implementation)
            vertices[index * 8 + 3] = 0.0f;
            vertices[index * 8 + 4] = 0.0f;
            vertices[index * 8 + 5] = 1.0f;

            // Texture coordinates
            vertices[index * 8 + 6] = (float)x / (width - 1);
            vertices[index * 8 + 7] = (float)y / (height - 1);
        }
    }

    // Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);

}

void ClothMesh::CreateAndUploadVertexBuffer()
{
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    std::vector<float> vertices(width * height * 8); // x, y, z, nx, ny, nz, u, v
    std::vector<unsigned int> indices;

    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {
            int topLeft = y * width + x;
            int topRight = topLeft + 1;
            int bottomLeft = (y + 1) * width + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coords
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

}

void ClothMesh::drawObject(glm::mat4 viewMat, glm::mat4 proj)
{
}
