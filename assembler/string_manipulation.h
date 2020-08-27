#ifndef __STRING_MANIPULATION_H__
#define __STRING_MANIPULATION_H__

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

char *extract_from_quotes(const char *line, size_t size);
char *get_next_word(const char *line, size_t size, size_t pos);

#endif
