#ifndef VSTRING_CR
#define VSTRING_CR

#include "vstring.h"
#include "vector.cr"

struct VString
{
	struct Vector base;
#ifndef NDEBUG
	const struct Class *secondClass;
#endif
	uint64_t charNumber;
};

struct vstring_vtable
{
	struct vector_vtable base;
	void (*vstring_print)(const void *self, bool newLine);
	void *(*vstring_get_string_file)(void *self, FILE *fp);
	bool (*vstring_write_string_file)(const void *self, FILE *fp);
	uint64_t (*vstring_get_char_number)(const void *self);
	uint64_t (*vstring_get_word)(const void *self, void *word,  uint64_t pos);
	uint64_t (*vstring_get_word_in_line)(const void *self, void *word,  uint64_t pos);
	munichar (*vstring_get_unichar_at)(const void *self, uint64_t pos);
	bool (*vstring_get_next_line_pos)(const void *self, uint64_t start, uint64_t *position);
	bool (*vstring_replace_substring)(void *self, const void *original, const void *replacement);
	bool (*vstring_replace_word)(void *self, const void *original, const void *replacement);
	bool (*vstring_delete_substring)(void *self, const void *substring);
	bool (*vstring_delete_word)(void *self, const void *word);
	bool (*vstring_delete_line)(void *self, uint64_t pos);
	bool (*vstring_compare_string)(const void *self, uint64_t pos, const void *string, int64_t len);
	uint64_t (*vstring_string_to_hex)(const void *self);
	void *(*vstring_hex_to_string)(void *self, uint64_t number);
};

void *vstring_constructor(void *_self, va_list *pa);
void *vstring_insert_array_at(void *self, uint64_t pos, const void *elem_array, uint64_t length);
void *vstring_insert_vector_at(void *self, uint64_t posTo, const void *other, uint64_t posFrom, 
							  uint64_t length);
void *vstring_erase_at(void *self, uint64_t pos, uint64_t length);
bool vstring_set_element_at(void *self, const void *value, uint64_t pos);
bool vstring_get_element_at(const void *self, void *element, uint64_t pos);
bool vstring_find_in_vector(const void *self, uint64_t pos, const void *elem_array, uint64_t len, 
						   uint64_t *position);
bool vstring_find_vector_in_vector(const void *self, uint64_t pos1, const void *other, uint64_t pos2, 
			 					   uint64_t len, uint64_t *position);
void *vstring_get_data(const void *self);

void vstring_print(const void *self, bool newLine);
void *vstring_get_string_file(void *self, FILE *fp);
bool vstring_write_string_file(const void *self, FILE *fp);
uint64_t vstring_get_char_number(const void *self);
uint64_t vstring_get_word(const void *self, void *word, uint64_t pos);
uint64_t vstring_get_word_in_line(const void *self, void *word, uint64_t pos);
munichar vstring_get_unichar_at(const void *self, uint64_t pos);
bool vstring_get_next_line_pos(const void *self, uint64_t start, uint64_t *position);
bool vstring_replace_substring(void *self, const void *original, const void *replacement);
bool vstring_replace_word(void *self, const void *original, const void *replacement);
bool vstring_delete_substring(void *self, const void *substring);
bool vstring_delete_word(void *self, const void *word);
bool vstring_delete_line(void *self, uint64_t pos);
bool vstring_compare_string(const void *self, uint64_t pos, const void *string, int64_t len);
uint64_t vstring_string_to_hex(const void *self);
void *vstring_hex_to_string(void *self, uint64_t number);


#endif
