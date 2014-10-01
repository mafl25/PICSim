#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct
{
	GObject *textProgram;
	GObject *programBuffer;
	GObject *textOutput;
	GObject *outputBuffer;
	FILE *file;
	gboolean named;
	char *filename;
}fileStruct;

static gboolean open_file_cb(GtkWidget *widget, gpointer data);
static gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data);
static gboolean save_text_view_to_file(GtkWidget *widget, gpointer data);

int main(int argc, char *argv[])
{
	GtkBuilder *builder;
	GObject *window;
	GObject *menu_item;
	fileStruct openFile;
	openFile.named = FALSE;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "project_gui.ui", NULL);

	window = gtk_builder_get_object(builder, "window1");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	openFile.file = NULL;
	openFile.filename = NULL;
	openFile.textProgram = gtk_builder_get_object(builder, "text_program");
	openFile.programBuffer = gtk_builder_get_object(builder, "program_buffer");
	openFile.textOutput = gtk_builder_get_object(builder, "text_output");
	openFile.outputBuffer = gtk_builder_get_object(builder, "output_buffer");

	menu_item = gtk_builder_get_object(builder, "quit_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(gtk_main_quit), NULL);

	menu_item = gtk_builder_get_object(builder, "save_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(save_text_view_to_file), (gpointer)(&openFile));

//	menu_item = gtk_builder_get_object(builder, "new_file");
//	g_signal_connect(menu_item, "activate", G_CALLBACK(new_file_cb), NULL);
	
	menu_item = gtk_builder_get_object(builder, "open_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(open_file_cb), (gpointer)(&openFile));
	g_signal_connect(menu_item, "activate", G_CALLBACK(file_load_to_text_view_cb), (gpointer)(&openFile));

	g_object_unref(G_OBJECT(builder));
	gtk_main();

	if(openFile.file != NULL){
		fclose(openFile.file);
		free(openFile.filename);
	}
	return 0;
}

//Crear Back Up
static gboolean save_text_view_to_file(GtkWidget *widget, gpointer data)
{
	fileStruct *fileSave = (fileStruct *)data;
	GtkTextIter start;
	GtkTextIter end;
	gboolean returnValue = FALSE;
	char *text;

	if(fileSave->named){
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(fileSave->programBuffer), &start);
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(fileSave->programBuffer), &end);

		fclose(fileSave->file);
		fileSave->file = fopen(fileSave->filename, "w");	

		if(fileSave->file == NULL){
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(fileSave->programBuffer), &end, "File could not be written.\n", -1);
			returnValue = TRUE;
		}

		text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(fileSave->programBuffer), &start, &end, TRUE);

		if(fputs(text, fileSave->file) == EOF){
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(fileSave->programBuffer), &end, "File could not be written.\n", -1);
			returnValue = TRUE;
		}

		fclose(fileSave->file);
		fileSave->file = fopen(fileSave->filename, "r");
		if(fileSave->file == NULL){
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(fileSave->programBuffer), &end, "File could not be reopened.\n", -1);
			returnValue = TRUE;
		}
	}

	return returnValue;
}

static gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data)
{
	fileStruct *fileLoad = (fileStruct *)data;
	int fileSize;
	int character;
	gboolean returnValue = FALSE;
	int j;

	if(fileLoad->file == NULL)
		returnValue = TRUE;
		
	if(!returnValue){
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

		fileLoad->named =TRUE;
		gtk_widget_show(GTK_WIDGET(fileLoad->textProgram));
	}

	return returnValue;
}

static gboolean open_file_cb(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	fileStruct *openFile = (fileStruct *)data;
	gboolean returnValue = FALSE;

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
		if(openFile->file != NULL)
			fclose(openFile->file);

		openFile->filename = gtk_file_chooser_get_filename(chooser);
		openFile->file = fopen(openFile->filename, "r");

		if(openFile->file == NULL){
			GtkTextIter end;
			gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(openFile->programBuffer), &end);
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(openFile->programBuffer), &end, "File could not be opened.\n", -1);
			g_free(openFile->filename);
			returnValue = TRUE;
		}
	}

	gtk_widget_destroy(dialog);
	//aparentemente tengo que darle unref a esto or not, check note 2

	return returnValue;
}
