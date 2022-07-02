#include "DebugRenderer.h"

DebugRenderer::DebugRenderer()
{
	// Initial camera position 
	camera.position = glm::vec3(-2.0f, 0.5, 1.0f);
	camera.yaw = 0.0f;
	camera.pitch = -5.0f;
	camera.UpdateDirection();

	// Populate the previous mouse positions
	UpdateMousePosition();

	// Create the fb render target
	CreateFramebuffer(fb_id, rgb_buffer, depth_buffer);
}

bool DebugRenderer::IsKeyPressed(GLFWwindow* window, int key_code) {
	return (glfwGetKey(window, key_code) == GLFW_PRESS);
}

void DebugRenderer::GetMouseDelta(GLFWwindow* window, double& d_x, double& d_y)
{
	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
 
	// Y axis is inverted so it is the other way around
	d_x = mouse_x - prev_mouse_x;
	d_y = prev_mouse_y - mouse_y;
}

// Must be called after GetMouseDelta in the application mainloop
void DebugRenderer::UpdateMousePosition()
{
	GLFWwindow* window = glfwGetCurrentContext();
	glfwGetCursorPos(window, &prev_mouse_x, &prev_mouse_y);
}

void DebugRenderer::HandleInput(float frame_time) {
	GLFWwindow* window = glfwGetCurrentContext();

	double d_x, d_y;
	GetMouseDelta(window, d_x, d_y);

	if (IsKeyPressed(window, GLFW_KEY_W)) {
		camera.position += camera.direction * camera.camera_speed * (float)frame_time;
	}
	if (IsKeyPressed(window, GLFW_KEY_S)) {
		camera.position -= camera.direction * camera.camera_speed * (float)frame_time;
	}
	if (IsKeyPressed(window, GLFW_KEY_A)) {
		camera.position -= glm::cross(camera.direction, camera.up) * camera.camera_speed * (float)frame_time;
	}
	if (IsKeyPressed(window, GLFW_KEY_D)) {
		camera.position += glm::cross(camera.direction, camera.up) * camera.camera_speed * (float)frame_time;
	}

	camera.yaw += d_x * camera.sensitivity;
	// Clamped to -89 and 89 because after that the image will flip
	camera.pitch = glm::clamp((float)(camera.pitch + d_y * camera.sensitivity), -89.0f, 89.0f);
	camera.UpdateDirection();
}

// TODO: Clean up magic numbers, check on all HRESULT calls, ensure pointer managment
void DebugRenderer::MainLoop() 
{
	// points take the following format:
	//	each point has 6 floats: 3 for position and 3 for colour
	/*
	std::shared_ptr<float> depth_points(new float[512*424*6]);
	PointCloud depth_point_cloud(depth_points, 512*424, 1.0f);

	std::shared_ptr<float> eye_points(new float[BODY_COUNT * 6 * 2]);
	PointCloud eye_point_cloud(eye_points, BODY_COUNT, 10.0f);

	CreateFramebuffer();

	int point_cloud_shader = CreateShaderFromFiles("shaders/v_point_cloud.glsl", "shaders/f_point_cloud.glsl");
	int basic_mesh_shader = CreateShaderFromFiles("shaders/v_basic_mesh.glsl", "shaders/f_basic_mesh.glsl");

	bool show_point_cloud = true;
	bool show_eye_points = true;

	Vertex* cube_vertices = Primitives::GetCubeVertices();
	GLuint* cube_indices = Primitives::GetCubeIndices();
	Mesh cube_mesh(cube_vertices, 24, cube_indices, 36, GL_STATIC_DRAW);

	std::vector<SceneObject*> debug_scene_objects;
	std::vector<SceneObject*> perspective_scene_objects;

	SceneObject kinect_sensor("Kinect Sensor", cube_mesh, basic_mesh_shader);
	kinect_sensor.scale = kinect_dimensions;
	debug_scene_objects.push_back(&kinect_sensor);

	SceneObject world_screen("World Screen", cube_mesh, basic_mesh_shader);
	world_screen.position = glm::vec3(0.0f, 0.0f, -0.5f);
	world_screen.scale = glm::vec3(1.0f, 1.0f, 0.05f);
	world_screen.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	debug_scene_objects.push_back(&world_screen);

	SceneObject cube("Cube", cube_mesh, basic_mesh_shader);
	cube.position = glm::vec3(0.0f, 0.0f, -2.0f);
	cube.scale = glm::vec3(0.1f);
	perspective_scene_objects.push_back(&cube);


	while (!(glfwWindowShouldClose(window.get()) || program_should_close)) {
		frame_time = glfwGetTime() - prev_frame_time;
		prev_frame_time = glfwGetTime();

		// Draw the perspective view into a framebuffer
		glViewport(0, 0, prev_perspective_render_window_sz.x, prev_perspective_render_window_sz.y);
		perspective_renderer->SetScreenModelMatrix(world_screen.model_matrix);
		perspective_renderer->SetSceneObjects(perspective_scene_objects);
		perspective_renderer->DrawFrame();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		

		glfwSwapBuffers(window.get());
		glfwPollEvents();
	}

	glfwTerminate();
	*/
}

void DebugRenderer::DrawFrame(std::vector<SceneObject>& debug_scene_objects, std::vector<SceneObject>& perspective_scene_objects)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fb_id);
	glViewport(0, 0, renderer_size.x, renderer_size.y);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view_matrix = camera.GetViewMatrix();
	glm::mat4 projection_matrix = camera.GetProjectionMatrix(renderer_size.x, renderer_size.y);

	bool show_point_cloud = true;
	for (SceneObject scene_obj : debug_scene_objects) {
		if (scene_obj.name == "depth_point_cloud" && !show_point_cloud)
			continue;
		if (scene_obj.name == "eye_point_cloud" && !show_point_cloud)
			continue;

		scene_obj.Draw(view_matrix, projection_matrix);
	}

	for (SceneObject scene_obj : perspective_scene_objects) {
		scene_obj.Draw(view_matrix, projection_matrix);
	}
}