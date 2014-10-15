#include "settings.h"
#include "datastructure.h"
#include "commoncallbacks.h"
#include "filemanager.h"
#include "customstring.h"
#include "outputbuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

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
	const gchar *(*get_property)(GtkWidget *widget);
}settingsWidgets;

typedef enum {
	LINE_NUMBER, 
	RIGHT_MARGIN, 
	RIGHT_MARGIN_WIDTH, 
	SPLIT_WORD, //This one always before
	TEXT_WRAP, //This one
	HIGHLIGHT_LINE, 
	HIGHLIGHT_BRACKET,  
	INDENT_SIZE, 
	SPACE_TABS, 
	AUTO_INDENT,
	WIDGET_NUMBER
}setEnum;

static settingsWidgets sWidgets[WIDGET_NUMBER];
static GtkWrapMode textWrapMode;

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
}

static const gchar *toggle_button_get_property(GtkWidget *widget)
{
	static const gchar trueString[] = P_TRUE;
	static const gchar falseString[] = P_FALSE;

	return (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) ? trueString : falseString;
}

static void spin_button_init(GtkWidget *widget, const gchar *property)
{
	int value = atoi(property);

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), (gdouble)value);
}

static const gchar * spin_button_get_property_value(GtkWidget *widget)
{
	static gchar value[10] = {0};

	sprintf(value, "%d", (guint)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)));

	return (const gchar *)value;
}

//In the future, use a function or macros to make this simpler 
//Also, add object_ref?
void build_settings(GtkBuilder *builder, textStruct *text)
{
	sWidgets[LINE_NUMBER].name = LINE_NUMBER_NAME;
	sWidgets[LINE_NUMBER].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[LINE_NUMBER].name));
	sWidgets[LINE_NUMBER].property_change = toggle_button_init;
	sWidgets[LINE_NUMBER].get_property = toggle_button_get_property;

	g_signal_connect(sWidgets[LINE_NUMBER].widget, "toggled", G_CALLBACK(line_number_toggle_cb), (gpointer)text);


	sWidgets[RIGHT_MARGIN].name = RIGHT_MARGIN_NAME;
	sWidgets[RIGHT_MARGIN].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[RIGHT_MARGIN].name));
	sWidgets[RIGHT_MARGIN].property_change = toggle_button_init;
	sWidgets[RIGHT_MARGIN].get_property = toggle_button_get_property;

	sWidgets[RIGHT_MARGIN_WIDTH].name = RIGHT_MARGIN_WIDTH_NAME;
	sWidgets[RIGHT_MARGIN_WIDTH].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[RIGHT_MARGIN_WIDTH].name));
	sWidgets[RIGHT_MARGIN_WIDTH].property_change = spin_button_init;
	sWidgets[RIGHT_MARGIN_WIDTH].get_property = spin_button_get_property_value;

	g_signal_connect(sWidgets[RIGHT_MARGIN].widget, "toggled", G_CALLBACK(right_margin_toggle_cb), (gpointer)text);
	g_signal_connect(sWidgets[RIGHT_MARGIN].widget, "toggled", G_CALLBACK(toggle_sensitive_cb), 
			(gpointer)sWidgets[RIGHT_MARGIN_WIDTH].widget);

	g_signal_connect(sWidgets[RIGHT_MARGIN_WIDTH].widget, "value-changed", G_CALLBACK(right_margin_width_set_cb), 
			(gpointer)text);

	sWidgets[TEXT_WRAP].name = TEXT_WRAP_NAME;
	sWidgets[TEXT_WRAP].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[TEXT_WRAP].name));
	sWidgets[TEXT_WRAP].property_change = toggle_button_init;
	sWidgets[TEXT_WRAP].get_property = toggle_button_get_property;

	sWidgets[SPLIT_WORD].name = SPLIT_WORD_NAME;
	sWidgets[SPLIT_WORD].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[SPLIT_WORD].name));
	sWidgets[SPLIT_WORD].property_change = toggle_button_init;
	sWidgets[SPLIT_WORD].get_property = toggle_button_get_property;

	g_signal_connect(sWidgets[TEXT_WRAP].widget, "toggled", G_CALLBACK(wrap_text_cb), (gpointer)text);
	g_signal_connect(sWidgets[TEXT_WRAP].widget, "toggled", G_CALLBACK(toggle_sensitive_cb), 
			(gpointer)sWidgets[SPLIT_WORD].widget);

	g_signal_connect(sWidgets[SPLIT_WORD].widget, "toggled", G_CALLBACK(wrap_mode_mode_change_cb), (gpointer)text);

	sWidgets[HIGHLIGHT_LINE].name = HIGHLIGHT_LINE_NAME;
	sWidgets[HIGHLIGHT_LINE].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[HIGHLIGHT_LINE].name));
	sWidgets[HIGHLIGHT_LINE].property_change = toggle_button_init;
	sWidgets[HIGHLIGHT_LINE].get_property = toggle_button_get_property;

	g_signal_connect(sWidgets[HIGHLIGHT_LINE].widget, "toggled", G_CALLBACK(highlight_line_cb), (gpointer)text);

	sWidgets[HIGHLIGHT_BRACKET].name = HIGHLIGHT_BRACKET_NAME;
	sWidgets[HIGHLIGHT_BRACKET].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[HIGHLIGHT_BRACKET].name));
	sWidgets[HIGHLIGHT_BRACKET].property_change = toggle_button_init;
	sWidgets[HIGHLIGHT_BRACKET].get_property = toggle_button_get_property;

	g_signal_connect(sWidgets[HIGHLIGHT_BRACKET].widget, "toggled", G_CALLBACK(highlight_bracket_cb), (gpointer)text);

	sWidgets[INDENT_SIZE].name = INDENT_SIZE_NAME;
	sWidgets[INDENT_SIZE].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[INDENT_SIZE].name));
	sWidgets[INDENT_SIZE].property_change = spin_button_init;
	sWidgets[INDENT_SIZE].get_property = spin_button_get_property_value;

	g_signal_connect(sWidgets[INDENT_SIZE].widget, "value-changed", G_CALLBACK(tab_width_change_cb), (gpointer)text);

	sWidgets[SPACE_TABS].name = SPACE_TABS_NAME;
	sWidgets[SPACE_TABS].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[SPACE_TABS].name));
	sWidgets[SPACE_TABS].property_change = toggle_button_init;
	sWidgets[SPACE_TABS].get_property = toggle_button_get_property;

	g_signal_connect(sWidgets[SPACE_TABS].widget, "toggled", G_CALLBACK(space_tab_change_cb), (gpointer)text);

	sWidgets[AUTO_INDENT].name = AUTO_INDENT_NAME;
	sWidgets[AUTO_INDENT].widget = GTK_WIDGET(gtk_builder_get_object(builder, sWidgets[AUTO_INDENT].name));
	sWidgets[AUTO_INDENT].property_change = toggle_button_init;
	sWidgets[AUTO_INDENT].get_property = toggle_button_get_property;

	g_signal_connect(sWidgets[AUTO_INDENT].widget, "toggled", G_CALLBACK(auto_indent_change_cb), (gpointer)text);


	//Saving and default buttons
	GObject *settings_buttons;
	settings_buttons = gtk_builder_get_object(builder, "save_settings");
	g_signal_connect(settings_buttons, "clicked", G_CALLBACK(store_settings_cb), NULL);

	settings_buttons = gtk_builder_get_object(builder, "default_settings");
	g_signal_connect(settings_buttons, "clicked", G_CALLBACK(default_settings_cb), NULL);
}

int set_settings(void)
{
	FILE *settingsFile;

	if(!fileOpen(&settingsFile, FILE_SETTINGS, FALSE)){
		fileWrite(&settingsFile, FILE_SETTINGS, TRUE);

		fprintf(settingsFile, "%s %s\n", sWidgets[LINE_NUMBER].name, P_FALSE);
		fprintf(settingsFile, "%s %s\n", sWidgets[RIGHT_MARGIN].name, P_FALSE);
		fprintf(settingsFile, "%s %s\n", sWidgets[RIGHT_MARGIN_WIDTH].name, "90");
		fprintf(settingsFile, "%s %s\n", sWidgets[INDENT_SIZE].name, "8");
		fprintf(settingsFile, "%s %s\n", sWidgets[SPLIT_WORD].name, P_FALSE); //Always call this one before
		fprintf(settingsFile, "%s %s\n", sWidgets[TEXT_WRAP].name, P_FALSE); //this one
		fprintf(settingsFile, "%s %s\n", sWidgets[HIGHLIGHT_LINE].name, P_FALSE);
		fprintf(settingsFile, "%s %s\n", sWidgets[HIGHLIGHT_BRACKET].name, P_FALSE);
		fprintf(settingsFile, "%s %s\n", sWidgets[SPACE_TABS].name, P_FALSE);
		fprintf(settingsFile, "%s %s\n", sWidgets[AUTO_INDENT].name, P_FALSE);

		output_print("Creating new \"", FALSE);
		output_print(FILE_SETTINGS, FALSE);
		output_print("\" file.", TRUE);

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
					g_free(property);
					break;
				}
			}
		}
		g_free(name);
	}
	g_free(buffer);

	fileClose(&settingsFile, FILE_SETTINGS, TRUE);
	return 0;
}

//-------------------------------------------------------------------------------------------------------
//Toolbar CallBack Functions
//-------------------------------------------------------------------------------------------------------

gboolean store_settings_cb(GtkWidget *widget, gpointer data)
{
	FILE *settingsFile;
	int j;

	if(!fileWrite(&settingsFile, FILE_SETTINGS, TRUE))
		return FALSE;

	for (j = 0; j < WIDGET_NUMBER; ++j)
		fprintf(settingsFile, "%s %s\n", sWidgets[j].name, sWidgets[j].get_property(sWidgets[j].widget));

	fileClose(&settingsFile, FILE_SETTINGS, TRUE);

	return TRUE;
}

gboolean default_settings_cb(GtkWidget *widget, gpointer data)
{
	if(remove(FILE_SETTINGS) == -1)
		return FALSE;

	set_settings();

	return TRUE;
}

gboolean line_number_toggle_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;
	gboolean buttonState = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(text->programTextView), buttonState);

	return TRUE;
}

gboolean toggle_sensitive_cb(GtkWidget *widget, gpointer data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(data), gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));

	return TRUE;
}

gboolean right_margin_toggle_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;

	gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(text->programTextView), 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));

	return TRUE;
}

gboolean right_margin_width_set_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;

	gtk_source_view_set_right_margin_position(GTK_SOURCE_VIEW(text->programTextView), 
			(guint)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)));

	return TRUE;
}

gboolean wrap_text_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;

	assert(textWrapMode != 0);

	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text->programTextView),
			(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))? (textWrapMode):(GTK_WRAP_NONE));

	return TRUE;
}

gboolean wrap_mode_mode_change_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;

	textWrapMode = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))? (GTK_WRAP_CHAR):(GTK_WRAP_WORD);

	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text->programTextView), textWrapMode);

	return TRUE;
}

gboolean highlight_line_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;

	gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(text->programTextView), 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));

	return TRUE;
}

gboolean highlight_bracket_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;

	gtk_source_buffer_set_highlight_matching_brackets(GTK_SOURCE_BUFFER(text->programBuffer), 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));

	return TRUE;
}

gboolean tab_width_change_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;

	gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(text->programTextView), 
			(guint)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)));

	return TRUE;
}

gboolean space_tab_change_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;

	gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(text->programTextView), 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));

	return TRUE;
}

gboolean auto_indent_change_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;

	gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(text->programTextView), 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));

	return TRUE;
}
