#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <gtk/gtk.h>

typedef struct
{
	GObject *textProgram;
	GObject *programBuffer;
	GObject *textOutput;
	GObject *outputBuffer;
	GObject *label;
	FILE *file;
	char *filename;
	GtkClipboard *clipboard;
}textStruct;

gboolean fileClose(textStruct *file);
gboolean fileOpen(textStruct *file);
gboolean fileWrite(textStruct *file);

void outputPrint(GtkTextBuffer *outputBuffer, char * message, gboolean addNewline);

#endif
