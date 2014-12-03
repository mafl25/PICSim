#ifndef SETTINGS_H
#define SETTINGS_H

#include "datastructure.h"
#include <gtk/gtk.h>

gboolean store_settings_cb(GtkWidget *widget, gpointer data);
gboolean default_settings_cb(GtkWidget *widget, gpointer data);

gboolean line_number_toggle_cb(GtkWidget *widget, gpointer data);
gboolean right_margin_toggle_cb(GtkWidget *widget, gpointer data);
gboolean right_margin_width_set_cb(GtkWidget *widget, gpointer data);

gboolean wrap_text_cb(GtkWidget *widget, gpointer data);
gboolean wrap_mode_mode_change_cb(GtkWidget *widget, gpointer data);

gboolean highlight_line_cb(GtkWidget *widget, gpointer data);
gboolean highlight_bracket_cb(GtkWidget *widget, gpointer data);
gboolean tab_width_change_cb(GtkWidget *widget, gpointer data);
gboolean space_tab_change_cb(GtkWidget *widget, gpointer data);
gboolean auto_indent_change_cb(GtkWidget *widget, gpointer data);

void build_settings(GtkBuilder *builder, textStruct *text);
int set_settings(char *directory);
void free_setting(void);

#endif


