#ifndef FILE_DIALOG
#define FILE_DIALOG
#include <gtk/gtk.h>
#include "filemanager.h"

#define TERMINATION "*.asm"
#define UNSAVED_FILE "Unsaved Document"
#define UNSAVED_FILE_MOD "*Unsave Document"

typedef struct
{
	MFile file;
	GObject *textProgram;
	GObject *programBuffer;
	GObject *textOutput;
	GObject *outputBuffer;
	GObject *label;
	gboolean isSaved;
	gulong programChangedHandlerId;
}textStruct;

void textStructInit(textStruct *text, GtkBuilder *builder, gchar *text_program, gchar *text_output,
		gchar *program_buffer, gchar *output_buffer, gchar *label_text);
void textStructDestroy(textStruct *text);

gboolean set_saved_cb(GtkWidget *widget, gpointer data);

gboolean open_file_cb(GtkWidget *widget, gpointer data);
gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data);
gboolean save_text_view_to_file_cb(GtkWidget *widget, gpointer data);
gboolean save_as_text_view_to_file_cb(GtkWidget *widget, gpointer data);
gboolean new_file_cb(GtkWidget *widget, gpointer data);

#endif
