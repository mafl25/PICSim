#ifndef DATA_STRUCT
#define DATA_STRUCT

#include <gtk/gtk.h>

typedef struct
{
	FILE *file;
	gchar *filename;
	GObject *programTextView;
	GObject *programBuffer;
	GObject *outputTextView;
	GObject *outputBuffer;
	GObject *label;
	gboolean isSaved;
	gulong programChangedHandlerId;
}textStruct;

void textStructInit(textStruct *text, GtkBuilder *builder);
void textStructDestroy(textStruct *text);

#endif
