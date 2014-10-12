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
	GObject *toolbar_secondary;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "project_gui.ui", NULL);

	window = gtk_builder_get_object(builder, "window1");
	gtk_window_maximize(GTK_WINDOW(window));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	textStruct openText;
	textStructInit(&openText, builder);

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

	toolbar_button = gtk_builder_get_object(builder, "line_number_check");
	g_signal_connect(toolbar_button, "toggled", G_CALLBACK(line_number_toggle_cb), (gpointer)&openText);

	toolbar_button = gtk_builder_get_object(builder, "right_margin");
	toolbar_secondary = gtk_builder_get_object(builder, "right_margin_width");
	g_signal_connect(toolbar_button, "toggled", G_CALLBACK(right_margin_toggle_cb), (gpointer)&openText);
	g_signal_connect(toolbar_button, "toggled", G_CALLBACK(toggle_sensitive_cb), (gpointer)toolbar_secondary);

	g_signal_connect(toolbar_secondary, "value-changed", G_CALLBACK(right_margin_width_set_cb), (gpointer)&openText);

	toolbar_button = gtk_builder_get_object(builder, "text_wrap");
	toolbar_secondary = gtk_builder_get_object(builder, "split_word");
	g_signal_connect(toolbar_button, "toggled", G_CALLBACK(wrap_text_cb), (gpointer)&openText);
	g_signal_connect(toolbar_button, "toggled", G_CALLBACK(toggle_sensitive_cb), (gpointer)toolbar_secondary);

	toolbar_button = gtk_builder_get_object(builder, "highlight_line");
	g_signal_connect(toolbar_button, "toggled", G_CALLBACK(highlight_line_cb), (gpointer)&openText);

	openText.programChangedHandlerId = g_signal_connect(openText.programBuffer, "changed", 
			G_CALLBACK(program_changed_cd), (gpointer)&openText);

	g_signal_connect(toolbar_secondary, "toggled", G_CALLBACK(wrap_mode_mode_change_cb), (gpointer)&openText);

	gtk_main();
	g_object_unref(G_OBJECT(builder));

	fileClose(&openText.file, openText.filename, GTK_TEXT_BUFFER(openText.outputBuffer));
	textStructDestroy(&openText);

	return 0;
}



