#ifndef __FILE_BUFFER_H__
#define __FILE_BUFFER_H__

#include <stddef.h>
#include <stdbool.h>

/* File Buffer */
struct fbuffer {
	const char *name;
	char *data;
	size_t size;
};

struct fbuffer *fbuffer_from(const char *filename);

void fbuffer_destroy(struct fbuffer *buffer);

bool fbuffer_include(struct fbuffer *buffer, const char *filename, size_t pos);
bool fbuffer_replace(struct fbuffer *buffer, const char *string, const char *replacement, size_t pos);

#endif
