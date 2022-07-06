#include "DebugRenderer.h"

DebugRenderer::DebugRenderer()
{
	// Initial camera position 
	camera.position = glm::vec3(-2.0f, 0.5, 1.0f);
	camera.yaw = 0.0f;
	camera.pitch = -5.0f;
	camera.UpdateDirection();

	// Populate the previous mouse positions
	UpdateMousePosition();

	// Create the fb render target
	CreateFramebuffer(fb_id, rgb_buffer, depth_buffer, renderer_size.x, renderer_size.y);
}

bool DebugRenderer::IsKeyPressed(GLFWwindow* window, int key_code) {
	return (glfwGetKey(window, key_code) == GLFW_PRESS);
}

void DebugRenderer::GetMouseDelta(GLFWwindow* window, double& d_x, double& d_y)
{
	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
 
	// Y axis is inverted so it is the other way around
	d_x = mouse_x - prev_mouse_x;
	d_y = prev_mouse_y - mouse_y;
}

// Must be called after GetMouseDelta in the application mainloop
void DebugRenderer::UpdateMousePosition()
{
	GLFWwindow* window = glfwGetCurrentContext();
	glfwGetCursorPos(window, &prev_mouse_x, &prev_mouse_y);
}

void DebugRenderer::HandleInput(float frame_time) {
	GLFWwindow* window = glfwGetCurrentContext();

	double d_x, d_y;
	GetMouseDelta(window, d_x, d_y);

	if (IsKeyPressed(window, GLFW_KEY_W)) {
		camera.position += camera.direction * camera.camera_speed * (float)frame_time;
	}
	if (IsKeyPressed(window, GLFW_KEY_S)) {
		camera.position -= camera.direction * camera.camera_speed * (float)frame_time;
	}
	if (IsKeyPressed(window, GLFW_KEY_A)) {
		camera.position -= glm::cross(camera.direction, camera.up) * camera.camera_speed * (float)frame_time;
	}
	if (IsKeyPressed(window, GLFW_KEY_D)) {
		camera.position += glm::cross(camera.direction, camera.up) * camera.camera_speed * (float)frame_time;
	}
	if (IsKeyPressed(window, GLFW_KEY_RIGHT)) {
		d_x += 3;
	}
	if (IsKeyPressed(window, GLFW_KEY_LEFT)) {
		d_x -= 3;
	}
	if (IsKeyPressed(window, GLFW_KEY_UP)) {
		d_y += 3;
	}
	if (IsKeyPressed(window, GLFW_KEY_DOWN)) {
		d_y -= 3;
	}

	camera.yaw += d_x * camera.sensitivity * frame_time;
	// Clamped to -89 and 89 because after that the image will flip
	camera.pitch = glm::clamp((float)(camera.pitch + d_y * camera.sensitivity * frame_time), -89.0f, 89.0f);
	camera.UpdateDirection();
}

void DebugRenderer::DrawFrame(std::vector<SceneObject>& debug_scene_objects, std::vector<SceneObject>& perspective_scene_objects)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fb_id);
	glViewport(0, 0, renderer_size.x, renderer_size.y);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view_matrix = camera.GetViewMatrix();
	glm::mat4 projection_matrix = camera.GetProjectionMatrix(renderer_size.x, renderer_size.y);

	if (show_depth_point_cloud) {
		glPointSize(1.0f);
		depth_point_cloud.Draw(view_matrix, projection_matrix);
	}
	if (show_eye_point_cloud) {
		glPointSize(7.5f);
		eye_point_cloud.Draw(view_matrix, projection_matrix);
	}

	for (SceneObject& scene_obj : debug_scene_objects) {
		scene_obj.Draw(view_matrix, projection_matrix);
	}

	for (SceneObject& scene_obj : perspective_scene_objects) {
		scene_obj.Draw(view_matrix, projection_matrix);
	}
}