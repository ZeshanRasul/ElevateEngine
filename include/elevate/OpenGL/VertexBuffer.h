#pragma once
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "OpenGL/RenderData.h"

class VertexBuffer {
public:
    void init();
    void uploadData(Mesh vertexData) const;
    void bind() const;
    void unbind();
    void draw(GLuint mode, unsigned int start, unsigned int num);
    void bindAndDraw(GLuint mode, unsigned int start, unsigned int num);
    void cleanup() const;

private:
    GLuint mVAO = 0;
    GLuint mVertexVBO = 0;
};