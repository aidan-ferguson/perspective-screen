#include "KinectRenderer.h"

KinectRenderer::KinectRenderer()
{
	glfwInit();

	window = CreateKinectWindow("Kinect", 800, 600);
	glfwGetCursorPos(window.get(), &prev_mouse_x, &prev_mouse_y);

	// Initliase GLAD so we have access to OpenGL functions
	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0);

	OpenGLSetup();

	
}

void KinectRenderer::OpenGLSetup()
{
	glPointSize(1.0f);
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

bool KinectRenderer::IsKeyPressed(int key_code) {
	return (glfwGetKey(window.get(), key_code) == GLFW_PRESS);
}

void KinectRenderer::GetMouseDelta(double& d_x, double& d_y)
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

void KinectRenderer::HandleInput() {
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
	if (IsKeyPressed(GLFW_KEY_ESCAPE)) {
		glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	
	double d_x, d_y;
	GetMouseDelta(d_x, d_y);

	camera.yaw += d_x * camera.sensitivity;
	// Clamped to -89 and 89 because after that the image will flip
	camera.pitch = glm::clamp((float)(camera.pitch + d_y * camera.sensitivity), -89.0f, 89.0f);
	camera.UpdateDirection();
}

void KinectRenderer::MainLoop() 
{
	// points take the following format:
	//	each point has 6 floats: 3 for position and 3 for colour
	std::shared_ptr<float> points(new float[1920*1080*6]);
	PointCloud point_cloud(points, 1920*1080);

	int point_cloud_shader = CreateShaderFromFiles("VertexShader.glsl", "FragmentShader.glsl");

	while (!glfwWindowShouldClose(window.get())) {
		frame_time = glfwGetTime() - prev_frame_time;
		prev_frame_time = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		HandleInput();

		glUseProgram(point_cloud_shader);
		int width, height;
		glfwGetWindowSize(window.get(), &width, &height);
		SetUniformMat4(point_cloud_shader, "projection_matrix", camera.GetProjectionMatrix(width, height));
		SetUniformMat4(point_cloud_shader, "view_matrix", camera.GetViewMatrix());
		SetUniformMat4(point_cloud_shader, "model_matrix", glm::mat4(1.0f));

		if (sensor.GetDepthPoints(points)) {
			point_cloud.UpdatePoints(points);
		}
		point_cloud.Draw();

		glfwSwapBuffers(window.get());
		glfwPollEvents();
	}

	glfwTerminate();
}