#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <gtk/gtk.h>
#include <stdio.h>

gboolean fileClose(FILE **file, const gchar *filename, GtkTextBuffer *error_buffer);
gboolean fileOpen(FILE **file,const gchar *filename, GtkTextBuffer *error_buffer);
gboolean fileWrite(FILE **file, const gchar *filename, GtkTextBuffer *error_buffer);

#endif
