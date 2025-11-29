#version 460 core

layout(location = 0) in vec3 aPos0;
layout(location = 1) in vec3 aColor0;

uniform mat4 uViewProj;

out vec3 vColor;

void main()
{
    gl_Position = uViewProj * vec4(aPos0, 1.0);
    vColor = aColor0;
}
