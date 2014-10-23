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

typedef struct
{
	GString **labelList;
	int *labelPos;
	int *labelDir;
	int labelCount;
}labelsArray;

void text_struct_init(textStruct *text, GtkBuilder *builder);
void text_struct_destroy(textStruct *text);

variablesArray *variables_array_new(void);
gboolean variables_array_init(const textStruct *text, variablesArray *variables);
gboolean variables_array_set_addresses(variablesArray *variables);
gboolean variables_array_replace_to_file(const textStruct *text, variablesArray *variables);
gboolean variables_array_destroy(variablesArray *variables);

labelsArray *labels_array_new(void);
gboolean labels_array_init(const textStruct *text, labelsArray *labels);
gboolean labels_array_destroy(labelsArray *labels);

gboolean label_array_conflict_check(const variablesArray *variables, const labelsArray *labels);

#endif
