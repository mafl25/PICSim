#ifndef FILE_DIALOG
#define FILE_DIALOG
#include <gtk/gtk.h>
#define UNSAVED_FILE "Unsaved Document"
#define UNSAVED_FILE_MOD "*Unsave Document"

void set_file_filter(gchar **file_patterns);
void file_filter_unref(void);

gboolean open_file(FILE **file, gchar **filename, /*gchar *termination,*/ gboolean print_output);
gboolean save_text_view_to_file(FILE **file, gchar **filename, GtkTextBuffer *text_buffer, gboolean print_output);
gboolean save_as_text_view_to_file(FILE **file, gchar **filename, GtkTextBuffer *text_buffer, gboolean print_output);
gboolean new_file(FILE **file, gchar **filename, GtkTextBuffer *text_buffer, gboolean print_output);

#endif
