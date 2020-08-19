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
