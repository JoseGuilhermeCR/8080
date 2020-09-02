#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#include "string_manipulation.h"

/* Given a line with only ONE quotation: 
 * ... "inside quote" ...
 * this function returns "inside quote" allocated in the heap.
 * If it's missing a quote or doesn't have any quotes at all
 * NULL will be returned. */
char *extract_from_quotes(const char *line, size_t size)
{
	size_t quote_begin = 0;
	size_t quote_size = 0;

	while (quote_begin != size && line[quote_begin] != '"')
		++quote_begin;

	/* Not able to find any quotes. */
	if (quote_begin == size)
		return NULL;

	/* Position it under the first character of the quotation. */
	++quote_begin;

	while (quote_begin + quote_size != size
			&& line[quote_begin + quote_size] != '"')
		++quote_size;

	/* Not able to find closing quote. */
	if (quote_begin + quote_size == size)
		return NULL;

	char *quotation = malloc(quote_size + 1);
	if (quotation)
	{
		memcpy(quotation, line + quote_begin, quote_size);
		quotation[quote_size] = '\0';
	}

	return quotation;
}

/* Returns the next word it finds starting from pos.
 * If pos is in the middle of a word, it will return the rest
 * of the current word. If there's no word, will return NULL. */
char *get_next_word(const char *line, size_t size, size_t pos)
{
	char *word = NULL;
	size_t start = 0;
	
	while (pos < size && isspace(*(line + pos)))
		++pos;

	if (pos == size)
		return NULL;

	start = pos;

	while (pos < size && !isspace(*(line + pos)))
		++pos;

	word = malloc(pos - start + 1);

	if (word)
	{
		memcpy(word, line + start, pos - start);
		word[pos - start] = '\0';
	}

	return word;
}

void cfprintf(FILE *stream, const char *color, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	fprintf(stream, color);
	vfprintf(stream, fmt, ap);
	fprintf(stream, ANSI_RESET);

	va_end(ap);
}
