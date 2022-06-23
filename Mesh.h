#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Mesh
{
public:
	Mesh(float* vertices, int n_vertices, unsigned int* indices, int n_indices);
	void Draw();

private:
	GLuint VAO, VBO, EBO = 0;
	unsigned int n_indices = 0;
};

