#ifndef MLIST
#define MLIST

#include <stdbool.h>
#include <stdint.h>

/*
 * Components of an MList.
 * Any data structure based of it must have this same scheme
 *struct MArray
 *{
 *	void *array;
 *	uint64_t len;
 *	uint64_t allocated_len;
 *};
 */
typedef struct MArray MArray;


void *m_array_new0(uint64_t size, uint64_t elem_size);
#define m_array_new(size, type) m_array_new0(size, sizeof(type))


uint64_t m_array_get_len(const void *array);


bool m_array_equal(const void *array1, const void *array2);


void *m_array_insert_pos_len(void *array, uint64_t pos, const void *elem_array, uint64_t len);
void *m_array_insert_len(void *array, const void *elem_array, uint64_t len);
void *m_array_insert_m_array(void *array1, uint64_t pos1, const void *array2, uint64_t pos2, uint64_t len);


void *m_array_erase_pos(void *array, uint64_t pos, uint64_t len);
void *m_array_erase(void *array, uint64_t len);


bool m_array_set_elem_pos(void *array, const void *element, uint64_t pos);
bool m_array_set_elem(void *array, const void *element);


bool m_array_get_elem_pos(void *array, void *element, uint64_t pos);
bool m_array_get_elem(void *array, void *element);


bool m_array_find_pos_len(void *array, uint64_t pos, uint64_t len1,
						  const void *elem_array, uint64_t len2, 
						  uint64_t *position);
bool m_array_find(void *array, const void *elem_array, uint64_t len,
				  uint64_t *position);
bool m_array_find_m_array_pos_len(void *array1, uint64_t pos1, uint64_t len1,
								  void *array2, uint64_t pos2, uint64_t len2, 
								  uint64_t *position);
bool m_array_find_m_array(void *array1, void *array2, uint64_t elem_size, uint64_t *position);


void *m_array_free(void *array, bool free_segment);


void m_array_begin(void *array);
void m_array_end(void *array);
uint64_t m_array_get_pos(void *array);
bool m_array_set_pos(void *array, uint64_t pos);



#endif
