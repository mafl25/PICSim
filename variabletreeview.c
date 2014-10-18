#include "variabletreeview.h"
#include "datastructure.h"

#include <gtk/gtk.h>

#define N_COLUMNS 3

static GtkTreeView *variableTreeView;

gboolean variable_tree_view_init(GtkTreeView *variableTree)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	variableTreeView = variableTree;

	GtkListStore *variableListStore = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
	gtk_tree_view_set_model(variableTreeView, GTK_TREE_MODEL(variableListStore));
	g_object_unref(variableListStore);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Variable", renderer, "text", 0, NULL);
	gtk_tree_view_append_column(variableTreeView, column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Address", renderer, "text", 1, NULL);
	gtk_tree_view_append_column(variableTreeView, column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Value", renderer, "text", 2, NULL);
	gtk_tree_view_append_column(variableTreeView, column);

	g_object_ref(variableTreeView);

	return TRUE;
}

gboolean variable_tree_view_clear(void)
{
	GtkListStore *variableListStore = GTK_LIST_STORE(gtk_tree_view_get_model(variableTreeView));
	gtk_list_store_clear(variableListStore);

	return TRUE;
}

gboolean variable_tree_view_append_row(const gchar *variable, int address, int value)
{
	GtkTreeIter iter;
	GtkListStore *variableListStore = GTK_LIST_STORE(gtk_tree_view_get_model(variableTreeView));

	gtk_list_store_append(variableListStore, &iter);

	gtk_list_store_set(variableListStore, &iter, 0, variable, -1);
	gtk_list_store_set(variableListStore, &iter, 1, address, -1);
	gtk_list_store_set(variableListStore, &iter, 2, value, -1);

	return TRUE;
} 




