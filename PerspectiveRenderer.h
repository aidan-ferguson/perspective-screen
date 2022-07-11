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
#include "OpenGLUtility.h"

class PerspectiveRenderer
{
public:
	PerspectiveRenderer();
	void DrawFrame(std::vector<SceneObject>& scene_objects);
	void Resize(float x, float y);

	GLuint world_fb, world_rgb_texture, world_depth_texture, intermediate_fb, intermediate_texture, intermediate_depth_texture, screen_fb, screen_texture, screen_depth_texture;
	Camera camera;
	glm::vec2 window_size = glm::vec2(1, 1);
	glm::vec2 renderer_size = glm::vec2(1, 1);
	glm::mat4 screen_model_matrix = glm::mat4(1.0f);

private:
	glm::vec2 WorldToScreenSpace(glm::mat4 model_matrix, glm::vec3 object_point);
	float CalculateMinimumViableFov();

	const GLuint resolution_scalar = 1;
	const GLuint multisampling_n_samples = 3;
	
	// Calibration will need to be improved (currently screen will be inline with sensor)
	glm::vec3 screen_top_left = glm::vec3(-0.5f, 0.5f, 0.0f);
	glm::vec3 screen_top_right = glm::vec3(0.5f, 0.5f, 0.0f);
	glm::vec3 screen_bottom_left = glm::vec3(-0.5f, -0.5f, 0.0f);
	glm::vec3 screen_bottom_right = glm::vec3(0.5f, -0.5f, 0.0f);

	// Replace with scaled primitives
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
};

