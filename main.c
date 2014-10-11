#include "filemanager.h"
#include "outputbuffer.h"
#include "callbacks.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gtksourceview/gtksource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	GtkBuilder *builder;
	GObject *window;
	GObject *menu_item;
	GObject *toolbar_button;

	gtk_init(&argc, &argv);


	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "project_gui.ui", NULL);

	window = gtk_builder_get_object(builder, "window1");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);


	textStruct openText;
	textStructInit(&openText, builder, "text_program", "text_output", "program_buffer", "output_buffer",
			"text_name");

	menu_item = gtk_builder_get_object(builder, "quit_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(gtk_main_quit), NULL);

	menu_item = gtk_builder_get_object(builder, "save_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(save_text_view_to_file_cb), (gpointer)(&openText));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(&openText));

	menu_item = gtk_builder_get_object(builder, "save_as_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(save_as_text_view_to_file_cb), (gpointer)(&openText));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(&openText));
	
	menu_item = gtk_builder_get_object(builder, "new_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(new_file_cb), (gpointer)(&openText));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(&openText));
	
	menu_item = gtk_builder_get_object(builder, "open_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(open_file_cb), (gpointer)(&openText));
	g_signal_connect(menu_item, "activate", G_CALLBACK(file_load_to_text_view_cb), (gpointer)(&openText));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(&openText));

	menu_item = gtk_builder_get_object(builder, "cut_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(cut_text), (gpointer)window);

	menu_item = gtk_builder_get_object(builder, "copy_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(copy_text), (gpointer)window);

	menu_item = gtk_builder_get_object(builder, "paste_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(paste_text), (gpointer)window);

	menu_item = gtk_builder_get_object(builder, "delete_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(delete_text), (gpointer)window);

	toolbar_button = gtk_builder_get_object(builder, "open_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(open_file_cb), (gpointer)(&openText));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(file_load_to_text_view_cb), (gpointer)(&openText));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(set_saved_cb), (gpointer)(&openText));

	toolbar_button = gtk_builder_get_object(builder, "save_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(save_text_view_to_file_cb), (gpointer)(&openText));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(set_saved_cb), (gpointer)(&openText));

	toolbar_button = gtk_builder_get_object(builder, "new_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(new_file_cb), (gpointer)(&openText));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(set_saved_cb), (gpointer)(&openText));

	toolbar_button = gtk_builder_get_object(builder, "copy_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(copy_text), (gpointer)window);

	toolbar_button = gtk_builder_get_object(builder, "cut_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(cut_text), (gpointer)window);

	toolbar_button = gtk_builder_get_object(builder, "paste_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(paste_text), (gpointer)window);

	openText.programChangedHandlerId = g_signal_connect(openText.programBuffer, "changed", 
			G_CALLBACK(program_changed_cd), (gpointer)&openText);

	g_object_unref(G_OBJECT(builder));
	gtk_main();

	fileClose(&openText.file, openText.filename, GTK_TEXT_BUFFER(openText.outputBuffer));
	textStructDestroy(&openText);

	return 0;
}



