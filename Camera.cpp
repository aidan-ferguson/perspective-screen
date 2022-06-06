#include "Camera.h"

glm::mat4 Camera::GetProjectionMatrix(int window_width, int window_height) 
{
	return glm::perspective(glm::radians(fov), (float)window_width / (float)window_height, 0.0001f, 100.0f);
}

glm::mat4 Camera::GetViewMatrix() 
{
	return glm::lookAt(position, position + direction, up);
}

void Camera::UpdateDirection() 
{
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);
}