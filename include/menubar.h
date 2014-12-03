#ifndef MENUBAR_H
#define MENUBAR_H

#include <gtk/gtk.h>
#include "datastructure.h"

gboolean open_file_cb(GtkWidget *widget, gpointer data);
gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data);
gboolean save_text_view_to_file_cb(GtkWidget *widget, gpointer data);
gboolean save_as_text_view_to_file_cb(GtkWidget *widget, gpointer data);
gboolean new_file_cb(GtkWidget *widget, gpointer data);

gboolean copy_text_cb(GtkWidget *widget, gpointer data);
gboolean cut_text_cb(GtkWidget *widget, gpointer data);
gboolean paste_text_cb(GtkWidget *widget, gpointer data);
gboolean delete_text_cb(GtkWidget *widget, gpointer data);

gboolean undo_text_cb(GtkWidget *widget, gpointer data);
gboolean redo_text_cb(GtkWidget *widget, gpointer data);

gboolean build_program_cb(GtkWidget *widget, gpointer data);

void build_menubar(GtkBuilder *builder, textStruct *text, GtkWindow *window);

#endif
