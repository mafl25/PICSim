#include "filemanager.h"
#include "outputbuffer.h"
#include <gtk/gtk.h>
#include <errno.h>
#include <string.h>

//This function closes a file. It checks if the file pointer is a NULL or if it is closed already
//in which case it will do nothing and will return FALSE. If the file pointer is valid and the file
//is indeed open, it will close it. If the closing was successful, it will return TRUE and set isOpen to
//FALSE. If it fails, it will print a message to the output buffer and return FALSE, leaving isOpen as true.
//Eventually, add truth table
gboolean fileClose(FILE **file, const gchar *filename, GtkTextBuffer *error_output)
{
	gboolean returnValue = FALSE;

	if(*file != NULL){
		int output = fclose(*file);
		if(output == EOF){
			if(error_output != NULL){
				outputPrint(error_output, "Error: File \"", FALSE);
				if(filename != NULL)
					outputPrint(error_output, filename, FALSE);
				outputPrint(error_output, "\" could not be closed.", TRUE);
			}
		}else{
			*file = NULL;
			returnValue = TRUE;
		}
	}

	return returnValue;
}

gboolean fileOpen(FILE **file, const gchar *filename, GtkTextBuffer *error_output)
{
	gboolean returnValue = FALSE;

	if(filename != NULL){
		*file = fopen(filename, "r");
		if(*file == NULL){
			if(error_output != NULL){
				char *error = strerror(errno);
				outputPrint(error_output, "Error opening file \"", FALSE);
				outputPrint(error_output, filename, FALSE);
				outputPrint(error_output, "\": ", FALSE);
				outputPrint(error_output, error, TRUE);
			}
		}else{
			returnValue = TRUE;
		}
	}else{
		*file = NULL;
		if(error_output != NULL)
			outputPrint(error_output, "Error opening file: No filename provided.", TRUE);
	}

	return returnValue;
}

gboolean fileWrite(FILE **file, const gchar *filename, GtkTextBuffer *error_output)
{
	gboolean returnValue = FALSE;

	if(filename != NULL){
		*file = fopen(filename, "w");
		if(*file == NULL){
			if(error_output != NULL){
				char *error = strerror(errno);
				outputPrint(error_output, "Error writting file \"", FALSE);
				outputPrint(error_output, filename, FALSE);
				outputPrint(error_output, "\": ", FALSE);
				outputPrint(error_output, error, TRUE);
			}
		}else{
			returnValue = TRUE;
		}
	}else{
		*file = NULL;
		if(error_output != NULL)
			outputPrint(error_output, "Error writting file: No filename provided.", TRUE);
	}

	return returnValue;
}


