#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "file_buffer.h"
#include "error.h"

/* Reads a file and put it into the buffer. If any error occurs, NULL is returned */
/* TODO: Beautify error handling... less duplicated ifs... 
 *
 * BUG: If you pass in a directory it will complaing about allocating memory. That happens
 * because you can fopen apparently returns non NULL. It tries to allocate 0 bytes and
 * falls into !buffer->data if.*/
struct fbuffer *fbuffer_from(const char *path)
{
	struct fbuffer *buffer = malloc(sizeof(struct fbuffer));
	FILE *file = fopen(path, "r");
	size_t size;

	if (!buffer)
	{
		ALLOC_ERROR();
		return NULL;
	}

	if (!file)
	{
		ERROR("[FILE_BUFFER] Specified file (\"%s\") could not be opened.", path);
		free(buffer);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size = (size_t)ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer->data = malloc(size + 1);
	if (!buffer->data)
	{
		ALLOC_ERROR();
		free(buffer);
		fclose(file);
		return NULL;
	}

	buffer->path = path;
	buffer->size = size;
	
	fread(buffer->data, 1, buffer->size + 1, file);
	buffer->data[buffer->size] = '\0';

	fclose(file);

	return buffer;
}

void fbuffer_destroy(struct fbuffer *buffer)
{
	free(buffer->data);
	free(buffer);
	buffer = NULL;
}

/* Removes empty lines and unnecessary spaces
 * from the buffer. 
 * FIXME: Ugly code here, needs refactor! */
void fbuffer_optimize(struct fbuffer *buffer)
{
	char *data_tmp = malloc(buffer->size + 1);
	size_t new_size = 0;
	size_t i = 0;

	if (!data_tmp)
		goto error;
	
	/* Remove unnecessary spaces in every line. */
	bool in_quotes = false;
	while (i < buffer->size + 1)
	{
		if (!isspace(buffer->data[i]) || buffer->data[i] == '\n' || in_quotes)
		{
			data_tmp[new_size++] = buffer->data[i];

			if (buffer->data[i] == '"')
				in_quotes = !in_quotes;
		}
		else if (isspace(buffer->data[i]))
		{
			/* This is a space. Keep going until the next character
			 * is not a space so that we eliminate the extra spaces. */
			while (i < buffer->size
					&& (isspace(buffer->data[i + 1]) && buffer->data[i + 1] != '\n'))
				++i;
			data_tmp[new_size++] = buffer->data[i];
		}

		++i;
	}
	data_tmp[new_size] = '\0';
	
	free(buffer->data);
	buffer->data = data_tmp;
	buffer->size = new_size;

	/* Reset variables for next step. */
	data_tmp = malloc(buffer->size + 1);
	new_size = 0;
	i = 0;

	if (!data_tmp)
		goto error;

	bool empty_line = true;
	while (i < buffer->size + 1)
	{
		if (!isspace(buffer->data[i]))
			empty_line = false;

		if (!empty_line)
		{
			data_tmp[new_size++] = buffer->data[i];
		}

		/* After a new line, reset. */
		if (buffer->data[i] == '\n')
			empty_line = true;

		++i;
	}
	data_tmp[new_size] = '\0';

	free(buffer->data);
	buffer->data = data_tmp;
	buffer->size = new_size;

	return;
error:
	ALLOC_ERROR();
	return;
}

/* Includes another file in the buffer at given position and returns true.
 * If the file is not found or new buffer can't be allocated,
 * returns false.
 * 
 * TODO: Check if filename doesn't already contain a full path.
 * */
bool fbuffer_include(struct fbuffer *buffer, const char *filename, size_t pos)
{
	char *path = malloc(strlen(filename) + strlen(buffer->path));

	if (!path)
	{
		ALLOC_ERROR();
		return false;
	}

	const size_t last_dash = (size_t)(strrchr(buffer->path, '/') - buffer->path) + 1;
	memcpy(path, buffer->path, last_dash);
	memcpy(path + last_dash, filename, strlen(filename));

	FILE *file = fopen(path, "r");
	size_t size;

	free(path);

	if (!file)
	{
		ERROR("[FILE_BUFFER] Failed to open included file.\n");
		return false;
	}

	fseek(file, 0, SEEK_END);
	size = (size_t)ftell(file);
	fseek(file, 0, SEEK_SET);

	char *new_data = malloc(buffer->size + size + 1);
	if (!new_data)
	{
		ALLOC_ERROR();
		fclose(file);
		return false;
	}

	/* Copy pos elements to new data buffer. */
	memcpy(new_data, buffer->data, pos);
	/* Read size elements to new data buffer. */
	fread(new_data + pos, 1, size, file);
	/* Copy restant of old elements to new dat buffer. */
	memcpy(new_data + pos + size, buffer->data + pos, buffer->size - pos);

	new_data[buffer->size + size] = '\0';

	free(buffer->data);
	buffer->data = new_data;
	buffer->size += size + 1;

	fclose(file);

	return true;
}

/* Replaces all occurrences of string with replacement beginning from pos. */
bool fbuffer_replace(struct fbuffer *buffer, const char *string, const char *replacement, size_t pos)
{
	/* Won't replace two things that are equal. */
	if (strcmp(string, replacement) == 0)
	{
		WARNING("[FILE_BUFFER] Trying to replace equal strings.\n");
		return true;
	}

	const size_t str_len = strlen(string);
	const size_t rep_len = strlen(replacement);
	char *start = NULL;

	/* No need to allocate more memory. */
	if (rep_len <= str_len)
	{
		while ((start = strstr(buffer->data + pos, string)))
		{
			for (size_t i = 0; i < rep_len; ++i)
				start[i] = replacement[i];

			if (rep_len < str_len)
			{
				/* Fill the gap left by replacing a smaller string. */
				for (size_t i = rep_len; i < str_len; ++i)
					start[i] = ' ';

				/* Put every character in the line to the position
				 * they would have had if the replacement was actually
				 * the original string. */
				while (*(start + str_len) != '\n')
				{
					*(start + rep_len) = *(start + str_len);
					*(start + str_len) = ' ';
					++start;
				}
			}
		}

		return true;
	}
	else
	{
		/* Count the amount of replacements we'll have to make. */
		unsigned replacements = 0;
		start = buffer->data + pos;
		while (start + str_len < buffer->data + buffer->size
				&& (start = strstr(start, string)))
		{
			++replacements;
			start += str_len;
		}

		/* Allocate enough memory to fit everything. */
		const size_t new_size = buffer->size + (replacements * rep_len);
		char *new_data = malloc(new_size + 1);

		if (!new_data)
		{
			ALLOC_ERROR();
			return false;
		}

		char *new_data_off = new_data;
		char *last_start = buffer->data;

		start = buffer->data + pos;

		while (start + str_len < buffer->data + buffer->size
				&& (start = strstr(start, string)))
		{
			/* Write everything up to the replacement. */
			memcpy(new_data_off + (last_start - buffer->data), last_start, (size_t)(start - last_start));
			/* Write the replacement. */
			memcpy(new_data_off + (start - buffer->data), replacement, rep_len);

			/* Next time write to the position the replaced word ended. */
			new_data_off += (rep_len - str_len);

			start += str_len;
			last_start = start;
		}

		/* Copy rest of buffer. */
		memcpy(new_data_off + (last_start - buffer->data), last_start, buffer->size - (size_t)(last_start - buffer->data));

		new_data[new_size] = '\0';

		free(buffer->data);
		buffer->size = new_size;
		buffer->data = new_data;

		return true;
	}
}
