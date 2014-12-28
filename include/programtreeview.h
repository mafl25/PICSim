#ifndef PROGRAM_TREEVIEW
#define PROGRAM_TREEVIEW

#include "datastructure.h"
#include <gtk/gtk.h>

gboolean program_tree_view_init(GtkTreeView *programTree);
gboolean program_tree_view_clear(void);
gboolean program_tree_view_add_program(const struct programData *program);

#endif

