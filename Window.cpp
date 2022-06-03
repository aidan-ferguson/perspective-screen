#include "Window.h"

void DestroyKinectWindow(GLFWwindow* window) {
	glfwDestroyWindow(window);
}

void WindowSizeChangeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

std::shared_ptr<GLFWwindow> CreateKinectWindow(std::string window_name, int width, int height)
{
	// Define the window
	std::shared_ptr<GLFWwindow> window(glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL), &DestroyKinectWindow);
	assert(window != nullptr);

	// Assign the OpenGL context to the window
	glfwMakeContextCurrent(window.get());
	glfwSetFramebufferSizeCallback(window.get(), WindowSizeChangeCallback);

	// Capture mouse 
	glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return window;
}
