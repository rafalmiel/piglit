
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

#define QUERY_EXPECTED_RESULT 2600

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

static GLuint
generate_query_result(void)
{
	GLuint queryId;

	piglit_ortho_projection(piglit_width, piglit_height, GL_FALSE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glGenQueries(1, &queryId);

	glColor3ub(0xff, 0x00, 0x00);
	draw_box(20.0f, 20.0f, 0.0f, 55.0f, 55.0f);
	/* Generate some occlusion tests */
	glBeginQuery(GL_SAMPLES_PASSED, queryId);
		glColor3ub(0xff, 0xff, 0x00);
		draw_box(10.0f, 10.0f, -0.3f, 75.0f, 75.0f);
	glEndQuery(GL_SAMPLES_PASSED);

	return queryId;
}

static void
reinit_query_buffer_data(unsigned size)
{
	glBufferData(GL_QUERY_BUFFER,
		     size,
		     NULL, GL_DYNAMIC_COPY);
}

static void
read_buffer_data(unsigned size, void *value)
{
	glGetBufferSubData(GL_QUERY_BUFFER,
			   0, size,
			   value);
}

static int
check_result(int value, const char *function)
{
	if (value != QUERY_EXPECTED_RESULT) {
		fprintf(stderr, "Function %s failed to store "
			"correct result in query buffer object. "
			"Got %d, expected %d\n",
			function, value, QUERY_EXPECTED_RESULT);
		return 0;
	}

	return 1;
}

static int
do_test(void)
{
	GLuint queryId;
	GLuint queryBuffer;
	GLint queryBufferResulti = 0;
	GLuint queryBufferResultui = 0;
	GLint64 queryBufferResulti64 = 0;
	GLuint64 queryBufferResultui64 = 0;
	int passed = 1;

	queryId = generate_query_result();

	glGenBuffers(1, &queryBuffer);
	glBindBuffer(GL_QUERY_BUFFER, queryBuffer);

/* Macro stores asynchronous query result into query buffer object
 * and checks if value stored in it is correct.
 * It is run for every variant of glGetQueryObject{ui64}v function */
#define TEST_QUERY_BUFFER(variant) \
	reinit_query_buffer_data(sizeof(queryBufferResult ## variant)); \
	glGetQueryObject ## variant ## v(queryId, GL_QUERY_RESULT, \
					 BUFFER_OFFSET(0)); \
	read_buffer_data(sizeof(queryBufferResult ## variant), \
			 &queryBufferResult ## variant); \
	passed &= check_result(queryBufferResult ## variant, \
			       "glGetQueryObject" #variant "v"); \

	TEST_QUERY_BUFFER(i)
	TEST_QUERY_BUFFER(ui)
	TEST_QUERY_BUFFER(i64)
	TEST_QUERY_BUFFER(ui64)

#undef TEST_QUERY_RESULT

	glDeleteQueries(1, &queryId);
	glDeleteBuffers(1, &queryBuffer);

	return passed;
}

void
piglit_init(int argc, char **argv)
{
	int test_pass;

	piglit_require_extension("GL_ARB_query_buffer_object");
	piglit_require_extension("GL_ARB_occlusion_query");

	glClearColor(0.0, 0.2, 0.3, 0.0);
	glClearDepth(1.0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	test_pass = do_test();

	piglit_report_result(test_pass ? PIGLIT_PASS : PIGLIT_FAIL);
}

enum piglit_result
piglit_display(void)
{
	return PIGLIT_FAIL;
}
