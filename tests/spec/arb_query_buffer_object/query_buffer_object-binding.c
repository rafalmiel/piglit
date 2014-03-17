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

PIGLIT_GL_TEST_CONFIG_END

#if !defined GL_QUERY_BUFFER
#define GL_QUERY_BUFFER GL_QUERY_BUFFER_AMD
#define GL_QUERY_BUFFER_BINDING GL_QUERY_BUFFER_BINDING_AMD
#endif

void
piglit_init(int argc, char **argv)
{
	int test_pass = 1;
	GLuint queryBuffer;
	GLint queryBufferBinding = 0;

	piglit_require_extension("GL_ARB_query_buffer_object");

	glGenBuffers(1, &queryBuffer);
	glBindBuffer(GL_QUERY_BUFFER, queryBuffer);

	glGetIntegerv(GL_QUERY_BUFFER_BINDING, &queryBufferBinding);

	/* Check if query buffer object binding has correct non zero value */
	test_pass = (queryBufferBinding != 0);

	if (!test_pass) {
		fprintf(stderr, "glGetInteger(GL_QUERY_BUFFER_BINDING) failed\n");
	}

	piglit_report_result(test_pass ? PIGLIT_PASS : PIGLIT_FAIL);
}

enum piglit_result
piglit_display(void)
{
	return PIGLIT_FAIL;
}
