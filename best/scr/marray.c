#include "marray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

struct MArray
{
	void *array;
	uint64_t len;
	uint64_t allocated_len;
	uint64_t position;
	uint32_t elem_size;
};

static inline uint64_t m_nearest_two_power(uint64_t number)
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

static inline void m_array_maybe_expand(struct MArray *array, uint64_t len)
{
	assert(array != NULL); 

	if (array->len + len >= array->allocated_len) {
		array->allocated_len = m_nearest_two_power(array->len + len + 1);
		array->array = realloc(array->array, array->allocated_len);
	}
}


void *m_array_new0(uint64_t size, uint64_t elem_size)
{
	struct MArray *array = malloc(sizeof(struct MArray));
	assert(array != NULL);//What to do?
	assert(size);
	assert(elem_size);

	array->len = 0;
	array->allocated_len = 0;
	array->position = 0;
	array->array = NULL;
	array->elem_size = elem_size;

	m_array_maybe_expand(array, MAX(size * elem_size, 4));
	array->allocated_len = array->allocated_len / array->elem_size;

	return array;
}


uint64_t m_array_get_len(const void *_array)
{
	const struct MArray *array = _array;

	if (array == NULL)
		return 0;

	return array->len;
}


bool m_array_equal(const void *_array1, const void *_array2)
{
	const struct MArray *array1 = _array1;
	const struct MArray *array2 = _array2;

	if (array1 == NULL || array2 == NULL)
		return false;

	if (array1->len != array2->len)
		return false;

	if (array1 == array2)
		return true;

	if (array1->elem_size != array2->elem_size)
		return false;
	
	uint64_t i = array1->len * array1->elem_size;

	return (memcmp(array1->array, array2->array, i) == 0) ? true : false;
}


void *m_array_insert_pos_len(void *_array, uint64_t pos, const void *elem_array, uint64_t len)
{
	struct MArray *array = _array;

	if (array == NULL || len == 0 || elem_array == NULL) 
		return array;

	pos *= array->elem_size;
	len *= array->elem_size;
	array->len *= array->elem_size;
	array->allocated_len *= array->elem_size;

	if (pos > array->len)
		return array;

	if (elem_array >= array->array && elem_array <= array->array + array->len) {
		uint64_t offset = elem_array - array->array;
		uint64_t precount = 0;

		m_array_maybe_expand(array, len);
		elem_array = array->array + offset;

      	if (pos < array->len)
        	memmove(array->array + pos + len, array->array + pos, array->len - pos);

      	if (offset < pos) {
          precount = MIN (len, pos - offset);
          memcpy (array->array + pos, elem_array, precount);
        }
		
      	if (len > precount)
        	memcpy (array->array + pos + precount, elem_array + precount + len, len - precount);
   		} else {
      		m_array_maybe_expand (array, len);

      	if (pos < array->len)
        	memmove (array->array + pos + len, array->array + pos, array->len - pos);

      	if (len == 1)
        	*(uint8_t *)(array->array + pos) = *(uint8_t *)elem_array;
      	else
        	memcpy (array->array + pos, elem_array, len);
	}

 	array->len += len;
	array->len /= array->elem_size;
	array->allocated_len /= array->elem_size;
	array->position = (pos + len) / array->elem_size;

	return array;
}

void *m_array_insert_len(void *_array, const void *elem_array, uint64_t len)
{
	MArray *array = _array;

	return m_array_insert_pos_len(array, array->position, elem_array, len);
}

void *m_array_insert_m_array(void *_array1, uint64_t pos1, const void *_array2, uint64_t pos2, uint64_t len)
{
	MArray *array1 = _array1;
	const MArray *array2 = _array2;
	assert(array1->elem_size == array2->elem_size);

	if (pos2 + len > array2->len)
		len = array2->len - pos2;

	return m_array_insert_pos_len(array1, pos1, array2->array + (pos2 * array1->elem_size), len);
}

void *m_array_erase_pos(void *_array, uint64_t pos, uint64_t len)
{
	struct MArray *array = _array;

	if (array == NULL || pos > array->len)
		return array;

	pos *= array->elem_size;
	len *= array->elem_size;
	array->allocated_len *= array->elem_size;
	array->len *= array->elem_size;

	if (pos + len > array->len)
		len = array->len - pos;

	if (pos + len < array->len)
		memmove(array->array + pos, array->array + pos + len, array->len - (pos + len));
	

	array->len -= len;

	array->allocated_len /= array->elem_size;
	array->len /= array->elem_size;
	array->position = pos / array->elem_size;

	return array;
}

void *m_array_erase(void *_array, uint64_t len)
{
	MArray *array = _array;

	return m_array_erase_pos(array, array->position, len);
}


bool m_array_set_elem_pos(void *_array, const void *_element, uint64_t pos)
{
	struct MArray *array = _array;
	const uint8_t *element = _element;

	if (array == NULL || element == NULL || array->elem_size == 0)
		return false;

	if(pos >= array->len)
		return false;

	pos *= array->elem_size;
	int j;
	for (j = 0; j < array->elem_size; ++j)
		*(uint8_t *)(array->array + pos + j) = *(uint8_t *)(element + j);

	array->position = (pos / array->elem_size) + 1;

	return true;
}

bool m_array_set_elem(void *_array, const void *element)
{
	MArray *array = _array;

	return m_array_set_elem_pos(array, element, array->position);
}


bool m_array_get_elem_pos(void *_array, void *_element, uint64_t pos)
{
	struct MArray *array = _array;
	uint8_t *element = _element;

	if (array == NULL || element == NULL || array->elem_size == 0)
		return false;

	if (pos >= array->len)
		return false;

	pos *= array->elem_size;
	int j;
	for (j = 0; j < array->elem_size; ++j)
		*(uint8_t *)(element + j) = *(uint8_t *)(array->array + pos + j);

	array->position = (pos / array->elem_size) + 1;

	return true;
}

bool m_array_get_elem(void *_array, void *element)
{
	MArray *array = _array;

	return m_array_get_elem_pos(array, element, array->position);
}


bool m_array_find_pos_len(void *_array, uint64_t pos, uint64_t len1,
						  const void *elem_array, uint64_t len2,
						  uint64_t *position)
{
	MArray *array = _array;

	if (array == NULL || elem_array == NULL)
		return false;

	if (len1 == 0 || len2 == 0) 
		return false;

	if (pos >= array->len) 
		return false;

	if (pos + len1 > array->len)
		len1 = array->len - pos;

	if (len1 < len2)
		return false;

	len1 *= array->elem_size;
	len2 *= array->elem_size;
	pos *= array->elem_size;
	*position = 0;

	uint64_t j;
	for (j = 0; (j + len2) < len1; j += array->elem_size) {
		if (memcmp(array->array + pos + j, elem_array, len2) == 0) {
			*position = (pos + j) / array->elem_size;
			array->position = *position + 1;
			return true;
		}
	}

	return false;
}

bool m_array_find(void *_array, const void *elem_array, uint64_t len,
				  uint64_t *position)
{
	MArray *array = _array;

	return m_array_find_pos_len(array, array->position, array->len, elem_array, len, position);
}

bool m_array_find_m_array_pos_len(void *_array1, uint64_t pos1, uint64_t len1,
								  void *_array2, uint64_t pos2, uint64_t len2,
								  uint64_t *position)
{
	MArray *array1 = _array1;
	MArray *array2 = _array2;
	assert(array1->elem_size == array2->elem_size);

	if (array1 == NULL || array2 == NULL)
		return false;

	if (len1 == 0 || len2 == 0) 
		return false;

	if (pos1 >= array1->len || pos2 >= array2->len) 
		return false;

	if (pos1 + len1 > array1->len)
		len1 = array1->len - pos1;

	if (pos2 + len2 > array2->len)
		len2 = array2->len - pos2;

	if (len1 < len2)
		return false;

	len1 *= array1->elem_size;
	len2 *= array1->elem_size;
	pos1 *= array1->elem_size;
	pos2 *= array1->elem_size;
	*position = 0;

	uint64_t j;
	for (j = 0; (j + len2) < len1; j += array1->elem_size) {
		if (memcmp(array1->array + pos1 + j, array2->array + pos2, len2) == 0) {
			*position = (pos1 + j) / array1->elem_size;
			array1->position = *position + 1;
			return true;
		}
	}
		
	return false;
}

bool m_array_find_m_array(void *_array1, void *_array2, uint64_t elem_size, uint64_t *position)
{
	MArray *array1 = _array1;
	MArray *array2 = _array2;

	return m_array_find_m_array_pos_len(array1, array1->position, array1->len, 
										array2, 0, array2->len, position);
}


void *m_array_free(void *_array, bool free_segment)
{
	struct MArray *array = _array;
	void *segment;

	if (free_segment) {
		free(array->array);
		segment = NULL;
	} else {
		segment = array->array;
	}

	free(array);

	return segment;
}


void m_array_begin(void *array)
{
	((MArray *)array)->position = 0;
}

void m_array_end(void *array)
{
	((MArray *)array)->position = ((MArray *)array)->len;
}

uint64_t m_array_get_pos(void *array)
{
	return ((MArray *)array)->position;
}

bool m_array_set_pos(void *_array, uint64_t pos)
{
	MArray *array = _array;

	if (pos > array->len) {
		array->position = array->len;
		return false;
	}

	array->position = pos;

	return true;
}

#undef	MIN
#undef	MAX
