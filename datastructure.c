#include "datastructure.h"
#include "filedialog.h"
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

void textStructInit(textStruct *text, GtkBuilder *builder)
{
	text->programBuffer = G_OBJECT(gtk_source_buffer_new(NULL));
	text->programTextView = G_OBJECT(gtk_source_view_new_with_buffer(
				GTK_SOURCE_BUFFER(text->programBuffer)));
	GObject *scrolled = gtk_builder_get_object(builder, "text_scrolled");
	gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(text->programTextView));
	gtk_widget_show(GTK_WIDGET(text->programTextView));

	text->outputTextView = gtk_builder_get_object(builder, "text_output");
	//Not sure if I use outputTextView so far
	text->outputBuffer = gtk_builder_get_object(builder, "output_buffer");
	text->label = gtk_builder_get_object(builder, "text_name");
	text->file = NULL;
	text->filename = NULL;

	g_object_ref(text->programTextView);
	g_object_ref(text->outputTextView);
	g_object_ref(text->programBuffer);
	g_object_ref(text->outputBuffer);
	g_object_ref(text->label);

	gtk_label_set_text(GTK_LABEL(text->label), UNSAVED_FILE);

	text->isSaved = TRUE;
}

void textStructDestroy(textStruct *text)
{
	g_free(text->filename);
	g_object_unref(text->programTextView);
	g_object_unref(text->outputTextView);
	g_object_unref(text->programBuffer);
	g_object_unref(text->outputBuffer);
	g_object_unref(text->label);
}
