#pragma once

#include <string>
#include <assert.h>
#include <memory>
#include <GLFW/glfw3.h>

std::shared_ptr<GLFWwindow> CreateKinectWindow(std::string window_name, int width, int height);
void WindowSizeChangeCallback(GLFWwindow* window, int width, int height);