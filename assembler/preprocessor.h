#ifndef __PREPROCESSOR_H__
#define __PREPROCESSOR_H__

#include <stdbool.h>
#include "file_buffer.h"

/* These defines don't take into account the null byte at the end. */
#define DIRECTIVE_LINE_SIZE 299
#define DIRECTIVE_FILENAME_SIZE 255

void preprocess_comments(struct fbuffer *buffer);

bool preprocess(struct fbuffer *buffer);

bool preprocess_include(struct fbuffer *buffer, const char *line,
	       	size_t line_size, size_t pos);

bool preprocess_define(struct fbuffer *buffer, const char *line,
		size_t line_size, size_t pos);
#endif
