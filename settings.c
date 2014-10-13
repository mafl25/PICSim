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
#define P_TRUE "TRUE"
#define P_FALSE "FALSE"

typedef struct
{
	GtkWidget *widget;
	gchar *name;
	void (*property_change)(GtkWidget *widget, const gchar *property);
}settingsWidgets;

typedef enum {LINE_NUMBER, RIGHT_MARGIN, RIGHT_MARGIN_WIDTH, WIDGET_NUMBER} setEnum;

static settingsWidgets sWidgets[WIDGET_NUMBER];

static void toggle_button_init(GtkWidget *widget, const gchar *property)
{
	int state = -1;

	if(!(strcmp(P_TRUE, property)))
		state = TRUE;
	else if(!(strcmp(P_FALSE, property)))
		state = FALSE;

	if(state != -1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), state);

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
}

void set_settings(void)
{
	FILE *settingsFile;

	if(!fileOpen(&settingsFile, "settings.pic", NULL)){
		fileWrite(&settingsFile, "settings.pic", NULL);

		fprintf(settingsFile, "%s %s\n", LINE_NUMBER_NAME, P_TRUE);
		fprintf(settingsFile, "%s %s\n", RIGHT_MARGIN_NAME, P_TRUE);
		fprintf(settingsFile, "%s %s\n", RIGHT_MARGIN_WIDTH_NAME, "90");

		fileClose(&settingsFile, "settings.pic", NULL);
		fileOpen(&settingsFile, "settings.pic", NULL);
	}

	guchar *buffer;
	guchar *word;
	int bufferPosition = 0;
	int bufferPositionHistory = 0;
	int byteCount;
	int letter;

	for (byteCount = 0; ((letter = fgetc(settingsFile) != EOF) || (byteCount == (INT_MAX - 1))); ++byteCount);

	buffer = calloc(1, byteCount + 1);
	
	rewind(settingsFile);

	int j;
	for (j = 0; j < byteCount; ++j)
		buffer[j] = fgetc(settingsFile);

	while((bufferPosition = getWordString(&buffer[bufferPositionHistory], &word)) > 0){
		bufferPositionHistory += bufferPosition;
		
		for (j = 0; j < WIDGET_NUMBER; ++j)
		{
			if(strcmp((const char *)word, sWidgets[j].name) == 0){
				if((bufferPosition = getWordString(&buffer[bufferPositionHistory], &word)) > 0){
					bufferPositionHistory += bufferPosition;
					g_print("%s\n", word);
					sWidgets[j].property_change(sWidgets[j].widget, (const gchar *)word);
					break;
				}
			}
		}
		g_print("%s\n", word);
		g_free(word);
	}
	g_free(buffer);

	fileClose(&settingsFile, "settings.pic", NULL);
/*	
	const gchar *name = gtk_widget_get_name(widget);
	g_print("%s\n", name);*/
}

