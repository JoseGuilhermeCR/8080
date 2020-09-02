#include <stdio.h>
#include <stdlib.h>

#include "vecu8.h"

#define VECU8_DEFAULT_CAPACITY 256

/*
 * Creates a vector with given initial capacity.
 * If capacity is zero, a default value will be
 * used.
 * */
struct vecu8 *vecu8_create(size_t capacity)
{
	struct vecu8 *vec = malloc(sizeof(*vec));

	if (!vec)
		return NULL;

	if (capacity == 0)
		capacity = VECU8_DEFAULT_CAPACITY;

	vec->elements = malloc(sizeof(unsigned char) * capacity);

	if (!vec->elements)
	{
		free(vec);
		return NULL;
	}

	vec->size = 0;
	vec->capacity = capacity;

	return vec;
}

/*
 * Deletes a given vector and set it to NULL.
 * */ 
void vecu8_destroy(struct vecu8 *vec)
{
	free(vec->elements);
	free(vec);
	vec = NULL;
}

/* SHOULD NOT BE CALLED MANUALLY
 * Grows a vector when size reaches
 * capacity.
 * Returns whether it had success in resizing.
 * */
bool vecu8_grow(struct vecu8 *vec)
{
	vec->capacity *= 2;

	/* Hold old ptr to elements. */
	unsigned char *hold_ptr = vec->elements;

	vec->elements = realloc(vec->elements, vec->capacity);

	/* Realloc failed. Try allocating and moving elements. */
	if (!vec->elements)
	{
		vec->elements = malloc(sizeof(unsigned char) * vec->capacity);

		if (!vec->elements)
		{
			/* Just leave the vector as it is.
			 * It won't grow. */
			vec->elements = hold_ptr;
			return false;
		}

		/* Copy old elements to newly allocated memory. */
		for (size_t i = 0; i != vec->size; ++i)
			vec->elements[i] = hold_ptr[i];

		/* Deallocate old memory. */
		free(hold_ptr);
	}

	return true;
}

/* Pushes an element to the back of the vector. */
void vecu8_push_back(struct vecu8 *vec, unsigned char u)
{
	if (vec->size != vec->capacity || vecu8_grow(vec))
		vec->elements[vec->size++] = u;
}

/* Pops an element from the back of the vector. */
void vecu8_pop_back(struct vecu8 *vec)
{
	if (vec->size != 0)
		--vec->size;
}

/* Returns a pointer to the element at pos.
 * If pos >= vec->size, returns NULL. */
unsigned char *vecu8_at(const struct vecu8 *vec, size_t pos)
{
	if (pos < vec->size)
		return &vec->elements[pos];

	return NULL;
}
