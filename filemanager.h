#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <gtk/gtk.h>

typedef struct
{
	FILE *file;
	char *filename;
}MFile;

gboolean fileClose(MFile *file, GtkTextBuffer *outputBuffer);
gboolean fileOpen(MFile *file, GtkTextBuffer *outputBuffer);
gboolean fileWrite(MFile *file, GtkTextBuffer *outputBuffer);

#endif
