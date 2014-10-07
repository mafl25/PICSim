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
gboolean fileClose(MFile *file, GtkTextBuffer *outputBuffer)
{
	gboolean returnValue = FALSE;

	if(file->file != NULL){
		int output = fclose(file->file);
		if(output == EOF){
			outputPrint(outputBuffer, "Error: File \"", FALSE);
			outputPrint(outputBuffer, file->filename, FALSE);
			outputPrint(outputBuffer, "\" could not be closed.", TRUE);
		}else{
			file->file = NULL;
			returnValue = TRUE;
		}
	}

	return returnValue;
}

gboolean fileOpen(MFile *file, GtkTextBuffer *outputBuffer)
{
	gboolean returnValue = FALSE;

	if(file->filename != NULL){
		file->file = fopen(file->filename, "r");
		if(file->file == NULL){
			char *error = strerror(errno);
			outputPrint(outputBuffer, "Error opening file \"", FALSE);
			outputPrint(outputBuffer, file->filename, FALSE);
			outputPrint(outputBuffer, "\": ", FALSE);
			outputPrint(outputBuffer, error, TRUE);
		}else{
			returnValue = TRUE;
		}
	}else{
		file->file = NULL;
		outputPrint(outputBuffer, "Error opening file: No filename provided.", TRUE);
	}

	return returnValue;
}

gboolean fileWrite(MFile *file, GtkTextBuffer *outputBuffer)
{
	gboolean returnValue = FALSE;

	if(file->filename != NULL){
		file->file = fopen(file->filename, "w");
		if(file->file == NULL){
			char *error = strerror(errno);
			outputPrint(outputBuffer, "Error writting file \"", FALSE);
			outputPrint(outputBuffer, file->filename, FALSE);
			outputPrint(outputBuffer, "\": ", FALSE);
			outputPrint(outputBuffer, error, TRUE);
		}else{
			returnValue = TRUE;
		}
	}else{
		file->file = NULL;
		outputPrint(outputBuffer, "Error writting file: No filename provided.", TRUE);
	}

	return returnValue;
}


