/*
 * Copyright © 2014 Intel Corporation
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

/**
 * \file sub-overlap.c
 *
 * Clear multiple overlapping ranges of a buffer and check the result.
 */

#include "piglit-util-gl-common.h"
#include "common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 15;
	config.supports_gl_core_version = 31;

PIGLIT_GL_TEST_CONFIG_END


void
piglit_init(int argc, char **argv)
{
	bool pass = true;
	const int buffer_size = 1<<20;
	unsigned int buffer;
	static const char *const data_7f   = "\x7f\x7f\x7f\x7f";
	static const char *const data_zero = "\x00\x00\x00\x00";
	static const char *const data_one  = "\xff\xff\xff\xff";
	static const char *const data_init = "\xff\xff\xff\xff"
					     "\xff\xff\xff\xff"
					     "\x00\x00\x00\x00"
					     "\x00\x00\x00\x00"
					     "\x55\x55\x55\x55"
					     "\x55\x55\x55\x55"
					     "\xaa\xaa\xaa\xaa"
					     "\xaa\xaa\xaa\xaa"
					     "\xff\x00\xff\x00"
					     "\xff\x00\xff\x00"
					     "\x00\xff\x00\xff"
					     "\x00\xff\x00\xff"
					     "\x91\xcc\x45\x36"
					     "\xd3\xe4\xe3\x5b"
					     "\x79\x1e\x21\x39"
					     "\xa8\xfa\x69\x6a";


	piglit_require_extension("GL_ARB_clear_buffer_object");

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STREAM_READ);
	fill_array_buffer(64, data_init);

	/* Buffer contents:
	 * xx xx xx xx xx xx xx xx
	 */
	glClearBufferSubData(GL_ARRAY_BUFFER, GL_RGBA8, 1*buffer_size/8,
			buffer_size/4, GL_RGBA, GL_UNSIGNED_BYTE, data_7f);
	/* xx 7f 7f xx xx xx xx xx */
	glClearBufferSubData(GL_ARRAY_BUFFER, GL_RGBA8, 2*buffer_size/8,
			buffer_size/4, GL_RGBA, GL_UNSIGNED_BYTE, data_zero);
	/* xx 7f 00 00 xx xx xx xx */
	glClearBufferSubData(GL_ARRAY_BUFFER, GL_RGBA8, 3*buffer_size/8,
			buffer_size/4, GL_RGBA, GL_UNSIGNED_BYTE, data_one);
	/* xx 7f 00 FF FF xx xx xx */

	pass = check_array_buffer_sub_data(0, buffer_size/8,
			64, data_init) && pass;
	pass = check_array_buffer_sub_data(1*buffer_size/8, buffer_size/8,
			4, data_7f) && pass;
	pass = check_array_buffer_sub_data(2*buffer_size/8, buffer_size/8,
			4, data_zero) && pass;
	pass = check_array_buffer_sub_data(3*buffer_size/8, buffer_size/4,
			4, data_one) && pass;
	pass = check_array_buffer_sub_data(5*buffer_size/8, 3*buffer_size/8,
			64, data_init) && pass;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &buffer);

	pass = piglit_check_gl_error(GL_NO_ERROR) && pass;

	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);
}


enum piglit_result
piglit_display(void)
{
	return PIGLIT_PASS;
}

