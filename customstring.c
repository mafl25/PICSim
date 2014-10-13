#include "customstring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

enum bool {FALSE, TRUE};
typedef enum bool Bool;

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

int getWordString(unsigned char *string, unsigned char **word) 
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
				|| (string[position + byteCount] >= 0x80)){
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

int getWordFile (FILE *fp, char *string, int length)
{
	int i = 0;
	Bool  wordFound = FALSE;
	int letter = fgetc(fp);

	while(wordFound == FALSE){
		while((letter >= '!') && (letter <= '~')){
			wordFound = TRUE;
			string[i] = (char)letter;
			i++;
			if(i == length - 1)
				break;
			letter = fgetc(fp);
			if(letter == EOF)
				break;
		}

		if(letter == EOF)
			break;

		if(wordFound == FALSE)
			letter = fgetc(fp);
	}
	
	string[i] = '\0';
	return i;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hexString
 *  Description:  This function take in a positive hexadecimal number written in a 
 *  	          string, starting with 0x or 0X, and returns an integer. If the
 *  	          convertion couldn't take place, it will return EOF. 
 *    Arguments:  char *hexStr	= The string from which the number will be obtained.
 * =====================================================================================
 */

int hexString (char *hexStr)
{
	int strLength = strlen(hexStr);
	int numHexInt = sizeof(int) * 2;
	int sizeOfHex = (strLength - 2);
	int j = 2;
	Bool isCorrectFormat = TRUE;
	
	if(strLength < 3)
		isCorrectFormat = FALSE;
	
	if(hexStr[0] != '0' || (hexStr[1] != 'x' && hexStr[1] != 'X'))
		isCorrectFormat = FALSE;

	if (sizeOfHex > numHexInt) {
		for ( j = 2; j < (strLength - numHexInt); j++) {
			if(hexStr[j] != '0'){
				isCorrectFormat = FALSE;
				break;
			}
		}
		if((isCorrectFormat == TRUE) && (hexStr[j] >= 56))
			isCorrectFormat = FALSE;
	}else if(sizeOfHex == numHexInt){
		if(hexStr[2] >= 56)
			isCorrectFormat = FALSE;
	}

	j = 2;

	while (isCorrectFormat == TRUE && j < strLength) {
		if(!((hexStr[j] <= '9' && hexStr[j] >= '0') || (hexStr[j] <= 'F' && hexStr[j] >= 'A') 
				|| (hexStr[j] <= 'f' && hexStr[j] >= 'a'))){
			isCorrectFormat = FALSE;
		}
		j++;
	}
	
	return (isCorrectFormat) ? strtol(hexStr, NULL, 0) :  EOF;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  isLineEmpty
 *  Description:  This function checks a string if it does not contains any characters.
 *		  		  It returns a 1 if it is empty or a 0 otherwise.
 *   Argunments:  char* string	= The string that will be checked if it is empty.
 * =====================================================================================
 */

int isLineEmpty (char *string)
{
	int j;
	Bool isEmpty = TRUE;

	for(j = 0; j < strlen(string); j++){
		if(string[j] >= '!' && string[j] <= '~')
			isEmpty = FALSE;
	}

	return (isEmpty) ? 1 : 0;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  stringCopy
 *  Description:  This function copies a string into another, adding the terminanting
 *				  character '\0' at the end. NOTE: It is important to take into account
 *			 	  that if you want to copy a certain amount of letters from a string to 
 *				  another, you will have to increase by one the "length" parameter, 
 *				  otherwise, the function won't copy the last letter, as it will use 
 *				  that position for the termination character.
 *    Arguments:  char *destiny	= A pointer to the character array where the copied
 *		   		  string will be stored.
 *				  char *source	= A pointer to the string that will be copied.
 *				  int length	= The length of the destiny array. It can also be used 
 *				  to copy a certain amount of letters to the other string, as long as it
 *				  is less than the length of the destiny array, but it is important to 
 *				  pay attention to the note above.
 * =====================================================================================
 */

void stringCopy (char *destiny, char *source, int length)
{
	int j;

	for (j = 0; (j < length - 1)  && (source[j] != '\0'); j++)
		destiny[j] = source[j];

	destiny[j] = '\0';
}

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

int getLineFile (char *line, int maxLength, FILE *fp)
{
	int letter = fgetc(fp);
	int i = 0;

	while((i < maxLength - 1) && (letter != EOF) && (letter != '\n')){
		line[i] = letter;
		i++;
		if(i < maxLength - 1)
			letter = fgetc(fp);
	}
	if((letter == '\n') && (i < maxLength - 1 )){
		line[i] = letter;
		i++;
	}
	
	line[i] = '\0';
	return i;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  findNumStr
 *  Description:  This function takes as argument a string and a subtring and gives back
 *				  the amount of times one string appears in the other.
 *    Arguments:  char *haystack = A pointer to a string where another string will be 
 *				  looked for.
 *				  char *needle	= A pointer to a string that will be looked for.
 * =====================================================================================
 */

int findNumStr(char *haystack, char *needle)
{
	int i = 0;
	char *initPos = haystack;
	char *position = strstr(haystack, needle);
	int cut;
	int needleLen = strlen(needle);

	while(position != NULL){
		cut = position - initPos;
		position = strstr(&haystack[cut + needleLen], needle);
		i++;
	}

	return i;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  isWord
 *  Description:  This functions checks if a substring within a string is considered a 
 *				  word. To do this, it uses a character array with the characters that 
 *				  are considered to delimit a word. It returns a 1 if it's a word, a
 *				  0 if it isn't or a -1 if a NULL pointer was given to it.
 *    Arguments:  char *string	= a pointer to the string that contains the substring.
 *				  char *pTrposition = A pointer to the position of the substring within
 *				  the string.
 *				  int sizeOfSubS = The size of the substring.
 * =====================================================================================
 */

int isWord (char *string, char *pTrposition, int sizeOfSubS)
{
	Bool consideredWord = TRUE;
	int position = pTrposition - string;

	if (pTrposition == NULL)
		return -1;
	
	if(string == pTrposition){
		if((string[position + sizeOfSubS] == '_') 
				|| (string[position + sizeOfSubS] >= '0' && string[position + sizeOfSubS] <= '9') 
				|| (string[position + sizeOfSubS] >= 'a' && string[position + sizeOfSubS] <= 'z')
				|| (string[position + sizeOfSubS] >= 'A' && string[position + sizeOfSubS] <= 'Z'))
			consideredWord = FALSE;
	}else{
		if((string[position + sizeOfSubS] == '_') 
				|| (string[position + sizeOfSubS] >= '0' && string[position + sizeOfSubS] <= '9') 
				|| (string[position + sizeOfSubS] >= 'a' && string[position + sizeOfSubS] <= 'z')
				|| (string[position + sizeOfSubS] >= 'A' && string[position + sizeOfSubS] <= 'Z'))
			consideredWord = FALSE;
		if((string[position - 1] == '_') 
				|| (string[position - 1] >= '0' && string[position - 1] <= '9') 
				||(string[position - 1] >= 'a' && string[position - 1] <= 'z')
				|| (string[position - 1] >= 'A' && string[position - 1] <= 'Z'))
			consideredWord = FALSE;	
	}

	return (consideredWord) ? 1 : 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  findNumWord
 *  Description:  This function takes as argument a strings and a substring and gives
 *				  back the amount of times one string appears in the other that can be 
 *				  considered a word.
 *    Arguments:  char *haystack = A pointer to a string where another string will be 
 *				  looked for.
 *				  char *needle	= A pointer to a string that will be looked for.
 * =====================================================================================
 */

int findNumWord(char *haystack, char *needle)
{
	int i = 0;
	char *initPos = haystack;
	char *position = strstr(haystack, needle);
	int cut;
	int needleLen = strlen(needle);

	while(position != NULL){
		cut = position - initPos;
		if(isWord(haystack, position, needleLen))
			i++;
		position = strstr(&haystack[cut + needleLen], needle);
	}

	return i;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  stringReplaceWord
 *  Description:  This function takes as argument a string and two substrings, and will
 *				  replace the instances of one substring which can be considered a word
 *				  (see isWord function) for the other substring. It returns a pointer to
 *				  another string with the substrings replaced. The function
 *				  destroyStringReplaceWord must be called after the string is no longer 
 *				  needed in order to free the memory taken by it. A  NULL pointer is
 *				  returned if memory cannot be allocated for the return string.
 *    Arguments:  char *haystack = The string from where a substring will be replaced.
 *				  char *needle	= The substring which will be replaced.
 *				  char *replacement = The substring that will replace the other.
 * =====================================================================================
 */

char *stringReplaceWord(char *haystack, char *needle, char *replacement)
{
	char *position;
	char *initPos;
	int cut;
	int wordComplete;
	
	//The length of the return string is computed.
	int instances = findNumWord(haystack, needle);
	int sizeOfReplacement = strlen(replacement);
	int sizeOfNeedle = strlen(needle);
	int sizeOfHaystack = strlen(haystack);
	int newSize = sizeOfHaystack + instances*(sizeOfReplacement - sizeOfNeedle);
	int length = (newSize > sizeOfHaystack) ? newSize : sizeOfHaystack;
	length++; //To take into account the '\0' character, the final length is incremented by one.

	char *lineTempOne = (char *)calloc(length, sizeof(char));
	if(lineTempOne == NULL)
		return NULL;
	char *lineTempTwo = (char *)calloc(length, sizeof(char));
	if(lineTempTwo == NULL)
		return NULL;
	
	stringCopy(lineTempTwo, haystack, length);
	initPos =  lineTempTwo;
	position =  strstr(lineTempTwo, needle);
	while(position != NULL) {
		wordComplete = isWord(lineTempTwo, position,  sizeOfNeedle);
		cut = position - initPos;
		if(wordComplete){
			//Here one is added to cut. That is because we need a "cut" amount of characters copied from
			//lineTemp One to lineTempTwo, but the stringCopy function would only copy "cut - 1" characters,
			//as the las one is used for '\0'. Read the descrption of stringCopy for more information.
			stringCopy(lineTempOne, lineTempTwo, cut + 1);
			strcat(lineTempOne, replacement);
			strcat(lineTempOne, &lineTempTwo[cut + sizeOfNeedle]);
			stringCopy(lineTempTwo, lineTempOne, length);
			position = strstr(&lineTempTwo[cut + sizeOfReplacement], needle);
		}else{
			position = strstr(&lineTempTwo[cut + sizeOfNeedle], needle);
		}
	}

	free(lineTempOne);

	return lineTempTwo;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  destroyStringReplaceWord
 *  Description:  Frees the memory used by stringReplaceWord function.
 *    Arguments:  char *haystack = A poniter to a string. It must be the ine returned by 
 *				  by the stringReplaceWord function.
 * =====================================================================================
 */

void destroyStringReplaceWord(char  *haystack)
{
	free(haystack);
}
