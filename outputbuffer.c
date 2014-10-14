#include "outputbuffer.h"
#include <gtk/gtk.h>
#include <assert.h>

static GtkTextBuffer *outputBuffer;

void set_output_buffer(GtkTextBuffer *buffer)
{
	outputBuffer = buffer;
}

GtkTextBuffer * get_ouput_buffer(void)
{
	return outputBuffer;
}

//Prints a message to the output buffer, adding an \n character at the end.
void output_print(const gchar *message, gboolean addNewline)
{
	assert(outputBuffer != NULL);
	assert(message);

	GtkTextIter end;
	gtk_text_buffer_get_end_iter(outputBuffer, &end);
	gtk_text_buffer_insert(outputBuffer, &end, message, -1);
	if(addNewline){
		gtk_text_buffer_get_end_iter(outputBuffer, &end);
		gtk_text_buffer_insert(outputBuffer, &end, "\n", -1);
	}
}
