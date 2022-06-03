#include "Window.h"

std::shared_ptr<GLFWwindow> CreateKinectWindow(std::string window_name, int width, int height)
{
	// Define the window
	GLFWwindow* window = glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL);
	assert(window != nullptr);

	// Assign the OpenGL context to the window
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, WindowSizeChangeCallback);

	// Capture mouse 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return std::shared_ptr<GLFWwindow>(window);
}

void WindowSizeChangeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
