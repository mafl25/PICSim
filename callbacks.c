#include "callbacks.h"
#include "filedialog.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <assert.h>

static void change_label(GtkLabel *label, gchar *filename)
{
	assert(label);
	if(filename){
		gchar *name = g_filename_display_basename(filename);
		gtk_label_set_text(label, name);
		g_free(name);
	}else{
		gtk_label_set_text(label, UNSAVED_FILE);
	}
}

gboolean set_saved_cb(GtkWidget *widget, gpointer data)
{
	textStruct *label = (textStruct *)data;
	label->isSaved = TRUE;

	return TRUE;
}

gboolean open_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *openFile = (textStruct *)data;

	gboolean returnValue = open_file(&openFile->file, &openFile->filename, GTK_TEXT_BUFFER(openFile->outputBuffer));
	if(returnValue && (openFile->label != NULL))
		change_label(GTK_LABEL(openFile->label), openFile->filename);

	return returnValue;
}

gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data)
{
	textStruct *fileLoad = (textStruct *)data;
	int fileSize;
	int character;
	gboolean returnValue = TRUE;
	int j;

	if(fileLoad->file == NULL)
		returnValue = FALSE;
		
	if(returnValue){
		rewind(fileLoad->file);
		for (fileSize = 0; (character = getc(fileLoad->file)) != EOF; ++fileSize)
			j = 0;

		gchar *text = (gchar *)calloc(sizeof(gchar), fileSize + 1);

		rewind(fileLoad->file);
		for (j = 0; j < fileSize; ++j)
			text[j] = (gchar)getc(fileLoad->file);	

		g_signal_handler_block((gpointer)fileLoad->programBuffer, fileLoad->programChangedHandlerId);
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(fileLoad->programBuffer), text, -1);
		g_signal_handler_unblock((gpointer)fileLoad->programBuffer, fileLoad->programChangedHandlerId);

		rewind(fileLoad->file);
		free(text);

		fileClose(&fileLoad->file,fileLoad->filename, GTK_TEXT_BUFFER(fileLoad->outputBuffer));
	}

	return returnValue;
}

gboolean save_text_view_to_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *fileSave = (textStruct *)data;
	gboolean returnValue = FALSE;
	
	returnValue	= save_text_view_to_file(&fileSave->file, &fileSave->filename, 
			GTK_TEXT_BUFFER(fileSave->programBuffer), GTK_TEXT_BUFFER(fileSave->outputBuffer));

	if(returnValue && (fileSave->label != NULL))
		change_label(GTK_LABEL(fileSave->label), fileSave->filename);
	
	return returnValue;
}

gboolean save_as_text_view_to_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *fileSave = (textStruct *)data;
	gboolean returnValue = FALSE;
	
	returnValue	= save_as_text_view_to_file(&fileSave->file, &fileSave->filename, 
			GTK_TEXT_BUFFER(fileSave->programBuffer), GTK_TEXT_BUFFER(fileSave->outputBuffer));

	if(returnValue && (fileSave->label != NULL))
		change_label(GTK_LABEL(fileSave->label), fileSave->filename);
	
	return returnValue;
}

gboolean new_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *newFile= (textStruct *)data;
	gboolean returnValue = FALSE;
	g_signal_handler_block((gpointer)newFile->programBuffer, newFile->programChangedHandlerId);

	returnValue = new_file(&newFile->file, &newFile->filename, 
			GTK_TEXT_BUFFER(newFile->programBuffer), GTK_TEXT_BUFFER(newFile->outputBuffer));

	if(returnValue && (newFile->label != NULL))
		change_label(GTK_LABEL(newFile->label), newFile->filename);

	g_signal_handler_unblock((gpointer)newFile->programBuffer, newFile->programChangedHandlerId);

	return returnValue;
}
