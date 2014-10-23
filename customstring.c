#include "customstring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <gtk/gtk.h>

/*
 * =====================================================================================
 *
 *       Filename:  customstring.c
 *
 *    Description:  A collection of string related functions.
 *
 *        Version:  1.0
 *        Created:  03/09/14 21:30:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Manuel Fernandez (mafl), 
 *   Organization:  
 *
 * =====================================================================================
 */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getWordString
 *  Description:  This function looks in a string for the first word it can find, 
 *				  allocates memory for the word, and stores it in a character array 
 *				  and returns the position of the character after the last letter of the 
 *				  word in the string. If no word is found, a cero is returned. Note: 
 *				  This function considers a word any continuos substring.
 *   Arguments:	  unsigned char *string	= The string from which a word will be looked 
 *				  from.
 *				  unsigned char **word = The pointer to the array where the word found 
 *				  will be stored.
 * =====================================================================================
 */

int get_word_string(char *string, char **word) 
{
	assert(string);
	assert(word);
	
	int i = 0;
	int position = 0;
	int byteCount = 0;
	int wordFound = 0;
	
	while (wordFound == 0) {
		if(string[position] == '\0')
			break;
			
		while (((string[position + byteCount] >= '!') && (string[position + byteCount] <= '~')) 
				|| (string[position + byteCount] & 0x80)){
			wordFound = 1;
			byteCount++;
		}
		
		if(wordFound == 0){
			position++;
		}else{
			(*word) = calloc(byteCount + 1, 1);
			if((*word) == NULL){
				return -1;//Actualizar descripcion luego
			}
			for(i = 0; i < byteCount; i++){
				(*word)[i] = string[position];
				position++;
			}
		}
	}

	if (wordFound == 1)
		return position;
	else
		return 0;
}

gboolean glib_get_word_string(GString *word, const gchar *string, gsize *position)
{
	assert(word);
	assert(string);

	int length = 0;
	gboolean wordFound = FALSE;
	g_string_erase(word, 0, -1);

	while(!wordFound){
		if(string[(*position)] == '\0')
			break;

		while (((string[(*position) + length] >= '!') && (string[(*position) + length] <= '~')) 
				|| (string[(*position) + length] & 0x80)){
			wordFound = TRUE;
			length++;
		}

		if(wordFound){
			g_string_insert_len(word, -1, &string[(*position)], length);
			(*position) += length;
		}else{
			(*position)++;
		}
	}

	return wordFound;
}

gboolean glib_get_line_string(GString *line, const gchar *string, gsize *position)
{
	assert(line);
	assert(string);
	gboolean lineFound = FALSE;
	gsize length = 0;
	
	g_string_erase(line, 0, -1);

	while(!lineFound){
		if(string[*position] == '\0')
			break;

		lineFound = TRUE;

		while(string[*position + length] != '\n' && string[*position + length] != '\0')
			length++;

		if(string[*position + length] == '\n')
			length++;

		g_string_insert_len(line, -1, &string[(*position)], length);
		(*position) += length;
	}
	
	return lineFound;
}

GString *glib_replace_word_from_string(GString *haystack,  const GString *needle, const GString *replacement)
{
	gchar *needlePosition = g_strstr_len(haystack->str, -1, needle->str);
	gchar *initPosition = haystack->str;

	while(needlePosition){
		if(is_word(haystack->str, haystack->len, needlePosition, needle->len)){
			g_string_erase(haystack, needlePosition - initPosition, needle->len);
			g_string_insert_len(haystack, needlePosition - initPosition, replacement->str, replacement->len);
			needlePosition = g_strstr_len(needlePosition + replacement->len, -1, needle->str);
		}else{
			needlePosition = g_strstr_len(needlePosition + needle->len, -1, needle->str);
		}
	}

	return haystack;
}

int glib_hex_string_to_int(GString *string)
{
	int charValue = 0;
	int j;

	if(string->len < 3)
		return EOF;

	if(string->str[0] != '0' || (string->str[1] != 'x' && string->str[1] != 'X'))
		return EOF;

	for (j = 2; j < string->len; ++j)
		if((charValue = g_ascii_xdigit_value(string->str[j])) == -1)
			return EOF;

	return strtol(string->str, NULL, 0);
}

gboolean is_line_whitespace_before_newline(const gchar *line)
{
	int j;
	for (j = 0; line[j] != '\n' && line[j] != '\0'; ++j){
		if(!g_ascii_isspace(line[j]))
			return FALSE;
	}

	return TRUE;
}

gboolean is_word(const gchar *string, gsize lengthString, const gchar *substring, gsize lengthSubstring)
{
	assert(substring >= string);
	assert(substring < (string + lengthString));
	int position = substring - string;
	char character;

	character = string[position + lengthSubstring];
	if((g_ascii_isalpha(character) || character == '_' || (character & 0x80)))
		return FALSE;

	if(string != substring){
		character = string[position - 1];
		if((g_ascii_isalpha(character) || character == '_' || (character & 0x80)))
			return FALSE;
	}

	return TRUE;
}


