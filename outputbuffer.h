#ifndef OUTPUT_BUFFER
#define OUTPUT_BUFFER 

#include <gtk/gtk.h>

void set_output_buffer(GtkTextBuffer *buffer);
GtkTextBuffer * get_output_buffer(void);
void output_print(const gchar *message, gboolean addNewline);

#endif
