#include "PerspectiveRenderer.h"

PerspectiveRenderer::PerspectiveRenderer(std::shared_ptr<KinectSensor> snsr)
{
	sensor = snsr;

	glfwInit();

	window = CreateKinectWindow(window_name, 800, 600);

	// Initliase GLAD so we have access to OpenGL functions
	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0);

	OpenGLSetup();
}

void PerspectiveRenderer::OpenGLSetup()
{
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.952f, 0.866f, 0.898f, 1.0f);
}

void PerspectiveRenderer::UpdateCamera()
{

}

void PerspectiveRenderer::MainLoop()
{
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3 
	};

	int basic_mesh_shader = CreateShaderFromFiles("v_basic_mesh.glsl", "f_basic_mesh.glsl");
	Mesh mesh(vertices, 12, indices, 6);

	camera.position = glm::vec3(0, 0, 2);
	camera.yaw = 270.0f;
	camera.UpdateDirection();
	std::shared_ptr<CameraSpacePoint> eye_pos = nullptr;

	while (!glfwWindowShouldClose(window.get())) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sensor->GetFrame();
		sensor->UpdateFaceData();
		int face_index = sensor->GetFirstNotableFaceIndex();
		if (face_index >= 0) {
			eye_pos = sensor->eyes[face_index * 2];
			Vector4 rotation = sensor->face_rotations[face_index];
			
			double x = rotation.x;
			double y = rotation.y;
			double z = rotation.z;
			double w = rotation.w;
	
			//float pitch, yaw, roll;
			//pitch = atan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z) / M_PI * 180.0;
			//yaw = asin(2 * (w * y - x * z)) / M_PI * 180.0;
			//roll = atan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z) / M_PI * 180.0;

			//camera.pitch = pitch;
			//camera.yaw = yaw;

			camera.position = glm::vec3(eye_pos->X, eye_pos->Y, eye_pos->Z);
			camera.UpdateDirection();
		}

		std::cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z  << std::endl;
		std::cout << camera.pitch << std::endl;

		glUseProgram(basic_mesh_shader);
		int width, height;
		glfwGetWindowSize(window.get(), &width, &height);
		SetUniformMat4(basic_mesh_shader, "projection_matrix", camera.GetProjectionMatrix(width, height));
		SetUniformMat4(basic_mesh_shader, "view_matrix", camera.GetViewMatrix());
		SetUniformMat4(basic_mesh_shader, "model_matrix", glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));
		mesh.Draw();

		glfwSwapBuffers(window.get());
		glfwPollEvents();
	}

	glfwTerminate();
}