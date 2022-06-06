#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
	float fov = 90.0f;

public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);

	float camera_speed = 3.0f;
	float yaw = -90.0f;
	float pitch = 0.0f;

	float sensitivity = 0.1f;

	Camera() = default;

	glm::mat4 GetProjectionMatrix(int window_width, int window_height);
	glm::mat4 GetViewMatrix();

	void UpdateDirection();
};
