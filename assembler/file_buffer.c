#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_buffer.h"

/* Reads a file and put it into the buffer. If any error occurs, NULL is returned */
struct fbuffer *fbuffer_from(const char *filename)
{
	struct fbuffer *buffer = malloc(sizeof(struct fbuffer));
	FILE *file = fopen(filename, "r");
	size_t size;

	if (!buffer)
		return NULL;

	if (!file)
	{
		free(buffer);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer->data = malloc(size + 1);
	if (!buffer->data)
	{
		free(buffer);
		fclose(file);
		return NULL;
	}

	buffer->name = filename;
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

/* Includes another file in the buffer at given position and returns true.
 * If the file is not found or new buffer can't be allocated,
 * returns false.
 * */
bool fbuffer_include(struct fbuffer *buffer, const char *filename, size_t pos)
{
	FILE *file = fopen(filename, "r");
	size_t size;

	if (!file)
		return false;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *new_data = malloc(buffer->size + size + 1);
	if (!new_data)
	{
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
