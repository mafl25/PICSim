#ifndef SETTINGS_H
#define SETTINGS_H
#include <gtk/gtk.h>
#include "callbacks.h"

void build_settings(GtkBuilder *builder, textStruct *text);
int set_settings(void);

gboolean store_settings_cb(GtkWidget *widget, gpointer data);
gboolean default_settings_cb(GtkWidget *widget, gpointer data);

#endif


