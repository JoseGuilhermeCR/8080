#ifndef __STRING_MANIPULATION_H__
#define __STRING_MANIPULATION_H__

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define ANSI_RED "\033[38;2;255;0;0m"
#define ANSI_YELLOW "\033[38;2;255;255;0m"
#define ANSI_GREEN "\033[38;2;0;255;0m"
#define ANSI_RESET "\033[0m"

char *extract_from_quotes(const char *line, size_t size);
char *get_next_word(const char *line, size_t size, size_t pos);

void cfprintf(FILE *stream, const char *color, const char* fmt, ...);

#endif
