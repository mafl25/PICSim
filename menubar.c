#include "menubar.h"
#include "datastructure.h"
#include "filedialog.h"
#include "filemanager.h"
#include "commoncallbacks.h"
#include "outputbuffer.h"
#include "customstring.h"
#include "variabletreeview.h"

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <assert.h>
#include <stdlib.h>

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

//-------------------------------------------------------------------------------------------------------
//File Menu CallBack Functions
//-------------------------------------------------------------------------------------------------------

gboolean open_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *openFile = (textStruct *)data;

	gboolean returnValue = open_file(&openFile->file, &openFile->filename, TRUE);
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

		fileClose(&fileLoad->file,fileLoad->filename, TRUE);
	}

	return returnValue;
}

gboolean save_text_view_to_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *fileSave = (textStruct *)data;
	gboolean returnValue = FALSE;
	
	returnValue	= save_text_view_to_file(&fileSave->file, &fileSave->filename, 
			GTK_TEXT_BUFFER(fileSave->programBuffer), TRUE);

	if(returnValue && (fileSave->label != NULL)){
		change_label(GTK_LABEL(fileSave->label), fileSave->filename);
		fileSave->isSaved = TRUE;
	}
	
	return returnValue;
}

gboolean save_as_text_view_to_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *fileSave = (textStruct *)data;
	gboolean returnValue = FALSE;
	
	returnValue	= save_as_text_view_to_file(&fileSave->file, &fileSave->filename, 
			GTK_TEXT_BUFFER(fileSave->programBuffer), TRUE);

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
			GTK_TEXT_BUFFER(newFile->programBuffer), TRUE);

	if(returnValue && (newFile->label != NULL))
		change_label(GTK_LABEL(newFile->label), newFile->filename);

	g_signal_handler_unblock((gpointer)newFile->programBuffer, newFile->programChangedHandlerId);

	return returnValue;
}

//-------------------------------------------------------------------------------------------------------
//Edit Menu CallBack Functions
//-------------------------------------------------------------------------------------------------------

gboolean copy_text_cb(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_c, GDK_CONTROL_MASK);

	return TRUE;
}

gboolean cut_text_cb(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_x, GDK_CONTROL_MASK);

	return TRUE;
}

gboolean paste_text_cb(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_v, GDK_CONTROL_MASK);

	return TRUE;
}

gboolean delete_text_cb(GtkWidget *widget, gpointer data)
{
	send_key(GTK_WIDGET(data), GDK_KEY_Delete, 0);

	return TRUE;
}

gboolean undo_text_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;
	
	if(gtk_source_buffer_can_undo(GTK_SOURCE_BUFFER(text->programBuffer))){
		gtk_source_buffer_undo(GTK_SOURCE_BUFFER(text->programBuffer));
		return TRUE;
	}else{
		return FALSE;
	}
}

gboolean redo_text_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;
	
	if(gtk_source_buffer_can_redo(GTK_SOURCE_BUFFER(text->programBuffer))){
		gtk_source_buffer_redo(GTK_SOURCE_BUFFER(text->programBuffer));
		return TRUE;
	}else{
		return FALSE;
	}
}

gboolean build_program_cb(GtkWidget *widget, gpointer data)
{
	textStruct *text = (textStruct *)data;
	variablesArray *variables = variables_array_new();
	labelsArray *labels = labels_array_new();
	gboolean output = TRUE;

	save_text_view_to_file_cb(widget, text);
	if(text->filename != NULL){
		if(!g_str_has_suffix(text->filename, ".asm")){
			output_print("ERROR: Wrong filetype.", TRUE);
			output = FALSE;
		}
	}else{
		output = FALSE;
	}

	if(output){
		variables->lastAddress = LAST_RAM_ADDRESS;
		output = variables_array_init(text, variables);
	}

	if(output)
		output = variables_array_set_addresses(variables);

	if(output)
		output = variables_array_replace_to_file(text, variables);

	if(output)
		output = labels_array_init(text, labels); 

	if(output)
		output = label_array_conflict_check(variables, labels);

	//luego calculoar las direcciones finales de los labels.

	if(output){
		variable_tree_view_clear();
		int j;
		for (j = 0; j < variables->variableCount; ++j){
			variable_tree_view_append_row(variables->variableList[j]->str, variables->variableAddress[j], 0xA);	
		}
	}

	labels_array_destroy(labels);
	variables_array_destroy(variables);

	if(!output)
		output_print("Build failed.", TRUE);
	else
		output_print("Build succesful.", TRUE);

	return output;
}

//-------------------------------------------------------------------------------------------------------
//Initialization
//-------------------------------------------------------------------------------------------------------

void build_menubar(GtkBuilder *builder, textStruct *text, GtkWindow *window)
{
	gtk_source_buffer_set_max_undo_levels(GTK_SOURCE_BUFFER(text->programBuffer), -1);

	GObject *menu_item;

	menu_item = gtk_builder_get_object(builder, "quit_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(gtk_main_quit), NULL);

	menu_item = gtk_builder_get_object(builder, "save_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(save_text_view_to_file_cb), (gpointer)(text));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(text));

	menu_item = gtk_builder_get_object(builder, "save_as_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(save_as_text_view_to_file_cb), (gpointer)(text));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(text));
	
	menu_item = gtk_builder_get_object(builder, "new_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(new_file_cb), (gpointer)(text));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(text));
	
	menu_item = gtk_builder_get_object(builder, "open_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(open_file_cb), (gpointer)(text));
	g_signal_connect(menu_item, "activate", G_CALLBACK(file_load_to_text_view_cb), (gpointer)(text));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_saved_cb), (gpointer)(text));

	menu_item = gtk_builder_get_object(builder, "undo_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(undo_text_cb), (gpointer)(text));

	menu_item = gtk_builder_get_object(builder, "redo_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(redo_text_cb), (gpointer)(text));

	menu_item = gtk_builder_get_object(builder, "cut_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(cut_text_cb), (gpointer)window);

	menu_item = gtk_builder_get_object(builder, "copy_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(copy_text_cb), (gpointer)window);

	menu_item = gtk_builder_get_object(builder, "paste_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(paste_text_cb), (gpointer)window);

	menu_item = gtk_builder_get_object(builder, "delete_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(delete_text_cb), (gpointer)window);

	menu_item = gtk_builder_get_object(builder, "build_menu");
	g_signal_connect(menu_item, "activate", G_CALLBACK(build_program_cb), (gpointer)(text));
}

