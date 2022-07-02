#pragma once

#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

#include "KinectSensor.h"
#include "PerspectiveRenderer.h"
#include "Window.h"
#include "Shader.h"
#include "PointCloud.h"
#include "Camera.h"
#include "Primitives.h"
#include "SceneObject.h"
#include "OpenGLUtility.h"

class DebugRenderer {
public:
	DebugRenderer();
	void MainLoop();
	void DrawFrame(std::vector<SceneObject>& debug_scene_objects, std::vector<SceneObject>& perspective_scene_objects);
	void HandleInput(float frame_time);
	void UpdateMousePosition();

	glm::vec2 renderer_size = glm::vec2(1, 1);

	GLuint rgb_buffer = 0;
	GLuint depth_buffer = 0;
private:
	bool IsKeyPressed(GLFWwindow* window, int key_code);
	void GetMouseDelta(GLFWwindow* window, double& d_x, double& d_y);

	Camera camera;
	GLuint fb_id = 0;
	double prev_mouse_x = 0, prev_mouse_y = 0;
	const glm::vec3 kinect_dimensions = glm::vec3(0.249f, 0.045f, 0.067f);
};