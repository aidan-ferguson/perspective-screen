#include "PerspectiveRenderer.h"

PerspectiveRenderer::PerspectiveRenderer()
{
	screen_quad_shader = CreateShaderFromFiles("shaders/v_screen_quad.glsl", "shaders/f_screen_quad.glsl");
	screen_quad_mesh = Mesh(screen_quad_vertices, 4, screen_quad_indices, 6, GL_DYNAMIC_DRAW, GL_TRIANGLES);

	Vertex* quad_vertices = Primitives::GetQuadVertices();
	GLuint* quad_indices = Primitives::GetQuadIndices();
	Mesh screen_mesh(quad_vertices, 4, quad_indices, 6, GL_STATIC_DRAW, GL_TRIANGLES);

	camera.position = glm::vec3(0, 0, 2);
	camera.yaw = 270.0f;
	camera.UpdateDirection();
	// Human eye fov TODO: split horizontal and vertical fov?
	camera.fov = 150.0f;

	// Multisample frame buffer
	CreateMultisampleFrameBuffer(world_fb, world_rgb_texture, world_depth_texture, multisampling_n_samples, renderer_size.x, renderer_size.y);
	// Intermediate framebuffer (needed to convert from multisample texture to rgb texture)
	CreateFramebuffer(intermediate_fb, intermediate_texture, intermediate_depth_texture, renderer_size.x, renderer_size.y);
	// Final screen quad framebuffer
	CreateFramebuffer(screen_fb, screen_texture, screen_depth_texture, window_size.x, window_size.y);
}

void PerspectiveRenderer::Resize(float x, float y)
{
	window_size = glm::vec2(x, y);
	renderer_size = glm::vec2(x * resolution_scalar, y * resolution_scalar);

	ResizeMultisampleFrameBuffer(world_rgb_texture, world_depth_texture, multisampling_n_samples, renderer_size.x, renderer_size.y);
	ResizeFramebuffer(intermediate_texture, intermediate_depth_texture, renderer_size.x, renderer_size.y);
	ResizeFramebuffer(screen_texture, screen_depth_texture, renderer_size.x, renderer_size.y);
}

glm::vec2 PerspectiveRenderer::WorldToScreenSpace(glm::mat4 model_matrix, glm::vec3 object_point)
{
	glm::vec4 ndc;
	glm::vec2 screen_position;

	int width = renderer_size.x, height = renderer_size.y;

	ndc = camera.GetProjectionMatrix(width, height) * camera.GetViewMatrix() * model_matrix * glm::vec4(object_point, 1.0f);
	ndc = ndc / ndc.w;
	screen_position.x = ((float)((width / 2 * ndc.x) + (0 + width / 2))) / (float)width;
	screen_position.y = ((float)((height / 2 * ndc.y) + (0 + height / 2))) / (float)height;

	return screen_position;
}

// Calculates the required FOV of the camera for maximum resolution without clipping out the world screen
float PerspectiveRenderer::CalculateMinimumViableFov()
{
	// Using dot product, calculate the vertical angle from the cameras perspective and the horizontal,
	//  then return the minimum of the two
	glm::vec3 cam_to_topleft = screen_top_left - camera.position;
	glm::vec3 cam_to_topright = screen_top_right - camera.position;
	glm::vec3 cam_to_bottomleft = screen_bottom_left - camera.position;

	// Dot product between horizontal and vertical vectors
	float horizontal_dot = glm::dot(cam_to_topleft, cam_to_topright);
	float vertical_dot = glm::dot(cam_to_topleft, cam_to_bottomleft);

	// Calculate angle in radians
	float horizontal_angle = acos(horizontal_dot/(glm::length(cam_to_topleft) * glm::length(cam_to_topright)));
	float vertical_angle = acos(vertical_dot / (glm::length(cam_to_topleft) * glm::length(cam_to_bottomleft)));

	// Convert to degrees
	horizontal_angle = horizontal_angle * (180.0 / M_PI);
	vertical_angle = vertical_angle * (180.0 / M_PI);

	return std::max(horizontal_angle, vertical_angle);
}


void PerspectiveRenderer::DrawFrame(std::vector<SceneObject>& scene_objects)
{
	// Draw scene to framebuffer
	glViewport(0, 0, renderer_size.x, renderer_size.y);
	glBindFramebuffer(GL_FRAMEBUFFER, world_fb);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Make camera look at screen
	camera.direction = glm::normalize(glm::vec3(screen_model_matrix[3]) - camera.position);

	// Calculate minimum viable fov
	camera.fov = CalculateMinimumViableFov();

	glm::mat4 view_matrix = camera.GetViewMatrix();
	glm::mat4 projection_matrix = camera.GetProjectionMatrix(renderer_size.x, renderer_size.y);
	for (SceneObject& scene_obj : scene_objects) {
		scene_obj.Draw(view_matrix, projection_matrix);
	}

	// Resolve multisample framebuffer and copy it into the texture of the intermediate framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, world_fb);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediate_fb);
	glBlitFramebuffer(0, 0, renderer_size.x, renderer_size.y, 0, 0, renderer_size.x, renderer_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Draw cut-out quad to framebuffer for access later by imgui
	glBindFramebuffer(GL_FRAMEBUFFER, screen_fb);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update screen quad texture coordinates to cut out the "screen" portion of the framebuffer
	screen_quad_mesh.vertices[0].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(-0.5f, 0.5f, 0.0f));
	screen_quad_mesh.vertices[1].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(0.5f, 0.5f, 0.0f));
	screen_quad_mesh.vertices[2].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(-0.5f, -0.5f, 0.0f));
	screen_quad_mesh.vertices[3].texture_coordinate = WorldToScreenSpace(screen_model_matrix, glm::vec3(0.5f, -0.5f, 0.0f));
	screen_quad_mesh.Update();

	glUseProgram(screen_quad_shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, intermediate_texture);
	SetUniformInt(screen_quad_shader, "fb_texture", 0);
	screen_quad_mesh.Draw();
}
