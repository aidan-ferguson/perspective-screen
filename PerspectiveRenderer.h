#pragma once

#define _USE_MATH_DEFINES
#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <math.h>

#include "KinectSensor.h"
#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Primitives.h"
#include "SceneObject.h"

class PerspectiveRenderer
{
public:
	PerspectiveRenderer(std::shared_ptr<KinectSensor> snsr);
	GLuint GetFrameBufferTexture();
	void DrawFrame();
	void Resize(float x, float y);
	void SetScreenModelMatrix(glm::mat4 model_matrix);
	void SetSceneObjects(std::vector<SceneObject*> scene_objects);

private:
	void CreateFramebuffer(GLuint& fb_id, GLuint& fb_texture_id, GLuint& depth_texture_id);
	glm::vec2 WorldToScreenSpace(glm::mat4 model_matrix, glm::vec3 object_point);

	std::shared_ptr<KinectSensor> sensor;
	Camera camera;

	glm::vec2 window_size = glm::vec2(1, 1);

	double frame_time = 0, prev_frame_time = 0;
	double prev_mouse_x = 0, prev_mouse_y = 0;

	std::vector<SceneObject*> scene_objects;

	GLuint world_fb, world_texture, world_depth_texture, screen_fb, screen_texture, screen_depth_texture;

	// Calibration will need to be improved (currently screen will be inline with sensor
	glm::mat4 screen_model_matrix = glm::mat4(1.0f);
	glm::vec3 screen_top_left = glm::vec3(-0.5f, 0.5f, 0.0f);
	glm::vec3 screen_top_right = glm::vec3(0.5f, 0.5f, 0.0f);
	glm::vec3 screen_bottom_left = glm::vec3(-0.5f, -0.5f, 0.0f);
	glm::vec3 screen_bottom_right = glm::vec3(0.5f, -0.5f, 0.0f);

	int basic_mesh_shader;
	Mesh mesh;

	Vertex screen_quad_vertices[4] = {
		glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0, 1), glm::vec3(0, 0, 1),
		glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1, 1), glm::vec3(0, 0, 1),
		glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0, 0), glm::vec3(0, 0, 1),
		glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1, 0), glm::vec3(0, 0, 1),
	};
	unsigned int screen_quad_indices[6] = {
		0, 1, 2,
		1, 2, 3,
	};

	int screen_quad_shader;
	Mesh screen_quad_mesh;

	std::shared_ptr<CameraSpacePoint> eye_pos = nullptr;
};

