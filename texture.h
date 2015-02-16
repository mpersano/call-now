#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

struct texture
{
	texture() { }

	texture(const texture&) = delete;
	texture& operator=(const texture&) = delete;

	void init()
	{ glGenTextures(1, &id_); }

	void bind() const
	{ glBindTexture(GL_TEXTURE_2D, id_); }

	void load(int width, int height, const GLvoid *pixels);

	GLuint id_;
};
