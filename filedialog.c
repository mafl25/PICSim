#include "filedialog.h"
#include "filemanager.h"
#include "outputbuffer.h"
#include <gtk/gtk.h>
#include <stdlib.h>


gboolean new_file(FILE **file, gchar **filename, GtkTextBuffer *text_buffer, GtkTextBuffer *output_buffer)
{
	gboolean returnValue = FALSE;
	
	returnValue = save_text_view_to_file(file, filename, text_buffer, output_buffer);

	if(returnValue){
		if(*filename != NULL){
			g_free(*filename);
			*filename = NULL;
		}

		fileClose(file, *filename, output_buffer);
		
		GtkTextIter start;
		GtkTextIter end;

		gtk_text_buffer_get_start_iter(text_buffer, &start);
		gtk_text_buffer_get_end_iter(text_buffer, &end);
		gtk_text_buffer_delete(text_buffer, &start, &end);
	}

	return returnValue;
}

gboolean save_text_view_to_file(FILE **file, gchar **filename, GtkTextBuffer *text_buffer, GtkTextBuffer *output_buffer)
{
	GtkTextIter start;
	GtkTextIter end;
	gboolean returnValue = FALSE;
	char *text;

	if(*filename != NULL){
		g_print("Hola MAria\n");
		gtk_text_buffer_get_start_iter(text_buffer, &start);
		gtk_text_buffer_get_end_iter(text_buffer, &end);

		fileClose(file, *filename, output_buffer);
		if(fileWrite(file, *filename, output_buffer)){
			text = gtk_text_buffer_get_text(text_buffer, &start, &end, TRUE);
			if(fputs(text, *file) == EOF){
				if(output_buffer != NULL){
					outputPrint(output_buffer, "Error: File: \"", FALSE);
					outputPrint(output_buffer, *filename, FALSE);
					outputPrint(output_buffer, "\" could not be saved.", TRUE);
				}
			}else{
				//Maybe need to free text
				if(output_buffer != NULL){
					outputPrint(output_buffer, "Document: \"", FALSE);
					outputPrint(output_buffer, *filename, FALSE);
					outputPrint(output_buffer, "\" saved.", TRUE);
				}
				returnValue = TRUE;
			}
			fileClose(file, *filename, output_buffer);
		}
	}else{
			g_print("Hola\n");
			returnValue = save_as_text_view_to_file(file, filename, text_buffer, output_buffer);
	}	

	return returnValue;
}


gboolean open_file(FILE **file, gchar **filename, /*gchar *termination,*/ GtkTextBuffer *output_buffer)
{
	GtkWidget *dialog;
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
	gtk_file_filter_add_pattern(filter, "*.asm");
	gtk_file_chooser_set_filter(chooser, filter);
	gint res = gtk_dialog_run(GTK_DIALOG(dialog));

	if(res == GTK_RESPONSE_ACCEPT){

		g_free(*filename);
		fileClose(file, *filename, output_buffer);

		*filename = gtk_file_chooser_get_filename(chooser);
		if(!fileOpen(file, *filename, output_buffer)){
			returnValue = FALSE;
			g_free(*filename);
		}
	}

	gtk_widget_destroy(dialog);

	return returnValue;
}

gboolean save_as_text_view_to_file(FILE **file, gchar **filename, GtkTextBuffer *text_buffer, GtkTextBuffer *output_buffer)
{
	GtkWidget *dialog;
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
//	GtkFileFilter *filter = gtk_file_filter_new();
//	gtk_file_filter_add_pattern(filter, "*.asm");
//	gtk_file_chooser_set_filter(chooser, filter);

	if(*filename != NULL){
		gtk_file_chooser_set_filename(chooser, *filename);
	}else{
		gtk_file_chooser_set_current_name(chooser, UNSAVED_FILE);
	}


	gint res = gtk_dialog_run(GTK_DIALOG(dialog));

	if(res == GTK_RESPONSE_ACCEPT){
		if(*filename != NULL)
			g_free(*filename);
		fileClose(file, *filename, output_buffer);

		*filename = gtk_file_chooser_get_filename(chooser);
		returnValue = save_text_view_to_file(file, filename, text_buffer, output_buffer);
	}

	gtk_widget_destroy(dialog);
	
	return returnValue;
}
