#ifndef CALLBACKS
#define CALLBACKS
#include <gtk/gtk.h>
#include "filemanager.h"
#define TERMINATION "*.asm"

//This will be kept here, since they don't really have a unique place
//For now at least
gboolean program_changed_cd(GtkWidget *widget, gpointer data);
gboolean set_saved_cb(GtkWidget *widget, gpointer data);

gboolean toggle_sensitive_cb(GtkWidget *widget, gpointer data);

#endif

