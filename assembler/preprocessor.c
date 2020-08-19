#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "preprocessor.h"
#include "string_manipulation.h"

/* TODO: Maybe we could also use a smaller buffer by removing
 * the space used to store empty space. We just need byte to
 * indicate a space. */

/* Erases all comments from the buffer. */
void preprocess_comments(struct fbuffer *buffer)
{
	char *comment_beg = NULL;
	while ((comment_beg = strstr(buffer->data, ";")))
	{
		while (comment_beg < buffer->data + buffer->size
				&& *comment_beg != '\n')
		{
			*comment_beg = ' ';
			++comment_beg;
		}
	}
}

bool preprocess(struct fbuffer *buffer)
{
	/* First, remove every comment from the buffer. */
	preprocess_comments(buffer);

	bool success = true;

	char directive_line[DIRECTIVE_LINE_SIZE];
	bool file_included = false;
	char *directive_beg = NULL;

	while ((directive_beg = strstr(buffer->data, "#")) && success)
	{
		const size_t directive_pos = directive_beg - buffer->data;

		/* Get the directive line and erase the line from the buffer. */
		unsigned i = 0; /* Also represents the size of the line. */
		while (i < DIRECTIVE_LINE_SIZE
				&& directive_beg + i < buffer->data + buffer->size
				&& directive_beg[i] != '\n')
		{
			directive_line[i] = directive_beg[i];
			directive_beg[i] = ' ';
			++i;
		}
		/* Properly end the directive line we copied. */
		directive_line[i] = '\0';

		/* Discover which directive it is */
		if (strncmp("#include", directive_line, 8) == 0)
		{
			success &= preprocess_include(buffer, directive_line, i, directive_pos);
			file_included = true;
		}

		/* If a file was included, it's comments need
		 *  to be removed as well. */
		if (file_included)
		{
			preprocess_comments(buffer);
			file_included = false;
		}
	}

	return success;
}

/*
 * #include "filename.asm"
 * If directive is in invalid format, file doesn't exist
 * or a new buffer couldn't be allocated to fit everything
 * this function returns FALSE.
 * */
bool preprocess_include(struct fbuffer *buffer, const char *line,
	       	size_t line_size, size_t pos)
{
	char *filename = extract_from_quotes(line, line_size);

	/* Invalid line and directive. */
	if (!filename)
		return false;

	bool included = fbuffer_include(buffer, filename, pos);

	free(filename);

	return included;
}

