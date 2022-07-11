#pragma once

#include <string>
#include <vector>

#include "Shader.h"
#include "Mesh.h"

class SceneObject
{
public:
	SceneObject() = default;
	SceneObject(std::string name, Mesh mesh, GLuint shader);
	void Draw(glm::mat4 view_matrix, glm::mat4 projection_matrix);

	std::string name = std::string("");
	std::vector<GLuint> textures;
	Mesh mesh;
	GLuint shader;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::mat4 model_matrix = glm::mat4(1.0f);
	glm::mat4 parent_model_matrix = glm::mat4(1.0f);

private:
	void UpdateModelMatrix();
};

