#include "programtreeview.h"
#include "instructionset.h"
#include <stdlib.h>

#define N_COLUMNS 3

static GtkTreeView *programTreeView;

gboolean program_tree_view_init(GtkTreeView *programTree)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	programTreeView = programTree;

	GtkListStore *programListStore = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(programTreeView, GTK_TREE_MODEL(programListStore));
	g_object_unref(programListStore);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Address", renderer, "text", 0, NULL);
	gtk_tree_view_append_column(programTreeView, column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Mnemonic", renderer, "text", 1, NULL);
	gtk_tree_view_append_column(programTreeView, column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Operand", renderer, "text", 2, NULL);
	gtk_tree_view_append_column(programTreeView, column);

	g_object_ref(programTreeView);

	return TRUE;
}

gboolean program_tree_view_clear(void)
{
	GtkListStore *programListStore = GTK_LIST_STORE(gtk_tree_view_get_model(programTreeView));
	gtk_list_store_clear(programListStore);

	return TRUE;
}

gboolean program_tree_view_add_program(const struct programData *program)
{
	GtkTreeIter iter;
	GtkListStore *programListStore = GTK_LIST_STORE(gtk_tree_view_get_model(programTreeView));
	GString *hexWord = g_string_sized_new(6);


	char *mnemonic;
	uint16_t operand;
	int j;
	for (j = 0; j < MAX_PROGRAM_MEMORY; ++j) {
		operand = program[j].operand;
		mnemonic = get_data(InstructionSet[program[j].instructionPos].mnemonic);
		gtk_list_store_append(programListStore, &iter);
		g_string_printf(hexWord, "0x%03x", j);
		gtk_list_store_set(programListStore, &iter, 0, hexWord->str, -1);
		gtk_list_store_set(programListStore, &iter, 1, mnemonic, -1);
		g_string_printf(hexWord, "0x%04x", operand);
		gtk_list_store_set(programListStore, &iter, 2, hexWord->str, -1);
		free(mnemonic);
	}

	g_string_free(hexWord, TRUE);

	return TRUE;
} 

#undef N_COLUMNS
