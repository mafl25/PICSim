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

void textStructInit(textStruct *text, GtkBuilder *builder);
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

gboolean toggle_sensitive_cb(GtkWidget *widget, gpointer data);

gboolean line_number_toggle_cb(GtkWidget *widget, gpointer data);
gboolean right_margin_toggle_cb(GtkWidget *widget, gpointer data);
gboolean right_margin_width_set_cb(GtkWidget *widget, gpointer data);

gboolean wrap_text_cb(GtkWidget *widget, gpointer data);
gboolean wrap_mode_mode_change_cb(GtkWidget *widget, gpointer data);

gboolean highlight_line_cb(GtkWidget *widget, gpointer data);
gboolean highlight_bracket_cb(GtkWidget *widget, gpointer data);
gboolean tab_width_change_cb(GtkWidget *widget, gpointer data);
gboolean space_tab_change_cb(GtkWidget *widget, gpointer data);
gboolean auto_indent_change_cb(GtkWidget *widget, gpointer data);







#endif

