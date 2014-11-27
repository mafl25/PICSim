/*
 * =====================================================================================
 *
 *       Filename:  customstring.h
 *
 *    Description:  Header file for customstring.c
 *
 *        Version:  1.0
 *        Created:  03/09/14 22:09:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Manuel Fernandez (mafl), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <gtk/gtk.h>

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getWordString
 *  Description:  This function looks in a string for the first word it can find, stores
 *				  it in an character array and returns the position of the character 
 *				  after the last letter of the word in the string. If no word is found,
 *				  a cero is returned.Note: This function considers a word any continuos 
 *				  substring.
 *   Arguments:	  char *string	= The string from which a word will be looked from.
 *				  char *word  	= The pointer to the array where the word found will be
 * 				  stored.
 *		 		  int length	= The size of the word array. 	
 * =====================================================================================
 */

typedef struct
{
	GString *line;
	guint originalPosition;
	guint currentPosition;
}MLine;

typedef struct
{
	MLine **lineStack;
	guint numberOfLines;
	guint allocatedLines;
	guint currentLine;
}MLineStack;

void mlinestack_delete_line(MLineStack, guint position);
MLineStack *mlinestack_new(gchar *text);

int get_word_string(char *string, char **word) ;
gboolean glib_get_word_string(GString *word, const gchar *string, gsize *position);
gboolean glib_get_line_string(GString *line, const gchar *string, gsize *position);
GString *glib_replace_word_from_string(GString *haystack,  const GString *needle, const GString *replacement);

int glib_hex_string_to_int(GString *string);

gboolean is_line_whitespace_before_newline(const gchar *line);
gboolean is_line_empty(const gchar *line);
gboolean is_word(const gchar *string, gsize lengthString, const gchar *substring, gsize lengthSubstring);
