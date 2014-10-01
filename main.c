#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct
{
	GObject *textProgram;
	GObject *programBuffer;
	FILE *file;
	gboolean named;
	char *filename;
}fileStruct;

static gboolean new_file_cb(GtkWidget *widget, gpointer data);
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

	openFile.textProgram = gtk_builder_get_object(builder, "text_program");
	openFile.programBuffer = gtk_builder_get_object(builder, "program_buffer");

	menu_item = gtk_builder_get_object(builder, "quit_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(gtk_main_quit), NULL);

//	menu_item = gtk_builder_get_object(builder, "new_file");
//	g_signal_connect(menu_item, "activate", G_CALLBACK(new_file_cb), NULL);
	
	menu_item = gtk_builder_get_object(builder, "open_file");
	g_signal_connect(menu_item, "activate", G_CALLBACK(new_file_cb), (gpointer)(&openFile));
	g_signal_connect(menu_item, "activate", G_CALLBACK(file_load_to_text_view_cb), (gpointer)(&openFile));

	g_object_unref(G_OBJECT(builder));
	gtk_main();

	free(openFile.filename);
	return 0;
}

static gboolean save_text_view_to_file(GtkWidget *widget, gpointer data)
{
	fileStruct *fileSave = (fileStruct *)data;
	GtkTextIter *start;
	GtkTextIter *end;
	char *text;

	if(fileSave->named){
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(fileSave->programBuffer), start);
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(fileSave->programBuffer), end);
		fileSave->file = fopen(fileSave->filename, "w");	
		gtk_text_buffer_get_text(GTK_TEXT_BUFFER(fileSave->textProgram), start, end, TRUE);

	}

	return FALSE;
}

static gboolean file_load_to_text_view_cb(GtkWidget *widget, gpointer data)
{
	fileStruct *fileLoad = (fileStruct *)data;
	int fileSize;
	int character;
	int j;

	for (fileSize = 0; (character = getc(fileLoad->file)) != EOF; ++fileSize);

 	gchar *text = (gchar *)calloc(sizeof(gchar), fileSize + 1);

	rewind(fileLoad->file);
	for (j = 0; j < fileSize; ++j)
		text[j] = (gchar)getc(fileLoad->file);	

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(fileLoad->programBuffer), text, -1);
	rewind(fileLoad->file);
	free(text);

	fileLoad->named =TRUE;
	gtk_widget_show(GTK_WIDGET(fileLoad->textProgram));

	return FALSE;
}

static gboolean new_file_cb(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	fileStruct *openFile = (fileStruct *)data;

	dialog = gtk_file_chooser_dialog_new("Open File",
			NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			"_Cancel",
			GTK_RESPONSE_CANCEL,
			"_Open",
			GTK_RESPONSE_ACCEPT,
			NULL);

	gint res = gtk_dialog_run(GTK_DIALOG(dialog));

	if(res == GTK_RESPONSE_ACCEPT){
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		openFile->filename = gtk_file_chooser_get_filename(chooser);
		openFile->file = fopen(openFile->filename, "r");
	}

	gtk_widget_destroy(dialog);

	return FALSE;
}
