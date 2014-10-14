#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <gtk/gtk.h>
#include <stdio.h>

gboolean fileClose(FILE **file, const gchar *filename, gboolean print_output);
gboolean fileOpen(FILE **file,const gchar *filename, gboolean print_output);
gboolean fileWrite(FILE **file, const gchar *filename, gboolean print_output);
off_t fileSize(const gchar *filename);

#endif
