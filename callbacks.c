#include "callbacks.h"
#include "filedialog.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static void change_label(GtkLabel *label, gchar *filename)
{
	assert(label);
	if(filename){
		gchar *name = g_filename_display_basename(filename);
		gtk_label_set_text(label, name);
		g_free(name);
	}else{
		gtk_label_set_text(label, UNSAVED_FILE);
	}
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

	gchar *file_patterns[] = {"*.asm", "*.txt", "NULL"};
	set_file_filter(file_patterns);

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

	file_filter_unref();
}

gboolean set_saved_cb(GtkWidget *widget, gpointer data)
{
	textStruct *label = (textStruct *)data;
	label->isSaved = TRUE;

	return TRUE;
}

//need to rethink this
gboolean program_changed_cd(GtkWidget *widget, gpointer data)
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
	}
	
	return TRUE;
}

gboolean open_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *openFile = (textStruct *)data;

	gboolean returnValue = open_file(&openFile->file, &openFile->filename, GTK_TEXT_BUFFER(openFile->outputBuffer));
	if(returnValue && (openFile->label != NULL))
		change_label(GTK_LABEL(openFile->label), openFile->filename);

	return returnValue;
}

gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data)
{
	textStruct *fileLoad = (textStruct *)data;
	int fileSize;
	int character;
	gboolean returnValue = TRUE;
	int j;

	if(fileLoad->file == NULL)
		returnValue = FALSE;
		
	if(returnValue){
		rewind(fileLoad->file);
		for (fileSize = 0; (character = getc(fileLoad->file)) != EOF; ++fileSize)
			j = 0;

		gchar *text = (gchar *)calloc(sizeof(gchar), fileSize + 1);

		rewind(fileLoad->file);
		for (j = 0; j < fileSize; ++j)
			text[j] = (gchar)getc(fileLoad->file);	

		g_signal_handler_block((gpointer)fileLoad->programBuffer, fileLoad->programChangedHandlerId);
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(fileLoad->programBuffer), text, -1);
		g_signal_handler_unblock((gpointer)fileLoad->programBuffer, fileLoad->programChangedHandlerId);

		rewind(fileLoad->file);
		free(text);

		fileClose(&fileLoad->file,fileLoad->filename, GTK_TEXT_BUFFER(fileLoad->outputBuffer));
	}

	return returnValue;
}

gboolean save_text_view_to_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *fileSave = (textStruct *)data;
	gboolean returnValue = FALSE;
	
	returnValue	= save_text_view_to_file(&fileSave->file, &fileSave->filename, 
			GTK_TEXT_BUFFER(fileSave->programBuffer), GTK_TEXT_BUFFER(fileSave->outputBuffer));

	if(returnValue && (fileSave->label != NULL))
		change_label(GTK_LABEL(fileSave->label), fileSave->filename);
	
	return returnValue;
}

gboolean save_as_text_view_to_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *fileSave = (textStruct *)data;
	gboolean returnValue = FALSE;
	
	returnValue	= save_as_text_view_to_file(&fileSave->file, &fileSave->filename, 
			GTK_TEXT_BUFFER(fileSave->programBuffer), GTK_TEXT_BUFFER(fileSave->outputBuffer));

	if(returnValue && (fileSave->label != NULL))
		change_label(GTK_LABEL(fileSave->label), fileSave->filename);
	
	return returnValue;
}

gboolean new_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *newFile= (textStruct *)data;
	gboolean returnValue = FALSE;
	g_signal_handler_block((gpointer)newFile->programBuffer, newFile->programChangedHandlerId);

	returnValue = new_file(&newFile->file, &newFile->filename, 
			GTK_TEXT_BUFFER(newFile->programBuffer), GTK_TEXT_BUFFER(newFile->outputBuffer));

	if(returnValue && (newFile->label != NULL))
		change_label(GTK_LABEL(newFile->label), newFile->filename);

	g_signal_handler_unblock((gpointer)newFile->programBuffer, newFile->programChangedHandlerId);

	return returnValue;
}

gboolean delete_text(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_Delete, 0);

	return TRUE;
}

gboolean cut_text(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_x, GDK_CONTROL_MASK);

	return TRUE;
}

gboolean paste_text(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_v, GDK_CONTROL_MASK);

	return TRUE;
}

gboolean copy_text(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_c, GDK_CONTROL_MASK);

	return TRUE;
}