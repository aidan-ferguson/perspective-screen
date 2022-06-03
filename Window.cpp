#include "Window.h"

Window::Window(std::string window_name, int width, int height)
{
	// Define the window
	window = glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL);
	assert(window != nullptr);

	// Assign the OpenGL context to the window
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, Window::WindowSizeChangeCallback);

	// Capture mouse 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void Window::WindowSizeChangeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
