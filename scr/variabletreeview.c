#include "variabletreeview.h"
#include "datastructure.h"
#include "regAndMem.h"

#include <gtk/gtk.h>
#include <stdlib.h>

#define N_COLUMNS 3

static GtkTreeView *variableTreeView;
static int rowNum = 0;

gboolean variable_tree_view_init(GtkTreeView *variableTree)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	variableTreeView = variableTree;

	GtkListStore *variableListStore = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
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

	rowNum = 0;

	return TRUE;
}

gboolean variable_tree_view_append_row(const gchar *variable, int address, int value)
{
	GtkTreeIter iter;
	GtkListStore *variableListStore = GTK_LIST_STORE(gtk_tree_view_get_model(variableTreeView));
	GString *hexWord = g_string_sized_new(6);

	gtk_list_store_append(variableListStore, &iter);

	gtk_list_store_set(variableListStore, &iter, 0, variable, -1);
	g_string_printf(hexWord, "0x%02X", address);
	gtk_list_store_set(variableListStore, &iter, 1, hexWord->str, -1);
	g_string_printf(hexWord, "0x%02X", value);
	gtk_list_store_set(variableListStore, &iter, 2, hexWord->str, -1);

	rowNum++;

	g_string_free(hexWord, TRUE);

	return TRUE;
} 

void variable_tree_view_update(void) 
{
	GtkTreeIter iter;
	GtkListStore *variableListStore = GTK_LIST_STORE(gtk_tree_view_get_model(variableTreeView));
	GString *hexWord = g_string_sized_new(6);

	if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(variableListStore), &iter)) {
		uint16_t value = 0;
		char *address;
		int j;
		for (j = 0; j < rowNum; ++j) {
			gtk_tree_model_get(GTK_TREE_MODEL(variableListStore), &iter, 1, &address, -1);
			value = getDataMemory(strtol(address, NULL, 0));
			g_string_printf(hexWord, "0x%02X", value);
			gtk_list_store_set(variableListStore, &iter, 2, hexWord->str, -1);
			gtk_tree_model_iter_next(GTK_TREE_MODEL(variableListStore), &iter);
			g_free(address);
		}
	}

	g_string_free(hexWord, TRUE);
}

#undef N_COLUMNS

