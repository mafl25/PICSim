#ifndef DATA_STRUCT
#define DATA_STRUCT

#include <gtk/gtk.h>
//
//Luego buscarle otro lado
#define LAST_RAM_ADDRESS 0x0A

typedef struct
{
	FILE *file;
	gchar *filename;
	GObject *programTextView;
	GObject *programBuffer;
	GObject *outputBuffer;
	GObject *label;
	gboolean isSaved;
	gulong programChangedHandlerId;
}textStruct;

typedef struct
{
	GString **variableList;
	int *variableAddress;
	int startingPoint;
	int lastAddress;
	int variableCount;
}variablesArray;

void text_struct_init(textStruct *text, GtkBuilder *builder);
void text_struct_destroy(textStruct *text);

gboolean variables_array_init(textStruct *text, variablesArray *variables);
gboolean variables_array_set_addresses(variablesArray *variables);
gboolean variables_array_destroy(variablesArray *variables);

#endif
