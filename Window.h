#pragma once

#include <string>
#include <assert.h>
#include <GLFW/glfw3.h>

class Window {
public:
	Window() = default;
	Window(std::string window_name, int width, int height);

	GLFWwindow* window = nullptr;
	int width = 0, height = 0;

private:
	static void WindowSizeChangeCallback(GLFWwindow* window, int width, int height);
};