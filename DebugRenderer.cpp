#include "DebugRenderer.h"

DebugRenderer::DebugRenderer(std::shared_ptr<KinectSensor> snsr)
{
	sensor = snsr;
	glfwInit();

	// TODO: get physical of the monitors attached and make screen that size automatically using glfwGetMonitorPhysicalSize()
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	window = CreateKinectWindow(window_name, mode->width, mode->height, false);
	glfwSetWindowPos(window.get(), 0, 0);
	glfwGetCursorPos(window.get(), &prev_mouse_x, &prev_mouse_y);
	glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetInputMode(window.get(), GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE); // for accurate mouse polling https://www.glfw.org/docs/3.3/input_guide.html#input_mouse_button

	// Initliase GLAD so we have access to OpenGL functions
	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0);
	OpenGLSetup();

	// Initalise the perspective renderer
	perspective_renderer = std::shared_ptr<PerspectiveRenderer>(new PerspectiveRenderer(sensor));

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	imgui_io = &ImGui::GetIO();
	imgui_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	imgui_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
	ImGui_ImplOpenGL3_Init(nullptr);
	// Setup Dear ImGui style
	ImGui::StyleColorsClassic();

	camera.position = glm::vec3(-2.0f, 0.5, 1.0f);
	camera.yaw = 0.0f;
	camera.pitch = -5.0f;
}

void DebugRenderer::OpenGLSetup()
{
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool DebugRenderer::IsKeyPressed(int key_code) {
	return (glfwGetKey(window.get(), key_code) == GLFW_PRESS);
}

void DebugRenderer::GetMouseDelta(double& d_x, double& d_y)
{
	double mouse_x, mouse_y;
	glfwGetCursorPos(window.get(), &mouse_x, &mouse_y);
 
	// Y axis is inverted so it is the other way around
	d_x = mouse_x - prev_mouse_x;
	d_y = prev_mouse_y - mouse_y;

	// Set the new previous positions
	prev_mouse_x = mouse_x;
	prev_mouse_y = mouse_y;
}

void DebugRenderer::HandleInput() {
	double d_x, d_y;
	GetMouseDelta(d_x, d_y);

	if (render_window_focused) {
		if (IsKeyPressed(GLFW_KEY_W)) {
			camera.position += camera.direction * camera.camera_speed * (float)frame_time;
		}
		if (IsKeyPressed(GLFW_KEY_S)) {
			camera.position -= camera.direction * camera.camera_speed * (float)frame_time;
		}
		if (IsKeyPressed(GLFW_KEY_A)) {
			camera.position -= glm::cross(camera.direction, camera.up) * camera.camera_speed * (float)frame_time;
		}
		if (IsKeyPressed(GLFW_KEY_D)) {
			camera.position += glm::cross(camera.direction, camera.up) * camera.camera_speed * (float)frame_time;
		}

		camera.yaw += d_x * camera.sensitivity;
		// Clamped to -89 and 89 because after that the image will flip
		camera.pitch = glm::clamp((float)(camera.pitch + d_y * camera.sensitivity), -89.0f, 89.0f);
	}
	camera.UpdateDirection();
}

// TODO: hardcoded values (fit to imgui window???)
void DebugRenderer::CreateFramebuffer()
{
	glGenFramebuffers(1, &fb_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

	glGenTextures(1, &fb_texture_id);
	glBindTexture(GL_TEXTURE_2D, fb_texture_id);

	// 0 in the last position indicates an empty image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &depth_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fb_texture_id, 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

// TODO: Clean up magic numbers, check on all HRESULT calls, ensure pointer managment
void DebugRenderer::MainLoop() 
{
	// points take the following format:
	//	each point has 6 floats: 3 for position and 3 for colour
	std::shared_ptr<float> depth_points(new float[512*424*6]);
	PointCloud depth_point_cloud(depth_points, 512*424, 1.0f);

	std::shared_ptr<float> eye_points(new float[BODY_COUNT * 6 * 2]);
	PointCloud eye_point_cloud(eye_points, BODY_COUNT, 10.0f);

	CreateFramebuffer();

	int point_cloud_shader = CreateShaderFromFiles("shaders/v_point_cloud.glsl", "shaders/f_point_cloud.glsl");
	bool show_point_cloud = true;
	bool show_eye_points = true;

	Vertex vertices[] = {
		// Front face
		glm::vec3(-0.5, 0.5, 0.5),  glm::vec2(0, 1), glm::vec3(0, 0, 1),
		glm::vec3(0.5, 0.5, 0.5),   glm::vec2(1, 1), glm::vec3(0, 0, 1),
		glm::vec3(-0.5, -0.5, 0.5), glm::vec2(0, 0), glm::vec3(0, 0, 1),
		glm::vec3(0.5, -0.5, 0.5),  glm::vec2(1, 0), glm::vec3(0, 0, 1),
		// Back face
		glm::vec3(-0.5, 0.5, -0.5), glm::vec2(1, 1), glm::vec3(0, 0, -1),
		glm::vec3(0.5, 0.5, -0.5),  glm::vec2(0, 1), glm::vec3(0, 0, -1),
		glm::vec3(-0.5, -0.5, -0.5),glm::vec2(1, 0), glm::vec3(0, 0, -1),
		glm::vec3(0.5, -0.5, -0.5), glm::vec2(0, 0), glm::vec3(0, 0, -1),
		// Top Face
		glm::vec3(-0.5, 0.5, 0.5),  glm::vec2(0, 1),  glm::vec3(0, 1, 0),
		glm::vec3(0.5, 0.5, 0.5),   glm::vec2(1, 1),  glm::vec3(0, 1, 0),
		glm::vec3(-0.5, 0.5, -0.5), glm::vec2(0, 0),  glm::vec3(0, 1, 0),
		glm::vec3(0.5, 0.5, -0.5),  glm::vec2(1, 0),  glm::vec3(0, 1, 0),
		// Bottom Face
		glm::vec3(-0.5, -0.5, 0.5),  glm::vec2(0, 1),  glm::vec3(0, -1, 0),
		glm::vec3(0.5, -0.5, 0.5),   glm::vec2(1, 1),  glm::vec3(0, -1, 0),
		glm::vec3(-0.5, -0.5, -0.5), glm::vec2(0, 0),  glm::vec3(0, -1, 0),
		glm::vec3(0.5, -0.5, -0.5),  glm::vec2(1, 0),  glm::vec3(0, -1, 0),
		// Left Face
		glm::vec3(-0.5, -0.5, -0.5),  glm::vec2(0, 0),  glm::vec3(-1, 0, 0),
		glm::vec3(-0.5, 0.5, -0.5),   glm::vec2(0, 1),  glm::vec3(-1, 0, 0),
		glm::vec3(-0.5, -0.5, 0.5),   glm::vec2(1, 0),  glm::vec3(-1, 0, 0),
		glm::vec3(-0.5, 0.5, 0.5),    glm::vec2(1, 1),  glm::vec3(-1, 0, 0),
		// Right Face
		glm::vec3(0.5, -0.5, 0.5),   glm::vec2(0, 0),  glm::vec3(1, 0, 0),
		glm::vec3(0.5, 0.5, 0.5),    glm::vec2(0, 1),  glm::vec3(1, 0, 0),
		glm::vec3(0.5, -0.5, -0.5),  glm::vec2(1, 0),  glm::vec3(1, 0, 0),
		glm::vec3(0.5, 0.5, -0.5),   glm::vec2(1, 1),  glm::vec3(1, 0, 0),
	};
	unsigned int indices[] = {
		// Front
		0, 3, 1,
		0, 2, 3,
		// Back
		4, 5, 7,
		4, 7, 6,
		// Top
		8, 9, 11,
		8, 11, 10,
		// Bottom
		12, 15, 13,
		12, 14, 15,
		// Left
		16, 19, 17,
		16, 18, 19,
		// Right
		20, 23, 21,
		20, 22, 23
	};

	int basic_mesh_shader = CreateShaderFromFiles("shaders/v_basic_mesh.glsl", "shaders/f_basic_mesh.glsl");
	Mesh kinect_mesh(vertices, 24, indices, 36, GL_STATIC_DRAW);
	glm::vec3 kinect_sensor_position = glm::vec3(0.0f, 0.0f, 0.0f);

	Mesh screen_quad_mesh(vertices, 4, indices, 6, GL_STATIC_DRAW);
	glm::vec3 world_screen_position = glm::vec3(0.0f, 0.0f, -0.5f);
	glm::vec3 world_screen_scale = glm::vec3(1.0f, 1.0f, 0.1f);

	while (!(glfwWindowShouldClose(window.get()) || program_should_close)) {
		frame_time = glfwGetTime() - prev_frame_time;
		prev_frame_time = glfwGetTime();

		HandleInput();


		glViewport(0, 0, prev_debug_render_window_sz.x, prev_debug_render_window_sz.y);
		glBindFramebuffer(GL_FRAMEBUFFER, fb_id);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(point_cloud_shader);
		SetUniformMat4(point_cloud_shader, "projection_matrix", camera.GetProjectionMatrix(prev_debug_render_window_sz.x, prev_debug_render_window_sz.y));
		SetUniformMat4(point_cloud_shader, "view_matrix", camera.GetViewMatrix());
		SetUniformMat4(point_cloud_shader, "model_matrix", glm::mat4(1.0f));

		if (sensor != nullptr) {
			sensor->GetFrame();
			if (show_point_cloud) {
				if (sensor->GetColourDepthPoints(depth_points)) {
					depth_point_cloud.UpdatePoints(depth_points);
				}
				depth_point_cloud.Draw();
			}

			if (show_eye_points) {
				if (sensor->GetColouredEyePoints(eye_points)) {
					eye_point_cloud.UpdatePoints(eye_points);
				}
				eye_point_cloud.Draw();
			}
		}

		glUseProgram(basic_mesh_shader);
		SetUniformMat4(basic_mesh_shader, "projection_matrix", camera.GetProjectionMatrix(prev_debug_render_window_sz.x, prev_debug_render_window_sz.y));
		SetUniformMat4(basic_mesh_shader, "view_matrix", camera.GetViewMatrix());

		glm::mat4 kinect_model_matrix = glm::mat4(1.0f);
		kinect_model_matrix = glm::translate(kinect_model_matrix, kinect_sensor_position);
		kinect_model_matrix = glm::scale(kinect_model_matrix, kinect_dimensions);
		SetUniformMat4(basic_mesh_shader, "model_matrix", kinect_model_matrix);
		kinect_mesh.Draw();

		glm::mat4 world_screen_matrix = glm::mat4(1.0f);
		world_screen_matrix = glm::translate(world_screen_matrix, world_screen_position);
		world_screen_matrix = glm::scale(world_screen_matrix, world_screen_scale);
		perspective_renderer->SetScreenModelMatrix(world_screen_matrix);
		SetUniformMat4(basic_mesh_shader, "model_matrix", world_screen_matrix);
		kinect_mesh.Draw();

		// Draw the perspective view into a framebuffer
		glViewport(0, 0, prev_perspective_render_window_sz.x, prev_perspective_render_window_sz.y);
		perspective_renderer->DrawFrame();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow();
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				ImGui::MenuItem("Exit", "ALT+F4", &program_should_close);
				ImGui::Separator();
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		{
			ImGui::Begin("test window");
			render_window_focused = false; // innocent until proven guilty
			// Check mouse click held and that window is focused
			if (glfwGetMouseButton(window.get(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && ImGui::IsWindowFocused()) {
				render_window_focused = true;
			}

			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x != prev_debug_render_window_sz.x || window_size.y != prev_debug_render_window_sz.y) {
				glViewport(0, 0, window_size.x, window_size.y);
				glBindTexture(GL_TEXTURE_2D, fb_texture_id);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_size.x, window_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
				glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_size.x, window_size.y);
			}
			ImGui::Image((void*)(intptr_t)fb_texture_id, ImVec2(window_size.x, window_size.y), ImVec2(0, 1), ImVec2(1, 0));
			prev_debug_render_window_sz = window_size;
			ImGui::End();
		}

		{
			ImGui::Begin("Perspective View");
			ImVec2 window_size = ImGui::GetWindowSize();
			if (window_size.x != prev_debug_render_window_sz.x || window_size.y != prev_debug_render_window_sz.y) {
				perspective_renderer->Resize(window_size.x, window_size.y);
			}
			ImGui::Image((void*)(intptr_t)perspective_renderer->GetFrameBufferTexture(), ImVec2(window_size.x, window_size.y), ImVec2(0, 1), ImVec2(1, 0));
			prev_perspective_render_window_sz = window_size;
			ImGui::End();
		}

		{
			ImGui::Begin("Demo window");
			ImGui::Checkbox("Render point cloud", &show_point_cloud);
			ImGui::Checkbox("Render eye points", &show_eye_points);
			ImGui::DragFloat("Screen position X", &world_screen_position.x, 0.005f);
			ImGui::DragFloat("Screen position Y", &world_screen_position.y, 0.005f);
			ImGui::DragFloat("Screen position Z", &world_screen_position.z, 0.005f);
			ImGui::DragFloat("Screen width", &world_screen_scale.x, 0.005f);
			ImGui::DragFloat("Screen height", &world_screen_scale.y, 0.005f);
			ImGui::End();
		}

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window.get());
		glfwPollEvents();
	}

	glfwTerminate();
}