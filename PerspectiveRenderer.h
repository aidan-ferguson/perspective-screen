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
	void HandleInput();
	bool IsKeyPressed(int key_code);
	void GetMouseDelta(double& d_x, double& d_y);
	void CreateFramebuffer();
	glm::vec2 WorldToScreenSpace(glm::mat4 model_matrix, glm::vec3 object_point);

	std::shared_ptr<KinectSensor> sensor;
	
	Camera camera;

	std::shared_ptr<GLFWwindow> window = nullptr;
	std::string window_name = std::string("Kinect Perspective Renderer");

	double frame_time = 0, prev_frame_time = 0;
	double prev_mouse_x = 0, prev_mouse_y = 0;
	bool manual_control = true;

	// Calibration will need to be improved (currently screen will be inline with sensor
	glm::mat4 screen_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5f));
	glm::vec3 screen_top_left = glm::vec3(-0.5f, 0.5f, 0.0f);
	glm::vec3 screen_top_right = glm::vec3(0.5f, 0.5f, 0.0f);
	glm::vec3 screen_bottom_left = glm::vec3(-0.5f, -0.5f, 0.0f);
	glm::vec3 screen_bottom_right = glm::vec3(0.5f, -0.5f, 0.0f);

	GLuint fb_id;
	GLuint fb_texture_id;
};

