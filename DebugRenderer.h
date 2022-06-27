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


class DebugRenderer {
public:
	DebugRenderer(std::shared_ptr<KinectSensor> snsr);
	void MainLoop();

private:
	void OpenGLSetup();
	void HandleInput();
	bool IsKeyPressed(int key_code);
	void GetMouseDelta(double& d_x, double& d_y);
	void CreateFramebuffer();

	std::shared_ptr<KinectSensor> sensor = nullptr;
	std::shared_ptr<PerspectiveRenderer> perspective_renderer = nullptr;

	std::string window_name = std::string("Kinect Debug Renderer");

	std::shared_ptr<GLFWwindow> window = nullptr;
	bool program_should_close = false;
	Camera camera;

	double frame_time = 0, prev_frame_time = 0;
	double prev_mouse_x = 0, prev_mouse_y = 0;
	ImVec2 prev_debug_render_window_sz = ImVec2(1,1), prev_perspective_render_window_sz = ImVec2(1, 1);
	bool render_window_focused = false;

	ImGuiIO* imgui_io = nullptr;

	// temp
	GLuint fb_id;
	GLuint fb_texture_id;
	GLuint depth_buffer;

	const glm::vec3 kinect_dimensions = glm::vec3(0.249f, 0.045f, 0.067f);
};