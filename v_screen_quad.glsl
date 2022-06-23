#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCord;
layout (location = 2) in vec3 aNormal;

out vec2 texture_coords;

void main()
{
	texture_coords = aTexCord;
	gl_Position = vec4(aPos, 1.0);
}