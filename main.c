#include "filemanager.h"
#include "filedialog.h"
#include "outputbuffer.h"
#include "callbacks.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
//#include <gtksourceview-3.0/gtksourceview/gtksource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


static gboolean copy_text(GtkWidget *widget, gpointer data);
static gboolean cut_text(GtkWidget *widget, gpointer data);
static gboolean paste_text(GtkWidget *widget, gpointer data);
static gboolean delete_text(GtkWidget *widget, gpointer data);

void textStructInit(textStruct *text, GtkBuilder *builder, gchar *text_program, gchar *text_output,
		gchar *program_buffer, gchar *output_buffer, gchar *label_text);
void textStructDestroy(textStruct *text);

static gboolean program_changed_cd(GtkWidget *widget, gpointer data);

static void send_key(GtkWidget *window, guint keyval, guint state);

int main(int argc, char *argv[])
{
	GtkBuilder *builder;
	GObject *window;
	GObject *menu_item;
	GObject *toolbar_button;


	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "project_gui.ui", NULL);

	window = gtk_builder_get_object(builder, "window1");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);


	textStruct openText;
	textStructInit(&openText, builder, "text_program", "text_output", "program_buffer", "output_buffer",
			"text_name");

	menu_item = gtk_builder_get_object(builder, "quit_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(gtk_main_quit), NULL);

	menu_item = gtk_builder_get_object(builder, "save_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(save_text_view_to_file_cb), (gpointer)(&openText));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(&openText));

	menu_item = gtk_builder_get_object(builder, "save_as_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(save_as_text_view_to_file_cb), (gpointer)(&openText));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(&openText));
	
	menu_item = gtk_builder_get_object(builder, "new_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(new_file_cb), (gpointer)(&openText));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(&openText));
	
	menu_item = gtk_builder_get_object(builder, "open_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(open_file_cb), (gpointer)(&openText));
	g_signal_connect(menu_item, "activate", G_CALLBACK(file_load_to_text_view_cb), (gpointer)(&openText));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(&openText));

	menu_item = gtk_builder_get_object(builder, "cut_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(cut_text), (gpointer)window);

	menu_item = gtk_builder_get_object(builder, "copy_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(copy_text), (gpointer)window);

	menu_item = gtk_builder_get_object(builder, "paste_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(paste_text), (gpointer)window);

	menu_item = gtk_builder_get_object(builder, "delete_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(delete_text), (gpointer)window);

	toolbar_button = gtk_builder_get_object(builder, "open_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(open_file_cb), (gpointer)(&openText));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(file_load_to_text_view_cb), (gpointer)(&openText));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(set_saved_cb), (gpointer)(&openText));

	toolbar_button = gtk_builder_get_object(builder, "save_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(save_text_view_to_file_cb), (gpointer)(&openText));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(set_saved_cb), (gpointer)(&openText));

	toolbar_button = gtk_builder_get_object(builder, "new_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(new_file_cb), (gpointer)(&openText));
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(set_saved_cb), (gpointer)(&openText));

	toolbar_button = gtk_builder_get_object(builder, "copy_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(copy_text), (gpointer)window);

	toolbar_button = gtk_builder_get_object(builder, "cut_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(cut_text), (gpointer)window);

	toolbar_button = gtk_builder_get_object(builder, "paste_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(paste_text), (gpointer)window);

	openText.programChangedHandlerId = g_signal_connect(openText.programBuffer, "changed", 
			G_CALLBACK(program_changed_cd), (gpointer)&openText);

	g_object_unref(G_OBJECT(builder));
	gtk_main();

	fileClose(&openText.file, openText.filename, GTK_TEXT_BUFFER(openText.outputBuffer));
	textStructDestroy(&openText);

	return 0;
}


void textStructInit(textStruct *text, GtkBuilder *builder, gchar *text_program, gchar *text_output,
		gchar *program_buffer, gchar *output_buffer, gchar *label_text)
{
	text->programTextView = gtk_builder_get_object(builder, text_program);
	text->programBuffer = gtk_builder_get_object(builder, program_buffer);
	text->outputTextView = gtk_builder_get_object(builder, text_output);
	text->outputBuffer = gtk_builder_get_object(builder, output_buffer);
	text->label = gtk_builder_get_object(builder, label_text);
	text->file = NULL;
	text->filename = NULL;

	g_object_ref(text->programTextView);
	g_object_ref(text->outputTextView);
	g_object_ref(text->programBuffer);
	g_object_ref(text->outputBuffer);
	g_object_ref(text->label);

	gtk_label_set_text(GTK_LABEL(text->label), UNSAVED_FILE);

	text->isSaved = TRUE;
}

void textStructDestroy(textStruct *text)
{
	g_free(text->filename);
	g_object_unref(text->programTextView);
	g_object_unref(text->outputTextView);
	g_object_unref(text->programBuffer);
	g_object_unref(text->outputBuffer);
	g_object_unref(text->label);
}

//need to rethink this
static gboolean program_changed_cd(GtkWidget *widget, gpointer data)
{
	textStruct *label = (textStruct *)data;
	gchar *name ;

	if(label->isSaved){
		label->isSaved = FALSE;
		if(label->filename == NULL)
			gtk_label_set_text(GTK_LABEL(label->label), UNSAVED_FILE_MOD);
		else{
			name = g_filename_display_basename(label->filename);
			int length = strlen(name);
			length = length + 2;
			gchar *newName = calloc(1, length);
			newName[0] = '*';
			strcat(&newName[1], name);
			gtk_label_set_text(GTK_LABEL(label->label), newName);
			g_free(newName);
			g_free(name);
		}
		outputPrint(GTK_TEXT_BUFFER(label->outputBuffer), "Modificado", TRUE);
	}
	

	return TRUE;
}

static gboolean delete_text(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_Delete, 0);

	return TRUE;
}

static gboolean cut_text(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_x, GDK_CONTROL_MASK);

	return TRUE;
}

static gboolean paste_text(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_v, GDK_CONTROL_MASK);

	return TRUE;
}

static gboolean copy_text(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_c, GDK_CONTROL_MASK);

	return TRUE;
}

static void send_key(GtkWidget *window, guint keyval, guint state)
{
	GdkWindow *gdk_window = g_object_ref(gtk_widget_get_window(window));
	GdkDisplay *display = gdk_display_get_default();
	GdkDeviceManager *device_manager = gdk_display_get_device_manager(display);
	GdkDevice *device = gdk_device_manager_get_client_pointer(device_manager);

	GdkKeymapKey *pKeys;
	gint numKeys = 0;

	gdk_keymap_get_entries_for_keyval(gdk_keymap_get_default(), keyval, &pKeys, &numKeys);

	GdkEvent *pressA = gdk_event_new(GDK_KEY_PRESS) ;
	gdk_event_set_device(pressA, device);

	pressA->key.type = GDK_KEY_PRESS;
	pressA->key.window = gdk_window; 
	pressA->key.send_event = TRUE;
	pressA->key.time = GDK_CURRENT_TIME;
	pressA->key.state = state;
	pressA->key.keyval = keyval;
	pressA->key.hardware_keycode = pKeys[0].keycode;
	pressA->key.group = pKeys[0].group;
	pressA->key.is_modifier = (state) ? 1 : 0;
	
	gdk_display_put_event(display, pressA);

	gdk_event_free(pressA);
	g_free(pKeys);
}
