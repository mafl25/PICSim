#ifndef VARIABLE_TREEVIEW
#define VARIABLE_TREEVIEW

#include "datastructure.h"
#include <gtk/gtk.h>

gboolean variable_tree_view_init(GtkTreeView *variableTree);
gboolean variable_tree_view_clear(void);
gboolean variable_tree_view_append_row(const gchar *variable, int address, int value);
void variable_tree_view_update(void);

#endif
