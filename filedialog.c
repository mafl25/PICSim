#include "filedialog.h"
#include "filemanager.h"
#include "outputbuffer.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

static GtkFileFilter *file_filter;

void set_file_filter(gchar **file_patterns)
{
	static gboolean wasCalled = FALSE;
	
	if(!wasCalled){
		wasCalled = TRUE;
		file_filter = gtk_file_filter_new();
		g_object_ref(G_OBJECT(file_filter));
		int j;
		for (j = 0; strcmp(file_patterns[j], "NULL") != 0; ++j)
		{
			gtk_file_filter_add_pattern(file_filter, file_patterns[j]);
		}
	}
}

//Check if I used this one
void file_filter_unref(void)
{
	g_object_unref(file_filter);
}

gboolean new_file(FILE **file, gchar **filename, GtkTextBuffer *text_buffer, gboolean print_output)
{
	gboolean returnValue = FALSE;
	
	returnValue = save_text_view_to_file(file, filename, text_buffer, print_output);

	if(returnValue){
		if(*filename != NULL){
			g_free(*filename);
			*filename = NULL;
		}

		fileClose(file, *filename, print_output);
		
		GtkTextIter start;
		GtkTextIter end;

		gtk_text_buffer_get_start_iter(text_buffer, &start);
		gtk_text_buffer_get_end_iter(text_buffer, &end);
		gtk_text_buffer_delete(text_buffer, &start, &end);
	}

	return returnValue;
}

gboolean save_text_view_to_file(FILE **file, gchar **filename, GtkTextBuffer *text_buffer, gboolean print_output)
{
	GtkTextIter start;
	GtkTextIter end;
	gboolean returnValue = FALSE;
	char *text;

	if(*filename != NULL){
		gtk_text_buffer_get_start_iter(text_buffer, &start);
		gtk_text_buffer_get_end_iter(text_buffer, &end);

		fileClose(file, *filename, print_output);
		if(fileWrite(file, *filename, print_output)){
			text = gtk_text_buffer_get_text(text_buffer, &start, &end, TRUE);
			if(fputs(text, *file) == EOF){
				if(print_output){
					output_print("Error: File: \"", FALSE);
					output_print(*filename, FALSE);
					output_print("\" could not be saved.", TRUE);
				}
			}else{
				if(print_output){
					output_print("Document: \"", FALSE);
					output_print(*filename, FALSE);
					output_print("\" saved.", TRUE);
				}
				returnValue = TRUE;
			}
			g_free(text);
			fileClose(file, *filename, print_output);
		}
	}else{
			returnValue = save_as_text_view_to_file(file, filename, text_buffer, print_output);
	}	

	return returnValue;
}


gboolean open_file(FILE **file, gchar **filename, gboolean print_output)
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
	gtk_file_chooser_set_filter(chooser, file_filter);
	gint res = gtk_dialog_run(GTK_DIALOG(dialog));

	if(res == GTK_RESPONSE_ACCEPT){

		g_free(*filename);
		fileClose(file, *filename, print_output);

		*filename = gtk_file_chooser_get_filename(chooser);
		if(!fileOpen(file, *filename, print_output)){
			returnValue = FALSE;
			g_free(*filename);
		}
	}

	gtk_widget_destroy(dialog);

	return returnValue;
}

gboolean save_as_text_view_to_file(FILE **file, gchar **filename, GtkTextBuffer *text_buffer, gboolean print_output)
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
	gtk_file_chooser_set_filter(chooser, file_filter);

	if(*filename != NULL){
		gtk_file_chooser_set_filename(chooser, *filename);
	}else{
		gtk_file_chooser_set_current_name(chooser, UNSAVED_FILE);
	}


	gint res = gtk_dialog_run(GTK_DIALOG(dialog));

	if(res == GTK_RESPONSE_ACCEPT){
		if(*filename != NULL)
			g_free(*filename);
		fileClose(file, *filename, print_output);

		*filename = gtk_file_chooser_get_filename(chooser);
		returnValue = save_text_view_to_file(file, filename, text_buffer, print_output);
	}

	gtk_widget_destroy(dialog);
	
	return returnValue;
}
