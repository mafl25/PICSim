#include "datastructure.h"
#include "filedialog.h"
#include "filemanager.h"
#include "outputbuffer.h"
#include "customstring.h"
#include "asmkeywords.h"
#include "procesor.h"

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <string.h>
#include <stdlib.h>

#define UNDER_SCORE 0x5F

static bool has_variable_format(const struct VString *string)
{
	munichar letter = get_unichar_at(string, 0);
	uint64_t j;

	if (!(g_unichar_isalpha(letter) || (letter == UNDER_SCORE))) 
		return false;

	for (j = 1; j < get_char_number(string); ++j) {
		letter = get_unichar_at(string, j);
		if(!(g_unichar_isalnum(letter) || (letter == UNDER_SCORE)))
			return false;
	}

	return true;
}

static bool is_repeated(const struct Vector *container, const struct VString *string)
{
	uint64_t j;
	const struct VString *contained;

	for (j = 0; get_element_at(container, &contained, j); ++j) {
		if (equal(contained, string))
			return true;
	}

	return false;
}

static int get_line_number(const struct VString *text)
{
	int j = 0;
	uint64_t position = 0;

	while (get_next_line_pos(text, position, &position))
		++j;


	return ++j;
}

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

	text->programMemory = malloc(sizeof(struct programData) * (MAX_PROGRAM_MEMORY + 1));
	text->programBuilt = FALSE;

	text->isSaved = TRUE;
}

void text_struct_destroy(textStruct *text)
{
	free(text->programMemory);
	g_free(text->filename);
	g_object_unref(text->programTextView);
	g_object_unref(text->programBuffer);
	g_object_unref(text->label);
}

void delete_empty_lines(struct VString *text)
{
	uint64_t j;
	uint64_t startLine = 0;

	char character[7] = {0};
	munichar unicharacter;
	bool lastLineEmpty = true;
	
	for (j = 0; get_element_at(text, character, j); ++j) {
		unicharacter = g_utf8_get_char(character);

		if (unicharacter == 0xA) {
			delete_line(text, startLine);
			j = startLine - 1;
		} else if (!g_unichar_isspace(unicharacter)) {
			if (get_next_line_pos(text, j, &startLine)) {
				j = startLine - 1;
			} else {
				lastLineEmpty = false;
				break;// bool value
			}
		}
	}

	if (lastLineEmpty)
		delete_line(text, startLine - 1);
}

variablesArray *variables_array_new(void)
{
	variablesArray *variables = NULL;
	variables = calloc(sizeof(variablesArray), 1);
	variables->variableList = new(Vector, NULL, 4, sizeof(struct VString *));
	variables->variableAddress = new(Vector, NULL, 4, sizeof(int));
	return variables;
}

gboolean variables_array_destroy(variablesArray *variables)
{
	int j;
	struct VString *element;
	for (j = 0; get_element_at(variables->variableList, &element, j); ++j) 
		delete(element);

	delete(variables->variableList);
	delete(variables->variableAddress);
	free(variables);

	return TRUE;
}

gboolean variables_array_init(struct VString *program, variablesArray *variables)
{
	struct VString *substring = new(VString, NULL, 6);
	struct VString *cloned;
	struct Vector *varContainer = variables->variableList;
	gboolean returnValue = FALSE;

	uint64_t position = 0;

	if (!(position = get_substring(program, substring, 0))){
		output_print("ERROR: Empty document.", TRUE); 
		goto Exit_1;
	}

	if (!compare_string(substring, 0, VARIABLES, -1)){
		output_print("ERROR: Document not properly initialized. It must begin with "VARIABLES, TRUE);  
		goto Exit_1;
	}

	if ((get_substring_in_line(program, substring, position))){
		output_print("ERROR: No word can follow "VARIABLES, TRUE);
		goto Exit_1;
	}

	erase_at(program, 0, position);

	while ((position = get_substring(program, substring, 0))) {
		if (compare_string(substring, 0, SPOINT, -1)) 
			break;
		
		if(has_variable_format(substring)) {
			if (!is_repeated(varContainer, substring)) {
				cloned = clone(substring);
				insert_array_at(varContainer, get_length(varContainer), &cloned, 1);

				if((get_substring_in_line(program, substring, position))){
					output_print("ERROR: No word can follow a variable.", FALSE);
					output_print_string(substring, TRUE);
					goto Exit_1;
				}
			} else {
				output_print("ERROR: Repeated variable: ", FALSE);
				output_print_string(substring, TRUE);
				goto Exit_1;
			}
		} else {
			output_print("ERROR: Variables has incorrect format: ", FALSE);
			output_print_string(substring, TRUE);
			output_print("Variables must start with a letter or underscore, and can only contain alphanumeric characters and underscores",
					     TRUE);
			goto Exit_1;
		}

		erase_at(program, 0, position);
	}


	if (!position){
		output_print("ERROR: "SPOINT" was not found.", TRUE); 
		goto Exit_1;
	}

	if ((get_substring_in_line(program, substring, position))){
		output_print("ERROR: No word can follow "SPOINT" ", FALSE);
		output_print_string(substring, TRUE);
		goto Exit_1;
	}

	erase_at(program, 0, position);

	position = get_substring(program, substring, 0);

	if (!position) {
		output_print("ERROR: "CODE" was not found", TRUE);
		goto Exit_1;
	}

	if (compare_string(substring, 0, CODE, -1)){
		variables->startingPoint = 0x00;

		if ((get_substring_in_line(program, substring, position))){
			output_print("ERROR: No word can follow "CODE" ", FALSE);
			output_print_string(substring, TRUE);
			goto Exit_1;
		}

		get_next_line_pos(program, position, &position);
		erase_at(program, 0, position);
	} else {
		variables->startingPoint = string_to_hex(substring);

		if(variables->startingPoint == -1){
			output_print("ERROR: Starting point has incorrect hex format: ", FALSE); 
			output_print_string(substring, TRUE);
			goto Exit_1;
		}

		

		erase_at(program, 0, position);
		if (!(position = get_substring(program, substring, 0))) {
			output_print("ERROR: "CODE" was not found.", TRUE); 
			goto Exit_1;
		}

		if (!compare_string(substring, 0, CODE, -1)){
			output_print("ERROR: "CODE" must follow the Starting point.", TRUE); 
			goto Exit_1;
		}

		if ((get_substring_in_line(program, substring, position))) {
			output_print("ERROR: No word can follow "CODE" ", FALSE);
			output_print_string(substring, TRUE);
			goto Exit_1;
		}

		get_next_line_pos(program, position, &position);
		erase_at(program, 0, position);
	}


	returnValue = TRUE;

Exit_1:
	delete(substring);

	return returnValue;
}

gboolean variables_array_set_addresses(variablesArray *variables)
{
	struct Vector *addresses = variables->variableAddress;
	uint64_t variableCount = get_length(variables->variableList);
	int startAddress = variables->startingPoint;
	int freeMem = variables->lastAddress + 1 - variables->startingPoint;

	if(freeMem < 0 || variableCount > freeMem){
		char numberVar[15] = {0};

		output_print("ERROR: Too many variables: ", FALSE); 
		sprintf(numberVar, "%lu", variableCount);
		output_print(numberVar, TRUE);

		output_print("Max number of variables is: ", FALSE);
		sprintf(numberVar, "%d", variables->lastAddress + 1);
		output_print(numberVar, FALSE);
		output_print(". Check also your Start point.", TRUE);

		return FALSE;
	}

	int j;
	for (j = startAddress; j < (variableCount + startAddress); ++j)
		insert_array_at(addresses, get_length(addresses), &j, 1);

	return TRUE;
}

gboolean variables_array_replace(struct VString *program, variablesArray *variables)
{
	gboolean returnValue = TRUE;
	int count = get_length(variables->variableAddress);
	struct Vector *variablesList = variables->variableList;
	struct Vector *addressesList = variables->variableAddress;
	struct VString *variable;
	struct VString *hexString = new(VString, NULL, 5);
	int address;

	int j;
	for (j = 0; j < count; ++j) {
		get_element_at(variablesList, &variable, j);
		get_element_at(addressesList, &address, j);

		hex_to_string(hexString, address);

		replace_word(program, variable, hexString);
	}

	delete(hexString);
	return returnValue;
}

labelsArray *labels_array_new(void)
{
	labelsArray *labels = NULL;
	labels = calloc(sizeof(labelsArray), 1);

	labels->labelList = new(Vector, NULL, 4, sizeof(struct VString *));
	labels->labelPos = new(Vector, NULL, 4, sizeof(int));
	labels->labelDir = new(Vector, NULL, 4, sizeof(int));

	return labels;
}

gboolean labels_array_destroy(labelsArray *labels)
{
	int j;
	struct VString *element;

	for (j = 0; get_element_at(labels->labelList, &element, j); ++j)
		delete(element);
	delete(labels->labelList);
	delete(labels->labelPos);
	delete(labels->labelDir);

	free(labels);

	return TRUE;
}


gboolean labels_array_init(struct VString *program, labelsArray *labels)
{
	gboolean returnValue = FALSE;
	struct VString *substring = new(VString, NULL, 5);
	struct VString *cloned = NULL;
	uint64_t linePos= 0;
	uint64_t substringPos= 0;
	munichar element;	
	char *error;
	int j;
	

	for (j = 0; j < get_char_number(program); ++j){
		element = get_unichar_at(program, linePos);
		if (!g_unichar_isspace(element)) {
			substringPos = get_substring(program, substring, linePos);

			if (has_variable_format(substring)) {
				if (!is_repeated(labels->labelList, substring)) {
					cloned = clone(substring);
					insert_array_at(labels->labelList, get_length(labels->labelList), &cloned, 1);
					insert_array_at(labels->labelPos, get_length(labels->labelPos), &j, 1);

					if((get_substring_in_line(program, substring, substringPos))){
						output_print("ERROR: No word can follow a label: ", FALSE);
						error = get_data(substring);
						output_print(error, TRUE);
						free(error);
						goto Exit_1;
					}
				} else {
					output_print("ERROR: Repeated variable:", FALSE);
					error = get_data(substring);
					output_print(error, TRUE);
					free(error);
					goto Exit_1;
				}
			} else {
				output_print("ERROR: Label has incorrect format: ", FALSE); 
				error = get_data(substring);
				output_print(error, TRUE);
				free(error);
				output_print("Labels must start with a letter or underscore, and can only contain alphanumeric characters and underscores",
							 TRUE);
				goto Exit_1;
			}
		}

		if (!get_next_line_pos(program, linePos, &linePos))
			break;
	}

	returnValue = TRUE;

Exit_1:
	delete(substring);

	return returnValue;

}

gboolean label_variable_array_conflict_check(const variablesArray *variables, const labelsArray *labels)
{
	int j;
	int i;
	struct Vector *labContainer = labels->labelList;
	struct Vector *varContainer = variables->variableList;
	int labelCount = get_length(labContainer);
	int variableCount = get_length(varContainer);
	struct VString *label;
	struct VString *variable;

	for (j = 0; j < labelCount; ++j){
		get_element_at(labContainer, &label, j);
		for (i = 0; i < variableCount; ++i){
			get_element_at(varContainer, &variable, i);

			if(equal(label, variable)){
				output_print("ERROR: Variable and label have the same name: ", FALSE);
				output_print_string(label, TRUE);
				return FALSE;
			}

			if(compare_string(variable, 0, END, -1)){
				output_print("ERROR: Variable has an unallowed name: ", FALSE);
				output_print_string(variable, TRUE);
				return FALSE;
			}

			if(compare_string(variable, 0, ORG, -1)){
				output_print("ERROR: Variable has an unallowed name: ", FALSE);
				output_print_string(variable, TRUE);
				return FALSE;
			}

		}
		if(compare_string(label, 0, END, -1)){
			output_print("ERROR: Label has an unallowed name: ", FALSE);
			output_print_string(label, TRUE);
			return FALSE;
		}

		if(compare_string(label, 0, ORG, -1)){
			output_print("ERROR: Label has an unallowed name: ", FALSE);
			output_print_string(label, TRUE);
			return FALSE;
		}
	}

	return TRUE;
}

orgArray *org_array_new(void)
{
	orgArray *orgs = NULL;
	orgs = calloc(sizeof(orgArray), 1);
	orgs->orgDir = new(Vector, NULL, 4, sizeof(int));
	orgs->orgPos = new(Vector, NULL, 4, sizeof(int));
	orgs->orgNop = new(Vector, NULL, 4, sizeof(int));
	return orgs;
}

gboolean org_array_destroy(orgArray *orgs)
{
	delete(orgs->orgNop);
	delete(orgs->orgDir);
	delete(orgs->orgPos);

	return TRUE;
}

gboolean org_array_init(struct VString *program, orgArray *orgs)
{
	gboolean returnValue = FALSE;
	struct VString *substring = new(VString, NULL, 5);

	uint64_t linePos = 0;
	uint64_t startPos = 0;
	uint64_t subPos = 0;
	int direction = 0;
	munichar unicharacter;

	for (linePos = 0; linePos < get_line_number(program); linePos++) {
		unicharacter = get_unichar_at(program, startPos);

		if (g_unichar_isspace(unicharacter)) {
			subPos = get_substring_in_line(program, substring, startPos);
			if(compare_string(substring, 0, ORG, -1)){
				if(!get_substring_in_line(program, substring, subPos)){
					output_print("ERROR: No address after org: ", FALSE);
					output_print_string(substring, TRUE);
					goto Exit_1;
				}

				if((direction = string_to_hex(substring)) > MAX_MEMORY){
					output_print("ERROR: org address exceeds the maximun amount of memory: ", FALSE);
					output_print_string(substring, TRUE);
					goto Exit_1;
				}

				if(direction == EOF){
					output_print("ERROR: Address was written incorrectly: ", FALSE);
					output_print_string(substring, TRUE);
					goto Exit_1;
				}
				
				insert_array_at(orgs->orgDir, get_length(orgs->orgDir), &direction, 1);
				insert_array_at(orgs->orgPos, get_length(orgs->orgPos), &linePos, 1);
			}
		}

		get_next_line_pos(program, startPos, &startPos);
	}


	returnValue = TRUE;

Exit_1:
	delete(substring);

	return returnValue;
}

gboolean labels_org_replace(struct VString *program, const labelsArray *labels, orgArray *orgs)
{
	struct VString *hexString = new(VString, NULL, 4);
	gboolean returnValue = FALSE;
	int previousLabels = 0;
	int labelsPos = 0;
	int orgPos = 0;
	int orgNops = 0;
	int orgDir = 0;
	int totalNops = 0;
	int j;
	int i;

	for (j = 0; j < get_length(orgs->orgPos); ++j) {
		get_element_at(orgs->orgPos, &orgPos, j);

		for (i = 0; i < get_length(labels->labelList); ++i) {
			get_element_at(labels->labelPos, &labelsPos, i);
			if(orgPos > labelsPos)
				previousLabels++;
		}

		get_element_at(orgs->orgDir, &orgDir, j);
		orgNops = orgDir - orgPos + j + previousLabels - totalNops;
		if (orgNops < 0) {
			output_print("ERROR: org pointing to place where instructions had already been written: ", FALSE);
			hex_to_string(hexString, orgDir);
			output_print_string(hexString, TRUE);
			goto Exit_1;
		}

		insert_element_at(orgs->orgNop, get_length(orgs->orgNop), orgNops);
		totalNops += orgNops;
		previousLabels = 0;
	}

	totalNops = 0;
	int previousOrgs = 0;
	int nops = 0;
	int labelDir = 0;

	for (j = 0; j < get_length(labels->labelList); ++j) {
		get_element_at(labels->labelPos, &labelsPos, j);
		for (i = 0; i < get_length(orgs->orgPos); ++i) {
			get_element_at(orgs->orgPos, &orgPos, i);
			if (orgPos < labelsPos) {
				previousOrgs++;
				get_element_at(orgs->orgNop, &nops, i);
				totalNops += nops;
			}
		}
		
		labelDir = labelsPos - j - previousOrgs + totalNops;
		insert_element_at(labels->labelDir, get_length(labels->labelDir), labelDir);
		totalNops = 0;
		previousOrgs = 0;
	}

	struct VString *progCopy = clone(program);
	struct VString *label;
	erase_at(program, 0, get_char_number(program));

	for (j = 0; j < get_length(labels->labelList); ++j) {
		get_element_at(labels->labelDir, &labelDir, j);
		get_element_at(labels->labelList, &label, j);

		hex_to_string(hexString, labelDir);
		replace_word(progCopy, label, hexString);
	}

	char *nopText = "\tnop\n";
	bool lineWritten = false;
	uint64_t linePosition = 0;
	uint64_t lineStart = 0;
	uint64_t lineEnd= 0;
	uint64_t k;
	for (k = 0; k < get_line_number(progCopy); ++k) {
		for (j = 0; j < get_length(orgs->orgPos); ++j) {
			get_element_at(orgs->orgPos, &orgPos, j);
			if (orgPos == linePosition) {
				get_element_at(orgs->orgNop, &orgNops, j);
				for (i = 0; i < orgNops; ++i) 
					insert_array_at(program, get_length(program), nopText, utf8_char_number(nopText, -1));
				lineWritten = true;
				break;
			}
		}

		for (j = 0; j < get_length(labels->labelList); ++j) {
			get_element_at(labels->labelPos, &labelsPos, j);
			if (labelsPos == linePosition) {
				lineWritten = true;
				break;
			}
		}

		if (!get_next_line_pos(progCopy, lineStart, &lineEnd)) 
			lineEnd = get_char_number(progCopy);

		if (!lineWritten) 
			insert_vector_at(program, get_length(program), progCopy, lineStart, lineEnd - lineStart);
		
		lineStart = lineEnd;

		linePosition++;
		lineWritten = false;
	}

	delete(progCopy);
	returnValue = TRUE;

Exit_1:
	delete(hexString);

	return returnValue;
}
