#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL/SDL.h>

#include "program.h"

#define M_PI 3.14159265

namespace {

const char *bg_frag_shader =
	"void main(void) { float c = (1.25 - .0015*distance(gl_FragCoord.xy, vec2(550., 350.)))*(.9 + .1*step(20., mod(gl_FragCoord.x + gl_FragCoord.y, 40.))); gl_FragColor = vec4(.75*c, .75*c, c, 1); }";

const char *bg_vert_shader =
	"void main(void) { gl_Position = vec4(gl_Vertex.xy, -1, 1); }";

const char *scroller_frag_shader =
	"uniform sampler2D texture;\n"
	"void main(void) { gl_FragColor = texture2D(texture, gl_TexCoord[0].st); }";

const char *scroller_vert_shader =
	"uniform float time;\n"
	"void main(void) { gl_TexCoord[0] = gl_MultiTexCoord0 + vec4(.001*time, 0, 0, 0); gl_Position = vec4(gl_Vertex.xy, -1, 1); }";

const char *blob_frag_shader =
	"varying vec3 f_normal;\n"
	"void main(void) { float c = smoothstep(.5, 0, length(f_normal.xy)); c *= c; c += .1; gl_FragColor = vec4(c, c, c, 1); }";

const char *blob_vert_shader =
	"#define PI 3.14159265\n"
	"\n"
	"uniform float time;\n"
	"\n"
	"varying vec3 f_normal;\n"
	"\n"
	"vec3 eval_coord(float s, float t)\n"
	"{\n"
	"	float a = .001*time;\n"
	"	float b = 7. + 2.*sin(.005*time);\n"
	"	float offs =\n"
	"		(b + 4.*b*sin(3.*a + t))*sin(a + 5.*s) + \n"
	"		(b + 4.*b*sin(2.*a + 2.*s))*sin(a + 3.*t); \n"
	"\n"
	"	return (30. + offs)*vec3(-sin(s)*cos(t), -sin(s)*sin(t), -cos(s));\n"
	"}\n"
	"\n"
	"void main(void)\n"
	"{\n"
	"	float s = gl_Vertex.y*PI;\n"
	"	float t = gl_Vertex.x*2.*PI;\n"
	"\n"
	"	vec3 p = eval_coord(s, t);\n"
	"\n"
	"	const float e = .01;\n"
	"\n"
	"	vec3 u = eval_coord(s + e, t) - p;\n"
	"	vec3 v = eval_coord(s, t + e) - p;\n"
	"\n"
	"	vec3 normal = normalize(cross(u, v));\n"
	"\n"
	"	gl_Position = gl_ModelViewProjectionMatrix*vec4(p, 1);\n"
	"	f_normal = gl_NormalMatrix*normal;\n"
	"}";

const char *text_frag_shader =
	"#define PI 3.14159265\n"
	"uniform float time;\n"
	"uniform sampler2D texture;\n"
	"void main(void) { vec2 p = gl_FragCoord.xy - vec2(320, 240); float l = length(p); "
	"if (l < 80. || l > 120.) discard; "
	"float v = (atan(p.x, p.y) + PI)/PI + 2.*sin(.0005*time); float u = (l - 80.)/40.; gl_FragColor = texture2D(texture, vec2(v, 1. - u)); }";

program bg_program, blob_program, scroller_program, text_program;

void
init()
{
	static const char *orig_texture[] {
		"***** ***** *     *         *   * ***** *   *  * * *",
		"*     *   * *     *         **  * *   * *   *  * * *",         
		"*     ***** *     *         * * * *   * * * *",
		"***** *   * ***** *****     *  ** ***** ** **  * * *" };

	static char texture_pixels[4*64] = {0};

	for (int i = 0; i < 4; i++) {
		char *p = &texture_pixels[i*64];

		for (const char *q = orig_texture[i]; *q; q++)
			*p++ = *q == '*' ? 128 : 0;
	}

	GLuint texture_id;

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 64, 4, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texture_pixels);

	bg_program.init();
	bg_program.attach_shader(GL_VERTEX_SHADER, bg_vert_shader);
	bg_program.attach_shader(GL_FRAGMENT_SHADER, bg_frag_shader);
	bg_program.link();

	scroller_program.init();
	scroller_program.attach_shader(GL_VERTEX_SHADER, scroller_vert_shader);
	scroller_program.attach_shader(GL_FRAGMENT_SHADER, scroller_frag_shader);
	scroller_program.link();

	blob_program.init();
	blob_program.attach_shader(GL_VERTEX_SHADER, blob_vert_shader);
	blob_program.attach_shader(GL_FRAGMENT_SHADER, blob_frag_shader);
	blob_program.link();

	text_program.init();
	text_program.attach_shader(GL_VERTEX_SHADER, bg_vert_shader);
	text_program.attach_shader(GL_FRAGMENT_SHADER, text_frag_shader);
	text_program.link();
}

void
redraw(unsigned now)
{
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	// background

	glDisable(GL_BLEND);

	bg_program.use();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(-1, -1);
	glVertex2f(-1, 1);
	glVertex2f(1, -1);
	glVertex2f(1, 1);
	glEnd();

	// scroller

	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

	scroller_program.use();
	scroller_program.set_uniform_f("time", now);

	glBegin(GL_TRIANGLE_STRIP);

	glTexCoord2f(0, 0);
	glVertex2f(-1, .05);

	glTexCoord2f(0, 1);
	glVertex2f(-1, -.05);

	glTexCoord2f(5, 0);
	glVertex2f(1, .05);

	glTexCoord2f(5, 1);
	glVertex2f(1, -.05);

	glEnd();

	// blob

	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.33, 1.33, -1, 1, 1, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -100);
	glRotatef(.01*now, 1, 0, 0);
	glRotatef(.03*now, 0, 1, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	blob_program.use();
	blob_program.set_uniform_f("time", now);

	static const int DENSITY = 64;

	for (int i = 0; i < DENSITY; i++) {
		float u = static_cast<float>(i)/DENSITY;

		glBegin(GL_TRIANGLE_STRIP);

		for (int j = 0; j <= DENSITY; j++) {
			const float v = static_cast<float>(j)/DENSITY;
			glVertex2f(u, v);
			glVertex2f(u + 1./DENSITY, v);
		}

		glEnd();
	}

	// text

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	text_program.use();
	text_program.set_uniform_f("time", now);

	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(-1, -1);
	glVertex2f(-1, 1);
	glVertex2f(1, -1);
	glVertex2f(1, 1);
	glEnd();
}

}

extern "C" {

void _start()
{
	SDL_SetVideoMode(640, 480, 0, SDL_OPENGL);

	init();

	for (;;) {
		redraw(SDL_GetTicks());

		SDL_GL_SwapBuffers();

		SDL_Event event;

		if (SDL_PollEvent(&event) &&
		  (event.type == SDL_QUIT || event.type == SDL_KEYDOWN))
			break;
	}

	asm("int $0x80" :: "a"(1), "b"(0));
}

}
