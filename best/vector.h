#ifndef VECTOR_H
#define VECTOR_H
 
#include "class.h"
#include <stdint.h>
#include <stdbool.h>

struct Vector;
extern const void *Vector;

uint64_t get_length(const void *self);

void *insert_array_at(void *self, uint64_t pos, const void *elem_array, uint64_t length);
void *insert_vector_at(void *self, uint64_t posTo, const void *other, uint64_t posFrom, uint64_t length);

void *erase_at(void *self, uint64_t pos, uint64_t length);

bool set_element_at(void *self, const void *value, uint64_t pos);
bool get_element_at(const void *self, void *element, uint64_t pos);

bool find_in_vector(const void *self, uint64_t pos, const void *elem_array, uint64_t len, uint64_t *position);
bool find_vector_in_vector(const void *self, uint64_t pos1, const void *other, uint64_t pos2, uint64_t len, 
						   uint64_t *position);

void *get_data(const void *self);

#endif
