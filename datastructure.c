#include "datastructure.h"
#include "filedialog.h"
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

gboolean variables_array_init(const textStruct *text, variablesArray *variables)
{
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
		output_print("Error: Empty document.", TRUE);
		g_string_free(word, TRUE);
		g_free(string);
		return FALSE;
	}

	if(strcmp(word->str, VARIABLES)){
		output_print("Error: Document not properly initialized. It must begin with "VARIABLES, TRUE);
		g_string_free(word, TRUE);
		g_free(string);
		return FALSE;
	}

	while((strcmp(word->str, SPOINT)) && (glib_get_word_string(word, string, &position)))
		variableNumber++;

	if(word->len == 0){
		output_print("Error: "SPOINT" was not found.", TRUE);
		g_string_free(word, TRUE);
		g_free(string);
		return FALSE;	
	}

	glib_get_word_string(word, string, &position);

	if(!strcmp(word->str, CODE)){
		variables->startingPoint = 0x00;
	}else{
		variables->startingPoint = glib_hex_string_to_int(word);

		if(variables->startingPoint == EOF){
			output_print("Error: Starting point has incorrect hex format: ", FALSE);
			output_print(word->str, TRUE);
			g_string_free(word, TRUE);
			g_free(string);
			return FALSE;
		}

		glib_get_word_string(word, string, &position);

		if(word->len == 0){
			output_print("Error: "CODE" was not found.", TRUE);
			g_string_free(word, TRUE);
			g_free(string);
			return FALSE;	
		}

		if(strcmp(word->str, CODE)){
			output_print("Error: "CODE" must follow the Starting point.", TRUE);
			g_string_free(word, TRUE);
			g_free(string);
			return FALSE;
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
			output_print("Error: Variable names must start with a letter or an underscore: ", FALSE);
			output_print(word->str, TRUE);
			g_string_free(word, TRUE);
			g_free(string);
			return FALSE;
		}

		for (i = 1; i < word->len; ++i){
			if(!(g_ascii_isalnum(word->str[i]) || (word->str[i] == '_') || (word->str[i] & 0x80))){
				output_print("Error: Variable has ilegal character: ", FALSE);
				output_print(word->str, TRUE);
				g_string_free(word, TRUE);
				g_free(string);
				return FALSE;	
			}
		}

		for (i = 0; i < j; ++i)
			if(!strcmp(variables->variableList[i]->str, word->str)){
				output_print("Error: Repeated variable:", FALSE);
				output_print(word->str, TRUE);
				g_string_free(word, TRUE);
				g_free(string);
				return FALSE;	
			}
			
		variables->variableList[j] = g_string_new_len(word->str, word->len);			
		variables->variableCount++;
	}

	g_string_free(word, TRUE);
	g_free(string);

	return TRUE;
}

gboolean variables_array_set_addresses(variablesArray *variables)
{
	if(variables->variableCount > (variables->lastAddress + 1 - variables->startingPoint)){
		char numberVar[15] = {0};

		output_print("Error: Too many variables: ", FALSE);
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
			output_print("Error: variableAddress could be initialized.", TRUE);
			return FALSE;
		}

		int j;
		for (j = 0; j < variables->variableCount; ++j)
			variables->variableAddress[j] = variables->startingPoint + j;
	}

	return TRUE;
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

	return TRUE;
}

gboolean labels_array_init(const textStruct *text, labelsArray *labels)
{
	labels->labelCount = 0;
	int labelNumber = 0;
	GtkTextIter start;
	GtkTextIter end;
	GString *word = g_string_new(NULL);
	GString *line = g_string_new(NULL);
	gsize position = 0;
	gsize secondPos = 0;
	gsize codeStartPos;
	int j;
	
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(text->programBuffer), &start, &end);
	gchar *string = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(text->programBuffer), &start, &end, TRUE);


	while((strcmp(word->str, CODE)) && (glib_get_word_string(word, string, &position)));
	codeStartPos = position;

	while(glib_get_line_string(line, string, &position)){
		if(!g_ascii_isspace(line->str[0])){
			glib_get_word_string(word, line->str, &secondPos);

			if(!(g_ascii_isalpha(word->str[0]) || (word->str[0] == '_') || (word->str[0] & 0x80))){
				output_print("Error: Labels must start with a letter or an underscore: ", FALSE);
				output_print(word->str, TRUE);
				g_string_free(word, TRUE);
				g_string_free(line, TRUE);
				g_free(string);
				return FALSE;
			}

			for (j = 0; j < word->len; ++j){
				if(!(g_ascii_isalnum(word->str[j]) || (word->str[j] == '_') || (word->str[j] & 0x80))){
					output_print("Error: Label has ilegal character: ", FALSE);
					output_print(word->str, TRUE);
					g_string_free(word, TRUE);
					g_string_free(line, TRUE);
					g_free(string);
					return FALSE;	
				}
				
			}

			if(glib_get_word_string(word, line->str, &secondPos)){
				output_print("Error: no words can follow a label: ", FALSE);
				output_print(line->str, TRUE);
				g_string_free(word, TRUE);
				g_string_free(line, TRUE);
				g_free(string);
				return FALSE;
			}

			labelNumber++;
			secondPos = 0;
		}
	}

	if(labelNumber){
		labels->labelList = (GString **)calloc(labelNumber, sizeof(GString *));
	}else{
		labels->labelList = NULL;	
	}

	//UNDER CONSTRUCTION

	g_print("%d\n", labelNumber);

	g_string_free(word, TRUE);
	g_string_free(line, TRUE);
	g_free(string);
	return TRUE;
}
