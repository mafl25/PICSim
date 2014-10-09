#include "outputbuffer.h"
#include <gtk/gtk.h>
#include <assert.h>

//Prints a message to the output buffer, adding an \n character at the end.
void outputPrint(GtkTextBuffer *outputBuffer, const gchar *message, gboolean addNewline)
{
	assert(outputBuffer);
	assert(message);

	GtkTextIter end;
	gtk_text_buffer_get_end_iter(outputBuffer, &end);
	gtk_text_buffer_insert(outputBuffer, &end, message, -1);
	if(addNewline){
		gtk_text_buffer_get_end_iter(outputBuffer, &end);
		gtk_text_buffer_insert(outputBuffer, &end, "\n", -1);
	}
}
