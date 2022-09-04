#pragma once

#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <iostream>
#include <filesystem>
#include <json/json.hpp>

#include "Window.h"
#include "DebugRenderer.h"
#include "PerspectiveRenderer.h"
#include "KinectSensor.h"
#include "UnitySharedMemory.h"

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
	void GetConfigFileNames();
	void LoadConfigFile(std::string filename);
	void SaveConfigFile(std::string filename);

	std::shared_ptr<GLFWwindow> window = nullptr;
	std::shared_ptr<DebugRenderer> debug_renderer = nullptr;
	std::shared_ptr<PerspectiveRenderer> perspective_renderer = nullptr;
	std::shared_ptr<KinectSensor> kinect_sensor = nullptr;
	UnitySharedMemory shared_memory;

	std::vector<SceneObject> debug_scene_objects;
	std::vector<SceneObject> perspective_scene_objects;

	double frame_time = 0, prev_frame_time = 0;
	bool should_program_close = false;

	const std::string config_path = ".\\config";
	std::vector<std::string> config_files = {};
	char save_filename_buffer[128] = "Hello, world!";
	int selected_config = -1;

	int hist_length = 200;
	std::vector<float> cam_hist_x;
	std::vector<float> cam_hist_y;
	std::vector<float> cam_hist_z;

	int cam_smoothing_lag = 5;
};

