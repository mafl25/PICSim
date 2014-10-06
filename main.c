#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


typedef struct
{
	GObject *textProgram;
	GObject *programBuffer;
	GObject *textOutput;
	GObject *outputBuffer;
	GObject *label;
	FILE *file;
	char *filename;
	GtkClipboard *clipboard;
}textStruct;

static gboolean fileClose(textStruct *file);
static gboolean fileOpen(textStruct *file);
static gboolean fileWrite(textStruct *file);

static void outputPrint(GtkTextBuffer *outputBuffer, char * message, gboolean addNewline);

static gboolean open_file_cb(GtkWidget *widget, gpointer data);
static gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data);
static gboolean save_text_view_to_file(GtkWidget *widget, gpointer data);
static gboolean save_as_text_view_to_file(GtkWidget *widget, gpointer data);
static gboolean new_file_cb(GtkWidget *widget, gpointer data);

static gboolean copy_text(GtkWidget *widget, gpointer data);
static gboolean cut_text(GtkWidget *widget, gpointer data);
static gboolean paste_text(GtkWidget *widget, gpointer data);
static gboolean delete_text(GtkWidget *widget, gpointer data);

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
	openText.file = NULL;
	openText.filename = NULL;
	openText.textProgram = gtk_builder_get_object(builder, "text_program");
	openText.programBuffer = gtk_builder_get_object(builder, "program_buffer");
	openText.textOutput = gtk_builder_get_object(builder, "text_output");
	openText.outputBuffer = gtk_builder_get_object(builder, "output_buffer");
	openText.label = gtk_builder_get_object(builder, "text_name");
	openText.clipboard = gtk_clipboard_get(GDK_NONE);


	menu_item = gtk_builder_get_object(builder, "quit_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(gtk_main_quit), NULL);

	menu_item = gtk_builder_get_object(builder, "save_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(save_text_view_to_file), (gpointer)(&openText));

	menu_item = gtk_builder_get_object(builder, "save_as_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(save_as_text_view_to_file), (gpointer)(&openText));
	
	menu_item = gtk_builder_get_object(builder, "new_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(new_file_cb), (gpointer)(&openText));
	
	menu_item = gtk_builder_get_object(builder, "open_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(open_file_cb), (gpointer)(&openText));
	g_signal_connect(menu_item, "activate", G_CALLBACK(file_load_to_text_view_cb), (gpointer)(&openText));

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

	toolbar_button = gtk_builder_get_object(builder, "save_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(save_text_view_to_file), (gpointer)(&openText));

	toolbar_button = gtk_builder_get_object(builder, "new_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(new_file_cb), (gpointer)(&openText));

	toolbar_button = gtk_builder_get_object(builder, "copy_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(copy_text), (gpointer)window);

	toolbar_button = gtk_builder_get_object(builder, "cut_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(cut_text), (gpointer)window);

	toolbar_button = gtk_builder_get_object(builder, "paste_button");
	g_signal_connect(toolbar_button, "clicked", G_CALLBACK(paste_text), (gpointer)window);

	g_object_unref(G_OBJECT(builder));
	gtk_main();

	g_free(openText.filename);
	fileClose(&openText);

	return 0;
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

static gboolean new_file_cb(GtkWidget *widget, gpointer data)
{
	textStruct *newFile= (textStruct *)data;
	gboolean returnValue = FALSE;

	returnValue = save_text_view_to_file(NULL, (gpointer)newFile);

	if(returnValue){
		if(newFile->filename != NULL){
			g_free(newFile->filename);
			newFile->filename = NULL;
		}
		fileClose(newFile);
		
		GtkTextIter start;
		GtkTextIter end;
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(newFile->programBuffer), &start);
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(newFile->programBuffer), &end);
		gtk_text_buffer_delete(GTK_TEXT_BUFFER(newFile->programBuffer), &start, &end);
		gtk_label_set_text(GTK_LABEL(newFile->label), "Unsaved file");
	}

	return returnValue;

}
static gboolean save_as_text_view_to_file(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	textStruct *fileSave = (textStruct *)data;
	gboolean returnValue = FALSE;

	dialog = gtk_file_chooser_dialog_new("Save File",
			NULL,
			GTK_FILE_CHOOSER_ACTION_SAVE,
			"_Cancel",
			GTK_RESPONSE_CANCEL,
			"_Save",
			GTK_RESPONSE_ACCEPT,
			NULL);

	GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
	gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.asm");
	gtk_file_chooser_set_filter(chooser, filter);

	if(fileSave->filename != NULL){
		gtk_file_chooser_set_filename(chooser, fileSave->filename);
	}else{
		gtk_file_chooser_set_current_name(chooser, "Untitled Document");
	}


	gint res = gtk_dialog_run(GTK_DIALOG(dialog));

	if(res == GTK_RESPONSE_ACCEPT){
		if(fileSave->filename != NULL)
			g_free(fileSave->filename);
		fileClose(fileSave);

		fileSave->filename = gtk_file_chooser_get_filename(chooser);
		returnValue = save_text_view_to_file(NULL, (gpointer)fileSave);
		if(returnValue){
			gchar *name = g_filename_display_basename(fileSave->filename);
			gtk_label_set_text(GTK_LABEL(fileSave->label), name);
			g_free(name);
		}
	}

	gtk_widget_destroy(dialog);
	
	return returnValue;
}
//Crear Back Up
static gboolean save_text_view_to_file(GtkWidget *widget, gpointer data)
{
	textStruct *fileSave = (textStruct *)data;
	GtkTextIter start;
	GtkTextIter end;
	gboolean returnValue = FALSE;
	char *text;

	if(fileSave->filename != NULL){
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(fileSave->programBuffer), &start);
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(fileSave->programBuffer), &end);

		fileClose(fileSave);
		if(fileWrite(fileSave)){
			text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(fileSave->programBuffer), &start, &end, TRUE);
			if(fputs(text, fileSave->file) == EOF){
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), "Error: File: \"", FALSE);
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), fileSave->filename, FALSE);
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), "\" could not be saved.", TRUE);
			}else{
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), "Document: \"", FALSE);
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), fileSave->filename, FALSE);
				outputPrint(GTK_TEXT_BUFFER(fileSave->outputBuffer), "\" saved.", TRUE);
				returnValue = TRUE;
			}
			fileClose(fileSave);
		}	
	}else{
		returnValue = save_as_text_view_to_file(NULL, fileSave);
	}

	return returnValue;
}

static gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data)
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

		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(fileLoad->programBuffer), text, -1);
		rewind(fileLoad->file);
		free(text);

		fileClose(fileLoad);
	}

	return returnValue;
}

static gboolean open_file_cb(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	textStruct *openFile = (textStruct *)data;
	gboolean returnValue = TRUE;

	dialog = gtk_file_chooser_dialog_new("Open File",
			NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			"_Cancel",
			GTK_RESPONSE_CANCEL,
			"_Open",
			GTK_RESPONSE_ACCEPT,
			NULL);

	GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.asm");
	gtk_file_chooser_set_filter(chooser, filter);
	gint res = gtk_dialog_run(GTK_DIALOG(dialog));

	if(res == GTK_RESPONSE_ACCEPT){

		g_free(openFile->filename);
		fileClose(openFile);

		openFile->filename = gtk_file_chooser_get_filename(chooser);
		if(!fileOpen(openFile)){
			returnValue = FALSE;
			g_free(openFile->filename);
		}else{
			gchar *name = g_filename_display_basename(openFile->filename);
			gtk_label_set_text(GTK_LABEL(openFile->label), name);
			g_free(name);
		}
	}

	gtk_widget_destroy(dialog);

	return returnValue;
}

//This function closes a file. It checks if the file pointer is a NULL or if it is closed already
//in which case it will do nothing and will return FALSE. If the file pointer is valid and the file
//is indeed open, it will close it. If the closing was successful, it will return TRUE and set isOpen to
//FALSE. If it fails, it will print a message to the output buffer and return FALSE, leaving isOpen as true.
//Eventually, add truth table
static gboolean fileClose(textStruct *file)
{
	gboolean returnValue = FALSE;

	if(file->file != NULL){
		int output = fclose(file->file);
		if(output == EOF){
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "Error: File \"", FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), file->filename, FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "\" could not be closed.", TRUE);
		}else{
			file->file = NULL;
			returnValue = TRUE;
		}
	}

	return returnValue;
}

static gboolean fileOpen(textStruct *file)
{
	gboolean returnValue = FALSE;

	if(file->filename != NULL){
		file->file = fopen(file->filename, "r");
		if(file->file == NULL){
			char *error = strerror(errno);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "Error opening file \"", FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), file->filename, FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "\": ", FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), error, TRUE);
		}else{
			returnValue = TRUE;
		}
	}else{
		file->file = NULL;
		outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "Error opening file: No filename provided.", TRUE);
	}

	return returnValue;
}

static gboolean fileWrite(textStruct *file)
{
	gboolean returnValue = FALSE;

	if(file->filename != NULL){
		file->file = fopen(file->filename, "w");
		if(file->file == NULL){
			char *error = strerror(errno);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "Error writting file \"", FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), file->filename, FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "\": ", FALSE);
			outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), error, TRUE);
		}else{
			returnValue = TRUE;
		}
	}else{
		file->file = NULL;
		outputPrint(GTK_TEXT_BUFFER(file->outputBuffer), "Error writting file: No filename provided.", TRUE);
	}

	return returnValue;
}

//Prints a message to the output buffer, adding an \n character at the end.
static void outputPrint(GtkTextBuffer *outputBuffer, char * message, gboolean addNewline)
{
	GtkTextIter end;
	gtk_text_buffer_get_end_iter(outputBuffer, &end);
	gtk_text_buffer_insert(outputBuffer, &end, message, -1);
	if(addNewline){
		gtk_text_buffer_get_end_iter(outputBuffer, &end);
		gtk_text_buffer_insert(outputBuffer, &end, "\n", -1);
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
