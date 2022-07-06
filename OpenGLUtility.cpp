#include "OpenGLUtility.h"

void CreateFramebuffer(GLuint& fb_id, GLuint& rgb_texture, GLuint& depth_texture, GLuint width, GLuint height)
{
	glGenFramebuffers(1, &fb_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

	glGenTextures(1, &rgb_texture);
	glBindTexture(GL_TEXTURE_2D, rgb_texture);

	// 0 in the last position indicates an empty image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &depth_texture);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_texture);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_texture);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rgb_texture, 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void ResizeFramebuffer(GLuint rgb_texture, GLuint depth_texture, GLuint width, GLuint height)
{
	glBindTexture(GL_TEXTURE_2D, rgb_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_texture);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void CreateMultisampleFrameBuffer(GLuint& fb_id, GLuint& ms_rgb_texture, GLuint& ms_depth_texture,  GLuint n_samples, GLuint width, GLuint height)
{
	glGenFramebuffers(1, &fb_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb_id);

	glGenTextures(1, &ms_rgb_texture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ms_rgb_texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, n_samples, GL_RGB, width, height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ms_rgb_texture, 0);

	glGenRenderbuffers(1, &ms_depth_texture);
	glBindRenderbuffer(GL_RENDERBUFFER, ms_depth_texture);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, n_samples, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ms_depth_texture);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void ResizeMultisampleFrameBuffer(GLuint ms_rgb_texture, GLuint ms_depth_texture, GLuint n_samples, GLuint width, GLuint height)
{
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ms_rgb_texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, n_samples, GL_RGB, width, height, GL_TRUE);
	glBindRenderbuffer(GL_RENDERBUFFER, ms_depth_texture);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, n_samples, GL_DEPTH24_STENCIL8, width, height);
}
