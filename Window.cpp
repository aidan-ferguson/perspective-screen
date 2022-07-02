#include "Window.h"

std::shared_ptr<GLFWwindow> CreateKinectWindow(std::string window_name, int width, int height, bool fullscreen)
{
	if (fullscreen) {
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		width = mode->width;
		height = mode->height;
	}

	// Define the window
	std::shared_ptr<GLFWwindow> window(glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL), &glfwDestroyWindow);
	assert(window != nullptr);

	if (fullscreen) {
		glfwSetWindowPos(window.get(), 0, 0);
	}

	// Assign the OpenGL context to the window
	glfwMakeContextCurrent(window.get());

	// Capture mouse 
	glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	return window;
}
