/**
 * Copyright © 2014 Rafał Mielniczuk
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 10;

	config.window_width = 180;
	config.window_height = 100;
	config.window_visual = PIGLIT_GL_VISUAL_RGB | PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_DEPTH;

PIGLIT_GL_TEST_CONFIG_END

#if !defined GL_QUERY_BUFFER
#define GL_QUERY_BUFFER GL_QUERY_BUFFER_AMD
#define GL_QUERY_BUFFER_BINDING GL_QUERY_BUFFER_BINDING_AMD
#endif

#define BUFFER_OFFSET(i) ((void*)NULL + (i))

static void
draw_box(float x, float y, float z, float w, float h)
{
	glBegin(GL_QUADS);
	glVertex3f(x, y, z);
	glVertex3f(x + w, y, z);
	glVertex3f(x + w, y + h, z);
	glVertex3f(x, y + h, z);
	glEnd();
}

static int
check_error(const char *func)
{
	if (piglit_check_gl_error(GL_INVALID_OPERATION)) {
		return 1;
	}

	fprintf(stderr,
		"Function %s did not generate expected "
		"GL_INVALID_OPERATION error\n", func);

	return 0;
}

static int
do_test(void)
{
	GLuint queryBuffer;
	GLuint queryId;
	int passed = 1;

	glClearColor(0.0, 0.2, 0.3, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glGenQueries(1, &queryId);
	glGenBuffers(1, &queryBuffer);
	/* Get query results to buffer object */
	glBindBuffer(GL_QUERY_BUFFER, queryBuffer);
	glBufferData(GL_QUERY_BUFFER, sizeof(GLuint64),
		     NULL, GL_DYNAMIC_COPY);

	glColor3ub(0xff, 0x00, 0x00);
	draw_box(20.0f, 20.0f, 0.0f, 55.0f, 55.0f);

	/* Generate some occlusion tests */
	glBeginQuery(GL_SAMPLES_PASSED, queryId);
		glColor3ub(0xff, 0xff, 0x00);
		draw_box(10.0f, 10.0f, 0.3f, 75.0f, 75.0f);
	glEndQuery(GL_SAMPLES_PASSED);

	if (!piglit_check_gl_error(GL_NO_ERROR)) {
		fprintf(stderr, "Unexpected error detected "
			"before calling glGetQueryObject{ui64v}\n");
		return 0;
	}

	/* Try to access beyond buffer size to generate error */
	glGetQueryObjectiv(queryId, GL_QUERY_RESULT, BUFFER_OFFSET(5));
	passed &= check_error("glGetQueryObjectiv");

	glGetQueryObjectuiv(queryId, GL_QUERY_RESULT, BUFFER_OFFSET(5));
	passed &= check_error("glGetQueryObjectuiv");

	glGetQueryObjecti64v(queryId, GL_QUERY_RESULT, BUFFER_OFFSET(5));
	passed &= check_error("glGetQueryObjecti64v");

	glGetQueryObjectui64v(queryId, GL_QUERY_RESULT, BUFFER_OFFSET(5));
	passed &= check_error("glGetQueryObjectui64v");

	return passed;
}

void
piglit_init(int argc, char **argv)
{
	int test_pass;

	piglit_require_extension("GL_ARB_query_buffer_object");
	piglit_require_extension("GL_ARB_occlusion_query");

	test_pass = do_test();

	piglit_report_result(test_pass ? PIGLIT_PASS : PIGLIT_FAIL);
}

enum piglit_result
piglit_display(void)
{
	return PIGLIT_FAIL;
}
