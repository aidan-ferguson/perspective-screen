#include "PerspectiveRenderer.h"

PerspectiveRenderer::PerspectiveRenderer(std::shared_ptr<KinectSensor> snsr)
{
	sensor = snsr;

	glfwInit();

	window = CreateKinectWindow(window_name, 800, 600, true);
	glfwGetCursorPos(window.get(), &prev_mouse_x, &prev_mouse_y);

	// Initliase GLAD so we have access to OpenGL functions
	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0);

	OpenGLSetup();

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

	basic_mesh_shader = CreateShaderFromFiles("shaders/v_basic_mesh.glsl", "shaders/f_basic_mesh.glsl");
	mesh = Mesh(vertices, 24, indices, 36, GL_STATIC_DRAW);


	screen_quad_shader = CreateShaderFromFiles("shaders/v_screen_quad.glsl", "shaders/f_screen_quad.glsl");
	screen_quad_mesh = Mesh(screen_quad_vertices, 4, screen_quad_indices, 6, GL_DYNAMIC_DRAW);

	Vertex screen_vertices[] = {
		glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0, 1), glm::vec3(1, 0, 0),
		glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1, 1), glm::vec3(1, 0, 0),
		glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0, 0), glm::vec3(1, 0, 0),
		glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1, 0), glm::vec3(1, 0, 0),
	};
	Mesh screen_mesh(screen_vertices, 4, screen_quad_indices, 6, GL_STATIC_DRAW);

	camera.position = glm::vec3(0, 0, 2);
	camera.yaw = 270.0f;
	camera.UpdateDirection();

	CreateFramebuffer();
}

void PerspectiveRenderer::OpenGLSetup()
{
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

bool PerspectiveRenderer::IsKeyPressed(int key_code) {
	return (glfwGetKey(window.get(), key_code) == GLFW_PRESS);
}

void PerspectiveRenderer::GetMouseDelta(double& d_x, double& d_y)
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

// May need to create a new one if the window gets resized (using hardcoded values for now)
// Make more modular (also dupe code in debug renderer)
void PerspectiveRenderer::CreateFramebuffer()
{
	glGenFramebuffers(1, &fb_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

	glGenTextures(1, &fb_texture_id);
	glBindTexture(GL_TEXTURE_2D, fb_texture_id);

	// 0 in the last position indicates an empty image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint depth_buffer;
	glGenRenderbuffers(1, &depth_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fb_texture_id, 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

glm::vec2 PerspectiveRenderer::WorldToScreenSpace(glm::mat4 model_matrix, glm::vec3 object_point)
{
	glm::vec4 ndc;
	glm::vec2 screen_position;

	int width, height;
	glfwGetWindowSize(window.get(), &width, &height);

	ndc = camera.GetProjectionMatrix(width, height) * camera.GetViewMatrix() * model_matrix * glm::vec4(object_point, 1.0f);
	ndc = ndc / ndc.w;
	screen_position.x = ((float)((width / 2 * ndc.x) + (0 + width / 2))) / (float)width;
	screen_position.y = ((float)((height / 2 * ndc.y) + (0 + height / 2))) / (float)height;

	return screen_position;
}

void PerspectiveRenderer::DrawFrame()
{
	if (glfwWindowShouldClose(window.get())) {
		glfwTerminate();
	} else {
		frame_time = glfwGetTime() - prev_frame_time;
		prev_frame_time = glfwGetTime();
		
		if (IsKeyPressed(GLFW_KEY_F)) {
			manual_control = !manual_control;
		} else {
			sensor->GetFrame();
			sensor->UpdateFaceData();
			int face_index = sensor->GetFirstNotableFaceIndex();
			if (face_index >= 0) {
				eye_pos = sensor->eyes[face_index * 2];
				Vector4 rotation = sensor->face_rotations[face_index];

				camera.position = glm::vec3(eye_pos->X, eye_pos->Y, eye_pos->Z);
				camera.UpdateDirection();
			}

			//std::cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z  << std::endl;
			//std::cout << camera.pitch << std::endl;
		}
		

		// Draw scene to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fb_id);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(basic_mesh_shader);
		int width, height;
		glfwGetWindowSize(window.get(), &width, &height);
		SetUniformMat4(basic_mesh_shader, "projection_matrix", camera.GetProjectionMatrix(width, height));
		SetUniformMat4(basic_mesh_shader, "view_matrix", camera.GetViewMatrix());
		
		glm::mat4 cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
		cube_model = glm::scale(cube_model, glm::vec3(0.2f));
		SetUniformMat4(basic_mesh_shader, "model_matrix", cube_model);
		mesh.Draw();

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		SetUniformMat4(basic_mesh_shader, "model_matrix", screen_model_matrix);
		//screen_mesh.Draw();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Update screen quad texture coordinates to only show the "screen"
		if (!manual_control) {
			screen_quad_vertices[0].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(-0.5f, 0.5f, 0.0f));
			screen_quad_vertices[1].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(0.5f, 0.5f, 0.0f));
			screen_quad_vertices[2].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(-0.5f, -0.5f, 0.0f));
			screen_quad_vertices[3].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(0.5f, -0.5f, 0.0f));
		}
		else {
			screen_quad_vertices[0].texture_coordinate = glm::vec2(0, 1);
			screen_quad_vertices[1].texture_coordinate = glm::vec2(1, 1);
			screen_quad_vertices[2].texture_coordinate = glm::vec2(0, 0);
			screen_quad_vertices[3].texture_coordinate = glm::vec2(1, 0);
		}
		screen_quad_mesh.Update(screen_quad_vertices);

		

		// Draw framebuffer to screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(screen_quad_shader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fb_texture_id);
		SetUniformInt(screen_quad_shader, "fb_texture", 0);
		screen_quad_mesh.Draw();

		glfwSwapBuffers(window.get());
		glfwPollEvents();
	}
}