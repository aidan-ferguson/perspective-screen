#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec2 texture_coordinate;
	glm::vec3 normal;

	Vertex() = default;
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(Vertex* vertices, int _n_vertices, unsigned int* indices, int n_indices, GLenum usage);
	void Update(Vertex* vertices);
	void Draw();

private:
	GLuint VAO, VBO, EBO = 0;
	unsigned int n_indices, n_vertices = 0;
	GLenum buffer_usage;
};

