#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

#include "Window.h"
#include "Shader.h"

class KinectRenderer {
public:
	KinectRenderer();
	void MainLoop();

private:
	std::string window_name = std::string("Kinect");
	std::shared_ptr<GLFWwindow> window = nullptr;
	double frame_time = 0, prev_frame_time = 0;

	void OpenGLSetup();
	void HandleInput();
	bool IsKeyPressed(int key_code);
};