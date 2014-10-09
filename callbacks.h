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

gboolean set_saved_cb(GtkWidget *widget, gpointer data);

gboolean open_file_cb(GtkWidget *widget, gpointer data);
gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data);
gboolean save_text_view_to_file_cb(GtkWidget *widget, gpointer data);
gboolean save_as_text_view_to_file_cb(GtkWidget *widget, gpointer data);
gboolean new_file_cb(GtkWidget *widget, gpointer data);

#endif
