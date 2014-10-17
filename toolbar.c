#include "toolbar.h"
#include "datastructure.h"
#include "menubar.h"
#include "commoncallbacks.h"

#include <gtk/gtk.h>

void build_toolbar(GtkBuilder *builder, textStruct *text, GtkWindow *window)
{
	GObject *toolbar_button;

	toolbar_button = gtk_builder_get_object(builder, "open_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(open_file_cb), (gpointer)(text));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(file_load_to_text_view_cb), (gpointer)(text));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(set_saved_cb), (gpointer)(text));

	toolbar_button = gtk_builder_get_object(builder, "save_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(save_text_view_to_file_cb), (gpointer)(text));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(set_saved_cb), (gpointer)(text));

	toolbar_button = gtk_builder_get_object(builder, "new_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(new_file_cb), (gpointer)(text));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(set_saved_cb), (gpointer)(text));

	toolbar_button = gtk_builder_get_object(builder, "undo_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(undo_text_cb), (gpointer)(text));

	toolbar_button = gtk_builder_get_object(builder, "redo_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(redo_text_cb), (gpointer)(text));

	toolbar_button = gtk_builder_get_object(builder, "copy_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(copy_text_cb), (gpointer)window);

	toolbar_button = gtk_builder_get_object(builder, "cut_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(cut_text_cb), (gpointer)window);

	toolbar_button = gtk_builder_get_object(builder, "paste_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(paste_text_cb), (gpointer)window);

	toolbar_button = gtk_builder_get_object(builder, "build_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(build_program_cb), (gpointer)(text));
}
