#pragma once

#include <glm/glm.hpp>

#include "Mesh.h"

class Primitives {
public:
	static Vertex* GetCubeVertices();
	static GLuint* GetCubeIndices();

	static Vertex* GetQuadVertices();
	static GLuint* GetQuadIndices();

private:
	static const Vertex cube_vertices[6 * 4];
	static const GLuint cube_indices[6 * 6];

	static const Vertex quad_vertices[4];
	static const GLuint quad_indices[6];
};