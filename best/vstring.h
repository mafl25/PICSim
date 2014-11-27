#ifndef VSTRING_H
#define VSTRING_H

#include "vector.h"
#include <stdio.h>


typedef uint32_t munichar;

struct VString;
extern const void *VString;

void print_string(const void *self, bool newLine);

void *get_string_file(void *self, FILE *fp);
bool write_string_file(const void *self, FILE *fp);

uint64_t get_char_number(const void *self);
uint64_t get_word(const void *self, void *word, uint64_t pos);
uint64_t get_word_in_line(const void *self, void *word, uint64_t pos);
munichar get_unichar_at(const void *self, uint64_t pos);
bool get_next_line_pos(const void *self, uint64_t start, uint64_t *position);

bool replace_substring(void *self, const void *original, const void *replacement);
bool replace_word(void *self, const void *original, const void *replacement);

bool delete_substring(void *self, const void *substring);
bool delete_word( void *self, const void *word);
bool delete_line(void *self, uint64_t pos);

bool compare_string(const void *self, uint64_t pos, const void *string, int64_t len);

uint64_t string_to_hex(const void *self);
void *hex_to_string(void *self, uint64_t number);

int64_t utf8_char_number(const char *string, int64_t size);

#endif


