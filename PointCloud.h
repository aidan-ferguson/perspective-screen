#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

struct Point {
	Point(glm::vec3 _position, glm::vec3 _colour) : position(_position), colour(_colour) {}

	glm::vec3 position;
	glm::vec3 colour;
};

class PointCloud
{
public:
	PointCloud(std::vector<Point> points);
	void UpdatePoints(std::vector<Point> points);
	void Draw();

private:
	unsigned int VAO = 0;
	unsigned int VBO = 0;

	int n_points = 0;
};

