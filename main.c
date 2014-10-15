#include "settings.h"
#include "datastructure.h"
#include "filemanager.h"
#include "filedialog.h"
#include "menubar.h"
#include "toolbar.h"
#include "commoncallbacks.h"
#include "outputbuffer.h"

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

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "project_gui.ui", NULL);

	window = gtk_builder_get_object(builder, "window1");
	gtk_window_maximize(GTK_WINDOW(window));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);


	textStruct openText;
	textStructInit(&openText, builder);

	set_output_buffer(GTK_TEXT_BUFFER(openText.outputBuffer));

	gchar *file_patterns[] = {"*.asm", "*.pic", "NULL"};
	set_file_filter(file_patterns);

	build_menu_bar(builder, &openText, GTK_WINDOW(window));
	build_toolbar(builder, &openText, GTK_WINDOW(window));
	build_settings(builder, &openText);
	set_settings();

	openText.programChangedHandlerId = g_signal_connect(openText.programBuffer, "changed", 
			G_CALLBACK(program_changed_cd), (gpointer)&openText);

	gtk_main();
	g_object_unref(G_OBJECT(builder));

	fileClose(&openText.file, openText.filename, TRUE);
	file_filter_unref();
	textStructDestroy(&openText);

	return 0;
}



