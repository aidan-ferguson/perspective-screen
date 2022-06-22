#include "PointCloud.h"

PointCloud::PointCloud(std::shared_ptr<float> points, int n_pts, float sz_pts)
{
	sz_points = sz_pts;
	n_points = n_pts;
	sz_arr = n_points * sizeof(float)*6;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sz_arr, points.get(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float) , (void*)offsetof(Point, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)offsetof(Point, colour));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void PointCloud::UpdatePoints(std::shared_ptr<float> points)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	void* vbo_buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(vbo_buffer, points.get(), sz_arr);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void PointCloud::Draw()
{
	glPointSize(sz_points);
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, n_points);
}
