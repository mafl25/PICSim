#include "vector.cr"
#include "class.cr"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))


//Class description

static const struct vector_vtable vtable = 
{
	vector_insert_array_at,
	vector_insert_vector_at,
	vector_erase_at,
	vector_set_element_at,
	vector_get_element_at,
	vector_find_in_vector,
	vector_find_vector_in_vector,
	vector_get_data
};

static const struct Class _Vector =
{
	sizeof(struct Vector),
	vector_constructor,
	vector_destructor,
	vector_clone,
	vector_equal
};

const void *Vector = &_Vector;

//Private functions

static inline uint64_t nearest_two_power(uint64_t number)
{
	if (number > UINT64_MAX / 2) {
		return UINT64_MAX;
	} else {
		uint64_t n = 1;
		while (n < number)
			n = n << 1;
		return n;
	}
}

static inline void *vector_maybe_expand(struct Vector *vector, uint64_t len)
{
	assert(vector != NULL); 

	if (vector->len + len >= vector->allocated_len) {
		vector->allocated_len = nearest_two_power(vector->len + len + 1);
		void *array = realloc(vector->array, vector->allocated_len);
		NULL_RETURN(array)
		vector->array = array;
	}

	return vector->array;
}

//To help with child classes
void *vector_new(void *_self, const void *array, uint64_t size, uint64_t elem_size)
{
	struct Vector *self = _self;;
	assert(self);
	assert(size);
	assert(elem_size);
	
#ifndef NDEBUG
	self->superClass = Vector;
#endif
	self->len = 0;
	self->allocated_len = 0;
	self->array = NULL;
	self->elem_size = elem_size;
	self->vtable = &vtable;

	void *check = vector_maybe_expand(self, MAX(size * elem_size, 4));
	NULL_RETURN(check)
	self->allocated_len = self->allocated_len / self->elem_size;

	if(array)
		vector_insert_array_at(self, 0, array, size);

	return self;
}

//Class methods

void *vector_constructor(void *self, va_list *pa)
{
	const void *array = va_arg(*pa, const void *);
	uint64_t size = va_arg(*pa, uint64_t);
	uint64_t elem_size = va_arg(*pa, uint64_t);

	return vector_new(self, array, size, elem_size);
}

void *vector_destructor(void *_self)
{
	struct Vector *self = _self;
	assert(self);

	free(self->array);	
	
	return self;
}

void *vector_clone(const void *_self)
{
	const struct Vector *self = _self;
	struct Vector *clone = new(self->class, self->array, self->len, self->elem_size);

	return clone;
}

bool vector_equal(const void *_self, const void *_other)
{
	const struct Vector *self = _self;
	const struct Vector *other = _other;

	if (self->len != other->len)
		return false;

	if (self->elem_size != other->elem_size)
		return false;

	uint64_t i = self->len * self->elem_size;	

	return (memcmp(self->array, other->array, i) == 0) ? true : false;
}

void *vector_insert_array_at(void *_self, uint64_t pos, const void *elem_array, uint64_t len)
{
	struct Vector *self = _self; 

	if(!len)
		return self;

	if (pos > self->len)
		return self;

	pos *= self->elem_size;
	len *= self->elem_size;
	self->len *= self->elem_size;
	self->allocated_len *= self->elem_size;

	if (elem_array >= self->array && elem_array <= self->array + self->len) {
		uint64_t offset = elem_array - self->array;
		uint64_t precount = 0;

		void * check = vector_maybe_expand(self, len);
		NULL_RETURN(check)
		elem_array = self->array + offset;

      	if (pos < self->len)
        	memmove(self->array + pos + len, self->array + pos, self->len - pos);

      	if (offset < pos) {
          precount = MIN (len, pos - offset);
          memcpy (self->array + pos, elem_array, precount);
        }
		
      	if (len > precount)
        	memcpy (self->array + pos + precount, elem_array + precount + len, len - precount);
   		} else {
      		vector_maybe_expand (self, len);

      	if (pos < self->len)
        	memmove (self->array + pos + len, self->array + pos, self->len - pos);

      	if (len == 1)
        	*(uint8_t *)(self->array + pos) = *(uint8_t *)elem_array;
      	else
        	memcpy (self->array + pos, elem_array, len);
	}

 	self->len += len;
	self->len /= self->elem_size;
	self->allocated_len /= self->elem_size;

	return self;
}

void *vector_insert_vector_at(void *_self, uint64_t posTo, const void *_other, uint64_t posFrom, 
							  uint64_t length)
{
	struct Vector *self = _self;	
	const struct Vector *other = _other;	

	if (posFrom + length > other->len)
		length = other->len - posFrom;

	return self->vtable->vector_insert_array_at(self, posTo, other->array + (posFrom * self->elem_size), 
												length);
}

void *vector_erase_at(void *_self, uint64_t pos, uint64_t len)
{
	struct Vector *self = _self;

	if(!len || pos >= self->len)
		return self;

	pos *= self->elem_size;
	len *= self->elem_size;
	self->allocated_len *= self->elem_size;
	self->len *= self->elem_size;

	if (pos + len > self->len)
		len = self->len - pos;

	if (pos + len < self->len)
		memmove(self->array + pos, self->array + pos + len, self->len - (pos + len));
	
	self->len -= len;

	self->allocated_len /= self->elem_size;
	self->len /= self->elem_size;
	
	return self;
}

bool vector_set_element_at(void *_self, const void *_value, uint64_t pos)
{
	struct Vector *self = _self;
	const uint8_t *value = _value;

	if(pos >= self->len)
		return false;

	pos *= self->elem_size;
	int j;
	for (j = 0; j < self->elem_size; ++j)
		*(uint8_t *)(self->array + pos + j) = *(uint8_t *)(value + j);

	return true;
}

bool vector_get_element_at(const void *_self, void *_element, uint64_t pos)
{
	const struct Vector *self = _self;
	uint8_t *element = _element;

	if (pos >= self->len)
		return false;

	pos *= self->elem_size;
	int j;
	for (j = 0; j < self->elem_size; ++j)
		*(uint8_t *)(element + j) = *(uint8_t *)(self->array + pos + j);

	return true;
}

bool vector_find_in_vector(const void *_self, uint64_t pos, const void *elem_array, uint64_t len, 
						   uint64_t *position)
{
	const struct Vector *self = _self;
	
	if (len == 0) 
		return false;

	if (pos >= self->len) 
		return false;

	if (self->len < len)
		return false;

	uint64_t len1 = self->len * self->elem_size;
	len *= self->elem_size;
	pos *= self->elem_size;
	*position = 0;

	uint64_t j;
	for (j = pos; (j + len) <= len1; j += self->elem_size) {
		if (memcmp(self->array + j, elem_array, len) == 0) {
			*position = j / self->elem_size;
			return true;
		}
	}

	return false;
}

bool vector_find_vector_in_vector(const void *_self, uint64_t pos1, const void *_other, uint64_t pos2, 
								  uint64_t len, uint64_t *position)
{
	const struct Vector *self = _self;
	const struct Vector *other = _other;

	return self->vtable->vector_find_in_vector(self, pos1, other->array + (pos2 * self->elem_size), len, 
											   position);
}

void *vector_get_data(const void *_self)
{
	const struct Vector *self = _self;
	
	void *copy = malloc(self->len * self->elem_size);
	NULL_RETURN(copy) 

	return memcpy(copy, self->array, self->len * self->elem_size);
}

//Class functions

uint64_t get_length(const void *_self)
{
	const struct Vector *self = _self;

	assert(self);
	assert(self->superClass == Vector);

	return self->len;
}

void *insert_array_at(void *_self, uint64_t pos, const void *elem_array, uint64_t length)
{
	struct Vector *self = _self;

	assert(self);
	assert(self->superClass == Vector);
	assert(self->vtable->vector_insert_array_at);
	assert(elem_array);

	return self->vtable->vector_insert_array_at(self, pos, elem_array, length);
}

void *insert_vector_at(void *_self, uint64_t posTo, const void *_other, uint64_t posFrom, uint64_t length)
{
	struct Vector *self = _self;
	const struct Vector *other = _other;

	assert(other);
	assert(self);
	assert(self->superClass == Vector);
	assert(other->superClass == Vector);
	assert(self->class == other->class);
	assert(self->elem_size == other->elem_size);

	return self->vtable->vector_insert_vector_at(self, posTo, other, posFrom, length);
}

void *erase_at(void *_self, uint64_t pos, uint64_t length)
{
	struct Vector *self = _self;
	
	assert(self);
	assert(self->superClass == Vector);

	return self->vtable->vector_erase_at(self, pos, length);
}

bool set_element_at(void *_self, const void *value, uint64_t pos)
{
	struct Vector *self = _self;
	
	assert(self);
	assert(self->superClass == Vector);
	assert(value);

	return self->vtable->vector_set_element_at(self, value, pos);
}

bool get_element_at(const void *_self, void *element, uint64_t pos)
{
	const struct Vector *self = _self;
	
	assert(self);
	assert(self->superClass == Vector);
	assert(element);
	
	return self->vtable->vector_get_element_at(self, element, pos);
}

bool find_in_vector(const void *_self, uint64_t pos, const void *elem_array, uint64_t len, 	uint64_t *position)
{
	const struct Vector *self = _self;

	assert(self);
	assert(self->superClass == Vector);
	assert(elem_array);
	assert(position);

	return self->vtable->vector_find_in_vector(self, pos, elem_array, len, position);
}

bool find_vector_in_vector(const void *_self, uint64_t pos1, const void *_other, uint64_t pos2, uint64_t len, 
						   uint64_t *position)
{
	const struct Vector *self = _self;
	const struct Vector *other = _other;

	assert(other);
	assert(self);
	assert(self->superClass == Vector);
	assert(other->superClass == Vector);
	assert(self->class == other->class);
	assert(self->elem_size == other->elem_size);

	return self->vtable->vector_find_vector_in_vector(self, pos1, other, pos2, len, position);
}

void *get_data(const void *_self)
{
	const struct Vector *self = _self;

	assert(self);
	assert(self->superClass == Vector);

	return self->vtable->vector_get_data(self);	
}


#undef	MIN
#undef	MAX
