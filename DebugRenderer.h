#pragma once

#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

#include "KinectSensor.h"
#include "Window.h"
#include "Shader.h"
#include "PointCloud.h"
#include "Camera.h"

class DebugRenderer {
public:
	DebugRenderer(std::shared_ptr<KinectSensor> snsr);
	void MainLoop();

private:
	std::shared_ptr<KinectSensor> sensor;

	std::string window_name = std::string("Kinect Debug Renderer");

	std::shared_ptr<GLFWwindow> window = nullptr;
	Camera camera;

	double frame_time = 0, prev_frame_time = 0;
	double prev_mouse_x = 0, prev_mouse_y = 0;

	void OpenGLSetup();
	void HandleInput();
	bool IsKeyPressed(int key_code);
	void GetMouseDelta(double& d_x, double& d_y);
};