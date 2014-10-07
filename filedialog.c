#include "filedialog.h"
#include "outputbuffer.h"
#include <gtk/gtk.h>
#include <stdlib.h>

//Tal vez tenga que hacer una funcion aparte para modificar los labels

void textStructInit(textStruct *text, GtkBuilder *builder, gchar *text_program, gchar *text_output,
		gchar *program_buffer, gchar *output_buffer, gchar *label_text)
{
	text->textProgram = gtk_builder_get_object(builder, text_program);
	text->programBuffer = gtk_builder_get_object(builder, program_buffer);
	text->textOutput = gtk_builder_get_object(builder, text_output);
	text->outputBuffer = gtk_builder_get_object(builder, output_buffer);
	text->label = gtk_builder_get_object(builder, label_text);
	text->file.file = NULL;
	text->file.filename = NULL;

	g_object_ref(text->textProgram);
	g_object_ref(text->textOutput);
	g_object_ref(text->programBuffer);
	g_object_ref(text->outputBuffer);
	g_object_ref(text->label);

	gtk_label_set_text(GTK_LABEL(text->label), UNSAVED_FILE);

	text->isSaved = TRUE;
}

void textStructDestroy(textStruct *text)
{
	g_free(text->file.filename);
	g_object_unref(text->textProgram);
	g_object_unref(text->textOutput);
	g_object_unref(text->programBuffer);
	g_object_unref(text->outputBuffer);
	g_object_unref(text->label);
}

gboolean set_saved_cb(GtkWidget *widget, gpointer data)
{
	textStruct *label = (textStruct *)data;
	label->isSaved = TRUE;

	return TRUE;
}

gboolean new_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *newFile= (textStruct *)data;
	gboolean returnValue = FALSE;

	returnValue = save_text_view_to_file_cb(NULL, (gpointer)newFile);

	if(returnValue){
		if(newFile->file.filename != NULL){
			g_free(newFile->file.filename);
			newFile->file.filename = NULL;
		}
		fileClose(&newFile->file, GTK_TEXT_BUFFER(newFile->outputBuffer));
		
		GtkTextIter start;
		GtkTextIter end;
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(newFile->programBuffer), &start);
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(newFile->programBuffer), &end);
		g_signal_handler_block((gpointer)newFile->programBuffer, newFile->programChangedHandlerId);
		gtk_text_buffer_delete(GTK_TEXT_BUFFER(newFile->programBuffer), &start, &end);
		g_signal_handler_unblock((gpointer)newFile->programBuffer, newFile->programChangedHandlerId);
		gtk_label_set_text(GTK_LABEL(newFile->label), UNSAVED_FILE);
	}

	return returnValue;
}

//Crear Back Up
gboolean save_text_view_to_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *fileSave = (textStruct *)data;
	GtkTextIter start;
	GtkTextIter end;
	gboolean returnValue = FALSE;
	char *text;

	if(fileSave->file.filename != NULL){
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(fileSave->programBuffer), &start);
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(fileSave->programBuffer), &end);

		fileClose(&fileSave->file, GTK_TEXT_BUFFER(fileSave->outputBuffer));
		if(fileWrite(&fileSave->file, GTK_TEXT_BUFFER(fileSave->outputBuffer))){
			text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(fileSave->programBuffer), &start, &end, TRUE);
			if(fputs(text, fileSave->file.file) == EOF){
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), "Error: File: \"", FALSE);
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), fileSave->file.filename, FALSE);
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), "\" could not be saved.", TRUE);
			}else{
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), "Document: \"", FALSE);
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), fileSave->file.filename, FALSE);
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), "\" saved.", TRUE);
				gchar *name = g_filename_display_basename(fileSave->file.filename);
				gtk_label_set_text(GTK_LABEL(fileSave->label), name);
				g_free(name);
				returnValue = TRUE;
			}
			fileClose(&fileSave->file, GTK_TEXT_BUFFER(fileSave->outputBuffer));
		}	
	}else{
		returnValue = save_as_text_view_to_file_cb(NULL, fileSave);
	}

	return returnValue;
}

gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data)
{
	textStruct *fileLoad = (textStruct *)data;
	int fileSize;
	int character;
	gboolean returnValue = TRUE;
	int j;

	if(fileLoad->file.file == NULL)
		returnValue = FALSE;
		
	if(returnValue){
		rewind(fileLoad->file.file);
		for (fileSize = 0; (character = getc(fileLoad->file.file)) != EOF; ++fileSize)
			j = 0;

		gchar *text = (gchar *)calloc(sizeof(gchar), fileSize + 1);

		rewind(fileLoad->file.file);
		for (j = 0; j < fileSize; ++j)
			text[j] = (gchar)getc(fileLoad->file.file);	

		g_signal_handler_block((gpointer)fileLoad->programBuffer, fileLoad->programChangedHandlerId);
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(fileLoad->programBuffer), text, -1);
		g_signal_handler_unblock((gpointer)fileLoad->programBuffer, fileLoad->programChangedHandlerId);

		rewind(fileLoad->file.file);
		free(text);

		fileClose(&fileLoad->file, GTK_TEXT_BUFFER(fileLoad->outputBuffer));
	}

	return returnValue;
}

gboolean open_file_cb(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	textStruct *openFile = (textStruct *)data;
	gboolean returnValue = TRUE;

	dialog = gtk_file_chooser_dialog_new("Open File",
			NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			"_Cancel",
			GTK_RESPONSE_CANCEL,
			"_Open",
			GTK_RESPONSE_ACCEPT,
			NULL);

	GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, TERMINATION);
	gtk_file_chooser_set_filter(chooser, filter);
	gint res = gtk_dialog_run(GTK_DIALOG(dialog));

	if(res == GTK_RESPONSE_ACCEPT){

		g_free(openFile->file.filename);
		fileClose(&openFile->file, GTK_TEXT_BUFFER(openFile->outputBuffer));

		openFile->file.filename = gtk_file_chooser_get_filename(chooser);
		if(!fileOpen(&openFile->file, GTK_TEXT_BUFFER(openFile->outputBuffer))){
			returnValue = FALSE;
			g_free(openFile->file.filename);
		}else{
			gchar *name = g_filename_display_basename(openFile->file.filename);
			gtk_label_set_text(GTK_LABEL(openFile->label), name);
			g_free(name);
		}
	}

	gtk_widget_destroy(dialog);

	return returnValue;
}

gboolean save_as_text_view_to_file_cb(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	textStruct *fileSave = (textStruct *)data;
	gboolean returnValue = FALSE;

	dialog = gtk_file_chooser_dialog_new("Save File",
			NULL,
			GTK_FILE_CHOOSER_ACTION_SAVE,
			"_Cancel",
			GTK_RESPONSE_CANCEL,
			"_Save",
			GTK_RESPONSE_ACCEPT,
			NULL);

	GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
	gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.asm");
	gtk_file_chooser_set_filter(chooser, filter);

	if(fileSave->file.filename != NULL){
		gtk_file_chooser_set_filename(chooser, fileSave->file.filename);
	}else{
		gtk_file_chooser_set_current_name(chooser, UNSAVED_FILE);
	}


	gint res = gtk_dialog_run(GTK_DIALOG(dialog));

	if(res == GTK_RESPONSE_ACCEPT){
		if(fileSave->file.filename != NULL)
			g_free(fileSave->file.filename);
		fileClose(&fileSave->file, GTK_TEXT_BUFFER(fileSave->outputBuffer));

		fileSave->file.filename = gtk_file_chooser_get_filename(chooser);
		returnValue = save_text_view_to_file_cb(NULL, (gpointer)fileSave);
		if(returnValue){
			gchar *name = g_filename_display_basename(fileSave->file.filename);
			gtk_label_set_text(GTK_LABEL(fileSave->label), name);
			g_free(name);
		}
	}

	gtk_widget_destroy(dialog);
	
	return returnValue;
}
