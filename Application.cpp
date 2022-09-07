#include "Application.h"

Application::Application(std::string name)
{
	// Create and position window
	glfwInit();
	window = CreateKinectWindow(name, 1500, 800, false);
	
	// Initliase GLAD and OpenGL
	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0);
	OpenGLSetup();
	
	// ImGui setup
	ImGuiSetup();
	
	// Setup both renderers
	debug_renderer = std::shared_ptr<DebugRenderer>(new DebugRenderer());
	perspective_renderer = std::shared_ptr<PerspectiveRenderer>(new PerspectiveRenderer());
	
	// Setup kinect sensor
	kinect_sensor = std::shared_ptr<KinectSensor>(new KinectSensor());
	
	// Create default scene objects
	SceneObjectSetup();

	// Populate the vector which stores names for config files
	GetConfigFileNames();
}

void Application::OpenGLSetup()
{
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::ImGuiSetup()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
	ImGui_ImplOpenGL3_Init(nullptr);

	// Setup Dear ImGui style
	ImGui::StyleColorsClassic();
}

void Application::GetConfigFileNames()
{
	config_files = {};
	std::vector<std::string> tmp_cfg_files = {};
	for (const auto& elem : std::filesystem::directory_iterator(config_path)) {
		if (elem.is_regular_file()) {
			config_files.push_back(elem.path().stem().string());
		}
	}
}

void Application::LoadConfigFile(std::string filename)
{
	std::string filepath = (std::filesystem::path(config_path) / std::filesystem::path(filename + std::string(".json"))).string();
	std::string json_text = ReadPlaintextFile(filepath);

	nlohmann::json config = nlohmann::json::parse(json_text);

	if (config.contains("debug_scene_objects")) {
		for (SceneObject& scene_object : debug_scene_objects) {
			if (config["debug_scene_objects"].contains(scene_object.name)) {
				for (auto& [key, value] : config["debug_scene_objects"][scene_object.name].items()) {
					if (value.is_array() && value.size() == 3) {
						if (key == "position") {
							scene_object.position = glm::vec3(value[0], value[1], value[2]);
						}
						else if (key == "rotation") {
							scene_object.rotation = glm::vec3(value[0], value[1], value[2]);
						}
						else if (key == "scale") {
							scene_object.scale = glm::vec3(value[0], value[1], value[2]);
						}
					}
				}
			}
		}
	}
}

void Application::SaveConfigFile(std::string filename) 
{
	nlohmann::json config = {};
	config["debug_scene_objects"] = {};
	for (SceneObject& obj : debug_scene_objects) {
		config["debug_scene_objects"][obj.name] = {};
		config["debug_scene_objects"][obj.name]["position"] = { obj.position[0], obj.position[1], obj.position[2] };
		config["debug_scene_objects"][obj.name]["rotation"] = { obj.rotation[0], obj.rotation[1], obj.rotation[2] };
		config["debug_scene_objects"][obj.name]["scale"] = { obj.scale[0], obj.scale[1], obj.scale[2] };
 	}

	std::string json_text = config.dump();
	std::string filepath = (std::filesystem::path(config_path) / std::filesystem::path(filename + std::string(".json"))).string();
	WritePlaintextFile(filepath, json_text);
}

void Application::DrawImGui() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Create a dockspace and main menu bar
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("Exit", "ALT+F4", &should_program_close);
			ImGui::Separator();
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}


	//ImGui::ShowDemoWindow();


	ImGui::Begin("Debug view");
	if (glfwGetMouseButton(window.get(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && ImGui::IsWindowFocused()) {
		debug_renderer->HandleInput(frame_time);
	}

	ImVec2 debug_window_size = ImGui::GetWindowSize();
	// TODO: Have resize function in renderer
	if (debug_window_size.x != debug_renderer->renderer_size.x || debug_window_size.y != debug_renderer->renderer_size.y) {
		glViewport(0, 0, debug_window_size.x, debug_window_size.y);
		glBindTexture(GL_TEXTURE_2D, debug_renderer->rgb_buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, debug_window_size.x, debug_window_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, debug_renderer->depth_buffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, debug_window_size.x, debug_window_size.y);
	}
	ImGui::Image((void*)(intptr_t)debug_renderer->rgb_buffer, ImVec2(debug_window_size.x, debug_window_size.y), ImVec2(0, 1), ImVec2(1, 0));
	debug_renderer->renderer_size.x = debug_window_size.x;
	debug_renderer->renderer_size.y = debug_window_size.y;
	ImGui::End();


	/*ImGui::Begin("Perspective view");
	ImVec2 perspective_window_size = ImGui::GetWindowSize();
	if (perspective_window_size.x != perspective_renderer->window_size.x || perspective_window_size.y != perspective_renderer->window_size.y) {
		perspective_renderer->Resize(perspective_window_size.x, perspective_window_size.y);
	}
	ImGui::Image((void*)(intptr_t)perspective_renderer->screen_texture, ImVec2(perspective_window_size.x, perspective_window_size.y), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();*/


	ImGui::Begin("Debug tools");
	ImGui::SetNextItemOpen(true);
	if (ImGui::TreeNode("General options")) {
		ImGui::Checkbox("Update Kinect Data", &update_kinect_data);
		ImGui::Checkbox("Render point cloud", &debug_renderer->show_depth_point_cloud);
		ImGui::Checkbox("Render eye points", &debug_renderer->show_eye_point_cloud);
		ImGui::DragFloat3("Perspective camera", (float*)(void*)&perspective_renderer->camera.position, 0.005);

		ImGui::Separator();
		ImGui::PlotLines("Cam X", cam_hist_x.data(), cam_hist_x.size());
		ImGui::PlotLines("Cam Y", cam_hist_y.data(), cam_hist_y.size());
		ImGui::PlotLines("Cam Z", cam_hist_z.data(), cam_hist_z.size());
		ImGui::DragInt("Position moving lag (frames)", &cam_smoothing_lag, 0, hist_length-1);
		
		// Saving and loading of scenes
		ImGui::Separator();
		ImGui::InputText("CFG filename", save_filename_buffer, 128);
		if (ImGui::Button("Save config")) {
			SaveConfigFile(std::string(save_filename_buffer));
		}

		if (ImGui::Button("Reload")) {
			GetConfigFileNames();
		}
		ImGui::SameLine();
		if (ImGui::BeginCombo("Load CFG", "")) {
			for (int i = 0; i < config_files.size(); i++) {

				const bool is_selected = (selected_config == i);
				if (ImGui::Selectable(config_files[i].c_str(), is_selected)) {
					selected_config = i;
					LoadConfigFile(config_files[i]);
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		ImGui::Separator();
		if (ImGui::Button("Start calibration")) {

		}

		ImGui::Separator();
		ImGui::Text("Connected monitors");
		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		
		for (int monitor = 0; monitor < count; monitor++) {
			int width, height;
			char* name = (char*)glfwGetMonitorName(monitors[monitor]);
			glfwGetMonitorPhysicalSize(monitors[monitor], &width, &height);

			std::string monitor_text = name + std::string(": ") + std::to_string(width) + "mm x " + std::to_string(height) + "mm";
			ImGui::Text(monitor_text.c_str());
		}

		ImGui::Separator();
		ImGui::DragFloat("Frame time Limit (ms)", &frame_limit_ms);
		ImGui::Text(std::string("Real frame time: " + std::to_string(frame_time * 1000)).c_str());
		ImGui::Separator();
		/*ImGui::Combo("Monitor", &current_selected_monitor, monitor_names.data(), monitor_names.size());
		if (current_selected_monitor >= 0) {
			glm::vec2 monitor_phys_size = monitor_physical_sizes[current_selected_monitor];
			world_screen.scale.x = monitor_phys_size.x;
			world_screen.scale.y = monitor_phys_size.y;
			current_selected_monitor = -1;
		}
		ImGui::Separator();
		for (bool face_available : sensor->GetAvailableFaces()) {
			const char* text = face_available ? "1" : "0";
			ImGui::Text(text);
		}
		*/
		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("Scene Objects")) {
		if (ImGui::TreeNode("Debug Objects")) {
			for (SceneObject& scene_obj : debug_scene_objects) {
				ImGui::SetNextItemOpen(true);
				if (ImGui::TreeNode(scene_obj.name.c_str())) {
					ImGui::DragFloat3("Position", (float*)(void*)&scene_obj.position, 0.005f);
					ImGui::DragFloat3("Rotation", (float*)(void*)&scene_obj.rotation, 0.25f);
					ImGui::DragFloat3("Scale", (float*)(void*)&scene_obj.scale, 0.005f, 0, INT_MAX);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		/*if (ImGui::TreeNode("Perspective Objects")) {
			for (SceneObject& scene_obj : perspective_scene_objects) {
				ImGui::SetNextItemOpen(true);
				if (ImGui::TreeNode(scene_obj.name.c_str())) {
					ImGui::DragFloat3("Position", (float*)(void*)&scene_obj.position, 0.005f);
					ImGui::DragFloat3("Rotation", (float*)(void*)&scene_obj.rotation, 0.25f);
					ImGui::DragFloat3("Scale", (float*)(void*)&scene_obj.scale, 0.005f, 0, INT_MAX);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}*/
		ImGui::TreePop();
	}
	ImGui::End();
	

	// Draw to default frame buffer (main window)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render dear imgui onto screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void Application::SceneObjectSetup()
{
	// TODO: can shared pointers be used anywhere here without breaking everything?
	Vertex* cube_vertices = Primitives::GetCubeVertices();
	GLuint* cube_indices = Primitives::GetCubeIndices();
	Mesh cube_mesh(cube_vertices, 24, cube_indices, 36, GL_STATIC_DRAW, GL_TRIANGLES);

	int debug_mesh_shader = CreateShaderFromFiles("shaders/v_debug_mesh.glsl", "shaders/f_debug_mesh.glsl");
	int point_cloud_shader = CreateShaderFromFiles("shaders/v_point_cloud.glsl", "shaders/f_point_cloud.glsl");
	int perspective_mesh_shader = CreateShaderFromFiles("shaders/v_perspective_mesh.glsl", "shaders/f_perspective_mesh.glsl");

	// TODO: find better solution for storing constants like the kinect sensor dimensions
	SceneObject kinect_sensor("Kinect Sensor", cube_mesh, debug_mesh_shader);
	kinect_sensor.scale = glm::vec3(0.249f, 0.045f, 0.067f);
	debug_scene_objects.push_back(kinect_sensor);

	SceneObject world_screen("World Screen", cube_mesh, debug_mesh_shader);
	world_screen.position = glm::vec3(0.0f, 0.0f, 0.0f);
	world_screen.scale = glm::vec3(0.40f, 0.225f, 0.05f);
	world_screen.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	debug_scene_objects.push_back(world_screen);

	SceneObject perspective_camera("Perspective Camera", cube_mesh, debug_mesh_shader);
	perspective_camera.position = perspective_renderer->camera.position;
	perspective_camera.scale = glm::vec3(0.1f);
	debug_scene_objects.push_back(perspective_camera);

	// hardcoded values, replace with getting depth frame dimensions from sensor
	Vertex* depth_vertices = new Vertex[512 * 424];
	Mesh depth_mesh(depth_vertices, 512 * 424, nullptr, 0, GL_DYNAMIC_DRAW, GL_POINTS);
	SceneObject depth_point_cloud("Depth Point Cloud", depth_mesh, point_cloud_shader);
	debug_renderer->depth_point_cloud = depth_point_cloud;

	Vertex* eye_vertices = new Vertex[2 * BODY_COUNT];
	Mesh eye_mesh(eye_vertices, 2 * BODY_COUNT, nullptr, 0, GL_DYNAMIC_DRAW, GL_POINTS);
	SceneObject eye_point_cloud("Eye Point Cloud", eye_mesh, point_cloud_shader);
	debug_renderer->eye_point_cloud = eye_point_cloud;
}

void Application::MainLoop() 
{
	while (!(glfwWindowShouldClose(window.get()) || should_program_close)) {

		// Fps limit
		t_since_last_frame = glfwGetTime() - prev_frame_time;
		if (t_since_last_frame < frame_limit_ms) {
			Sleep(((frame_limit_ms - t_since_last_frame)*1000)/2);
			continue;
		}
		t_since_last_frame = 0;

		cam_hist_x.push_back(perspective_renderer->camera.position.x);
		cam_hist_y.push_back(perspective_renderer->camera.position.y);
		cam_hist_z.push_back(perspective_renderer->camera.position.z);
		if (cam_hist_x.size() > 200) {
			cam_hist_x.erase(cam_hist_x.begin());
			cam_hist_y.erase(cam_hist_y.begin());
			cam_hist_z.erase(cam_hist_z.begin());
		}

		frame_time = glfwGetTime() - prev_frame_time;
		prev_frame_time = glfwGetTime();

		// Update kinect data
		if (update_kinect_data) {
			kinect_sensor->GetFrame();
			if (debug_renderer->show_depth_point_cloud) {
				if (kinect_sensor->GetColourDepthPoints(debug_renderer->depth_point_cloud.mesh.vertices))
					debug_renderer->depth_point_cloud.mesh.Update();
			}
			// TODO: get coloured eye somehow is needed to compute camera position, seperate functionality?
			if (kinect_sensor->GetColouredEyePoints(debug_renderer->eye_point_cloud.mesh.vertices))
				debug_renderer->eye_point_cloud.mesh.Update();


			// Update perspective renderer properties
			// World screen is position 1 in vector TODO: maybe have hashmap for easy access?
			perspective_renderer->screen_model_matrix = debug_scene_objects[1].model_matrix;
			auto available_faces = kinect_sensor->GetAvailableFaces();
			for (int i = 0; i < BODY_COUNT; i++) {
				if (available_faces[i]) {
					perspective_renderer->camera.position.x = kinect_sensor->GetEyePositions(i)[0]->X;
					perspective_renderer->camera.position.y = kinect_sensor->GetEyePositions(i)[0]->Y;
					perspective_renderer->camera.position.z = kinect_sensor->GetEyePositions(i)[0]->Z;
				}
			}
			debug_scene_objects[2].position = perspective_renderer->camera.position;
			debug_scene_objects[2].rotation = perspective_renderer->camera.direction;
		}

		// Update the parent model matrix of every perspetive object
		/*for (SceneObject& scene_object : perspective_scene_objects) {
			scene_object.parent_model_matrix = debug_scene_objects[1].model_matrix;
		}*/

		// Draw
		debug_renderer->DrawFrame(debug_scene_objects, perspective_scene_objects);
		debug_renderer->UpdateMousePosition();
		
		//perspective_renderer->DrawFrame(perspective_scene_objects);
		// Calculate smoothing
		float smoothed_x = 0.0f;
		for (int i = 0; i <= cam_smoothing_lag; i++) {
			if (i == cam_hist_x.size()) {
				break;
			}
			smoothed_x += cam_hist_x[cam_hist_x.size() - 1 - i];
		}
		smoothed_x /= cam_smoothing_lag;
		float smoothed_y = 0.0f;
		for (int i = 0; i <= cam_smoothing_lag; i++) {
			if (i == cam_hist_y.size()) {
				break;
			}
			smoothed_y += cam_hist_y[cam_hist_y.size() - 1 - i];
		}
		smoothed_y /= cam_smoothing_lag;
		float smoothed_z = 0.0f;
		for (int i = 0; i <= cam_smoothing_lag; i++) {
			if (i == cam_hist_z.size()) {
				break;
			}
			smoothed_z += cam_hist_z[cam_hist_z.size() - 1 - i];
		}
		smoothed_z /= cam_smoothing_lag;

		// Update Unity camera position information
		shared_memory.UpdateCameraPosition(glm::vec3(smoothed_x, smoothed_y, smoothed_z));
		shared_memory.UpdatePerspectiveScreen(debug_scene_objects[1].position, debug_scene_objects[1].rotation, debug_scene_objects[1].scale);

		// Finally draw the UI
		DrawImGui();

		glfwSwapBuffers(window.get());
		glfwPollEvents();
	}

	glfwTerminate();
}