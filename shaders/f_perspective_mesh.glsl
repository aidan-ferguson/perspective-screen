#version 330 core

in vec3 normal;
in vec3 pos;

layout(location = 0) out vec3 color;

void main()
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(vec3(-1, 1, 1) - pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

    color = vec3(0.1, 0.4, 0.8) * diffuse;
}