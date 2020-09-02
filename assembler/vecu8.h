#ifndef __VECU8_H__
#define __VECU8_H__

#include <stddef.h>
#include <stdbool.h>

struct vecu8 {
	unsigned char *elements;
	size_t size;
	size_t capacity;
};

struct vecu8 *vecu8_create(size_t capacity);
void vecu8_destroy(struct vecu8 *vec);
bool vecu8_grow(struct vecu8 *vec);

void vecu8_push_back(struct vecu8 *vec, unsigned char u);
unsigned char *vecu8_at(const struct vecu8 *vec, size_t pos);

#endif
