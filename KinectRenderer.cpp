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

void KinectRenderer::MainLoop() 
{
	while (!glfwWindowShouldClose(window.get())) {
		frame_time = glfwGetTime() - prev_frame_time;
		prev_frame_time = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Input

		// Draw


		glfwSwapBuffers(window.get());
		glfwPollEvents();
	}

	glfwTerminate();
}