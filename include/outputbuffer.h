#ifndef OUTPUT_BUFFER
#define OUTPUT_BUFFER 

#include <gtk/gtk.h>
#include "vstring.h"

void set_output_buffer(GtkTextBuffer *buffer);
void destroy_output_buffer(void);
void set_output_text(GtkTextView *text);
void destroy_output_text(void);
void output_print(const gchar *message, gboolean addNewline);
void output_print_string(const struct VString *string, gboolean newLine);

#endif
