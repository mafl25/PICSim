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

int getWordString(unsigned char *string, unsigned char **word) ;
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getWordFile
 *  Description:  This function looks for the next word in a file, stores it in a
 *				  character array and returns the size of that word. Note: This function
 *				  considers any continuos substring.
 *    Arguments:  FILE *fp	= Pointer to the file from where the words will be taken
 *  			  out.
 *				  char *word	= The pointer to the array where the word found will be
 * 				  stored.
 *				  int length	= The size of the word array. 	
 * =====================================================================================
 */

int getWordFile (FILE *fp, char *string, int length);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hexString
 *  Description: This function take in a positive hexadecimal number written in a 
 *  	         string, starting with 0x or 0X, and returns an integer. If the
 *  	         convertion couldn't take place, it will return EOF. 
 *   Arguments:	 char *hexStr	= The string from which the number will be obtained.
 * =====================================================================================
 */

int hexString (char *hexStr);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  isLineEmpty
 *  Description:  This function checks a string if it does not contains any characters.
 *   Argunments:  char* string	= The string that will be checked if it is empty.
 * =====================================================================================
 */

int isLineEmpty (char *string);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  stringCopy
 *  Description:  This function copies a string into another, adding the terminanting
 *				  character '\0' at the end. NOTE: It is important to take into account
 *		 		  that if you want to copy a certain amount of letters from a string to 
 *				  another, you will have to increase by one the "length" parameter, 
 *				  otherwise, the function won't copy the last letter, as it will use 
 *				  that position for the termination character.
 *    Arguments:  char *destiny	= A pointer to the character array where the copied
 *				  string will be stored.
 *				  char *source	= A pointer to the string that will be copied.
 *				  int length	= The length of the destiny array. It can also be used 
 *				  to copy a certain amount of letters to the other string, as long as it
 *				  is less than the length of the destiny array, but it is important to 
 *				  pay attention to the note above.
 * =====================================================================================
 */

void stringCopy (char *destiny, char *source, int length);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  getLineFile
 *  Description:  This function copies a line from a file to a char array, adding the 
 *				  terminating character at the end. It returns the length of the line.
 *    Arguments:  char *line	= A pointer to the array where the chracters will be 
 *				  copied.
 *				  int maxLength	= The length of the array.
 *				  FILE *fp	= A pointer to the file from where characters will be 
 *				  copied.
 * =====================================================================================
 */

int getLineFile (char *line, int maxLength, FILE *fp);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  findNumStr
 *  Description:  This function takes as argument two strings and gives back the amount
 *                of times one string appears in the other.
 *    Arguments:  char *haystack = A pointer to a string where another string will be 
 *		 		  looked for.
 *				  char *needle = A pointer to a string that will be looked for.
 * =====================================================================================
 */

int findNumStr(char *haystack, char *needle);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  isWord
 *  Description:  This functions checks if a substring within a string is considered a 
 *				  word. To do this, it uses a character array with the characters that 
 *		 		  are considered to delimit a word. It returns a 1 if it's a word or a
 *				  0 if it isn't.
 *    Arguments:  char *string	= a pointer to the string that contains the substring.
 *				  char *pTrposition = A pointer to the position of the substring within
 *				  the string.
 *				  int sizeOfSubS = The size of the substring.
 * =====================================================================================
 */

int isWord (char *string, char *pTrposition, int sizeOfSubS);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  findNumWord
 *  Description:  This function takes as argument a strings and a substring and gives
 *				  back the amount of times one string appears in the other that can be 
 *				  considered a word.
 *    Arguments:  char *haystack = A pointer to a string where another string will be 
 *				  looked for.
 *			 	  char *needle	= A pointer to a string that will be looked for.
 * =====================================================================================
 */

int findNumWord(char *haystack, char *needle);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  stringReplaceWord
 *  Description:  This function takes as argument a string and two substrings, and will
 *				  replace the instances of one substring which can be considered a word
 *				  (see isWord function) for the other substring. It returns a pointer to
 *				  another string with the substrings replaced. The function
 *				  destroyStringReplace must be called after the string is no longer 
 *				  needed to free the memory taken by it. A  NULL pointer is returned if
 *				  memory cannot be allocated for the return string.
 *    Arguments:  char *haystack = The string from where a substring will be replaced.
 *				  char *needle	= The substring which will be replaced.
 *				  char *replacement = The substring that will replace the other.
 * =====================================================================================
 */

char *stringReplaceWord(char *haystack, char *needle, char *replacement);

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  destroyStringReplaceWord
 *  Description:  Frees the memory used by stringReplaceWord function.
 *    Arguments:  char *haystack = A poniter to a string. It must be the ine returned by 
 *				  by the stringReplaceWord function.
 * =====================================================================================
 */

void destroyStringReplaceWord(char  *haystack);

