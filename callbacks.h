#ifndef CALLBACKS
#define CALLBACKS
#include <gtk/gtk.h>
#include "filemanager.h"
#define TERMINATION "*.asm"

typedef struct
{
	FILE *file;
	gchar *filename;
	GObject *programTextView;
	GObject *programBuffer;
	GObject *outputTextView;
	GObject *outputBuffer;
	GObject *label;
	gboolean isSaved;
	gulong programChangedHandlerId;
}textStruct;

void textStructInit(textStruct *text, GtkBuilder *builder, gchar *text_program, gchar *text_output,
		gchar *program_buffer, gchar *output_buffer, gchar *label_text);
void textStructDestroy(textStruct *text);

gboolean set_saved_cb(GtkWidget *widget, gpointer data);
gboolean program_changed_cd(GtkWidget *widget, gpointer data);

gboolean open_file_cb(GtkWidget *widget, gpointer data);
gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data);
gboolean save_text_view_to_file_cb(GtkWidget *widget, gpointer data);
gboolean save_as_text_view_to_file_cb(GtkWidget *widget, gpointer data);
gboolean new_file_cb(GtkWidget *widget, gpointer data);

gboolean copy_text(GtkWidget *widget, gpointer data);
gboolean cut_text(GtkWidget *widget, gpointer data);
gboolean paste_text(GtkWidget *widget, gpointer data);
gboolean delete_text(GtkWidget *widget, gpointer data);

#endif

