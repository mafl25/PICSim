#include "outputbuffer.h"
#include <gtk/gtk.h>
#include <assert.h>

static GtkTextBuffer *outputBuffer;
static GtkTextView *outputText;

void set_output_buffer(GtkTextBuffer *buffer)
{
	outputBuffer = buffer;
	g_object_ref(outputBuffer);
}

void destroy_output_buffer(void)
{
	g_object_unref(outputBuffer);
}

void set_output_text(GtkTextView *text)
{
	outputText = text;
}

void destroy_output_text(void)
{
	g_object_unref(outputText);
}

//Prints a message to the output buffer, adding an \n character at the end.
//Do I really need the add new line thing? I could just add the \n in the message...
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

	if(outputText){
		gtk_text_buffer_get_end_iter(outputBuffer, &end);
		gtk_text_view_scroll_to_iter(outputText, &end, 0.1, FALSE, 1.0, 0.0);	
	}
}
