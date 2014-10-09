#ifndef OUTPUT_BUFFER
#define OUTPUT_BUFFER 

#include <gtk/gtk.h>

void outputPrint(GtkTextBuffer *outputBuffer, const gchar *message, gboolean addNewline);

#endif
