#include "SceneObject.h"

SceneObject::SceneObject(std::string name, Mesh mesh, GLuint shader)
{
	this->name = name;
	this->mesh = mesh;
	this->shader = shader;
}

void SceneObject::UpdateModelMatrix() 
{
	glm::mat4 new_model_matrix = glm::mat4(1.0f);
	new_model_matrix = glm::translate(new_model_matrix, position);
	new_model_matrix = glm::rotate(new_model_matrix, glm::radians(rotation.x), glm::vec3(1.0, 0.0f, 0.0f));
	new_model_matrix = glm::rotate(new_model_matrix, glm::radians(rotation.y), glm::vec3(0.0, 1.0f, 0.0f));
	new_model_matrix = glm::rotate(new_model_matrix, glm::radians(rotation.z), glm::vec3(0.0, 0.0f, 1.0f));
	new_model_matrix = glm::scale(new_model_matrix, scale);

	model_matrix = new_model_matrix;
}

void SceneObject::Draw(glm::mat4 view_matrix, glm::mat4 projection_matrix)
{
	UpdateModelMatrix();

	glUseProgram(shader);
	SetUniformMat4(shader, "projection_matrix", projection_matrix);
	SetUniformMat4(shader, "view_matrix", view_matrix);
	SetUniformMat4(shader, "model_matrix", model_matrix);
	mesh.Draw();
}
