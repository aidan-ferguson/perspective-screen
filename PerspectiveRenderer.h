#pragma once

#define _USE_MATH_DEFINES
#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <math.h>

#include "KinectSensor.h"
#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

class PerspectiveRenderer
{
public:
	PerspectiveRenderer(std::shared_ptr<KinectSensor> snsr);
	void MainLoop();

private:
	void OpenGLSetup();
	void UpdateCamera();

	std::shared_ptr<KinectSensor> sensor;

	std::shared_ptr<GLFWwindow> window = nullptr;
	std::string window_name = std::string("Kinect Perspective Renderer");

	Camera camera;
};

