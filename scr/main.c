#include "settings.h"
#include "datastructure.h"
#include "filemanager.h"
#include "filedialog.h"
#include "menubar.h"
#include "toolbar.h"
#include "commoncallbacks.h"
#include "outputbuffer.h"
#include "variabletreeview.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gtksourceview/gtksource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

GString *get_dir_app(char *);

int main(int argc, char *argv[])
{
	GtkBuilder *builder;
	GObject *window;
	GObject *outputBuffer;
	GObject *outputText;
	GObject *varTreeView;

	gtk_init(&argc, &argv);

	char *ui_dir = "project_gui.ui";
	GString *buf = get_dir_app(argv[0]);
	g_string_append(buf, ui_dir);
	builder = gtk_builder_new_from_file(buf->str);
	g_string_free(buf, TRUE);

	window = gtk_builder_get_object(builder, "window1");
	gtk_window_maximize(GTK_WINDOW(window));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	textStruct openText;
	text_struct_init(&openText, builder);

	outputBuffer = gtk_builder_get_object(builder, "output_buffer");
	set_output_buffer(GTK_TEXT_BUFFER(outputBuffer));

	outputText = gtk_builder_get_object(builder, "text_output");
	set_output_text(GTK_TEXT_VIEW(outputText));

	gchar *file_patterns[] = {"*.asm", "*.asmmed", "*.pic", "NULL"};
	set_file_filter(file_patterns);

	build_menubar(builder, &openText, GTK_WINDOW(window));
	build_toolbar(builder, &openText, GTK_WINDOW(window));
	build_settings(builder, &openText);
	set_settings(argv[0]);

	openText.programChangedHandlerId = g_signal_connect(openText.programBuffer, "changed", 
			G_CALLBACK(program_changed_cd), (gpointer)&openText);

	varTreeView = gtk_builder_get_object(builder, "variable_view_tree");
	variable_tree_view_init(GTK_TREE_VIEW(varTreeView));


	gtk_main();
	g_object_unref(G_OBJECT(builder));

	fileClose(&openText.file, openText.filename, TRUE);
	file_filter_unref();
	destroy_output_buffer();
	destroy_output_text();
	text_struct_destroy(&openText);
	free_setting();

	return 0;
}

GString *get_dir_app(char *directory)
{
	GString *buf = g_string_sized_new(100);
	g_string_append(buf, directory);
	
	size_t j;
	for (j = buf->len; j ; --j) {
		if (buf->str[j] == '/' || buf->str[j] == '\\')
			break;
	}

	g_string_truncate(buf, j + 1);

	return buf;
}
