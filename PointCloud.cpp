#include "PointCloud.h"

PointCloud::PointCloud(std::vector<Point> points)
{
	n_points = points.size();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, colour));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void PointCloud::UpdatePoints(std::vector<Point> points)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	void* vbo_buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(vbo_buffer, points.data(), points.size() * sizeof(Point));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void PointCloud::Draw()
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, n_points);
}
