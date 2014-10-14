#include "settings.h"
#include "callbacks.h"
#include "filemanager.h"
#include "customstring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#define LINE_NUMBER_NAME "line_number_check" 
#define RIGHT_MARGIN_NAME "right_margin"
#define RIGHT_MARGIN_WIDTH_NAME "right_margin_width"
#define TEXT_WRAP_NAME "text_wrap"
#define SPLIT_WORD_NAME "split_word" 
#define HIGHLIGHT_LINE_NAME "highlight_line"
#define HIGHLIGHT_BRACKET_NAME "highlight_bracket"
#define TAB_WIDTH_NAME "tab_width"
#define INDENT_SIZE_NAME "indent_size"
#define SPACE_TABS_NAME "space_tabs"
#define AUTO_INDENT_NAME "auto_indent"

#define FILE_SETTINGS "settings.pic"

#define P_TRUE "TRUE"
#define P_FALSE "FALSE"

typedef struct
{
	GtkWidget *widget;
	gchar *name;
	void (*property_change)(GtkWidget *widget, const gchar *property);
}settingsWidgets;

typedef enum {
	LINE_NUMBER, 
	RIGHT_MARGIN, 
	RIGHT_MARGIN_WIDTH, 
	TEXT_WRAP,
	SPLIT_WORD,
	HIGHLIGHT_LINE, 
	HIGHLIGHT_BRACKET,  
	INDENT_SIZE, 
	SPACE_TABS, 
	AUTO_INDENT,
	WIDGET_NUMBER
}setEnum;

static settingsWidgets sWidgets[WIDGET_NUMBER];

static void toggle_button_init(GtkWidget *widget, const gchar *property)
{
	int state = -1;

	if(!(strcmp(P_TRUE, property)))
		state = TRUE;
	else if(!(strcmp(P_FALSE, property)))
		state = FALSE;

	if(state != -1){
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), !state);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), state);
	}

	g_print("State: %d\n", state);
	g_print("Property: %s\n", property);
}

static void spin_button_init(GtkWidget *widget, const gchar *property)
{
	int value = atoi(property);

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), (gdouble)value);
}

void build_settings(GtkBuilder *builder, textStruct *text)
{
	sWidgets[LINE_NUMBER].name = LINE_NUMBER_NAME;
	sWidgets[LINE_NUMBER].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[LINE_NUMBER].name));
	sWidgets[LINE_NUMBER].property_change = toggle_button_init;

	g_signal_connect(sWidgets[LINE_NUMBER].widget, "toggled", G_CALLBACK(line_number_toggle_cb), (gpointer)text);


	sWidgets[RIGHT_MARGIN].name = RIGHT_MARGIN_NAME;
	sWidgets[RIGHT_MARGIN].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[RIGHT_MARGIN].name));
	sWidgets[RIGHT_MARGIN].property_change = toggle_button_init;

	sWidgets[RIGHT_MARGIN_WIDTH].name = RIGHT_MARGIN_WIDTH_NAME;
	sWidgets[RIGHT_MARGIN_WIDTH].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[RIGHT_MARGIN_WIDTH].name));
	sWidgets[RIGHT_MARGIN_WIDTH].property_change = spin_button_init;

	g_signal_connect(sWidgets[RIGHT_MARGIN].widget, "toggled", G_CALLBACK(right_margin_toggle_cb), (gpointer)text);
	g_signal_connect(sWidgets[RIGHT_MARGIN].widget, "toggled", G_CALLBACK(toggle_sensitive_cb), 
			(gpointer)sWidgets[RIGHT_MARGIN_WIDTH].widget);

	g_signal_connect(sWidgets[RIGHT_MARGIN_WIDTH].widget, "value-changed", G_CALLBACK(right_margin_width_set_cb), 
			(gpointer)text);

	sWidgets[TEXT_WRAP].name = TEXT_WRAP_NAME;
	sWidgets[TEXT_WRAP].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[TEXT_WRAP].name));
	sWidgets[TEXT_WRAP].property_change = toggle_button_init;

	sWidgets[SPLIT_WORD].name = SPLIT_WORD_NAME;
	sWidgets[SPLIT_WORD].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[SPLIT_WORD].name));
	sWidgets[SPLIT_WORD].property_change = toggle_button_init;

	g_signal_connect(sWidgets[TEXT_WRAP].widget, "toggled", G_CALLBACK(wrap_text_cb), (gpointer)text);
	g_signal_connect(sWidgets[TEXT_WRAP].widget, "toggled", G_CALLBACK(toggle_sensitive_cb), 
			(gpointer)sWidgets[SPLIT_WORD].widget);

	g_signal_connect(sWidgets[SPLIT_WORD].widget, "toggled", G_CALLBACK(wrap_mode_mode_change_cb), (gpointer)text);

	sWidgets[HIGHLIGHT_LINE].name = HIGHLIGHT_LINE_NAME;
	sWidgets[HIGHLIGHT_LINE].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[HIGHLIGHT_LINE].name));
	sWidgets[HIGHLIGHT_LINE].property_change = toggle_button_init;

	g_signal_connect(sWidgets[HIGHLIGHT_LINE].widget, "toggled", G_CALLBACK(highlight_line_cb), (gpointer)text);

	sWidgets[HIGHLIGHT_BRACKET].name = HIGHLIGHT_BRACKET_NAME;
	sWidgets[HIGHLIGHT_BRACKET].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[HIGHLIGHT_BRACKET].name));
	sWidgets[HIGHLIGHT_BRACKET].property_change = toggle_button_init;

	g_signal_connect(sWidgets[HIGHLIGHT_BRACKET].widget, "toggled", G_CALLBACK(highlight_bracket_cb), (gpointer)text);

	sWidgets[INDENT_SIZE].name = INDENT_SIZE_NAME;
	sWidgets[INDENT_SIZE].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[INDENT_SIZE].name));
	sWidgets[INDENT_SIZE].property_change = spin_button_init;

	g_signal_connect(sWidgets[INDENT_SIZE].widget, "value-changed", G_CALLBACK(tab_width_change_cb), (gpointer)text);

	sWidgets[SPACE_TABS].name = SPACE_TABS_NAME;
	sWidgets[SPACE_TABS].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[SPACE_TABS].name));
	sWidgets[SPACE_TABS].property_change = toggle_button_init;

	g_signal_connect(sWidgets[SPACE_TABS].widget, "toggled", G_CALLBACK(space_tab_change_cb), (gpointer)text);

	sWidgets[AUTO_INDENT].name = AUTO_INDENT_NAME;
	sWidgets[AUTO_INDENT].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[AUTO_INDENT].name));
	sWidgets[AUTO_INDENT].property_change = toggle_button_init;

	g_signal_connect(sWidgets[AUTO_INDENT].widget, "toggled", G_CALLBACK(auto_indent_change_cb), (gpointer)text);
}

int set_settings(void)
{
	FILE *settingsFile;

	if(!fileOpen(&settingsFile, FILE_SETTINGS, TRUE)){
		fileWrite(&settingsFile, FILE_SETTINGS, TRUE);

		fprintf(settingsFile, "%s %s\n", sWidgets[LINE_NUMBER].name, P_TRUE);
		fprintf(settingsFile, "%s %s\n", sWidgets[RIGHT_MARGIN].name, P_TRUE);
		fprintf(settingsFile, "%s %s\n", sWidgets[RIGHT_MARGIN_WIDTH].name, "90");
		fprintf(settingsFile, "%s %s\n", sWidgets[INDENT_SIZE].name, "8");
		fprintf(settingsFile, "%s %s\n", sWidgets[TEXT_WRAP].name, P_FALSE);
		fprintf(settingsFile, "%s %s\n", sWidgets[HIGHLIGHT_LINE].name, P_TRUE);
		fprintf(settingsFile, "%s %s\n", sWidgets[HIGHLIGHT_BRACKET].name, P_TRUE);
		fprintf(settingsFile, "%s %s\n", sWidgets[SPACE_TABS].name, P_TRUE);
		fprintf(settingsFile, "%s %s\n", sWidgets[AUTO_INDENT].name, P_TRUE);
		fprintf(settingsFile, "%s %s\n", sWidgets[SPLIT_WORD].name, P_TRUE);

		fileClose(&settingsFile, FILE_SETTINGS, TRUE);
		if(!fileOpen(&settingsFile, FILE_SETTINGS, TRUE))
			return -1;
	}

	guchar *buffer;
	guchar *name;
	guchar *property;
	int bufferPosition = 0;
	int bufferPositionHistory = 0;
	int j;

	off_t byteCount = fileSize(FILE_SETTINGS);

	buffer = calloc(1, byteCount + 1);
	
	rewind(settingsFile);

	for (j = 0; j < byteCount; ++j)
		buffer[j] = fgetc(settingsFile);

	while((bufferPosition = getWordString(&buffer[bufferPositionHistory], &name)) > 0){
		bufferPositionHistory += bufferPosition;
		
		for (j = 0; j < WIDGET_NUMBER; ++j)
		{
			if(strcmp((const char *)name, sWidgets[j].name) == 0){
				if((bufferPosition = getWordString(&buffer[bufferPositionHistory], &property)) > 0){
					bufferPositionHistory += bufferPosition;
					sWidgets[j].property_change(sWidgets[j].widget, (const gchar *)property);
					g_print("%s\n", property);
					g_free(property);
					break;
				}
			}
		}
		g_print("%s\n", name);
		g_free(name);
	}
	g_free(buffer);

	fileClose(&settingsFile, FILE_SETTINGS, TRUE);
	return 0;
}

int store_settings(GtkWidget *widget, const gchar *property)
{
	FILE *settingsFile;

	if(!fileOpen(&settingsFile, FILE_SETTINGS, TRUE))
		return -1;

	return 0;
}
