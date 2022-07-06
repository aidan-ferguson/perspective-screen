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
	Mesh(Vertex* vertices, int _n_vertices, unsigned int* indices, int n_indices, GLenum buffer_usage, GLenum draw_mode);
	void Update();
	void Draw();

	Vertex* vertices = nullptr;
private:
	GLuint VAO, VBO, EBO = 0;
	unsigned int n_indices, n_vertices = 0;
	bool use_draw_elements = false;
	GLenum buffer_usage, draw_mode;

};

