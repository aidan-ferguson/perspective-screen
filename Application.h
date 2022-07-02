#pragma once

#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

#include "Window.h"
#include "DebugRenderer.h"
#include "PerspectiveRenderer.h"
#include "KinectSensor.h"

class Application
{
public:
	Application(std::string name);
	void MainLoop();

private:
	void OpenGLSetup();
	void ImGuiSetup();
	void DrawImGui();
	void SceneObjectSetup();

	std::shared_ptr<GLFWwindow> window = nullptr;
	std::shared_ptr<DebugRenderer> debug_renderer = nullptr;
	std::shared_ptr<PerspectiveRenderer> perspective_renderer = nullptr;
	std::shared_ptr<KinectSensor> kinect_sensor = nullptr;

	std::vector<SceneObject> debug_scene_objects;
	std::vector<SceneObject> perspective_scene_objects;

	double frame_time = 0, prev_frame_time = 0;
	bool should_program_close = false;
};

