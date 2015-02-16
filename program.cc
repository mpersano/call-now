#ifndef RELEASE
#include <unistd.h>
#endif

#include "program.h"

void
program::attach_shader(GLenum type, const char *source) const
{
	GLuint shader_id;

	shader_id = glCreateShader(type);

	glShaderSource(shader_id, 1, &source, 0);
	glCompileShader(shader_id);

#ifndef RELEASE
	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);

	if (!status) {
		static char log_buffer[8192];
		GLint log_size;
		glGetShaderInfoLog(shader_id, sizeof(log_buffer), &log_size, log_buffer);
		write(STDERR_FILENO, log_buffer, log_size);
	}
#endif

	glAttachShader(id_, shader_id);
}
