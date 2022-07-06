#pragma once

#include <assert.h>
#include <glad/glad.h>

void CreateFramebuffer(GLuint& fb_id, GLuint& rgb_texture, GLuint& depth_texture, GLuint width, GLuint height);
void ResizeFramebuffer(GLuint rgb_texture, GLuint depth_texture, GLuint width, GLuint height);
void CreateMultisampleFrameBuffer(GLuint& fb_id, GLuint& ms_rgb_texture, GLuint& ms_depth_texture, GLuint n_samples, GLuint width, GLuint height);
void ResizeMultisampleFrameBuffer(GLuint ms_rgb_texture, GLuint ms_depth_texture, GLuint n_samples, GLuint width, GLuint height);