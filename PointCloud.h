#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

struct Point {
	Point(glm::vec3 _position, glm::vec3 _colour) : position(_position), colour(_colour) {}
	Point() = default;

	glm::vec3 position;
	glm::vec3 colour;
};

class PointCloud
{
public:
	PointCloud(std::shared_ptr<float> points, int n_pts, float sz_pts);
	void UpdatePoints(std::shared_ptr<float> points);
	void Draw();

private:
	unsigned int VAO = 0;
	unsigned int VBO = 0;

	int n_points = 0;
	int sz_arr = 0;
	float sz_points = 0;
};

