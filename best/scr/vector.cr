#ifndef VECTOR_CR
#define VECTOR_CR

#include "vector.h"
#include <stdint.h>
#include <stdarg.h>

struct Vector{
	const struct Class *class;
	#ifndef NDEBUG
	const struct Class *superClass;
	#endif
	const struct vector_vtable *vtable;
	void *array;
	uint64_t len;
	uint64_t allocated_len;
	uint64_t elem_size;
};

struct vector_vtable
{
	void *(*vector_insert_array_at)(void *self, uint64_t pos, const void *elem_array, uint64_t length);
	void *(*vector_insert_vector_at)(void *self, uint64_t posTo, const void *other, uint64_t posFrom, 
									 uint64_t length);
	void *(*vector_erase_at)(void *self, uint64_t pos, uint64_t length);
	bool (*vector_set_element_at)(void *self, const void *value, uint64_t pos);
	bool (*vector_get_element_at)(const void *self, void *element, uint64_t pos);
	bool (*vector_find_in_vector)(const void *self, uint64_t pos, const void *elem_array, uint64_t len, 
								  uint64_t *position);
	bool (*vector_find_vector_in_vector)(const void *self, uint64_t pos1, const void *other, uint64_t pos2, 
										 uint64_t len, uint64_t *position);
	void *(*vector_get_data)(const void *self);
};

//Function for child
void *vector_new(void *_self, const void *array, uint64_t size, uint64_t elem_size);

//Method prototypes
void *vector_constructor(void *_self, va_list *pa);
void *vector_destructor(void *_self);
void *vector_clone(const void *_self);
bool vector_equal(const void *self, const void *other);

void *vector_insert_array_at(void *self, uint64_t pos, const void *elem_array, uint64_t length);
void *vector_insert_vector_at(void *self, uint64_t posTo, const void *other, uint64_t posFrom, 
							  uint64_t length);
void *vector_erase_at(void *self, uint64_t pos, uint64_t length);
bool vector_set_element_at(void *self, const void *value, uint64_t pos);
bool vector_get_element_at(const void *self, void *element, uint64_t pos);
bool vector_find_in_vector(const void *self, uint64_t pos, const void *elem_array, uint64_t len, 
					       uint64_t *position);
bool vector_find_vector_in_vector(const void *self, uint64_t pos1, const void *other, uint64_t pos2, 
								  uint64_t len, uint64_t *position);
void *vector_get_data(const void *self);

#endif
