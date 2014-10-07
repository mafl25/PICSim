#include "filemanager.h"
#include <gtk/gtk.h>
#include <errno.h>
#include <string.h>

//Prints a message to the output buffer, adding an \n character at the end.
void outputPrint(GtkTextBuffer *outputBuffer, char * message, gboolean addNewline)
{
	GtkTextIter end;
	gtk_text_buffer_get_end_iter(outputBuffer, &end);
	gtk_text_buffer_insert(outputBuffer, &end, message, -1);
	if(addNewline){
		gtk_text_buffer_get_end_iter(outputBuffer, &end);
		gtk_text_buffer_insert(outputBuffer, &end, "\n", -1);
	}
}

//This function closes a file. It checks if the file pointer is a NULL or if it is closed already
//in which case it will do nothing and will return FALSE. If the file pointer is valid and the file
//is indeed open, it will close it. If the closing was successful, it will return TRUE and set isOpen to
//FALSE. If it fails, it will print a message to the output buffer and return FALSE, leaving isOpen as true.
//Eventually, add truth table
gboolean fileClose(textStruct *file)
{
	gboolean returnValue = FALSE;

	if(file->file != NULL){
		int output = fclose(file->file);
		if(output == EOF){
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "Error: File \"", FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), file->filename, FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "\" could not be closed.", TRUE);
		}else{
			file->file = NULL;
			returnValue = TRUE;
		}
	}

	return returnValue;
}

gboolean fileOpen(textStruct *file)
{
	gboolean returnValue = FALSE;

	if(file->filename != NULL){
		file->file = fopen(file->filename, "r");
		if(file->file == NULL){
			char *error = strerror(errno);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "Error opening file \"", FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), file->filename, FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "\": ", FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), error, TRUE);
		}else{
			returnValue = TRUE;
		}
	}else{
		file->file = NULL;
		outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "Error opening file: No filename provided.", TRUE);
	}

	return returnValue;
}

gboolean fileWrite(textStruct *file)
{
	gboolean returnValue = FALSE;

	if(file->filename != NULL){
		file->file = fopen(file->filename, "w");
		if(file->file == NULL){
			char *error = strerror(errno);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "Error writting file \"", FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), file->filename, FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "\": ", FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), error, TRUE);
		}else{
			returnValue = TRUE;
		}
	}else{
		file->file = NULL;
		outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "Error writting file: No filename provided.", TRUE);
	}

	return returnValue;
}

