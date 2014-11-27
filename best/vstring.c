#include "vstring.cr"
#include "class.cr"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <glib.h>

#define BYTE_LEN_UTF8 7
#define NEW_LINE 0x0A

//Class description

static const struct vstring_vtable vtable = 
{
   {vstring_insert_array_at,
	vstring_insert_vector_at,
	vstring_erase_at,
	vstring_set_element_at,
	vstring_get_element_at,
	vstring_find_in_vector,
	vstring_find_vector_in_vector,
	vstring_get_data},
   vstring_print,
   vstring_get_string_file,
   vstring_write_string_file,
   vstring_get_char_number,
   vstring_get_word,
   vstring_get_word_in_line,
   vstring_get_unichar_at,
   vstring_get_next_line_pos,
   vstring_replace_substring,
   vstring_replace_word,
   vstring_delete_substring,
   vstring_delete_word,
   vstring_delete_line,
   vstring_compare_string,
   vstring_string_to_hex,
   vstring_hex_to_string
};

static const struct Class _VString =
{
	sizeof(struct VString),
	vstring_constructor,
	vector_destructor,
	vector_clone,
	vector_equal
};

const void *VString = &_VString;

//Private functions

static void set_last_elem_zero(struct VString *self)
{
	uint64_t j;
	for (j = 0; j < self->base.elem_size; ++j) 
		*((uint8_t *)(self->base.array + (self->base.len * self->base.elem_size) + j)) = 0;
}

static uint64_t get_byte_pos(const struct VString *self, uint64_t charPos)
{
	uint64_t position = 0;
	const char *end;
	uint64_t byteLen = 0;
	const char *string = self->base.array;

	uint64_t j;
	for (j = 0; j < charPos; ++j) {
		end = g_utf8_find_next_char(string, NULL);
		byteLen = end - string;
		position += byteLen;
		string += byteLen;
	}

	return position;
}

//Class Methods

void *vstring_constructor(void *_self, va_list *pa)
{
	struct VString *self = _self;
	const char *string = va_arg(*pa, const char *);
	int64_t size = va_arg(*pa, int64_t);
	uint64_t elem_size = sizeof(char);

	if(size < 0 && self)
		size = strlen(string) + 1;

	self = vector_new(self, NULL, size, elem_size);
	NULL_RETURN(self)

	self->base.vtable = (const struct vector_vtable *)(&vtable);
	self->secondClass = VString;
	self->charNumber = 0;

	if (string) {
		vstring_insert_array_at(self, 0, string, size);
		NULL_RETURN(self)
	}

	return self;
}

void *vstring_insert_array_at(void *_self, uint64_t pos, const void *_string, uint64_t length)
{
	struct VString *self = _self;
	const char *string = _string;
	const char *end;
	uint64_t byteLen = 0;
	pos = get_byte_pos(self, pos);

	uint64_t j;
	for (j = 0; j < length; ++j) {
		end = g_utf8_find_next_char(string, NULL);
		byteLen = end - string;
		if (g_utf8_validate(string, byteLen, NULL)) {
			self = vector_insert_array_at(self, pos, string, byteLen);
			NULL_RETURN(self)
			self->charNumber++;
		} else {
			break;
		}

		string += byteLen;
		pos += byteLen;
	}

	set_last_elem_zero(self);

	return self;
}

void *vstring_insert_vector_at(void *_self, uint64_t posTo, const void *_other, uint64_t posFrom, 
							  uint64_t length)
{
	struct VString *self = _self;
	const struct VString *other = _other;
	const char *end;
	uint64_t byteLen = 0;
	const char *string = other->base.array + get_byte_pos(other, posFrom);
	posTo = get_byte_pos(self, posTo);

	uint64_t j;
	for (j = 0; j < length; ++j) {
		end = g_utf8_find_next_char(string, NULL);
		byteLen = end - string;
		self = vector_insert_array_at(self, posTo, string, byteLen);
		NULL_RETURN(self)
		self->charNumber++;
		string += byteLen;
		posTo += byteLen;
	}

	set_last_elem_zero(self);

	return self;
}

void *vstring_erase_at(void *_self, uint64_t pos, uint64_t length)
{
	struct VString *self = _self;
	uint8_t charNum = get_char_number(self);


	if(pos >= charNum)
		return self;

	self->charNumber -= length;

	if(pos + length >= charNum){
		pos = get_byte_pos(self, pos);
		length = self->base.len - pos;
	} else {
		length = get_byte_pos(self, pos + length);
		pos = get_byte_pos(self, pos);
		length -= pos;
	}

	self = vector_erase_at(self, pos, length);

	set_last_elem_zero(self);

	return self;
}

bool vstring_set_element_at(void *_self, const void *_value, uint64_t pos)
{
	struct VString *self = _self;
	const char *value = _value;

	if(pos >= self->charNumber)
		return false;

	if (g_utf8_validate(value, -1, NULL)) {
		vstring_erase_at(self, pos, 1);
		return insert_array_at(self, pos, value, 1) != NULL;
	} 

	return false;
}

bool vstring_get_element_at(const void *_self, void *_element, uint64_t pos)
{
	const struct VString *self = _self;
	char *element = _element;
	const char *string = self->base.array;

	if (pos >= self->charNumber) {
		element[0] = 0;
		return false;
	} 

	pos = get_byte_pos(self, pos);
	string += pos;
	char *end = g_utf8_find_next_char(string, NULL);
	uint64_t length = end - string;

	int j;
	for (j = 0; j < length; ++j) 
		element[j] = string[j];
	
	element[j] = 0;

	return true;
}

bool vstring_find_in_vector(const void *_self, uint64_t pos, const void *elem_array, uint64_t len, 
						   uint64_t *position)
{
	const struct VString *self = _self;
	const char *string = elem_array;

	if(pos >= self->charNumber)
		return false;

	if(self->charNumber < len)
		return false;

	if(pos + len > self->charNumber)
		return false;

	pos = get_byte_pos(self, pos);

	uint64_t j;
	void *end = g_utf8_find_next_char(elem_array, NULL);;
	uint64_t byteLen = end - elem_array;
	for (j = 1; j < len; ++j) {
		end = g_utf8_find_next_char(elem_array + byteLen, NULL);;
		byteLen = end - elem_array;
	}

	if(!vector_find_in_vector(self, pos, string, byteLen, position))
		return false;

	end = g_utf8_find_next_char(self->base.array, NULL);
	byteLen = end - self->base.array;
	for (j = 0; *position >= byteLen; ++j) {
		end = g_utf8_find_next_char(self->base.array + byteLen, NULL);
		byteLen = end - self->base.array;
	}

	*position = j;

	return true;
}

bool vstring_find_vector_in_vector(const void *_self, uint64_t pos1, const void *_other, uint64_t pos2, 
			 					   uint64_t len, uint64_t *position)
{
	const struct VString *self = _self;
	const struct VString *other = _other;

	pos2 = get_byte_pos(other, pos2);

	return self->base.vtable->vector_find_in_vector(self, pos1, other->base.array + pos2, len, position);
}

void *vstring_get_data(const void *_self)
{
	const struct VString *self = _self;
	void *copy = malloc((self->base.len * self->base.elem_size) + self->base.elem_size);
	NULL_RETURN(copy) 

	copy = memcpy(copy, self->base.array, (self->base.len * self->base.elem_size) + self->base.elem_size);

	uint64_t j;
	for (j = 0; j < self->base.elem_size; ++j) 
		*((uint8_t *)(copy + (self->base.len * self->base.elem_size) + j)) = 0;

	return copy;
}

void vstring_print(const void *_self, bool newLine)
{
	const struct VString *self = _self;

	printf("%s", (char *)self->base.array);
	if (newLine) 
		printf("\n");
}

void *vstring_get_string_file(void *_self, FILE *fp)
{
	struct VString *self = _self;
	int character = fgetc(fp);
	char insert[BYTE_LEN_UTF8] = {0};
	
	vstring_erase_at(self, 0, self->base.len);

	int i = 0;
	int j = 0;
	while (character != EOF && i < 6) {
		insert[i] = character;
		i++;
		if(g_utf8_validate(insert, -1, NULL)) {
			self = vstring_insert_array_at(self, self->base.len, &insert, 1);
			NULL_RETURN(self)
			for (j = 0; j < i; ++j) 
				insert[j] = 0;
			i = 0;
		}
		character = fgetc(fp);
	}

	return self;
}

bool vstring_write_string_file(const void *_self, FILE *fp)
{
	const struct VString *self = _self;

	uint64_t j;
	for (j = 0; j < self->base.len; ++j) {
		if (fputc(((unsigned char*)self->base.array)[j], fp) == EOF) 
			return false;
	}

	return true;
}

uint64_t vstring_get_char_number(const void *_self)
{
	const struct VString *self = _self;

	return self->charNumber;
}

uint64_t vstring_get_word(const void *_self, void *_word, uint64_t pos)
{
	const struct VString *self = _self;
	struct VString *word = _word;
	bool returnValue = false;
	char character[BYTE_LEN_UTF8] = {0};
	uint64_t j;
	uint64_t i;
	munichar unicharacter = 0;

	for (j = pos; vstring_get_element_at(self, character, j); ++j) {
		unicharacter = g_utf8_get_char(character);
		if (g_unichar_isalnum(unicharacter)){
			returnValue = true;
			for (i = j + 1; vstring_get_element_at(self, character, i); ++i) {
				unicharacter = g_utf8_get_char(character);
				if(!g_unichar_isalnum(unicharacter))
					break;
			}
			vstring_erase_at(word, 0, word->charNumber);
			vstring_insert_vector_at(word, 0, self, j, i - j);
			break;
		}
	}

	return returnValue ? i : 0;
}

uint64_t vstring_get_word_in_line(const void *_self, void *_word, uint64_t pos)
{
	const struct VString *self = _self;
	struct VString *word = _word;
	char character[BYTE_LEN_UTF8] = {0};
	uint64_t j;
	munichar unicharacter = 0;
	
	for (j = pos; vstring_get_element_at(self, character, j); ++j) {
		unicharacter = g_utf8_get_char(character);

		if (unicharacter == NEW_LINE)
			return false;
		else if (g_unichar_isalnum(unicharacter))
			return vstring_get_word(self, word, j);
	}

	return false;
}

munichar vstring_get_unichar_at(const void *_self, uint64_t pos)
{
	const struct VString *self = _self;
	char character[BYTE_LEN_UTF8] = {0};
	
	if (vstring_get_element_at(self, character, pos))
		return g_utf8_get_char(character);
	else
		return (munichar) - 1;
}

bool vstring_get_next_line_pos(const void *_self, uint64_t start, uint64_t *position)
{
	const struct VString *self = _self;

	uint64_t j = 0;
	char newLine[BYTE_LEN_UTF8] = {0};

	do {
		vstring_get_element_at(self, newLine, start + j);
		j++;
	} while(strcmp(newLine, "\n") && newLine[0]);
	
	*position = j + start;

	return newLine[0];
}

bool vstring_replace_substring(void *_self, const void *_original, const void *_replacement)
{
	struct VString *self = _self;
	const struct VString *original = _original;
	const struct VString *replacement = _replacement;
	bool returnValue = false;
	uint64_t position = 0;

	returnValue = vstring_find_vector_in_vector(self, 0, original, 0, original->charNumber, &position);

	if (returnValue) {
		vstring_erase_at(self, position, original->charNumber);
		vstring_insert_vector_at(self, position, replacement, 0, replacement->charNumber);

		while (vstring_find_vector_in_vector(self, position + 1, original, 0, 
			   original->charNumber, &position)) {
			vstring_erase_at(self, position, original->charNumber);
			vstring_insert_vector_at(self, position, replacement, 0, replacement->charNumber);
		}
	}

	return returnValue;
}

bool vstring_replace_word(void *_self, const void *_original, const void *_replacement)
{
	struct VString *self = _self;
	const struct VString *original = _original;
	const struct VString *replacement = _replacement;
	bool returnValue = false;
	uint64_t position = 0;
	char before[BYTE_LEN_UTF8] = {0};
	char after[BYTE_LEN_UTF8] = {0};

	returnValue = vstring_find_vector_in_vector(self, 0, original, 0, original->charNumber, &position);

	if (returnValue) {
		if (position)
			vstring_get_element_at(self, before, position - 1);
		vstring_get_element_at(self, after, position + original->charNumber);

		if (!g_unichar_isalnum(g_utf8_get_char(before)) && !g_unichar_isalnum(g_utf8_get_char(after))) {
			vstring_erase_at(self, position, original->charNumber);
			vstring_insert_vector_at(self, position, replacement, 0, replacement->charNumber);
		}

		while (vstring_find_vector_in_vector(self, position + 1, original, 0, 
			   original->charNumber, &position)) {
			vstring_get_element_at(self, before, position - 1);
			vstring_get_element_at(self, after, position + original->charNumber);

			if (!g_unichar_isalnum(g_utf8_get_char(before)) && !g_unichar_isalnum(g_utf8_get_char(after))) {
				vstring_erase_at(self, position, original->charNumber);
				vstring_insert_vector_at(self, position, replacement, 0, replacement->charNumber);
			}
		}
	}

	return returnValue;
	
}

bool vstring_delete_substring(void *_self, const void *_substring)
{
	struct VString *self = _self;
	const struct VString *substring = _substring;
	bool returnValue = false;
	uint64_t position = 0;

	returnValue = vstring_find_vector_in_vector(self, 0, substring, 0, substring->charNumber, &position);

	if (returnValue) {
		vstring_erase_at(self, position, substring->charNumber);
		while (vstring_find_vector_in_vector(self, position, substring, 0, substring->charNumber, &position))
			vstring_erase_at(self, position, substring->charNumber);
	}

	return returnValue;
}

bool vstring_delete_word(void *_self, const void *_word)
{
	struct VString *self = _self;
	const struct VString *word = _word;
	bool returnValue = false;
	uint64_t position = 0;
	char before[BYTE_LEN_UTF8] = {0};
	char after[BYTE_LEN_UTF8] = {0};

	returnValue = vstring_find_vector_in_vector(self, 0, word, 0, word->charNumber, &position);

	if (returnValue) {
		if (position)
			vstring_get_element_at(self, before, position - 1);
		vstring_get_element_at(self, after, position + word->charNumber);

		if (!g_unichar_isalnum(g_utf8_get_char(before)) && !g_unichar_isalnum(g_utf8_get_char(after))) 
			vstring_erase_at(self, position, word->charNumber);
		else
			position++;

		while (vstring_find_vector_in_vector(self, position, word, 0, word->charNumber, &position)) {
			vstring_get_element_at(self, before, position - 1);
			vstring_get_element_at(self, after, position + word->charNumber);

			if (!g_unichar_isalnum(g_utf8_get_char(before)) && !g_unichar_isalnum(g_utf8_get_char(after))) 
				vstring_erase_at(self, position, word->charNumber);
			else
				position++;
		}
	}
	
	return returnValue;
}

bool vstring_delete_line(void *_self, uint64_t pos)
{
	struct VString *self = _self;

	if (pos >= self->charNumber) 
		return false;

	uint64_t j = 0;
	char newLine[BYTE_LEN_UTF8] = {0};

	do {
		vstring_get_element_at(self, newLine, pos + j);
		j++;
	} while(strcmp(newLine, "\n") && newLine[0]);

	vstring_erase_at(self, pos, j);

	return true;
}

bool vstring_compare_string(const void *_self, uint64_t pos, const void *_string, int64_t len)
{
	const struct VString *self = _self;
	const char *string = _string;

	if (len < 0) 
		len = strlen(string);

	return (strncmp(self->base.array + pos, string, len) == 0); 
}

uint64_t vstring_string_to_hex(const void *_self)
{
	const struct VString *self = _self;
	const char *string = self->base.array;
	
	if(self->base.len < 3)
		return -1;

	if (string[0] != '0' || (string[1] != 'x' && string[1] != 'X'))
		return -1;

	uint64_t j;
	for (j = 2; j < self->base.len; ++j) {
		if(!g_ascii_isxdigit(string[j]))
			return -1;
	}

	return strtol(string, NULL, 0);
}

void *vstring_hex_to_string(void *_self, uint64_t number)
{
	struct VString *self = _self;

	char *preppend = "0x";
	int length = utf8_char_number(preppend, -1);
	int nibble = 0;
	uint8_t *byte = (uint8_t *)(&number);
	int lastByte = sizeof(number) - 1;
	unsigned char digit = 0;

	vstring_erase_at(self, 0, self->charNumber);
	struct VString *check = vstring_insert_array_at(self, 0, preppend, length);
	NULL_RETURN(check)

	if (!number) {
		check = vstring_insert_array_at(self, 2, "0", 1);
		NULL_RETURN(check);
		return self;
	}

	int j;
	for (j = sizeof(number) * 2; !(nibble = byte[lastByte] & 0xF0); j--) 
		number = number << 4;

	nibble = nibble >> 4;
	printf("%d\n", j);

	for (; j > 0; j--) {
		if (nibble <= 9)
			digit = nibble + 48;
		else
			digit = nibble + 55;

		check = vstring_insert_array_at(self, self->charNumber, &digit, 1);
		NULL_RETURN(check);

		nibble = byte[lastByte] & 0x0F;
		number = number << 4;
	}

	return self;
}

//Class functions

void print_string(const void *_self, bool newLine)
{
	const struct VString *self = _self;

	assert(self);
	assert(self->secondClass == VString);

	((const struct vstring_vtable *)self->base.vtable)->vstring_print(self, newLine);
}

void *get_string_file(void *_self, FILE *fp)
{
	struct VString *self = _self;

	assert(self);
	assert(self->secondClass == VString);
	
	return  ((const struct vstring_vtable *)self->base.vtable)->vstring_get_string_file(self, fp);
}

bool write_string_file(const void *_self, FILE *fp)
{
	const struct VString *self = _self;

	assert(self);
	assert(self->secondClass == VString);
	
	return  ((const struct vstring_vtable *)self->base.vtable)->vstring_write_string_file(self, fp);
}

uint64_t get_char_number(const void *_self)
{
	const struct VString *self = _self;

	assert(self);
	assert(self->secondClass == VString);
	
	return  ((const struct vstring_vtable *)self->base.vtable)->vstring_get_char_number(self);
}

uint64_t get_word(const void *_self, void *_word, uint64_t pos)
{
	const struct VString *self = _self;
	struct VString *word = _word;

	assert(self);
	assert(word);
	assert(self->secondClass == VString);
	assert(word->secondClass == VString);

	return ((const struct vstring_vtable *)self->base.vtable)->vstring_get_word(self, word, pos);
}

uint64_t get_word_in_line(const void *_self, void *_word, uint64_t pos)
{
	const struct VString *self = _self;
	struct VString *word = _word;

	assert(self);
	assert(word);
	assert(self->secondClass == VString);
	assert(word->secondClass == VString);

	return ((const struct vstring_vtable *)self->base.vtable)->vstring_get_word_in_line(self, word, pos);
}

munichar get_unichar_at(const void *_self, uint64_t pos)
{
	const struct VString *self = _self;

	assert(self);
	assert(self->secondClass == VString);
	
	return  ((const struct vstring_vtable *)self->base.vtable)->vstring_get_unichar_at(self, pos);
}

bool get_next_line_pos(const void *_self, uint64_t start, uint64_t *position)
{
	const struct VString *self = _self;

	assert(self);
	assert(position);
	assert(self->secondClass == VString);
	
	return  ((const struct vstring_vtable *)self->base.vtable)->vstring_get_next_line_pos(self, start, position);
}

bool replace_substring(void *_self, const void *_original, const void *_replacement)
{
	struct VString *self = _self;
	const struct VString *original = _original;
	const struct VString *replacement = _replacement;

	assert(self);
	assert(original);
	assert(replacement);
	assert(self->secondClass == VString);
	assert(original->secondClass == VString);
	assert(replacement->secondClass == VString);
	
	return ((const struct vstring_vtable *)self->base.vtable)->vstring_replace_substring(self, original, replacement);
}

bool replace_word(void *_self, const void *_original, const void *_replacement)
{
	struct VString *self = _self;
	const struct VString *original = _original;
	const struct VString *replacement = _replacement;

	assert(self);
	assert(original);
	assert(replacement);
	assert(self->secondClass == VString);
	assert(original->secondClass == VString);
	assert(replacement->secondClass == VString);
	
	return ((const struct vstring_vtable *)self->base.vtable)->vstring_replace_word(self, original, replacement);
}

bool delete_substring(void *_self, const void *_substring)
{
	struct VString *self = _self;
	const struct VString *substring = _substring;

	assert(self);
	assert(substring);
	assert(self->secondClass == VString);
	assert(substring->secondClass == VString);

	return ((const struct vstring_vtable *)self->base.vtable)->vstring_delete_substring(self, substring);
}

bool delete_word( void *_self, const void *_word)
{
	struct VString *self = _self;
	const struct VString *word = _word;

	assert(self);
	assert(word);
	assert(self->secondClass == VString);
	assert(word->secondClass == VString);

	return ((const struct vstring_vtable *)self->base.vtable)->vstring_delete_word(self, word);
}

bool delete_line(void *_self, uint64_t pos)
{
	struct VString *self = _self;

	assert(self);
	assert(self->secondClass == VString);

	return ((const struct vstring_vtable *)self->base.vtable)->vstring_delete_line(self, pos);
}

bool compare_string(const void *_self, uint64_t pos, const void *_string, int64_t len)
{
	const struct VString *self = _self;
	const char *string = _string;

	assert(self);
	assert(string);
	assert(self->secondClass == VString);

	return ((const struct vstring_vtable *)self->base.vtable)->vstring_compare_string(self, pos, string, len);
}

uint64_t string_to_hex(const void *_self)
{
	const struct VString *self = _self;

	assert(self);
	assert(self->secondClass == VString);
	
	return ((const struct vstring_vtable *)self->base.vtable)->vstring_string_to_hex(self);
}

void *hex_to_string(void *_self, uint64_t number)
{
	struct VString *self = _self;

	assert(self);
	assert(self->secondClass == VString);

	return ((const struct vstring_vtable *)self->base.vtable)->vstring_hex_to_string(self, number);
}

int64_t utf8_char_number(const char *string, int64_t size)
{
	return g_utf8_strlen(string, size);
}
