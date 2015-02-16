#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

struct program
{
	program() { }

	program(const program&) = delete;
	program& operator=(const program&) = delete;

	void init()
	{ id_ = glCreateProgram(); }

	void attach_shader(GLenum type, const char *source) const;

	void link() const
	{ glLinkProgram(id_); }

	void use() const
	{ glUseProgram(id_); }

	GLint get_uniform_location(const char *name) const
	{ return glGetUniformLocation(id_, name); }

	void set_uniform_f(const char *name, GLfloat v0)
	{ glUniform1f(get_uniform_location(name), v0); }

	void set_uniform_f(const char *name, GLfloat v0, GLfloat v1)
	{ glUniform2f(get_uniform_location(name), v0, v1); }

	void set_uniform_f(const char *name, GLfloat v0, GLfloat v1, GLfloat v2)
	{ glUniform3f(get_uniform_location(name), v0, v1, v2); }

	void set_uniform_f(const char *name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
	{ glUniform4f(get_uniform_location(name), v0, v1, v2, v3); }

	GLuint id_;
};
