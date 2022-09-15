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
#include "SharedMemory.h"

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
	SharedMemory shared_memory;

	std::vector<SceneObject> debug_scene_objects;
	std::vector<SceneObject> perspective_scene_objects;

	float frame_limit_ms = 1.0f / 60.0f;
	double frame_time = 0, prev_frame_time = 0, t_since_last_frame = 0;

	bool should_program_close = false;
	bool update_kinect_data = false;

	const std::string config_path = ".\\config";
	std::vector<std::string> config_files = {};
	char save_filename_buffer[128] = "Hello, world!";
	int selected_config = -1;

	int cam_hist_sz = 200;
	std::vector<glm::vec3> cam_hist;

	int cam_smoothing_lag = 5;
};

