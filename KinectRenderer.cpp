#include "KinectRenderer.h"

KinectRenderer::KinectRenderer()
{
	glfwInit();

	window = CreateKinectWindow("Kinect", 800, 600);

	// Initliase GLAD so we have access to OpenGL functions
	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0);

	OpenGLSetup();
}

void KinectRenderer::OpenGLSetup()
{
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
}

bool KinectRenderer::IsKeyPressed(int key_code) {
	return (glfwGetKey(window.get(), key_code) == GLFW_PRESS);
}

void KinectRenderer::HandleInput() {
	/*
	if (IsKeyPressed(GLFW_KEY_W)) {
		cam->SetPosition(cam->GetPosition() + (cam->GetDirection() * cam->GetCameraSpeed() * (float)frame_time));
	}
	if (IsKeyPressed(GLFW_KEY_S)) {
		cam->SetPosition(cam->GetPosition() - (cam->GetDirection() * cam->GetCameraSpeed() * (float)frame_time));
	}
	if (IsKeyPressed(GLFW_KEY_A)) {
		cam->SetPosition(cam->GetPosition() - (glm::cross(cam->GetDirection(), cam->GetUpVector()) * cam->GetCameraSpeed() * (float)frame_time));
	}
	if (IsKeyPressed(GLFW_KEY_D)) {
		cam->SetPosition(cam->GetPosition() + (glm::cross(cam->GetDirection(), cam->GetUpVector()) * cam->GetCameraSpeed() * (float)frame_time));
	}*/
	if (IsKeyPressed(GLFW_KEY_ESCAPE)) {
		glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	/*
	double mouseDX = 0;
	double mouseDY = 0;

	if (GetMouseDelta(mouseDX, mouseDY)) {
		cam->SetYaw(cam->GetYaw() + mouseDX * cam->GetSensitivity());
		// Clamped to -89 and 89 because after that the image will flip
		cam->SetPitch(std::clamp((float)(cam->GetPitch() + mouseDY * cam->GetSensitivity()), -89.0f, 89.0f));
	}
	*/
}

void KinectRenderer::MainLoop() 
{
	while (!glfwWindowShouldClose(window.get())) {
		frame_time = glfwGetTime() - prev_frame_time;
		prev_frame_time = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		HandleInput();

		// Draw


		glfwSwapBuffers(window.get());
		glfwPollEvents();
	}

	glfwTerminate();
}