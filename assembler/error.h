#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdio.h>

#include "string_manipulation.h"

#define ALLOC_ERROR() ERROR("[8080_ASSEMBLER] Failed to allocate memory.\n")

/* ##__VA_ARGS__ is a (probably non-portable)
 * GCC extension that removes the trailling
 * comma when __VA_ARGS__ is empty.
 * */
#define ERROR(fmt,...) cfprintf(stderr, ANSI_RED, fmt, ##__VA_ARGS__)
#define WARNING(fmt,...) cfprintf(stderr, ANSI_YELLOW, fmt, ##__VA_ARGS__)
#define SUCCESS(fmt,...) cfprintf(stderr, ANSI_GREEN, fmt, ##__VA_ARGS__)

#endif
