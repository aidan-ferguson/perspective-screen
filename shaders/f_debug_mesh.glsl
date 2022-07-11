#version 330 core

in vec3 normal;
in vec3 pos;

layout(location = 0) out vec3 color;

void main()
{
    color = (normal/2) + 0.5;//vec3(1.0f, 0.5f, 0.2f) * clamp((dot(normal, vec3(-2, 5, 4))), 0.3f, 1.0f);
}