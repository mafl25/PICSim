#include "datastructure.h"
#include "filedialog.h"
#include "filemanager.h"
#include "outputbuffer.h"
#include "customstring.h"
#include "asmkeywords.h"

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <string.h>
#include <stdlib.h>

void text_struct_init(textStruct *text, GtkBuilder *builder)
{
	text->programBuffer = G_OBJECT(gtk_source_buffer_new(NULL));
	text->programTextView = G_OBJECT(gtk_source_view_new_with_buffer(
				GTK_SOURCE_BUFFER(text->programBuffer)));
	GObject *scrolled = gtk_builder_get_object(builder, "text_scrolled");
	gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(text->programTextView));
	gtk_widget_show(GTK_WIDGET(text->programTextView));

	text->label = gtk_builder_get_object(builder, "text_name");
	text->file = NULL;
	text->filename = NULL;

	g_object_ref(text->programTextView);
	g_object_ref(text->programBuffer);
	g_object_ref(text->label);

	gtk_label_set_text(GTK_LABEL(text->label), UNSAVED_FILE);

	text->isSaved = TRUE;
}

void text_struct_destroy(textStruct *text)
{
	g_free(text->filename);
	g_object_unref(text->programTextView);
	g_object_unref(text->programBuffer);
	g_object_unref(text->label);
}

variablesArray *variables_array_new(void)
{
	variablesArray *variables = NULL;
	variables = calloc(sizeof(variablesArray), 1);
	return variables;
}

gboolean variables_array_init(const textStruct *text, variablesArray *variables)
{
	gboolean returnValue = FALSE;
	int j;
	int i;
	GtkTextIter start;
	GtkTextIter end;
	int variableNumber = -1;
	variables->variableCount = 0;

	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(text->programBuffer), &start, &end);
	gchar *string = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(text->programBuffer), &start, &end, TRUE);

	GString *word = g_string_new(NULL);
	gsize position = 0;

	glib_get_word_string(word, string, &position);

	if(word->len == 0){
		output_print("ERROR: Empty document.", TRUE); 
		goto Exit_1;
	}

	if(strcmp(word->str, VARIABLES)){
		output_print("ERROR: Document not properly initialized. It must begin with "VARIABLES, TRUE);  
		goto Exit_1;
	}

	if(!is_line_whitespace_before_newline(&string[position])){
		output_print("ERROR: No word can follow ", FALSE);
		output_print(VARIABLES, TRUE);
		goto Exit_1;
	}

	while((strcmp(word->str, SPOINT)) && (glib_get_word_string(word, string, &position)))
		variableNumber++;

	if(word->len == 0){
		output_print("ERROR: "SPOINT" was not found.", TRUE); 
		goto Exit_1;
	}

	if(!is_line_whitespace_before_newline(&string[position])){
		output_print("ERROR: No word can follow ", FALSE);
		output_print(SPOINT, TRUE);
		goto Exit_1;
	}

	glib_get_word_string(word, string, &position);

	if(!strcmp(word->str, CODE)){
		variables->startingPoint = 0x00;

		if(!is_line_whitespace_before_newline(&string[position])){
			output_print("ERROR: No word can follow ", FALSE);
			output_print(CODE, TRUE);
			goto Exit_1;
		}
	}else{
		variables->startingPoint = glib_hex_string_to_int(word);

		if(variables->startingPoint == EOF){
			output_print("ERROR: Starting point has incorrect hex format: ", FALSE); 
			output_print(word->str, TRUE);
			goto Exit_1;
		}

		glib_get_word_string(word, string, &position);

		if(word->len == 0){
			output_print("ERROR: "CODE" was not found.", TRUE); 
			goto Exit_1;
		}

		if(strcmp(word->str, CODE)){
			output_print("ERROR: "CODE" must follow the Starting point.", TRUE); 
			goto Exit_1;
		}

		if(!is_line_whitespace_before_newline(&string[position])){
			output_print("ERROR: No word can follow ", FALSE);
			output_print(CODE, TRUE);
			goto Exit_1;
		}
	}

	if(variableNumber){
		variables->variableList = (GString **)calloc(variableNumber, sizeof(GString *));
	}else{
		variables->variableList =  NULL;
	}

	position = 0;
	glib_get_word_string(word, string, &position);

	for (j = 0; j < variableNumber; ++j){
		glib_get_word_string(word, string, &position);

		if(!(g_ascii_isalpha(word->str[0]) || (word->str[0] == '_') || (word->str[0] & 0x80))){
			output_print("ERROR: Variable names must start with a letter or an underscore: ", FALSE); 
			output_print(word->str, TRUE);
			goto Exit_1;
		}

		for (i = 1; i < word->len; ++i){
			if(!(g_ascii_isalnum(word->str[i]) || (word->str[i] == '_') || (word->str[i] & 0x80))){
				output_print("ERROR: Variable has ilegal character: ", FALSE); 
				output_print(word->str, TRUE);
				goto Exit_1;
			}
		}

		for (i = 0; i < j; ++i)
			if(!strcmp(variables->variableList[i]->str, word->str)){
				output_print("ERROR: Repeated variable:", FALSE); 
				output_print(word->str, TRUE);
				goto Exit_1;
			}
			
		variables->variableList[j] = g_string_new_len(word->str, word->len);			
		variables->variableCount++;
	}

	returnValue = TRUE;

Exit_1:
	g_string_free(word, TRUE);
	g_free(string);

	return returnValue;
}

gboolean variables_array_set_addresses(variablesArray *variables)
{
	if(variables->variableCount > (variables->lastAddress + 1 - variables->startingPoint)){
		char numberVar[15] = {0};

		output_print("ERROR: Too many variables: ", FALSE); 
		sprintf(numberVar, "%d", variables->variableCount);
		output_print(numberVar, TRUE);

		output_print("Max number of variables is: ", FALSE);
		sprintf(numberVar, "%d", variables->lastAddress + 1);
		output_print(numberVar, FALSE);
		output_print(". Check also your Start point.", TRUE);

		return FALSE;
	}

	if(variables->variableCount == 0){
		variables->variableAddress = NULL;
	}else{
		variables->variableAddress = (int *)calloc(variables->variableCount, sizeof(int));
		if(variables->variableAddress == NULL){
			output_print("ERROR: variableAddress could be initialized.", TRUE); 
			return FALSE;
		}

		int j;
		for (j = 0; j < variables->variableCount; ++j)
			variables->variableAddress[j] = variables->startingPoint + j;
	}

	return TRUE;
}

gboolean variables_array_replace_to_file(const textStruct *text, variablesArray *variables)
{
	gboolean returnValue = TRUE;	
	GtkTextIter start;
	GtkTextIter end;
	GString *word = g_string_new(NULL);
	GString *line = g_string_new(NULL);
	GString *hexString = g_string_new(NULL);
	gsize position = 0;
	gchar *basename = g_filename_display_basename(text->filename);
	FILE *fWrite;

	GString *medBasename = g_string_new(basename);
	g_string_append(medBasename, ASM_MED);

	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(text->programBuffer), &start, &end);
	gchar *string = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(text->programBuffer), &start, &end, TRUE);
	while((strcmp(word->str, CODE)) && (glib_get_word_string(word, string, &position)));

	fileWrite(&fWrite, medBasename->str, TRUE);

	int j;

	while(glib_get_line_string(line, string, &position)){
		for (j = 0; j < variables->variableCount; ++j){
			g_string_printf(hexString, "0X%X", variables->variableAddress[j]);
			glib_replace_word_from_string(line, variables->variableList[j], hexString);
		}

		if(!is_line_empty(line->str)){
			fprintf(fWrite, "%s", line->str);
		}
	}

	fileClose(&fWrite, medBasename->str, TRUE);

	g_string_free(word, TRUE);
	g_string_free(hexString, TRUE);
	g_string_free(line, TRUE);
	g_string_free(medBasename, TRUE);
	g_free(string);
	g_free(basename);

	return returnValue;
}

gboolean variables_array_destroy(variablesArray *variables)
{
	int j;
	if(variables->variableCount > 0){
		for (j = 0; j < variables->variableCount; ++j){
			g_string_free(variables->variableList[j], TRUE);
		}
		g_free(variables->variableList);
	}

	g_free(variables);

	return TRUE;
}

labelsArray *labels_array_new(void)
{
	labelsArray *labels = NULL;
	labels = calloc(sizeof(labelsArray), 1);
	return labels;
}

gboolean labels_array_init(const textStruct *text, labelsArray *labels)
{
	gboolean returnValue = FALSE;
	labels->labelCount = 0;
	int labelNumber = 0;
	GtkTextIter start;
	GtkTextIter end;
	GString *word = g_string_new(NULL);
	GString *line = g_string_new(NULL);
	gsize firstPos = 0;
	gsize secondPos = 0;
	gsize codePos;
	int j;
	int i;
	
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(text->programBuffer), &start, &end);
	gchar *string = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(text->programBuffer), &start, &end, TRUE);


	while((strcmp(word->str, CODE)) && (glib_get_word_string(word, string, &firstPos)));
	codePos = firstPos;

	while(glib_get_line_string(line, string, &firstPos)){
		if(!g_ascii_isspace(line->str[0])){
			glib_get_word_string(word, line->str, &secondPos);

			if(!(g_ascii_isalpha(word->str[0]) || (word->str[0] == '_') || (word->str[0] & 0x80))){
				output_print("ERROR: Labels must start with a letter or an underscore: ", FALSE); 
				output_print(word->str, TRUE);
				goto Exit_1;
			}

			for (j = 0; j < word->len; ++j){
				if(!(g_ascii_isalnum(word->str[j]) || (word->str[j] == '_') || (word->str[j] & 0x80))){
					output_print("ERROR: Label has ilegal character: ", FALSE); 
					output_print(word->str, TRUE);
					goto Exit_1;
				}
				
			}

			if(glib_get_word_string(word, line->str, &secondPos)){
				output_print("ERROR: no words can follow a label: ", FALSE); 
				output_print(line->str, TRUE);
				goto Exit_1;
			}

			labelNumber++;
			secondPos = 0;
		}
	}

	labels->labelList = NULL;	
	labels->labelPos = NULL; 
	labels->labelDir = NULL;

	if(labelNumber){
		labels->labelList = (GString **)calloc(labelNumber, sizeof(GString *));
		if(!labels->labelList){
			output_print("ERROR: Memory for labelList could not be allocated.", TRUE);
			goto Exit_1;
		}

		labels->labelPos = (int *)calloc(labelNumber, sizeof(int));
		if(!labels->labelPos){
			output_print("ERROR: Memory for labelPos could not be allocated.", TRUE);
			goto Exit_1;
		}
	
		labels->labelDir = (int *)calloc(labelNumber, sizeof(int));
		if(!labels->labelList){
			output_print("ERROR: Memory for labelDir could not be allocated.", TRUE);
			goto Exit_1;
		}

		firstPos = codePos;
		for(j = 0; glib_get_line_string(line, string, &firstPos); ++j){
			if(!g_ascii_isspace(line->str[0])){
				glib_get_word_string(word, line->str, &secondPos);

				for (i = 0; i < labels->labelCount; ++i){
					if(!strcmp(labels->labelList[i]->str, word->str)){
						output_print("ERROR: Labels are repeated: ", FALSE);	
						output_print(word->str, TRUE);
						goto Exit_1;
					}
				}

				labels->labelList[labels->labelCount] = g_string_new_len(word->str, word->len);
				labels->labelPos[labels->labelCount] = j;
				labels->labelCount++;

				secondPos = 0;
			}
		}
	}

	returnValue = TRUE;

Exit_1:
	g_string_free(word, TRUE);
	g_string_free(line, TRUE);
	g_free(string);
	return returnValue;
}

gboolean labels_array_destroy(labelsArray *labels)
{
	int j;

	if(labels->labelCount > 0){
		for (j = 0; j < labels->labelCount; ++j)
			g_string_free(labels->labelList[j], TRUE);
		g_free(labels->labelList);
		g_free(labels->labelPos);
		g_free(labels->labelDir);
	}

	g_free(labels);

	return TRUE;
}

gboolean label_variable_array_conflict_check(const variablesArray *variables, const labelsArray *labels)
{
	int j;
	int i;

	for (j = 0; j < labels->labelCount; ++j){
		for (i = 0; i < variables->variableCount; ++i){
			if(!strcmp(labels->labelList[j]->str, variables->variableList[i]->str)){
				output_print("ERROR: Variable and label have the same name: ", FALSE);
				output_print(labels->labelList[j]->str, TRUE);
				return FALSE;
			}

			if(!strcmp(variables->variableList[i]->str, END)){
				output_print("ERROR: Variable has an unallowed name: ", FALSE);
				output_print(variables->variableList[i]->str, TRUE);
				return FALSE;
			}

			if(!strcmp(variables->variableList[i]->str, ORG)){
				output_print("ERROR: Variable has an unallowed name: ", FALSE);
				output_print(variables->variableList[i]->str, TRUE);
				return FALSE;
			}

		}
		if(!strcmp(labels->labelList[j]->str, END)){
			output_print("ERROR: Label has an unallowed name: ", FALSE);
			output_print(labels->labelList[j]->str, TRUE);
			return FALSE;
		}

		if(!strcmp(labels->labelList[j]->str, ORG)){
			output_print("ERROR: Label has an unallowed name: ", FALSE);
			output_print(labels->labelList[j]->str, TRUE);
			return FALSE;
		}
	}

	return TRUE;
}

orgArray *org_array_new(void)
{
	orgArray *orgs = NULL;
	orgs = calloc(sizeof(orgArray), 1);
	return orgs;
}

gboolean org_array_init(const textStruct *text, orgArray *orgs)
{
	gboolean returnValue = FALSE;
	
	GString *filename = g_string_new(text->filename);
	g_string_append(filename, ASM_MED);

	off_t sizeOfFile = fileSize(filename->str);
	if(sizeOfFile == -1){
		output_print("ERROR: ", FALSE);
		output_print(filename->str, FALSE);
		output_print(" not found.", TRUE);
		goto Exit_1;
	}
	GString *string = g_string_sized_new(sizeOfFile + 1);

	FILE *fp;
	fileOpen(&fp, filename->str, TRUE);

	char character;
	while((character = fgetc(fp)) != EOF)
		g_string_append_c(string, character);

	gsize firstPos = 0;
	gsize secondPos = 0;
	GString *word = g_string_new(NULL);
	GString *line = g_string_new(NULL);
	while(glib_get_line_string(line, string->str, &firstPos)){
		if(g_ascii_isspace(line->str[0])){
			glib_get_word_string(word, line->str, &secondPos);
			if(!strcmp(ORG, word->str))
				orgs->orgCount++;
			secondPos = 0;	
		}
	}

	if(orgs->orgCount > 0){
		orgs->orgPos = (int *)calloc(orgs->orgCount, sizeof(int));
		if (!orgs->orgPos){
			output_print("ERROR: orgPos could not be initialized.", TRUE);
			goto Exit_2;
		}

		orgs->orgDir = (int *)calloc(orgs->orgCount, sizeof(int));
		if (!orgs->orgDir){
			output_print("ERROR: orgDir could not be initialized.", TRUE);
			goto Exit_2;
		}

		orgs->orgNop = (int *)calloc(orgs->orgCount, sizeof(int));
		if (!orgs->orgNop){
			output_print("ERROR: orgNop could not be initialized.", TRUE);
			goto Exit_2;
		}

		int linePosition = 0;
		int j = 0;
		firstPos = 0;
		secondPos = 0;
		while(glib_get_line_string(line, string->str, &firstPos)){
			if(g_ascii_isspace(line->str[0])){
				glib_get_word_string(word, line->str, &secondPos);	
				if(!strcmp(word->str, ORG)){
					if(!glib_get_word_string(word, line->str, &secondPos)){
						output_print("ERROR: No address after org.", TRUE);
						goto Exit_2;
					}

					if((orgs->orgDir[j] = glib_hex_string_to_int(word)) > MAX_MEMORY){
						output_print("ERROR: org address exceeds the maximun amount of memory: ", FALSE);
						output_print(line->str, TRUE);
						goto Exit_2;
					}

					if(orgs->orgDir[j] == EOF){
						output_print("ERROR: Address was written incorrectly: ", FALSE);
						output_print(line->str, TRUE);
						goto Exit_2;
					}
					
					orgs->orgPos[j] = linePosition;
					j++;
				}
			}
			linePosition++;
			secondPos = 0;
		}

	}

	returnValue = TRUE;

Exit_2:
	fileClose(&fp, filename->str, TRUE);
	g_string_free(word, TRUE);
	g_string_free(line, TRUE);
	g_string_free(string, TRUE);
Exit_1:
	g_string_free(filename, TRUE);

	return returnValue;
}

gboolean org_array_destroy(orgArray *orgs)
{
	g_free(orgs->orgDir);
	g_free(orgs->orgNop);
	g_free(orgs->orgPos);

	return TRUE;
}

gboolean labels_org_replace_to_file(const textStruct *text, const labelsArray *labels, orgArray *orgs)
{
	gboolean returnValue = FALSE;
	int j;
	int i;
	int previousLabels = 0;
	int totalNops = 0;
	GString *errorMessage = g_string_new(NULL);

	for (j = 0; j < orgs->orgCount; ++j){
		for (i = 0; i < labels->labelCount; ++i){
			if(orgs->orgPos[j] > labels->labelPos[i])
				previousLabels++;
		}

		orgs->orgNop[j] = orgs->orgDir[j] - orgs->orgPos[j] + j + previousLabels - totalNops;
		if(orgs->orgNop[j] < 0){
			g_string_printf(errorMessage, "ERROR: org pointing to place where instructions had already been written: 0x%X",
					orgs->orgDir[j]);	
			output_print(errorMessage->str, TRUE);
			goto Exit_1;
		}

		totalNops += orgs->orgNop[j];
		previousLabels = 0;
	}

	//Hay problemas por que org position y labels position no obtienen sus posiciones del mismo texto.
	//Creo que se debe hacer algo de manera muy distinta: los primero en hacer sería cargar todo el texto a un string?
	//o un buffer, e ir modificando ese buffer. El primer paso sería limpiar el buffer de comentarios y 
	//espacios en blanco innecesarios. O tal vez no. Lo que importa es que se necsita cambair bastante el programa.

	returnValue = TRUE;
Exit_1:
	g_string_free(errorMessage, TRUE);
	return returnValue;
} 








