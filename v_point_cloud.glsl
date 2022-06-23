#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

out vec3 colour;

void main()
{
	colour = aCol;
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(aPos, 1.0);
}