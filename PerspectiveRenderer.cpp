#include "PerspectiveRenderer.h"

PerspectiveRenderer::PerspectiveRenderer(std::shared_ptr<KinectSensor> snsr)
{
	sensor = snsr;

	Vertex* vertices = Primitives::GetCubeVertices();
	GLuint* indices = Primitives::GetCubeIndices();

	basic_mesh_shader = CreateShaderFromFiles("shaders/v_basic_mesh.glsl", "shaders/f_basic_mesh.glsl");
	mesh = Mesh(vertices, 24, indices, 36, GL_STATIC_DRAW);

	screen_quad_shader = CreateShaderFromFiles("shaders/v_screen_quad.glsl", "shaders/f_screen_quad.glsl");
	screen_quad_mesh = Mesh(screen_quad_vertices, 4, screen_quad_indices, 6, GL_DYNAMIC_DRAW);

	Vertex* quad_vertices = Primitives::GetQuadVertices();
	GLuint* quad_indices = Primitives::GetQuadIndices();
	Mesh screen_mesh(quad_vertices, 4, quad_indices, 6, GL_STATIC_DRAW);

	camera.position = glm::vec3(0, 0, 2);
	camera.yaw = 270.0f;
	camera.UpdateDirection();

	CreateFramebuffer(world_fb, world_texture, world_depth_texture);
	CreateFramebuffer(screen_fb, screen_texture, screen_depth_texture);
}

GLuint PerspectiveRenderer::GetFrameBufferTexture()
{
	return screen_texture;
}

// May need to create a new one if the window gets resized (using hardcoded values for now)
// Make more modular (also dupe code in debug renderer)
void PerspectiveRenderer::CreateFramebuffer(GLuint& fb_id, GLuint& fb_texture_id, GLuint& depth_texture_id)
{
	glGenFramebuffers(1, &fb_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

	glGenTextures(1, &fb_texture_id);
	glBindTexture(GL_TEXTURE_2D, fb_texture_id);

	// 0 in the last position indicates an empty image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &depth_texture_id);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_texture_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_texture_id);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fb_texture_id, 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void PerspectiveRenderer::Resize(float x, float y)
{
	window_size = glm::vec2(x, y);
	glBindTexture(GL_TEXTURE_2D, screen_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_size.x, window_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, screen_depth_texture);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_size.x, window_size.y);

	glBindTexture(GL_TEXTURE_2D, world_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_size.x, window_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, world_depth_texture);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_size.x, window_size.y);
}

void PerspectiveRenderer::SetScreenModelMatrix(glm::mat4 model_matrix)
{
	screen_model_matrix = model_matrix;
}

void PerspectiveRenderer::SetSceneObjects(std::vector<SceneObject*> scene_objects)
{
	this->scene_objects = scene_objects;
}

glm::vec2 PerspectiveRenderer::WorldToScreenSpace(glm::mat4 model_matrix, glm::vec3 object_point)
{
	glm::vec4 ndc;
	glm::vec2 screen_position;

	int width = window_size.x, height = window_size.y;

	ndc = camera.GetProjectionMatrix(width, height) * camera.GetViewMatrix() * model_matrix * glm::vec4(object_point, 1.0f);
	ndc = ndc / ndc.w;
	screen_position.x = ((float)((width / 2 * ndc.x) + (0 + width / 2))) / (float)width;
	screen_position.y = ((float)((height / 2 * ndc.y) + (0 + height / 2))) / (float)height;

	return screen_position;
}


void PerspectiveRenderer::DrawFrame()
{
	//sensor->GetFrame();
	//sensor->UpdateFaceData();
	/*int face_index = sensor->GetFirstNotableFaceIndex();
	if (face_index >= 0) {
		eye_pos = sensor->eyes[face_index * 2];
		Vector4 rotation = sensor->face_rotations[face_index];

		if (eye_pos) {
			camera.position = glm::vec3(eye_pos->X, eye_pos->Y, eye_pos->Z);
			camera.UpdateDirection();
		}
	}*/

	// Draw scene to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, world_fb);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view_matrix = camera.GetViewMatrix();
	glm::mat4 projection_matrix = camera.GetProjectionMatrix(window_size.x, window_size.y);
	for (SceneObject* scene_obj : scene_objects) {
		scene_obj->Draw(view_matrix, projection_matrix);
	}
	//glm::mat4 cube_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
	//cube_model = glm::scale(cube_model, glm::vec3(1.0f));
	//SetUniformMat4(basic_mesh_shader, "model_matrix", cube_model);
	//mesh.Draw();

	// Update screen quad texture coordinates to cut out the "screen" portion of the framebuffer
	screen_quad_vertices[0].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(-0.5f, 0.5f, 0.0f));
	screen_quad_vertices[1].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(0.5f, 0.5f, 0.0f));
	screen_quad_vertices[2].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(-0.5f, -0.5f, 0.0f));
	screen_quad_vertices[3].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(0.5f, -0.5f, 0.0f));
	screen_quad_mesh.Update(screen_quad_vertices);

	// Draw cut-out quad to framebuffer for access later by imgui
	glBindFramebuffer(GL_FRAMEBUFFER, screen_fb);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(screen_quad_shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, world_texture);
	SetUniformInt(screen_quad_shader, "fb_texture", 0);
	screen_quad_mesh.Draw();
}
