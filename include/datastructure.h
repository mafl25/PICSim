#ifndef DATA_STRUCT
#define DATA_STRUCT

#include <gtk/gtk.h>
#include "vstring.h"
#include "procesor.h"

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
	struct programData *programMemory;
	gboolean programBuilt;
}textStruct;


typedef struct
{
	struct Vector *variableList;
	struct Vector *variableAddress;
	int startingPoint;
	int lastAddress;
}variablesArray;

typedef struct
{
	struct Vector *labelList;
	struct Vector *labelPos;
	struct Vector *labelDir;
}labelsArray;

typedef struct
{
	struct Vector *orgPos;
	struct Vector *orgNop;
	struct Vector *orgDir;
}orgArray;

void text_struct_init(textStruct *text, GtkBuilder *builder);
void text_struct_destroy(textStruct *text);

void delete_empty_lines(struct VString *text);//at some moment, add to VString

variablesArray *variables_array_new(void);
gboolean variables_array_destroy(variablesArray *variables);
gboolean variables_array_init(struct VString *program, variablesArray *variables);
gboolean variables_array_set_addresses(variablesArray *variables);
gboolean variables_array_replace(struct VString *program, variablesArray *variables);

labelsArray *labels_array_new(void);
gboolean labels_array_destroy(labelsArray *labels);
gboolean labels_array_init(struct VString *program, labelsArray *labels);
gboolean label_variable_array_conflict_check(const variablesArray *variables, const labelsArray *labels);

orgArray *org_array_new(void);
gboolean org_array_init(struct VString *program, orgArray *orgs);
gboolean org_array_destroy(orgArray *orgs);
gboolean labels_org_replace(struct VString *program, const labelsArray *labels, orgArray *orgs);

#endif
