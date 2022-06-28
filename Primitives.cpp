#pragma once
#include "Primitives.h"

const Vertex Primitives::cube_vertices[4 * 6] = {
	// Front face
	glm::vec3(-0.5, 0.5, 0.5),  glm::vec2(0, 1), glm::vec3(0, 0, 1),
	glm::vec3(0.5, 0.5, 0.5),   glm::vec2(1, 1), glm::vec3(0, 0, 1),
	glm::vec3(-0.5, -0.5, 0.5), glm::vec2(0, 0), glm::vec3(0, 0, 1),
	glm::vec3(0.5, -0.5, 0.5),  glm::vec2(1, 0), glm::vec3(0, 0, 1),
	// Back face
	glm::vec3(-0.5, 0.5, -0.5), glm::vec2(1, 1), glm::vec3(0, 0, -1),
	glm::vec3(0.5, 0.5, -0.5),  glm::vec2(0, 1), glm::vec3(0, 0, -1),
	glm::vec3(-0.5, -0.5, -0.5),glm::vec2(1, 0), glm::vec3(0, 0, -1),
	glm::vec3(0.5, -0.5, -0.5), glm::vec2(0, 0), glm::vec3(0, 0, -1),
	// Top Face
	glm::vec3(-0.5, 0.5, 0.5),  glm::vec2(0, 1),  glm::vec3(0, 1, 0),
	glm::vec3(0.5, 0.5, 0.5),   glm::vec2(1, 1),  glm::vec3(0, 1, 0),
	glm::vec3(-0.5, 0.5, -0.5), glm::vec2(0, 0),  glm::vec3(0, 1, 0),
	glm::vec3(0.5, 0.5, -0.5),  glm::vec2(1, 0),  glm::vec3(0, 1, 0),
	// Bottom Face
	glm::vec3(-0.5, -0.5, 0.5),  glm::vec2(0, 1),  glm::vec3(0, -1, 0),
	glm::vec3(0.5, -0.5, 0.5),   glm::vec2(1, 1),  glm::vec3(0, -1, 0),
	glm::vec3(-0.5, -0.5, -0.5), glm::vec2(0, 0),  glm::vec3(0, -1, 0),
	glm::vec3(0.5, -0.5, -0.5),  glm::vec2(1, 0),  glm::vec3(0, -1, 0),
	// Left Face
	glm::vec3(-0.5, -0.5, -0.5),  glm::vec2(0, 0),  glm::vec3(-1, 0, 0),
	glm::vec3(-0.5, 0.5, -0.5),   glm::vec2(0, 1),  glm::vec3(-1, 0, 0),
	glm::vec3(-0.5, -0.5, 0.5),   glm::vec2(1, 0),  glm::vec3(-1, 0, 0),
	glm::vec3(-0.5, 0.5, 0.5),    glm::vec2(1, 1),  glm::vec3(-1, 0, 0),
	// Right Face
	glm::vec3(0.5, -0.5, 0.5),   glm::vec2(0, 0),  glm::vec3(1, 0, 0),
	glm::vec3(0.5, 0.5, 0.5),    glm::vec2(0, 1),  glm::vec3(1, 0, 0),
	glm::vec3(0.5, -0.5, -0.5),  glm::vec2(1, 0),  glm::vec3(1, 0, 0),
	glm::vec3(0.5, 0.5, -0.5),   glm::vec2(1, 1),  glm::vec3(1, 0, 0),
};

const GLuint Primitives::cube_indices[6 * 6] = {
	// Front
	0, 3, 1,
	0, 2, 3,
	// Back
	4, 5, 7,
	4, 7, 6,
	// Top
	8, 9, 11,
	8, 11, 10,
	// Bottom
	12, 15, 13,
	12, 14, 15,
	// Left
	16, 19, 17,
	16, 18, 19,
	// Right
	20, 23, 21,
	20, 22, 23,
};

Vertex* Primitives::GetCubeVertices()
{
	Vertex* vertices = new Vertex[sizeof(cube_vertices)];
	memcpy(vertices, cube_vertices, sizeof(cube_vertices));
	return vertices;
}

GLuint* Primitives::GetCubeIndices()
{
	GLuint* indices = new GLuint[sizeof(cube_indices)];
	memcpy(indices, cube_indices, sizeof(cube_indices));
	return indices;
}

const Vertex Primitives::quad_vertices[4] = {
	// Front face
	glm::vec3(-0.5, 0.5, 0.0),  glm::vec2(0, 1), glm::vec3(0, 0, 1),
	glm::vec3(0.5, 0.5, 0.0),   glm::vec2(1, 1), glm::vec3(0, 0, 1),
	glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0, 0), glm::vec3(0, 0, 1),
	glm::vec3(0.5, -0.5, 0.0),  glm::vec2(1, 0), glm::vec3(0, 0, 1),
};

const GLuint Primitives::quad_indices[6] = {
	// Front
	0, 3, 1,
	0, 2, 3,
};

Vertex* Primitives::GetQuadVertices()
{
	Vertex* vertices = new Vertex[sizeof(quad_vertices)];
	memcpy(vertices, quad_vertices, sizeof(quad_vertices));
	return vertices;
}

GLuint* Primitives::GetQuadIndices()
{
	GLuint* indices = new GLuint[sizeof(quad_indices)];
	memcpy(indices, quad_indices, sizeof(quad_indices));
	return indices;
}
