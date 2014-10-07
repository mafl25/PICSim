#include "outputbuffer.h"
#include <gtk/gtk.h>

//Prints a message to the output buffer, adding an \n character at the end.
void outputPrint(GtkTextBuffer *outputBuffer, gchar * message, gboolean addNewline)
{
	GtkTextIter end;
	gtk_text_buffer_get_end_iter(outputBuffer, &end);
	gtk_text_buffer_insert(outputBuffer, &end, message, -1);
	if(addNewline){
		gtk_text_buffer_get_end_iter(outputBuffer, &end);
		gtk_text_buffer_insert(outputBuffer, &end, "\n", -1);
	}
}
